#pragma once
#include "../GameLib/ItemData.h"

struct SAffects
{
	enum
	{
		AFFECT_MAX_NUM = 32,
	};

	SAffects() : dwAffects (0) {}
	SAffects (const DWORD& c_rAffects)
	{
		__SetAffects (c_rAffects);
	}
	int operator = (const DWORD& c_rAffects)
	{
		__SetAffects (c_rAffects);
	}

	BOOL IsAffect (BYTE byIndex)
	{
		return dwAffects & (1 << byIndex);
	}

	void __SetAffects (const DWORD& c_rAffects)
	{
		dwAffects = c_rAffects;
	}

	DWORD dwAffects;
};

extern std::string g_strGuildSymbolPathName;

const DWORD c_Name_Max_Length = 64;
const DWORD c_FileName_Max_Length = 128;
const DWORD c_Short_Name_Max_Length = 32;

const DWORD c_Inventory_Page_Size = 5*9; // x*y
const DWORD c_Inventory_Page_Count = 4;
const DWORD c_ItemSlot_Count = c_Inventory_Page_Size * c_Inventory_Page_Count;
const DWORD c_Equipment_Count = 12;

const DWORD c_Equipment_Start = c_ItemSlot_Count;

#ifdef ENABLE_ADDITIONAL_EQUIPMENT_PAGE
const DWORD c_Equipment_Body			= c_Equipment_Start + 0;
const DWORD c_Equipment_Head			= c_Equipment_Start + 1;
const DWORD c_Equipment_Shoes			= c_Equipment_Start + 2;
const DWORD c_Equipment_Wrist			= c_Equipment_Start + 3;
const DWORD c_Equipment_Weapon			= c_Equipment_Start + 4;
const DWORD c_Equipment_Neck			= c_Equipment_Start + 5;
const DWORD c_Equipment_Ear				= c_Equipment_Start + 6;
const DWORD c_Equipment_Arrow			= c_Equipment_Start + 7;
const DWORD c_Equipment_Shield			= c_Equipment_Start + 8;
const DWORD c_Equipment_Belt			= c_Equipment_Start + 9;
const DWORD c_Equipment_Pendant			= c_Equipment_Start + 10;
const DWORD c_Equipment_Glove			= c_Equipment_Start + 11;

const DWORD c_Second_Equipment_Body		= c_Equipment_Start + 12;
const DWORD c_Second_Equipment_Head		= c_Equipment_Start + 13;
const DWORD c_Second_Equipment_Shoes	= c_Equipment_Start + 14;
const DWORD c_Second_Equipment_Wrist	= c_Equipment_Start + 15;
const DWORD c_Second_Equipment_Weapon	= c_Equipment_Start + 16;
const DWORD c_Second_Equipment_Neck		= c_Equipment_Start + 17;
const DWORD c_Second_Equipment_Ear		= c_Equipment_Start + 18;
const DWORD c_Second_Equipment_Arrow	= c_Equipment_Start + 19;
const DWORD c_Second_Equipment_Shield	= c_Equipment_Start + 20;
const DWORD c_Second_Equipment_Belt		= c_Equipment_Start + 21;
const DWORD c_Second_Equipment_Pendant	= c_Equipment_Start + 22;
const DWORD c_Second_Equipment_Glove	= c_Equipment_Start + 23;

const DWORD c_Equipment_Second_Start	= c_Second_Equipment_Body;
#else
const DWORD c_Equipment_Body = c_Equipment_Start + 0;
const DWORD c_Equipment_Head = c_Equipment_Start + 1;
const DWORD c_Equipment_Shoes = c_Equipment_Start + 2;
const DWORD c_Equipment_Wrist = c_Equipment_Start + 3;
const DWORD c_Equipment_Weapon = c_Equipment_Start + 4;
const DWORD c_Equipment_Neck = c_Equipment_Start + 5;
const DWORD c_Equipment_Ear = c_Equipment_Start + 6;
const DWORD c_Equipment_Unique1 = c_Equipment_Start + 7;
const DWORD c_Equipment_Unique2 = c_Equipment_Start + 8;
const DWORD c_Equipment_Arrow = c_Equipment_Start + 9;
const DWORD c_Equipment_Shield = c_Equipment_Start + 10;
#endif

