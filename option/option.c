#include  "option.h"

option *option_emty(size_t n) {
  option *o = malloc(sizeof(*o));
  if (o == NULL || (o->tab_option = malloc(sizeof(bool) * n)) == NULL) {
    return NULL;
  }
  for (size_t i = 0; i < n; i++) {
    o->tab_option[i] = false;
  }
  o->tab_option[OPT_HASHTABLE] = true;
  o->tab_option[OPT_S_] = true;
  o->n = n;
  o->option_i = 0;
  return o;
}

//* Compare une chaîne de caractères avec un tableau de chaînes de caractères
//* pour trouver une correspondance.
//*
//* @param option_ Un tableau de chaînes de caractères à comparer.
//* @param nmemb Le nombre d'éléments dans le tableau.
//* @param arg La chaîne de caractères à rechercher.
//* @return L'index de la chaîne correspondante dans le tableau, ou FAIL si
//* aucune correspondance n'est trouvée.
static int compar_option(const char *option_[], size_t nmemb, char *arg) {
  for (size_t i = 0; i < nmemb; i++) {
    if (strcmp(option_[i], arg) == 0) {
      return (int) i;
    }
  }
  return FAIL;
}

//La fonction get_name_file vérifie si l'argument actuel (argv[*curr]) est égal
//à  chaîne (SIGN). Si tel est le cas et qu'il y a un argument
//suivant dans la ligne de commande, elle enregistre cet argument suivant dans
//le tableau tab_file[], incrémente le nombre de fichiers (*nb_file) et renvoie
//0 pour indiquer un succès. Sinon, elle renvoie -1 pour indiquer une erreur.

static int get_name_file(char *tab_file[], int *nb_file, char **argv, int argc,
    int *curr) {
  if (strcmp(argv[*curr], SIGN) == 0) {
    if (*curr + 1 < argc) {
      tab_file[*nb_file] = argv[++(*curr)];
      *nb_file += 1;
      return 0;
    }
  }
  return -1;
}

#define BASE 10
#define ERROR_VALUE(argv_curr) fprintf(stderr, "ERROR_VALUE '%s'\n", \
    (argv_curr))

//reserve pour option i;
//Extrait et convertit une valeur entière à partir d'une chaîne de caractères
//spécifiée dans la ligne de commande.
//*
//* @param argv Le tableau des arguments de la ligne de commande.
//* @param curr L'index de la chaîne de caractères à convertir dans le tableau
//argv.
//* @param argc Le nombre total d'arguments de la ligne de commande.
//* @return La valeur entière extraite si la conversion réussit, FAIL sinon.

static long int get_value_i(char **argv, int curr, int argc) {
  if (curr < argc) {
    errno = 0;
    char *x;
    long int n = strtol(argv[curr], &x, BASE);
    if (*x != '\0' || errno != 0) {
      ERROR_VALUE(argv[curr]);
      return FAIL;
    }
    return n;
  }
  return FAIL;
}

//reserve option w

//Cette fonction get_value_w vérifie si l'argument actuel (argv[curr])
//correspond à une des deux options possibles pour le traitement des mots
//(H ou AVL). Si l'argument correspond à l'option H, elle désactive l'option
//OPT_AVL dans la structure option et renvoie OPT_HASHTABLE. Si l'argument
//correspond à l'option AVL, elle désactive l'option OPT_HASHTABLE et
//renvoie OPT_AVL. Si aucun des deux cas n'est vérifié, un message d'erreur est
//affiché et la fonction renvoie FAIL.

static int get_value_w(char **argv, int curr, int argc, option *o) {
  if (curr < argc) {
    const char *p = H;
    const char *q = AVL;
    p++;
    q++;
    if (strcmp(p, argv[curr]) == 0) {
      o->tab_option[OPT_AVL] = false;
      return OPT_HASHTABLE;
    } else if (strcmp(q, argv[curr]) == 0) {
      o->tab_option[OPT_HASHTABLE] = false;
      return OPT_AVL;
    }
    printf("= %s p = %s q= %s\n", argv[curr], p, q);
    TRACK ERROR_VALUE(argv[curr]);
  }
  return FAIL;
}

