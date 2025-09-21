#include <locale.h>
#include "hashtable.h"
#include "holdall.h"
#include "input.h"
#include "gestion.h"
#include "option.h"
#include "bst.h"
#include <limits.h>

#define SIZE 10
#define  TRACK fprintf(stderr, "*** %s:%d\n", __func__, __LINE__);
//scptr_display, scptr_display2, scptr_display, scptr_display4:
//* Affiche les informations d'une cellule de compteur spécifié.
//* Cette fonction affiche les informations d'une cellule de compteur spécifié.
//* @param s La chaîne de caractères à afficher.
//* @param t Un pointeur vers la cellule de compteur à afficher.
//* @return 0 si aucune information n'est affichée, une valeur différente de
//zéro sinon.
static int scptr_display(const char *s, const void *t);
static int scptr_display2(const char *s, const void *t);
static int scptr_display3(const char *s, const void *t);
static int scptr_display4(const char *s, const void *t);

enum gestion_error {
  no_error,
  error_malloc,
  error_input
};

//reserve a la option numeric et mode AVL
typedef struct type type;
struct type {
  char *s;
  cell *cpt;
};
//  rfree : rfree2 libère la zone mémoire pointée par ptr et renvoie zéro.
static int rfree(void *ptr);
//meme chose mais ici type passer est type*
int rfree2(void *ptr);
//  str_hashfun, str_hashfun2 : l'une des fonctions de pré-hachage conseillées
// par Kernighan
//    et Pike pour les chaines de caractères.
static size_t str_hashfun(const char *s);
// memme chose  mais ici avec s->s
static size_t str_hashfun2(const type *s);

//* Recherche un élément dans une structure de données spécifiée.
//*
//* Cette fonction recherche un élément dans une structure de données spécifiée,
//* en utilisant le mode de recherche indiqué et la chaîne de caractères
// fournie.
//* Elle prend également en compte les options de recherche spécifiées.
//*
//* @param mode Un pointeur vers la structure de données dans laquelle effectuer
//* la recherche.
//* @param s La chaîne de caractères à rechercher.
//* @param opt Un pointeur vers la structure d'options de recherche.
//* @return Un pointeur vers l'élément recherché s'il est trouvé, sinon NULL.
static void *search(void *mode, char *s, const option *opt);

#define UPDATE_OR_RESET(t, c, number_file) \
  do { \
    if (t != NULL && get_numfile(t) == number_file) { \
      t = update(c, t, number_file); \
    } else if (t != NULL) { \
      if (get_count(t) != 0) { \
        t = reset_count(c, t); \
      } \
    } \
  } while (0)

#define UPDATE_OR_RESET_(t, c, number_file) \
  if (get_count(t) > 0 && get_numfile(t) == 0) { \
    t = reset_count(c, t); \
    t = ajout(c, number_file); \
  } else if (get_numfile(t) == number_file) { \
    t = update(c, t, number_file); \
  } else { \
    t = reset_count(c, t); \
  }

//* Cette fonction effectue le traitement d'une opération sur une structure de
//données en fonction desoptions spécifiées. Elle recherche d'abord l'élément
//dans la structure de données spécifiée.Si l'élément est trouvé,
//elle met à jour les données associées et les ajoute à la structure si
//nécessaire.Si l'élément n'est pas trouvé, elle crée un nouvel élément et
//l'ajoute à la structure de données.
//* @param mode Un pointeur vers la structure de données dans laquelle
//effectuerle traitement.
//* @param s Un pointeur vers la chaîne de caractères à traiter.
//* @param o Un pointeur vers la structure d'options pour spécifier le
//comportement du traitement.
//* @param holdall_s Un pointeur vers la structure holdall pour stocker les
//éléments en cas de besoin.
//* @param number_file Le nombre de fichiers (compteurs).
//* @param length Un pointeur vers une taille_t pour stocker la longueur de la
//chaîne de caractères.
//* @param length_ La longueur de la chaîne de caractères.
//* @param use Un pointeur vers un booléen indiquant si l'élément doit être
//utilisé.
//* @param c Un pointeur vers la structure cptr pour gérer les compteurs
//associés aux éléments.
//* @return 0 en cas de succès, une valeur différente de zéro en cas d'erreur.
int process1(void *mode, char **s, option *o,
    holdall *holdall_s, int number_file, size_t *length, long int length_,
    bool *use,
    cptr *c);

