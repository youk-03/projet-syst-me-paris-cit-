# Projet SY5

Projet de SY5 2023-2024

Structures utilisées: 

Dans mystring.h est définie une structure argument qui est utilisée pour découper la ligne de commande lue par rapport à un délimiteur choisi grâce à la fonction split.

Dans job.h sont définies une structure job et une structure  job_table. Comme son nom l’indique, la structure job nous sert à stocker les jobs et les informations qui nous sont utiles afin de travailler avec la structure job_table qui est une table de job.


Organisation des fichiers:

mystring:

Fonction split qui retourne un argument et toutes les fonctions qui allouent ou libèrent la structure argument.

prompt:
Fonctions qui génèrent le prompt, et fonctions qui testent sur la ligne de commande quelle commande doit être lancée.

cd:
Fonction pour la commande cd.

forkexec:
Fonctions forkexec et forkexecBackground qui lancent de nouveaux jobs simples (non pipeline). C’est ici aussi qu’est la fonction reset_signal qui met à jour le handler de signaux dans le processus qui l’appelle, ainsi que les fonctions qui traitent la commande bg et fg. La fonction wait_for_process est un wait non bloquant qui attend après un job donné en paramètre et le gère en cas d’arrêt ou de mort.

interrogation_exit:
Fonction pour la commande ?, et le exit du jsh.

job:
Fonctions qui gèrent la structure job et la structure job_table, comprend aussi la fonction maj_job_table qui s’occupe de mettre à jour le statut des jobs en fonction de leurs états actuels avec un wait.

jobs_command:
Fonctions pour la commande jobs, ainsi que pour l’affichage suite à la mise à jour de maj_job_table.
Pour l’option -t de jobs, les fonctions auxiliaires se chargent de chercher dans le pseudo-SGF /proc de linux pour trouver récursivement les arborescences des jobs.
Contient la méthode pour détecter les jobs détachés, également grâce à un parcours du pseudo-SGF /proc de linux.


kill:
La fonction killcmd se charge d’envoyer les signaux aux jobs correspondants, selon les options passées en paramètre.

pwd:
Affiche la variable d’environnement correspondant au répertoire courant.

redirect:
Contient les fonctions qui gèrent les redirections, ainsi que la fonction qui gère les pipelines, puis la fonction qui gère les substitutions de processus. On y trouve aussi de nombreuses fonctions auxiliaires nécessaires au bon fonctionnement de ces programmes.

main:
Boucle qui fait tourner le programme

Makefile: 
Permet de compiler le programme grâce à la commande 
```
make
```
Pour retirer les fichiers créés  à la fin, il suffit de saisir la commande
```
make clean
```


Implémentation du sujet:

Pour la boucle principale, tous les arguments nécessaires sont d’abord déclarés (job*, job_table*...), et on effectue des copies des sorties et entrées du terminal. On commence par mettre à jour la job_table puis on affiche le prompt, on récupère la ligne lue et elle est  ensuite découpée en un argument passé à un switch afin de lancer la commande décrite par la ligne de commande. À la fin de la boucle while, la mémoire est libérée afin de pouvoir réallouer de nouveaux pointeurs.

Dans le cas où le job lancé possède des redirections (mais pas de pipeline), cela est capté avant le switch du main afin d’échanger les descripteurs correspondants. Cela crée un nouvel argument ne possédant cette fois que la ligne de commande à exécuter. 

Dans le cas où le job lancé n’est pas une commande interne, on rentre dans le cas forkexec du switch et ici, un job est créé. Afin de pouvoir le stocker en cas d’arrêt soudain, il est passé en argument à la fonction qui va utiliser exec(), fonction qui dans le cas d’un job pas en arrière plan va mettre le job en premier plan. Le jsh est remis en premier plan une fois sorti de la fonction forkexec. 

Pour la fonction cd, on va commencer par récupérer le chemin vers le répertoire courant à l’aide de la fonction getcwd. Ensuite, cd entre dans un switch pour différencier les différents cas. Le changement de répertoire se fait avec la fonction chdir. Pour finir, nous mettons à jour la variable d’environnement oldpath avec la valeur récupérée au début.

Pour fg et bg, fg va mettre en premier plan le job indiqué (put_foreground()), et si le job est à l'arrêt elle envoie le signal SIGCONT au pgid de celui-ci. bg agit de manière similaire.

Pour les pipelines, la base de la structure de la fonction a été inspirée d’un pseudocode trouvé sur internet, le crédit est cité en commentaire tout en haut de redirect.c. Grossièrement, la ligne est partagée une première fois par rapport au “|” puis une deuxième fois lorsque le fils est créé par rapport à “ “. Les différents cas sont gérés dans la boucle for en fonction de la valeur de i, on détecte si la redirection est correcte ou non, dans le cas contraire le fils dans lequel le problème de redirection s’est produit se tue par SIGUSR2, cela est capté par le père qui s’occupe de tuer les autres fils également avec SIGUSR2. Dans le cas d’une ligne de commande correcte, le père met à jour le job lors du premier tour de boucle avec le pgid correspondant, et à chaque tour de boucle il ajoute un nouveau “job” à sa process_table. Une fois tous les processus lancés, il attend ses fils et agit en conséquence.

Pour les substitutions de processus, la ligne est tout d’abord divisée par rapport aux “<”. Ensuite, toutes les commandes redirigées sont effectuées dans une boucle et l’adresse du pipe où est redirigée leur sortie est stockée dans un tableau. Pour finir, la fonction renvoie une ligne de commande avec la première commande suivie de toutes les adresses stockées en paramètre.

C’est la fonction exit_jsh qui va libérer la mémoire de la job_table.
