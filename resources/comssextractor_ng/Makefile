###################################################################################################

.PHONY:	r d p lr ld lp config all install install-bin clean distclean
all:	r lr

## Load Previous Configuration ####################################################################

-include config.mk

## Configurable options ###########################################################################

# Directory to store object files, libraries, executables, and dependencies:
BUILD_DIR      ?= build

# Include debug-symbols in release builds
CMP_RELSYM ?= -g

# Sets of compile flags for different build types
CMP_REL    ?= -O3 -D NDEBUG
CMP_DEB    ?= -O0 -D DEBUG 
CMP_PRF    ?= -O2 -D NDEBUG

# GNU Standard Install Prefix
prefix         ?= /usr/local

## Write Configuration  ###########################################################################

config:
	@( echo 'BUILD_DIR?=$(BUILD_DIR)'           ; \
	   echo 'CMP_RELSYM?=$(CMP_RELSYM)' ; \
	   echo 'CMP_REL?=$(CMP_REL)'       ; \
	   echo 'CMP_DEB?=$(CMP_DEB)'       ; \
	   echo 'CMP_PRF?=$(CMP_PRF)'       ; \
	   echo 'prefix?=$(prefix)'                 ) > config.mk

## Configurable options end #######################################################################

INSTALL ?= install

# GNU Standard Install Variables
exec_prefix ?= $(prefix)
includedir  ?= $(prefix)/include
bindir      ?= $(exec_prefix)/bin
libdir      ?= $(exec_prefix)/lib
datarootdir ?= $(prefix)/share
mandir      ?= $(datarootdir)/man

# Target file names
CMP      = cmp#       Name of CMP main executable.
CMP_SLIB = lib$(CMP).a#  Name of CMP static library.


CMP_CXXFLAGS = -I. -D __STDC_LIMIT_MACROS -D __STDC_FORMAT_MACROS -Wall -Wno-parentheses -Wextra -Wno-unused-parameter -Wno-literal-suffix -Wno-class-memaccess -Wno-misleading-indentation -Wno-shift-negative-value -Wno-unused-but-set-variable -Wno-sign-compare -Wno-reorder
CMP_LDFLAGS  = -Wall -lz

ECHO=@echo
ifeq ($(VERB),)
VERB=@
else
VERB=
endif

