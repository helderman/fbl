#----------------------------------------------------------------
#  makefile (GNU make) for FBL compiler and FBL runtime
#
#  build + install
#  specify variable 'prefix' to (un)install at desired location
#----------------------------------------------------------------

.PHONY: install uninstall clean FORCE

#----------------------------------------------------------------
# install, uninstall
#----------------------------------------------------------------

prefix = .
#prefix = /usr/local

exec_prefix = $(prefix)
bindir = $(exec_prefix)/bin
libdir = $(exec_prefix)/lib
includedir = $(prefix)/include
datarootdir = $(prefix)/share
datadir = $(datarootdir)

TARGETS = $(bindir)/fbl $(bindir)/fblc $(libdir)/fblrt.a $(libdir)/fblrt.debug.a $(includedir)/fblrt.h $(datadir)/fblrt.fbl

install: $(TARGETS)

uninstall:
	$(RM) $(TARGETS)

$(bindir)/fbl: src/fbl
	cp -p $< $@

$(bindir)/fblc: release/fblc
	cp -p $< $@

$(libdir)/fblrt.a: release/fblrt.a
	cp -p $< $@

$(libdir)/fblrt.debug.a: debug/fblrt.a
	cp -p $< $@

$(includedir)/fblrt.h: src/ast.h src/prim.h src/fblrt.h
	awk 'BEGIN{print"/*** FBL runtime ***/"}FNR==1{print""}1' $^ > $@

$(datadir)/fblrt.fbl: src/fblrt.fbl
	cp -p $< $@

###  recursively call make to build targets

release/fblc release/fblrt.a: FORCE
	$(MAKE) -C release all

debug/fblrt.a: FORCE
	$(MAKE) -C debug all

#----------------------------------------------------------------
#  clean
#----------------------------------------------------------------

clean:
	$(MAKE) -C release clean
	$(MAKE) -C debug clean