#ifdef ENABLE_NEW_EQUIPMENT_SYSTEM
		const DWORD c_New_Equipment_Start = c_Equipment_Start + 30;

	#ifdef ENABLE_ADDITIONAL_EQUIPMENT_PAGE
		const DWORD c_New_Equipment_Count = 2;
		const DWORD c_Equipment_Unique1 = c_New_Equipment_Start + 0;
		const DWORD c_Equipment_Unique2 = c_New_Equipment_Start + 1;
	#else
		const DWORD c_New_Equipment_Start = c_Equipment_Start + 21;
		const DWORD c_New_Equipment_Count = 3;

		const DWORD c_Equipment_Ring1 = c_New_Equipment_Start + 0;
		const DWORD c_Equipment_Ring2 = c_New_Equipment_Start + 1;
		const DWORD c_Equipment_Belt  = c_New_Equipment_Start + 2;
	#endif
#endif


#ifdef ENABLE_COSTUME_SYSTEM
	const DWORD c_Costume_Slot_Start	= c_Equipment_Start + 24;
	const DWORD	c_Costume_Slot_Body		= c_Costume_Slot_Start + 0;
	const DWORD	c_Costume_Slot_Hair		= c_Costume_Slot_Start + 1;
#ifdef ENABLE_COSTUME_WEAPON_SYSTEM
	const DWORD c_Costume_Slot_Weapon = c_Costume_Slot_Start + 2;
#endif
#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
	const DWORD	c_Costume_Slot_Mount	= c_Costume_Slot_Start + 3;
#endif
#ifdef ENABLE_ACCE_SYSTEM
	const DWORD	c_Costume_Slot_Acce		= c_Costume_Slot_Start + 4;
#endif
#ifdef ENABLE_AURA_SYSTEM
	const DWORD c_Costume_Slot_Aura		= c_Costume_Slot_Start + 5;
#endif
	const DWORD c_Costume_Slot_Count	= 6;
	const DWORD c_Costume_Slot_End		= c_Costume_Slot_Start + c_Costume_Slot_Count;
#endif

enum EDragonSoulDeckType
{
	DS_DECK_1,
	DS_DECK_2,
	DS_DECK_MAX_NUM = 2,
};

enum EDragonSoulGradeTypes
{
	DRAGON_SOUL_GRADE_NORMAL,
	DRAGON_SOUL_GRADE_BRILLIANT,
	DRAGON_SOUL_GRADE_RARE,
	DRAGON_SOUL_GRADE_ANCIENT,
	DRAGON_SOUL_GRADE_LEGENDARY,
#ifdef ENABLE_DS_GRADE_MYTH
	DRAGON_SOUL_GRADE_MYTH,
#endif
	DRAGON_SOUL_GRADE_MAX,

};

enum EDragonSoulStepTypes
{
	DRAGON_SOUL_STEP_LOWEST,
	DRAGON_SOUL_STEP_LOW,
	DRAGON_SOUL_STEP_MID,
	DRAGON_SOUL_STEP_HIGH,
	DRAGON_SOUL_STEP_HIGHEST,
	DRAGON_SOUL_STEP_MAX,
};


// [주의] 숫자(32) 하드코딩 주의. 현재 서버에서 용혼석 슬롯은 32부터임.
// 서버 common/length.h 파일의 EWearPositions 열거형이 32까지 확장될 것을 염두하고(32 이상은 확장 하기 힘들게 되어있음.),
// 그 이후부터를 용혼석 장착 슬롯으로 사용.
const DWORD c_Wear_Max = 32;
const DWORD c_DragonSoul_Equip_Start = c_ItemSlot_Count + c_Wear_Max;
const DWORD c_DragonSoul_Equip_Slot_Max = 6;
const DWORD c_DragonSoul_Equip_End = c_DragonSoul_Equip_Start + c_DragonSoul_Equip_Slot_Max * DS_DECK_MAX_NUM;

