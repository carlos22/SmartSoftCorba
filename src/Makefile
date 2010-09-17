# --------------------------------------------------------------------------
#
#  Copyright (C) 2002/2004 Christian Schlegel
#
#        schlegel@fh-ulm.de
#
#        Prof. Dr. Christian Schlegel
#        University of Applied Sciences
#        Prittwitzstr. 10
#        D-89075 Ulm
#        Germany
#
#        Previous affiliation:
#
#        FAW Ulm
#        Research Institute for Applied Knowledge Processing
#        Helmholtzstr. 16
#        D-89081 Ulm
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
# --------------------------------------------------------------------------

NAME  = SmartSoft

CONF  = Makefile.smart.conf
RULES = Makefile.smart.rules

LOCAL_CONF = Makefile.local.conf

include $(CONF)

ifeq ($(LOCAL_CONF),$(wildcard $(LOCAL_CONF)))
include $(LOCAL_CONF)
endif

#
# original categories
#
CATEGORIES = 	utility \
		interfaceClasses \
                smartSoftKernel \
                components \
		$(LOCAL_CATEGORIES)

PACKAGES = \
	$(strip $(foreach C, $(CATEGORIES), \
		$(foreach P, $(wildcard $(C)/*), \
			$(subst /SMARTSOFT_PACKAGE,,$(wildcard $(P)/SMARTSOFT_PACKAGE)) \
		) \
))

PACKAGES += \
	$(strip $(foreach C, $(CATEGORIES), \
		$(foreach P, $(wildcard $(C)/*), \
			$(subst /SMARTSOFT_PACKAGE_MDSD,/src,$(wildcard $(P)/SMARTSOFT_PACKAGE_MDSD)) \
		) \
	))

DOC_FILE_LIST = \
	$(strip $(foreach P, $(PACKAGES), \
		$(if $(wildcard $(P)/DOC_FILE_LIST), \
			$(addprefix $(P)/, $(shell cat $(P)/DOC_FILE_LIST)) \
		) \
	))

RECURSE = $(SILENT) \
	echo "Considering $(PACKAGES)" ; \
	for i in $(PACKAGES) ; do \
		if [ -d $$i ] ; then \
			if ( cd $$i ; echo ; echo "    MODULE:"  $$i ; \
				${MAKE} $@ ; ) then \
				echo -n ; \
			else \
				exit 1 ; \
			fi ; \
		fi \
	done

##
## Compulsory rules
##

all:  export $(DEPEND) build-libs build-components doc success

tests: build-tests

doc:
	$(ECHO)
	$(ECHO) "       D O C U M E N T A T I O N"
	$(ECHO)
	$(SILENT) \
	(INPUT_FILES="$(DOC_FILE_LIST)" doxygen)

build:
	$(ECHO)
	$(ECHO) "       B U I L D"
	$(ECHO)
	$(RECURSE)

build-libs:
	$(ECHO)
	$(ECHO) "       B U I L D   L I B S"
	$(ECHO)
	$(RECURSE)

build-components:
	$(ECHO)
	$(ECHO) "       B U I L D   C O M P O N E N T S"
	$(ECHO)
	$(RECURSE)

build-tests:
	$(ECHO)
	$(ECHO) "       B U I L D   T E S T S"
	$(ECHO)
	$(RECURSE)

export:
	$(ECHO)
	$(ECHO) "       E X P O R T"
	$(ECHO)
	$(RECURSE)

install: all

new:  clean all tags

clean:
	$(ECHO)
	$(ECHO) "       C L E A N"
	$(ECHO)
	$(RECURSE)
	$(RM) *~

distclean:
	$(ECHO)
	$(ECHO) "       D I S T     C L E A N"
	$(ECHO)
	$(RECURSE)
	$(RM) ../include ../lib/* ../bin/* ../etc ../log ../doc/html ../doc/latex

dep $(DEPEND) depend: 
	$(ECHO)
	$(ECHO) "       D E P E N D"
	$(ECHO)
	$(RECURSE)

success:
	$(ECHO)
	$(ECHO) "---------------------------------------------------"
	$(ECHO)
	$(ECHO) "       Software  S U C C E S S F U L L Y  compiled."
	$(ECHO)
	$(ECHO) "---------------------------------------------------"

