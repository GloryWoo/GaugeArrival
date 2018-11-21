# This is an automatically generated record.
# The area between QNX Internal Start and QNX Internal End is controlled by
# the QNX IDE properties.

ifndef QCONFIG
QCONFIG=qconfig.mk
endif
include $(QCONFIG)

#===== USEFILE - the file containing the usage message for the application. 
USEFILE=

# Next lines are for C++ projects only

EXTRA_SUFFIXES+=cxx cpp

#===== LDFLAGS - add the flags to the linker command line.
LDFLAGS+=-lang-c++ -ljsoncpp -lPocoFoundation -lz -lcurl  \
	-pthread -lsvcipc

VFLAG_g=-gstabs+

#===== CCFLAGS - add the flags to the C compiler command line. 
CCFLAGS+=-DDEBUG_TAG

include $(MKFILES_ROOT)/qmacros.mk
ifndef QNX_INTERNAL
QNX_INTERNAL=$(PROJECT_ROOT)/.qnx_internal.mk
endif
include $(QNX_INTERNAL)

include $(MKFILES_ROOT)/qtargets.mk

#===== EXTRA_INCVPATH - a space-separated list of directories to search for include files.
EXTRA_INCVPATH+=D:/QNX/workspace/GaugeArrival/include

#===== EXTRA_LIBVPATH - a space-separated list of directories to search for library files.
EXTRA_LIBVPATH+=D:/QNX/workspace/GaugeArrival/lib

OPTIMIZE_TYPE_g=none
OPTIMIZE_TYPE=$(OPTIMIZE_TYPE_$(filter g, $(VARIANTS)))

