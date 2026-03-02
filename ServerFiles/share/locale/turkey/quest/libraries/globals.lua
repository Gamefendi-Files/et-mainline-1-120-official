-- https://metin2.dev/topic/29230-syreldars-useful-globals/
NOWINDOW  = 0;
NORMAL    = 1;
SCROLL    = 4;
CINEMATIC = 5;

CONFIRM_NO      = 0;
CONFIRM_YES     = 1;
CONFIRM_OK      = CONFIRM_YES;
CONFIRM_TIMEOUT = 2;

NO_SKILL_GROUP             = 0;
BASE_SKILL_LEVEL           = 1;
MASTER_SKILL_LEVEL         = 20;
GRAND_MASTER_SKILL_LEVEL   = 30;
PERFECT_MASTER_SKILL_LEVEL = 40;

MALE   = 0;
FEMALE = 1;

DOCTRINE_1 = 1;
DOCTRINE_2 = 2;

WARRIOR_BODY_FORCE   = DOCTRINE_1;
WARRIOR_MENTAL_FIGHT = DOCTRINE_2;

NINJA_BLADE_FIGHT    = DOCTRINE_1;
NINJA_ARCHERY        = DOCTRINE_2;

SURA_WEAPONRY        = DOCTRINE_1;
SURA_BLACK_MAGIC     = DOCTRINE_2;

SHAMAN_DRAGON_FORCE  = DOCTRINE_1;
SHAMAN_HEALING_FORCE = DOCTRINE_2;

LYCAN_INSTINCT       = DOCTRINE_1;

--** Essentially, pc.get_job()'s output.
WARRIOR  = 0;
NINJA    = 1;
ASSASSIN = NINJA;
SURA     = 2;
SHAMAN   = 3;
LYCAN    = 4;
WOLFMAN  = LYCAN;

--** Essentially, pc.get_race()'s output.
WARRIOR_M  = 0;
NINJA_W	   = 1;
ASSASSIN_W = NINJA_W;
SURA_M	   = 2;
SHAMAN_W   = 3;
WARRIOR_W  = 4;
NINJA_M    = 5;
ASSASSIN_M = NINJA_M;
SURA_W     = 6;
SHAMAN_M   = 7;
LYCAN_M    = 8;
WOLFMAN_M  = LYCAN_M;

PART_MAIN        = 0;
PART_WEAPON      = 1;
PART_HEAD        = 2;
PART_WEAPON_LEFT = 3;
PART_HAIR        = 4;
PART_ACCE        = 5;
PART_AURA        = 6;

WEAPON_SWORD       = 0;
WEAPON_DAGGER      = 1;
WEAPON_BOW         = 2;
WEAPON_TWO_HANDED  = 3;
WEAPON_BELL        = 4;
WEAPON_FAN         = 5;
WEAPON_ARROW       = 6;
WEAPON_MOUNT_SPEAR = 7;
WEAPON_CLAW        = 8;

APPLY = {
	["MAX_HP"] = 1,
	["MAX_SP"] = 2,
	["CON"] = 3,
	["INT"] = 4,
	["STR"] = 5,
	["DEX"] = 6,
	["ATT_SPEED"] = 7,
	["MOV_SPEED"] = 8,
	["CAST_SPEED"] = 9,
	["HP_REGEN"] = 10,
	["SP_REGEN"] = 11,
	["POISON_PCT"] = 12,
	["STUN_PCT"] = 13,
	["SLOW_PCT"] = 14,
	["CRITICAL_PCT"] = 15,
	["PENETRATE_PCT"] = 16,
	["ATTBONUS_HUMAN"] = 17,
	["ATTBONUS_ANIMAL"] = 18,
	["ATTBONUS_ORC"] = 19,
	["ATTBONUS_MILGYO"] = 20,
	["ATTBONUS_UNDEAD"] = 21,
	["ATTBONUS_DEVIL"] = 22,
	["STEAL_HP"] = 23,
	["STEAL_SP"] = 24,
	["MANA_BURN_PCT"] = 25,
	["DAMAGE_SP_RECOVER"] = 26,
	["BLOCK"] = 27,
	["DODGE"] = 28,
	["RESIST_SWORD"] = 29,
	["RESIST_TWOHAND"] = 30,
	["RESIST_DAGGER"] = 31,
	["RESIST_BELL"] = 32,
	["RESIST_FAN"] = 33,
	["RESIST_BOW"] = 34,
	["RESIST_FIRE"] = 35,
	["RESIST_ELEC"] = 36,
	["RESIST_MAGIC"] = 37,
	["RESIST_WIND"] = 38,
	["REFLECT_MELEE"] = 39,
	["REFLECT_CURSE"] = 40,
	["POISON_REDUCE"] = 41,
	["KILL_SP_RECOVER"] = 42,
	["EXP_DOUBLE_BONUS"] = 43,
	["GOLD_DOUBLE_BONUS"] = 44,
	["ITEM_DROP_BONUS"] = 45,
	["POTION_BONUS"] = 46,
	["KILL_HP_RECOVER"] = 47,
	["IMMUNE_STUN"] = 48,
	["IMMUNE_SLOW"] = 49,
	["IMMUNE_FALL"] = 50,
	["SKILL"] = 51,
	["BOW_DISTANCE"] = 52,
	["ATT_GRADE_BONUS"] = 53,
	["DEF_GRADE_BONUS"] = 54,
	["MAGIC_ATT_GRADE"] = 55,
	["MAGIC_DEF_GRADE"] = 56,
	["CURSE_PCT"] = 57,
	["MAX_STAMINA"] = 58,
	["ATTBONUS_WARRIOR"] = 59,
	["ATTBONUS_ASSASSIN"] = 60,
	["ATTBONUS_SURA"] = 61,
	["ATTBONUS_SHAMAN"] = 62,
	["ATTBONUS_MONSTER"] = 63,
	["MALL_ATTBONUS"] = 64,
	["MALL_DEFBONUS"] = 65,
	["MALL_EXPBONUS"] = 66,
	["MALL_ITEMBONUS"] = 67,
	["MALL_GOLDBONUS"] = 68,
	["MAX_HP_PCT"] = 69,
	["MAX_SP_PCT"] = 70,
	["SKILL_DAMAGE_BONUS"] = 71,
	["NORMAL_HIT_DAMAGE_BONUS"] = 72,
	["SKILL_DEFEND_BONUS"] = 73,
	["NORMAL_HIT_DEFEND_BONUS"] = 74,
	["PC_BANG_EXP_BONUS"] = 75,
	["PC_BANG_DROP_BONUS"] = 76,
	["EXTRACT_HP_PCT"] = 77,
	["RESIST_WARRIOR"] = 78,
	["RESIST_ASSASSIN"] = 79,
	["RESIST_SURA"] = 80,
	["RESIST_SHAMAN"] = 81,
	["ENERGY"] = 82,
	["DEF_GRADE"] = 83,
	["COSTUME_ATTR_BONUS"] = 84,
	["MAGIC_ATTBONUS_PER"] = 85,
	["MELEE_MAGIC_ATTBONUS_PER"] = 86,
	["RESIST_ICE"] = 87,
	["RESIST_EARTH"] = 88,
	["RESIST_DARK"] = 89,
	["ANTI_CRITICAL_PCT"] = 90,
	["ANTI_PENETRATE_PCT"] = 91,
	["BLEEDING_REDUCE"] = 92,
	["BLEEDING_PCT"] = 93,
	["ATTBONUS_WOLFMAN"] = 94,
	["RESIST_WOLFMAN"] = 95,
	["RESIST_CLAW"] = 96,
};

