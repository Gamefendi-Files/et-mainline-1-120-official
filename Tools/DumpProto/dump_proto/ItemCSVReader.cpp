#include <math.h>
#include "ItemCSVReader.h"
#include "../../../ClientSource-Extern/source/UserInterface/Locale_inc.h"

using namespace std;

inline string trim_left(const string& str)
{
	string::size_type n = str.find_first_not_of(" \t\v\n\r");
	return n == string::npos ? str : str.substr(n, str.length());
}

inline string trim_right(const string& str)
{
	string::size_type n = str.find_last_not_of(" \t\v\n\r");
	return n == string::npos ? str : str.substr(0, n + 1);
}

string trim(const string& str) { return trim_left(trim_right(str)); }

static string* StringSplit(string strOrigin, string strTok)
{
	int	cutAt;								//자르는위치
	int	index = 0;							//문자열인덱스
	string* strResult = new string[30];		//결과return 할변수

	//strTok을찾을때까지반복
	while ((cutAt = strOrigin.find_first_of(strTok)) != strOrigin.npos)
	{
		if (cutAt > 0)  //자르는위치가0보다크면(성공시)
		{
			strResult[index++] = strOrigin.substr(0, cutAt);  //결과배열에추가
		}
		strOrigin = strOrigin.substr(cutAt + 1);  //원본은자른부분제외한나머지
	}

	if (strOrigin.length() > 0)  //원본이아직남았으면
	{
		strResult[index++] = strOrigin.substr(0, cutAt);  //나머지를결과배열에추가
	}

	for (int i = 0; i < index; i++)
	{
		strResult[i] = trim(strResult[i]);
	}

	return strResult;  //결과return
}



int get_Item_Type_Value(string inputString)
{
	string arType[] =
	{
	"ITEM_NONE", "ITEM_WEAPON",
	"ITEM_ARMOR", "ITEM_USE",
	"ITEM_AUTOUSE", "ITEM_MATERIAL",
	"ITEM_SPECIAL", "ITEM_TOOL",
	"ITEM_LOTTERY", "ITEM_ELK",

	"ITEM_METIN", "ITEM_CONTAINER",
	"ITEM_FISH", "ITEM_ROD",
	"ITEM_RESOURCE", "ITEM_CAMPFIRE",
	"ITEM_UNIQUE", "ITEM_SKILLBOOK",
	"ITEM_QUEST", "ITEM_POLYMORPH",

	"ITEM_TREASURE_BOX", "ITEM_TREASURE_KEY",
	"ITEM_SKILLFORGET", "ITEM_GIFTBOX",
	"ITEM_PICK", "ITEM_HAIR",
	"ITEM_TOTEM", "ITEM_BLEND",
	"ITEM_COSTUME", "ITEM_DS",

	"ITEM_SPECIAL_DS",	"ITEM_EXTRACT",
	"ITEM_SECONDARY_COIN",

	"ITEM_RING",
	"ITEM_BELT",
	"ITEM_PET",
	"ITEM_MEDIUM",
	"ITEM_GACHA",
	"ITEM_SOUL",
	"ITEM_PASSIVE",
	"ITEM_MERCENARY"
	};


	int retInt = -1;
	//cout << "Type : " << typeStr << " -> ";
	for (int j = 0; j < sizeof(arType) / sizeof(arType[0]); j++)
	{
		string tempString = arType[j];
		if (inputString.find(tempString) != string::npos && tempString.find(inputString) != string::npos)
		{
			//cout << j << " ";
			retInt = j;
			break;
		}
	}
	//cout << endl;

	return retInt;

}