//meme chose  mais reserve pour option restricted
int process2(void *mode, char **s, option *o,
    holdall *holdall_s, int number_file, size_t *length, long int length_,
    bool *use,
    cptr *c);

//Cette fonction compare deux éléments de type type. Elle utilise la
//fonction strcoll pour comparer les champs s des deux éléments. Elle
//retourne un entier négatif si a est inférieur à b, un entier positif si a
//est supérieur à b et zéro si les deux éléments sont égaux.
int compar(const type *a, const type *b);

//Cette fonction compare deux chaînes de caractères (const char *). Elle utilise
//la fonction strcoll pour comparer les chaînes a et b. Elle retourne un entier
//négatif si a est inférieur à b, un entier positif si a est supérieur à b et
//zéro si les deux chaînes sont égales.
int compar_(const char *a, const char *b);

//: Cette fonction est similaire à compar, mais elle multiplie le résultat de
//la comparaison par la valeur de la macro REN.
int compar_2(const type *a, const type *b);

int compar_3(const type *a, const type *b);
//: Cette fonction compare deux éléments de type type en fonction de la valeur
//du champ cpt. Si les valeurs de cpt sont égales, elle compare les champs s
//des éléments en utilisant strcoll. Sinon, elle compare les valeurs de cpt.
//Elle retourne un entier négatif si a est inférieur à b, un entier positif
//si a est supérieur à b et zéro si les deux éléments sont égaux.
int compar_4(const type *a, const type *b);
//: Cette fonction est similaire à compar_3, mais elle multiplie le résultat
//de la comparaison par la valeur de la macro REN.
int compar_5(const char *a, const char *b);
//: Cette fonction est similaire à compar_, mais elle multiplie le résultat
//de la comparaison par la valeur de la macro REN.
typedef int (*compare_func)(const void *, const void *);

//Cette fonction sélectionne et retourne une fonction de comparaison en fonction
//des options spécifiées. La fonction de comparaison sélectionnée dépend des
//paramètres de configuration tels que le type d'arbre (avl), l'ordre
//lexicographique (lexico),reverse
compare_func select_comparison_function(const bool avl, const bool lexico,
    const bool reverse);

//* Effectue une opération d'entrée en fonction des options spécifiées.
//*
//* Cette fonction effectue une opération d'entrée en fonction des options
//spécifiées.
//* Si l'option INITIALE est activée et la valeur de l'option option_i est
//différente de zéro,
//* elle appelle la fonction input_limited pour une entrée limitée, sinon elle
//appelle la fonction input_unlimited pour une entrée illimitée.
//*
//* @param s Un pointeur vers un pointeur de caractères pour stocker l'entrée.
//* @param use Un pointeur vers un booléen pour indiquer si l'entrée doit être
//utilisée.
//* @param length Un pointeur vers une taille_t pour stocker la longueur de
//'entrée.
//* @param stream Un pointeur vers le flux de fichier à partir duquel lire
//l'entrée.
//* @param o Un pointeur vers la structure d'options pour spécifier le
//comportemente l'entrée.
//* @return 0 en cas de succès, une valeur différente de zéro en cas d'erreur.

extern int input(char **s, bool *use, size_t *length, FILE *stream,
    const option *o);

#define INPUT_STAN(n)  \
  printf("--- starts reading for #%d FILE \n", n);
#define END_INPUT_STAN(n) \
  printf("--- ends reading for #%d FILE \n", n);