SRCS = $(wildcard minisat/core/*.cc) $(wildcard minisat/simp/*.cc) $(wildcard minisat/utils/*.cc) \
	$(wildcard glucose/core/*.cc) $(wildcard glucose/simp/*.cc) $(wildcard glucose/utils/*.cc) \
	$(wildcard cmp/core/*.cc) $(wildcard cmp/enum/*.cc) $(wildcard cmp/approx/*.cc) \
	$(wildcard cmp/ifaces/*.cc) $(wildcard cmp/utils/*.cc)
MINISAT_HDRS = $(wildcard minisat/mtl/*.h) $(wildcard minisat/core/*.h) $(wildcard minisat/simp/*.h) $(wildcard minisat/utils/*.h)
GLUCOSE_HDRS = $(wildcard glucose/mtl/*.h) $(wildcard glucose/core/*.h) $(wildcard glucose/simp/*.h) $(wildcard glucose/utils/*.h)
CMP_HDRS = $(wildcard cmp/utils/*.h) $(wildcard cmp/ifaces/*.h) $(wildcard cmp/core/*.h) $(wildcard cmp/enum/*.h) $(wildcard cmp/approx/*.h)

OBJS = $(filter-out %Main.o, $(SRCS:.cc=.o))

r:	$(BUILD_DIR)/release/bin/$(CMP)
d:	$(BUILD_DIR)/debug/bin/$(CMP)
p:	$(BUILD_DIR)/profile/bin/$(CMP)


lr:	$(BUILD_DIR)/release/lib/$(CMP_SLIB)
ld:	$(BUILD_DIR)/debug/lib/$(CMP_SLIB)
lp:	$(BUILD_DIR)/profile/lib/$(CMP_SLIB)



## Build-type Compile-flags:
$(BUILD_DIR)/release/%.o:			CMP_CXXFLAGS +=$(CMP_REL) $(CMP_RELSYM)
$(BUILD_DIR)/debug/%.o:				CMP_CXXFLAGS +=$(CMP_DEB) -g
$(BUILD_DIR)/profile/%.o:			CMP_CXXFLAGS +=$(CMP_PRF) -pg

## Build-type Link-flags:
$(BUILD_DIR)/profile/bin/$(CMP):		CMP_LDFLAGS += -pg
$(BUILD_DIR)/release/bin/$(CMP):		CMP_LDFLAGS += --static $(CMP_RELSYM)


## Executable dependencies
$(BUILD_DIR)/release/bin/$(CMP): 	$(BUILD_DIR)/release/cmp/core/Main.o $(BUILD_DIR)/release/lib/$(CMP_SLIB)
$(BUILD_DIR)/debug/bin/$(CMP):	 	$(BUILD_DIR)/debug/cmp/core/Main.o $(BUILD_DIR)/debug/lib/$(CMP_SLIB)
$(BUILD_DIR)/profile/bin/$(CMP): 	$(BUILD_DIR)/profile/cmp/core/Main.o $(BUILD_DIR)/profile/lib/$(CMP_SLIB)



## Library dependencies
$(BUILD_DIR)/release/lib/$(CMP_SLIB):	$(foreach o,$(OBJS),$(BUILD_DIR)/release/$(o))
$(BUILD_DIR)/debug/lib/$(CMP_SLIB):		$(foreach o,$(OBJS),$(BUILD_DIR)/debug/$(o))
$(BUILD_DIR)/profile/lib/$(CMP_SLIB):	$(foreach o,$(OBJS),$(BUILD_DIR)/profile/$(o))


## Compile rules (these should be unified, buit I have not yet found a way which works in GNU Make)
$(BUILD_DIR)/release/%.o:	%.cc
	$(ECHO) Compiling: $@
	$(VERB) mkdir -p $(dir $@)
	$(VERB) $(CXX) $(CMP_CXXFLAGS) $(CXXFLAGS) -c -o $@ $< -MMD -MF $(BUILD_DIR)/release/$*.d

$(BUILD_DIR)/profile/%.o:	%.cc
	$(ECHO) Compiling: $@
	$(VERB) mkdir -p $(dir $@)
	$(VERB) $(CXX) $(CMP_CXXFLAGS) $(CXXFLAGS) -c -o $@ $< -MMD -MF $(BUILD_DIR)/profile/$*.d

$(BUILD_DIR)/debug/%.o:	%.cc
	$(ECHO) Compiling: $@
	$(VERB) mkdir -p $(dir $@)
	$(VERB) $(CXX) $(CMP_CXXFLAGS) $(CXXFLAGS) -c -o $@ $< -MMD -MF $(BUILD_DIR)/debug/$*.d


## Linking rule
$(BUILD_DIR)/release/bin/$(CMP) $(BUILD_DIR)/debug/bin/$(CMP) $(BUILD_DIR)/profile/bin/$(CMP):
	$(ECHO) Linking Binary: $@
	$(VERB) mkdir -p $(dir $@)
	$(VERB) $(CXX) $^ $(CMP_LDFLAGS) $(LDFLAGS) -o $@

## Static Library rule
%/lib/$(CMP_SLIB):
	$(ECHO) Linking Static Library: $@
	$(VERB) mkdir -p $(dir $@)
	$(VERB) $(AR) -rcs $@ $^


install:	install-headers install-lib install-bin
install-debug:	install-headers install-lib-debug

install-headers:
#       Create directories
	$(INSTALL) -d $(DESTDIR)$(includedir)/cmp
	$(INSTALL) -d $(DESTDIR)$(includedir)/minisat
	$(INSTALL) -d $(DESTDIR)$(includedir)/glucose	
#
	for dir in  cmp/utils cmp/ifaces cmp/core cmp/enum cmp/approx; do \
	  $(INSTALL) -d $(DESTDIR)$(includedir)/cmp/$$dir ; \
	done
	for dir in minisat/mtl minisat/utils minisat/core minisat/simp; do \
	  $(INSTALL) -d $(DESTDIR)$(includedir)/minisat/$$dir ; \
	done
	for dir in glucose/mtl glucose/utils glucose/core glucose/simp; do \
	  $(INSTALL) -d $(DESTDIR)$(includedir)/glucose/$$dir ; \
	done
#       Install headers
	for h in $(MINISAT_HDRS) ; do \
	  $(INSTALL) -m 644 $$h $(DESTDIR)$(includedir)/$$h ; \
	done
	for h in $(GLUCOSE_HDRS) ; do \
	  $(INSTALL) -m 644 $$h $(DESTDIR)$(includedir)/$$h ; \
	done
	for h in $(CMP_HDRS) ; do \
	  $(INSTALL) -m 644 $$h $(DESTDIR)$(includedir)/$$h ; \
	done

install-lib-debug: $(BUILD_DIR)/debug/lib/$(CMP_SLIB)
	$(INSTALL) -d $(DESTDIR)$(libdir)
	$(INSTALL) -m 644 $(BUILD_DIR)/debug/lib/$(CMP_SLIB) $(DESTDIR)$(libdir)

install-lib: $(BUILD_DIR)/release/lib/$(CMP_SLIB)
	$(INSTALL) -d $(DESTDIR)$(libdir)
	$(INSTALL) -m 644 $(BUILD_DIR)/release/lib/$(CMP_SLIB) $(DESTDIR)$(libdir)

install-bin: $(BUILD_DIR)/release/bin/$(CMP)
	$(INSTALL) -d $(DESTDIR)$(bindir)
	$(INSTALL) -m 755 $(BUILD_DIR)/release/bin/$(CMP) $(DESTDIR)$(bindir)

clean:
	rm -f $(foreach t, release debug profile, $(foreach o, $(SRCS:.cc=.o), $(BUILD_DIR)/$t/$o)) \
          $(foreach t, release debug profile, $(foreach d, $(SRCS:.cc=.d), $(BUILD_DIR)/$t/$d)) \
	  $(foreach t, release debug profile, $(BUILD_DIR)/$t/bin/$(CMP)) \
	  $(foreach t, release debug profile, $(BUILD_DIR)/$t/lib/$(CMP_SLIB))


distclean:	clean
	rm -f config.mk

## Include generated dependencies
-include $(foreach s, $(SRCS:.cc=.d), $(BUILD_DIR)/release/$s)
-include $(foreach s, $(SRCS:.cc=.d), $(BUILD_DIR)/debug/$s)
-include $(foreach s, $(SRCS:.cc=.d), $(BUILD_DIR)/profile/$s)