int get_Item_SubType_Value (int type_value, string inputString)
{
	static string arSub1[] =
{ "WEAPON_SWORD", "WEAPON_DAGGER", "WEAPON_BOW", "WEAPON_TWO_HANDED",
			"WEAPON_BELL", "WEAPON_FAN", "WEAPON_ARROW", "WEAPON_MOUNT_SPEAR", "WEAPON_CLAW", "WEAPON_QUIVER", "WEAPON_BOUQUET"};
	static string arSub2[] =
{ "ARMOR_BODY", "ARMOR_HEAD", "ARMOR_SHIELD", "ARMOR_WRIST", "ARMOR_FOOTS",
			"ARMOR_NECK", "ARMOR_EAR", "ARMOR_PENDANT", "ARMOR_GLOVE", "ARMOR_NUM_TYPES"};
	static string arSub3[] =
	{ "USE_POTION", "USE_TALISMAN", "USE_TUNING", "USE_MOVE", "USE_TREASURE_BOX", "USE_MONEYBAG", "USE_BAIT",
			"USE_ABILITY_UP", "USE_AFFECT", "USE_CREATE_STONE", "USE_SPECIAL", "USE_POTION_NODELAY", "USE_CLEAR",
			"USE_INVISIBILITY", "USE_DETACHMENT", "USE_BUCKET", "USE_POTION_CONTINUE", "USE_CLEAN_SOCKET",
			"USE_CHANGE_ATTRIBUTE", "USE_ADD_ATTRIBUTE", "USE_ADD_ACCESSORY_SOCKET", "USE_PUT_INTO_ACCESSORY_SOCKET",
			"USE_ADD_ATTRIBUTE2", "USE_RECIPE", "USE_CHANGE_ATTRIBUTE2", "USE_BIND", "USE_UNBIND", "USE_TIME_CHARGE_PER", "USE_TIME_CHARGE_FIX", "USE_PUT_INTO_BELT_SOCKET", "USE_PUT_INTO_RING_SOCKET",
			"USE_PUT_INTO_AURA_SOCKET",
			"USE_RESET_COSTUME_ATTR", "USE_CHANGE_COSTUME_ATTR",
			"USE_UNK33", "USE_CHANGE_ATTRIBUTE_PLUS", "USE_SELECT_ATTRIBUTE",
			"USE_EMOTION_PACK", "USE_ELEMENT_UPGRADE", "USE_ELEMENT_DOWNGRADE", "USE_ELEMENT_CHANGE", "USE_CALL",
			"USE_POTION_TOWER", "USE_POTION_NODELAY_TOWER", "USE_REMOVE_AFFECT", "USE_EMOTION_TOWER",
			"USE_SECRET_DUNGEON_SCROLL"};

	static string arSub4[] =
	{ "AUTOUSE_POTION", "AUTOUSE_ABILITY_UP", "AUTOUSE_BOMB", "AUTOUSE_GOLD", "AUTOUSE_MONEYBAG", "AUTOUSE_TREASURE_BOX"};

	static string arSub5[] =
	{ "MATERIAL_LEATHER", "MATERIAL_BLOOD", "MATERIAL_ROOT", "MATERIAL_NEEDLE", "MATERIAL_JEWEL",
			"MATERIAL_DS_REFINE_NORMAL", "MATERIAL_DS_REFINE_BLESSED", "MATERIAL_DS_REFINE_HOLLY", "MATERIAL_DS_CHANGE_ATTR",
			"MATERIAL_PASSIVE_WEAPON", "MATERIAL_PASSIVE_ARMOR", "MATERIAL_PASSIVE_ACCE", "MATERIAL_PASSIVE_ELEMENT", "MATERIAL_DS_REFINE_HOLLY"};

	static string arSub6[] =
{ "SPECIAL_MAP", "SPECIAL_KEY", "SPECIAL_DOC", "SPECIAL_SPIRIT"};
	static string arSub7[] =
	{
		"TOOL_FISHING_ROD"
	};
	static string arSub8[] =
		{ "LOTTERY_TICKET", "LOTTERY_INSTANT"};

	static string arSub10[] =
	{
		"METIN_NORMAL",
		"METIN_GOLD",
		"METIN_SUNGMA"
	};
	static string arSub12[] =
	{
		"FISH_ALIVE",
		"FISH_DEAD"
	};
	static string arSub14[] =
{ "RESOURCE_FISHBONE", "RESOURCE_WATERSTONEPIECE", "RESOURCE_WATERSTONE", "RESOURCE_BLOOD_PEARL",
			"RESOURCE_BLUE_PEARL", "RESOURCE_WHITE_PEARL", "RESOURCE_BUCKET", "RESOURCE_CRYSTAL", "RESOURCE_GEM",
			"RESOURCE_STONE", "RESOURCE_METIN", "RESOURCE_ORE", "RESOURCE_AURA"};

	static string arSub16[] =
	{
		"UNIQUE_NONE",
		"UNIQUE_BOOK",
		"UNIQUE_SPECIAL_RIDE",
		"UNIQUE_3",
		"UNIQUE_4",
		"UNIQUE_5",
		"UNIQUE_6",
		"UNIQUE_7",
		"UNIQUE_8",
		"UNIQUE_9",
		"USE_SPECIAL"
	};
	static string arSub18[] =
		{"QUEST_NONE", "QUEST_PET_PAY", "QUEST_WARP"};

	static string arSub23[] =
		{"UNK23_1", "UNK23_2"};

	static string arSub28[] =
		{ "COSTUME_BODY", "COSTUME_HAIR", "COSTUME_WEAPON", "COSTUME_MOUNT", "COSTUME_ACCE", "COSTUME_AURA"};

	static string arSub29[] =
		{ "DS_SLOT1", "DS_SLOT2", "DS_SLOT3", "DS_SLOT4", "DS_SLOT5", "DS_SLOT6", "DS_SLOT7"};

	static string arSub31[] =
	{
		"EXTRACT_DRAGON_SOUL",
		"EXTRACT_DRAGON_HEART"
	};

	static string arSub35[] =
		{
			"PET_EGG",
			"PET_UPBRINGING",
			"PET_BAG",
			"PET_FEEDSTUFF",
			"PET_SKILL",
			"PET_SKILL_DEL_BOOK",
			"PET_NAME_CHANGE",
			"PET_EXPFOOD",
			"PET_SKILL_ALL_DEL_BOOK",
			"PET_EXPFOOD_PER",
			"PET_ITEM_TYPE",
			"PET_ATTR_CHANGE",
			"PET_ATTR_DETERMINE",
			"PET_PAY",
			"PET_PRIMIUM_FEEDSTUFF"
		};

	static string arSub36[] = {
		"MEDIUM_MOVE_COSTUME_ATTR",
		"MEDIUM_MOVE_ACCE_ATTR"
	};

	static string arSub37[] =
		{"USE_GACHA", "SPECIAL_GACHA", "GEM_LUCKY_BOX_GACHA", "SPECIAL_LUCKY_BOX_GACHA"};

	static string arSub38[] =
		{"SOUL_RED", "SOUL_BLUE"};

	static string arSub39[] =
		{"MERCENARY_0", "MERCENARY_1", "MERCENARY_2", "MERCENARY_3", "MERCENARY_4", "MERCENARY_5", "MERCENARY_6"};

	static string arSub40[] =
		{"MERCENARY_0", "MERCENARY_1", "MERCENARY_2", "MERCENARY_3", "MERCENARY_4", "MERCENARY_5", "MERCENARY_6"};


	static string* arSubType[] = {0,	//0
		arSub1,		//1
		arSub2,	//2
		arSub3,	//3
		arSub4,	//4
		arSub5,	//5
		arSub6,	//6
		arSub7,	//7
		arSub8,	//8
		0,			//9
		arSub10,	//10
		0,			//11
		arSub12,	//12
		0,			//13
		arSub14,	//14
		0,			//15
		arSub16,	//16
		0,			//17
		0,			//18
		0,			//19
		0,			//20
		0,			//21
		0,			//22
		0,			//23
		0,			//24
		0,			//25
		0,			//26
		0,			//27
		arSub28,	//28
		arSub29,		//29
		arSub29,	//30
		arSub31,	//31
		0,			//32
		0,			//33 반지
		0,			//34 벨트
		arSub35,
		arSub36,
		arSub37,
		arSub38,
		arSub39,
		arSub40,
	};
	static int arNumberOfSubtype[_countof(arSubType)] = {
		0,
		sizeof(arSub1)/sizeof(arSub1[0]),
		sizeof(arSub2)/sizeof(arSub2[0]),
		sizeof(arSub3)/sizeof(arSub3[0]),
		sizeof(arSub4)/sizeof(arSub4[0]),
		sizeof(arSub5)/sizeof(arSub5[0]),
		sizeof(arSub6)/sizeof(arSub6[0]),
		sizeof(arSub7)/sizeof(arSub7[0]),
		sizeof(arSub8)/sizeof(arSub8[0]),
		0,
		sizeof(arSub10)/sizeof(arSub10[0]),
		0,
		sizeof(arSub12)/sizeof(arSub12[0]),
		0,
		sizeof(arSub14)/sizeof(arSub14[0]),
		0,
		sizeof(arSub16)/sizeof(arSub16[0]),
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		sizeof(arSub28)/sizeof(arSub28[0]),
		sizeof(arSub29)/sizeof(arSub29[0]),
		sizeof(arSub29)/sizeof(arSub29[0]),
		sizeof(arSub31)/sizeof(arSub31[0]),
		0, // 32
		0, // 33 반지
		0, // 34 벨트
		sizeof(arSub35)/sizeof(arSub35[0]),
		sizeof(arSub36) / sizeof(arSub36[0]),
		sizeof(arSub37)/sizeof(arSub37[0]),
		sizeof(arSub38)/sizeof(arSub38[0]),
		sizeof(arSub39)/sizeof(arSub39[0]),
		sizeof(arSub40)/sizeof(arSub40[0]),
	};

	//아이템 타입의 서브타입 어레이가 존재하는지 알아보고, 없으면 0 리턴
	if (arSubType[type_value] == 0) {
		return 0;
	}
	//

	int retInt = -1;
	//cout << "SubType : " << subTypeStr << " -> ";
	for (int j = 0; j < arNumberOfSubtype[type_value]; j++) {
		string tempString = arSubType[type_value][j];
		string tempInputString = trim(inputString);
		if (tempInputString.compare(tempString) == 0)
		{
			//cout << j << " ";
			retInt = j;
			break;
		}
	}
	//cout << endl;

	return retInt;
}

