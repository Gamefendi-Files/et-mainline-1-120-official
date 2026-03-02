PLATFORM = $(shell file /bin/ls | cut -d' ' -f3 | cut -d'-' -f1)
BSD_VERSION = $(shell freebsd-version | grep -oE '[0-9]+\.[0-9]+')

CC = clang++-devel

INCDIR = -I/usr/local/include
LIBDIR =
BINDIR = ../../Bin
OBJDIR = OBJDIR
$(shell if [ ! -d $(OBJDIR) ]; then mkdir $(OBJDIR); fi)

# Standard Setting
LIBS = -pthread -lm -lmd

CFLAGS = -g -Wall -O2 -m32 -pipe -fexceptions -mtune=i686 -fstack-protector-all -D_THREAD_SAFE -DNDEBUG

# boost
INCDIR += -I../../../ServerExtern-FreeBSD/include/boost

# DevIL
INCDIR += -I../../../ServerExtern-FreeBSD/include/IL
LIBS += ../../../ServerExtern-FreeBSD/lib/libIL.a\
		../../../ServerExtern-FreeBSD/lib/libjasper.a\
		../../../ServerExtern-FreeBSD/lib/libpng.a\
		../../../ServerExtern-FreeBSD/lib/libtiff.a\
		../../../ServerExtern-FreeBSD/lib/libjbig.a\
		../../../ServerExtern-FreeBSD/lib/libmng.a\
		/usr/lib/liblzma.a\
		../../../ServerExtern-FreeBSD/lib/liblcms.a\
		../../../ServerExtern-FreeBSD/lib/libjpeg.a

# MySQL
LIBS += /usr/local/lib/mysql/libmysqlclient.a /usr/lib/libz.a /usr/local/lib/libzstd.a

# Miscellaneous external libraries
INCDIR += -I../../../ServerExtern-FreeBSD/include
LIBDIR += -L../../../ServerExtern-FreeBSD/lib
LIBS += -lcryptopp

# openssl
INCDIR += -I/usr/include
LIBDIR += -L/usr/local/lib
LIBS += -lunwind -lssl -lcrypto

# Project Library
INCDIR += -I../../liblua/include
INCDIR += -I/usr/local/include
LIBDIR += -L../../libthecore/lib -L../../libpoly -L../../libsql -L../../libgame/lib -L../../liblua/lib
LIBDIR += -L/usr/local/lib
LIBS += -lthecore -lpoly -llua -llualib -lsql -lgame

USE_STACKTRACE = 0
ifeq ($(USE_STACKTRACE), 1)
LIBS += /usr/local/lib/libexecinfo.a
endif

TARGET  = $(BINDIR)/game_$(PLATFORM)

CFILE	= minilzo.c

CPPFILE = FSM.cpp MarkConvert.cpp MarkImage.cpp MarkManager.cpp OXEvent.cpp ani.cpp\
		  arena.cpp banword.cpp battle.cpp blend_item.cpp buffer_manager.cpp building.cpp\
		  char.cpp char_affect.cpp char_battle.cpp char_change_empire.cpp char_horse.cpp char_item.cpp char_manager.cpp\
		  char_quickslot.cpp char_resist.cpp char_skill.cpp char_state.cpp PetSystem.cpp cmd.cpp cmd_emotion.cpp cmd_general.cpp\
		  cmd_gm.cpp cmd_oxevent.cpp config.cpp constants.cpp crc32.cpp cube.cpp db.cpp desc.cpp\
		  desc_client.cpp desc_manager.cpp desc_p2p.cpp dungeon.cpp empire_text_convert.cpp entity.cpp\
		  entity_view.cpp event.cpp event_queue.cpp exchange.cpp file_loader.cpp fishing.cpp gm.cpp guild.cpp\
		  guild_manager.cpp guild_war.cpp horse_rider.cpp horsename_manager.cpp input.cpp input_auth.cpp input_db.cpp\
		  input_login.cpp input_main.cpp input_p2p.cpp\
		  item.cpp item_addon.cpp item_attribute.cpp item_manager.cpp item_manager_idrange.cpp locale.cpp\
		  locale_service.cpp log.cpp login_data.cpp lzo_manager.cpp marriage.cpp\
		  messenger_manager.cpp mining.cpp mob_manager.cpp motion.cpp p2p.cpp packet_info.cpp\
		  party.cpp polymorph.cpp priv_manager.cpp pvp.cpp\
		  questevent.cpp questlua.cpp questlua_affect.cpp questlua_arena.cpp questlua_building.cpp\
		  questlua_danceevent.cpp questlua_dungeon.cpp questlua_game.cpp questlua_global.cpp\
		  questlua_guild.cpp questlua_horse.cpp questlua_pet.cpp questlua_item.cpp questlua_marriage.cpp\
		  questlua_npc.cpp questlua_oxevent.cpp questlua_party.cpp questlua_pc.cpp\
		  questlua_quest.cpp questlua_target.cpp questmanager.cpp questnpc.cpp questpc.cpp\
		  refine.cpp regen.cpp safebox.cpp sectree.cpp sectree_manager.cpp shop.cpp\
		  skill.cpp start_position.cpp target.cpp text_file_loader.cpp trigger.cpp utils.cpp vector.cpp war_map.cpp\
		  wedding.cpp xmas_event.cpp panama.cpp map_location.cpp\
		  BlueDragon.cpp BlueDragon_Binder.cpp DragonLair.cpp questlua_dragonlair.cpp\
		  skill_power.cpp affect.cpp\
		  ClientPackageCryptInfo.cpp cipher.cpp\
		  buff_on_attributes.cpp dragon_soul_table.cpp DragonSoul.cpp\
		  group_text_parse_tree.cpp char_dragonsoul.cpp questlua_dragonsoul.cpp\
		  shop_manager.cpp shopEx.cpp item_manager_read_tables.cpp char_aura.cpp item_apply_random_table.cpp loot_filter.cpp MountSystem.cpp questlua_shipdefense_mgr.cpp\
		  ShipDefense.cpp Transmutation.cpp


COBJS	= $(CFILE:%.c=$(OBJDIR)/%.o)
CPPOBJS	= $(CPPFILE:%.cpp=$(OBJDIR)/%.o)

MAINOBJ = $(OBJDIR)/main.o
MAINCPP = main.cpp

default: $(TARGET)

$(OBJDIR)/minilzo.o: minilzo.c
	@$(CC) $(CFLAGS) $(INCDIR) -c $< -o $@
	@echo -e "\033[0;32m [---] \033[0m	   \033[0;33m Compiling:\033[0m" $<

$(OBJDIR)/%.o: %.cpp
	@echo -e "\033[0;32m [---] \033[0m	   \033[0;33m Compiling:\033[0m" $<
	@$(CC) $(CFLAGS) $(INCDIR) -c $< -o $@

$(TARGET): $(CPPOBJS) $(COBJS) $(MAINOBJ)
	@echo -e "\033[0;32m [---] \033[0m	   \033[0;33m Linking:\033[0m" $(TARGET)
	@$(CC) $(CFLAGS) $(LIBDIR) $(COBJS) $(CPPOBJS) $(MAINOBJ) $(LIBS) -o $(TARGET)

clean:
	@rm -f $(COBJS) $(CPPOBJS)
	@rm -f $(BINDIR)/game_* $(BINDIR)/conv

tag:
	ctags *.cpp *.h *.c

dep:
	makedepend -f Depend $(INCDIR) -p$(OBJDIR)/ $(CPPFILE) $(CFILE) $(MAINCPP) 2> /dev/null > Depend

sinclude Depend
