# Line separator must be LF
lib_LTLIBRARIES = libgridstore.la
ACLOCAL_AMFLAGS = -I m4

srcdir = ..
rootdir = $(srcdir)/../..
utildir = $(rootdir)/utility
serverdir = $(rootdir)/server
commondir = $(serverdir)
picojsondir = $(rootdir)/3rd_party/picojson
ebbdir = $(rootdir)/3rd_party/ebb
shadir = $(rootdir)/3rd_party/sha2
uuiddir = $(rootdir)/3rd_party/uuid
bindir = $(rootdir)/bin

version = 0:0:0
soprefix1 = .0.0.0
soprefix2 = .0

sofile = libgridstore.so
sofile0 = .libs/$(sofile)$(soprefix1)
sofile1 = $(sofile)$(soprefix1)
sofile2 = $(sofile)$(soprefix2)
sofile3 = $(sofile)

all-local: $(bindir)/$(sofile1) $(bindir)/$(sofile2) $(bindir)/$(sofile3)

$(bindir)/$(sofile1): $(lib_LTLIBRARIES)
	test `nm -u $(sofile0) | grep -v '@@GLIBC\|@@CXXABI\|@@GCC\| w ' | wc -l` -eq 0
	test -x $(sofile0) && mkdir -p $(bindir) && cp $(sofile0) $(bindir)/

$(bindir)/$(sofile2): $(bindir)/$(sofile1)
	( cd $(bindir) && test -x $(sofile1) && rm -f $(sofile2) && ln -s $(sofile1) $(sofile2) )

$(bindir)/$(sofile3): $(bindir)/$(sofile1)
	( cd $(bindir) && test -x $(sofile1) && rm -f $(sofile3) && ln -s $(sofile1) $(sofile3) )

clean-local:
	rm -fv $(bindir)/$(sofile3) $(bindir)/$(sofile2) $(bindir)/$(sofile1)
	rmdir -v $(bindir) 2> /dev/null || test 1

libgridstore_la_SOURCES = \
$(utildir)/util/allocator.cpp \
$(utildir)/util/allocator.h \
$(utildir)/util/code.cpp \
$(utildir)/util/code.h \
$(utildir)/util/container.cpp \
$(utildir)/util/container.h \
$(utildir)/util/file.cpp \
$(utildir)/util/file.h \
$(utildir)/util/net.cpp \
$(utildir)/util/net.h \
$(utildir)/util/os.cpp \
$(utildir)/util/os.h \
$(utildir)/util/system.cpp \
$(utildir)/util/system.h \
$(utildir)/util/thread.cpp \
$(utildir)/util/thread.h \
$(utildir)/util/time.cpp \
$(utildir)/util/time.h \
$(utildir)/util/trace.cpp \
$(utildir)/util/trace.h \
$(utildir)/util/type.cpp \
$(utildir)/util/type.h \
$(commondir)/authentication.cpp \
$(commondir)/http.cpp \
$(commondir)/json.cpp \
$(commondir)/service_address.cpp \
$(commondir)/uuid_utils.cpp \
$(ebbdir)/ebb_request_parser.c \
$(shadir)/sha2.c \
$(uuiddir)/uuid/clear.c \
$(uuiddir)/uuid/compare.c \
$(uuiddir)/uuid/copy.c \
$(uuiddir)/uuid/gen_uuid.c \
$(uuiddir)/uuid/isnull.c \
$(uuiddir)/uuid/pack.c \
$(uuiddir)/uuid/parse.c \
$(uuiddir)/uuid/unpack.c \
$(uuiddir)/uuid/unparse.c \
$(uuiddir)/uuid/uuid_time.c \
$(srcdir)/client.cpp

libgridstore_la_CFLAGS = $(AM_CFLAGS) -fvisibility=hidden
libgridstore_la_CXXFLAGS = $(AM_CXXFLAGS) -fvisibility=hidden
libgridstore_la_LDFLAGS = $(AM_LDFLAGS) -version-info $(version)
libgridstore_la_LIBADD = -lpthread -lrt

INCLUDES = \
-I$(utildir) \
-I$(commondir) \
-I$(serverdir) \
-I$(picojsondir)/include \
-I$(ebbdir) \
-I$(shadir) \
-I$(uuiddir) \
-I$(srcdir)/include
