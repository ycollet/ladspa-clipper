PLUG = clipper
INSTALL_PATH ?= /usr/lib64/ladspa/

all: $(PLUG).so

run: all

$(PLUG).so: $(PLUG).o
	gcc -shared -o $@ $<

.c.s:
	gcc $(CFLAGS) -S $<

.c.o:
	gcc $(CFLAGS) -c $<

clean:
	rm -f *.o *.so

install: all
	install -m 644 $(PLUG).so $(DESTDIR)$(INSTALL_PATH)

uninstall:
	rm $(DESTDIR)/$(PLUG).so

tarball:
	@ln -s . $(PLUG)
	@tar cvfz $(PLUG).tar.gz $(PLUG)/Makefile $(PLUG)/$(PLUG).c
	@rm $(PLUG)