// NOTE: 2013년 2월 5일 현재... 용혼석 데크는 2개가 존재하는데, 향후 확장 가능성이 있어서 3개 데크 여유분을 할당 해 둠. 그 뒤 공간은 벨트 인벤토리로 사용
const DWORD c_DragonSoul_Equip_Reserved_Count = c_DragonSoul_Equip_Slot_Max * 3;

#ifdef ENABLE_NEW_EQUIPMENT_SYSTEM
	// 벨트 아이템이 제공하는 인벤토리
	const DWORD c_Belt_Inventory_Slot_Start = c_DragonSoul_Equip_End + c_DragonSoul_Equip_Reserved_Count;
	const DWORD c_Belt_Inventory_Width = 4;
	const DWORD c_Belt_Inventory_Height= 4;
	const DWORD c_Belt_Inventory_Slot_Count = c_Belt_Inventory_Width * c_Belt_Inventory_Height;
	const DWORD c_Belt_Inventory_Slot_End = c_Belt_Inventory_Slot_Start + c_Belt_Inventory_Slot_Count;

	const DWORD c_Inventory_Count	= c_Belt_Inventory_Slot_End;
#else
	const DWORD c_Inventory_Count	= c_DragonSoul_Equip_End;
#endif

// 용혼석 전용 인벤토리
const DWORD c_DragonSoul_Inventory_Start = 0;
const DWORD c_DragonSoul_Inventory_Box_Size = 32;
#ifdef ENABLE_EXTENDED_DS_INVENTORY
	const DWORD c_DragonSoul_Inventory_Page_Count = 3;
	const DWORD c_DragonSoul_Inventory_Count = CItemData::DS_SLOT_NUM_TYPES * DRAGON_SOUL_GRADE_MAX * c_DragonSoul_Inventory_Page_Count * c_DragonSoul_Inventory_Box_Size;
#else
	const DWORD c_DragonSoul_Inventory_Count = CItemData::DS_SLOT_NUM_TYPES * DRAGON_SOUL_GRADE_MAX * c_DragonSoul_Inventory_Box_Size;
#endif
const DWORD c_DragonSoul_Inventory_End = c_DragonSoul_Inventory_Start + c_DragonSoul_Inventory_Count;

#if defined(__BL_OFFICIAL_LOOT_FILTER__)
enum ELootFilter
{
	WEAPON_SELECT_DATA_MAX = 5,
	ARMOR_SELECT_DATA_MAX = 5,
	HEAD_SELECT_DATA_MAX = 5,
	COMMON_SELECT_DATA_MAX = 10,
	COSTUME_SELECT_DATA_MAX = 6,
	DS_SELECT_DATA_MAX = 2,
	UNIQUE_SELECT_DATA_MAX = 2,
	REFINE_SELECT_DATA_MAX = 3,
	POTION_SELECT_DATA_MAX = 3,
	FISH_MINING_SELECT_DATA_MAX = 3,
	MOUNT_PET_SELECT_DATA_MAX = 4,
	SKILL_BOOK_SELECT_DATA_MAX = 6,
	ETC_SELECT_DATA_MAX = 8,
	EVENT_SELECT_DATA_MAX = 0, //NOT USING

	WEAPON_ON_OFF = 0,
	WEAPON_REFINE_MIN,
	WEAPON_REFINE_MAX,
	WEAPON_WEARING_LEVEL_MIN,
	WEAPON_WEARING_LEVEL_MAX,
	WEAPON_SELECT_DATA_JOB_WARRIOR,
	WEAPON_SELECT_DATA_JOB_SURA,
	WEAPON_SELECT_DATA_JOB_ASSASSIN,
	WEAPON_SELECT_DATA_JOB_SHAMAN,
	WEAPON_SELECT_DATA_JOB_LYCAN,