int get_Item_AntiFlag_Value (string inputString)
{

	string arAntiFlag[] =
{"ANTI_FEMALE", "ANTI_MALE", "ANTI_MUSA", "ANTI_ASSASSIN", "ANTI_SURA", "ANTI_MUDANG",
							"ANTI_GET", "ANTI_DROP", "ANTI_SELL", "ANTI_EMPIRE_A", "ANTI_EMPIRE_B", "ANTI_EMPIRE_C",
							"ANTI_SAVE", "ANTI_GIVE", "ANTI_PKDROP", "ANTI_STACK", "ANTI_MYSHOP", "ANTI_SAFEBOX", "ANTI_WOLFMAN",
							"ANTI_RT_REMOVE", "ANTI_QUICKSLOT", "ANTI_CHANGELOOK",
							"ANTI_REINFORCE", "ANTI_ENCHANT", "ANTI_ENERGY",
							"ANTI_PETFEED", "ANTI_APPLY", "ANTI_ACCE", "ANTI_MAIL", "ANTI_PET20", "ANTI_PET21"};

	int retValue = 0;
	string* arInputString = StringSplit(inputString, "|");				//프로토 정보 내용을 단어별로 쪼갠 배열.
	for (int i = 0; i < sizeof(arAntiFlag) / sizeof(arAntiFlag[0]); i++) {
		string tempString = arAntiFlag[i];
		for (int j = 0; j < 30; j++)		//최대 30개 단어까지. (하드코딩)
		{
			string tempString2 = arInputString[j];
			if (tempString2.compare(tempString) == 0) {				//일치하는지 확인.
				retValue = retValue + pow((float)2, (float)i);
			}

			if (tempString2.compare("") == 0)
				break;
		}
	}
	delete[]arInputString;
	//cout << "AntiFlag : " << antiFlagStr << " -> " << retValue << endl;

	return retValue;
}

int get_Item_Flag_Value(string inputString)
{

	string arFlag[] =
{"ITEM_TUNABLE", "ITEM_SAVE", "ITEM_STACKABLE", "COUNT_PER_1GOLD", "ITEM_SLOW_QUERY", "ITEM_UNIQUE",
			"ITEM_MAKECOUNT", "ITEM_IRREMOVABLE", "CONFIRM_WHEN_USE", "QUEST_USE", "QUEST_USE_MULTIPLE",
			"QUEST_GIVE", "ITEM_QUEST", "LOG", "STACKABLE", "SLOW_QUERY", "REFINEABLE", "IRREMOVABLE", "ITEM_APPLICABLE"};


	int retValue = 0;
	string* arInputString = StringSplit(inputString, "|");				//프로토 정보 내용을 단어별로 쪼갠 배열.
	for (int i = 0; i < sizeof(arFlag) / sizeof(arFlag[0]); i++) {
		string tempString = arFlag[i];
		for (int j = 0; j < 30; j++)		//최대 30개 단어까지. (하드코딩)
		{
			string tempString2 = arInputString[j];
			if (tempString2.compare(tempString) == 0) {				//일치하는지 확인.
				retValue = retValue + pow((float)2, (float)i);
			}

			if (tempString2.compare("") == 0)
				break;
		}
	}
	delete[]arInputString;
	//cout << "Flag : " << flagStr << " -> " << retValue << endl;

	return retValue;
}

