#--------------------------------------------------------------------------
#
#  Copyright (C) 2003 Boris Kluge
#
#        schlegel@hs-ulm.de
#
#        Prof. Dr. Christian Schlegel
#        University of Applied Sciences
#        Prittwitzstr. 10
#        D-89075 Ulm
#        Germany
#
#  This program is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License, or (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this program; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
#
#--------------------------------------------------------------------------
 
#----------------------------------------------------------------------------
#
#----------------------------------------------------------------------------
CONF  = ../../Makefile.smart.conf
RULES = ../../Makefile.smart.rules

include $(CONF)

# directory for test binaries
TST_DIR = $(PWD)/tests

#----------------------------------------------------------------------------
#       Include macros and targets
#----------------------------------------------------------------------------

DEPENDENCIES += *.cc
INCLUDES     += -I$(TAO_ROOT)/orbsvcs

#----------------------------------------------------------------------------
# Variables
#
# USER: IDL_FILES         IDL files in this directory
#       EXPORT_FILES      files to be exported not including the IDL files
#       INI_FILES         ini files needed by that component
#       OBJ_FILES         generated object files without IDL object files
#       DISTCLEAN_FILES   files to be removed when cleaning up
#
#       SERVER_OBJS       all object files which build the component
#       SERVER_LIBS       libraries needed by the component
#       CLIENT_OBJS       all object files for the client library
#       CLIENT_LIBS       libraries needed by the client library
#
#---------------------------------------------------------------------------- 
IDL_FILES    =  

EXPORT_FILES = 	

INI_FILES    = smartVisualizationMultiClient.ini

##----------------------------------------------------------------------------
## Libraries to build
##----------------------------------------------------------------------------

LIBS =	

# Defaults

LIB_CFLAGS = $(CFLAGS)
LIB_LFLAGS = $(LFLAGS)

# Library 1

LIB_1_CFLAGS =	$(LIB_CFLAGS)
LIB_1_OBJ =	
LIB_1_LFLAGS =	$(LIB_LFLAGS)
LIB_1_BIN =	

##----------------------------------------------------------------------------
## Components to build
##----------------------------------------------------------------------------

CPNT =	1

# Defaults

CPNT_CFLAGS = 	-DQT_THREAD_SUPPORT -I$(QTDIR)/include
CPNT_LFLAGS = 	$(LFLAGS) \
		-lsmartBasicComm -lsmartProperty \
		-lfawGenericTransmittable -lfawVisualization \
		-lTAO_CosNaming -lTAO -lTAO_PortableServer \
		-lsmartSoft -lACE -ldl -lpthread $(OS_LIBS) -lm -lrt -lz

CFLAGS += $(CPNT_CFLAGS)

# Component 1: smartVisualizationMultiClient

CPNT_1_CFLAGS =	$(CPNT_CFLAGS)
CPNT_1_OBJ =	smartVisualizationMultiClient.o \
		smartVisualizationThread.o \
		smartBaseStateVisualizationThread.o \
		smartSonarDataVisualizationThread.o
CPNT_1_LFLAGS =	$(CPNT_LFLAGS)
CPNT_1_BIN =	smartVisualizationMultiClient

##----------------------------------------------------------------------------
## Tests to build
##----------------------------------------------------------------------------

TEST = 

# Defaults

TEST_CFLAGS =	$(CFLAGS)
TEST_LFLAGS =	$(LFLAGS) \
		-lTAO_CosNaming -lTAO -lTAO_PortableServer \
		-lsmartSoft -lACE -ldl -lpthread $(OS_LIBS) -lm -lrt

# Test 1

TEST_1_CFLAGS =	$(TEST_CFLAGS)
TEST_1_OBJ =	
TEST_1_LFLAGS =	$(TEST_LFLAGS)
TEST_1_BIN =	

#---------------------------------------------------------------------------- 
#
#---------------------------------------------------------------------------- 

OBJ_FILES = 	$(foreach lib, $(LIBS), $(LIB_$(lib)_OBJ)) \
		$(foreach cpnt, $(CPNT), $(CPNT_$(cpnt)_OBJ)) \
		$(foreach test, $(TEST), $(TEST_$(test)_OBJ))

LIBS_BIN =	$(patsubst %, lib%.a, $(foreach lib, $(LIBS), $(LIB_$(lib)_BIN)))
CPNT_BIN =	$(foreach cpnt, $(CPNT), $(CPNT_$(cpnt)_BIN))
TEST_BIN =	$(foreach test, $(TEST), $(TEST_$(test)_BIN))

