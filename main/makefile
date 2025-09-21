hashtable_dir = ../hashtable/
holdall_dir = ../holdall/
input_dir = ../input/
gestion_dir = ../gestion/
option_dir = ../option/
bst_dir = ../bst/
CC = gcc
CFLAGS = -std=c2x \
  -Wall -Wconversion -Werror -Wextra -Wpedantic -Wwrite-strings \
  -O2 \
   -I$(hashtable_dir) -I$(holdall_dir) -I$(input_dir) -I$(gestion_dir) \
   -I$(option_dir) -I$(bst_dir)
vpath %.c $(hashtable_dir) $(holdall_dir) $(input_dir) $(gestion_dir)  \
$(option_dir) $(bst_dir)
vpath %.h $(hashtable_dir) $(holdall_dir) $(input_dir) $(gestion_dir) \
$(option_dir) $(bst_dir)
objects = main.o hashtable.o holdall.o input.o gestion.o option.o bst.o
executable = xwc
makefile_indicator = .\#makefile\#

.PHONY: all clean

all: $(executable)

clean:
	$(RM) $(objects) $(executable)
	@$(RM) $(makefile_indicator)

$(executable): $(objects)
	$(CC) $(objects) -o $(executable)

main.o: main.c hashtable.h holdall.h gestion.h option.h bst.h
hashtable.o: hashtable.c hashtable.h
holdall.o: holdall.c holdall.h
input.o: input.c input.h
gestion.o: gestion.c gestion.h
option.o: option.c option.h
bst.o: bst.c bst.h

include $(makefile_indicator)

$(makefile_indicator): makefile
	@touch $@
	@$(RM) $(objects) $(executable)
