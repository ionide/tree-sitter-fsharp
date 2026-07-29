VERSION := 0.3.10

LANGUAGE_NAME := tree-sitter-fsharp

# repository
FSHARP_DIR := fsharp
SIGNATURE_DIR := fsharp_signature

PARSER_REPO_URL := $(shell git remote get-url origin 2>/dev/null)

ifeq ($(PARSER_URL),)
	PARSER_URL := $(subst .git,,$(PARSER_REPO_URL))
ifeq ($(shell echo $(PARSER_URL) | grep '^[a-z][-+.0-9a-z]*://'),)
	PARSER_URL := $(subst :,/,$(PARSER_URL))
	PARSER_URL := $(subst git@,https://,$(PARSER_URL))
endif
endif

TS ?= tree-sitter

# ABI versioning
SONAME_MAJOR := $(word 1,$(subst ., ,$(VERSION)))
SONAME_MINOR := $(word 2,$(subst ., ,$(VERSION)))

# install directory layout
PREFIX ?= /usr/local
DATADIR ?= $(PREFIX)/share
INCLUDEDIR ?= $(PREFIX)/include
LIBDIR ?= $(PREFIX)/lib
PCLIBDIR ?= $(LIBDIR)/pkgconfig

# object files
# Both grammars link into one library. Each .c is its own translation unit, so
# the two scanners' static helpers in common/scanner.h do not collide, and the
# exported symbols are namespaced per grammar.
SRCS := $(FSHARP_DIR)/src/parser.c $(FSHARP_DIR)/src/scanner.c \
	$(SIGNATURE_DIR)/src/parser.c $(SIGNATURE_DIR)/src/scanner.c
OBJS := $(SRCS:.c=.o)

# flags
ARFLAGS := rcs
override CFLAGS += -I$(FSHARP_DIR)/src -I$(SIGNATURE_DIR)/src -std=c11 -fPIC

# OS-specific bits
ifeq ($(OS),Windows_NT)
	$(error "Windows is not supported")
else ifeq ($(shell uname),Darwin)
	SOEXT = dylib
	SOEXTVER_MAJOR = $(SONAME_MAJOR).dylib
	SOEXTVER = $(SONAME_MAJOR).$(SONAME_MINOR).dylib
	LINKSHARED := $(LINKSHARED)-dynamiclib -Wl,
	ifneq ($(ADDITIONAL_LIBS),)
	LINKSHARED := $(LINKSHARED)$(ADDITIONAL_LIBS),
	endif
	LINKSHARED := $(LINKSHARED)-install_name,$(LIBDIR)/lib$(LANGUAGE_NAME).$(SONAME_MAJOR).dylib,-rpath,@executable_path/../Frameworks
else ifneq ($(findstring mingw32,$(shell $(CC) -dumpmachine)),)
	SOEXT = dll
	LINKSHARED += -s -shared -Wl,--out-implib,$(@:dll=lib)
lib$(LANGUAGE_NAME).lib: lib$(LANGUAGE_NAME).$(SOEXT)
else
	SOEXT = so
	SOEXTVER_MAJOR = so.$(SONAME_MAJOR)
	SOEXTVER = so.$(SONAME_MAJOR).$(SONAME_MINOR)
	LINKSHARED := $(LINKSHARED)-shared -Wl,
	ifneq ($(ADDITIONAL_LIBS),)
	LINKSHARED := $(LINKSHARED)$(ADDITIONAL_LIBS)
	endif
	LINKSHARED := $(LINKSHARED)-soname,lib$(LANGUAGE_NAME).so.$(SONAME_MAJOR)
endif
ifneq ($(filter $(shell uname),FreeBSD NetBSD DragonFly),)
	PCLIBDIR := $(PREFIX)/libdata/pkgconfig
endif

all: lib$(LANGUAGE_NAME).a lib$(LANGUAGE_NAME).$(SOEXT) $(LANGUAGE_NAME).pc

# The real scanner logic lives in the shared header, not in the per-grammar
# shims, so the implicit .c -> .o rule would otherwise miss edits to it. Keep
# this below `all` so it does not become the default goal.
$(FSHARP_DIR)/src/scanner.o $(SIGNATURE_DIR)/src/scanner.o: common/scanner.h

lib$(LANGUAGE_NAME).a: $(OBJS)
	$(AR) $(ARFLAGS) $@ $^

lib$(LANGUAGE_NAME).$(SOEXT): $(OBJS)
	$(CC) $(LDFLAGS) $(LINKSHARED) $^ $(LDLIBS) -o $@
ifneq ($(STRIP),)
	$(STRIP) $@
endif

$(LANGUAGE_NAME).pc: bindings/c/$(LANGUAGE_NAME).pc.in
	sed  -e 's|@URL@|$(PARSER_URL)|' \
		-e 's|@VERSION@|$(VERSION)|' \
		-e 's|@LIBDIR@|$(LIBDIR)|' \
		-e 's|@INCLUDEDIR@|$(INCLUDEDIR)|' \
		-e 's|@REQUIRES@|$(REQUIRES)|' \
		-e 's|@ADDITIONAL_LIBS@|$(ADDITIONAL_LIBS)|' \
		-e 's|=$(PREFIX)|=$${prefix}|' \
		-e 's|@PREFIX@|$(PREFIX)|' $< > $@

$(FSHARP_DIR)/src/parser.c: $(FSHARP_DIR)/grammar.js
	cd $(FSHARP_DIR) && $(TS) generate

$(SIGNATURE_DIR)/src/parser.c: $(FSHARP_DIR)/grammar.js $(SIGNATURE_DIR)/grammar.js
	cd $(SIGNATURE_DIR) && $(TS) generate

install: all
	install -d '$(DESTDIR)$(DATADIR)'/tree-sitter/queries/$(FSHARP_DIR) '$(DESTDIR)$(DATADIR)'/tree-sitter/queries/$(SIGNATURE_DIR) '$(DESTDIR)$(INCLUDEDIR)'/tree_sitter '$(DESTDIR)$(PCLIBDIR)' '$(DESTDIR)$(LIBDIR)'
	install -m644 bindings/c/tree_sitter/$(LANGUAGE_NAME).h '$(DESTDIR)$(INCLUDEDIR)'/tree_sitter/$(LANGUAGE_NAME).h
	install -m644 $(LANGUAGE_NAME).pc '$(DESTDIR)$(PCLIBDIR)'/$(LANGUAGE_NAME).pc
	install -m644 lib$(LANGUAGE_NAME).a '$(DESTDIR)$(LIBDIR)'/lib$(LANGUAGE_NAME).a
	install -m755 lib$(LANGUAGE_NAME).$(SOEXT) '$(DESTDIR)$(LIBDIR)'/lib$(LANGUAGE_NAME).$(SOEXTVER)
	ln -sf lib$(LANGUAGE_NAME).$(SOEXTVER) '$(DESTDIR)$(LIBDIR)'/lib$(LANGUAGE_NAME).$(SOEXTVER_MAJOR)
	ln -sf lib$(LANGUAGE_NAME).$(SOEXTVER_MAJOR) '$(DESTDIR)$(LIBDIR)'/lib$(LANGUAGE_NAME).$(SOEXT)
	install -m644 queries/*.scm '$(DESTDIR)$(DATADIR)'/tree-sitter/queries/$(FSHARP_DIR)
	install -m644 $(SIGNATURE_DIR)/queries/*.scm '$(DESTDIR)$(DATADIR)'/tree-sitter/queries/$(SIGNATURE_DIR)

uninstall:
	$(RM) '$(DESTDIR)$(LIBDIR)'/lib$(LANGUAGE_NAME).a \
		'$(DESTDIR)$(LIBDIR)'/lib$(LANGUAGE_NAME).$(SOEXTVER) \
		'$(DESTDIR)$(LIBDIR)'/lib$(LANGUAGE_NAME).$(SOEXTVER_MAJOR) \
		'$(DESTDIR)$(LIBDIR)'/lib$(LANGUAGE_NAME).$(SOEXT) \
		'$(DESTDIR)$(INCLUDEDIR)'/tree_sitter/$(LANGUAGE_NAME).h \
		'$(DESTDIR)$(PCLIBDIR)'/$(LANGUAGE_NAME).pc
	$(RM) -r '$(DESTDIR)$(DATADIR)'/tree-sitter/queries/$(FSHARP_DIR) \
		'$(DESTDIR)$(DATADIR)'/tree-sitter/queries/$(SIGNATURE_DIR)

clean:
	$(RM) $(OBJS) $(LANGUAGE_NAME).pc lib$(LANGUAGE_NAME).a lib$(LANGUAGE_NAME).$(SOEXT) lib$(LANGUAGE_NAME).lib

test:
	$(TS) test

.PHONY: all install uninstall clean test
