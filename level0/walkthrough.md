## Comportement
- Je dispose des droits `level0` que je peux vérifier en tapant `whoami`.
- Un binaire est présent avec des droits différents : `-rwsr-x---+ 1 level1 users 747441 Mar  6  2016 level0*`.
- En exécutant le binaire (`./level0`), on observe :
  - `Segmentation fault (core dumped)` si aucun argument n'est passé.
  - `No !` si un argument est passé.

→ On en déduit que le programme attend un argument. Reste à connaître lequel.

## Analyse
En analysant le binaire (`gdb ./level0`), on observe :
  - La fonction appelle `atoi()` puis compare son résultat avec `0x1a7` (`423` en hexadécimal) :
    ```shell
    (gdb) disas main
    ...
    0x08048ed4 <+20>:	call   0x8049710 <atoi>
    0x08048ed9 <+25>:	cmp    eax,0x1a7
    ...
    ```
  - Si le paramètre est égal à `423`, alors un appel à `execv()` sera fait. Cette fonction permet d'exécuter une commande.
    ```shell
    (gdb) disas main
    ...
    0x08048f4a <+138>:	movl   $0x80c5348,(%esp) # argument envoyé à la fonction execv() en dessous
    0x08048f51 <+145>:	call   0x8054640 <execv>
    ...
    ```
  - Il suffit d'examiner l'adresse du paramètre envoyé à la fonction `execv()` (située une ligne au-dessus) pour constater qu'on exécute `/bin/sh`. C'est une technique courante pour lancer un shell interactif. Bingo Mango !
    ```shell
    (gdb) x/s 0x80c5348
    0x80c5348:	 "/bin/sh"
    ```

## Résolution
- On exécute donc le binaire avec le bon argument `./level0 423`. 
- Un shell s'ouvre avec de nouveaux privilèges : en effet, si l'on tape `whoami`, on obtient `level1`.
- Il ne reste plus qu'à :
  - récupérer le mot de passe (`cat /home/user/level1/.pass`)
  - quitter le shell (`exit`)
  - et passer au niveau 1 (`su level1`) en renseignant ce mot de passe.
