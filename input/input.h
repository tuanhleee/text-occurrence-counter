#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdlib.h>
#define IS_CARACTERE_UTF 0xC0

//* Lit une entrée de manière illimitée depuis un flux de fichier.
//*
//* @param s Un pointeur vers la chaîne de caractères où stocker l'entrée lue.
//* @param use Un pointeur vers une variable booléenne indiquant si des
//* caractères ont été lus avec succès.
//* @param length Un pointeur vers la longueur actuelle de la chaîne de
// caractères.
//* @param stream Le flux de fichier à partir duquel lire l'entrée.
//* @param o Un pointeur vers la structure d'option, utilisée pour les tests
//* personnalisés sur les caractères.
//* @param get_opt_p Un pointeur vers une fonction de test qui prend
//* la structure d'option et un caractère en tant qu'arguments et retourne vrai
//*  ou faux.
//* @return 0 en cas de succès, ERROR_CAPACITY si la capacité mémoire est
//* insuffisante, ERROR_INPUT en cas d'erreur de lecture du flux, ou -1 si
//* la fin du fichier est atteinte sans lire aucun caractère.
extern int input_unlimited(char **s, bool *use, size_t *length, FILE *stream,
    const void *o,
    bool (*get_opt_p)(const void *, int n));

//comme input_unlimited mais cette fois limitee par *length
extern int input_limited(char **s, bool *use, size_t *length, FILE *stream,
    const void *o,
    bool (*get_opt_p)(const void *, int n));