APPLY_NAME = {
	[1] = "Max HP",
	[2] = "Max MP",
	[3] = "Vitality",
	[4] = "Intelligence",
	[5] = "Strength",
	[6] = "Dexterity",
	[7] = "Attack Speed",
	[8] = "Movement Speed",
	[9] = "Casting Speed",
	[10] = "HP Regeneration",
	[11] = "MP Regeneration",
	[12] = "Poisoning chance",
	[13] = "Blackout chance",
	[14] = "Slowing chance",
	[15] = "Chance of critical hit",
	[16] = "Chance of piercing hit",
	[17] = "Strong against Half Humans",
	[18] = "Strong against Animals",
	[19] = "Strong against Orcs",
	[20] = "Strong against Mystics",
	[21] = "Strong against Undead",
	[22] = "Strong against Devils",
	[23] = "damage will be absorbed by HP",
	[24] = "damage will be absorbed by MP",
	[25] = "chance to rob SP",
	[27] = "Chance to block a close-combat attack",
	[28] = "Chance to avoid Arrows",
	[29] = "Sword Defence",
	[30] = "Two-Handed Defence",
	[31] = "Dagger Defence",
	[32] = "Bell Defence",
	[33] = "Fan Defence",
	[34] = "Arrow Defence",
	[35] = "Fire Resistance",
	[36] = "Lightning Resistance",
	[37] = "Magic Resistance",
	[38] = "Wind Resistance",
	[39] = "Chance to reflect a close combat attack",
	[41] = "Poisoning Resistance",
	[43] = "Chance for EXP Bonus",
	[44] = "Chance to drop double Yang",
	[45] = "Chance to drop double the Items",
	[47] = "Chance to restore HP",
	[48] = "Defence against blackouts",
	[49] = "Defence against slowing",
	[53] = "Attack Value",
	[54] = "Defence",
	[59] = "Strong against Warriors",
	[60] = "Strong against Ninjas",
	[61] = "Strong against Sura",
	[62] = "Strong against Shamans",
	[63] = "Strong against monsters",
	[71] = "Skill Damage",
	[72] = "Average Damage",
	[73] = "Skill Damage Resistance",
	[74] = "Average Damage Resistance",
	[78] = "Defence chance against warrior attacks",
	[79] = "Defence chance against ninja attacks",
	[80] = "Defence chance against sura attacks",
	[81] = "Defence chance against shaman attacks",
	[82] = "Energy",
	[87] = "Ice Resistance",
	[88] = "Earth Resistance",
	[89] = "Resistance against darkness",
	[90] = "Resistance against critical hits",
	[91] = "Resistance against piercing hits",
	[92] = "Resistance against bleeding attacks",
	[93] = "Bleeding chance",
	[94] = "Strong against lycans",
	[95] = "Defence chance against lycan attacks",
	[96] = "Claw Resistance",
};

