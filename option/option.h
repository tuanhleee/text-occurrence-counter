#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>

//Cette définition de type option crée une structure nommée option contenant
//trois membres :

//tab_option : un pointeur vers un tableau de booléens.
//n : un entier non signé représentant la taille du tableau tab_option.
//option_i : un entier long représentant une option spécifiqu
typedef struct option option;
struct option {
  bool *tab_option;
  size_t n;
  long int option_i;
};

typedef enum {
  OPT_RESTRICTED,
  OPT_AVL,
  OPT_HASHTABLE,
  OPT_LEXICOGRAPHIE,
  OPT_NUMERIC,
  OPT_RENVERSE,
  OPT_INITIAL,
  OPT_PUNC,
  OPT_S_, OPT_W,
  OPT_SORT,
  OPT_C
} OptionIndex;

#define  TRACK fprintf(stderr, "*** %s:%d\n", __func__, __LINE__);
#define AVL "-a"
#define AVL_BINARY_TREE "--avl-binary-tree"
#define H "-h"
#define HASTABLE "--hash-table"
#define L "-l"
#define LEXICOGRAPHICAL "--sort=lexicographie"
#define N  "-n"
#define NUMERIC  "--numeric"
#define R  "-R"
#define RENVERSE "--reverse"
#define P "-p"
#define PONCTUATION_LIKE_SPACE "--punctuation-like-space"
#define C "-c"
#define COUNTER "--counter"

//option with value
#define RESTRICTED "--restrict="
#define R_ "-r="
#define W "-w="
#define WORD_PROCESSING "--word-processing="
#define I "-i="
#define INITIAL "--initial="
#define S "-s="
#define SORT "--sort="
#define S_ "-S"
#define NONE_SORT "--sort=none"

#define DASH '-'
#define SHORT 0
#define LONG 1
#define NO_FILE -1
#define NONE 0

#define FAIL -1
#define SIGN_STDIN 1
#define SIGN "--"
#define FILE_ "-"

//Program Information

#define HELP "--help"
#define  VERSION "--version"
#define  USAGE "--usage"
#define VERSION_()                                                    \
  printf("21/04/2024\n")
#define SUCCESS -2

//Crée un nouvel objet option avec une taille spécifiée et initialise ses
// attributs.
//@param n La taille du tableau tab_option.
//@return Un pointeur vers le nouvel objet option créé si réussi, NULL sinon.
option *option_emty(size_t n);

//Renvoie vrai ou faux en fonction de la valeur à l'indice n dans le tableau
// c->tab.
//*
//* @param c Un pointeur vers la structure option contenant le tableau et sa
// taille.
//* @param n L'indice dans le tableau à vérifier.
//* @return Vrai si la valeur à l'indice n est vraie, Faux sinon ou si c est
// NULL.

extern bool get_value_option(const option *c, size_t n);

//* Analyse les arguments de la ligne de commande pour extraire les options et
// les fichiers.
//*
//* @param argc Le nombre d'arguments de la ligne de commande.
//* @param argv Le tableau d'arguments de la ligne de commande.
//* @param tab_file Un tableau dans lequel stocker les noms des fichiers.
//* @param nb_file Un pointeur vers un entier pour stocker le nombre de fichiers
// trouvés.
//* @return Un pointeur vers une structure option contenant les options
// analysées si réussi, NULL sinon.
option *custom_getopt(int argc, char *argv[], char *tab_file[], int *nb_file);

//liberer tout les ressource de qui pointe par  de option *c
extern void dispose_opt(option **c);

//La fonction get_opt_p renvoie vrai ou faux en fonction de la valeur de
//l'option de ponctuation (OPT_PUNC) dans la structure option et de la valeur
//de n. Si l'option de ponctuation est désactivée (faux), la fonction renvoie
//toujours vrai. Si l'option de ponctuation est activée (vrai), la fonction
//renvoie vrai si n n'est pas un caractère de ponctuation, sinon elle renvoie
//faux.
extern bool get_opt_p(const option *c, int n);
