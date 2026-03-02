-- ########################################################################################
-- ### MARTY LUA LIBRARY FOR ASF_PROJECT
-- ########################################################################################
if QUESTLIB_EXTRA_LUA ~= nil then return end -- include_guard begin
QUESTLIB_EXTRA_LUA=2.1

ENABLE_AUTOSET_EVENT_FLAGS = false -- if true, set the event flags automatically at startup

-- POINT DECLARING BEGIN
POINT_RAMADAN_CANDY_BONUS_EXP = 127
POINT_ENERGY = 128
POINT_ENERGY_END_TIME = 129

POINT_COSTUME_ATTR_BONUS = 130
POINT_MAGIC_ATT_BONUS_PER = 131
POINT_MELEE_MAGIC_ATT_BONUS_PER = 132

POINT_RESIST_ICE = 133
POINT_RESIST_EARTH = 134
POINT_RESIST_DARK = 135

POINT_RESIST_CRITICAL = 136
POINT_RESIST_PENETRATE = 137

POINT_BLEEDING_REDUCE = 138
POINT_BLEEDING_PCT = 139

POINT_ATTBONUS_WOLFMAN = 140
POINT_RESIST_WOLFMAN = 141
POINT_RESIST_CLAW = 142

POINT_ACCEDRAIN_RATE = 143
POINT_RESIST_MAGIC_REDUCTION = 144
-- POINT DECLARING END

-- RACE DECLARING BEGIN
WARRIOR_M	= 0
NINJA_W		= 1
SURA_M		= 2
SHAMAN_W	= 3
WARRIOR_W	= 4
NINJA_M		= 5
SURA_W		= 6
SHAMAN_M	= 7
WOLFMAN_M	= 8
-- RACE DECLARING END
-- ########################################################################################

-- ########################################################################################
-- AFF DECLARING BEGIN
AFF_NONE = 0

AFF_YMIR = 1
AFF_INVISIBILITY = 2
AFF_SPAWN = 3

AFF_POISON = 4
AFF_SLOW = 5
AFF_STUN = 6

AFF_DUNGEON_READY = 7
AFF_DUNGEON_UNIQUE = 8

AFF_BUILDING_CONSTRUCTION_SMALL = 9
AFF_BUILDING_CONSTRUCTION_LARGE = 10
AFF_BUILDING_UPGRADE = 11

AFF_MOV_SPEED_POTION = 12
AFF_ATT_SPEED_POTION = 13

AFF_FISH_MIND = 14

AFF_JEONGWIHON = 15
AFF_GEOMGYEONG = 16
AFF_CHEONGEUN = 17
AFF_GYEONGGONG = 18
AFF_EUNHYUNG = 19
AFF_GWIGUM = 20
AFF_TERROR = 21
AFF_JUMAGAP = 22
AFF_HOSIN = 23
AFF_BOHO = 24
AFF_KWAESOK = 25
AFF_MANASHIELD = 26
AFF_MUYEONG = 27
AFF_REVIVE_INVISIBLE = 28
AFF_FIRE = 29
AFF_GICHEON = 30
AFF_JEUNGRYEOK = 31
AFF_TANHWAN_DASH = 32
AFF_PABEOP = 33
AFF_CHEONGEUN_WITH_FALL = 34
AFF_POLYMORPH = 35
AFF_WAR_FLAG1 = 36
AFF_WAR_FLAG2 = 37
AFF_WAR_FLAG3 = 38

AFF_CHINA_FIREWORK = 39
AFF_HAIR = 40
AFF_GERMANY =  41

AFF_RAMADAN_RING =  42

AFF_BLEEDING =  43
AFF_RED_POSSESSION =  44
AFF_BLUE_POSSESSION =  45

AFF_BITS_MAX = 46
-- AFF DECLARING END
-- ########################################################################################

-- ########################################################################################
-- C3 NEMERE BEGIN
function setDFR_from_table(antiflagList, vid)
	for i,v in ipairs(antiflagList) do
		d.set_damage_from_race(v, vid)
	end
end
SetDamageFromRaceFromTable = setDFR_from_table

function setDFA_from_table(antiflagList, vid)
	for i,v in ipairs(antiflagList) do
		d.set_damage_from_affect(v, vid)
	end
end
SetDamageFromAffectFromTable = setDFR_from_table

-- 6151
Szel_4thfloor_affectlimit	= {AFF_HOSIN}
Szel_7thfloor_affectlimit	= {AFF_GICHEON}
-- 8058
IceMetin_racelimit			= {WARRIOR_M,WARRIOR_W,NINJA_W,NINJA_M,SURA_M,SURA_W}
-- 20399
IceStonePillar_racelimit	= {WARRIOR_M,WARRIOR_W,SURA_M,SURA_W}
-- C3 NEMERE END
-- ########################################################################################

-- ########################################################################################
-- AUTO CHECK EVENT FLAGS AT STARTUP BEGIN
function check_event_flags()
	local event_table = {
		{"gold_drop_limit_time", 1},
		{"item_drop_limit_time", 1},
		{"box_use_limit_time", 1},
		{"buysell_limit_time", 1},
		{"no_drop_metin_stone", 1},
		{"no_mount_at_guild_war", 1},
		{"no_potions_on_pvp", 1},
	}
	for num1,str1 in ipairs(event_table) do
		if game.get_event_flag(str1[1])~=str1[2] then
			game.set_event_flag(str1[1], str1[2])
		end
	end
end
if ENABLE_AUTOSET_EVENT_FLAGS then
	check_event_flags()
end
-- AUTO CHECK EVENT FLAGS AT STARTUP END
-- ########################################################################################

-- EXTENDED FUNCTIONS BEGIN

function TimeFormat(seconds)
	if seconds >= 3600 then
		return  string.format("%dh %dm %ds", seconds/3600, math.floor(math.mod(seconds/60,60)), math.floor(math.mod(seconds,60)))
    elseif seconds >= 60 then
		return  string.format("%dm %ds", seconds/60, math.floor(math.mod(seconds,60)))
    else
        return string.format("%ds", seconds)
    end
end

-- from https://metin2.dev/topic/13247-colored-quest-scrolls-system-v2/
function send_letter_ex_new(name, icon_type, icon_name) make_quest_button_ex(name, icon_type, icon_name) set_skin(NOWINDOW) q.set_title(name) q.set_icon(icon_name) q.start() end
function resend_letter_ex(name, icon_type, icon_name) make_quest_button_ex(name, icon_type, icon_name) q.set_title(name) q.set_icon(icon_name) q.start() end
function resend_letter(title) makequestbutton(title) q.set_title(title) q.start() end

-- from https://metin2.dev/topic/4295-color-chat-text/?do=findComment&comment=27882
function say_blue(name) say(color256(0, 0, 255) .. name .. color256(0, 0, 255)) end
function say_red(name) say(color256(255, 0, 0) .. name .. color256(255, 0, 0)) end
function say_green(name) say(color256(0, 238, 0) .. name .. color256(0, 238, 0)) end
function say_gold(name) say(color256(255, 215, 0) .. name .. color256(255, 215, 0)) end
function say_black(name) say(color256(0, 0, 0) .. name .. color256(0, 0, 0)) end
function say_white(name) say(color256(255, 255, 255) .. name .. color256(255, 255, 255)) end
function say_yellow(name) say(color256(255, 255, 0) .. name .. color256(255, 255, 0)) end
function say_blue2(name) say(color256(0, 206, 209) .. name .. color256(0, 206, 209)) end
-- EXTENDED FUNCTIONS END