APPLY_NAME_ARG = {
	[1] = "Max HP +%d",
	[2] = "Max MP +%d",
	[3] = "Vitality +%d",
	[4] = "Intelligence +%d",
	[5] = "Strength +%d",
	[6] = "Dexterity +%d",
	[7] = "Attack Speed +%d%%",
	[8] = "Movement Speed +%d%%",
	[9] = "Casting Speed +%d%%",
	[10] = "HP Regeneration +%d%%",
	[11] = "MP Regeneration +%d%%",
	[12] = "Poisoning chance +%d%%",
	[13] = "Blackout chance +%d%%",
	[14] = "Slowing chance +%d%%",
	[15] = "Chance of critical hit +%d%%",
	[16] = "Chance of piercing hit +%d%%",
	[17] = "Strong against Half Humans +%d%%",
	[18] = "Strong against Animals +%d%%",
	[19] = "Strong against Orcs +%d%%",
	[20] = "Strong against Mystics +%d%%",
	[21] = "Strong against Undead +%d%%",
	[22] = "Strong against Devils +%d%%",
	[23] = "+%d%% damage will be absorbed by HP",
	[24] = "+%d%% damage will be absorbed by MP",
	[25] = "+%d%% chance to rob SP",
	[27] = "Chance to block a close-combat attack +%d%%",
	[28] = "Chance to avoid Arrows +%d%%",
	[29] = "Sword Defence +%d%%",
	[30] = "Two-Handed Defence +%d%%",
	[31] = "Dagger Defence +%d%%",
	[32] = "Bell Defence +%d%%",
	[33] = "Fan Defence +%d%%",
	[34] = "Arrow Defence +%d%%",
	[35] = "Fire Resistance +%d%%",
	[36] = "Lightning Resistance +%d%%",
	[37] = "Magic Resistance +%d%%",
	[38] = "Wind Resistance +%d%%",
	[39] = "Chance to reflect a close combat attack: +%d%%",
	[41] = "Poisoning Resistance +%d%%",
	[43] = "+%d%% Chance for EXP Bonus",
	[44] = "+%d%% Chance to drop double Yang",
	[45] = "+%d%% Chance to drop double the Items",
	[47] = "+%d%% Chance to restore HP",
	[48] = "Defence against blackouts",
	[49] = "Defence against slowing",
	[53] = "Attack Value +%d",
	[54] = "Defence +%d",
	[59] = "Strong against Warriors +%d%%",
	[60] = "Strong against Ninjas +%d%%",
	[61] = "Strong against Sura +%d%%",
	[62] = "Strong against Shamans +%d%%",
	[63] = "Strong against monsters +%d%%",
	[71] = "Skill Damage +%d%%",
	[72] = "Average Damage +%d%%",
	[73] = "Skill Damage Resistance +%d%%",
	[74] = "Average Damage Resistance +%d%%",
	[78] = "Defence chance against warrior attacks +%d%%",
	[79] = "Defence chance against ninja attacks +%d%%",
	[80] = "Defence chance against sura attacks +%d%%",
	[81] = "Defence chance against shaman attacks +%d%%",
	[82] = "Energy +%d",
	[87] = "Ice Resistance +%d%%",
	[88] = "Earth Resistance +%d%%",
	[89] = "Resistance against darkness +%d%%",
	[90] = "Resistance against critical hits +%d%%",
	[91] = "Resistance against piercing hits +%d%%",
	[92] = "Resistance against bleeding attacks +%d%%",
	[93] = "Bleeding chance +%d%%",
	[94] = "Strong against lycans +%d%%",
	[95] = "Defence chance against lycan attacks +%d%%",
	[96] = "Claw Resistance +%d%%",
};

--[[
	The list of all the normal skills for each race and class.
]]
ACTIVE_SKILL_LIST = {
	--** Warrior
	[WARRIOR] = {
		-- Body
		[WARRIOR_BODY_FORCE] = {1, 2, 3, 4, 5, 6},
		-- Mental
		[WARRIOR_MENTAL_FIGHT] = {16, 17, 18, 19, 20, 21}
	},

	--** Ninja
	[NINJA] = {
		-- Blade Fight
		[NINJA_BLADE_FIGHT] = {31, 32, 33, 34, 35, 36},
		-- Archery
		[NINJA_ARCHERY] = {46, 47, 48, 49, 50, 51}
	},

	--** Sura
	[SURA] = {
		-- Weaponary
		[SURA_WEAPONRY] = {61, 62, 63, 64, 65, 66},
		-- Black Magic
		[SURA_BLACK_MAGIC] = {76, 77, 78, 79, 80, 81}
	},

	--** Shaman
	[SHAMAN] = {
		-- Dragon Power
		[SHAMAN_DRAGON_FORCE] = {91, 92, 93, 94, 95, 96},
		-- Healing Power
		[SHAMAN_HEALING_FORCE] = {106, 107, 108, 109, 110, 111}
	},

	--** Lycan
	[LYCAN] = {
		-- Instinct
		[LYCAN_INSTINCT] = {170, 171, 172, 173, 174, 175}
	}
};

--[[
	The list of all the yohara skills for each race and class.
]]
YOHARA_ACTIVE_SKILL_LIST = {
	--** Warrior
	[WARRIOR] = {
		-- Body
		[WARRIOR_BODY_FORCE] = {176},
		-- Mental
		[WARRIOR_MENTAL_FIGHT] = {176}
	},

	--** Ninja
	[NINJA] = {
		-- Blade Fight
		[NINJA_BLADE_FIGHT] = {177},
		-- Archery
		[NINJA_ARCHERY] = {178}
	},

	--** Sura
	[SURA] = {
		-- Weaponary
		[SURA_WEAPONRY] = {179},
		-- Black Magic
		[SURA_BLACK_MAGIC] = {180}
	},

	--** Shaman
	[SHAMAN] = {
		-- Dragon Power
		[SHAMAN_DRAGON_FORCE] = {181},
		-- Healing Power
		[SHAMAN_HEALING_FORCE] = {182}
	},

	--** Lycan
	[LYCAN] = {
		-- Instinct
		[LYCAN_INSTINCT] = {183}
	}
};