#define ONE_FILE 2
#define START 1
int main(int argc, char *argv[]) {
  if (argc < ONE_FILE) {
    fprintf(stderr, "ERROR INPUT\n");
    return EXIT_FAILURE;
  }
  char *tab_file[argc];
  int nb_file = START;
  option *o = custom_getopt(argc, argv, tab_file, &nb_file);
  if (nb_file < 1 || o == NULL) {
    if (nb_file == SUCCESS) {
      return EXIT_SUCCESS;
    }
    fprintf(stderr, "ERROR INPUT\n");
    return EXIT_FAILURE;
  }
  if (nb_file == ONE_FILE
      && get_value_option(o, OPT_RESTRICTED)) {
    tab_file[nb_file] = (char *) FILE_;
    nb_file++;
  }
  setlocale(LC_COLLATE, "");
  int r = EXIT_SUCCESS;
  void *mode = NULL;
  if (get_value_option(o, OPT_AVL)) {
    mode = (void *) bst_empty((int (*)(const void *, const void *))compar);
  } else if (get_value_option(o, OPT_NUMERIC)) {
    mode = (void *) hashtable_empty((int (*)(const void *,
        const void *))compar,
        (size_t (*)(const void *))str_hashfun2);
  } else {
    mode = (void *) hashtable_empty((int (*)(const void *,
        const void *))compar_,
        (size_t (*)(const void *))str_hashfun);
  }
  holdall *holdall_s = holdall_empty();
  cptr *c = cptr_empty(argc - 1);
  if (mode == NULL || holdall_s == NULL || c == NULL) {
    goto error_capacity;
  }
  for (int n = 1; n < nb_file; n++) {
    bool standard_input = (strcmp(tab_file[n], FILE_) == 0);
    FILE *file;
    if (standard_input) {
      file = stdin;
    } else {
      file = fopen(tab_file[n], "r");
      if (file == NULL) {
        goto error_oppen_file;
      }
    }
    long int length_ = 0;
    if (!get_value_option(o, OPT_INITIAL) || o->option_i == 0) {
      length_ = SIZE;
    } else {
      length_ = o->option_i + 1;
    }
    char *s = malloc(sizeof(*s) * (size_t) length_);
    if (s == NULL) {
      goto error_capacity;
    }
    size_t length = (size_t) length_;
    int number_file = n - 1;
    bool use = false;
    if (standard_input) {
      INPUT_STAN(n);
    }
    int x = input(&s, &use, &length, file, o);
    while (x != -1) {
      if (use) {
        if (!get_value_option(o, OPT_RESTRICTED)) {
          if (process1(mode, &s, o, holdall_s, number_file, &length,
              length_,
              &use, c) == FAIL) {
            goto error_capacity;
          }
        } else {
          if (process2(mode, &s, o, holdall_s, number_file, &length,
              length_,
              &use, c) == FAIL) {
            goto error_capacity;
          }
        }
      }
      x = input(&s, &use, &length, file, o);
      if (x == -1 && use == false) {
        free(s);
      }
    }
    if (x != -1) {
      if (x == error_malloc) {
        goto error_capacity;
      }
      goto  error_input;
    }
    if (standard_input) {
      END_INPUT_STAN(n);
      clearerr(file);
      rewind(file);
    } else {
      if (fclose(file) != 0) {
        goto error_close_file;
      }
    }
  }
  printf("\t");
  for (int i = 1; i < nb_file; i++) {
    if (strcmp(tab_file[i], FILE_) == 0) {
      printf("\"%s\"\t", tab_file[i]);
    } else {
      printf("%s\t", tab_file[i]);
    }
  }
  printf("\n");
  if (!get_value_option(o, OPT_S_)) {
    const bool avl = get_value_option(o, OPT_AVL);
    const bool lexico = get_value_option(o, OPT_LEXICOGRAPHIE);
    const bool reverse = get_value_option(o, OPT_RENVERSE);
    compare_func sort_func = select_comparison_function(avl, lexico, reverse);
    holdall_sort(holdall_s, sort_func);
  }
  void *(*search_func)(void *, void *);
  int (*display_func)(void *, void *);
  search_func
    = get_value_option(o,
      OPT_AVL) ? (void *(*)(void *, void *))bst_search : (void *(*)(void *,
          void *))hashtable_search;
  if (get_value_option(o, OPT_RESTRICTED)) {
    display_func
      = (get_value_option(o,
        OPT_AVL)
        || get_value_option(o, OPT_NUMERIC)) ? (int (*)(void *,
            void *))scptr_display4 : (int (*)(void *, void *))scptr_display3;
  } else {
    display_func
      = (get_value_option(o,
        OPT_AVL)
        || get_value_option(o, OPT_NUMERIC)) ? (int (*)(void *,
            void *))scptr_display2 : (int (*)(void *, void *))scptr_display;
  }
  if (holdall_apply_context(holdall_s, mode, search_func, display_func) != 0) {
    goto error_write;
  }
  goto dispose;
error_input:
  fprintf(stderr, "*** Error: intput\n");
  goto error;
error_capacity:
  fprintf(stderr, "*** Error: Not enough memory\n");
  goto error;
error_write:
  fprintf(stderr, "*** Error: A write error occurs\n");
  goto error;
error_close_file:
  fprintf(stderr, "*** Error:  error of close file\n");
  goto error;
error_oppen_file:
  fprintf(stderr, "*** Error:  error of open file\n");
  goto error;
error:
  r = EXIT_FAILURE;
  goto dispose;
dispose:
  if (get_value_option(o, OPT_AVL)) {
    bst_dispose((bst **) &mode);
  } else {
    hashtable_dispose((hashtable **) &mode);
  }
  if (get_value_option(o, OPT_AVL) || get_value_option(o, OPT_NUMERIC)) {
    if (holdall_s != NULL) {
      holdall_apply(holdall_s, rfree2);
    }
  } else {
    if (holdall_s != NULL) {
      holdall_apply(holdall_s, rfree);
    }
  }
  holdall_dispose(&holdall_s);
  if (get_value_option(o, OPT_C)) {
    display(c);
  }
  dispose_cptr(&c);
  dispose_opt(&o);
  return r;
}

