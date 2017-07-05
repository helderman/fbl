#----------------------------------------------------------------
#  makefile (GNU make) for FBL compiler and FBL runtime
#
#  out-of-source build for debug
#----------------------------------------------------------------

-include ../src/makefile.gmake

CFLAGS += -g -DAST_NAMES -DTRACE

#----------------------------------------------------------------
# Generate include file with dependencies for NMAKE on MS Windows
#----------------------------------------------------------------

all: ../debug.ms/deps.nmake

../debug.ms/deps.nmake: $(OBJS)
	awk 'FNR==1{print""}{gsub(/\.o:/,".obj:");print}' $(DEPS) > $@