int get_Item_WearFlag_Value(string inputString)
{

	string arWearrFlag[] = {
							"WEAR_BODY", "WEAR_HEAD", "WEAR_FOOTS", "WEAR_WRIST", "WEAR_WEAPON",
							"WEAR_NECK", "WEAR_EAR", "WEAR_ARROW", "WEAR_SHIELD", "WEAR_BELT",
							"WEAR_PENDANT", "WEAR_GLOVE", "WEAR_COSTUME_BODY", "WEAR_COSTUME_HAIR",
							"WEAR_COSTUME_WEAPON", "WEAR_COSTUME_MOUNT", "WEAR_COSTUME_ACCE", "WEAR_UNIQUE"
	};



	int retValue = 0;
	string* arInputString = StringSplit(inputString, "|");				//프로토 정보 내용을 단어별로 쪼갠 배열.
	for (int i = 0; i < sizeof(arWearrFlag) / sizeof(arWearrFlag[0]); i++) {
		string tempString = arWearrFlag[i];
		for (int j = 0; j < 30; j++)		//최대 30개 단어까지. (하드코딩)
		{
			string tempString2 = arInputString[j];
			if (tempString2.compare(tempString) == 0) {				//일치하는지 확인.
				retValue = retValue + pow((float)2, (float)i);
			}

			if (tempString2.compare("") == 0)
				break;
		}
	}
	delete[]arInputString;
	//cout << "WearFlag : " << wearFlagStr << " -> " << retValue << endl;

	return retValue;
}

int get_Item_Immune_Value(string inputString)
{

	string arImmune[] =
	{
		"PARA",
		"CURSE",
		"STUN",
		"SLEEP",
		"SLOW",
		"POISON",
		"TERROR"
	};

	int retValue = 0;
	string* arInputString = StringSplit(inputString, "|");				//프로토 정보 내용을 단어별로 쪼갠 배열.
	for (int i = 0; i < sizeof(arImmune) / sizeof(arImmune[0]); i++) {
		string tempString = arImmune[i];
		for (int j = 0; j < 30; j++)		//최대 30개 단어까지. (하드코딩)
		{
			string tempString2 = arInputString[j];
			if (tempString2.compare(tempString) == 0) {				//일치하는지 확인.
				retValue = retValue + pow((float)2, (float)i);
			}

			if (tempString2.compare("") == 0)
				break;
		}
	}
	delete[]arInputString;
	//cout << "Immune : " << immuneStr << " -> " << retValue << endl;

	return retValue;
}




int get_Item_LimitType_Value(string inputString)
{
	string arLimitType[] =
{"LIMIT_NONE", "LEVEL", "STR", "DEX", "INT", "CON", "REAL_TIME", "REAL_TIME_FIRST_USE", "TIMER_BASED_ON_WEAR", "PC_BANG", "NEWWORLD_LEVEL", "DURATION"};


	int retInt = -1;
	//cout << "LimitType : " << limitTypeStr << " -> ";
	for (int j = 0; j < sizeof(arLimitType) / sizeof(arLimitType[0]); j++) {
		string tempString = arLimitType[j];
		string tempInputString = trim(inputString);
		if (tempInputString.compare(tempString) == 0)
		{
			//cout << j << " ";
			retInt = j;
			break;
		}
	}
	//cout << endl;

	return retInt;
}

