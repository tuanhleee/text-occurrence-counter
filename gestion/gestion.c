#include "gestion.h"

#define SIZE 4
#define MUL 2
//(struct stock) qui stocke des pointeurs de cellules (cell *) pour
//permettre le recyclage lorsque nécessaire :
typedef struct stock stock;
struct stock {
  cell *g; // Pointeur vers la première cellule dans le stock
  size_t size; // Taille du stock (nombre de cellules)
};

//Structure représentant un ensemble de pointeurs vers des cellules, avec un
//stock et une cellule spéciale.
struct cptr {
  cell **head; // Tableau de pointeurs de cellules
  stock s; // Stock de cellules
  cell zero; // Cellule spéciale = compteur avev value 0
  int nb_file; // Nombre de fichiers (compteurs)
};

cptr *cptr_empty(int n) {
  if (n <= 0) {
    return NULL;
  }
  cptr *c = malloc(sizeof(*c));
  if (c == NULL) {
    return NULL;
  }
  c->s.g = NULL;
  cell **tab = malloc(sizeof(cell *) * (size_t) n);
  if (tab == NULL) {
    free(c);
    return NULL;
  }
  for (size_t i = 0; i < (size_t) n; i++) {
    tab[i] = NULL;
  }
  c->nb_file = n;
  c->head = tab;
  c->s.size = 0;
  // pour marqer le chaine 0;
  c->zero.value = 0;
  c->zero.file = -1;
  return c;
}

//* Fonction qui vérifie si le stockage de la structure cptr est vide.
//* @param c Un pointeur vers la structure cptr.
//* @return true si le stockage est vide, false sinon.

static bool stockage_is_empty(cptr *c) {
  return c->s.size == 0;
}

//* Fonction qui gère le stockage des cellules dans la structure cptr.
//* @param c Un pointeur vers la structure cptr.
//* @param s Un pointeur vers la cellule à stocker.
//* @return Un pointeur vers la cellule stockée ou NULL en cas d'erreur.

static void *stockage(cptr *c, cell *s) {
  if (c == NULL || s == NULL) {
    return NULL;
  }
  size_t n = (size_t) get_numfile(s);
  if (s->prev == NULL) {
    c->head[n] = s->next;
    if (c->head[n] != NULL) {
      c->head[n]->prev = NULL;
    }
  } else if (s->next == NULL) {
    s->prev->next = NULL;
  } else {
    s->prev->next = s->next;
    s->next->prev = s->prev;
  }
  s->value = 0;
  s->nmemb = 0;
  s->prev = NULL;
  s->next = c->s.g;
  c->s.g = s;
  ++c->s.size;
  return (void *) s;
}

//* Fonction qui récupère un élément du stockage dans la structure cptr.
//* @param c Un pointeur vers la structure cptr.
//* @return Un pointeur vers l'élément récupéré ou NULL si le stockage est vide.
static void *get_element_stockage(cptr *c) {
  if (stockage_is_empty(c)) {
    return NULL;
  }
  c->s.size--;
  void *t = c->s.g;
  c->s.g = c->s.g->next;
  return t;
}

//* Fonction qui crée une cellule vide avec le numéro de fichier spécifié.
//* @param n Le numéro de fichier associé à la cellule.
//* @return Un pointeur vers la cellule créée ou NULL en cas
//d'échec d'allocation mémoire.

static cell *cell_empty(int n) {
  cell *t = malloc(sizeof(*t));
  if (t == NULL) {
    return NULL;
  }
  t->file = n;
  t->value = 1;
  t->nmemb = 1;
  t->next = NULL;
  t->prev = NULL;
  return t;
}

void *ajout(cptr *c, int n) {
  if (c == NULL || n >= c->nb_file) {
    return NULL;
  }
  if (c->head[n] == NULL || c->head[n]->value != 1) {
    cell *t;
    if (stockage_is_empty(c)) {
      t = cell_empty(n);
      if (t == NULL) {
        return NULL;
      }
    } else {
      t = get_element_stockage(c);
      t->file = n;
    }
    t->next = c->head[n];
    c->head[n] = t;
    t->value = 1;
    t->nmemb = 1;
    return (void *) t;
  }
  c->head[n]->nmemb++;
  return (void *) c->head[n];
}

//* Vérifie si un compteur est unique, c'est-à-dire s'il ne contient
//qu'une seule occurrence.
//* @param count Un pointeur vers la structure cell représentant le compteur.
//* @return true si le compteur est unique, sinon false.

static bool is_unique(cell *count) {
  return count->nmemb == 1;
}

//La fonction aux_update effectue une mise à jour auxiliaire d'un compteur
//unique. Elle prend en paramètres un pointeur vers la structure cptr et un
//pointeur vers la structure cell représentant le compteur à mettre à jour.
//Elle renvoie un pointeur vers la structure cell mise à jour ou NULL en cas
//d'erreur.

static void *aux_update(cptr *c, cell *count) {
  long int p = get_count(count) + 1;
  cell *t = count->next;
  if (t == NULL || p < t->value) {
    count->value += 1;
    return (void *) count;
  }
  if (stockage(c, count) == NULL) {
    return NULL;
  }
  t->nmemb++;
  return t;
}

void *update(cptr *c, cell *count, int n) {
  if (c == NULL || count == NULL || n >= c->nb_file) {
    return NULL;
  }
  if (is_unique(count)) {
    return aux_update(c, count);
  }
  count->nmemb--;
  long int value = count->value + 1;
  cell *p = count->next;
  if (p == NULL || value < p->value) {
    cell *t;
    if (stockage_is_empty(c)) {
      t = cell_empty(n);
      if (t == NULL) {
        return NULL;
      }
    } else {
      t = get_element_stockage(c);
      t->file = n;
    }
    t->nmemb = 1;
    t->value = value;
    t->prev = count;
    cell *temp = count->next;
    count->next = t;
    t->next = temp;
    if (temp != NULL) {
      temp->prev = t;
    }
    return t;
  }
  p->nmemb++;
  return (void *) p;
}

long int get_count(const cell *t) {
  return t->value;
}

int get_numfile(const cell *t) {
  return t->file;
}

void dispose_cptr(cptr **c) {
  if (*c == NULL) {
    return;
  }
  cell *p = (*c)->s.g;
  while (p != NULL) {
    cell *t = p;
    p = p->next;
    free(t);
  }
  size_t i = 0;
  while (i < (size_t) (*c)->nb_file) {
    cell *p = (*c)->head[i];
    while (p != NULL) {
      cell *t = p;
      p = t->next;
      free(t);
    }
    i++;
  }
  free((*c)->head);
  free(*c);
  *c = NULL;
}

void *reset_count(cptr *c, cell *count) {
  if (c == NULL || count == NULL) {
    return NULL;
  }
  if (is_unique(count)) {
    if (stockage(c, count) == NULL) {
      return NULL;
    }
  }
  count->nmemb--;
  return (void *) (&c->zero);
}

void display(const cptr *c) {
  if (c == NULL) {
    return;
  }
  int n = 0;
  size_t i = 0;
  while (n < c->nb_file) {
    cell *p = c->head[n];
    while (p != NULL) {
      //printf(" member= %zu..value = %ld \n", p->nmemb, p->value);
      p = p->next;
      i++;
    }
    n++;
  }
  printf(" total %zu compteurs et   %zu non utilise \n", i, c->s.size);
}