--[[
	The list of race names based on sex. (In english they don't change)
]]
RACE_NAME_LIST = {
	[WARRIOR] = {
		[MALE] = "Warrior",
		[FEMALE] = "Warrior"
	},
	[NINJA] = {
		[MALE] = "Ninja",
		[FEMALE] = "Ninja"
	},
	[SURA] = {
		[MALE] = "Sura",
		[FEMALE] = "Sura"
	},
	[SHAMAN] = {
		[MALE] = "Shaman",
		[FEMALE] = "Shaman"
	},
	[LYCAN] = {
		[MALE] = "Lycan",
		[FEMALE] = "Lycan"
	}
};

--[[
	The list of class names.
]]
DOCTRINE_NAME_LIST = {
	[WARRIOR] = {
		[WARRIOR_BODY_FORCE] = "Body-Force",
		[WARRIOR_MENTAL_FIGHT] = "Mental-Fight"
	},
	[NINJA] = {
		[NINJA_BLADE_FIGHT] = "Blade-Fight",
		[NINJA_ARCHERY] = "Archery"
	},
	[SURA] = {
		[SURA_WEAPONRY] = "Weaponry",
		[SURA_BLACK_MAGIC] = "Black Magic"
	},
	[SHAMAN] = {
		[SHAMAN_DRAGON_FORCE] = "Dragon Force",
		[SHAMAN_HEALING_FORCE] = "Healing Force"
	},
	[LYCAN] = {
		[LYCAN_INSTINCT] = "Instinct",
	}
};

--[[
	The list of the names of the skills for each race and class.
]]
SKILL_NAME_TABLE = {
	--** Body Warrior
	[1] = "Three-Way Cut",
	[2] = "Sword Spin",
	[3] = "Berserk",
	[4] = "Aura of the Sword",
	[5] = "Dash",
	[6] = "Life Force",
	[176] = "Earthquake",

	--** Mental Warrior
	[16] = "Spirit Strike (W)",
	[17] = "Bash",
	[18] = "Stump",
	[19] = "Strong Body",
	[20] = "Sword Strike",
	[21] = "Sword Orb",
	--[176] = "Earthquake", -- duplicate, it's the same for both warrior classes

	--** Blade Fight Ninja
	[31] = "Ambush",
	[32] = "Fast Attack",
	[33] = "Rolling Dagger",
	[34] = "Stealth",
	[35] = "Poisonous Cloud",
	[36] = "Insidious Poison",
	[177] = "Astral Light",

	--** Archery Ninja
	[46] = "Repetitive Shot",
	[47] = "Arrow Shower",
	[48] = "Fire Arrow",
	[49] = "Feather Walk",
	[50] = "Poison Arrow",
	[51] = "Spark",
	[178] = "Storm Shot",

	--** Weaponry Sura
	[61] = "Finger Strike",
	[62] = "Dragon Swirl",
	[63] = "Enchanted Blade",
	[64] = "Fear",
	[65] = "Enchanted Armour",
	[66] = "Dispel",
	[179] = "Hell Strike",

	--** Black Magic Sura
	[76] = "Dark Strike",
	[77] = "Flame Strike",
	[78] = "Flame Spirit",
	[79] = "Dark Protection",
	[80] = "Spirit Strike (S)",
	[81] = "Dark Orb",
	[180] = "Death Wave",

	--** Dragon Force Shaman
	[91] = "Flying Talisman",
	[92] = "Shooting Dragon",
	[93] = "Dragon's Roar",
	[94] = "Blessing",
	[95] = "Reflect",
	[96] = "Dragon's Aid",
	[181] = "Meteor",

	--** Healing Force Shaman
	[106] = "Lightning Throw",
	[107] = "Summon Lightning",
	[108] = "Lightning Claw",
	[109] = "Cure",
	[110] = "Swiftness",
	[111] = "Attack Up",
	[182] = "Aether Ward",

	--** Instinct Lycan
	[170] = "Shred",
	[171] = "Wolf's Breath",
	[172] = "Wolf Pounce",
	[173] = "Wolf's Claw",
	[174] = "Crimson Wolf Soul",
	[175] = "Indigo Wolf Soul",
	[183] = "Talon Storm",
};

--[[
	The list of the names for each empire.
]]
EMPIRE_NAME_LIST = {
	[1] = "Shinsoo",
	[2] = "Chunjo",
	[3] = "Jinno"
};

--[[
	A list of colors. You can use color_say from my quest_functions list to easily use it.
]]
COLORS = {
	["aliceblue"] = {240, 248, 255},     ["antiquewhite"] = {250, 235, 215},    ["aqua"] = {0, 255, 255},                   ["aquamarine"] = {127, 255, 212},
	["azure"] = {240, 255, 255},         ["beige"] = {245, 245, 220},           ["bisque"] = {255, 228, 196},               ["black"] = {0, 0, 0},
	["bronze"] = {205, 127, 50},         ["nabb_blue"] = {63, 108, 191},
	["blanchedalmond"] = {255, 235, 205},["blue"] = {0, 0, 255},                ["blueviolet"] = {138, 43, 226},            ["brown"] = {165, 42, 42},
	["burlywood"] = {222, 184, 135},     ["cadetblue"] = {95, 158, 160},        ["chartreuse"] = {127, 255, 0},             ["chocolate"] = {210, 105, 30},
	["coral"] = {255, 127, 80},          ["cornflowerblue"] = {100, 149, 237},  ["cornsilk"] = {255, 248, 220},             ["crimson"] = {220, 20, 60},
	["cyan"] = {0, 255, 255},            ["darkblue"] = {0, 0, 139},            ["darkcyan"] = {0, 139, 139},               ["darkgoldenrod"] = {184, 134, 11},
	["darkgray"] = {169, 169, 169},      ["darkgreen"] = {0, 100, 0},           ["darkkhaki"] = {189, 183, 107},            ["darkmagenta"] = {139, 0, 139},
	["darkolivegreen"] = {85, 107, 47},  ["darkorange"] = {255, 140, 0},        ["darkorchid"] = {153, 50, 204},            ["darkred"] = {139, 0, 0},
	["darksalmon"] = {233, 150, 122},    ["darkseagreen"] = {143, 188, 139},    ["darkslateblue"] = {72, 61, 139},          ["darkslategray"] = {47, 79, 79},
	["darkturquoise"] = {0, 206, 209},   ["darkviolet"] = {148, 0, 211},        ["deeppink"] = {255, 20, 147},              ["deepskyblue"] = {0, 191, 255},
	["dimgray"] = {105, 105, 105},       ["dodgerblue"] = {30, 144, 255},       ["firebrick"] = {178, 34, 34},              ["floralwhite"] = {255, 250, 240},
	["forestgreen"] = {34, 139, 34},     ["fuchsia"] = {255, 0, 255},           ["gainsboro"] = {220, 220, 220},            ["ghostwhite"] = {248, 248, 255},
	["gold"] = {255, 215, 0},            ["goldenrod"] = {218, 165, 32},        ["gray"] = {128, 128, 128},                 ["green"] = {0, 128, 0},
	["greenyellow"] = {173, 255, 47},    ["honeydew"] = {240, 255, 240},        ["hotpink"] = {255, 105, 180},              ["indianred"] = {205, 92, 92},
	["indigo"] = {75, 0, 130},           ["ivory"] = {255, 255, 240},           ["khaki"] = {240, 230, 140},                ["lavender"] = {230, 230, 250},
	["lavenderblush"] = {255, 240, 245}, ["lawngreen"] = {124, 252, 0},         ["lemonchiffon"] = {255, 250, 205},         ["lightblue"] = {173, 216, 230},
	["lightcoral"] = {240, 128, 128},    ["lightcyan"] = {224, 255, 255},       ["lightgoldenrodyellow"] = {250, 250, 210}, ["lightgray"] = {211, 211, 211},
	["lightgreen"] = {144, 238, 144},    ["lightpink"] = {255, 182, 193},       ["lightsalmon"] = {255, 160, 122},          ["lightseagreen"] = {32, 178, 170},
	["lightskyblue"] = {135, 206, 250},  ["lightslategray"] = {119, 136, 153},  ["lightsteelblue"] = {176, 196, 222},       ["lightyellow"] = {255, 255, 224},
	["lime"] = {0, 255, 0},              ["limegreen"] = {50, 205, 50},         ["linen"] = {250, 240, 230},                ["magenta"] = {255, 0, 255},
	["maroon"] = {128, 0, 0},            ["mediumaquamarine"] = {102, 205, 170},["mediumblue"] = {0, 0, 205},               ["mediumorchid"] = {186, 85, 211},
	["mediumpurple"] = {147, 112, 219},  ["mediumseagreen"] = {60, 179, 113},   ["mediumslateblue"] = {123, 104, 238},      ["mediumspringgreen"] = {0, 250, 154},
	["mediumturquoise"] = {72, 209, 204},["mediumvioletred"] = {199, 21, 133},  ["midnightblue"] = {25, 25, 112},           ["mintcream"] = {245, 255, 250},
	["mistyrose"] = {255, 228, 225},     ["moccasin"] = {255, 228, 181},        ["navajowhite"] = {255, 222, 173},          ["navy"] = {0, 0, 128},
	["oldlace"] = {253, 245, 230},       ["olive"] = {128, 128, 0},             ["olivedrab"] = {107, 142, 35},             ["orange"] = {255, 165, 0},
	["orangered"] = {255, 69, 0},        ["orchid"] = {218, 112, 214},          ["palegoldenrod"] = {238, 232, 170},        ["palegreen"] = {152, 251, 152},
	["paleturquoise"] = {175, 238, 238}, ["palevioletred"] = {219, 112, 147},   ["papayawhip"] = {255, 239, 213},           ["peachpuff"] = {255, 218, 185},
	["peru"] = {205, 133, 63},           ["pink"] = {255, 192, 203},            ["plum"] = {221, 160, 221},                 ["powderblue"] = {176, 224, 230},
	["purple"] = {128, 0, 128},          ["red"] = {255, 0, 0},                 ["rosybrown"] = {188, 143, 143},            ["royalblue"] = {65, 105, 225},
	["saddlebrown"] = {139, 69, 19},     ["salmon"] = {250, 128, 114},          ["sandybrown"] = {244, 164, 96},            ["seagreen"] = {46, 139, 87},
	["seashell"] = {255, 245, 238},      ["sienna"] = {160, 82, 45},            ["silver"] = {192, 192, 192},               ["skyblue"] = {135, 206, 235},
	["slateblue"] = {106, 90, 205},      ["slategray"] = {112, 128, 144},       ["snow"] = {255, 250, 250},                 ["springgreen"] = {0, 255, 127},
	["steelblue"] = {70, 130, 180},      ["tan"] = {210, 180, 140},             ["teal"] = {0, 128, 128},                   ["thistle"] = {216, 191, 216},
	["tomato"] = {255, 99, 71},          ["turquoise"] = {64, 224, 208},        ["violet"] = {238, 130, 238},               ["wheat"] = {245, 222, 179},
	["white"] = {255, 255, 255},         ["whitesmoke"] = {245, 245, 245},      ["yellow"] = {255, 255, 0},                 ["yellowgreen"] = {154, 205, 50}
};

--[[
	The list of the map_ids and names. (Taken from official locale_name.txt)
]]
MAP_NAME_TABLE = {
	[200] = {["id"] = "gm_guild_build",                  ["name"] = "GM Guild Map"}, -- Unofficial name, not present in the locale, but it's commonly known as GM Guild Map.
	[64]  = {["id"] = "map_a2",                          ["name"] = "Valley of Seungryong"},
	[61]  = {["id"] = "map_n_snowm_01",                  ["name"] = "Mount Sohan"},
	[355] = {["id"] = "metin2_12zi_stage",               ["name"] = "Zodiac Temple"},
	[372] = {["id"] = "metin2_guild_pve",                ["name"] = "Palace of the Dragon King"},
	[6]   = {["id"] = "metin2_guild_village_01",         ["name"] = "Miryang"},
	[26]  = {["id"] = "metin2_guild_village_02",         ["name"] = "Songpa"},
	[46]  = {["id"] = "metin2_guild_village_03",         ["name"] = "Daeyami"},
	[130] = {["id"] = "metin2_guild_war1",               ["name"] = "Guild War Area"},
	[131] = {["id"] = "metin2_guild_war2",               ["name"] = "Guild War Area"},
	[132] = {["id"] = "metin2_guild_war3",               ["name"] = "Guild War Area"},
	[133] = {["id"] = "metin2_guild_war4",               ["name"] = "Guild War Area"},
	[1]   = {["id"] = "metin2_map_a1",                   ["name"] = "Yongan Area"},
	[3]   = {["id"] = "metin2_map_a3",                   ["name"] = "Yayang Area"},
	[21]  = {["id"] = "metin2_map_b1",                   ["name"] = "Joan Area"},
	[23]  = {["id"] = "metin2_map_b3",                   ["name"] = "Bokjung Area"},
	[357] = {["id"] = "metin2_map_battlefied",           ["name"] = "Combat Zone"},
	[303] = {["id"] = "metin2_map_BayBlackSand",         ["name"] = "Nephrite Bay"},
	[371] = {["id"] = "metin2_map_boss_awaken_dawnmist", ["name"] = "RX: Enchanted Forest"},
	[369] = {["id"] = "metin2_map_boss_awaken_flame",    ["name"] = "RX: Red Dragon Fortress"},
	[368] = {["id"] = "metin2_map_boss_awaken_skipia",   ["name"] = "RX: Grotto of Exile"},
	[370] = {["id"] = "metin2_map_boss_awaken_snow",     ["name"] = "RX: Nemere's Watchtower"},
	[367] = {["id"] = "metin2_map_boss_crack_dawnmist",  ["name"] = "TR: Enchanted Forest"},
	[365] = {["id"] = "metin2_map_boss_crack_flame",     ["name"] = "TR: Red Dragon Fortress"},
	[364] = {["id"] = "metin2_map_boss_crack_skipia",    ["name"] = "TR: Grotto of Exile"},
	[366] = {["id"] = "metin2_map_boss_crack_snow",      ["name"] = "TR: Nemere's Watchtower"},
	[41]  = {["id"] = "metin2_map_c1",                   ["name"] = "Pyungmoo Area"},
	[43]  = {["id"] = "metin2_map_c3",                   ["name"] = "Bakra Area"},
	[301] = {["id"] = "Metin2_map_CapeDragonHead",       ["name"] = "Cape Dragon Fire"},
	[353] = {["id"] = "metin2_map_dawnmist_dungeon_01",  ["name"] = "Enchanted Forest"},
	[302] = {["id"] = "metin2_map_dawnmistwood",         ["name"] = "Gautama Cliff"},
	[358] = {["id"] = "metin2_map_defensewave",          ["name"] = "Ship Defence"},
	[359] = {["id"] = "metin2_map_defensewave_port",     ["name"] = "Harbour"},
	[216] = {["id"] = "metin2_map_devilsCatacomb",       ["name"] = "Devil's Catacomb"},
	[66]  = {["id"] = "metin2_map_deviltower1",          ["name"] = "Demon Tower"},
	[112] = {["id"] = "metin2_map_duel",                 ["name"] = "Training Fight Arena"},
	[91]  = {["id"] = "metin2_map_e1",                   ["name"] = "Colony - Shinsoo"},
	[92]  = {["id"] = "metin2_map_e1",                   ["name"] = "Colony - Chunjo"},
	[93]  = {["id"] = "metin2_map_e1",                   ["name"] = "Colony - Jinno"},
	[4]   = {["id"] = "metin2_map_guild_01",             ["name"] = "Jungrang"},
	[24]  = {["id"] = "metin2_map_guild_02",             ["name"] = "Waryong"},
	[44]  = {["id"] = "metin2_map_guild_03",             ["name"] = "Imha"},
	[363] = {["id"] = "metin2_map_labyrinth",            ["name"] = "Plateau of Illusions"},
	[65]  = {["id"] = "metin2_map_milgyo",               ["name"] = "Hwang Temple"},
	[361] = {["id"] = "metin2_map_miniboss_01",          ["name"] = "Blue Dungeon - Lvl 40-80"},
	[362] = {["id"] = "metin2_map_miniboss_02",          ["name"] = "Blue Dungeon - Lvl 81+"},
	[360] = {["id"] = "metin2_map_mists_of_island",      ["name"] = "Isle of Mist"},
	[5]   = {["id"] = "metin2_map_monkeydungeon",        ["name"] = "Hasun Dong - Shinsoo"},
	[25]  = {["id"] = "metin2_map_monkeydungeon",        ["name"] = "Hasun Dong - Chunjo"},
	[45]  = {["id"] = "metin2_map_monkeydungeon",        ["name"] = "Hasun Dong - Jinno"},
	[107] = {["id"] = "metin2_map_monkeydungeon",        ["name"] = "Hasun Dong"},
	[108] = {["id"] = "metin2_map_monkeydungeon_02",     ["name"] = "Jungsun Dong"},
	[109] = {["id"] = "metin2_map_monkeydungeon_03",     ["name"] = "Sangsun Dong"},
	[354] = {["id"] = "metin2_map_Mt_Th_dungeon_01",     ["name"] = "Temple of the Ochao"},
	[304] = {["id"] = "metin2_map_Mt_Thunder",           ["name"] = "Thunder Mountains"},
	[63]  = {["id"] = "metin2_map_n_desert_01",          ["name"] = "Yongbi Desert"},
	[62]  = {["id"] = "metin2_map_n_flame_01",           ["name"] = "Fireland"},
	[356] = {["id"] = "metin2_map_n_flame_dragon",       ["name"] = "Meley's Lair"},
	[351] = {["id"] = "metin2_map_n_flame_dungeon_01",   ["name"] = "Red Dragon Fortress"},
	[352] = {["id"] = "metin2_map_n_snow_dungeon_01",    ["name"] = "Nemere's Watchtower"},
	[201] = {["id"] = "metin2_map_pvp_arena",            ["name"] = "PvP Arena"}, -- Unofficial name, not present in the locale but should be obvious.
	[73]  = {["id"] = "metin2_map_skipia_dungeon_02",    ["name"] = "Grotto of Exile 2"},
	[208] = {["id"] = "metin2_map_skipia_dungeon_boss",  ["name"] = "Dragon's Temple"},
	[104] = {["id"] = "metin2_map_spiderdungeon",        ["name"] = "Spider Dungeon 1"},
	[71]  = {["id"] = "metin2_map_spiderdungeon_02",     ["name"] = "Spider Dungeon 2"},
	[217] = {["id"] = "metin2_map_spiderdungeon_03",     ["name"] = "Spider Dungeon 3"},
	[103] = {["id"] = "metin2_map_t1",                   ["name"] = "Guild War Area"},
	[105] = {["id"] = "metin2_map_t2",                   ["name"] = "Guild War Area"},
	[110] = {["id"] = "metin2_map_t3",                   ["name"] = "Guild War Area"},
	[111] = {["id"] = "metin2_map_t4",                   ["name"] = "Guild War Area"},
	[67]  = {["id"] = "metin2_map_trent",                ["name"] = "Ghost Forest"},
	[68]  = {["id"] = "metin2_map_trent02",              ["name"] = "Red Forest"},
	[81]  = {["id"] = "metin2_map_wedding_01",           ["name"] = "Wedding Map"},
	[70]  = {["id"] = "season1/metin2_map_nusluck01",             ["name"] = "Land of Giants"},
	[113] = {["id"] = "season1/metin2_map_oxevent",               ["name"] = "OX contest"},
	[114] = {["id"] = "season1/metin2_map_sungzi",                ["name"] = "Kingdom Battle"},
	[125] = {["id"] = "season1/metin2_map_sungzi_desert_01",      ["name"] = "Desert Land Kingdom Battle Arena"},
	[126] = {["id"] = "season1/metin2_map_sungzi_desert_hill_01", ["name"] = "Desert Land Kingdom Battle - Shinsoo"},
	[127] = {["id"] = "season1/metin2_map_sungzi_desert_hill_02", ["name"] = "Desert Land Kingdom Battle - Chunjo"},
	[128] = {["id"] = "season1/metin2_map_sungzi_desert_hill_03", ["name"] = "Desert Land Kingdom Battle - Jinno"},
	[118] = {["id"] = "season1/metin2_map_sungzi_flame_hill_01",  ["name"] = "Fireland Kingdom Battle - Shinsoo"},
	[119] = {["id"] = "season1/metin2_map_sungzi_flame_hill_02",  ["name"] = "Fireland Kingdom Battle - Chunjo"},
	[120] = {["id"] = "season1/metin2_map_sungzi_flame_hill_03",  ["name"] = "Fireland Kingdom Battle - Jinno"},
	[121] = {["id"] = "season1/metin2_map_sungzi_snow",           ["name"] = "Ice Land Kingdom Battle Arena"},
	[122] = {["id"] = "season1/metin2_map_sungzi_snow_pass_01",   ["name"] = "Ice Land Kingdom Battle - Shinsoo"},
	[123] = {["id"] = "season1/metin2_map_sungzi_snow_pass_02",   ["name"] = "Ice Land Kingdom Battle - Chunjo"},
	[124] = {["id"] = "season1/metin2_map_sungzi_snow_pass_03",   ["name"] = "Ice Land Kingdom Battle - Jinno"},
	[69]  = {["id"] = "season1/metin2_map_WL_01",                 ["name"] = "Snakefield"},
	[181] = {["id"] = "season2/metin2_map_empirewar01",           ["name"] = "Castle Battle - Shinsoo"},
	[182] = {["id"] = "season2/metin2_map_empirewar02",           ["name"] = "Castle Battle - Chunjo"},
	[183] = {["id"] = "season2/metin2_map_empirewar03",           ["name"] = "Castle Battle - Jinno"},
	[72]  = {["id"] = "season2/metin2_map_skipia_dungeon_01",     ["name"] = "Grotto of Exile"},
	[378] = {["id"] = "metin2_map_elemental_01",         ["name"] = "Elemental Realm"},
	[379] = {["id"] = "metin2_map_elemental_02",         ["name"] = "Elemental Realm"},
	[380] = {["id"] = "metin2_map_elemental_03",         ["name"] = "Elemental Realm"},
	[381] = {["id"] = "metin2_map_elemental_04",         ["name"] = "Elemental Realm"},
	[374] = {["id"] = "metin2_map_empirecastle",         ["name"] = "Abandoned Fortress"},
	[373] = {["id"] = "metin2_map_eastplain_01",         ["name"] = "Dong Gwang Plain"},
	[376] = {["id"] = "metin2_map_eastplain_02",         ["name"] = "Seo Gwang Wastelands"},
	[377] = {["id"] = "metin2_map_eastplain_03",         ["name"] = "Nam Gwang Chasm"},
	[382] = {["id"] = "metin2_map_maze_dungeon1",        ["name"] = "Gnoll Caves Level 1"},
	[383] = {["id"] = "metin2_map_maze_dungeon2",        ["name"] = "Gnoll Caves Level 2"},
	[384] = {["id"] = "metin2_map_maze_dungeon3",        ["name"] = "Gnoll Caves Level 3"},
	[386] = {["id"] = "metin2_map_smhdungeon_01",        ["name"] = "Entrance Hall"},
	[387] = {["id"] = "metin2_map_smhdungeon_02",        ["name"] = "Sung Mahi Tower"},
	[375] = {["id"] = "metin2_map_battleroyale",         ["name"] = "Battle Royale"},
	[392] = {["id"] = "metin2_map_privateshop",          ["name"] = "Bazaar"},
	[396] = {["id"] = "metin2_map_anglar_dungeon_01",    ["name"] = "Mysterious Dungeon"},
	[385] = {["id"] = "metin2_map_snakevalley",          ["name"] = "Yilad Pass"},
	[390] = {["id"] = "metin2_map_snake_temple_01",      ["name"] = "1st Level of the Serpent Temple"},
	[391] = {["id"] = "metin2_map_snake_temple_02",      ["name"] = "Serpent Temple"},
	[388] = {["id"] = "metin2_map_icecrystalcave",       ["name"] = "Northwind Canyon"},
	[389] = {["id"] = "metin2_map_whitdragonvalley",     ["name"] = "Soul Gorge"},
	[393] = {["id"] = "metin2_map_whitedragoncave_01",   ["name"] = "Northwind Shelter"},
	[394] = {["id"] = "metin2_map_whitedragoncave_02",   ["name"] = "Northwind Descent"},
	[395] = {["id"] = "metin2_map_whitedragoncave_boss", ["name"] = "Northwind Deeps"},
	[399] = {["id"] = "metin2_map_secretdungeon_01",     ["name"] = "Enigma Temple"},
	[400] = {["id"] = "metin2_map_otherworld_01",        ["name"] = "Salvation"},
	[401] = {["id"] = "metin2_map_otherworld_02",        ["name"] = "Otherworld"},
	[402] = {["id"] = "metin2_map_otherworld_03",        ["name"] = "Room of Trials"},
	[403] = {["id"] = "metin2_map_otherworld_04",        ["name"] = "Damnation"},

	-- [[Scrapped Maps]]

	-- Scrapped Chunjo Orc Valley
	--[0] = {["id"] = "map_b2",                                  ["name"] = "Valley of Imji"},
	-- Scrapped Jinno Orc Valley
	--[0] = {["id"] = "map_c2",                                  ["name"] = "Valley of Bangsan"},
	-- Scrapped Battlefield map
	--[0] = {["id"] = "metin2_map_bf",                           ["name"] = "Battlefield"},
	-- Scrapped Battlefield map
	--[0] = {["id"] = "metin2_map_bf_02",                        ["name"] = "Battlefield 2"},
	-- Scrapped Battlefield map
	--[0] = {["id"] = "metin2_map_bf_03",                        ["name"] = "Battlefield 3"},
	-- Scrapped Fireland Kingdom Battle Passage map
	--[0] = {["id"] = "metin2_map_sungzi_flame_pass01",          ["name"] = "Fireland Kingdom Battle - Shinsoo"},
	-- Scrapped Fireland Kingdom Battle Passage map
	--[0] = {["id"] = "metin2_map_sungzi_flame_pass02",          ["name"] = "Fireland Kingdom Battle - Chunjo"},
	-- Scrapped Fireland Kingdom Battle Passage map
	--[0] = {["id"] = "metin2_map_sungzi_flame_pass03",          ["name"] = "Fireland Kingdom Battle - Jinno"},
	-- Scrapped Temple Hwang-Themed Kingdom Battle Arena
	--[0] = {["id"] = "season1/metin2_map_sungzi_milgyo",        ["name"] = "Temple Hwang Kingdom Battle Arena"},
	-- Scrapped Temple Hwang-Themed Kingdom Passage map
	--[0] = {["id"] = "season1/metin2_map_sungzi_milgyo_pass01", ["name"] = "Temple Hwang Kingdom Battle - Shinsoo"},
	-- Scrapped Temple Hwang-Themed Kingdom Passage map
	--[0] = {["id"] = "season1/metin2_map_sungzi_milgyo_pass02", ["name"] = "Temple Hwang Kingdom Battle - Chunjo"},
	-- Scrapped Temple Hwang-Themed Kingdom Passage map
	--[0] = {["id"] = "season1/metin2_map_sungzi_milgyo_pass03", ["name"] = "Temple Hwang Kingdom Battle - Jinno"},
	-- Scrapped Castle Map
	--[0] = {["id"] = "season1/metin2_map_ew02",                 ["name"] = "Castle Battle"},
	-- Secondary Mount Sohan scrapped since Alpha
	--[0] = {["id"] = "season2/map_n_snowm_02",                  ["name"] = "Mount Sohan 2"},
	-- Secondary Temple Hwang scrapped since Alpha
	--[0] = {["id"] = "season2/metin2_map_milgyo_a",             ["name"] = "Temple Hwang 2"},
	-- Secondary Yongbi Desert scrapped since Alpha
	--[0] = {["id"] = "season2/metin2_map_n_desert_02",          ["name"] = "Yongbi Desert 2"},
	-- Secondary Fireland scrapped since Alpha
	--[0] = {["id"] = "season2/metin2_map_n_flame_02",           ["name"] = "Fireland 2"},
	-- Secondary Valley of Seungryong scrapped since Alpha
	--[0] = {["id"] = "season2/metin2_map_a2_1",                 ["name"] = "Valley of Seungryong 2"},
	-- Secondary Ghost Forest scrapped since Alpha
	--[0] = {["id"] = "season2/metin2_map_trent_a",              ["name"] = "Ghost Forest 2"},
	-- Secondary Red Forest scrapped since Alpha
	--[0] = {["id"] = "season2/metin2_map_trent02_a",            ["name"] = "Red Forest 2"},
};
