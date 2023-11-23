#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <limits.h>
#include <string.h>
#include <unistd.h>

#include "cd.h"

/*Change de répertoire de travail courant en le répertoire ref (s'il
s'agit d'une référence valide), le précédent répertoire de travail si le
paramètre est -, ou $HOME en l'absence de paramètre.
La valeur de retour est 0 en cas de succès, 1 en cas d'échec.*/

int cd(int nb_arg, char * arg) {
    char old [PATH_MAX];
    getcwd(old,PATH_MAX);

    switch (nb_arg) {
        case 0 : 
            if (chdir(getenv("HOME"))<0){
                goto error;
            }
            break;
        case 1 : 
            if (strcmp(arg,"-")==0) {
                if (chdir(getenv("OLDPATH"))<0){
                    perror("cd : chdir");
                    goto error;
                }
            } else {
                if (chdir(arg)<0){
                    perror("cd : chdir"); 
                    goto error;
                }
            } break;
        default : goto error;
    }

    setenv("OLDPATH",old,1);

    return 0;

    error :
        return 1;
}