#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define TRACK fprintf(stderr, "*** %s:%d\n", __func__, __LINE__);

//Dans cette structure :

//value: représente la valeur du compteur dans la cellule.
//nmemb: indique le nombre d'éléments dans le compteur.
//file :est un indicateur de fichier (cela pourrait être un identifiant de
//fichier: ou une autre information pertinente, c'est facultatif selon les
// besoins).
//next: est un pointeur vers la cellule suivante dans la liste.
//prev :est un pointeur vers la cellule précédente dans la liste.

//Cette structure peut être utilisée pour représenter une liste chaînée de
//compteurs où les valeurs des compteurs sont strictement croissantes et chaque
//valeur n'apparaît qu'une seule fois dans la liste.

typedef struct cell cell;
struct cell {
  long int value;
  size_t nmemb;
  int file;
  cell *next;
  cell *prev;
};
typedef struct cptr cptr;

//* Fonction qui crée et initialise une structure cptr vide avec un certain
//nombre de fichiers.
//* @param n Le nombre de fichiers (compteurs).
//* @return Un pointeur vers la structure cptr créée, ou NULL en cas d'erreur
//d'allocation de mémoire ou si n est inférieur ou égal à zéro.
extern cptr *cptr_empty(int n);

//Cette fonction prend en paramètre un pointeur vers une structure cptr et
//le numéro de fichier associé à l'élément à ajouter. Elle vérifie si la
//structure cptr est valide et si le numéro de fichier est valide. Ensuite,
//elle ajoute un nouvel élément à la liste associée au fichier n si la tête
//de liste est nulle ou si sa valeur n'est pas égale à 1. Sinon, elle
//incrémente simplement le nombre d'éléments de la tête de liste et renvoie
//cette tête de liste. Si l'ajout échoue ou si le numéro de fichier est
//invalide, la fonction renvoie NULL.
extern void *ajout(cptr *c, int n);

//* Met à jour un compteur dans la liste des compteurs.
//* @param c Un pointeur vers la structure cptr.
//* @param count Un pointeur vers la structure cell représentant le compteur
//*  à mettre à jour.
//* @param n Le numéro de fichier pour lequel le compteur est mis à jour.
//* @return Un pointeur vers la structure cell mise à jour ou NULL en cas
//* d'erreur
extern void *update(cptr *c, cell *count, int n);

//* Récupère le numéro de fichier associé au compteur.
//* @param t Un pointeur vers la structure cell représentant le compteur.
//* @return Le numéro de fichier associé au compteur.
extern int get_numfile(const cell *t);

//* Libère la mémoire allouée pour la structure cptr et les cellules associées.
//* @param c Un pointeur vers un pointeur de la structure cptr à libérer.
extern void dispose_cptr(cptr **c);

//* Récupère la valeur du compteur.
//* @param t Un pointeur vers la structure cell représentant le compteur.
//* @return La valeur du compteur

extern long int get_count(const cell *t);

//* Réinitialise le compteur et le remet dans le stock si nécessaire.
//* @param c Un pointeur vers la structure cptr.
//* @param count Un pointeur vers la cellule dont le compteur doit être
//réinitialisé.
//* @return Un pointeur vers la cellule zéro si la réinitialisation a réussi,
//sinon NULL.
extern void *reset_count(cptr *c, cell *count);

//pour tester test
//* Affiche les compteurs stockés dans la structure cptr ainsi que le nombre
//de compteurs non utilisés.
//* @param c Un pointeur vers la structure cptr.
extern void display(const cptr *c);