int get_Item_ApplyType_Value (string inputString)
{
	string arApplyType[] =
{	
	"APPLY_NONE",
	"APPLY_MAX_HP", 
	"APPLY_MAX_SP", 
	"APPLY_CON", 
	"APPLY_INT", 
	"APPLY_STR", 
	"APPLY_DEX", 
	"APPLY_ATT_SPEED",
	"APPLY_MOV_SPEED", 
	"APPLY_CAST_SPEED", 
	"APPLY_HP_REGEN",
	"APPLY_SP_REGEN",
	"APPLY_POISON_PCT",
	"APPLY_STUN_PCT",
			"APPLY_SLOW_PCT", 
			"APPLY_CRITICAL_PCT",
			"APPLY_PENETRATE_PCT",
			"APPLY_ATTBONUS_HUMAN",
			"APPLY_ATTBONUS_ANIMAL",
			"APPLY_ATTBONUS_ORC",
			"APPLY_ATTBONUS_MILGYO",
			"APPLY_ATTBONUS_UNDEAD",
			"APPLY_ATTBONUS_DEVIL",
			"APPLY_STEAL_HP",
			"APPLY_STEAL_SP",
			"APPLY_MANA_BURN_PCT",
			"APPLY_DAMAGE_SP_RECOVER",
			"APPLY_BLOCK",
			"APPLY_DODGE",
			"APPLY_RESIST_SWORD",
			"APPLY_RESIST_TWOHAND",
			"APPLY_RESIST_DAGGER",
			"APPLY_RESIST_BELL",
			"APPLY_RESIST_FAN",
			"APPLY_RESIST_BOW",
			"APPLY_RESIST_FIRE",
			"APPLY_RESIST_ELEC",
			"APPLY_RESIST_MAGIC",
			"APPLY_RESIST_WIND",
			"APPLY_REFLECT_MELEE",
			"APPLY_REFLECT_CURSE",
			"APPLY_POISON_REDUCE",
			"APPLY_KILL_SP_RECOVER",
			"APPLY_EXP_DOUBLE_BONUS",
			"APPLY_GOLD_DOUBLE_BONUS",
			"APPLY_ITEM_DROP_BONUS",
			"APPLY_POTION_BONUS",
			"APPLY_KILL_HP_RECOVER",
			"APPLY_IMMUNE_STUN",
			"APPLY_IMMUNE_SLOW",
			"APPLY_IMMUNE_FALL",
			"APPLY_SKILL",
			"APPLY_BOW_DISTANCE",
			"APPLY_ATT_GRADE_BONUS",
			"APPLY_DEF_GRADE_BONUS",
			"APPLY_MAGIC_ATT_GRADE",
			"APPLY_MAGIC_DEF_GRADE",
			"APPLY_CURSE_PCT",
			"APPLY_MAX_STAMINA",
			"APPLY_ATTBONUS_WARRIOR",
			"APPLY_ATTBONUS_ASSASSIN",
			"APPLY_ATTBONUS_SURA",
			"APPLY_ATTBONUS_SHAMAN",
			"APPLY_ATTBONUS_MONSTER",
			"APPLY_MALL_ATTBONUS",
			"APPLY_MALL_DEFBONUS",
			"APPLY_MALL_EXPBONUS",
			"APPLY_MALL_ITEMBONUS",
			"APPLY_MALL_GOLDBONUS",
			"APPLY_MAX_HP_PCT",
			"APPLY_MAX_SP_PCT",
			"APPLY_SKILL_DAMAGE_BONUS",
			"APPLY_NORMAL_HIT_DAMAGE_BONUS",
			"APPLY_SKILL_DEFEND_BONUS",
			"APPLY_NORMAL_HIT_DEFEND_BONUS",
			"APPLY_EXTRACT_HP_PCT",
			"APPLY_RESIST_WARRIOR",
			"APPLY_RESIST_ASSASSIN",
			"APPLY_RESIST_SURA",
			"APPLY_RESIST_SHAMAN",
			"APPLY_ENERGY",
			"APPLY_DEF_GRADE",
			"APPLY_COSTUME_ATTR_BONUS",
			"APPLY_MAGIC_ATTBONUS_PER",
			"APPLY_MELEE_MAGIC_ATTBONUS_PER",
			"APPLY_RESIST_ICE",
			"APPLY_RESIST_EARTH",
			"APPLY_RESIST_DARK",
			"APPLY_ANTI_CRITICAL_PCT",
			"APPLY_ANTI_PENETRATE_PCT",
			"APPLY_ENCHANT_ELECT",
			"APPLY_ENCHANT_FIRE",
			"APPLY_ENCHANT_ICE",
			"APPLY_ENCHANT_WIND",
			"APPLY_ENCHANT_EARTH",
			"APPLY_ENCHANT_DARK",
			"APPLY_ATTBONUS_INSECT",
			"APPLY_ATTBONUS_DESERT",
			"APPLY_ATTBONUS_SWORD",
			"APPLY_ATTBONUS_TWOHAND",
			"APPLY_ATTBONUS_DAGGER",
			"APPLY_ATTBONUS_BELL",
			"APPLY_ATTBONUS_FAN",
			"APPLY_ATTBONUS_BOW",
			"APPLY_RESIST_HUMAN",
			"APPLY_ATTBONUS_STONE",
			"APPLY_ATTBONUS_BOSS",
			"APPLY_ACCEDRAIN_RATE",
			"APPLY_SUNGMA_STR",
			"APPLY_SUNGMA_HP",
			"APPLY_SUNGMA_MOVE",
			"APPLY_SUNGMA_IMMUNE",
			"APPLY_RANDOM",//yeni bonusları burdan sonra ekle
			"APPLY_PC_BANG_EXP_BONUS", "APPLY_PC_BANG_DROP_BONUS",
			"APPLY_BLEEDING_REDUCE", "APPLY_BLEEDING_PCT", "APPLY_ATTBONUS_WOLFMAN", "APPLY_RESIST_WOLFMAN", "APPLY_RESIST_CLAW",
			"APPLY_RESIST_MAGIC_REDUCTION",
			"APPLY_ATTBONUS_CLAW",
			"APPLY_RESIST_MOUNT_FALL", "APPLY_UNK117",
			"APPLY_SKILL_DAMAGE_SAMYEON",
			"APPLY_MOUNT",
	"APPLY_SKILL_DAMAGE_TANHWAN",
	"APPLY_SKILL_DAMAGE_PALBANG",
	"APPLY_SKILL_DAMAGE_GIGONGCHAM",
	"APPLY_SKILL_DAMAGE_GYOKSAN",
	"APPLY_SKILL_DAMAGE_GEOMPUNG",
	"APPLY_SKILL_DAMAGE_AMSEOP",
	"APPLY_SKILL_DAMAGE_GUNGSIN",
	"APPLY_SKILL_DAMAGE_CHARYUN",
	"APPLY_SKILL_DAMAGE_SANGONG",
	"APPLY_SKILL_DAMAGE_YEONSA",
	"APPLY_SKILL_DAMAGE_KWANKYEOK",
	"APPLY_SKILL_DAMAGE_GIGUNG",
	"APPLY_SKILL_DAMAGE_HWAJO",
	"APPLY_SKILL_DAMAGE_SWAERYUNG",
	"APPLY_SKILL_DAMAGE_YONGKWON",
	"APPLY_SKILL_DAMAGE_PABEOB",
	"APPLY_SKILL_DAMAGE_MARYUNG",
	"APPLY_SKILL_DAMAGE_HWAYEOMPOK",
	"APPLY_SKILL_DAMAGE_MAHWAN",
	"APPLY_SKILL_DAMAGE_BIPABU",
	"APPLY_SKILL_DAMAGE_YONGBI",
	"APPLY_SKILL_DAMAGE_PAERYONG",
	"APPLY_SKILL_DAMAGE_NOEJEON",
	"APPLY_SKILL_DAMAGE_BYEURAK",
	"APPLY_SKILL_DAMAGE_CHAIN",
	"APPLY_SKILL_DAMAGE_CHAYEOL",
	"APPLY_SKILL_DAMAGE_SALPOONG",
	"APPLY_SKILL_DAMAGE_GONGDAB",
	"APPLY_SKILL_DAMAGE_PASWAE",
	"APPLY_NORMAL_HIT_DEFEND_BONUS_BOSS_OR_MORE",
	"APPLY_SKILL_DEFEND_BONUS_BOSS_OR_MORE",
	"APPLY_NORMAL_HIT_DAMAGE_BONUS_BOSS_OR_MORE",
	"APPLY_SKILL_DAMAGE_BONUS_BOSS_OR_MORE",
	"APPLY_HIT_BUFF_ENCHANT_FIRE",
	"APPLY_HIT_BUFF_ENCHANT_ICE",
	"APPLY_HIT_BUFF_ENCHANT_ELEC",
	"APPLY_HIT_BUFF_ENCHANT_WIND",
	"APPLY_HIT_BUFF_ENCHANT_DARK",
	"APPLY_HIT_BUFF_ENCHANT_EARTH",
	"APPLY_HIT_BUFF_RESIST_FIRE",
	"APPLY_HIT_BUFF_RESIST_ICE",
	"APPLY_HIT_BUFF_RESIST_ELEC",
	"APPLY_HIT_BUFF_RESIST_WIND",
	"APPLY_HIT_BUFF_RESIST_DARK",
	"APPLY_HIT_BUFF_RESIST_EARTH",
	"APPLY_USE_SKILL_CHEONGRANG_MOV_SPEED",
	"APPLY_USE_SKILL_CHEONGRANG_CASTING_SPEED",
	"APPLY_USE_SKILL_CHAYEOL_CRITICAL_PCT",
	"APPLY_USE_SKILL_SANGONG_ATT_GRADE_BONUS",
	"APPLY_USE_SKILL_GIGUNG_ATT_GRADE_BONUS",
	"APPLY_USE_SKILL_JEOKRANG_DEF_BONUS",
	"APPLY_USE_SKILL_GWIGEOM_DEF_BONUS",
	"APPLY_USE_SKILL_TERROR_ATT_GRADE_BONUS",
	"APPLY_USE_SKILL_MUYEONG_ATT_GRADE_BONUS",
	"APPLY_USE_SKILL_MANASHILED_CASTING_SPEED",
	"APPLY_USE_SKILL_HOSIN_DEF_BONUS",
	"APPLY_USE_SKILL_GICHEON_ATT_GRADE_BONUS",
	"APPLY_USE_SKILL_JEONGEOP_ATT_GRADE_BONUS",
	"APPLY_USE_SKILL_JEUNGRYEOK_DEF_BONUS",
	"APPLY_USE_SKILL_GIHYEOL_ATT_GRADE_BONUS",
	"APPLY_USE_SKILL_CHUNKEON_CASTING_SPEED",
	"APPLY_USE_SKILL_NOEGEOM_ATT_GRADE_BONUS",
	"APPLY_SKILL_DURATION_INCREASE_EUNHYUNG",
	"APPLY_SKILL_DURATION_INCREASE_GYEONGGONG",
	"APPLY_SKILL_DURATION_INCREASE_GEOMKYUNG",
	"APPLY_SKILL_DURATION_INCREASE_JEOKRANG",
	"APPLY_USE_SKILL_PALBANG_HP_ABSORB",
	"APPLY_USE_SKILL_AMSEOP_HP_ABSORB",
	"APPLY_USE_SKILL_YEONSA_HP_ABSORB",
	"APPLY_USE_SKILL_YONGBI_HP_ABSORB",
	"APPLY_USE_SKILL_CHAIN_HP_ABSORB",
	"APPLY_USE_SKILL_PASWAE_SP_ABSORB",
	"APPLY_USE_SKILL_GIGONGCHAM_STUN",
	"APPLY_USE_SKILL_CHARYUN_STUN",
	"APPLY_USE_SKILL_PABEOB_STUN",
	"APPLY_USE_SKILL_MAHWAN_STUN",
	"APPLY_USE_SKILL_GONGDAB_STUN",
	"APPLY_USE_SKILL_SAMYEON_STUN",
	"APPLY_USE_SKILL_GYOKSAN_KNOCKBACK",
	"APPLY_USE_SKILL_SEOMJEON_KNOCKBACK",
	"APPLY_USE_SKILL_SWAERYUNG_KNOCKBACK",
	"APPLY_USE_SKILL_HWAYEOMPOK_KNOCKBACK",
	"APPLY_USE_SKILL_GONGDAB_KNOCKBACK",
	"APPLY_USE_SKILL_KWANKYEOK_KNOCKBACK",
	"APPLY_USE_SKILL_SAMYEON_NEXT_COOLTIME_DECREASE_10PER",
	"APPLY_USE_SKILL_GEOMPUNG_NEXT_COOLTIME_DECREASE_10PER",
	"APPLY_USE_SKILL_GUNGSIN_NEXT_COOLTIME_DECREASE_10PER",
	"APPLY_USE_SKILL_KWANKYEOK_NEXT_COOLTIME_DECREASE_10PER",
	"APPLY_USE_SKILL_YONGKWON_NEXT_COOLTIME_DECREASE_10PER",
	"APPLY_USE_SKILL_MARYUNG_NEXT_COOLTIME_DECREASE_10PER",
	"APPLY_USE_SKILL_BIPABU_NEXT_COOLTIME_DECREASE_10PER",
	"APPLY_USE_SKILL_NOEJEON_NEXT_COOLTIME_DECREASE_10PER",
	"APPLY_USE_SKILL_SALPOONG_NEXT_COOLTIME_DECREASE_10PER",
	"APPLY_USE_SKILL_PASWAE_NEXT_COOLTIME_DECREASE_10PER",
	"APPLY_DAMAGE_HP_RECOVERY",
	"APPLY_DAMAGE_SP_RECOVERY",
	"APPLY_ALIGNMENT_DAMAGE_BONUS",
	"APPLY_NORMAL_DAMAGE_GUARD",
	"APPLY_MORE_THEN_HP90_DAMAGE_REDUCE",
	"APPLY_USE_SKILL_TUSOK_HP_ABSORB",
	"APPLY_USE_SKILL_PAERYONG_HP_ABSORB",
	"APPLY_USE_SKILL_BYEURAK_HP_ABSORB",
	"APPLY_FIRST_ATTRIBUTE_BONUS",
	"APPLY_SECOND_ATTRIBUTE_BONUS",
	"APPLY_THIRD_ATTRIBUTE_BONUS",
	"APPLY_FOURTH_ATTRIBUTE_BONUS",
	"APPLY_FIFTH_ATTRIBUTE_BONUS",
	"APPLY_USE_SKILL_SAMYEON_NEXT_COOLTIME_DECREASE_20PER",
	"APPLY_USE_SKILL_GEOMPUNG_NEXT_COOLTIME_DECREASE_20PER",
	"APPLY_USE_SKILL_GUNGSIN_NEXT_COOLTIME_DECREASE_20PER",
	"APPLY_USE_SKILL_KWANKYEOK_NEXT_COOLTIME_DECREASE_20PER",
	"APPLY_USE_SKILL_YONGKWON_NEXT_COOLTIME_DECREASE_20PER",
	"APPLY_USE_SKILL_MARYUNG_NEXT_COOLTIME_DECREASE_20PER",
	"APPLY_USE_SKILL_BIPABU_NEXT_COOLTIME_DECREASE_20PER",
	"APPLY_USE_SKILL_NOEJEON_NEXT_COOLTIME_DECREASE_20PER",
	"APPLY_USE_SKILL_SALPOONG_NEXT_COOLTIME_DECREASE_20PER",
	"APPLY_USE_SKILL_PASWAE_NEXT_COOLTIME_DECREASE_20PER",
	"APPLY_USE_SKILL_CHAYEOL_HP_ABSORB",
	"APPLY_HIT_PCT",
	"APPLY_ATTBONUS_PER_HUMAN",
	"APPLY_ATTBONUS_PER_ANIMAL",
	"APPLY_ATTBONUS_PER_ORC",
	"APPLY_ATTBONUS_PER_MILGYO",
	"APPLY_ATTBONUS_PER_UNDEAD",
	"APPLY_ATTBONUS_PER_DEVIL",
	"APPLY_ENCHANT_PER_ELECT",
	"APPLY_ENCHANT_PER_FIRE",
	"APPLY_ENCHANT_PER_ICE",
	"APPLY_ENCHANT_PER_WIND",
	"APPLY_ENCHANT_PER_EARTH",
	"APPLY_ENCHANT_PER_DARK",
	"APPLY_ATTBONUS_PER_CZ",
	"APPLY_ATTBONUS_PER_INSECT",
	"APPLY_ATTBONUS_PER_DESERT",
	"APPLY_ATTBONUS_PER_STONE",
	"APPLY_ATTBONUS_PER_MONSTER",
	"APPLY_RESIST_PER_HUMAN",
	"APPLY_RESIST_PER_ICE",
	"APPLY_RESIST_PER_DARK",
	"APPLY_RESIST_PER_EARTH",
	"APPLY_RESIST_PER_FIRE",
	"APPLY_RESIST_PER_ELEC",
	"APPLY_RESIST_PER_MAGIC",
	"APPLY_RESIST_PER_WIND",
	"APPLY_HIT_BUFF_SUNGMA_STR",
	"APPLY_HIT_BUFF_SUNGMA_MOVE",
	"APPLY_HIT_BUFF_SUNGMA_HP",
	"APPLY_HIT_BUFF_SUNGMA_IMMUNE",
	"APPLY_MOUNT_MELEE_MAGIC_ATTBONUS_PER",
	"APPLY_DISMOUNT_MOVE_SPEED_BONUS_PER",
	"APPLY_HIT_AUTO_HP_RECOVERY",
	"APPLY_HIT_AUTO_SP_RECOVERY",
	"APPLY_USE_SKILL_COOLTIME_DECREASE_ALL",
	"APPLY_HIT_STONE_ATTBONUS_STONE",
	"APPLY_HIT_STONE_DEF_GRADE_BONUS",
	"APPLY_KILL_BOSS_ITEM_BONUS",
	"APPLY_MOB_HIT_MOB_AGGRESSIVE",
	"APPLY_NO_DEATH_AND_HP_RECOVERY30",
	"APPLY_AUTO_PICKUP",
	"APPLY_MOUNT_NO_KNOCKBACK",
	"APPLY_SUNGMA_PER_STR",
	"APPLY_SUNGMA_PER_HP",
	"APPLY_SUNGMA_PER_MOVE",
	"APPLY_SUNGMA_PER_IMMUNE",
	"APPLY_IMMUNE_POISON100",
	"APPLY_IMMUNE_BLEEDING100",
	"APPLY_MONSTER_DEFEND_BONUS",
	};

	int retInt = -1;
	//cout << "ApplyType : " << applyTypeStr << " -> ";
	for (int j = 0; j < sizeof(arApplyType) / sizeof(arApplyType[0]); j++) {
		string tempString = arApplyType[j];
		string tempInputString = trim(inputString);
		if (tempInputString.compare(tempString) == 0)
		{
			//cout << j << " ";
			retInt = j;
			break;
		}
	}
	//cout << endl;

	return retInt;

}


