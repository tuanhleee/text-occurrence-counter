//  bst.c : partie implantation d'un module polymorphe pour la spécification
//   ABINR du TDA ABinR(T).

#include "bst.h"

//=== Type cbst ================================================================

//--- Définition cbst ----------------------------------------------------------

typedef struct cbst cbst;

struct cbst {
  cbst *next[2];
  const void *ref;
  size_t size;
  int height;
};
#define TRACK fprintf(stderr, "*** %s:%d\n", __func__, __LINE__);

//--- Raccourcis cbst ----------------------------------------------------------

#define EMPTY()     NULL
#define SIZE(p) ((p)->size)
#define HEIGHT(p) ((p)->height)
#define IS_EMPTY(p) ((p) == NULL)
#define LEFT(p)     ((p)->next[0])
#define RIGHT(p)    ((p)->next[1])
#define REF(p)      ((p)->ref)
#define NEXT(p, d)  ((p)->next[(d) > 0])

//--- Divers -------------------------------------------------------------------

static size_t add__size_t(size_t x1, size_t x2) {
  return x1 + x2;
}

static size_t max__size_t(size_t x1, size_t x2) {
  return x1 > x2 ? x1 : x2;
}

static size_t min__size_t(size_t x1, size_t x2) {
  return x1 < x2 ? x1 : x2;
}

//--- Fonctions cbst -----------------------------------------------------------

//  DEFUN_CBST__MEASURE : définit la fonction récursive de nom « cbst__ ## fun »
//    et de paramètre un pointeur d'arbre binaire, qui renvoie zéro si l'arbre
//    est vide et « 1 + oper(r0, r1) » sinon, où r0 et r1 sont les valeurs
//    renvoyées par les appels récursifs de la fonction avec les pointeurs des
//    sous-arbres gauche et droit de l'arbre comme paramètres.
#define DEFUN_CBST__MEASURE(fun, oper)                                         \
  static size_t cbst__ ## fun(const cbst * p) {                                \
    return IS_EMPTY(p)                                                         \
      ? 0                                                                      \
      : 1 + oper(cbst__ ## fun(LEFT(p)), cbst__ ## fun(RIGHT(p)));             \
  }

//  cbst__size, cbst__height, cbst__distance : définition.

//DEFUN_CBST__MEASURE(size, add__size_t)
//DEFUN_CBST__MEASURE(height, max__size_t)
DEFUN_CBST__MEASURE(distance, min__size_t)

//  cbst__dispose : libère les ressources allouées à l'arbre binaire pointé par
//    p.
static void cbst__dispose(cbst *p) {
  while (p != NULL) {
    cbst *t;
    if (!IS_EMPTY(LEFT(p)) && !IS_EMPTY(RIGHT(p))
        && SIZE(LEFT(p)) < SIZE(RIGHT(p))) {
      cbst__dispose(LEFT(p));
      t = RIGHT(p);
    } else {
      cbst__dispose(RIGHT(p));
      t = LEFT(p);
    }
    free(p);
    p = t;
  }
}

static size_t cbst__size(const cbst *p) {
  return IS_EMPTY(p) ? 0 : SIZE(p);
}

static void cbst__update_size(cbst *p) {
  SIZE(p) = 1 + add__size_t(cbst__size(LEFT(p)), cbst__size(RIGHT(p)));
}

static size_t cbst__height(const cbst *p) {
  return IS_EMPTY(p) ? 0 : (size_t) HEIGHT(p);
}

static void cbst__update_height(cbst *p) {
  HEIGHT(p) = 1 + (int) max__size_t(cbst__height(LEFT(p)), cbst__height(RIGHT(
      p)));
}

#define UPDATE(t) \
  cbst__update_size(t); \
  cbst__update_height(t);

static int cbst__balance(const cbst *p) {
  return (int) (cbst__height(LEFT(p)) - cbst__height(RIGHT(p)));
}

#define PRINT_ROTATION printf("%s\n", __func__);
static void cbst__rotation_left(cbst **pp) {
  cbst *t = RIGHT(*pp);
  RIGHT(*pp) = LEFT(t);
  LEFT(t) = *pp;
  UPDATE(*pp);
  *pp = t;
  UPDATE(*pp);
}

