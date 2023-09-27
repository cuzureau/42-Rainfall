## Comportement
- J'ai des droits `level0` que je peux verifier en tapant `whoami`
- Un binaire est present avec des droits differents : `-rwsr-x---+ 1 level1 users 747441 Mar  6  2016 level0*`
- En exécutant le binaire (`./level0`), on observe :
  - `Segmentation fault (core dumped)` si aucun argument n'est passé.
  - `No !` si un argument est passé.

→ On en déduit que le programme attend un argument. Reste à connaître lequel.


## Analyse
En analysant le binaire (`gdb ./level0`), on observe :
  - La fonction appelle `atoi()` puis compare son résultat avec `0x1a7` (`423` en héxadécimal):
    ```shell
    (gdb) disas main
    ...
    0x08048ed4 <+20>:	call   0x8049710 <atoi>
    0x08048ed9 <+25>:	cmp    eax,0x1a7
    ...
    ```
  - Si le parametre est egal a `423`, alors un appel a `execv()` sera fait. Cette fonction permet d'executer une commande.
    ```shell
    (gdb) disas main
    ...
    0x08048f4a <+138>:	movl   $0x80c5348,(%esp) # argument envoye a la fonction execv() en dessous
    0x08048f51 <+145>:	call   0x8054640 <execv>
    ...
    ```
  - Il suffit d'examiner l'adresse du parametre envoye a la fonction `execv()` (situe une ligne au dessus) pour constater qu'on execute `/bin/sh`. Ce qui est une technique courante pour lancer un shell interactif. Bingo Mango !
    ```shell
    (gdb) x/s 0x80c5348
    0x80c5348:	 "/bin/sh"
    ```


## Résolution
On exécute donc le binaire avec le bon argument `./level0 423`. 
Un shell s'ouvre avec de nouveaux privileges : en effet, si l'on tape `whoami`, on obtient `level1`.

Il ne reste plus qu'à :
- récupérer le password (`cat /home/user/level1/.pass`)
- quitter le shell (`exit`)
- et passer au level1 (`su level1`) en renseignant ce password.
