.PHONY: clean dist

dist: clean
	tar -hzcf "$(CURDIR).tar.gz" hashtable/* main/* holdall/* input/* gestion/* \
	bst/* option/* makefile

clean:
	$(MAKE) -C main clean