size_t str_hashfun(const char *s) {
  size_t h = 0;
  for (const unsigned char *p = (const unsigned char *) s; *p != '\0'; ++p) {
    h = 37 * h + *p;
  }
  return h;
}

size_t str_hashfun2(const type *s) {
  const char *t = s->s;
  size_t h = 0;
  for (const unsigned char *p = (const unsigned char *) t; *p != '\0'; ++p) {
    h = 37 * h + *p;
  }
  return h;
}

//Affiche une chaîne de caractères  qui pointer par s en prenant en compte
//les caractères UTF-8 multioctets.
//Retourne : Le nombre total de caractères affichés, y compris les caractères
//multioctets.

//IS_CARACTERE_UTF dans input.h
static size_t affiche_chaine(const char *s) {
  size_t n = 0;
  while (*s != '\0') {
    if ((*s & IS_CARACTERE_UTF) == IS_CARACTERE_UTF) {
      printf("%c%c", *s, *(s + 1));
      s += 2;
    } else {
      printf("%c", *s);
      s++;
    }
    n++;
  }
  return n;
}

#define REPR_TAB 8
#define SEPP(n) \
  printf("%*s", (int) (REPR_TAB * n), "")
#define SEP(n) \
  printf("%*s", (int) (n), "")
#define ESPACE(espace, n) \
  espace = espace - (int) n;
#define TABULATION printf("\t");

//* Affiche les informations d'une cellule sur la sortie standard,
//* conditionnellement à un test.
//*
//* Cette fonction affiche les informations d'une cellule sur la sortie standard
//*  uniquement si le paramètre `test` est vrai. Elle prend en compte la
// longueur
//*  de la chaîne `s` et ajuste la position de l'affichage en fonction.
//*
//* @param s La chaîne de caractères à afficher.
//* @param c Un pointeur vers la structure cellule contenant des informations
//supplémentaires.
//@param test Un booléen indiquant si l'affichage doit être effectué ou non.
//* @param cptr_ Une valeur longue à afficher.
//* @return retourne 0.
static int display_cell(const char *s, const cell *c, const bool test,
    long int cptr_) {
  if (test) {
    size_t n = affiche_chaine(s);
    int file = get_numfile(c);
    int espace = REPR_TAB * (file == 0 ? 1 : file);
    if (n < REPR_TAB) {
      ESPACE(espace, n);
      SEP(espace);
    } else {
      if ((int) n < espace) {
        TABULATION SEP(REPR_TAB - (n % REPR_TAB));
      } else {
        SEP(espace - ((int) n % REPR_TAB));
      }
    }
    printf("%ld\n", cptr_);
  }
  return 0;
}

