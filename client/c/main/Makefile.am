lib_LTLIBRARIES = libgridstore.la
ACLOCAL_AMFLAGS = -I m4

srcdir1 = ..
rootdir = $(srcdir1)/../..
utildir = $(rootdir)/utility
serverdir = $(rootdir)/server
commondir = $(serverdir)
picojsondir = $(rootdir)/3rd_party/picojson
ebbdir = $(rootdir)/3rd_party/ebb
shadir = $(rootdir)/3rd_party/sha2
securitydir = $(rootdir)/3rd_party/omaha/omaha/base/security
uuiddir = $(rootdir)/3rd_party/uuid
extdir = $(srcdir1)/ext
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
	test `nm -u $(sofile0) | grep -v '@GLIBC\|@CXXABI\|@GCC\| w ' | wc -l` -eq 0
	test -x $(sofile0) && mkdir -p $(bindir) && cp $(sofile0) $(bindir)/

$(bindir)/$(sofile2): $(bindir)/$(sofile1)
	( cd $(bindir) && test -x $(sofile1) && rm -f $(sofile2) && ln -s $(sofile1) $(sofile2) )

$(bindir)/$(sofile3): $(bindir)/$(sofile1)
	( cd $(bindir) && test -x $(sofile1) && rm -f $(sofile3) && ln -s $(sofile1) $(sofile3) )

clean-local:
	rm -fv $(bindir)/$(sofile3) $(bindir)/$(sofile2) $(bindir)/$(sofile1)
	rmdir -v $(bindir) 2> /dev/null || test 1

libgridstore_la_SOURCES = \
$(utildir)/util/code.cpp \
$(utildir)/util/container.cpp \
$(utildir)/util/file.cpp \
$(utildir)/util/net.cpp \
$(utildir)/util/os.cpp \
$(utildir)/util/system.cpp \
$(utildir)/util/thread.cpp \
$(utildir)/util/time.cpp \
$(utildir)/util/trace.cpp \
$(utildir)/util/type.cpp \
$(commondir)/authentication.cpp \
$(commondir)/bit_array.cpp \
$(commondir)/container_key.cpp \
$(commondir)/http.cpp \
$(commondir)/json.cpp \
$(commondir)/service_address.cpp \
$(commondir)/socket_wrapper.cpp \
$(commondir)/uuid_utils.cpp \
$(ebbdir)/ebb_request_parser.c \
$(shadir)/sha2.c \
$(securitydir)/md5.c \
$(uuiddir)/uuid/clear.c \
$(uuiddir)/uuid/compare.c \
$(uuiddir)/uuid/copy.c \
$(uuiddir)/uuid/gen_uuid.c \
$(uuiddir)/uuid/isnull.c \
$(uuiddir)/uuid/pack.c \
$(uuiddir)/uuid/parse.c \
$(uuiddir)/uuid/unpack.c \
$(uuiddir)/uuid/uuid_time.c \
$(srcdir1)/client.cpp \
$(srcdir1)/client_logging.cpp \
$(srcdir1)/container_key_utils.cpp 

includes = \
-I$(utildir) \
-I$(commondir) \
-I$(serverdir) \
-I$(picojsondir)/include \
-I$(ebbdir) \
-I$(shadir) \
-I$(securitydir) \
-I$(uuiddir) \
-I$(srcdir1)/include \
-I$(extdir)/include

libgridstore_la_CFLAGS   = $(includes) $(AM_CFLAGS)   -fvisibility=hidden
libgridstore_la_CXXFLAGS = $(includes) $(AM_CXXFLAGS) -fvisibility=hidden
libgridstore_la_LDFLAGS = $(AM_LDFLAGS) -version-info $(version)
libgridstore_la_LIBADD = -lpthread -lrt