static void cbst__rotation_right(cbst **pp) {
  cbst *t = LEFT(*pp);
  LEFT(*pp) = RIGHT(t);
  RIGHT(t) = *pp;
  UPDATE(*pp);
  *pp = t;
  UPDATE(*pp);
}

static void cbst__rotation_left_right(cbst **pp) {
  cbst__rotation_left(&LEFT(*pp));
  cbst__rotation_right(pp);
}

static void cbst__rotation_right_left(cbst **pp) {
  cbst__rotation_right(&RIGHT(*pp));
  cbst__rotation_left(pp);
}

static int cbst__balancing(cbst **pp) {
  UPDATE(*pp);
  int c = cbst__balance(*pp);
  if (c == 0 || c == 1 || c == -1) {
    return 0;
  } else if (c == 2) {
    cbst__balance(LEFT(*pp)) >= 0 ? cbst__rotation_right(pp)
    : cbst__rotation_left_right(pp);
  } else {
    cbst__balance(RIGHT(*pp)) <= 0 ? cbst__rotation_left(pp)
    : cbst__rotation_right_left(pp);
  }
  return 1;
}

static void *cbst__remove_max(cbst **pp) {
  if (IS_EMPTY(RIGHT(*pp))) {
    const void *r = REF(*pp);
    cbst *t = *pp;
    *pp = LEFT(t);
    free(t);
    return (void *) r;
  }
  void *r = cbst__remove_max(&RIGHT(*pp));
  cbst__balancing(pp);
  return r;
}

static void cbst__remove_root(cbst **pp) {
  cbst *t = *pp;
  *pp = RIGHT(t);
  free(t);
}

static void *cbst__remove_climbup_left(cbst **pp, const void *ref,
    int (*compar)(const void *, const void *)) {
  if (IS_EMPTY(*pp)) {
    return NULL;
  }
  int c = compar(ref, REF(*pp));
  if (c == 0) {
    if (IS_EMPTY(LEFT(*pp))) {
      const void *r = REF(*pp);
      cbst__remove_root(&(*pp));
      return (void *) r;
    }
    const void *r = cbst__remove_max(&LEFT(*pp));
    (*pp)->ref = r;
    cbst__balancing(pp);
    return (void *) r;
  }
  void *r = cbst__remove_climbup_left(&NEXT((*pp), c), ref, compar);
  cbst__balancing(pp);
  return r;
}

static void *cbst__search(const cbst *p, const void *ref,
    int (*compar)(const void *, const void *)) {
  if (IS_EMPTY(p)) {
    return EMPTY();
  }
  int c = compar(ref, REF(p));
  return (c == 0) ? (void *) REF(p) : cbst__search(NEXT(p, c), ref, compar);
}

static void *cbst__add_endofpath(cbst **pp, const void *ref,
    int (*compar)(const void *, const void *)) {
  if (IS_EMPTY(*pp)) {
    cbst *s = malloc(sizeof(cbst));
    if (IS_EMPTY(s)) {
      return EMPTY();
    }
    REF(s) = ref;
    LEFT(s) = EMPTY();
    RIGHT(s) = EMPTY();
    *pp = s;
    (*pp)->size = 1;
    (*pp)->height = 1;
    return (void *) ref;
  }
  int c = compar(ref, REF(*pp));
  if (c == 0) {
    const void *r = REF(*pp);
    (*pp)->ref = ref;
    return (void *) r;
  }
  void *r = cbst__add_endofpath(&NEXT(*pp, c),
      ref, compar);
  cbst__balancing(pp);
  return r;
}

static size_t cbst__number(const cbst *p, const void *ref,
    int (*compar)(const void *, const void *), size_t number) {
  if (IS_EMPTY(p)) {
    return number;
  }
  int c = compar(ref, REF(p));
  if (c == 0) {
    return number;
  }
  return cbst__number(NEXT(p, c), ref, compar, number * 2 + ((c > 0) ? 1 : 0));
}

static size_t cbst__rank(const cbst *p, const void *ref,
    int (*compar)(const void *, const void *), size_t rank) {
  if (IS_EMPTY(p)) {
    return rank;
  }
  int c = compar(ref, REF(p));
  if (c == 0) {
    return rank;
  }
  if (c > 0) {
    rank += (IS_EMPTY(RIGHT(p)) ? 0 : cbst__size(LEFT(RIGHT(p)))) + 1;
  } else {
    size_t n = 0;
    if (!IS_EMPTY(LEFT(p))) {
      n += cbst__size(RIGHT(LEFT(p)));
    }
    --rank;
    rank -= n;
  }
  return cbst__rank(NEXT(p, c), ref, compar, rank);
}