	ARMOR_ON_OFF,
	ARMOR_REFINE_MIN,
	ARMOR_REFINE_MAX,
	ARMOR_WEARING_LEVEL_MIN,
	ARMOR_WEARING_LEVEL_MAX,
	ARMOR_SELECT_DATA_JOB_WARRIOR,
	ARMOR_SELECT_DATA_JOB_SURA,
	ARMOR_SELECT_DATA_JOB_ASSASSIN,
	ARMOR_SELECT_DATA_JOB_SHAMAN,
	ARMOR_SELECT_DATA_JOB_LYCAN,

	HEAD_ON_OFF,
	HEAD_REFINE_MIN,
	HEAD_REFINE_MAX,
	HEAD_WEARING_LEVEL_MIN,
	HEAD_WEARING_LEVEL_MAX,
	HEAD_SELECT_DATA_JOB_WARRIOR,
	HEAD_SELECT_DATA_JOB_SURA,
	HEAD_SELECT_DATA_JOB_ASSASSIN,
	HEAD_SELECT_DATA_JOB_SHAMAN,
	HEAD_SELECT_DATA_JOB_LYCAN,

	COMMON_ON_OFF,
	COMMON_REFINE_MIN,
	COMMON_REFINE_MAX,
	COMMON_WEARING_LEVEL_MIN,
	COMMON_WEARING_LEVEL_MAX,
	COMMON_SELECT_DATA_SHOE,
	COMMON_SELECT_DATA_BELT,
	COMMON_SELECT_DATA_BRACELET,
	COMMON_SELECT_DATA_NECKLACE,
	COMMON_SELECT_DATA_EARRING,
	COMMON_SELECT_DATA_SHIELD,
	COMMON_SELECT_DATA_GLOVE,
	COMMON_SELECT_DATA_TALISMAN,
	COMMON_SELECT_DATA_FISHING_ROD,
	COMMON_SELECT_DATA_PICKAXE,

	COSTUME_ON_OFF,
	COSTUME_SELECT_DATA_WEAPON,
	COSTUME_SELECT_DATA_ARMOR,
	COSTUME_SELECT_DATA_HAIR,
	COSTUME_SELECT_DATA_ACCE,
	COSTUME_SELECT_DATA_AURA,
	COSTUME_SELECT_DATA_ETC,

	DS_ON_OFF,
	DS_SELECT_DATA_DS,
	DS_SELECT_DATA_ETC,

	UNIQUE_ON_OFF,
	UNIQUE_SELECT_DATA_ABILITY,
	UNIQUE_SELECT_DATA_ETC,

	REFINE_ON_OFF,
	REFINE_SELECT_DATA_MATERIAL,
	REFINE_SELECT_DATA_STONE,
	REFINE_SELECT_DATA_ETC,

	POTION_ON_OFF,
	POTION_SELECT_DATA_ABILITY,
	POTION_SELECT_DATA_HAIRDYE,
	POTION_SELECT_DATA_ETC,

	FISH_MINING_ON_OFF,
	FISH_MINING_SELECT_DATA_FOOD,
	FISH_MINING_SELECT_DATA_STONE,
	FISH_MINING_SELECT_DATA_ETC,

	MOUNT_PET_ON_OFF,
	MOUNT_PET_SELECT_DATA_CHARGED_PET,
	MOUNT_PET_SELECT_DATA_MOUNT,
	MOUNT_PET_SELECT_DATA_FREE_PET,
	MOUNT_PET_SELECT_DATA_EGG,

	SKILL_BOOK_ON_OFF,
	SKILL_BOOK_SELECT_DATA_JOB_WARRIOR,
	SKILL_BOOK_SELECT_DATA_JOB_SURA,
	SKILL_BOOK_SELECT_DATA_JOB_ASSASSIN,
	SKILL_BOOK_SELECT_DATA_JOB_SHAMAN,
	SKILL_BOOK_SELECT_DATA_JOB_LYCAN,
	SKILL_BOOK_SELECT_DATA_JOB_PUBLIC,

	ETC_ON_OFF,
	ETC_SELECT_DATA_GIFTBOX,
	ETC_SELECT_DATA_MATRIMONY,
	ETC_SELECT_DATA_SEAL,
	ETC_SELECT_DATA_PARTY,
	ETC_SELECT_DATA_POLYMORPH,
	ETC_SELECT_DATA_RECIPE,
	ETC_SELECT_DATA_WEAPON_ARROW,
	ETC_SELECT_DATA_ETC,