int scptr_display(const char *s, const void *t) {
  cell *c = (cell *) t;
  long int cptr_ = get_count(c);
  return display_cell(s, c, (cptr_ != 0), cptr_);
}

#define UNUSED(x) (void) (x)

int scptr_display2(const char *s, const void *t) {
  UNUSED(s);
  type *p = (type *) t;
  long int cptr_ = get_count(p->cpt);
  return display_cell(p->s, p->cpt, cptr_ != 0, cptr_);
}

int scptr_display3(const char *s, const void *t) {
  cell *c = (cell *) t;
  long int cptr_ = get_count(c);
  return display_cell(s, c, (cptr_ > 0 && get_numfile(c) > 0), cptr_);
}

int scptr_display4(const char *s, const void *t) {
  UNUSED(s);
  type *p = (type *) t;
  long int cptr_ = get_count(p->cpt);
  int n = get_numfile(p->cpt);
  return display_cell(p->s, p->cpt, (cptr_ > 0 && n > 0), cptr_);
}

int rfree(void *ptr) {
  free(ptr);
  return 0;
}

int rfree2(void *ptr) {
  type *t = ptr;
  free(t->s);
  free(t);
  return 0;
}

void *search(void *mode, char *s, const option *opt) {
  void *x = NULL;
  if (get_value_option(opt, OPT_AVL) || get_value_option(opt, OPT_NUMERIC)) {
    type p;
    p.s = s;
    p.cpt = NULL;
    x = get_value_option(opt, OPT_AVL) ? bst_search(mode, &p)
        : hashtable_search(mode, &p);
    if (x == NULL) {
    }
    return x;
  }
  x = hashtable_search(mode, s);
  return x;
}

typedef int (*compare_func)(const void *, const void *);

compare_func select_comparison_function(const bool avl, const bool lexico,
    const bool reverse) {
  if (avl) {
    if (lexico) {
      return reverse ? (compare_func) compar_2 : (compare_func) compar;
    } else {
      return reverse ? (compare_func) compar_4 : (compare_func) compar_3;
    }
  } else {
    if (lexico) {
      return reverse ? (compare_func) compar_5 : (compare_func) compar_;
    } else {
      return reverse ? (compare_func) compar_4 : (compare_func) compar_3;
    }
  }
}

#define REN -1
int compar(const type *a, const type *b) {
  return strcoll(a->s, b->s);
}

int compar_2(const type *a, const type *b) {
  return strcoll(a->s, b->s) * REN;
}

int compar_3(const type *a, const type *b) {
  long int x = a->cpt->value;
  long int y = b->cpt->value;
  if (x == y) {
    return strcoll(a->s, b->s);
  }
  return (x > y) - (y > x);
}

int compar_4(const type *a, const type *b) {
  long int x = a->cpt->value;
  long int y = b->cpt->value;
  if (x == y) {
    return strcoll(a->s, b->s) * REN;
  }
  return ((x > y) - (y > x)) * REN;
}

int compar_5(const char *a, const char *b) {
  return strcoll(a, b) * REN;
}

int compar_(const char *a, const char *b) {
  return strcoll(a, b);
}

int input(char **s, bool *use, size_t *length, FILE *stream,
    const option *o) {
  if (!get_value_option(o, OPT_INITIAL) || o->option_i == 0) {
    return input_unlimited(s, use, length, stream, o, (bool (*)(const void *,
        int))get_opt_p);
  }
  return input_limited(s, use, length, stream, o, (bool (*)(const void *,
      int))get_opt_p);
}

