# Prog-parallele-sous-tableau-maximal
Le but de ce projet est d'écrire un programme C/OpenMP qui trouve le sous-tableau maximal dans un tableau d'entiers. 
Plus formellement, étant donné un tableau Q de N entiers signés, le sous-tableau maximal est le plus grand sous-tableau de Q tel que la somme de ses éléments est maximale. Dans le cas ou plusieurs sous-tableaux existent, on prend par convention le premier.

## Exemple :

                                                    Étant donné le tableau suivant
                                         -1	2	-3	6	-1	8	-10	-5
                                                    Le sous tableau maximal est
                                                  6	-1	8
## Algorithmes :
Il existe de nombreux algorithmes pour résoudre ce problème. Le plus efficace est celui de Kadane qui a une complexité en O(N). Malheureusement il n'est pas parallélisable. 
Nous allons nous intéresser à une solution proposée par Perumalla et al. en 1995 (voir le pdf). 
Cette solution utilise 2 versions de SCAN/Prefix. La première est un prefix-somme qui utilise l'opération somme. La deuxième est un prefix-max qui applique la fonction max. C'est une opération binaire associative donc il suffit de prendre la version prefix-somme écrite dans le TP précédent et remplacer somme par max, et les valeurs initiales (0) par l'élément neutre de max. 
Cet algorithme exécute aussi un Suffix qui est un prefix de droite à gauche. 
Les étapes de l'algorithme sont les suivantes, en prenant en entrée un tableau Q de N entiers signés.
1. Compute sum-prefix of Q and store them in array PSUM
2. Compute sum-suffix of Q and store them in array SSUM
3. Compute max-suffix of PSUM and store them in array SMAX
4. Compute max-prefix of SSUM and store them in array PMAX
5. for 1 <= i <= n do in parallel
  Ms[i] = PMAX[i] - SSUM[i] + Q[i]
  Mp[i] = SMAX[i] - PSUM[i] + Q[i]
  M[i] = Ms[i] + Mp[i] - Q[i]
6.Find the subarray with the largest value in M. The indexes of this subarray are the indexes of the subarray of Q giving this sum.
Notez que dans l'étape 5, on peut simplifier entre +Q[i] et -Q[i].