//몬스터 프로토도 읽는다.


int get_Mob_Rank_Value(string inputString)
{
	string arRank[] =
	{
		"PAWN",
		"S_PAWN",
		"KNIGHT",
		"S_KNIGHT",
		"BOSS",
		"KING"
	};

	int retInt = -1;
	//cout << "Rank : " << rankStr << " -> ";
	for (int j = 0; j < sizeof(arRank) / sizeof(arRank[0]); j++) {
		string tempString = arRank[j];
		string tempInputString = trim(inputString);
		if (tempInputString.compare(tempString) == 0)
		{
			//cout << j << " ";
			retInt = j;
			break;
		}
	}
	//cout << endl;

	return retInt;
}


int get_Mob_Type_Value(string inputString)
{
	string arType[] =
	{
		"MONSTER",
		"NPC",
		"STONE",
		"WARP",
		"DOOR",
		"BUILDING",
		"PC",
		"POLYMORPH_PC",
		"HORSE",
		"GOTO"
	};

	int retInt = -1;
	//cout << "Type : " << typeStr << " -> ";
	for (int j = 0; j < sizeof(arType) / sizeof(arType[0]); j++) {
		string tempString = arType[j];
		string tempInputString = trim(inputString);
		if (tempInputString.compare(tempString) == 0)
		{
			//cout << j << " ";
			retInt = j;
			break;
		}
	}
	//cout << endl;

	return retInt;
}

