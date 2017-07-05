#----------------------------------------------------------------
#  makefile (GNU make) for FBL compiler and FBL runtime
#
#  out-of-source build for release
#----------------------------------------------------------------

-include ../src/makefile.gmake

CFLAGS += -s

#----------------------------------------------------------------
# Generate include file with dependencies for NMAKE on MS Windows
#----------------------------------------------------------------

all: ../release.ms/deps.nmake

../release.ms/deps.nmake: $(OBJS)
	awk 'FNR==1{print""}{gsub(/\.o:/,".obj:");print}' $(DEPS) > $@