	EVENT_ON_OFF,

	LOOT_FILTER_SETTINGS_MAX
};
#endif

enum ESlotType
{
	SLOT_TYPE_NONE,
	SLOT_TYPE_INVENTORY,
	SLOT_TYPE_SKILL,
	SLOT_TYPE_EMOTION,
	SLOT_TYPE_SHOP,
	SLOT_TYPE_EXCHANGE_OWNER,
	SLOT_TYPE_EXCHANGE_TARGET,
	SLOT_TYPE_QUICK_SLOT,
	SLOT_TYPE_SAFEBOX,
	SLOT_TYPE_PRIVATE_SHOP,
	SLOT_TYPE_MALL,
	SLOT_TYPE_DRAGON_SOUL_INVENTORY,
#ifdef ENABLE_AURA_SYSTEM
	SLOT_TYPE_AURA,
#endif
#if defined(__BL_TRANSMUTATION__)
	SLOT_TYPE_CHANGE_LOOK,
#endif
	SLOT_TYPE_MAX,
};

#ifdef ENABLE_ADDITIONAL_EQUIPMENT_PAGE
enum SecondEquipment
{
	SECOND_EQUIPMENT_COUNT = 12,
};
#endif

#ifdef ENABLE_AURA_SYSTEM
const BYTE c_AuraMaxLevel = 250;

enum EAuraRefineInfoSlot
{
	AURA_REFINE_INFO_SLOT_CURRENT,
	AURA_REFINE_INFO_SLOT_NEXT,
	AURA_REFINE_INFO_SLOT_EVOLVED,
	AURA_REFINE_INFO_SLOT_MAX
};

enum EAuraWindowType
{
	AURA_WINDOW_TYPE_ABSORB,
	AURA_WINDOW_TYPE_GROWTH,
	AURA_WINDOW_TYPE_EVOLVE,
	AURA_WINDOW_TYPE_MAX,
};

enum EAuraSlotType
{
	AURA_SLOT_MAIN,
	AURA_SLOT_SUB,
	AURA_SLOT_RESULT,
	AURA_SLOT_MAX
};

enum EAuraRefineInfoType
{
	AURA_REFINE_INFO_STEP,
	AURA_REFINE_INFO_LEVEL_MIN,
	AURA_REFINE_INFO_LEVEL_MAX,
	AURA_REFINE_INFO_NEED_EXP,
	AURA_REFINE_INFO_MATERIAL_VNUM,
	AURA_REFINE_INFO_MATERIAL_COUNT,
	AURA_REFINE_INFO_NEED_GOLD,
	AURA_REFINE_INFO_EVOLVE_PCT,
	AURA_REFINE_INFO_MAX
};
#endif

#ifdef WJ_ENABLE_TRADABLE_ICON
enum ETopWindowTypes
{
	ON_TOP_WND_NONE,
	ON_TOP_WND_SHOP,
	ON_TOP_WND_EXCHANGE,
	ON_TOP_WND_SAFEBOX,
	ON_TOP_WND_PRIVATE_SHOP,
	ON_TOP_WND_ITEM_COMB,
	ON_TOP_WND_PET_FEED,
#if defined(__BL_67_ATTR__)
	ON_TOP_WND_ATTR_67,
#endif

	ON_TOP_WND_MAX,
};
#endif

enum EWindows
{
	RESERVED_WINDOW,
	INVENTORY,				// 기본 인벤토리. (45칸 짜리가 2페이지 존재 = 90칸)
	EQUIPMENT,
	SAFEBOX,
	MALL,
	DRAGON_SOUL_INVENTORY,
	GROUND,					// NOTE: 2013년 2월5일 현재까지 unused.. 왜 있는거지???
	BELT_INVENTORY,			// NOTE: W2.1 버전에 새로 추가되는 벨트 슬롯 아이템이 제공하는 벨트 인벤토리
#ifdef ENABLE_AURA_SYSTEM
	AURA_REFINE,
#endif
	WINDOW_TYPE_MAX,
};