int get_Mob_BattleType_Value(string inputString)
{
	string arBattleType[] =
	{
		"MELEE",
		"RANGE",
		"MAGIC",
		"SPECIAL",
		"POWER",
		"TANKER",
		"SUPER_POWER",
		"SUPER_TANKER"
	};

	int retInt = -1;
	//cout << "Battle Type : " << battleTypeStr << " -> ";
	for (int j = 0; j < sizeof(arBattleType) / sizeof(arBattleType[0]); j++) {
		string tempString = arBattleType[j];
		string tempInputString = trim(inputString);
		if (tempInputString.compare(tempString) == 0)
		{
			//cout << j << " ";
			retInt = j;
			break;
		}
	}
	//cout << endl;

	return retInt;
}

int get_Mob_Size_Value(string inputString)
{
	string arSize[] =
	{
		"SMALL", // @fix5
		"MEDIUM",
		"BIG"
	};

	int retInt = 0;
	//cout << "Size : " << sizeStr << " -> ";
	for (int j = 0; j < sizeof(arSize) / sizeof(arSize[0]); j++) {
		string tempString = arSize[j];
		string tempInputString = trim(inputString);
		if (tempInputString.compare(tempString) == 0)
		{
			//cout << j << " ";
			retInt = j + 1;
			break;
		}
	}
	//cout << endl;

	return retInt;
}