int process1(void *mode, char **s, option *o,
    holdall *holdall_s, int number_file, size_t *length, long int length_,
    bool *use,
    cptr *c) {
  void *t = search(mode, *s, o);
  if (t != NULL) {
    if (get_value_option(o, OPT_AVL) || get_value_option(o, OPT_NUMERIC)) {
      type *cptr_ = (type *) t;
      cell *count = cptr_->cpt;
      if (get_count(count) != 0) {
        UPDATE_OR_RESET(count, c, number_file);
        cptr_->cpt = count;
        if (get_value_option(o, OPT_AVL)) {
          if (bst_add_endofpath(mode, cptr_) == NULL) {
            return FAIL;
          }
        } else {
          if (hashtable_add(mode, cptr_, cptr_) == NULL) {
            return FAIL;
          }
        }
      }
    } else {
      if (get_count((cell *) t) != 0) {
        UPDATE_OR_RESET(t, c, number_file);
        if (hashtable_add(mode, *s, t) == NULL) {
          return FAIL;
        }
      }
    }
    if (get_value_option(o, OPT_INITIAL)) {
      *length = (size_t) o->option_i + 1;
    }
    *use = false;
  } else {
    if (get_value_option(o, OPT_AVL) || get_value_option(o, OPT_NUMERIC)) {
      type *cell_ = malloc(sizeof(*cell_));
      if (cell_ == NULL) {
        return FAIL;
      }
      cell_->s = *s;
      cell_->cpt = ajout(c, number_file);
      if (holdall_put(holdall_s, cell_) != 0) {
        free(s);
        free(cell_);
        return FAIL;
      }
      if (get_value_option(o, OPT_AVL)) {
        if (bst_add_endofpath(mode, cell_) == NULL) {
          return FAIL;
        }
      } else {
        if (hashtable_add(mode, cell_, cell_) == NULL) {
          return FAIL;
        }
      }
    } else {
      if (holdall_put(holdall_s, *s) != 0) {
        free(*s);
        return FAIL;
      }
      if (hashtable_add(mode, *s, ajout(c, number_file)) == NULL) {
        return FAIL;
      }
    }
    *s = malloc((size_t) length_);
    if (s == NULL) {
      return FAIL;
    }
    *length = (size_t) length_;
  }
  return 0;
}

int process2(void *mode, char **s, option *o,
    holdall *holdall_s, int number_file, size_t *length, long int length_,
    bool *use,
    cptr *c) {
  void *t = search(mode, *s, o);
  if (t == NULL && number_file == 0) {
    if (get_value_option(o, OPT_AVL) || get_value_option(o, OPT_NUMERIC)) {
      type *cell_ = malloc(sizeof(*cell_));
      if (cell_ == NULL) {
        return FAIL;
      }
      cell_->s = *s;
      cell_->cpt = ajout(c, number_file);
      if (holdall_put(holdall_s, cell_) != 0) {
        free(s);
        free(cell_);
        return FAIL;
      }
      if (get_value_option(o, OPT_AVL)) {
        if (bst_add_endofpath(mode, cell_) == NULL) {
          return FAIL;
        }
      } else {
        if (hashtable_add(mode, cell_, cell_) == NULL) {
          return FAIL;
        }
      }
    } else {
      if (holdall_put(holdall_s, *s) != 0) {
        free(*s);
        return FAIL;
      }
      if (hashtable_add(mode, *s, ajout(c, number_file)) == NULL) {
        return FAIL;
      }
    }
    *s = malloc((size_t) length_);
    if (s == NULL) {
      return FAIL;
    }
    *length = (size_t) length_;
    *use = false;
  } else {
    if (number_file > 0 && t != NULL) {
      if (get_value_option(o, OPT_AVL) || get_value_option(o, OPT_NUMERIC)) {
        type *cptr_ = (type *) t;
        cell *count = cptr_->cpt;
        if (get_count(count) > 0) {
          UPDATE_OR_RESET_(count, c, number_file);
          cptr_->cpt = count;
          if (get_value_option(o, OPT_AVL)) {
            if (bst_add_endofpath(mode, cptr_) == NULL) {
              return FAIL;
            }
          } else {
            if (hashtable_add(mode, cptr_, cptr_) == NULL) {
              return FAIL;
            }
          }
        }
      } else {
        if (get_count((cell *) t) > 0) {
          UPDATE_OR_RESET_(t, c, number_file);
          if (hashtable_add(mode, *s, t) == NULL) {
            return FAIL;
          }
        }
      }
    }
    if (get_value_option(o, OPT_INITIAL)) {
      *length = (size_t) o->option_i + 1;
    }
    *use = false;
  }
  return 0;
}
