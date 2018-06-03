.SUFFIXES: .in

name = MonetDB-gsl
version = `sed -n 's/^Version:[ \t]*\(.*\)/\1/p' MonetDB-gsl.spec`

LIBDIR = $(shell pkg-config --variable=libdir monetdb5)

CC       = cc
M4       = m4
M4FLAGS  =
M4SCRIPT =
DBFARM   = ~/work/mydbfarm

CFLAGS += $(shell pkg-config --cflags monetdb5)
CFLAGS += $(shell pkg-config --cflags gsl)
LDFLAGS += $(shell pkg-config --libs monetdb5)
LDFLAGS += $(shell pkg-config --libs gsl)

all: lib_gsl.so

.in:
	${M4} ${M4FLAGS} ${M4SCRIPT} $< > $*

lib_gsl.so: gsl.o gsl.mal 73_gsl.sql
	$(CC) -fPIC -DPIC -o lib_gsl.so -shared gsl.o $(LDFLAGS) -Wl,-soname -Wl,lib_gsl.so

gsl.o: gsl.c
	$(CC) -fPIC -DPIC $(CFLAGS) -c gsl.c

clean:
	rm -f *.o *.so gsl.mal 73_gsl.sql

install: lib_gsl.so gsl.mal 73_gsl.sql
	mkdir -p $(DESTDIR)$(LIBDIR)/monetdb5/autoload $(DESTDIR)$(LIBDIR)/monetdb5/createdb
	cp gsl.mal lib_gsl.so $(DESTDIR)$(LIBDIR)/monetdb5
	cp 73_gsl.sql $(DESTDIR)$(LIBDIR)/monetdb5/createdb
	cp 73_gsl.mal $(DESTDIR)$(LIBDIR)/monetdb5/autoload

remove:
	rm $(DESTDIR)$(LIBDIR)/monetdb5/gsl.mal || true
	rm $(DESTDIR)$(LIBDIR)/monetdb5/lib_gsl.so || true
	rm $(DESTDIR)$(LIBDIR)/monetdb5/createdb/73_gsl.sql || true
	rm $(DESTDIR)$(LIBDIR)/monetdb5/autoload/73_gsl.mal || true

redo: clean lib_gsl.so install test

dist:
	tar -c -j -f $(name)-$(version).tar.bz2 --transform "s,^,$(name)-$(version)/," `hg files -X .hgtags`

sql:
	monetdbd start $(DBFARM) || true
	mclient -d testt

mal:
	monetdbd start $(DBFARM) || true
	mclient -l mal testt

start:
	monetdbd start $(DBFARM) || true

stop:
	monetdbd stop $(DBFARM) || true

test:
	monetdbd start $(DBFARM) || true
	monetdb destroy -f testt || true
	monetdb create testt
	monetdb release testt
	# monetdb set embedr=yes testt
	# monetdb set embedc=yes testt
	mclient -d testt -s "select gsl_cdf_chisq_p(20.0,5.0); select gsl_cdf_gamma_p(0.1,2.0,0.1); select poisson_ci(10,1), poisson_ci(10,2);" || echo Warning: check that MonetDB-rmath is not installed.

