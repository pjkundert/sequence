# 
# Sequence Arithmetic Framework
# 

VERSION		= 1.0.1
ARCH		= $(shell uname -m | sed -e s/i.86/i386/ -e s/9000.*/hppa/)
#DEBUG		= yes				# compile w/ DEBUG=yes make test
TARGET		= /usr/local/include

CXXFLAGS	+= -I.
ifneq ($(DEBUG),)
	CXXFLAGS += -DDEBUG
endif

# Default flags for building test executables.  Not required by
# default (only units.H is required by client programs)!  Figure out
# deguggable vs. optimized,
ifeq ($(ARCH),i386)
    ifneq ($(DEBUG),)
	CXXFLAGS += -g
    else
        CXXFLAGS += -O3
    endif

    ifeq ($(CXX),icc)
        CXXFLAGS += -Wall
    else
	ifeq ($(CXX),)
	    CXX	 = g++
	endif
        CXXFLAGS += -pipe -Wstrict-overflow=0		\
		    -W -Wall -Wwrite-strings		 \
		    -Wcast-align -Wpointer-arith	  \
		    -Wcast-qual -Wfloat-equal		   \
		    -Wnon-virtual-dtor -Woverloaded-virtual \
		    -Wsign-promo -Wshadow -Wstrict-overflow=0
    endif
endif
ifeq ($(ARCH),x86_64)
    ifneq ($(DEBUG),)
	CXXFLAGS += -g
    else
        CXXFLAGS += -O3
    endif

    ifeq ($(CXX),icc)
        CXXFLAGS += -Wall
    else
        CXX	 = g++
        CXXFLAGS += -pipe 				\
		    -W -Wall -Wwrite-strings		 \
		    -Wcast-align -Wpointer-arith	  \
		    -Wcast-qual -Wfloat-equal		   \
		    -Wnon-virtual-dtor -Woverloaded-virtual \
		    -Wsign-promo -Wshadow
    endif
endif
ifeq ($(ARCH),hppa)
    ifneq ($(DEBUG),)
	CXXFLAGS	+= -g
    else
        CXXFLAGS	+= +O3
    endif

    CXX		= aCC
    CXXFLAGS   += -AA +p		\
		  +DAportable		 \
		  -D_HPUX_SOURCE	  \
		  -D_POSIX_C_SOURCE=199506L\
		  -D_RWSTD_MULTI_THREAD
    CXXFLAGS   += -mt
endif

CXXFLAGS	+= -I../cut/

# 
# Basic Targets
# 
#     The default target 'all' builds (optional) unit test object, and
# source distribution package.  Neither of these are necessary to use
# sequence.  The sequence-test.o object may optionally be linked into
# applications, to include sequence's CUT unit tests.
# 
.PHONY: all configure test test-html install dist distclean clean

all:			sequence-test.o

configure:

test:			configure sequence-test
	time ./sequence-test

test-html:		sequence-test.html

install:
	cp sequence sequence.H $(TARGET)

dist:			distclean sequence-$(VERSION).tgz

distclean:		clean

clean:
	-rm -f  *.tgz 				\
		*.html 				 \
		*~ 	*/*~ 			  \
		*.o 	*/*.o 			   \
		core* 	*/core*			    \
		sequence-test

# 
# Unit Tests
# 
headers		= 	sequence		\
			sequence.H

sequence-test.o:	sequence-test.C $(headers)
	$(CXX) $(CXXFLAGS) -c -DTEST sequence-test.C -o $@

sequence-test: 		$(headers) sequence-test.C
	$(CXX) $(CXXFLAGS) -DTESTSTANDALONE -DTEST sequence-test.C -o $@

# 
# Force generation of HTML unit test output, by indicated to CUT that
# it is executing in a CGI environment (REQUEST_METHOD=...).
# 
sequence-test.html:	sequence-test
	REQUEST_METHOD=true ./sequence-test > sequence-test.html

# 
# Distribution
# 
sequence-$(VERSION).tgz:configure		\
			GNUmakefile		 \
			sequence		  \
			sequence.H		   \
			COPYING			    \
			README			     \
	 		INSTALL			      \
			sequence-test.C
	rm -f   sequence-$(VERSION)
	ln -s . sequence-$(VERSION)
	tar czf $@ $(addprefix sequence-$(VERSION)/,$^)
	rm -f   sequence-$(VERSION)