MOCHEADERS = $(shell grep -l Q_OBJECT $(wildcard *.hh))
MOCSOURCES = $(patsubst %.hh, %.moc.cc, $(MOCHEADERS))
MOCOBJECTS = $(patsubst %.hh, $(OBJ_DIR)/%.moc.o,  $(MOCHEADERS))

IDL_EXPORT   = $(addsuffix C.i,    $(IDL_FILES)) \
               $(addsuffix C.hh,   $(IDL_FILES)) \
               $(addsuffix S.i,    $(IDL_FILES)) \
               $(addsuffix S.hh,   $(IDL_FILES)) \
               $(addsuffix S_T.i,  $(IDL_FILES)) \
               $(addsuffix S_T.hh, $(IDL_FILES)) \
               $(addsuffix S_T.cc, $(IDL_FILES))
 
IDL_DEPEND   = $(addsuffix S.cc,   $(IDL_FILES)) \
               $(addsuffix S.hh,   $(IDL_FILES)) \
               $(addsuffix S.i,    $(IDL_FILES)) \
               $(addsuffix S_T.cc, $(IDL_FILES)) \
               $(addsuffix S_T.hh, $(IDL_FILES)) \
               $(addsuffix S_T.i,  $(IDL_FILES)) \
               $(addsuffix C.cc,   $(IDL_FILES)) \
               $(addsuffix C.hh,   $(IDL_FILES)) \
               $(addsuffix C.i,    $(IDL_FILES))
 
IDL_CLEAN    = $(IDL_DEPEND) \
               $(addprefix $(OBJ_DIR)/, $(addsuffix S.o,   $(IDL_FILES))) \
               $(addprefix $(OBJ_DIR)/, $(addsuffix S_T.o, $(IDL_FILES))) \
               $(addprefix $(OBJ_DIR)/, $(addsuffix C.o,   $(IDL_FILES))) \
               $(addprefix $(INC_DIR)/, $(IDL_EXPORT)) 

EXPORT_CLEAN = $(addprefix $(INC_DIR)/, $(EXPORT_FILES))

INI_CLEAN    = $(addprefix $(ETC_DIR)/, $(INI_FILES))

CLEAN_FILES = $(IDL_CLEAN) \
              $(EXPORT_CLEAN) \
              $(INI_CLEAN) \
              $(OBJ_DIR) \
              $(MOCSOURCES)
 
TAO_IDLFLAGS += -cs C.cc -hc C.hh -ss S.cc -hs S.hh -sT S_T.cc -hT S_T.hh -ci C.i -si S.i -st S_T.i

DISTCLEAN_FILES = $(OBJ_DIR) *~ \
		$(addprefix $(LIB_DIR)/, $(LIBS_BIN)) \
		$(addprefix $(BIN_DIR)/, $(CPNT_BIN)) \
		$(addprefix $(TST_DIR)/, $(TEST_BIN))

##
## Compulsory rules
##

all: $(IDL_DEPEND)
	$(MAKE) export $(DEPEND)
	$(MAKE) build

build: build-libs build-components
# build-tests

build-libs: $(OBJ_DIR) $(LIB_DIR) \
	$(addprefix $(LIB_DIR)/, $(LIBS_BIN))

build-components: $(OBJ_DIR) $(BIN_DIR) \
	$(addprefix $(BIN_DIR)/, $(CPNT_BIN))

build-tests: $(OBJ_DIR) $(TST_DIR) \
	$(addprefix $(TST_DIR)/, $(TEST_BIN))

export: $(IDL_EXPORT)
	$(ECHO) "   ---- Installing header files etc."
	$(SILENT) $(MKDIR) $(ETC_DIR)
	$(SILENT) $(MKDIR) $(INC_DIR)
	$(foreach exp,$(IDL_EXPORT),$(INSTALL_H) `pwd`/$(exp) $(INC_DIR);)
	$(foreach exp,$(EXPORT_FILES),$(INSTALL_H) `pwd`/$(exp) $(INC_DIR);)
	$(foreach exp,$(INI_FILES),$(INSTALL_H) `pwd`/$(exp) $(ETC_DIR);)

clean:
	$(RM) $(CLEAN_FILES)

distclean: clean
	$(RM) $(DEPEND) Makefile.depend* $(DISTCLEAN_FILES)

##
## local rules
##

$(OBJ_DIR) $(LIB_DIR) $(BIN_DIR) $(TST_DIR):
	$(SILENT) $(MKDIR) $@