int get_Mob_AIFlag_Value(string inputString)
{
	string arAIFlag[] =
	{
		"AGGR",
		"NOMOVE",
		"COWARD",
		"NOATTSHINSU",
		"NOATTCHUNJO",
		"NOATTJINNO",
		"ATTMOB",
		"BERSERK",
		"STONESKIN",
		"GODSPEED",
		"DEATHBLOW",
		"REVIVE"
	};


	int retValue = 0;
	string* arInputString = StringSplit(inputString, ",");				//프로토 정보 내용을 단어별로 쪼갠 배열.
	for (int i = 0; i < sizeof(arAIFlag) / sizeof(arAIFlag[0]); i++) {
		string tempString = arAIFlag[i];
		for (int j = 0; j < 30; j++)		//최대 30개 단어까지. (하드코딩)
		{
			string tempString2 = arInputString[j];
			if (tempString2.compare(tempString) == 0) {				//일치하는지 확인.
				retValue = retValue + pow((float)2, (float)i);
			}

			if (tempString2.compare("") == 0)
				break;
		}
	}
	delete[]arInputString;
	//cout << "AIFlag : " << aiFlagStr << " -> " << retValue << endl;

	return retValue;
}

int get_Mob_RaceFlag_Value(string inputString)
{
	string arRaceFlag[] =
	{
		"ANIMAL",
		"UNDEAD",
		"DEVIL",
		"HUMAN",
		"ORC",
		"MILGYO",
		"INSECT",
		"FIRE",
		"ICE",
		"DESERT",
		"TREE",
		"ATT_ELEC",
		"ATT_FIRE",
		"ATT_ICE",
		"ATT_WIND",
		"ATT_EARTH",
		"ATT_DARK"
	};

	int retValue = 0;
	string* arInputString = StringSplit(inputString, "|");				//프로토 정보 내용을 단어별로 쪼갠 배열.
	for (int i = 0; i < sizeof(arRaceFlag) / sizeof(arRaceFlag[0]); i++) {
		string tempString = arRaceFlag[i];
		for (int j = 0; j < 30; j++)		//최대 30개 단어까지. (하드코딩)
		{
			string tempString2 = arInputString[j];
			if (tempString2.compare(tempString) == 0) {				//일치하는지 확인.
				retValue = retValue + pow((float)2, (float)i);
			}

			if (tempString2.compare("") == 0)
				break;
		}
	}
	delete[]arInputString;
	//cout << "Race Flag : " << raceFlagStr << " -> " << retValue << endl;

	return retValue;
}
int get_Mob_ImmuneFlag_Value(string inputString)
{
	string arImmuneFlag[] =
	{
		"STUN",
		"SLOW",
		"FALL",
		"CURSE",
		"POISON",
		"TERROR"
	};

	int retValue = 0;
	string* arInputString = StringSplit(inputString, ",");				//프로토 정보 내용을 단어별로 쪼갠 배열.
	for (int i = 0; i < sizeof(arImmuneFlag) / sizeof(arImmuneFlag[0]); i++) {
		string tempString = arImmuneFlag[i];
		for (int j = 0; j < 30; j++)		//최대 30개 단어까지. (하드코딩)
		{
			string tempString2 = arInputString[j];
			if (tempString2.compare(tempString) == 0) {				//일치하는지 확인.
				retValue = retValue + pow((float)2, (float)i);
			}

			if (tempString2.compare("") == 0)
				break;
		}
	}
	delete[]arInputString;
	//cout << "Immune Flag : " << immuneFlagStr << " -> " << retValue << endl;


	return retValue;
}