enum EDSInventoryMaxNum
{
	DS_INVENTORY_MAX_NUM = c_DragonSoul_Inventory_Count,
	DS_REFINE_WINDOW_MAX_NUM = 15,
};

#if defined(__BL_MOVE_COSTUME_ATTR__)
enum ECombSlotType
{
	COMB_WND_SLOT_MEDIUM,
	COMB_WND_SLOT_BASE,
	COMB_WND_SLOT_MATERIAL,
	COMB_WND_SLOT_RESULT,

	COMB_WND_SLOT_MAX
};
#endif

#if defined(__BL_TRANSMUTATION__)
enum class ETRANSMUTATIONTYPE : BYTE
{
	TRANSMUTATION_TYPE_MOUNT,
	TRANSMUTATION_TYPE_ITEM
};

enum class ETRANSMUTATIONSLOTTYPE : size_t
{
	TRANSMUTATION_SLOT_LEFT,
	TRANSMUTATION_SLOT_RIGHT,

	TRANSMUTATION_SLOT_MAX
};

enum class ETRANSMUTATIONSETTINGS : DWORD
{
	TRANSMUTATION_ITEM_PRICE = 50000000, // 50M
	TRANSMUTATION_MOUNT_PRICE = 30000000, // 30M
	TRANSMUTATION_TICKET_1 = 72326,
	TRANSMUTATION_TICKET_2 = 72341,
	TRANSMUTATION_CLEAR_SCROLL = 72325,
};
#endif

#pragma pack (push, 1)
#define WORD_MAX 0xffff

typedef struct SItemPos
{
	BYTE window_type;
	WORD cell;
	SItemPos()
	{
		window_type =     INVENTORY;
		cell = WORD_MAX;
	}
	SItemPos (BYTE _window_type, WORD _cell)
	{
		window_type = _window_type;
		cell = _cell;
	}

	// 기존에 cell의 형을 보면 BYTE가 대부분이지만, oi
	// 어떤 부분은 int, 어떤 부분은 WORD로 되어있어,
	// 가장 큰 자료형인 int로 받는다.
	//  int operator=(const int _cell)
	//  {
	//window_type = INVENTORY;
	//      cell = _cell;
	//      return cell;
	//  }
	bool IsValidCell()
	{
		switch (window_type)
		{
			case INVENTORY:
				return cell < c_Inventory_Count;
				break;
			case EQUIPMENT:
				return cell < c_DragonSoul_Equip_End;
				break;
			case DRAGON_SOUL_INVENTORY:
				return cell < (DS_INVENTORY_MAX_NUM);
				break;
			default:
				return false;
		}
	}
	bool IsEquipCell()
	{
		switch (window_type)
		{
			case INVENTORY:
			case EQUIPMENT:
				return (c_Equipment_Start + c_Wear_Max > cell) && (c_Equipment_Start <= cell);
				break;

			case BELT_INVENTORY:
			case DRAGON_SOUL_INVENTORY:
				return false;
				break;

			default:
				return false;
		}
	}

	#ifdef ENABLE_NEW_EQUIPMENT_SYSTEM
	bool IsBeltInventoryCell()
	{
		bool bResult = c_Belt_Inventory_Slot_Start <= cell && c_Belt_Inventory_Slot_End > cell;
		return bResult;
	}
	#endif

	bool IsNPOS()
	{
		return (window_type == RESERVED_WINDOW && cell == WORD_MAX);
	}

	bool operator== (const struct SItemPos& rhs) const
	{
		return (window_type == rhs.window_type) && (cell == rhs.cell);
	}

	bool operator< (const struct SItemPos& rhs) const
	{
		return (window_type < rhs.window_type) || ((window_type == rhs.window_type) && (cell < rhs.cell));
	}
} TItemPos;
const TItemPos NPOS(RESERVED_WINDOW, WORD_MAX);
#pragma pack(pop)

const DWORD c_QuickBar_Line_Count = 3;
const DWORD c_QuickBar_Slot_Count = 12;