$(addprefix $(LIB_DIR)/,$(LIBS_BIN)): %: $(addprefix $(OBJ_DIR)/,$(foreach i, $(LIBS), $(LIB_$(i)_OBJ)))
	$(foreach i, $(LIBS), \
	  $(if $(findstring lib$(LIB_$(i)_BIN).a,$@), \
	    $(if $(filter $(addprefix $(OBJ_DIR)/,$(LIB_$(i)_OBJ)),$?), \
	      $(SILENT) $(AR) $@ $(addprefix $(OBJ_DIR)/, $(LIB_$(i)_OBJ)) \
	    ) \
	  ) \
	)
	$(foreach i, $(LIBS), \
	  $(if $(findstring lib$(LIB_$(i)_BIN).a,$@), \
	    $(if $(filter $(addprefix $(OBJ_DIR)/,$(LIB_$(i)_OBJ)),$?), \
	      $(SILENT) $(RANLIB) $@ \
	    ) \
	  ) \
	)

$(addprefix $(BIN_DIR)/,$(CPNT_BIN)): %: $(addprefix $(LIB_DIR)/,$(LIB_NAMES)) $(addprefix $(OBJ_DIR)/,$(foreach i, $(CPNT), $(CPNT_$(i)_OBJ)))
	$(foreach i, $(CPNT), \
	  $(if $(findstring $(CPNT_$(i)_BIN),$@), \
	    $(if $(filter $(addprefix $(LIB_DIR)/,$(LIB_NAMES)) $(addprefix $(OBJ_DIR)/,$(CPNT_$(i)_OBJ)),$?), \
	      $(CXX) $(INCLUDES) $(LIB_DIRS) -o $@ $(addprefix $(OBJ_DIR)/,$(CPNT_$(i)_OBJ)) $(CPNT_$(i)_LFLAGS) \
	    ) \
	  ) \
	)

$(addprefix $(TST_DIR)/,$(TEST_BIN)): %: $(addprefix $(LIB_DIR)/,$(LIB_NAMES)) $(addprefix $(OBJ_DIR)/,$(foreach i, $(TEST), $(TEST_$(i)_OBJ)))
	$(foreach i, $(TEST), \
	  $(if $(findstring $(TEST_$(i)_BIN),$@), \
	    $(if $(filter $(addprefix $(LIB_DIR)/,$(LIB_NAMES)) $(addprefix $(OBJ_DIR)/,$(TEST_$(i)_OBJ)),$?), \
	      $(CXX) $(INCLUDES) $(LIB_DIRS) -o $@ $(addprefix $(OBJ_DIR)/,$(TEST_$(i)_OBJ)) $(TEST_$(i)_LFLAGS) \
	    ) \
	  ) \
	)

# This is fun, isn't it? ;-)
#
# These rules are complicated since we want to build a dynamic number of targets
# with a static number of rules. The structure is as follows:
#
# [target set]: %: [requirement set]
# 	FOR EACH possible target DO
# 	  IF this is the target that shall be built THEN
# 	    IF this target depends on some files that have actually changed ($?) THEN
# 	      build it
# 	    ENDIF
# 	  ENDIF
# 	DONE

example: $(BIN_DIR)/smartVisualizationServer $(BIN_DIR)/smartExampleVisualizationClient 
	$(TAO_ROOT)/orbsvcs/Naming_Service/Naming_Service -m 0 -ORBEndpoint iiop://localhost:9999 &
	(sleep 1; $(BIN_DIR)/smartVisualizationServer -name=vis_server -w=640 -h=480 -min_x=-2 -max_x=2 -min_y=-1.5 -max_y=1.5 -interval=100 -ORBInitRef NameService=corbaloc:iiop:localhost:9999/NameService) &
	(sleep 2; $(BIN_DIR)/smartExampleVisualizationClient -server=vis_server -client=vis_example_client -ORBInitRef NameService=corbaloc:iiop:localhost:9999/NameService; $(MAKE) stop)

stop:
	-killall -9 smartExampleVisualizationClient
	-killall -9 smartVisualizationServer
	-killall -9 Naming_Service

##
## Standard suffix, version, packaging and dep rules
##
include $(RULES)


%S.cc %S.hh %S.i %C.cc %C.cc %C.hh %C.i : %.idl
	$(ECHO) "    ---- Processing $< (IDL)"
	$(IDL) $(TAO_IDLFLAGS) $<

$(MOCOBJECTS): $(OBJ_DIR)/%.moc.o: %.hh
$(MOCSOURCES): %.moc.cc: %.hh
	$(QTDIR)/bin/moc $< -o $@

