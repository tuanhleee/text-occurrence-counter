#include "input.h"

#define ERROR_INPUT 2
#define ERROR_CAPACITY 1
#define MUL 2

#define TRACK fprintf(stderr, "*** %s:%d\n", __func__, __LINE__);

//* Alloue dynamiquement de la mémoire pour une chaîne de caractères et met
//à jour sa longueur.
//*
//* @param s Un pointeur vers la chaîne de caractères.
//* @param length Un pointeur vers la longueur actuelle de la chaîne de
//caractères.
//* @param new_length La nouvelle longueur désirée de la chaîne de caractères.
//* @return Un pointeur vers la chaîne de caractères allouée dynamiquement si
//réussi, sinon NULL.

static char **malloc_chaine(char **s, size_t *length, size_t new_length) {
  if (*s == NULL || *length == 0) {
    return NULL;
  }
  char *temp = realloc(*s, new_length);
  if (temp == NULL) {
    return NULL;
  }
  *s = temp;
  (*length) = new_length;
  return s;
}

int input_unlimited(char **s, bool *use, size_t *length, FILE *stream,
    const void *o,
    bool (*get_opt_p)(const void *, int n)) {
  if (feof(stream)) {
    return 0;
  }
  *use = false;
  size_t i = 0;
  int c;
  while ((c = fgetc(stream)) != EOF && !isspace(c) && get_opt_p(o, c)) {
    if (i == *length - 1) {
      char **tmp = malloc_chaine(s, length, (*length) * MUL + 1);
      if (*tmp == NULL) {
        *use = false;
        return ERROR_CAPACITY;
      }
      *s = *tmp;
    }
    (*s)[i++] = (char) c;
  }
  if (c == EOF) {
    if (feof(stream)) {
      return -1;
    }
    return ERROR_INPUT;
  }
  if (i == 0) {
    return 0;
  }
  *use = true;
  (*s)[i] = '\0';
  return 0;
}

int input_limited(char **s, bool *use, size_t *length, FILE *stream,
    const void *o,
    bool (*get_opt_p)(const void *, int n)) {
  if (feof(stream)) {
    return 0;
  }
  *use = false;
  size_t i = 0;
  size_t n = 0;
  int c = 0;
  size_t longuer = *length - 1;
  while (n < longuer && (c = fgetc(stream)) != EOF && !isspace(c)
      && get_opt_p(o, c)) {
    if (i == *length - 1) {
      char **tmp = malloc_chaine(s, length, (*length) * MUL);
      if (tmp == NULL) {
        return ERROR_CAPACITY;
      }
      *s = *tmp;
    }
    if (i == *length - 1) {
      char **tmp = malloc_chaine(s, length, (*length) * MUL);
      if (tmp == NULL) {
        return ERROR_CAPACITY;
      }
      *s = *tmp;
    }
    if ((c & IS_CARACTERE_UTF) == IS_CARACTERE_UTF) {
      // Caractère UTF-8 multioctet
      (*s)[i++] = (char) c;
      c = fgetc(stream);
      if (c != EOF) {
        if (i == *length - 1) {
          char **tmp = malloc_chaine(s, length, (*length) * MUL);
          if (tmp == NULL) {
            return ERROR_CAPACITY;
          }
          *s = *tmp;
        }
        (*s)[i++] = (char) c;
      }
    } else {
      // Caractère ASCII
      (*s)[i++] = (char) c;
    }
    n++;
  }
  if (c == EOF) {
    if (feof(stream)) {
      return -1; //
    }
    return ERROR_INPUT;
  }
  if (i == 0) {
    return 0;
  }
  *use = true;
  if (i == *length - 1) {
    char **tmp = malloc_chaine(s, length, (*length) * MUL);
    if (tmp == NULL) {
      return ERROR_CAPACITY;
    }
    *s = *tmp;
  }
  (*s)[i] = '\0';
  if (n == longuer) {
    while (!isspace(c) && (c = fgetc(stream)) != EOF) {
    }
  }
  return 0;
}
