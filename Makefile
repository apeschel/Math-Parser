package = parser
version = 0.1
tarname = $(package)
distdir = $(tarname)-$(version)

all clean parser:
	cd src && $(MAKE) $@

dist: $(distdir).tar.gz

$(distdir).tar.gz: $(distdir)
	tar chof - $(distdir) | gzip -9 -c > $@
	rm -rf $(distdir)

$(distdir): FORCE
	mkdir -p $(distdir)/src
	cp Makefile $(distdir)/src
	cp src/Makefile $(distdir)/src
	cp src/funcs.c src/funcs.h $(distdir)/src
	cp src/token.l $(distdir)/src
	cp src/parse.y $(distdir)/src

FORCE:
	-rm $(distdir).tar.gz >/dev/null 2>&1
	-rm -rf $(distdir) >/dev/null 2>&1

.PHONY: FORCE all clean dist
