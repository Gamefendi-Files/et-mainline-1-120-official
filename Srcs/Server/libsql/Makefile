CXX	= clang++-devel

PLATFORM = $(shell file /bin/ls | cut -d' ' -f3 | cut -d'-' -f1)
BSD_VERSION = $(shell freebsd-version | grep -oE '[0-9]+\.[0-9]+')

BIN = ./libsql.a

IFLAGS = -I/usr/local/include
CFLAGS  = $(IFLAGS) -Wall -O2 -pipe -mtune=i686 -D_THREAD_SAFE -fno-exceptions 

LIBS =

OBJFILES = AsyncSQL.o Semaphore.o Tellwait.o Statement.o

default:
	$(MAKE) $(BIN)

$(BIN): $(OBJFILES)
	ar cru $(BIN) $(OBJFILES) $(LIBS)
	ranlib $(BIN)
	chmod 700 $(BIN)

clean:
	rm -f *.o
	rm -f $(BIN)

dep:
	touch Depend
	$(CXX) $(CFLAGS) -MM *.cpp > Depend

$(OBJFILES):
	$(CXX) $(CFLAGS) -c $<

sinclude Depend
