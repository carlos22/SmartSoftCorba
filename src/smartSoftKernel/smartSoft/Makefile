# --------------------------------------------------------------------------
#
#  Copyright (C) 2002/2004 Christian Schlegel
#
#        schlegel@hs-ulm.de
#
#        Prof. Dr. Christian Schlegel
#        University of Applied Sciences
#        Prittwitzstr. 10
#        D-89075 Ulm
#        Germany
#
#  This file is part of the "SmartSoft Communication Library".
#  It provides standardized patterns for communication between
#  different components.
#
#  This library is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License, or (at your option) any later version.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#
#
#  (partly based on joint work with Robert Wörz)
#
#----------------------------------------------------------------------------

#----------------------------------------------------------------------------
#
#----------------------------------------------------------------------------
CONF  = ../../Makefile.smart.conf
RULES = ../../Makefile.smart.rules

include $(CONF)

#----------------------------------------------------------------------------
#       Include macros and targets
#----------------------------------------------------------------------------

DEPENDENCIES += *.cc
INCLUDES     += -I$(TAO_ROOT)/orbsvcs

#----------------------------------------------------------------------------
# Variables
#
# USER: IDL_FILES
#       EXPORT_FILES
#       OBJ_FILES
#       DISTCLEAN_FILES
#
#       SERVER_OBJS
#       SERVER_LIBS
#       CLIENT_OBJS
#       CLIENT_LIBS
#
# goal build:
# local rules
#----------------------------------------------------------------------------
IDL_FILES    = smartQueryPattern \
               smartSendPattern \
               smartPushNewestPattern \
               smartPushTimedPattern \
               smartEventPattern \
               smartStatePattern \
               smartCommWiring
EXPORT_FILES = smartQueryClientPattern.hh \
               smartQueryServerPattern.hh \
               smartQuery.hh \
               smartQuery.th \
               smartSendClientPattern.hh \
               smartSendServerPattern.hh \
               smartSend.hh \
               smartSend.th \
               smartPushNewestClientPattern.hh \
               smartPushNewestServerPattern.hh \
               smartPushNewest.hh \
               smartPushNewest.th \
               smartPushTimedInternal.hh \
               smartPushTimedClientPattern.hh \
               smartPushTimedServerPattern.hh \
               smartPushTimed.hh \
               smartPushTimed.th \
               smartEventClientPattern.hh \
               smartEventServerPattern.hh \
               smartEvent.hh \
               smartEvent.th \
               smartStateClientPattern.hh \
               smartStateServerPattern.hh \
               smartState.hh \
               smartErrno.hh \
               smartSoft.hh \
               smartSys.hh \
               smartSys.th \
               smartTask.hh \
               smartComponent.hh \
               smartParameter.hh \
	       smartMessageQueue.hh \
	       smartMessageQueue.th \
	       smartProcessingPatterns.hh \
               smartTimerThread.hh \
               smartCommWiring.hh \
               smartWiring.hh
OBJ_FILES    = smartTask.o \
               smartComponent.o \
               smartParameter.o \
               smartQueryServerPattern.o \
               smartQueryClientPattern.o \
               smartSendServerPattern.o \
               smartSendClientPattern.o \
               smartPushNewestServerPattern.o \
               smartPushNewestClientPattern.o \
               smartPushTimedInternal.o \
               smartPushTimedServerPattern.o \
               smartPushTimedClientPattern.o \
               smartEventServerPattern.o \
               smartEventClientPattern.o \
               smartStateServerPattern.o \
               smartStateClientPattern.o \
               smartState.o \
               smartSys.o \
               smartTimerThread.o \
               smartErrno.o \
               smartCommWiring.o \
               smartWiring.o 


#----------------------------------------------------------------------------
#
#----------------------------------------------------------------------------
IDL_EXPORT   = $(addsuffix C.i,    $(IDL_FILES)) \
               $(addsuffix C.hh,   $(IDL_FILES)) \
               $(addsuffix S.i,    $(IDL_FILES)) \
               $(addsuffix S.hh,   $(IDL_FILES))

IDL_DEPEND   = $(addsuffix S.cc,   $(IDL_FILES)) \
               $(addsuffix S.hh,   $(IDL_FILES)) \
               $(addsuffix S.i,    $(IDL_FILES)) \
               $(addsuffix C.cc,   $(IDL_FILES)) \
               $(addsuffix C.hh,   $(IDL_FILES)) \
               $(addsuffix C.i,    $(IDL_FILES))

IDL_CLEAN    = $(IDL_DEPEND) \
               $(addprefix $(OBJ_DIR)/, $(addsuffix S.o,   $(IDL_FILES))) \
               $(addprefix $(OBJ_DIR)/, $(addsuffix S_T.o, $(IDL_FILES))) \
               $(addprefix $(OBJ_DIR)/, $(addsuffix C.o,   $(IDL_FILES))) \
               $(addprefix $(INC_DIR)/, $(IDL_EXPORT))

