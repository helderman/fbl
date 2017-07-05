#----------------------------------------------------------------
#  makefile (NMAKE) for FBL compiler and FBL runtime
#
#  build + install
#  specify variable 'prefix' to (un)install at desired location
#----------------------------------------------------------------

RM = del

#----------------------------------------------------------------
# install, uninstall
#----------------------------------------------------------------

prefix = .

exec_prefix = $(prefix)
bindir = $(exec_prefix)\bin
libdir = $(exec_prefix)\lib
includedir = $(prefix)\include
datarootdir = $(prefix)\share
datadir = $(datarootdir)

TARGETS = $(bindir)\fbl.bat $(bindir)\fblc.exe $(libdir)\fblrt.lib

install: $(TARGETS)

uninstall:
	$(RM) $(TARGETS)

$(bindir)\fbl.bat: src\fbl.bat
	copy $** $@

$(bindir)\fblc.exe: release.ms\fblc.exe
	copy $** $@

$(libdir)\fblrt.lib: release.ms\fblrt.lib
	copy $** $@
