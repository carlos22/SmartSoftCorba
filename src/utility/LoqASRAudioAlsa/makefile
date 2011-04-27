TARGET          = LASR.LINUX
TARGETVERSION	= 80
SRCDIR          = ./src/
OBJDIR          = .
LIBDIR          = .

LIBNAME          = LoqASRAudioALSA
LIBVERMAJOR      = 7
LIBVERMINOR      = 4
DLIBRARY         = lib$(LIBNAME).so

MAKEDEPEND	= makedepend -Y
STDINCLDIR	= /usr/include
INCLS           = -I../../includes -I./inc -I$(LASR_INSTALL_PATH)/includes
SLIBS            =
DLIBS            = -rpath ../../bin -L$(LASR_INSTALL_PATH)/bin -lm -L../../$(TARGET)/_lib -lLoqASR -lasound
MKDIR     = mkdir -p
RM	  = -\rm -f
MV	  = -\mv
CP	  = -\cp

CC        = gcc
AR        = ar
RANLIB    = ranlib
LD        = ld
DEFINE    = -Dlinux -D__linux__
COBJ      = -c -fPIC
OPT       = -g 
CFLAGS    =
XLIB      =
SOCKLIB   =
SYNTLIB   =

default:       $(DLIBRARY)

#==================================================================================
clean:
	$(RM) $(OBJDIR)*.o
	$(RM) $(LIBDIR)/$(DLIBRARY).$(LIBVERMAJOR).$(LIBVERMINOR)
	$(RM) $(LIBDIR)/$(DLIBRARY).$(LIBVERMAJOR)
	$(RM) $(LIBDIR)/$(DLIBRARY)
 
#==================================================================================

depend:
	$(MAKEDEPEND) $(SRCS) $(DEFINE) $(INCLS)

######################################################################

CLIENTS = $(LIBDIR)/$(DLIBRARY)

UIL =

SRCS = \
	$(SRCDIR)LoqASRAudioALSA.c 
	
OBJS = \
	$(OBJDIR)LoqASRAudioALSA.o 

$(OBJDIR)%.o: $(SRCDIR)%.cpp
	$(MKDIR) $(OBJDIR)
	$(CC) $(COBJ) $(OPT) $(ARCH) $(DEFINE) $(INCLS) $< -o $@

$(OBJDIR)%.o: $(SRCDIR)%.c
	$(MKDIR) $(OBJDIR)
	$(CC) $(COBJ) $(OPT) $(ARCH) $(DEFINE) $(INCLS) $< -o $@

# Process link rules

$(DLIBRARY):  $(OBJS)
	$(MKDIR) $(LIBDIR)
	$(RM) $(LIBDIR)/$(DLIBRARY)
	$(RM) $(LIBDIR)/$(DLIBRARY).$(LIBVERMAJOR)
	$(RM) $(LIBDIR)/$(DLIBRARY).$(LIBVERMAJOR).$(LIBVERMINOR)
	$(LD) $(OPT) -shared -soname lib$(LIBNAME).so.$(LIBVERMAJOR) -o $(LIBDIR)/$(DLIBRARY).$(LIBVERMAJOR).$(LIBVERMINOR) $(OBJS) $(SLIBS) $(DLIBS)
	ln -sf $(DLIBRARY).$(LIBVERMAJOR).$(LIBVERMINOR)  $(LIBDIR)/$(DLIBRARY).$(LIBVERMAJOR)
	ln -sf $(DLIBRARY).$(LIBVERMAJOR).$(LIBVERMINOR)  $(LIBDIR)/$(DLIBRARY)