//* Vérifie si l'argument actuel correspond à l'une des options possibles
//pour le tri des résultats.
//*
//* @param argv Le tableau des arguments de la ligne de commande.
//* @param curr L'index de l'argument actuel dans le tableau argv.
//* @param argc Le nombre total d'arguments de la ligne de commande.
//* @param o Un pointeur vers la structure option contenant les options.
//* @return OPT_LEXICOGRAPHIE si l'option L est spécifiée, OPT_NUMERIC si
//l'option N est spécifiée, OPT_S_ si l'option S_ est spécifiée, FAIL sinon.

static int get_value_s(char **argv, int curr, int argc, option *o) {
  if (curr < argc) {
    const char *a = L + 1;
    const char *b = N + 1;
    const char *c = S_ + 1;
    if (strcmp(a, argv[curr]) == 0) {
      o->tab_option[OPT_NUMERIC] = false;
      o->tab_option[OPT_S_] = false;
      o->tab_option[OPT_LEXICOGRAPHIE] = true;
      return OPT_LEXICOGRAPHIE;
    } else if (strcmp(b, argv[curr]) == 0) {
      o->tab_option[OPT_LEXICOGRAPHIE] = false;
      o->tab_option[OPT_S_] = false;
      o->tab_option[OPT_NUMERIC] = true;
      return OPT_NUMERIC;
    } else if (strcmp(c, argv[curr]) == 0) {
      o->tab_option[OPT_NUMERIC] = false;
      o->tab_option[OPT_LEXICOGRAPHIE] = false;
      o->tab_option[OPT_S_] = true;
      return OPT_S_;
    }
    ERROR_VALUE(argv[curr]);
  }
  return FAIL;
}

//* Modifie une chaîne en retirant les espaces et les tirets en début de chaîne.
//*
//* @param s La chaîne à modifier.
//* @param file Un pointeur vers la chaîne de caractères modifiée.
//* @return 0 en cas de succès, FAIL si la chaîne résultante est vide ou égale
//à FILE_.
//*/

static int modifierChaine(char *s, char **file) {
  size_t longueur = strlen(s);
  while (longueur > 0 && (*file)[0] == '-') {
    (*file)++;
    longueur--;
  }
  // Supprimer les espaces restants, le cas échéant
  while (longueur > 0 && (*file)[0] == ' ') {
    (*file)++;
    longueur--;
  }
  // Vérifier si la chaîne est vide après avoir supprimé tous les guillemets et
  // espaces
  if (longueur == 0 || strcmp(*file, FILE_) == 0) {
    return FAIL;
  }
  return 0;
}

//reserve pour option R
//* Extrait et valide une valeur associée à une option de traitement de fichier.
//*
//* @param argv Le tableau des arguments de la ligne de commande.
//* @param curr L'index de l'argument actuel dans le tableau argv.
//* @param argc Le nombre total d'arguments de la ligne de commande.
//* @param file Un pointeur vers la chaîne de caractères représentant le
//* nom du fichier.
//* @return 0 en cas de succès, FAIL en cas d'erreur ou SIGN_STDIN si l'entrée
//*  standard est spécifiée.

static int get_value_r(char **argv, int curr, int argc, char **file) {
  if (curr < argc) {
    char *s = argv[curr];
    if (strcmp(s, FILE_) == 0) {
      return SIGN_STDIN;
    }
    *file = s;
    if (modifierChaine(s, file) != 0) {
      ERROR_VALUE(argv[curr]);
      return FAIL;
    }
    return 0;
  } else {
    *file = argv[curr];
    return 0;
  }
  return FAIL;
}

#define HEAD 1

//* Échange les éléments du premier et du dernier indice d'un tableau de
//* pointeurs vers des chaînes de caractères.
//*
//* @param tableau Le tableau de pointeurs vers des chaînes de caractères.
//* @param taille La taille du tableau.

void echangePremierDernier(char **tableau, int taille) {
  if (taille <= HEAD) {
    return;
  }
  // Échange des pointeurs
  char *temp = tableau[HEAD];
  tableau[HEAD] = tableau[taille - 1];
  tableau[taille - 1] = temp;
}