const float c_Idle_WaitTime = 5.0f;

const int c_Monster_Race_Start_Number = 6;
const int c_Monster_Model_Start_Number = 20001;

const float c_fAttack_Delay_Time = 0.2f;
const float c_fHit_Delay_Time = 0.1f;
const float c_fCrash_Wave_Time = 0.2f;
const float c_fCrash_Wave_Distance = 3.0f;

const float c_fHeight_Step_Distance = 50.0f;

enum
{
	DISTANCE_TYPE_FOUR_WAY,
	DISTANCE_TYPE_EIGHT_WAY,
	DISTANCE_TYPE_ONE_WAY,
	DISTANCE_TYPE_MAX_NUM,
};

const float c_fMagic_Script_Version = 1.0f;
const float c_fSkill_Script_Version = 1.0f;
const float c_fMagicSoundInformation_Version = 1.0f;
const float c_fBattleCommand_Script_Version = 1.0f;
const float c_fEmotionCommand_Script_Version = 1.0f;
const float c_fActive_Script_Version = 1.0f;
const float c_fPassive_Script_Version = 1.0f;

// Used by PushMove
const float c_fWalkDistance = 175.0f;
const float c_fRunDistance = 310.0f;

#define FILE_MAX_LEN 128

enum
{
	ITEM_SOCKET_SLOT_MAX_NUM = 3,
	ITEM_ATTRIBUTE_SLOT_MAX_NUM = 7,
#if defined(ENABLE_APPLY_RANDOM)
	ITEM_APPLY_RANDOM_SLOT_MAX_NUM = 3,
#endif
};

#pragma pack(push)
#pragma pack(1)

typedef struct SQuickSlot
{
	BYTE Type;
	BYTE Position;
} TQuickSlot;

typedef struct TPlayerItemAttribute
{
	BYTE        bType;
	short       sValue;
#if defined(ENABLE_APPLY_RANDOM)
	BYTE bPath;
#endif
} TPlayerItemAttribute;

typedef struct packet_item
{
	DWORD       vnum;
	BYTE        count;
	DWORD		flags;
	DWORD		anti_flags;
	long		alSockets[ITEM_SOCKET_SLOT_MAX_NUM];
#if defined(ENABLE_APPLY_RANDOM)
	TPlayerItemAttribute aApplyRandom[ITEM_APPLY_RANDOM_SLOT_MAX_NUM];
#endif
	TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_SLOT_MAX_NUM];
#if defined(__BL_TRANSMUTATION__)
	DWORD		dwTransmutationVnum;
#endif
} TItemData;

typedef struct packet_shop_item
{
	DWORD       vnum;
	DWORD       price;
	BYTE        count;
	BYTE		display_pos;
	long		alSockets[ITEM_SOCKET_SLOT_MAX_NUM];
#if defined(ENABLE_APPLY_RANDOM)
	TPlayerItemAttribute aApplyRandom[ITEM_APPLY_RANDOM_SLOT_MAX_NUM];
#endif
	TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_SLOT_MAX_NUM];
#if defined(__BL_TRANSMUTATION__)
	DWORD		dwTransmutationVnum;
#endif
} TShopItemData;

#pragma pack(pop)

inline float GetSqrtDistance (int ix1, int iy1, int ix2, int iy2) // By sqrt
{
	float dx, dy;

	dx = float (ix1 - ix2);
	dy = float (iy1 - iy2);

	return sqrtf (dx*dx + dy*dy);
}

// DEFAULT_FONT
void DefaultFont_Startup();
void DefaultFont_Cleanup();
void DefaultFont_SetName (const char* c_szFontName);
CResource* DefaultFont_GetResource();
CResource* DefaultItalicFont_GetResource();
// END_OF_DEFAULT_FONT

void SetGuildSymbolPath (const char* c_szPathName);
const char* GetGuildSymbolFileName (DWORD dwGuildID);
BYTE SlotTypeToInvenType (BYTE bSlotType);
#ifdef ENABLE_AURA_SYSTEM
int* GetAuraRefineInfo(BYTE bLevel);
#endif