EXPORT_CLEAN = $(addprefix $(INC_DIR)/, $(EXPORT_FILES))

CLEAN_FILES = $(IDL_CLEAN) \
              $(EXPORT_CLEAN) \
              $(addprefix $(OBJ_DIR)/, $(OBJ_FILES))

TAO_IDLFLAGS += -cs C.cc -hc C.hh -ss S.cc -hs S.hh -sT S_T.cc -hT S_T.hh -ci C.i -si S.i

##
## local build variables
##

INCLUDES  += -I$(QTDIR)/include
LIB_DIRS  += -L$(QTDIR)/lib
LFLAGS     = -Wl,-rpath,$(QTDIR)/lib

DISTCLEAN_FILES = $(LIB_DIR)/libsmartSoft.a

SERVER_OBJS = $(OBJ_DIR)/smartTask.o \
              $(OBJ_DIR)/smartComponent.o \
              $(OBJ_DIR)/smartParameter.o \
              $(OBJ_DIR)/smartQueryPatternS.o \
              $(OBJ_DIR)/smartQueryPatternC.o \
              $(OBJ_DIR)/smartQueryServerPattern.o \
              $(OBJ_DIR)/smartQueryClientPattern.o \
              $(OBJ_DIR)/smartSendPatternS.o \
              $(OBJ_DIR)/smartSendPatternC.o \
              $(OBJ_DIR)/smartSendServerPattern.o \
              $(OBJ_DIR)/smartSendClientPattern.o \
              $(OBJ_DIR)/smartPushNewestPatternS.o \
              $(OBJ_DIR)/smartPushNewestPatternC.o \
              $(OBJ_DIR)/smartPushNewestServerPattern.o \
              $(OBJ_DIR)/smartPushNewestClientPattern.o \
              $(OBJ_DIR)/smartPushTimedInternal.o \
              $(OBJ_DIR)/smartPushTimedPatternS.o \
              $(OBJ_DIR)/smartPushTimedPatternC.o \
              $(OBJ_DIR)/smartPushTimedServerPattern.o \
              $(OBJ_DIR)/smartPushTimedClientPattern.o \
              $(OBJ_DIR)/smartEventPatternS.o \
              $(OBJ_DIR)/smartEventPatternC.o \
              $(OBJ_DIR)/smartEventServerPattern.o \
              $(OBJ_DIR)/smartEventClientPattern.o \
              $(OBJ_DIR)/smartStatePatternS.o \
              $(OBJ_DIR)/smartStatePatternC.o \
              $(OBJ_DIR)/smartStateServerPattern.o \
              $(OBJ_DIR)/smartStateClientPattern.o \
              $(OBJ_DIR)/smartState.o \
              $(OBJ_DIR)/smartSys.o \
              $(OBJ_DIR)/smartTimerThread.o \
              $(OBJ_DIR)/smartErrno.o \
              $(OBJ_DIR)/smartCommWiringS.o \
              $(OBJ_DIR)/smartCommWiringC.o \
              $(OBJ_DIR)/smartCommWiring.o \
              $(OBJ_DIR)/smartWiring.o

SERVER_LIBS = -lTAO_CosNaming -lTAO -lTAO_PortableServer -lsmartSoft -lACE -ldl -lpthread $(OS_LIBS) -lm -lrt

CLIENT_OBJS =

CLIENT_LIBS =

##
## Compulsory rules
##

all: $(IDL_DEPEND)
	$(MAKE) export $(DEPEND)
	$(MAKE) build

build: build-libs build-components

build-libs: $(OBJ_DIR) $(LIB_DIR) \
	$(LIB_DIR)/libsmartSoft.a

build-components:

build-tests:

export: $(IDL_EXPORT)
	$(ECHO) "   ---- Installing header files etc."
	$(SILENT) $(MKDIR) $(ETC_DIR)
	$(SILENT) $(MKDIR) $(INC_DIR)
	$(foreach exp,$(IDL_EXPORT),$(INSTALL_H) `pwd`/$(exp) $(INC_DIR);)
	$(foreach exp,$(EXPORT_FILES),$(INSTALL_H) `pwd`/$(exp) $(INC_DIR);)

clean:
	$(RM) $(CLEAN_FILES)

distclean: clean
	$(RM) $(DEPEND) Makefile.depend* $(DISTCLEAN_FILES)

##
## local rules
##

$(OBJ_DIR) $(LIB_DIR) $(BIN_DIR):
	$(SILENT) $(MKDIR) $@

$(LIB_DIR)/libsmartSoft.a: $(SERVER_OBJS)
	$(ECHO) "   ---- Creating $@"
	$(SILENT) $(AR) $@ $(SERVER_OBJS)
	$(SILENT) $(RANLIB) $@


##
## Standard suffix, version, packaging and dep rules
##
include $(RULES)


%S.cc %S.hh %S.i %C.cc %C.cc %C.hh %C.i : %.idl
	$(ECHO) "    ---- Processing $< (IDL)"
	$(IDL) $(TAO_IDLFLAGS) $<