#define REPR__TAB 4
#define GRAPHIC_VIDE "|\n"
#define NUMBER 3
#define BRANCHE "|"
#define BRANCH_LEFT "\\"
#define BRANCH_RIGHT "/"
#define BRANCHE__ "---"
#define ESPACE 1
#define MAX 50

#define CHEMIN 1
//print_branch : fonction permet de afficher les branche d'un noeud qui a numero
//egal a number.la chaine de caracte qui pointe par s est une chaine vide  qui
//permet de stoker les bits binaires qui a ete converti par number.
static void print_branch(size_t number, char *s) {
  size_t i = 0;
  while (number != 1) {
    s[MAX - i - 1] = number % 2;
    number /= 2;
    i++;
  }
  size_t n = MAX - i;
  size_t c = n;
  if (i != 0) {
    if (n == MAX - 1) {
      printf("%s", ((int) s[n] == CHEMIN) ? BRANCH_RIGHT : BRANCH_LEFT);
    } else {
      while (n != MAX - 1) {
        if (s[n] != s[n + 1]) {
          if (c == n) {
            printf(BRANCHE);
          } else {
            printf("%*s", (int) (REPR__TAB), BRANCHE);
          }
        } else {
          printf("%*s", (c == n) ? (int) ESPACE : (int) (REPR__TAB), "");
        }
        n++;
      }
      printf("%*s", (int) (REPR__TAB), ((int) s[n] == CHEMIN)
          ? BRANCH_RIGHT : BRANCH_LEFT);
    }
    printf(BRANCHE__);
  }
}

void cbst_repr_graphic(cbst *p, size_t number, char *s, void (*put)(
    const void *ref)) {
  if (IS_EMPTY(p)) {
    print_branch(number, s);
    printf(GRAPHIC_VIDE);
    return;
  }
  cbst_repr_graphic(RIGHT(p), 2 * number + 1, s, put);
  print_branch(number, s);
  put(REF(p));
  printf(" b=%d", cbst__balance(p));
  printf("\n");
  cbst_repr_graphic(LEFT(p), 2 * number, s, put);
}

//=== Type bst =================================================================

//--- Définition bst -----------------------------------------------------------

struct bst {
  int (*compar)(const void *, const void *);
  cbst *root;
};

//--- Fonctions bst ------------------------------------------------------------

bst *bst_empty(int (*compar)(const void *, const void *)) {
  bst *t = malloc(sizeof *t);
  if (t == NULL) {
    return NULL;
  }
  t->compar = compar;
  t->root = EMPTY();
  return t;
}

void bst_dispose(bst **tptr) {
  if (*tptr == NULL) {
    return;
  }
  cbst__dispose((*tptr)->root);
  free(*tptr);
  *tptr = NULL;
}

void *bst_add_endofpath(bst *t, const void *ref) {
  return (ref
    == NULL) ? NULL : (void *) (cbst__add_endofpath(&t->root, ref, t->compar));
}

void *bst_remove_climbup_left(bst *t, const void *ref) {
  return (ref
    == NULL) ? NULL : (void *) (cbst__remove_climbup_left(&t->root, ref,
      t->compar));
}

void *bst_search(bst *t, const void *ref) {
  return (ref
    == NULL) ? NULL : (void *) (cbst__search(t->root, ref, t->compar));
}

size_t bst_size(bst *t) {
  return cbst__size(t->root);
}

size_t bst_height(bst *t) {
  return cbst__height(t->root);
}

size_t bst_distance(bst *t) {
  return cbst__distance(t->root);
}

size_t bst_number(bst *t, const void *ref) {
  return cbst__number(t->root, ref, t->compar, 1);
}

size_t bst_rank(bst *t, const void *ref) {
  return cbst__rank(t->root, ref, t->compar, cbst__size(LEFT(t->root)));
}

void bst_repr_graphic(bst *t, void (*put)(const void *ref)) {
  char *s;
  s = malloc(MAX);
  if (s != NULL) {
    cbst_repr_graphic(t->root, 1, s, put);
    free(s);
  } else {
    printf(GRAPHIC_VIDE);
  }
}