option *custom_getopt(int argc, char *argv[], char *tab_file[], int *nb_file) {
  const char *options_short[] = {
    R_, AVL, H, L, N, R, I, P, S_, W, S, C
  };
  const char *options_long[] = {
    RESTRICTED, AVL_BINARY_TREE, HASTABLE, LEXICOGRAPHICAL, NUMERIC, RENVERSE,
    INITIAL, PONCTUATION_LIKE_SPACE, NONE_SORT, WORD_PROCESSING, SORT, COUNTER
  };
  if (strcmp(argv[*nb_file], HELP) == 0) {
    printf(
        "%s: xwc [OPTION]... [FILE]... ou l'inverse, l'ordre n'est pas\n"
        " important. On peut tout mélanger entre les deux.\n"
        "- S'il y a plusieurs options de tri,  il vaprendre "
        "\033[1;31m"
        "la dernière option \n"
        "\033[0m"
        "  en ligne de commande, donc la même chose pour toutes les options.\n"
        "  Pour les options avec valeur: \n"
        "\033[1;31m"
        "  il faut avoir un seul et seulement un seul espace \n"
        "\033[0m"
        "  entre la valeur et l'option.\n\n"
        "      %s   Afficher ce message d'aide et quitter.\n"
        "      %s   Afficher un bref message d'utilisation et quitter.\n"
        "      %s    Afficher les informations sur la version.\n\n"
        "\033[1;31m"
        "Traitement\n"
        "\033[0m"
        "  %s Identique à --traitement-mots=%s.\n"
        "  %s      Identique à --traitement-mots= %s.\n"
        "  %s, --traitement-mots= TYPE    Traiter les mots en fonction de la\n"
        "             structure de données spécifiée par TYPE. Les valeurs\n"
        "             disponibles pour TYPE sont explicites : %s\n"
        "             et %s. Par défaut, c'est %s.\n"
        "\033[1;32m"
        "              donc le valeur possoble est: %s, %s. \n\n"
        "\033[1;31m"
        "Contrôle de l'entrée\n"
        "\033[0m"
        "%s, %s VALEUR    Définir le nombre maximal de lettres initiales\n"
        "             significatives pour les mots à VALEUR. 0 signifie sans\n"
        "             limitation. Par défaut, c'est 0.\n"
        "  %s, %s    Faire en sorte que les caractères de\n"
        "            ponctuation jouent le même rôle que les caractères\n"
        "            d'espace blanc dans la signification des mots.\n"
        "  %s, %s FILE    Limiter le comptage à l'ensemble des mots\n"
        "             qui apparaissent dans FICHIER. FICHIER est affiché dans\n"
        "             la première colonne de la ligne d'en-tête. Si FICHIER\n"
        "             est \"-\", lire les mots depuis l'entrée standard ; dans\n"
        "             ce cas, \"\" est affiché dans la première colonne de la\n"
        "             ligne d'en-tête.\n\n"
        "\033[1;31m"
        "Contrôle de la sortie\n"
        "\033[0m"
        "  %s           Identique à %s = lexicographique.\n"
        "  %s            Identique à %s=numérique.\n"
        "  %s            Identique à %s.\n"
        "  %s,  %s  TYPE  Trier les résultats par défaut, de manière\n"
        "             ascendante, selon TYPE. Les valeurs disponibles pour\n"
        "             TYPE sont : 'lexicographique', tri sur les mots,\n"
        "             'numérique', tri sur le nombre d'occurrences, première\n"
        "             clé et mots, deuxième clé, et 'aucun', ne pas essayer\n"
        "             de trier, prendre tel quel. Par défaut, c'est 'aucun'.\n"
        "\033[1;32m"
        "              donc valeur possible sont %s %s %s . \n"
        "\033[0m"
        " %s, %s    Trier par ordre décroissant sur la clé unique ou la\n"
        "             première clé au lieu de l'ordre croissant. Cette option\n"
        "             n'a pas d'effet si l'option %s est activée.\n"
        " %s, %s      pour voir le total compteur utilisé             \n ",
        USAGE, HELP, USAGE, VERSION,
        options_short[OPT_AVL], options_long[OPT_AVL],
        options_short[OPT_HASHTABLE], options_long[OPT_HASHTABLE],
        options_short[OPT_W], options_long[OPT_AVL],
        options_long[OPT_HASHTABLE], options_long[OPT_HASHTABLE],
        (AVL + 1), (H + 1),
        options_short[OPT_INITIAL], options_long[OPT_INITIAL],
        options_short[OPT_PUNC], options_long[OPT_PUNC],
        options_short[OPT_RESTRICTED], options_long[OPT_RESTRICTED],
        options_short[OPT_LEXICOGRAPHIE], options_long[OPT_SORT],
        options_short[OPT_NUMERIC], options_long[OPT_SORT],
        options_short[OPT_S_], NONE_SORT, options_short[OPT_SORT],
        options_long[OPT_SORT],
        (L + 1), (N + 1), (S_ + 1),
        options_short[OPT_RENVERSE], options_long[OPT_RENVERSE],
        options_short[OPT_S_], options_short[OPT_C],
        options_long[OPT_C]);
    *nb_file = SUCCESS;
    return NULL;
  }
  if (strcmp(argv[*nb_file], USAGE) == 0) {
    printf("%s: xwc [OPTION]... [FILE]... ou l'inverse, l'ordre n'est pas\n"
        "important. On peut tout mélanger entre les deux.\n"
        "S'il y a plusieurs options de tri, il va prendre la dernière option en\n"
        "ligne de commande, donc la même chose pour toutes les options.\n"
        "Pour les options avec valeur, il faut avoir un seul et seulement \n"
        "un seul espace entre la valeur et l'option.\n",
        USAGE);
    *nb_file = SUCCESS;
    return NULL;
  }
  if (strcmp(argv[*nb_file], VERSION) == 0) {
    VERSION_();
    *nb_file = SUCCESS;
    return NULL;
  }
  size_t nmemb = sizeof(options_short) / sizeof(options_short[0]);
  option *o = option_emty(nmemb);
  if (o == NULL) {
    return NULL;
  }
  int i = 1;
  while (i < argc) {
    if (argv[i][SHORT] != DASH
        || (argv[i][SHORT] == DASH && strlen(argv[i]) == 1)) {
      tab_file[*nb_file] = argv[i];
      *nb_file += 1;
    } else {
      int resultat;
      if (argv[i][LONG] == DASH) {
        resultat = compar_option(options_long, nmemb, argv[i]);
      } else {
        resultat = compar_option(options_short, nmemb, argv[i]);
      }
      if (resultat == FAIL) {
        if (get_name_file(tab_file, nb_file, argv, argc, &i) != 0) {
          goto error;
        }
      } else {
        switch (resultat) {
          case OPT_RESTRICTED:
            char *file;
            int n = get_value_r(argv, i + 1, argc, &file);
            char *file_;
            if (n == FAIL) {
              goto error;
            } else {
              file_ = (n == SIGN_STDIN) ? (char *) FILE_ : file;
            }
            if (get_value_option(o, OPT_RESTRICTED)) {
              tab_file[0] = file_;
            } else {
              tab_file[*nb_file] = file_;
              (*nb_file)++;
              echangePremierDernier(tab_file, *nb_file);
            }
            i++;
            break;
          case OPT_SORT:
            n = get_value_s(argv, (i + 1), argc, o);
            if (n == FAIL) {
              goto error;
            }
            i++;
            resultat = n;
            break;
          case OPT_W:
            n = get_value_w(argv, i + 1, argc, o);
            if (n == FAIL) {
              goto error;
            }
            i++;
            resultat = n;
            break;
          case OPT_INITIAL:
            o->option_i = get_value_i(argv, i + 1, argc);
            if (o->option_i == FAIL) {
              goto error;
            }
            i++;
            break;
          case OPT_LEXICOGRAPHIE:
            o->tab_option[OPT_NUMERIC] = false;
            o->tab_option[OPT_S_] = false;
            break;
          case OPT_NUMERIC:
            o->tab_option[OPT_LEXICOGRAPHIE] = false;
            o->tab_option[OPT_S_] = false;
            break;
          case OPT_S_:
            o->tab_option[OPT_LEXICOGRAPHIE] = false;
            o->tab_option[OPT_NUMERIC] = false;
            break;
          default:
            break;
        }
        o->tab_option[resultat] = true;
      }
    }
    i++;
  }
  goto fin;
error:
  fprintf(stderr, "Invalid option format '%s'\n", argv[i]);
  *nb_file = 0;
  dispose_opt(&o);
  return NULL;
fin:
  return o;
}

bool get_value_option(const option *c, size_t n) {
  if (n >= c->n || !c->tab_option[n]) {
    return false;
  }
  return true;
}

void dispose_opt(option **c) {
  free((*c)->tab_option);
  free(*c);
  *c = NULL;
}

bool get_opt_p(const option *c, int n) {
  if (!get_value_option(c, OPT_PUNC)) {
    return true;
  }
  return ispunct(n) == 0;
}
