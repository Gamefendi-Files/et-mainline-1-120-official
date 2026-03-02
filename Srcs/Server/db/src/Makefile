MAKE = gmake
CC = clang++-devel

INCDIR = -I/usr/local/include
LIBDIR = -L/usr/local/lib
BINDIR = ../../Bin
OBJDIR = .obj

BSD_VERSION = $(shell freebsd-version | grep -oE '[0-9]+\.[0-9]+')
PLATFORM = $(shell file /bin/ls | cut -d' ' -f3 | cut -d'-' -f1)

$(shell if [ ! -d $(OBJDIR) ]; then mkdir $(OBJDIR); fi)

TARGET = $(BINDIR)/db_$(PLATFORM)

CFLAGS = -g -Wall -O2 -pipe -fno-rtti -mtune=i686 -fno-exceptions -pthread -D_THREAD_SAFE

# boost
INCDIR += -I../../../ServerExtern-FreeBSD/include

# Project Library
LIBDIR += -L../../libthecore/lib -L../../libsql -L../../libpoly -L../../libgame/lib
LIBS = -lthecore -lsql -lpoly -lgame -lm -lz

# MySQL
LIBS += /usr/local/lib/mysql/libmysqlclient.a /usr/lib/libz.a /usr/local/lib/libzstd.a

#OpenSSL
LIBS += -lunwind -lssl -lcrypto

SRCS =	Config.cpp NetBase.cpp Peer.cpp PeerBase.cpp Main.cpp Lock.cpp DBManager.cpp \
		Cache.cpp LoginData.cpp ClientManager.cpp ClientManagerPlayer.cpp ClientManagerLogin.cpp \
		ClientManagerBoot.cpp ClientManagerParty.cpp ClientManagerGuild.cpp GuildManager.cpp \
		PrivManager.cpp MoneyLog.cpp ItemAwardManager.cpp ClientManagerEventFlag.cpp Marriage.cpp \
		ItemIDRangeManager.cpp ClientManagerHorseName.cpp \
		ProtoReader.cpp CsvReader.cpp 

OBJS = $(SRCS:%.cpp=$(OBJDIR)/%.o)

default: $(TARGET)

$(TARGET): $(OBJS)
	@echo -e "\033[0;32m [---] \033[0m		\033[0;33m Linking:\033[0m" $(TARGET)
	@$(CC) $(CFLAGS) $(LIBDIR) $(OBJS) $(LIBS) -o $(TARGET)

$(OBJDIR)/%.o: %.cpp
	@echo -e "\033[0;32m [---] \033[0m		\033[0;33m Compiling:\033[0m" $<
	@$(CC) $(CFLAGS) $(INCDIR) -c $< -o $@

$(OBJDIR):
	@mkdir $(OBJDIR)

clean:
	@rm -f $(OBJS) $(BINDIR)/db_*

dep:
	@touch Depend
	makedepend -fDepend $(INCDIR) -I/usr/include/c++/v1 -p$(OBJDIR)/ $(SRCS) 2> /dev/null

sinclude Depend
