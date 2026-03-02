#include "stdafx.h"

#include <stack>

#include "utils.h"
#include "config.h"
#include "char.h"
#include "char_manager.h"
#include "item_manager.h"
#include "desc.h"
#include "desc_client.h"
#include "desc_manager.h"
#include "packet.h"
#include "affect.h"
#include "skill.h"
#include "start_position.h"
#include "mob_manager.h"
#include "db.h"
#include "log.h"
#include "vector.h"
#include "buffer_manager.h"
#include "questmanager.h"
#include "fishing.h"
#include "party.h"
#include "dungeon.h"
#include "refine.h"
#include "unique_item.h"
#include "war_map.h"
#include "xmas_event.h"
#include "marriage.h"
#include "polymorph.h"
#include "blend_item.h"
#include "arena.h"

#include "safebox.h"
#include "shop.h"

#include "../../common/VnumHelper.h"
#include "DragonSoul.h"
#include "buff_on_attributes.h"
#include "belt_inventory_helper.h"
#if defined(__BL_OFFICIAL_LOOT_FILTER__)
#include "loot_filter.h"
#endif
const int ITEM_BROKEN_METIN_VNUM = 28960;

// CHANGE_ITEM_ATTRIBUTES
const DWORD CHARACTER::msc_dwDefaultChangeItemAttrCycle = 10;
const char CHARACTER::msc_szLastChangeItemAttrFlag[] = "Item.LastChangeItemAttr";
const char CHARACTER::msc_szChangeItemAttrCycleFlag[] = "change_itemattr_cycle";
// END_OF_CHANGE_ITEM_ATTRIBUTES
const BYTE g_aBuffOnAttrPoints[] = { POINT_ENERGY, POINT_COSTUME_ATTR_BONUS };

struct FFindStone
{
	std::map<DWORD, LPCHARACTER> m_mapStone;

	void operator() (LPENTITY pEnt)
	{
		if (pEnt->IsType (ENTITY_CHARACTER) == true)
		{
			LPCHARACTER pChar = (LPCHARACTER)pEnt;

			if (pChar->IsStone() == true)
			{
				m_mapStone[ (DWORD)pChar->GetVID()] = pChar;
			}
		}
	}
};


//귀환부, 귀환기억부, 결혼반지
static bool IS_SUMMON_ITEM (int vnum)
{
	switch (vnum)
	{
		case 22000:
		case 22010:
		case 22011:
		case 22020:
		case ITEM_MARRIAGE_RING:
			return true;
	}

	return false;
}

static bool IS_MONKEY_DUNGEON (int map_index)
{
	switch (map_index)
	{
		case 5:
		case 25:
		case 45:
		case 108:
		case 109:
			return true;;
	}

	return false;
}

bool IS_SUMMONABLE_ZONE (int map_index)
{
	// 몽키던전
	if (IS_MONKEY_DUNGEON (map_index))
	{
		return false;
	}

	switch (map_index)
	{
		case 66 : // 사귀타워
		case 71 : // 거미 던전 2층
		case 72 : // 천의 동굴
		case 73 : // 천의 동굴 2층
		case 193 : // 거미 던전 2-1층
			#if 0
		case 184 : // 천의 동굴(신수)
		case 185 : // 천의 동굴 2층(신수)
		case 186 : // 천의 동굴(천조)
		case 187 : // 천의 동굴 2층(천조)
		case 188 : // 천의 동굴(진노)
		case 189 : // 천의 동굴 2층(진노)
			#endif
//		case 206 : // 아귀동굴
		case 216 : // 아귀동굴
		case 217 : // 거미 던전 3층
		case 208 : // 천의 동굴 (용방)
			return false;
	}

	// 모든 private 맵으론 워프 불가능
	if (map_index > 10000)
	{
		return false;
	}

	return true;
}

bool IS_BOTARYABLE_ZONE (int nMapIndex)
{
	if (!g_bEnableBootaryCheck)
	{
		return true;
	}

	switch (nMapIndex)
	{
		case 1 :
		case 3 :
		case 21 :
		case 23 :
		case 41 :
		case 43 :
			return true;
	}

	return false;
}

// item socket 이 프로토타입과 같은지 체크 -- by mhh
static bool FN_check_item_socket (LPITEM item)
{
	for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
	{
		if (item->GetSocket (i) != item->GetProto()->alSockets[i])
		{
			return false;
		}
	}

	return true;
}

// item socket 복사 -- by mhh
static void FN_copy_item_socket (LPITEM dest, LPITEM src)
{
	for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
	{
		dest->SetSocket (i, src->GetSocket (i));
	}
}
static bool FN_check_item_sex (LPCHARACTER ch, LPITEM item)
{
	// 남자 금지
	if (IS_SET (item->GetAntiFlag(), ITEM_ANTIFLAG_MALE))
	{
		if (SEX_MALE==GET_SEX (ch))
		{
			return false;
		}
	}
	// 여자금지
	if (IS_SET (item->GetAntiFlag(), ITEM_ANTIFLAG_FEMALE))
	{
		if (SEX_FEMALE==GET_SEX (ch))
		{
			return false;
		}
	}

	return true;
}


/////////////////////////////////////////////////////////////////////////////
// ITEM HANDLING
/////////////////////////////////////////////////////////////////////////////
bool CHARACTER::CanHandleItem (bool bSkipCheckRefine, bool bSkipObserver)
{
	if (!bSkipObserver)
		if (m_bIsObserver)
		{
			return false;
		}

	if (GetMyShop())
	{
		return false;
	}

	if (!bSkipCheckRefine)
		if (m_bUnderRefine)
		{
			return false;
		}

	if (IsCubeOpen() || NULL != DragonSoul_RefineWindow_GetOpener())
	{
		return false;
	}
#if defined(__BL_TRANSMUTATION__)
	if (GetTransmutation())
		return false;
#endif
#if defined(__BL_MOVE_COSTUME_ATTR__)
	if (IsItemComb())
		return false;
#endif

	if (IsWarping())
	{
		return false;
	}
#ifdef ENABLE_ACCE_SYSTEM
	if ((m_bAcceCombination) || (m_bAcceAbsorption))
		return false;
#endif
#ifdef __AURA_SYSTEM__
	if (IsAuraRefineWindowOpen() || NULL != GetAuraRefineWindowOpener())
		return false;
#endif
#if defined(__BL_67_ATTR__)
	if (Is67AttrOpen())
		return false;
#endif
	return true;
}

LPITEM CHARACTER::GetInventoryItem (WORD wCell) const
{
	return GetItem (TItemPos (INVENTORY, wCell));
}
LPITEM CHARACTER::GetItem (TItemPos Cell) const
{
	if (!IsValidItemPosition (Cell))
	{
		return NULL;
	}
	WORD wCell = Cell.cell;
	BYTE window_type = Cell.window_type;
	switch (window_type)
	{
		case INVENTORY:
		case EQUIPMENT:
			if (wCell >= INVENTORY_AND_EQUIP_SLOT_MAX)
			{
				sys_err ("CHARACTER::GetInventoryItem: invalid item cell %d", wCell);
				return NULL;
			}
			return m_pointsInstant.pItems[wCell];
		case DRAGON_SOUL_INVENTORY:
			if (wCell >= DRAGON_SOUL_INVENTORY_MAX_NUM)
			{
				sys_err ("CHARACTER::GetInventoryItem: invalid DS item cell %d", wCell);
				return NULL;
			}
			return m_pointsInstant.pDSItems[wCell];

		default:
			return NULL;
	}
	return NULL;
}

#if defined(__BL_ENABLE_PICKUP_ITEM_EFFECT__)
void CHARACTER::SetItem(TItemPos Cell, LPITEM pItem, bool bHighlight)
#else
void CHARACTER::SetItem(TItemPos Cell, LPITEM pItem)
#endif
{
	WORD wCell = Cell.cell;
	BYTE window_type = Cell.window_type;
	if ((unsigned long) ((CItem*)pItem) == 0xff || (unsigned long) ((CItem*)pItem) == 0xffffffff)
	{
		sys_err ("!!! FATAL ERROR !!! item == 0xff (char: %s cell: %u)", GetName(), wCell);
		core_dump();
		return;
	}

	if (pItem && pItem->GetOwner())
	{
		assert (!"GetOwner exist");
		return;
	}
	// 기본 인벤토리
	switch (window_type)
	{
		case INVENTORY:
		case EQUIPMENT:
		{
			if (wCell >= INVENTORY_AND_EQUIP_SLOT_MAX)
			{
				sys_err ("CHARACTER::SetItem: invalid item cell %d", wCell);
				return;
			}

			LPITEM pOld = m_pointsInstant.pItems[wCell];

			if (pOld)
			{
				if (wCell < INVENTORY_MAX_NUM)
				{
					for (int i = 0; i < pOld->GetSize(); ++i)
					{
						int p = wCell + (i * 5);

						if (p >= INVENTORY_MAX_NUM)
						{
							continue;
						}

						if (m_pointsInstant.pItems[p] && m_pointsInstant.pItems[p] != pOld)
						{
							continue;
						}

						m_pointsInstant.bItemGrid[p] = 0;
					}
				}
				else
				{
					m_pointsInstant.bItemGrid[wCell] = 0;
				}
			}

			if (pItem)
			{
				if (wCell < INVENTORY_MAX_NUM)
				{
					for (int i = 0; i < pItem->GetSize(); ++i)
					{
						int p = wCell + (i * 5);

						if (p >= INVENTORY_MAX_NUM)
						{
							continue;
						}

						// wCell + 1 로 하는 것은 빈곳을 체크할 때 같은
						// 아이템은 예외처리하기 위함
						m_pointsInstant.bItemGrid[p] = wCell + 1;
					}
				}
				else
				{
					m_pointsInstant.bItemGrid[wCell] = wCell + 1;
				}
			}

			m_pointsInstant.pItems[wCell] = pItem;
		}
		break;
		// 용혼석 인벤토리
		case DRAGON_SOUL_INVENTORY:
		{
			LPITEM pOld = m_pointsInstant.pDSItems[wCell];

			if (pOld)
			{
				if (wCell < DRAGON_SOUL_INVENTORY_MAX_NUM)
				{
					for (int i = 0; i < pOld->GetSize(); ++i)
					{
						int p = wCell + (i * DRAGON_SOUL_BOX_COLUMN_NUM);

						if (p >= DRAGON_SOUL_INVENTORY_MAX_NUM)
						{
							continue;
						}

						if (m_pointsInstant.pDSItems[p] && m_pointsInstant.pDSItems[p] != pOld)
						{
							continue;
						}

						m_pointsInstant.wDSItemGrid[p] = 0;
					}
				}
				else
				{
					m_pointsInstant.wDSItemGrid[wCell] = 0;
				}
			}

			if (pItem)
			{
				if (wCell >= DRAGON_SOUL_INVENTORY_MAX_NUM)
				{
					sys_err ("CHARACTER::SetItem: invalid DS item cell %d", wCell);
					return;
				}

				if (wCell < DRAGON_SOUL_INVENTORY_MAX_NUM)
				{
					for (int i = 0; i < pItem->GetSize(); ++i)
					{
						int p = wCell + (i * DRAGON_SOUL_BOX_COLUMN_NUM);

						if (p >= DRAGON_SOUL_INVENTORY_MAX_NUM)
						{
							continue;
						}

						// wCell + 1 로 하는 것은 빈곳을 체크할 때 같은
						// 아이템은 예외처리하기 위함
						m_pointsInstant.wDSItemGrid[p] = wCell + 1;
					}
				}
				else
				{
					m_pointsInstant.wDSItemGrid[wCell] = wCell + 1;
				}
			}

			m_pointsInstant.pDSItems[wCell] = pItem;
		}
		break;
		default:
			sys_err ("Invalid Inventory type %d", window_type);
			return;
	}

	if (GetDesc())
	{
		// 확장 아이템: 서버에서 아이템 플래그 정보를 보낸다
		if (pItem)
		{
			TPacketGCItemSet pack;
			pack.header = HEADER_GC_ITEM_SET;
			pack.Cell = Cell;

			pack.count = pItem->GetCount();
			pack.vnum = pItem->GetVnum();
			pack.flags = pItem->GetFlag();
			pack.anti_flags	= pItem->GetAntiFlag();
#if defined(__BL_ENABLE_PICKUP_ITEM_EFFECT__)
			pack.highlight = bHighlight;
#else
			pack.highlight = (Cell.window_type == DRAGON_SOUL_INVENTORY);
#endif


			thecore_memcpy (pack.alSockets, pItem->GetSockets(), sizeof (pack.alSockets));
#if defined(__ITEM_APPLY_RANDOM__)
			thecore_memcpy(pack.aApplyRandom, pItem->GetRandomApplies(), sizeof(pack.aApplyRandom));
#endif
#if defined(__BL_TRANSMUTATION__)
			pack.dwTransmutationVnum = pItem->GetTransmutationVnum();
#endif
			thecore_memcpy (pack.aAttr, pItem->GetAttributes(), sizeof (pack.aAttr));

			GetDesc()->Packet (&pack, sizeof (TPacketGCItemSet));
		}
		else
		{
			TPacketGCItemDelDeprecated pack;
			pack.header = HEADER_GC_ITEM_DEL;
			pack.Cell = Cell;
			pack.count = 0;
			pack.vnum = 0;
			memset (pack.alSockets, 0, sizeof (pack.alSockets));
#if defined(__ITEM_APPLY_RANDOM__)
			memset(pack.aApplyRandom, 0, sizeof(pack.aApplyRandom));
#endif
#if defined(__BL_TRANSMUTATION__)
			pack.dwTransmutationVnum = 0;
#endif
			memset (pack.aAttr, 0, sizeof (pack.aAttr));

			GetDesc()->Packet (&pack, sizeof (TPacketGCItemDelDeprecated));
		}
	}

	if (pItem)
	{
		pItem->SetCell (this, wCell);
		switch (window_type)
		{
			case INVENTORY:
			case EQUIPMENT:
				if ((wCell < INVENTORY_MAX_NUM) || (BELT_INVENTORY_SLOT_START <= wCell && BELT_INVENTORY_SLOT_END > wCell))
				{
					pItem->SetWindow (INVENTORY);
				}
				else
				{
					pItem->SetWindow (EQUIPMENT);
				}
				break;
			case DRAGON_SOUL_INVENTORY:
				pItem->SetWindow (DRAGON_SOUL_INVENTORY);
				break;
		}
	}
}

LPITEM CHARACTER::GetWear (BYTE bCell) const
{
	// > WEAR_MAX_NUM : 용혼석 슬롯들.
	if (bCell >= WEAR_MAX_NUM + DRAGON_SOUL_DECK_MAX_NUM * DS_SLOT_MAX)
	{
		sys_err ("CHARACTER::GetWear: invalid wear cell %d", bCell);
		return NULL;
	}

	return m_pointsInstant.pItems[INVENTORY_MAX_NUM + bCell];
}

void CHARACTER::SetWear (BYTE bCell, LPITEM item)
{
	// > WEAR_MAX_NUM : 용혼석 슬롯들.
	if (bCell >= WEAR_MAX_NUM + DRAGON_SOUL_DECK_MAX_NUM * DS_SLOT_MAX)
	{
		sys_err ("CHARACTER::SetItem: invalid item cell %d", bCell);
		return;
	}

#if defined(__BL_ENABLE_PICKUP_ITEM_EFFECT__)
	SetItem(TItemPos(INVENTORY, INVENTORY_MAX_NUM + bCell), item, false);
#else
	SetItem(TItemPos (INVENTORY, INVENTORY_MAX_NUM + bCell), item);
#endif

#ifdef ENABLE_ADDITIONAL_EQUIPMENT_PAGE
	if (!item && (bCell == WEAR_WEAPON || bCell == WEAR_SECOND_BODY))
#else
	if (!item && bCell == WEAR_WEAPON)
#endif
	{
		// 귀검 사용 시 벗는 것이라면 효과를 없애야 한다.
		if (IsAffectFlag (AFF_GWIGUM))
		{
			RemoveAffect (SKILL_GWIGEOM);
		}

		if (IsAffectFlag (AFF_GEOMGYEONG))
		{
			RemoveAffect (SKILL_GEOMKYUNG);
		}
	}
}

void CHARACTER::ClearItem()
{
	int		i;
	LPITEM	item;

	for (i = 0; i < INVENTORY_AND_EQUIP_SLOT_MAX; ++i)
	{
		if ((item = GetInventoryItem (i)))
		{
			item->SetSkipSave (true);
			ITEM_MANAGER::instance().FlushDelayedSave (item);

			item->RemoveFromCharacter();
			M2_DESTROY_ITEM (item);

			SyncQuickslot (QUICKSLOT_TYPE_ITEM, i, 255);
		}
	}
	for (i = 0; i < DRAGON_SOUL_INVENTORY_MAX_NUM; ++i)
	{
		if ((item = GetItem (TItemPos (DRAGON_SOUL_INVENTORY, i))))
		{
			item->SetSkipSave (true);
			ITEM_MANAGER::instance().FlushDelayedSave (item);

			item->RemoveFromCharacter();
			M2_DESTROY_ITEM (item);
		}
	}
}

#ifdef ENABLE_ADDITIONAL_EQUIPMENT_PAGE
bool CHARACTER::IsEmptyItemGrid(TItemPos Cell, BYTE bSize, int iExceptionCell)
#else
bool CHARACTER::IsEmptyItemGrid(TItemPos Cell, BYTE bSize, int iExceptionCell) const
#endif
{
	switch (Cell.window_type)
	{
		case INVENTORY:
		{
			BYTE bCell = Cell.cell;

			// bItemCell은 0이 false임을 나타내기 위해 + 1 해서 처리한다.
			// 따라서 iExceptionCell에 1을 더해 비교한다.
			++iExceptionCell;

			if (Cell.IsBeltInventoryPosition())
			{
#ifdef ENABLE_ADDITIONAL_EQUIPMENT_PAGE
				LPITEM beltItem = GetEquipWear(WEAR_BELT);
#else
				LPITEM beltItem = GetWear(WEAR_BELT);
#endif

				if (NULL == beltItem)
				{
					return false;
				}

				if (false == CBeltInventoryHelper::IsAvailableCell (bCell - BELT_INVENTORY_SLOT_START, beltItem->GetValue (0)))
				{
					return false;
				}

				if (m_pointsInstant.bItemGrid[bCell])
				{
					if (m_pointsInstant.bItemGrid[bCell] == iExceptionCell)
					{
						return true;
					}

					return false;
				}

				if (bSize == 1)
				{
					return true;
				}

			}
			else if (bCell >= INVENTORY_MAX_NUM)
			{
				return false;
			}

			if (m_pointsInstant.bItemGrid[bCell])
			{
				if (m_pointsInstant.bItemGrid[bCell] == iExceptionCell)
				{
					if (bSize == 1)
					{
						return true;
					}

					int j = 1;
					BYTE bPage = bCell / (INVENTORY_MAX_NUM / INVENTORY_PAGE_COUNT);

					do
					{
						BYTE p = bCell + (5 * j);

						if (p >= INVENTORY_MAX_NUM)
						{
							return false;
						}

						if (p / (INVENTORY_MAX_NUM / INVENTORY_PAGE_COUNT) != bPage)
						{
							return false;
						}

						if (m_pointsInstant.bItemGrid[p])
							if (m_pointsInstant.bItemGrid[p] != iExceptionCell)
							{
								return false;
							}
					}
					while (++j < bSize);

					return true;
				}
				else
				{
					return false;
				}
			}

			// 크기가 1이면 한칸을 차지하는 것이므로 그냥 리턴
			if (1 == bSize)
			{
				return true;
			}
			else
			{
				int j = 1;
				BYTE bPage = bCell / (INVENTORY_MAX_NUM / INVENTORY_PAGE_COUNT);

				do
				{
					BYTE p = bCell + (5 * j);

					if (p >= INVENTORY_MAX_NUM)
					{
						return false;
					}

					if (p / (INVENTORY_MAX_NUM / INVENTORY_PAGE_COUNT) != bPage)
					{
						return false;
					}

					if (m_pointsInstant.bItemGrid[p])
						if (m_pointsInstant.bItemGrid[p] != iExceptionCell)
						{
							return false;
						}
				}
				while (++j < bSize);

				return true;
			}
		}
		break;
		case DRAGON_SOUL_INVENTORY:
		{
			WORD wCell = Cell.cell;
			if (wCell >= DRAGON_SOUL_INVENTORY_MAX_NUM)
			{
				return false;
			}

			// bItemCell은 0이 false임을 나타내기 위해 + 1 해서 처리한다.
			// 따라서 iExceptionCell에 1을 더해 비교한다.
			iExceptionCell++;

			if (m_pointsInstant.wDSItemGrid[wCell])
			{
				if (m_pointsInstant.wDSItemGrid[wCell] == iExceptionCell)
				{
					if (bSize == 1)
					{
						return true;
					}

					int j = 1;

					do
					{
						BYTE p = wCell + (DRAGON_SOUL_BOX_COLUMN_NUM * j);

						if (p >= DRAGON_SOUL_INVENTORY_MAX_NUM)
						{
							return false;
						}

						if (m_pointsInstant.wDSItemGrid[p])
							if (m_pointsInstant.wDSItemGrid[p] != iExceptionCell)
							{
								return false;
							}
					}
					while (++j < bSize);

					return true;
				}
				else
				{
					return false;
				}
			}

			// 크기가 1이면 한칸을 차지하는 것이므로 그냥 리턴
			if (1 == bSize)
			{
				return true;
			}
			else
			{
				int j = 1;

				do
				{
					BYTE p = wCell + (DRAGON_SOUL_BOX_COLUMN_NUM * j);

					if (p >= DRAGON_SOUL_INVENTORY_MAX_NUM)
					{
						return false;
					}

					if (m_pointsInstant.bItemGrid[p])
						if (m_pointsInstant.wDSItemGrid[p] != iExceptionCell)
						{
							return false;
						}
				}
				while (++j < bSize);

				return true;
			}
		}
	}

	return true;
}

#ifdef ENABLE_ADDITIONAL_EQUIPMENT_PAGE
int CHARACTER::GetEmptyInventory(BYTE size)
#else
int CHARACTER::GetEmptyInventory(BYTE size) const
#endif
{
	// NOTE: 현재 이 함수는 아이템 지급, 획득 등의 행위를 할 때 인벤토리의 빈 칸을 찾기 위해 사용되고 있는데,
	//		벨트 인벤토리는 특수 인벤토리이므로 검사하지 않도록 한다. (기본 인벤토리: INVENTORY_MAX_NUM 까지만 검사)
	for (int i = 0; i < INVENTORY_MAX_NUM; ++i)
		if (IsEmptyItemGrid (TItemPos (INVENTORY, i), size))
		{
			return i;
		}
	return -1;
}

#ifdef ENABLE_ADDITIONAL_EQUIPMENT_PAGE
int CHARACTER::GetEmptyDragonSoulInventory(LPITEM pItem)
#else
int CHARACTER::GetEmptyDragonSoulInventory(LPITEM pItem) const
#endif
{
	if (NULL == pItem || !pItem->IsDragonSoul())
		return -1;
	if (!DragonSoul_IsQualified())
	{
		return -1;
	}
	BYTE bSize = pItem->GetSize();
	WORD wBaseCell = DSManager::instance().GetBasePosition(pItem);

	if (WORD_MAX == wBaseCell)
		return -1;

#ifdef ENABLE_EXTENDED_DS_INVENTORY
	for (int i = 0; i < (DRAGON_SOUL_BOX_SIZE * DRAGON_SOUL_INVENTORY_PAGE_COUNT); ++i)
#else
	for (int i = 0; i < DRAGON_SOUL_BOX_SIZE; ++i)
#endif
		if (IsEmptyItemGrid(TItemPos(DRAGON_SOUL_INVENTORY, i + wBaseCell), bSize))
			return i + wBaseCell;

	return -1;
}

#ifdef ENABLE_DRAGONSOUL_INVENTORY_BOX_SIZE
#ifdef ENABLE_ADDITIONAL_EQUIPMENT_PAGE
int CHARACTER::GetEmptyDragonSoulInventoryType(BYTE type, BYTE ds_type)
#else
inint CHARACTER::GetEmptyDragonSoulInventoryType(BYTE type, BYTE ds_type) const
#endif
{
	/*if (!DragonSoul_IsQualified())
	{
		return -1;
	}*/

	BYTE bSize = 1;

	if (type == 0)
	{
		WORD wBaseCell = ((192 + 32) * 0) + (ds_type * 32);

		if (WORD_MAX == wBaseCell)
			return -1;

		for (int i = 0; i < DRAGON_SOUL_BOX_SIZE; ++i)
			if (IsEmptyItemGrid(TItemPos(DRAGON_SOUL_INVENTORY, i + wBaseCell), bSize))
				return i + wBaseCell;
	}
	else if (type == 1)
	{
		WORD wBaseCell = ((192 + 32) * 1) + (ds_type * 32);

		if (WORD_MAX == wBaseCell)
			return -1;

		for (int i = 0; i < DRAGON_SOUL_BOX_SIZE; ++i)
			if (IsEmptyItemGrid(TItemPos(DRAGON_SOUL_INVENTORY, i + wBaseCell), bSize))
				return i + wBaseCell;
	}
	else if (type == 2)
	{
		WORD wBaseCell = ((192 + 32) * 2) + (ds_type * 32);

		if (WORD_MAX == wBaseCell)
			return -1;

		for (int i = 0; i < DRAGON_SOUL_BOX_SIZE; ++i)
			if (IsEmptyItemGrid(TItemPos(DRAGON_SOUL_INVENTORY, i + wBaseCell), bSize))
				return i + wBaseCell;
	}
	else if (type == 3)
	{
		WORD wBaseCell = ((192 + 32) * 3) + (ds_type * 32);

		if (WORD_MAX == wBaseCell)
			return -1;

		for (int i = 0; i < DRAGON_SOUL_BOX_SIZE; ++i)
			if (IsEmptyItemGrid(TItemPos(DRAGON_SOUL_INVENTORY, i + wBaseCell), bSize))
				return i + wBaseCell;
	}
	else if (type == 4)
	{
		WORD wBaseCell = ((192 + 32) * 4) + (ds_type * 32);

		if (WORD_MAX == wBaseCell)
			return -1;

		for (int i = 0; i < DRAGON_SOUL_BOX_SIZE; ++i)
			if (IsEmptyItemGrid(TItemPos(DRAGON_SOUL_INVENTORY, i + wBaseCell), bSize))
				return i + wBaseCell;
	}
	else if (type == 5)
	{
		WORD wBaseCell = ((192 + 32) * 5) + (ds_type * 32);

		if (WORD_MAX == wBaseCell)
			return -1;

		for (int i = 0; i < DRAGON_SOUL_BOX_SIZE; ++i)
			if (IsEmptyItemGrid(TItemPos(DRAGON_SOUL_INVENTORY, i + wBaseCell), bSize))
				return i + wBaseCell;
	}
	else if (type == 6)
	{
		WORD wBaseCell = ((192 + 32) * 6) + (ds_type * 32);

		if (WORD_MAX == wBaseCell)
			return -1;

		for (int i = 0; i < DRAGON_SOUL_BOX_SIZE; ++i)
			if (IsEmptyItemGrid(TItemPos(DRAGON_SOUL_INVENTORY, i + wBaseCell), bSize))
				return i + wBaseCell;
	}

	return -1;
}
#endif




void CHARACTER::CopyDragonSoulItemGrid (std::vector<WORD>& vDragonSoulItemGrid) const
{
	vDragonSoulItemGrid.resize (DRAGON_SOUL_INVENTORY_MAX_NUM);

	std::copy (m_pointsInstant.wDSItemGrid, m_pointsInstant.wDSItemGrid + DRAGON_SOUL_INVENTORY_MAX_NUM, vDragonSoulItemGrid.begin());
}

int CHARACTER::CountEmptyInventory() const
{
	int	count = 0;

	for (int i = 0; i < INVENTORY_MAX_NUM; ++i)
		if (GetInventoryItem (i))
		{
			count += GetInventoryItem (i)->GetSize();
		}

	return (INVENTORY_MAX_NUM - count);
}

void TransformRefineItem (LPITEM pkOldItem, LPITEM pkNewItem)
{
	// ACCESSORY_REFINE
	if (pkOldItem->IsAccessoryForSocket())
	{
		for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
		{
			pkNewItem->SetSocket (i, pkOldItem->GetSocket (i));
		}
		//pkNewItem->StartAccessorySocketExpireEvent();
	}
	// END_OF_ACCESSORY_REFINE
	else
	{
		// 여기서 깨진석이 자동적으로 청소 됨
		for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
		{
			if (!pkOldItem->GetSocket (i))
			{
				break;
			}
			else
			{
				pkNewItem->SetSocket (i, 1);
			}
		}

		// 소켓 설정
		int slot = 0;

		for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
		{
			long socket = pkOldItem->GetSocket (i);

			if (socket > 2 && socket != ITEM_BROKEN_METIN_VNUM)
			{
				pkNewItem->SetSocket (slot++, socket);
			}
		}

	}

	// 매직 아이템 설정
	pkOldItem->CopyAttributeTo (pkNewItem);
}

void NotifyRefineSuccess (LPCHARACTER ch, LPITEM item, const char* way)
{
	if (NULL != ch && item != NULL)
	{
		ch->ChatPacket (CHAT_TYPE_COMMAND, "RefineSuceeded");

		LogManager::instance().RefineLog (ch->GetPlayerID(), item->GetName(), item->GetID(), item->GetRefineLevel(), 1, way);
	}
}

void NotifyRefineFail (LPCHARACTER ch, LPITEM item, const char* way, int success = 0)
{
	if (NULL != ch && NULL != item)
	{
		ch->ChatPacket (CHAT_TYPE_COMMAND, "RefineFailed");

		LogManager::instance().RefineLog (ch->GetPlayerID(), item->GetName(), item->GetID(), item->GetRefineLevel(), success, way);
	}
}

void CHARACTER::SetRefineNPC (LPCHARACTER ch)
{
	if (ch != NULL)
	{
		m_dwRefineNPCVID = ch->GetVID();
	}
	else
	{
		m_dwRefineNPCVID = 0;
	}
}

bool CHARACTER::DoRefine (LPITEM item, bool bMoneyOnly)
{
	if (!CanHandleItem (true))
	{
		ClearRefineMode();
		return false;
	}

	//개량 시간제한 : upgrade_refine_scroll.quest 에서 개량후 5분이내에 일반 개량을
	//진행할수 없음
	if (quest::CQuestManager::instance().GetEventFlag ("update_refine_time") != 0)
	{
		if (get_global_time() < quest::CQuestManager::instance().GetEventFlag ("update_refine_time") + (60 * 5))
		{
			sys_log (0, "can't refine %d %s", GetPlayerID(), GetName());
			return false;
		}
	}

	const TRefineTable* prt = CRefineManager::instance().GetRefineRecipe (item->GetRefineSet());

	if (!prt)
	{
		return false;
	}

	DWORD result_vnum = item->GetRefinedVnum();

	// REFINE_COST
	int cost = ComputeRefineFee (prt->cost);

	int RefineChance = GetQuestFlag ("main_quest_lv7.refine_chance");

	if (RefineChance > 0)
	{
		if (!item->CheckItemUseLevel (20) || item->GetType() != ITEM_WEAPON)
		{
			ChatPacket (CHAT_TYPE_INFO, "[LS;1003]");
			return false;
		}

		cost = 0;
		SetQuestFlag ("main_quest_lv7.refine_chance", RefineChance - 1);
	}
	// END_OF_REFINE_COST

	if (result_vnum == 0)
	{
		ChatPacket (CHAT_TYPE_INFO, "[LS;991]");
		return false;
	}

	if (item->GetType() == ITEM_USE && item->GetSubType() == USE_TUNING)
	{
		return false;
	}

	TItemTable* pProto = ITEM_MANAGER::instance().GetTable (item->GetRefinedVnum());

	if (!pProto)
	{
		sys_err ("DoRefine NOT GET ITEM PROTO %d", item->GetRefinedVnum());
		ChatPacket (CHAT_TYPE_INFO, "[LS;1002]");
		return false;
	}

	// REFINE_COST
	if (GetGold() < cost)
	{
		ChatPacket (CHAT_TYPE_INFO, "[LS;1024]");
		return false;
	}

	if (!bMoneyOnly && !RefineChance)
	{
		for (int i = 0; i < prt->material_count; ++i)
		{
			if (CountSpecifyItem (prt->materials[i].vnum) < prt->materials[i].count)
			{
				if (test_server)
				{
					ChatPacket (CHAT_TYPE_INFO, "Find %d, count %d, require %d", prt->materials[i].vnum, CountSpecifyItem (prt->materials[i].vnum), prt->materials[i].count);
				}
				ChatPacket (CHAT_TYPE_INFO, "[LS;1035]");
				return false;
			}
		}

		for (int i = 0; i < prt->material_count; ++i)
		{
			RemoveSpecifyItem (prt->materials[i].vnum, prt->materials[i].count);
		}
	}

	int prob = number (1, 100);

	if (IsRefineThroughGuild() || bMoneyOnly)
	{
		prob -= 10;
	}

	// END_OF_REFINE_COST

	if (prob <= prt->prob)
	{
		// 성공! 모든 아이템이 사라지고, 같은 속성의 다른 아이템 획득
		LPITEM pkNewItem = ITEM_MANAGER::instance().CreateItem (result_vnum, 1, 0, false);

		if (pkNewItem)
		{
			ITEM_MANAGER::CopyAllAttrTo (item, pkNewItem);
			LogManager::instance().ItemLog (this, pkNewItem, "REFINE SUCCESS", pkNewItem->GetName());

			BYTE bCell = item->GetCell();

			// DETAIL_REFINE_LOG
			NotifyRefineSuccess (this, item, IsRefineThroughGuild() ? "GUILD" : "POWER");
			DBManager::instance().SendMoneyLog (MONEY_LOG_REFINE, item->GetVnum(), -cost);
			ITEM_MANAGER::instance().RemoveItem (item, "REMOVE (REFINE SUCCESS)");
			// END_OF_DETAIL_REFINE_LOG

			pkNewItem->AddToCharacter (this, TItemPos (INVENTORY, bCell));
			ITEM_MANAGER::instance().FlushDelayedSave (pkNewItem);

			sys_log (0, "Refine Success %d", cost);
			pkNewItem->AttrLog();
			//PointChange(POINT_GOLD, -cost);
			sys_log (0, "PayPee %d", cost);
			PayRefineFee (cost);
			sys_log (0, "PayPee End %d", cost);
		}
		else
		{
			// DETAIL_REFINE_LOG
			// 아이템 생성에 실패 -> 개량 실패로 간주
			sys_err ("cannot create item %u", result_vnum);
			NotifyRefineFail (this, item, IsRefineThroughGuild() ? "GUILD" : "POWER");
			// END_OF_DETAIL_REFINE_LOG
		}
	}
	else
	{
		// 실패! 모든 아이템이 사라짐.
		DBManager::instance().SendMoneyLog (MONEY_LOG_REFINE, item->GetVnum(), -cost);
		NotifyRefineFail (this, item, IsRefineThroughGuild() ? "GUILD" : "POWER");
		item->AttrLog();
		ITEM_MANAGER::instance().RemoveItem (item, "REMOVE (REFINE FAIL)");

		//PointChange(POINT_GOLD, -cost);
		PayRefineFee (cost);
	}

	return true;
}

enum enum_RefineScrolls
{
	CHUKBOK_SCROLL	= 0,
	HYUNIRON_CHN	= 1, // 중국에서만 사용
	YONGSIN_SCROLL	= 2,
	MUSIN_SCROLL	= 3,
	YAGONG_SCROLL	= 4,
	MEMO_SCROLL		= 5,
	BDRAGON_SCROLL	= 6,
};

bool CHARACTER::DoRefineWithScroll (LPITEM item)
{
	if (!CanHandleItem (true))
	{
		ClearRefineMode();
		return false;
	}

	ClearRefineMode();

	//개량 시간제한 : upgrade_refine_scroll.quest 에서 개량후 5분이내에 일반 개량을
	//진행할수 없음
	if (quest::CQuestManager::instance().GetEventFlag ("update_refine_time") != 0)
	{
		if (get_global_time() < quest::CQuestManager::instance().GetEventFlag ("update_refine_time") + (60 * 5))
		{
			sys_log (0, "can't refine %d %s", GetPlayerID(), GetName());
			return false;
		}
	}

	const TRefineTable* prt = CRefineManager::instance().GetRefineRecipe (item->GetRefineSet());

	if (!prt)
	{
		return false;
	}

	LPITEM pkItemScroll;

	// 개량서 체크
	if (m_iRefineAdditionalCell < 0)
	{
		return false;
	}

	pkItemScroll = GetInventoryItem (m_iRefineAdditionalCell);

	if (!pkItemScroll)
	{
		return false;
	}

	if (! (pkItemScroll->GetType() == ITEM_USE && pkItemScroll->GetSubType() == USE_TUNING))
	{
		return false;
	}

	if (pkItemScroll->GetVnum() == item->GetVnum())
	{
		return false;
	}

	DWORD result_vnum = item->GetRefinedVnum();
	DWORD result_fail_vnum = item->GetRefineFromVnum();

	if (result_vnum == 0)
	{
		ChatPacket (CHAT_TYPE_INFO, "[LS;991]");
		return false;
	}

	// MUSIN_SCROLL
	if (pkItemScroll->GetValue (0) == MUSIN_SCROLL)
	{
		if (item->GetRefineLevel() >= 4)
		{
			ChatPacket (CHAT_TYPE_INFO, "[LS;1045]");
			return false;
		}
	}
	// END_OF_MUSIC_SCROLL

	else if (pkItemScroll->GetValue (0) == MEMO_SCROLL)
	{
		if (item->GetRefineLevel() != pkItemScroll->GetValue (1))
		{
			ChatPacket (CHAT_TYPE_INFO, "[LS;1056]");
			return false;
		}
	}
	else if (pkItemScroll->GetValue (0) == BDRAGON_SCROLL)
	{
		if (item->GetType() != ITEM_METIN || item->GetRefineLevel() != 4)
		{
			ChatPacket (CHAT_TYPE_INFO, LC_TEXT ("이 아이템으로 개량할 수 없습니다."));
			return false;
		}
	}

	TItemTable* pProto = ITEM_MANAGER::instance().GetTable (item->GetRefinedVnum());

	if (!pProto)
	{
		sys_err ("DoRefineWithScroll NOT GET ITEM PROTO %d", item->GetRefinedVnum());
		ChatPacket (CHAT_TYPE_INFO, "[LS;1002]");
		return false;
	}

	if (GetGold() < prt->cost)
	{
		ChatPacket (CHAT_TYPE_INFO, "[LS;1024]");
		return false;
	}

	for (int i = 0; i < prt->material_count; ++i)
	{
		if (CountSpecifyItem (prt->materials[i].vnum) < prt->materials[i].count)
		{
			if (test_server)
			{
				ChatPacket (CHAT_TYPE_INFO, "Find %d, count %d, require %d", prt->materials[i].vnum, CountSpecifyItem (prt->materials[i].vnum), prt->materials[i].count);
			}
			ChatPacket (CHAT_TYPE_INFO, "[LS;1035]");
			return false;
		}
	}

	for (int i = 0; i < prt->material_count; ++i)
	{
		RemoveSpecifyItem (prt->materials[i].vnum, prt->materials[i].count);
	}

	int prob = number (1, 100);
	int success_prob = prt->prob;
	bool bDestroyWhenFail = false;

	const char* szRefineType = "SCROLL";

	if (pkItemScroll->GetValue (0) == HYUNIRON_CHN ||
			pkItemScroll->GetValue (0) == YONGSIN_SCROLL ||
			pkItemScroll->GetValue (0) == YAGONG_SCROLL) // 현철, 용신의 축복서, 야공의 비전서  처리
	{
		const char hyuniron_prob[9] = { 100, 75, 65, 55, 45, 40, 35, 25, 20 };
		const char yagong_prob[9] = { 100, 100, 90, 80, 70, 60, 50, 30, 20 };

		if (pkItemScroll->GetValue (0) == YONGSIN_SCROLL)
		{
			success_prob = hyuniron_prob[MINMAX (0, item->GetRefineLevel(), 8)];
		}
		else if (pkItemScroll->GetValue (0) == YAGONG_SCROLL)
		{
			success_prob = yagong_prob[MINMAX (0, item->GetRefineLevel(), 8)];
		}
		else
		{
			sys_err ("REFINE : Unknown refine scroll item. Value0: %d", pkItemScroll->GetValue (0));
		}

		if (test_server)
		{
			ChatPacket (CHAT_TYPE_INFO, "[Only Test] Success_Prob %d, RefineLevel %d ", success_prob, item->GetRefineLevel());
		}
		if (pkItemScroll->GetValue (0) == HYUNIRON_CHN) // 현철은 아이템이 부서져야 한다.
		{
			bDestroyWhenFail = true;
		}

		// DETAIL_REFINE_LOG
		if (pkItemScroll->GetValue (0) == HYUNIRON_CHN)
		{
			szRefineType = "HYUNIRON";
		}
		else if (pkItemScroll->GetValue (0) == YONGSIN_SCROLL)
		{
			szRefineType = "GOD_SCROLL";
		}
		else if (pkItemScroll->GetValue (0) == YAGONG_SCROLL)
		{
			szRefineType = "YAGONG_SCROLL";
		}
		// END_OF_DETAIL_REFINE_LOG
	}

	// DETAIL_REFINE_LOG
	if (pkItemScroll->GetValue (0) == MUSIN_SCROLL) // 무신의 축복서는 100% 성공 (+4까지만)
	{
		success_prob = 100;

		szRefineType = "MUSIN_SCROLL";
	}
	// END_OF_DETAIL_REFINE_LOG
	else if (pkItemScroll->GetValue (0) == MEMO_SCROLL)
	{
		success_prob = 100;
		szRefineType = "MEMO_SCROLL";
	}
	else if (pkItemScroll->GetValue (0) == BDRAGON_SCROLL)
	{
		success_prob = 80;
		szRefineType = "BDRAGON_SCROLL";
	}

	pkItemScroll->SetCount (pkItemScroll->GetCount() - 1);

	if (prob <= success_prob)
	{
		// 성공! 모든 아이템이 사라지고, 같은 속성의 다른 아이템 획득
		LPITEM pkNewItem = ITEM_MANAGER::instance().CreateItem (result_vnum, 1, 0, false);

		if (pkNewItem)
		{
			ITEM_MANAGER::CopyAllAttrTo (item, pkNewItem);
			LogManager::instance().ItemLog (this, pkNewItem, "REFINE SUCCESS", pkNewItem->GetName());

			BYTE bCell = item->GetCell();

			NotifyRefineSuccess (this, item, szRefineType);
			DBManager::instance().SendMoneyLog (MONEY_LOG_REFINE, item->GetVnum(), -prt->cost);
			ITEM_MANAGER::instance().RemoveItem (item, "REMOVE (REFINE SUCCESS)");

			pkNewItem->AddToCharacter (this, TItemPos (INVENTORY, bCell));
			ITEM_MANAGER::instance().FlushDelayedSave (pkNewItem);
			pkNewItem->AttrLog();
			//PointChange(POINT_GOLD, -prt->cost);
			PayRefineFee (prt->cost);
		}
		else
		{
			// 아이템 생성에 실패 -> 개량 실패로 간주
			sys_err ("cannot create item %u", result_vnum);
			NotifyRefineFail (this, item, szRefineType);
		}
	}
	else if (!bDestroyWhenFail && result_fail_vnum)
	{
		// 실패! 모든 아이템이 사라지고, 같은 속성의 낮은 등급의 아이템 획득
		LPITEM pkNewItem = ITEM_MANAGER::instance().CreateItem (result_fail_vnum, 1, 0, false);

		if (pkNewItem)
		{
			ITEM_MANAGER::CopyAllAttrTo (item, pkNewItem);
			LogManager::instance().ItemLog (this, pkNewItem, "REFINE FAIL", pkNewItem->GetName());

			BYTE bCell = item->GetCell();

			DBManager::instance().SendMoneyLog (MONEY_LOG_REFINE, item->GetVnum(), -prt->cost);
			NotifyRefineFail (this, item, szRefineType, -1);
			ITEM_MANAGER::instance().RemoveItem (item, "REMOVE (REFINE FAIL)");

			pkNewItem->AddToCharacter (this, TItemPos (INVENTORY, bCell));
			ITEM_MANAGER::instance().FlushDelayedSave (pkNewItem);

			pkNewItem->AttrLog();

			//PointChange(POINT_GOLD, -prt->cost);
			PayRefineFee (prt->cost);
		}
		else
		{
			// 아이템 생성에 실패 -> 개량 실패로 간주
			sys_err ("cannot create item %u", result_fail_vnum);
			NotifyRefineFail (this, item, szRefineType);
		}
	}
	else
	{
		NotifyRefineFail (this, item, szRefineType); // 개량시 아이템 사라지지 않음

		PayRefineFee (prt->cost);
	}

	return true;
}

bool CHARACTER::RefineInformation (BYTE bCell, BYTE bType, int iAdditionalCell)
{
	if (bCell > INVENTORY_MAX_NUM)
	{
		return false;
	}

	LPITEM item = GetInventoryItem (bCell);

	if (!item)
	{
		return false;
	}

	// REFINE_COST
	if (bType == REFINE_TYPE_MONEY_ONLY && !GetQuestFlag ("deviltower_zone.can_refine"))
	{
		ChatPacket (CHAT_TYPE_INFO, "[LS;1067]");
		return false;
	}
	// END_OF_REFINE_COST

	TPacketGCRefineInformation p;

	p.header = HEADER_GC_REFINE_INFORMATION;
	p.pos = bCell;
	p.src_vnum = item->GetVnum();
	p.result_vnum = item->GetRefinedVnum();
#if defined(__ITEM_APPLY_RANDOM__)
	thecore_memcpy(&p.aApplyRandom, item->GetNextRandomApplies(), sizeof(p.aApplyRandom));
#endif
	p.type = bType;

	if (p.result_vnum == 0)
	{
		sys_err ("RefineInformation p.result_vnum == 0");
		ChatPacket (CHAT_TYPE_INFO, "[LS;1002]");
		return false;
	}

	if (item->GetType() == ITEM_USE && item->GetSubType() == USE_TUNING)
	{
		if (bType == 0)
		{
			ChatPacket (CHAT_TYPE_INFO, "[LS;1077]");
			return false;
		}
		else
		{
			LPITEM itemScroll = GetInventoryItem (iAdditionalCell);
			if (!itemScroll || item->GetVnum() == itemScroll->GetVnum())
			{
				ChatPacket (CHAT_TYPE_INFO, "[LS;1086]");
				ChatPacket (CHAT_TYPE_INFO, "[LS;1096]");
				return false;
			}
		}
	}

	CRefineManager& rm = CRefineManager::instance();

	const TRefineTable* prt = rm.GetRefineRecipe (item->GetRefineSet());

	if (!prt)
	{
		sys_err ("RefineInformation NOT GET REFINE SET %d", item->GetRefineSet());
		ChatPacket (CHAT_TYPE_INFO, "[LS;1002]");
		return false;
	}

	// REFINE_COST

	//MAIN_QUEST_LV7
	if (GetQuestFlag ("main_quest_lv7.refine_chance") > 0)
	{
		// 일본은 제외
		if (!item->CheckItemUseLevel (20) || item->GetType() != ITEM_WEAPON)
		{
			ChatPacket (CHAT_TYPE_INFO, "[LS;1003]");
			return false;
		}
		p.cost = 0;
	}
	else
	{
		p.cost = ComputeRefineFee (prt->cost);
	}

	//END_MAIN_QUEST_LV7
	p.prob = prt->prob;
	if (bType == REFINE_TYPE_MONEY_ONLY)
	{
		p.material_count = 0;
		memset (p.materials, 0, sizeof (p.materials));
	}
	else
	{
		p.material_count = prt->material_count;
		thecore_memcpy (&p.materials, prt->materials, sizeof (prt->materials));
	}
	// END_OF_REFINE_COST

	GetDesc()->Packet (&p, sizeof (TPacketGCRefineInformation));

	SetRefineMode (iAdditionalCell);
	return true;
}

bool CHARACTER::RefineItem (LPITEM pkItem, LPITEM pkTarget)
{
	if (!CanHandleItem())
	{
		return false;
	}

	if (pkItem->GetSubType() == USE_TUNING)
	{
		// XXX 성능, 소켓 개량서는 사라졌습니다...
		// XXX 성능개량서는 축복의 서가 되었다!
		// MUSIN_SCROLL
		if (pkItem->GetValue (0) == MUSIN_SCROLL)
		{
			RefineInformation (pkTarget->GetCell(), REFINE_TYPE_MUSIN, pkItem->GetCell());
		}
		// END_OF_MUSIN_SCROLL
		else if (pkItem->GetValue (0) == HYUNIRON_CHN)
		{
			RefineInformation (pkTarget->GetCell(), REFINE_TYPE_HYUNIRON, pkItem->GetCell());
		}
		else if (pkItem->GetValue (0) == BDRAGON_SCROLL)
		{
			if (pkTarget->GetRefineSet() != 702)
			{
				return false;
			}
			RefineInformation (pkTarget->GetCell(), REFINE_TYPE_BDRAGON, pkItem->GetCell());
		}
		else
		{
			if (pkTarget->GetRefineSet() == 501)
			{
				return false;
			}
			RefineInformation (pkTarget->GetCell(), REFINE_TYPE_SCROLL, pkItem->GetCell());
		}
	}
	else if (pkItem->GetSubType() == USE_DETACHMENT && IS_SET (pkTarget->GetFlag(), ITEM_FLAG_REFINEABLE))
	{
		LogManager::instance().ItemLog (this, pkTarget, "USE_DETACHMENT", pkTarget->GetName());

		bool bHasMetinStone = false;

		for (int i = 0; i < ITEM_SOCKET_MAX_NUM; i++)
		{
			long socket = pkTarget->GetSocket (i);
			if (socket > 2 && socket != ITEM_BROKEN_METIN_VNUM)
			{
				bHasMetinStone = true;
				break;
			}
		}

		if (bHasMetinStone)
		{
			for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
			{
				long socket = pkTarget->GetSocket (i);
				if (socket > 2 && socket != ITEM_BROKEN_METIN_VNUM)
				{
					AutoGiveItem (socket);
					//TItemTable* pTable = ITEM_MANAGER::instance().GetTable(pkTarget->GetSocket(i));
					//pkTarget->SetSocket(i, pTable->alValues[2]);
					// 깨진돌로 대체해준다
					pkTarget->SetSocket (i, ITEM_BROKEN_METIN_VNUM);
				}
			}
			pkItem->SetCount (pkItem->GetCount() - 1);
			return true;
		}
		else
		{
			ChatPacket (CHAT_TYPE_INFO, "[LS;1108]");
			return false;
		}
	}

	return false;
}

EVENTFUNC (kill_campfire_event)
{
	char_event_info* info = dynamic_cast<char_event_info*> (event->info);

	if (info == NULL)
	{
		sys_err ("kill_campfire_event> <Factor> Null pointer");
		return 0;
	}

	LPCHARACTER	ch = info->ch;

	if (ch == NULL)   // <Factor>
	{
		return 0;
	}
	ch->m_pkMiningEvent = NULL;
	M2_DESTROY_CHARACTER (ch);
	return 0;
}

bool CHARACTER::GiveRecallItem (LPITEM item)
{
	int idx = GetMapIndex();
	int iEmpireByMapIndex = -1;

	if (idx < 20)
	{
		iEmpireByMapIndex = 1;
	}
	else if (idx < 40)
	{
		iEmpireByMapIndex = 2;
	}
	else if (idx < 60)
	{
		iEmpireByMapIndex = 3;
	}
	else if (idx < 10000)
	{
		iEmpireByMapIndex = 0;
	}

	switch (idx)
	{
		case 66:
		case 216:
			iEmpireByMapIndex = -1;
			break;
	}

	if (iEmpireByMapIndex && GetEmpire() != iEmpireByMapIndex)
	{
		ChatPacket (CHAT_TYPE_INFO, "[LS;1119]");
		return false;
	}

	int pos;

	if (item->GetCount() == 1)	// 아이템이 하나라면 그냥 셋팅.
	{
		item->SetSocket (0, GetX());
		item->SetSocket (1, GetY());
	}
	else if ((pos = GetEmptyInventory (item->GetSize())) != -1) // 그렇지 않다면 다른 인벤토리 슬롯을 찾는다.
	{
		LPITEM item2 = ITEM_MANAGER::instance().CreateItem (item->GetVnum(), 1);

		if (NULL != item2)
		{
			item2->SetSocket (0, GetX());
			item2->SetSocket (1, GetY());
			item2->AddToCharacter (this, TItemPos (INVENTORY, pos));

			item->SetCount (item->GetCount() - 1);
		}
	}
	else
	{
		ChatPacket (CHAT_TYPE_INFO, "[LS;1130]");
		return false;
	}

	return true;
}

void CHARACTER::ProcessRecallItem (LPITEM item)
{
	int idx;

	if ((idx = SECTREE_MANAGER::instance().GetMapIndex (item->GetSocket (0), item->GetSocket (1))) == 0)
	{
		return;
	}

	int iEmpireByMapIndex = -1;

	if (idx < 20)
	{
		iEmpireByMapIndex = 1;
	}
	else if (idx < 40)
	{
		iEmpireByMapIndex = 2;
	}
	else if (idx < 60)
	{
		iEmpireByMapIndex = 3;
	}
	else if (idx < 10000)
	{
		iEmpireByMapIndex = 0;
	}

	switch (idx)
	{
		case 66:
		case 216:
			iEmpireByMapIndex = -1;
			break;
		// 악룡군도 일때
		case 301:
		case 302:
		case 303:
		case 304:
			if (GetLevel() < 90)
			{
				ChatPacket (CHAT_TYPE_INFO, "[LS;1183]");
				return;
			}
			else
			{
				break;
			}
	}

	if (iEmpireByMapIndex && GetEmpire() != iEmpireByMapIndex)
	{
		ChatPacket (CHAT_TYPE_INFO, "[LS;1141]");
		item->SetSocket (0, 0);
		item->SetSocket (1, 0);
	}
	else
	{
		sys_log (1, "Recall: %s %d %d -> %d %d", GetName(), GetX(), GetY(), item->GetSocket (0), item->GetSocket (1));
		WarpSet (item->GetSocket (0), item->GetSocket (1));
		item->SetCount (item->GetCount() - 1);
	}
}

void CHARACTER::__OpenPrivateShop()
{
	unsigned bodyPart = GetPart (PART_MAIN);
	switch (bodyPart)
	{
		case 0:
		case 1:
		case 2:
			ChatPacket (CHAT_TYPE_COMMAND, "OpenPrivateShop");
			break;
		default:
			ChatPacket (CHAT_TYPE_INFO, "[LS;1025]");
			break;
	}
}

// MYSHOP_PRICE_LIST
void CHARACTER::SendMyShopPriceListCmd (DWORD dwItemVnum, DWORD dwItemPrice)
{
	char szLine[256];
	snprintf (szLine, sizeof (szLine), "MyShopPriceList %u %u", dwItemVnum, dwItemPrice);
	ChatPacket (CHAT_TYPE_COMMAND, szLine);
	sys_log (0, szLine);
}

//
// DB 캐시로 부터 받은 리스트를 User 에게 전송하고 상점을 열라는 커맨드를 보낸다.
//
void CHARACTER::UseSilkBotaryReal (const TPacketMyshopPricelistHeader* p)
{
	const TItemPriceInfo* pInfo = (const TItemPriceInfo*) (p + 1);

	if (!p->byCount)
		// 가격 리스트가 없다. dummy 데이터를 넣은 커맨드를 보내준다.
	{
		SendMyShopPriceListCmd (1, 0);
	}
	else
	{
		for (int idx = 0; idx < p->byCount; idx++)
		{
			SendMyShopPriceListCmd (pInfo[ idx ].dwVnum, pInfo[ idx ].dwPrice);
		}
	}

	__OpenPrivateShop();
}

//
// 이번 접속 후 처음 상점을 Open 하는 경우 리스트를 Load 하기 위해 DB 캐시에 가격정보 리스트 요청 패킷을 보낸다.
// 이후부터는 바로 상점을 열라는 응답을 보낸다.
//
void CHARACTER::UseSilkBotary (void)
{
	if (m_bNoOpenedShop)
	{
		DWORD dwPlayerID = GetPlayerID();
		db_clientdesc->DBPacket (HEADER_GD_MYSHOP_PRICELIST_REQ, GetDesc()->GetHandle(), &dwPlayerID, sizeof (DWORD));
		m_bNoOpenedShop = false;
	}
	else
	{
		__OpenPrivateShop();
	}
}
// END_OF_MYSHOP_PRICE_LIST


int CalculateConsume (LPCHARACTER ch)
{
	static const int WARP_NEED_LIFE_PERCENT	= 30;
	static const int WARP_MIN_LIFE_PERCENT	= 10;
	// CONSUME_LIFE_WHEN_USE_WARP_ITEM
	int consumeLife = 0;
	{
		// CheckNeedLifeForWarp
		const int curLife		= ch->GetHP();
		const int needPercent	= WARP_NEED_LIFE_PERCENT;
		const int needLife = ch->GetMaxHP() * needPercent / 100;
		if (curLife < needLife)
		{
			ch->ChatPacket (CHAT_TYPE_INFO, "[LS;1152]");
			return -1;
		}

		consumeLife = needLife;


		// CheckMinLifeForWarp: 독에 의해서 죽으면 안되므로 생명력 최소량는 남겨준다
		const int minPercent	= WARP_MIN_LIFE_PERCENT;
		const int minLife	= ch->GetMaxHP() * minPercent / 100;
		if (curLife - needLife < minLife)
		{
			consumeLife = curLife - minLife;
		}

		if (consumeLife < 0)
		{
			consumeLife = 0;
		}
	}
	// END_OF_CONSUME_LIFE_WHEN_USE_WARP_ITEM
	return consumeLife;
}

int CalculateConsumeSP (LPCHARACTER lpChar)
{
	static const int NEED_WARP_SP_PERCENT = 30;

	const int curSP = lpChar->GetSP();
	const int needSP = lpChar->GetMaxSP() * NEED_WARP_SP_PERCENT / 100;

	if (curSP < needSP)
	{
		lpChar->ChatPacket (CHAT_TYPE_INFO, "[LS;1162]");
		return -1;
	}

	return needSP;
}

bool CHARACTER::UseItemEx (LPITEM item, TItemPos DestCell)
{
	int iLimitRealtimeStartFirstUseFlagIndex = -1;
	int iLimitTimerBasedOnWearFlagIndex = -1;

	WORD wDestCell = DestCell.cell;
	BYTE bDestInven = DestCell.window_type;
	for (int i = 0; i < ITEM_LIMIT_MAX_NUM; ++i)
	{
		long limitValue = item->GetProto()->aLimits[i].lValue;

		switch (item->GetProto()->aLimits[i].bType)
		{
			case LIMIT_LEVEL:
				if (GetLevel() < limitValue)
				{
					ChatPacket (CHAT_TYPE_INFO, "[LS;1183]");
					return false;
				}
				break;

			case LIMIT_REAL_TIME_START_FIRST_USE:
				iLimitRealtimeStartFirstUseFlagIndex = i;
				break;

			case LIMIT_TIMER_BASED_ON_WEAR:
				iLimitTimerBasedOnWearFlagIndex = i;
				break;
		}
	}

	if (test_server)
	{
		sys_log (0, "USE_ITEM %s, Inven %d, Cell %d, ItemType %d, SubType %d", item->GetName(), bDestInven, wDestCell, item->GetType(), item->GetSubType());
	}

	if (CArenaManager::instance().IsLimitedItem (GetMapIndex(), item->GetVnum()) == true)
	{
		ChatPacket (CHAT_TYPE_INFO, "[LS;1205]");
		return false;
	}

	// 아이템 최초 사용 이후부터는 사용하지 않아도 시간이 차감되는 방식 처리.
	if (-1 != iLimitRealtimeStartFirstUseFlagIndex)
	{
		// 한 번이라도 사용한 아이템인지 여부는 Socket1을 보고 판단한다. (Socket1에 사용횟수 기록)
		if (0 == item->GetSocket (1))
		{
			// 사용가능시간은 Default 값으로 Limit Value 값을 사용하되, Socket0에 값이 있으면 그 값을 사용하도록 한다. (단위는 초)
			long duration = (0 != item->GetSocket (0)) ? item->GetSocket (0) : item->GetProto()->aLimits[iLimitRealtimeStartFirstUseFlagIndex].lValue;

			if (0 == duration)
			{
				duration = 60 * 60 * 24 * 7;
			}

			item->SetSocket (0, time (0) + duration);
			item->StartRealTimeExpireEvent();
		}

		if (false == item->IsEquipped())
		{
			item->SetSocket (1, item->GetSocket (1) + 1);
		}
	}

	switch (item->GetType())
	{
		case ITEM_HAIR:
			return ItemProcess_Hair (item, wDestCell);

		case ITEM_POLYMORPH:
			return ItemProcess_Polymorph (item);

		case ITEM_QUEST:
			if (GetArena() != NULL || IsObserverMode() == true)
			{
				if (item->GetVnum() == 50051 || item->GetVnum() == 50052 || item->GetVnum() == 50053)
				{
					ChatPacket (CHAT_TYPE_INFO, "[LS;1205]");
					return false;
				}
			}

#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
			if (GetWear(WEAR_COSTUME_MOUNT))
			{
				if (item->GetVnum() == 50051 || item->GetVnum() == 50052 || item->GetVnum() == 50053)
				{
					ChatPacket (CHAT_TYPE_INFO, "[LS;2028]");
					return false;
				}
			}
#endif

			if (!IS_SET (item->GetFlag(), ITEM_FLAG_QUEST_USE | ITEM_FLAG_QUEST_USE_MULTIPLE))
			{
				if (item->GetSIGVnum() == 0)
				{
					quest::CQuestManager::instance().UseItem (GetPlayerID(), item, false);
				}
				else
				{
					quest::CQuestManager::instance().SIGUse (GetPlayerID(), item->GetSIGVnum(), item, false);
				}
			}
			break;

		case ITEM_CAMPFIRE:
		{
			float fx, fy;
			GetDeltaByDegree (GetRotation(), 100.0f, &fx, &fy);

			LPSECTREE tree = SECTREE_MANAGER::instance().Get (GetMapIndex(), (long) (GetX()+fx), (long) (GetY()+fy));

			if (!tree)
			{
				ChatPacket (CHAT_TYPE_INFO, "[LS;1217]");
				return false;
			}

			if (tree->IsAttr ((long) (GetX()+fx), (long) (GetY()+fy), ATTR_WATER))
			{
				ChatPacket (CHAT_TYPE_INFO, "[LS;1228]");
				return false;
			}

			LPCHARACTER campfire = CHARACTER_MANAGER::instance().SpawnMob (fishing::CAMPFIRE_MOB, GetMapIndex(), (long) (GetX()+fx), (long) (GetY()+fy), 0, false, number (0, 359));

			char_event_info* info = AllocEventInfo<char_event_info>();

			info->ch = campfire;

			campfire->m_pkMiningEvent = event_create (kill_campfire_event, info, PASSES_PER_SEC (40));

			item->SetCount (item->GetCount() - 1);
		}
		break;

		case ITEM_UNIQUE:
		{
			switch (item->GetSubType())
			{
				case USE_ABILITY_UP:
				{
					switch (item->GetValue (0))
					{
						case APPLY_MOV_SPEED:
							AddAffect (AFFECT_UNIQUE_ABILITY, POINT_MOV_SPEED, item->GetValue (2), AFF_MOV_SPEED_POTION, item->GetValue (1), 0, true, true);
							break;

						case APPLY_ATT_SPEED:
							AddAffect (AFFECT_UNIQUE_ABILITY, POINT_ATT_SPEED, item->GetValue (2), AFF_ATT_SPEED_POTION, item->GetValue (1), 0, true, true);
							break;

						case APPLY_STR:
							AddAffect (AFFECT_UNIQUE_ABILITY, POINT_ST, item->GetValue (2), 0, item->GetValue (1), 0, true, true);
							break;

						case APPLY_DEX:
							AddAffect (AFFECT_UNIQUE_ABILITY, POINT_DX, item->GetValue (2), 0, item->GetValue (1), 0, true, true);
							break;

						case APPLY_CON:
							AddAffect (AFFECT_UNIQUE_ABILITY, POINT_HT, item->GetValue (2), 0, item->GetValue (1), 0, true, true);
							break;

						case APPLY_INT:
							AddAffect (AFFECT_UNIQUE_ABILITY, POINT_IQ, item->GetValue (2), 0, item->GetValue (1), 0, true, true);
							break;

						case APPLY_CAST_SPEED:
							AddAffect (AFFECT_UNIQUE_ABILITY, POINT_CASTING_SPEED, item->GetValue (2), 0, item->GetValue (1), 0, true, true);
							break;

						case APPLY_RESIST_MAGIC:
							AddAffect (AFFECT_UNIQUE_ABILITY, POINT_RESIST_MAGIC, item->GetValue (2), 0, item->GetValue (1), 0, true, true);
							break;

						case APPLY_ATT_GRADE_BONUS:
							AddAffect (AFFECT_UNIQUE_ABILITY, POINT_ATT_GRADE_BONUS,
									   item->GetValue (2), 0, item->GetValue (1), 0, true, true);
							break;

						case APPLY_DEF_GRADE_BONUS:
							AddAffect (AFFECT_UNIQUE_ABILITY, POINT_DEF_GRADE_BONUS,
									   item->GetValue (2), 0, item->GetValue (1), 0, true, true);
							break;
					}
				}

				if (GetDungeon())
				{
					GetDungeon()->UsePotion (this);
				}

				if (GetWarMap())
				{
					GetWarMap()->UsePotion (this, item);
				}

				item->SetCount (item->GetCount() - 1);
				break;

				default:
				{
					if (item->GetSubType() == USE_SPECIAL)
					{
						sys_log (0, "ITEM_UNIQUE: USE_SPECIAL %u", item->GetVnum());

						switch (item->GetVnum())
						{
							case 71049: // 비단보따리
								if (g_bEnableBootaryCheck)
								{
									if (IS_BOTARYABLE_ZONE (GetMapIndex()) == true)
									{
										UseSilkBotary();
									}
									else
									{
										ChatPacket (CHAT_TYPE_INFO, "[LS;216]");
									}
								}
								else
								{
									UseSilkBotary();
								}
								break;
						}
					}
					else
					{
						if (!item->IsEquipped())
						{
							EquipItem (item);
						}
						else
						{
							UnequipItem (item);
						}
					}
				}
				break;
			}
		}
		break;

		case ITEM_COSTUME:
		case ITEM_WEAPON:
		case ITEM_ARMOR:
		case ITEM_ROD:
		case ITEM_RING:		// 신규 반지 아이템
		case ITEM_BELT:		// 신규 벨트 아이템
		// MINING
		case ITEM_PICK:
			// END_OF_MINING
			if (!item->IsEquipped())
			{
				EquipItem (item);
			}
			else
			{
				UnequipItem (item);
			}
			break;
		// 착용하지 않은 용혼석은 사용할 수 없다.
		// 정상적인 클라라면, 용혼석에 관하여 item use 패킷을 보낼 수 없다.
		// 용혼석 착용은 item move 패킷으로 한다.
		// 착용한 용혼석은 추출한다.
		case ITEM_DS:
		{
			if (!item->IsEquipped())
			{
				return false;
			}
			return DSManager::instance().PullOut (this, NPOS, item);
			break;
		}
		case ITEM_SPECIAL_DS:
			if (!item->IsEquipped())
			{
				EquipItem (item);
			}
			else
			{
				UnequipItem (item);
			}
			break;

		case ITEM_FISH:
		{
			if (CArenaManager::instance().IsArenaMap (GetMapIndex()) == true)
			{
				ChatPacket (CHAT_TYPE_INFO, "[LS;1205]");
				return false;
			}

			if (item->GetSubType() == FISH_ALIVE)
			{
				fishing::UseFish (this, item);
			}
		}
		break;

		case ITEM_TREASURE_BOX:
		{
			return false;
			ChatPacket (CHAT_TYPE_TALKING, "[LS;1237]");
		}
		break;

		case ITEM_TREASURE_KEY:
		{
			LPITEM item2;

			if (!GetItem (DestCell) || ! (item2 = GetItem (DestCell)))
			{
				return false;
			}

			if (item2->IsExchanging())
			{
				return false;
			}

			if (item2->GetType() != ITEM_TREASURE_BOX)
			{
				ChatPacket (CHAT_TYPE_TALKING, "[LS;1248]");
				return false;
			}

			if (item->GetValue (0) == item2->GetValue (0))
			{
				ChatPacket (CHAT_TYPE_TALKING, "[LS;1258]");
				DWORD dwBoxVnum = item2->GetVnum();
				std::vector <DWORD> dwVnums;
				std::vector <DWORD> dwCounts;
				std::vector <LPITEM> item_gets;
				int count = 0;

				if (GiveItemFromSpecialItemGroup (dwBoxVnum, dwVnums, dwCounts, item_gets, count))
				{
					ITEM_MANAGER::instance().RemoveItem (item);
					ITEM_MANAGER::instance().RemoveItem (item2);

					for (int i = 0; i < count; i++)
					{
						switch (dwVnums[i])
						{
							case CSpecialItemGroup::GOLD:
								ChatPacket (CHAT_TYPE_INFO, "[LS;1269;%d]", dwCounts[i]);
								break;
							case CSpecialItemGroup::EXP:
								ChatPacket (CHAT_TYPE_INFO, "[LS;1279]");
								ChatPacket (CHAT_TYPE_INFO, "[LS;1290;%d]", dwCounts[i]);
								break;
							case CSpecialItemGroup::MOB:
								ChatPacket (CHAT_TYPE_INFO, "[LS;1299]");
								break;
							case CSpecialItemGroup::SLOW:
								ChatPacket (CHAT_TYPE_INFO, "[LS;1310]");
								break;
							case CSpecialItemGroup::DRAIN_HP:
								ChatPacket (CHAT_TYPE_INFO, "[LS;3]");
								break;
							case CSpecialItemGroup::POISON:
								ChatPacket (CHAT_TYPE_INFO, "[LS;13]");
								break;
							case CSpecialItemGroup::MOB_GROUP:
								ChatPacket (CHAT_TYPE_INFO, "[LS;1299]");
								break;
							default:
								if (item_gets[i])
								{
									if (dwCounts[i] > 1)
									{
										ChatPacket (CHAT_TYPE_INFO, "[LS;24;%s;%d]", item_gets[i]->GetName(), dwCounts[i]);
									}
									else
									{
										ChatPacket (CHAT_TYPE_INFO, "[LS;35;%s]", item_gets[i]->GetName());
									}

								}
						}
					}
				}
				else
				{
					ChatPacket (CHAT_TYPE_TALKING, "[LS;46]");
					return false;
				}
			}
			else
			{
				ChatPacket (CHAT_TYPE_TALKING, "[LS;46]");
				return false;
			}
		}
		break;

		case ITEM_GIFTBOX:
		{
#ifdef ENABLE_SHOW_CHEST_DROP
				if (GetEmptyInventory(3) == -1)
				{
					ChatPacket (CHAT_TYPE_INFO, "[LS;2034]");
					return false;
				}
#ifdef ENABLE_DRAGONSOUL_INVENTORY_BOX_SIZE
				DWORD dwBoxVnum = item->GetVnum();
				if ((dwBoxVnum > 51500 && dwBoxVnum < 52000) || (dwBoxVnum >= 50255 && dwBoxVnum <= 50260))
					if (GetEmptyDragonSoulInventoryType(0, 0) == -1 || GetEmptyDragonSoulInventoryType(1, 0) == -1 || GetEmptyDragonSoulInventoryType(2, 0) == -1
						|| GetEmptyDragonSoulInventoryType(3, 0) == -1 || GetEmptyDragonSoulInventoryType(4, 0) == -1 || GetEmptyDragonSoulInventoryType(5, 0) == -1
						|| GetEmptyDragonSoulInventoryType(6, 5) == -1
						)
					{

						//ChatPacket(CHAT_TYPE_INFO, "1Simya envanterinde i?lenmemi? simyayalardan biri dolu.!");
						ChatPacket (CHAT_TYPE_INFO, "[LS;2063]");
						return false;
					}
					if (GetEmptyDragonSoulInventoryType(0, 1) == -1 || GetEmptyDragonSoulInventoryType(1, 1) == -1 || GetEmptyDragonSoulInventoryType(2, 1) == -1
						|| GetEmptyDragonSoulInventoryType(3, 1) == -1 || GetEmptyDragonSoulInventoryType(4, 1) == -1 || GetEmptyDragonSoulInventoryType(5, 1) == -1
						|| GetEmptyDragonSoulInventoryType(6, 5) == -1
						)
					{

						//ChatPacket(CHAT_TYPE_INFO, "2Simya envanterinde yontulmu? simyayalardan biri dolu.!");
						ChatPacket (CHAT_TYPE_INFO, "[LS;2064]");
						return false;
					}
					if (GetEmptyDragonSoulInventoryType(0, 2) == -1 || GetEmptyDragonSoulInventoryType(1, 2) == -1 || GetEmptyDragonSoulInventoryType(2, 2) == -1
						|| GetEmptyDragonSoulInventoryType(3, 2) == -1 || GetEmptyDragonSoulInventoryType(4, 2) == -1 || GetEmptyDragonSoulInventoryType(5, 2) == -1
						|| GetEmptyDragonSoulInventoryType(6, 5) == -1
						)
					{

						//ChatPacket(CHAT_TYPE_INFO, "3Simya envanterinde ender simyayalardan biri dolu.!");
						ChatPacket (CHAT_TYPE_INFO, "[LS;2065]");
						return false;
					}
					if (GetEmptyDragonSoulInventoryType(0, 3) == -1 || GetEmptyDragonSoulInventoryType(1, 3) == -1 || GetEmptyDragonSoulInventoryType(2, 3) == -1
						|| GetEmptyDragonSoulInventoryType(3, 3) == -1 || GetEmptyDragonSoulInventoryType(4, 3) == -1 || GetEmptyDragonSoulInventoryType(5, 3) == -1
						|| GetEmptyDragonSoulInventoryType(6, 5) == -1
						)
					{

						//ChatPacket(CHAT_TYPE_INFO, "4Simya envanterinde antika simyayalardan biri dolu.!");
						ChatPacket (CHAT_TYPE_INFO, "[LS;2066]");
						return false;
					}
					if (GetEmptyDragonSoulInventoryType(0, 4) == -1 || GetEmptyDragonSoulInventoryType(1, 4) == -1 || GetEmptyDragonSoulInventoryType(2, 4) == -1
						|| GetEmptyDragonSoulInventoryType(3, 4) == -1 || GetEmptyDragonSoulInventoryType(4, 4) == -1 || GetEmptyDragonSoulInventoryType(5, 4) == -1
						|| GetEmptyDragonSoulInventoryType(6, 5) == -1
						)
					{

						//ChatPacket(CHAT_TYPE_INFO, "5Simya envanterinde efsanevi simyayalardan biri dolu.!");
						ChatPacket (CHAT_TYPE_INFO, "[LS;2067]");
						return false;
					}
					if (GetEmptyDragonSoulInventoryType(0, 5) == -1 || GetEmptyDragonSoulInventoryType(1, 5) == -1 || GetEmptyDragonSoulInventoryType(2, 5) == -1
						|| GetEmptyDragonSoulInventoryType(3, 5) == -1 || GetEmptyDragonSoulInventoryType(4, 5) == -1 || GetEmptyDragonSoulInventoryType(5, 5) == -1
						|| GetEmptyDragonSoulInventoryType(6, 5) == -1
						)
					{

						//ChatPacket(CHAT_TYPE_INFO, "66Simya envanterinde mitsi simyayalardan biri dolu.!");
						ChatPacket (CHAT_TYPE_INFO, "[LS;2068]");
						return false;
					}
#endif
#endif
		//	DWORD dwBoxVnum = item->GetVnum();
			std::vector <DWORD> dwVnums;
			std::vector <DWORD> dwCounts;
			std::vector <LPITEM> item_gets;
			int count = 0;

#if defined(__BL_67_ATTR__)
				switch (dwBoxVnum)
				{
				case POWERSHARD_CHEST:
					// The Powershard Chest can be purchased from Seon-Hae in exchange for 10 Skill Books.(From Wiki)
					// It can contain Powershards of any kind or a Skill Book.(From Wiki)
					// You can edit here for skill books(From black)
					if (number(1, 100) <= 30)
						AutoGiveItem(CItemVnumHelper::Get67MaterialVnum(number(0, gPlayerMaxLevel)));
					else
						ChatPacket (CHAT_TYPE_TALKING, "[LS;2048]");
					item->SetCount(item->GetCount() - 1);
					return true;
				case ELEGANT_POWERSHARD_CHEST:
					if (number(1, 100) <= 60)
						AutoGiveItem(CItemVnumHelper::Get67MaterialVnum(number(0, gPlayerMaxLevel)));
					else
						ChatPacket (CHAT_TYPE_TALKING, "[LS;2048]");
					item->SetCount(item->GetCount() - 1);
					return true;
				case LUCENT_POWERSHARD_CHEST:
					for (BYTE _i = 0; _i < 5; _i++)
						AutoGiveItem(CItemVnumHelper::Get67MaterialVnum(number(0, gPlayerMaxLevel)));
					item->SetCount(item->GetCount() - 1);
					return true;
				default:
					break;
				}
#endif

			if ((dwBoxVnum > 51500 && dwBoxVnum < 52000) || (dwBoxVnum >= 50255 && dwBoxVnum <= 50260))	// 용혼원석들
			{
				if (! (this->DragonSoul_IsQualified()))
				{
					ChatPacket (CHAT_TYPE_INFO, "[LS;1094]");
					return false;
				}
			}

			if (GiveItemFromSpecialItemGroup (dwBoxVnum, dwVnums, dwCounts, item_gets, count))
			{
				item->SetCount (item->GetCount()-1);

				for (int i = 0; i < count; i++)
				{
					switch (dwVnums[i])
					{
						case CSpecialItemGroup::GOLD:
							ChatPacket (CHAT_TYPE_INFO, "[LS;1269;%d]", dwCounts[i]);
							break;
						case CSpecialItemGroup::EXP:
							ChatPacket (CHAT_TYPE_INFO, "[LS;1279]");
							ChatPacket (CHAT_TYPE_INFO, "[LS;1290;%d]", dwCounts[i]);
							break;
						case CSpecialItemGroup::MOB:
							ChatPacket (CHAT_TYPE_INFO, "[LS;1299]");
							break;
						case CSpecialItemGroup::SLOW:
							ChatPacket (CHAT_TYPE_INFO, "[LS;1310]");
							break;
						case CSpecialItemGroup::DRAIN_HP:
							ChatPacket (CHAT_TYPE_INFO, "[LS;3]");
							break;
						case CSpecialItemGroup::POISON:
							ChatPacket (CHAT_TYPE_INFO, "[LS;13]");
							break;
						case CSpecialItemGroup::MOB_GROUP:
							ChatPacket (CHAT_TYPE_INFO, "[LS;1299]");
							break;
						default:
							if (item_gets[i])
							{
								if (dwCounts[i] > 1)
								{
									ChatPacket (CHAT_TYPE_INFO, "[LS;24;%s;%d]", item_gets[i]->GetName(), dwCounts[i]);
								}
								else
								{
									ChatPacket (CHAT_TYPE_INFO, "[LS;35;%s]", item_gets[i]->GetName());
								}
							}
					}
				}
			}
			else
			{
				ChatPacket (CHAT_TYPE_TALKING, "[LS;56]");
				return false;
			}
		}
		break;

		case ITEM_SKILLFORGET:
		{
			if (!item->GetSocket (0))
			{
				ITEM_MANAGER::instance().RemoveItem (item);
				return false;
			}

			DWORD dwVnum = item->GetSocket (0);

			if (SkillLevelDown (dwVnum))
			{
				ITEM_MANAGER::instance().RemoveItem (item);
				ChatPacket (CHAT_TYPE_INFO, "[LS;78]");
			}
			else
			{
				ChatPacket (CHAT_TYPE_INFO, "[LS;88]");
			}
		}
		break;

		case ITEM_SKILLBOOK:
		{
			if (IsPolymorphed())
			{
				ChatPacket (CHAT_TYPE_INFO, "[LS;1041]");
				return false;
			}

			DWORD dwVnum = 0;

			if (item->GetVnum() == 50300)
			{
				dwVnum = item->GetSocket (0);
			}
			else
			{
				// 새로운 수련서는 value 0 에 스킬 번호가 있으므로 그것을 사용.
				dwVnum = item->GetValue (0);
			}

			if (0 == dwVnum)
			{
				ITEM_MANAGER::instance().RemoveItem (item);

				return false;
			}

			if (true == LearnSkillByBook (dwVnum))
			{
				ITEM_MANAGER::instance().RemoveItem (item);
				int iReadDelay = number (SKILLBOOK_DELAY_MIN, SKILLBOOK_DELAY_MAX);
				SetSkillNextReadTime (dwVnum, get_global_time() + iReadDelay);
			}
		}
		break;

		case ITEM_USE:
		{
			if (item->GetVnum() > 50800 && item->GetVnum() <= 50820)
			{
				if (test_server)
				{
					sys_log (0, "ADD addtional effect : vnum(%d) subtype(%d)", item->GetOriginalVnum(), item->GetSubType());
				}

				int affect_type = AFFECT_EXP_BONUS_EURO_FREE;
				int apply_type = aApplyInfo[item->GetValue (0)].bPointType;
				int apply_value = item->GetValue (2);
				int apply_duration = item->GetValue (1);

				switch (item->GetSubType())
				{
					case USE_ABILITY_UP:
						if (FindAffect (affect_type, apply_type))
						{
							ChatPacket (CHAT_TYPE_INFO, "[LS;99]");
							return false;
						}

						{
							switch (item->GetValue (0))
							{
								case APPLY_MOV_SPEED:
									AddAffect (affect_type, apply_type, apply_value, AFF_MOV_SPEED_POTION, apply_duration, 0, true, true);
									break;

								case APPLY_ATT_SPEED:
									AddAffect (affect_type, apply_type, apply_value, AFF_ATT_SPEED_POTION, apply_duration, 0, true, true);
									break;

								case APPLY_STR:
								case APPLY_DEX:
								case APPLY_CON:
								case APPLY_INT:
								case APPLY_CAST_SPEED:
								case APPLY_RESIST_MAGIC:
								case APPLY_ATT_GRADE_BONUS:
								case APPLY_DEF_GRADE_BONUS:
									AddAffect (affect_type, apply_type, apply_value, 0, apply_duration, 0, true, true);
									break;
							}
						}

						if (GetDungeon())
						{
							GetDungeon()->UsePotion (this);
						}

						if (GetWarMap())
						{
							GetWarMap()->UsePotion (this, item);
						}

						item->SetCount (item->GetCount() - 1);
						break;

					case USE_AFFECT :
					{
						if (FindAffect (AFFECT_EXP_BONUS_EURO_FREE, aApplyInfo[item->GetValue (1)].bPointType))
						{
							ChatPacket (CHAT_TYPE_INFO, "[LS;99]");
						}
						else
						{
							AddAffect (AFFECT_EXP_BONUS_EURO_FREE, aApplyInfo[item->GetValue (1)].bPointType, item->GetValue (2), 0, item->GetValue (3), 0, false, true);
							item->SetCount (item->GetCount() - 1);
						}
					}
					break;

					case USE_POTION_NODELAY:
					{
						if (CArenaManager::instance().IsArenaMap (GetMapIndex()) == true)
						{
							if (quest::CQuestManager::instance().GetEventFlag ("arena_potion_limit") > 0)
							{
								ChatPacket (CHAT_TYPE_INFO, "[LS;403]");
								return false;
							}

							switch (item->GetVnum())
							{
								case 70020 :
								case 71018 :
								case 71019 :
								case 71020 :
									if (quest::CQuestManager::instance().GetEventFlag ("arena_potion_limit_count") < 10000)
									{
										if (m_nPotionLimit <= 0)
										{
											ChatPacket (CHAT_TYPE_INFO, "[LS;122]");
											return false;
										}
									}
									break;

								default :
									ChatPacket (CHAT_TYPE_INFO, "[LS;403]");
									return false;
									break;
							}
						}

						bool used = false;

						if (item->GetValue (0) != 0) // HP 절대값 회복
						{
							if (GetHP() < GetMaxHP())
							{
								PointChange (POINT_HP, item->GetValue (0) * (100 + GetPoint (POINT_POTION_BONUS)) / 100);
								EffectPacket (SE_HPUP_RED);
								used = TRUE;
							}
						}

						if (item->GetValue (1) != 0)	// SP 절대값 회복
						{
							if (GetSP() < GetMaxSP())
							{
								PointChange (POINT_SP, item->GetValue (1) * (100 + GetPoint (POINT_POTION_BONUS)) / 100);
								EffectPacket (SE_SPUP_BLUE);
								used = TRUE;
							}
						}

						if (item->GetValue (3) != 0) // HP % 회복
						{
							if (GetHP() < GetMaxHP())
							{
								PointChange (POINT_HP, item->GetValue (3) * GetMaxHP() / 100);
								EffectPacket (SE_HPUP_RED);
								used = TRUE;
							}
						}

						if (item->GetValue (4) != 0) // SP % 회복
						{
							if (GetSP() < GetMaxSP())
							{
								PointChange (POINT_SP, item->GetValue (4) * GetMaxSP() / 100);
								EffectPacket (SE_SPUP_BLUE);
								used = TRUE;
							}
						}

						if (used)
						{
							if (item->GetVnum() == 50085 || item->GetVnum() == 50086)
							{
								if (test_server)
								{
									ChatPacket (CHAT_TYPE_INFO, "[LS;132]");
								}
								SetUseSeedOrMoonBottleTime();
							}
							if (GetDungeon())
							{
								GetDungeon()->UsePotion (this);
							}

							if (GetWarMap())
							{
								GetWarMap()->UsePotion (this, item);
							}

							m_nPotionLimit--;

							//RESTRICT_USE_SEED_OR_MOONBOTTLE
							item->SetCount (item->GetCount() - 1);
							//END_RESTRICT_USE_SEED_OR_MOONBOTTLE
						}
					}
					break;
				}

				return true;
			}


			if (item->GetVnum() >= 27863 && item->GetVnum() <= 27883)
			{
				if (CArenaManager::instance().IsArenaMap (GetMapIndex()) == true)
				{
					ChatPacket (CHAT_TYPE_INFO, "[LS;1205]");
					return false;
				}
			}

			if (test_server)
			{
				sys_log (0, "USE_ITEM %s Type %d SubType %d vnum %d", item->GetName(), item->GetType(), item->GetSubType(), item->GetOriginalVnum());
			}

			switch (item->GetSubType())
			{
				case USE_TIME_CHARGE_PER:
				{
					LPITEM pDestItem = GetItem (DestCell);
					if (NULL == pDestItem)
					{
						return false;
					}
					// 우선 용혼석에 관해서만 하도록 한다.
					if (pDestItem->IsDragonSoul())
					{
						int ret;
						char buf[128];
						if (item->GetVnum() == DRAGON_HEART_VNUM)
						{
							ret = pDestItem->GiveMoreTime_Per ((float)item->GetSocket (ITEM_SOCKET_CHARGING_AMOUNT_IDX));
						}
						else
						{
							ret = pDestItem->GiveMoreTime_Per ((float)item->GetValue (ITEM_VALUE_CHARGING_AMOUNT_IDX));
						}
						if (ret > 0)
						{
							if (item->GetVnum() == DRAGON_HEART_VNUM)
							{
								sprintf (buf, "Inc %ds by item{VN:%d SOC%d:%d}", ret, item->GetVnum(), ITEM_SOCKET_CHARGING_AMOUNT_IDX, item->GetSocket (ITEM_SOCKET_CHARGING_AMOUNT_IDX));
							}
							else
							{
								sprintf (buf, "Inc %ds by item{VN:%d VAL%d:%d}", ret, item->GetVnum(), ITEM_VALUE_CHARGING_AMOUNT_IDX, item->GetValue (ITEM_VALUE_CHARGING_AMOUNT_IDX));
							}

							ChatPacket (CHAT_TYPE_INFO, "[LS;1093;%d]", ret);
							item->SetCount (item->GetCount() - 1);
							LogManager::instance().ItemLog (this, item, "DS_CHARGING_SUCCESS", buf);
							return true;
						}
						else
						{
							if (item->GetVnum() == DRAGON_HEART_VNUM)
							{
								sprintf (buf, "No change by item{VN:%d SOC%d:%d}", item->GetVnum(), ITEM_SOCKET_CHARGING_AMOUNT_IDX, item->GetSocket (ITEM_SOCKET_CHARGING_AMOUNT_IDX));
							}
							else
							{
								sprintf (buf, "No change by item{VN:%d VAL%d:%d}", item->GetVnum(), ITEM_VALUE_CHARGING_AMOUNT_IDX, item->GetValue (ITEM_VALUE_CHARGING_AMOUNT_IDX));
							}

							ChatPacket (CHAT_TYPE_INFO, "[LS;1066]");
							LogManager::instance().ItemLog (this, item, "DS_CHARGING_FAILED", buf);
							return false;
						}
					}
					else
					{
						return false;
					}
				}
				break;
				case USE_TIME_CHARGE_FIX:
				{
					LPITEM pDestItem = GetItem (DestCell);
					if (NULL == pDestItem)
					{
						return false;
					}
					// 우선 용혼석에 관해서만 하도록 한다.
					if (pDestItem->IsDragonSoul())
					{
						int ret = pDestItem->GiveMoreTime_Fix (item->GetValue (ITEM_VALUE_CHARGING_AMOUNT_IDX));
						char buf[128];
						if (ret)
						{
							ChatPacket (CHAT_TYPE_INFO, "[LS;1093;%d]", ret);
							sprintf (buf, "Increase %ds by item{VN:%d VAL%d:%d}", ret, item->GetVnum(), ITEM_VALUE_CHARGING_AMOUNT_IDX, item->GetValue (ITEM_VALUE_CHARGING_AMOUNT_IDX));
							LogManager::instance().ItemLog (this, item, "DS_CHARGING_SUCCESS", buf);
							item->SetCount (item->GetCount() - 1);
							return true;
						}
						else
						{
							ChatPacket (CHAT_TYPE_INFO, "[LS;1066]");
							sprintf (buf, "No change by item{VN:%d VAL%d:%d}", item->GetVnum(), ITEM_VALUE_CHARGING_AMOUNT_IDX, item->GetValue (ITEM_VALUE_CHARGING_AMOUNT_IDX));
							LogManager::instance().ItemLog (this, item, "DS_CHARGING_FAILED", buf);
							return false;
						}
					}
					else
					{
						return false;
					}
				}
				break;
				case USE_SPECIAL:

					switch (item->GetVnum())
					{
#if defined(__BL_TRANSMUTATION__)
						case TRANSMUTATION_REVERSAL:
							LPITEM item2;
							if (!IsValidItemPosition(DestCell) || !(item2 = GetItem(DestCell)))
								return false;

							if (item2->IsExchanging())
								return false;
							
							if (item2->isLocked())
								return false;

							if (item2->GetTransmutationVnum() == 0)
								return false;

							item2->SetTransmutationVnum(0);
							item2->UpdatePacket();

							item->SetCount(item->GetCount() - 1);
							break;
#endif
#ifdef ENABLE_ADDITIONAL_EQUIPMENT_PAGE
							case ITEM_ADDITIONAL_PAGE_NOT_TRADEABLE:
							case ITEM_ADDITIONAL_PAGE_TRADEABLE:
							{
								if (GetExchange() || GetMyShop() || GetShopOwner() || IsOpenSafebox() || IsCubeOpen()
#if defined(__BL_TRANSMUTATION__)
								|| GetTransmutation()
#endif
								)
								{
									ChatPacket (CHAT_TYPE_INFO, "[LS;1962]");
									return false;
								}

								if (GetSkillLevel(SKILL_ADDITIONAL_PAGE) > 0)
								{
									ChatPacket (CHAT_TYPE_INFO, "[LS;1963]");
									return false;
								}

								SetSkillLevel(SKILL_ADDITIONAL_PAGE, 1);
								SkillLevelPacket();

								ChangeEquip(FIRST_TYPE_EQUIPMENT, true);

								ChatPacket (CHAT_TYPE_INFO, "[LS;1964]");
								item->SetCount(item->GetCount() - 1);
							}
							break;
#endif
						//크리스마스 란주
						case ITEM_NOG_POCKET:
						{
							/*
							란주능력치 : item_proto value 의미
								이동속도  value 1
								공격력	  value 2
								경험치    value 3
								지속시간  value 0 (단위 초)

							*/
							if (FindAffect (AFFECT_NOG_ABILITY))
							{
								ChatPacket (CHAT_TYPE_INFO, "[LS;99]");
								return false;
							}
							long time = item->GetValue (0);
							long moveSpeedPer	= item->GetValue (1);
							long attPer	= item->GetValue (2);
							long expPer			= item->GetValue (3);
							AddAffect (AFFECT_NOG_ABILITY, POINT_MOV_SPEED, moveSpeedPer, AFF_MOV_SPEED_POTION, time, 0, true, true);
							AddAffect (AFFECT_NOG_ABILITY, POINT_MALL_ATTBONUS, attPer, AFF_NONE, time, 0, true, true);
							AddAffect (AFFECT_NOG_ABILITY, POINT_MALL_EXPBONUS, expPer, AFF_NONE, time, 0, true, true);
							item->SetCount (item->GetCount() - 1);
						}
						break;

						//라마단용 사탕
						case ITEM_RAMADAN_CANDY:
						{
							/*
							사탕능력치 : item_proto value 의미
								이동속도  value 1
								공격력	  value 2
								경험치    value 3
								지속시간  value 0 (단위 초)

							*/
							long time = item->GetValue (0);
							long moveSpeedPer	= item->GetValue (1);
							long attPer	= item->GetValue (2);
							long expPer			= item->GetValue (3);
							AddAffect (AFFECT_RAMADAN_ABILITY, POINT_MOV_SPEED, moveSpeedPer, AFF_MOV_SPEED_POTION, time, 0, true, true);
							AddAffect (AFFECT_RAMADAN_ABILITY, POINT_MALL_ATTBONUS, attPer, AFF_NONE, time, 0, true, true);
							AddAffect (AFFECT_RAMADAN_ABILITY, POINT_MALL_EXPBONUS, expPer, AFF_NONE, time, 0, true, true);
							item->SetCount (item->GetCount() - 1);
						}
						break;
						case ITEM_MARRIAGE_RING:
						{
							marriage::TMarriage* pMarriage = marriage::CManager::instance().Get (GetPlayerID());
							if (pMarriage)
							{
								if (pMarriage->ch1 != NULL)
								{
									if (CArenaManager::instance().IsArenaMap (pMarriage->ch1->GetMapIndex()) == true)
									{
										ChatPacket (CHAT_TYPE_INFO, "[LS;1205]");
										break;
									}
								}

								if (pMarriage->ch2 != NULL)
								{
									if (CArenaManager::instance().IsArenaMap (pMarriage->ch2->GetMapIndex()) == true)
									{
										ChatPacket (CHAT_TYPE_INFO, "[LS;1205]");
										break;
									}
								}

								int consumeSP = CalculateConsumeSP (this);

								if (consumeSP < 0)
								{
									return false;
								}

								PointChange (POINT_SP, -consumeSP, false);

								WarpToPID (pMarriage->GetOther (GetPlayerID()));
							}
							else
							{
								ChatPacket (CHAT_TYPE_INFO, "[LS;143]");
							}
						}
						break;

						//기존 용기의 망토
						case UNIQUE_ITEM_CAPE_OF_COURAGE:
						//라마단 보상용 용기의 망토
						case 70057:
						case REWARD_BOX_UNIQUE_ITEM_CAPE_OF_COURAGE:
							AggregateMonster();
							item->SetCount (item->GetCount()-1);
							break;

						case UNIQUE_ITEM_WHITE_FLAG:
							ForgetMyAttacker();
							item->SetCount (item->GetCount()-1);
							break;

						case UNIQUE_ITEM_TREASURE_BOX:
							break;

						case 30093:
						case 30094:
						case 30095:
						case 30096:
							// 복주머니
						{
							const int MAX_BAG_INFO = 26;
							static struct LuckyBagInfo
							{
								DWORD count;
								int prob;
								DWORD vnum;
							} luckybag[MAX_BAG_INFO] =
							{
								{ 1000,	302,	1		},
								{ 10,	150,	27002	},
								{ 10,	75,		27003	},
								{ 10,	100,	27005	},
								{ 10,	50,		27006	},
								{ 10,	80,		27001	},
								{ 10,	50,		27002	},
								{ 10,	80,		27004	},
								{ 10,	50,		27005	},
								{ 1,	10,		50300	},
								{ 1,	6,		92		},
								{ 1,	2,		132		},
								{ 1,	6,		1052	},
								{ 1,	2,		1092	},
								{ 1,	6,		2082	},
								{ 1,	2,		2122	},
								{ 1,	6,		3082	},
								{ 1,	2,		3122	},
								{ 1,	6,		5052	},
								{ 1,	2,		5082	},
								{ 1,	6,		7082	},
								{ 1,	2,		7122	},
								{ 1,	1,		11282	},
								{ 1,	1,		11482	},
								{ 1,	1,		11682	},
								{ 1,	1,		11882	},
							};

							int pct = number (1, 1000);

							int i;
							for (i=0; i<MAX_BAG_INFO; i++)
							{
								if (pct <= luckybag[i].prob)
								{
									break;
								}
								pct -= luckybag[i].prob;
							}
							if (i>=MAX_BAG_INFO)
							{
								return false;
							}

							if (luckybag[i].vnum == 50300)
							{
								// 스킬수련서는 특수하게 준다.
								GiveRandomSkillBook();
							}
							else if (luckybag[i].vnum == 1)
							{
								PointChange (POINT_GOLD, 1000, true);
							}
							else
							{
								AutoGiveItem (luckybag[i].vnum, luckybag[i].count);
							}
							ITEM_MANAGER::instance().RemoveItem (item);
						}
						break;

						case 50004: // 이벤트용 감지기
						{
							if (item->GetSocket (0))
							{
								item->SetSocket (0, item->GetSocket (0) + 1);
							}
							else
							{
								// 처음 사용시
								int iMapIndex = GetMapIndex();

								PIXEL_POSITION pos;

								if (SECTREE_MANAGER::instance().GetRandomLocation (iMapIndex, pos, 700))
								{
									item->SetSocket (0, 1);
									item->SetSocket (1, pos.x);
									item->SetSocket (2, pos.y);
								}
								else
								{
									ChatPacket (CHAT_TYPE_INFO, "[LS;154]");
									return false;
								}
							}

							int dist = 0;
							float distance = (DISTANCE_SQRT (GetX()-item->GetSocket (1), GetY()-item->GetSocket (2)));

							if (distance < 1000.0f)
							{
								// 발견!
								ChatPacket (CHAT_TYPE_INFO, "[LS;165]");

								// 사용횟수에 따라 주는 아이템을 다르게 한다.
								struct TEventStoneInfo
								{
									DWORD dwVnum;
									int count;
									int prob;
								};
								const int EVENT_STONE_MAX_INFO = 15;
								TEventStoneInfo info_10[EVENT_STONE_MAX_INFO] =
								{
									{ 27001, 10,  8 },
									{ 27004, 10,  6 },
									{ 27002, 10, 12 },
									{ 27005, 10, 12 },
									{ 27100,  1,  9 },
									{ 27103,  1,  9 },
									{ 27101,  1, 10 },
									{ 27104,  1, 10 },
									{ 27999,  1, 12 },

									{ 25040,  1,  4 },

									{ 27410,  1,  0 },
									{ 27600,  1,  0 },
									{ 25100,  1,  0 },

									{ 50001,  1,  0 },
									{ 50003,  1,  1 },
								};
								TEventStoneInfo info_7[EVENT_STONE_MAX_INFO] =
								{
									{ 27001, 10,  1 },
									{ 27004, 10,  1 },
									{ 27004, 10,  9 },
									{ 27005, 10,  9 },
									{ 27100,  1,  5 },
									{ 27103,  1,  5 },
									{ 27101,  1, 10 },
									{ 27104,  1, 10 },
									{ 27999,  1, 14 },

									{ 25040,  1,  5 },

									{ 27410,  1,  5 },
									{ 27600,  1,  5 },
									{ 25100,  1,  5 },

									{ 50001,  1,  0 },
									{ 50003,  1,  5 },

								};
								TEventStoneInfo info_4[EVENT_STONE_MAX_INFO] =
								{
									{ 27001, 10,  0 },
									{ 27004, 10,  0 },
									{ 27002, 10,  0 },
									{ 27005, 10,  0 },
									{ 27100,  1,  0 },
									{ 27103,  1,  0 },
									{ 27101,  1,  0 },
									{ 27104,  1,  0 },
									{ 27999,  1, 25 },

									{ 25040,  1,  0 },

									{ 27410,  1,  0 },
									{ 27600,  1,  0 },
									{ 25100,  1, 15 },

									{ 50001,  1, 10 },
									{ 50003,  1, 50 },

								};

								{
									TEventStoneInfo* info;
									if (item->GetSocket (0) <= 4)
									{
										info = info_4;
									}
									else if (item->GetSocket (0) <= 7)
									{
										info = info_7;
									}
									else
									{
										info = info_10;
									}

									int prob = number (1, 100);

									for (int i = 0; i < EVENT_STONE_MAX_INFO; ++i)
									{
										if (!info[i].prob)
										{
											continue;
										}

										if (prob <= info[i].prob)
										{
											AutoGiveItem (info[i].dwVnum, info[i].count);
											break;
										}
										prob -= info[i].prob;
									}
								}

								char chatbuf[CHAT_MAX_LEN + 1];
								int len = snprintf (chatbuf, sizeof (chatbuf), "StoneDetect %u 0 0", (DWORD)GetVID());

								if (len < 0 || len >= (int) sizeof (chatbuf))
								{
									len = sizeof (chatbuf) - 1;
								}

								++len;  // \0 문자까지 보내기

								TPacketGCChat pack_chat;
								pack_chat.header	= HEADER_GC_CHAT;
								pack_chat.size		= sizeof (TPacketGCChat) + len;
								pack_chat.type		= CHAT_TYPE_COMMAND;
								pack_chat.id		= 0;
								pack_chat.bEmpire	= GetDesc()->GetEmpire();
								//pack_chat.id	= vid;

								TEMP_BUFFER buf;
								buf.write (&pack_chat, sizeof (TPacketGCChat));
								buf.write (chatbuf, len);

								PacketAround (buf.read_peek(), buf.size());

								ITEM_MANAGER::instance().RemoveItem (item, "REMOVE (DETECT_EVENT_STONE) 1");
								return true;
							}
							else if (distance < 20000)
							{
								dist = 1;
							}
							else if (distance < 70000)
							{
								dist = 2;
							}
							else
							{
								dist = 3;
							}

							// 많이 사용했으면 사라진다.
							const int STONE_DETECT_MAX_TRY = 10;
							if (item->GetSocket (0) >= STONE_DETECT_MAX_TRY)
							{
								ChatPacket (CHAT_TYPE_INFO, "[LS;176]");
								ITEM_MANAGER::instance().RemoveItem (item, "REMOVE (DETECT_EVENT_STONE) 0");
								AutoGiveItem (27002);
								return true;
							}

							if (dist)
							{
								char chatbuf[CHAT_MAX_LEN + 1];
								int len = snprintf (chatbuf, sizeof (chatbuf),
													"StoneDetect %u %d %d",
													(DWORD)GetVID(), dist, (int)GetDegreeFromPositionXY (GetX(), item->GetSocket (2), item->GetSocket (1), GetY()));

								if (len < 0 || len >= (int) sizeof (chatbuf))
								{
									len = sizeof (chatbuf) - 1;
								}

								++len;  // \0 문자까지 보내기

								TPacketGCChat pack_chat;
								pack_chat.header	= HEADER_GC_CHAT;
								pack_chat.size		= sizeof (TPacketGCChat) + len;
								pack_chat.type		= CHAT_TYPE_COMMAND;
								pack_chat.id		= 0;
								pack_chat.bEmpire	= GetDesc()->GetEmpire();
								//pack_chat.id		= vid;

								TEMP_BUFFER buf;
								buf.write (&pack_chat, sizeof (TPacketGCChat));
								buf.write (chatbuf, len);

								PacketAround (buf.read_peek(), buf.size());
							}

						}
						break;

						case 27989: // 영석감지기
						case 76006: // 선물용 영석감지기
						{
							LPSECTREE_MAP pMap = SECTREE_MANAGER::instance().GetMap (GetMapIndex());

							if (pMap != NULL)
							{
								item->SetSocket (0, item->GetSocket (0) + 1);

								FFindStone f;

								// <Factor> SECTREE::for_each -> SECTREE::for_each_entity
								pMap->for_each (f);

								if (f.m_mapStone.size() > 0)
								{
									std::map<DWORD, LPCHARACTER>::iterator stone = f.m_mapStone.begin();

									DWORD max = UINT_MAX;
									LPCHARACTER pTarget = stone->second;

									while (stone != f.m_mapStone.end())
									{
										DWORD dist = (DWORD)DISTANCE_SQRT (GetX()-stone->second->GetX(), GetY()-stone->second->GetY());

										if (dist != 0 && max > dist)
										{
											max = dist;
											pTarget = stone->second;
										}
										stone++;
									}

									if (pTarget != NULL)
									{
										int val = 3;

										if (max < 10000)
										{
											val = 2;
										}
										else if (max < 70000)
										{
											val = 1;
										}

										ChatPacket (CHAT_TYPE_COMMAND, "StoneDetect %u %d %d", (DWORD)GetVID(), val,
													(int)GetDegreeFromPositionXY (GetX(), pTarget->GetY(), pTarget->GetX(), GetY()));
									}
									else
									{
										ChatPacket (CHAT_TYPE_INFO, "[LS;1071]");
									}
								}
								else
								{
									ChatPacket (CHAT_TYPE_INFO, "[LS;1071]");
								}

								if (item->GetSocket (0) >= 6)
								{
									ChatPacket (CHAT_TYPE_COMMAND, "StoneDetect %u 0 0", (DWORD)GetVID());
									ITEM_MANAGER::instance().RemoveItem (item);
								}
							}
							break;
						}
						break;

						case 27996: // 독병
							item->SetCount (item->GetCount() - 1);
							/*if (GetSkillLevel(SKILL_CREATE_POISON))
							  AddAffect(AFFECT_ATT_GRADE, POINT_ATT_GRADE, 3, AFF_DRINK_POISON, 15*60, 0, true);
							  else
							  {
							// 독다루기가 없으면 50% 즉사 50% 공격력 +2
							if (number(0, 1))
							{
							if (GetHP() > 100)
							PointChange(POINT_HP, -(GetHP() - 1));
							else
							Dead();
							}
							else
							AddAffect(AFFECT_ATT_GRADE, POINT_ATT_GRADE, 2, AFF_DRINK_POISON, 15*60, 0, true);
							}*/
							break;

						case 27987: // 조개
							// 50  돌조각 47990
							// 30  꽝
							// 10  백진주 47992
							// 7   청진주 47993
							// 3   피진주 47994
						{
							item->SetCount (item->GetCount() - 1);

							int r = number (1, 100);

							if (r <= 50)
							{
								ChatPacket (CHAT_TYPE_INFO, "[LS;221]");
								AutoGiveItem (27990);
							}
							else
							{
								const int prob_table[] =
								{
									95, 97, 99
								};

								if (r <= prob_table[0])
								{
									ChatPacket (CHAT_TYPE_INFO, "[LS;232]");
								}
								else if (r <= prob_table[1])
								{
									ChatPacket (CHAT_TYPE_INFO, "[LS;243]");
									AutoGiveItem (27992);
								}
								else if (r <= prob_table[2])
								{
									ChatPacket (CHAT_TYPE_INFO, "[LS;253]");
									AutoGiveItem (27993);
								}
								else
								{
									ChatPacket (CHAT_TYPE_INFO, "[LS;264]");
									AutoGiveItem (27994);
								}
							}
						}
						break;

						case 71013: // 축제용폭죽
							CreateFly (number (FLY_FIREWORK1, FLY_FIREWORK6), this);
							item->SetCount (item->GetCount() - 1);
							break;

						case 50100: // 폭죽
						case 50101:
						case 50102:
						case 50103:
						case 50104:
						case 50105:
						case 50106:
							CreateFly (item->GetVnum() - 50100 + FLY_FIREWORK1, this);
							item->SetCount (item->GetCount() - 1);
							break;

						case 50200: // 보따리
							if (g_bEnableBootaryCheck)
							{
								if (IS_BOTARYABLE_ZONE (GetMapIndex()) == true)
								{
									__OpenPrivateShop();
								}
								else
								{
									ChatPacket (CHAT_TYPE_INFO, "[LS;216]");
								}
							}
							else
							{
								__OpenPrivateShop();
							}
							break;

						case fishing::FISH_MIND_PILL_VNUM:
							AddAffect (AFFECT_FISH_MIND_PILL, POINT_NONE, 0, AFF_FISH_MIND, 20*60, 0, true);
							item->SetCount (item->GetCount() - 1);
							break;

						case 50301: // 통솔력 수련서
						case 50302:
						case 50303:
						{
							if (IsPolymorphed() == true)
							{
								ChatPacket (CHAT_TYPE_INFO, "[LS;521]");
								return false;
							}

							int lv = GetSkillLevel (SKILL_LEADERSHIP);

							if (lv < item->GetValue (0))
							{
								ChatPacket (CHAT_TYPE_INFO, "[LS;274]");
								return false;
							}

							if (lv >= item->GetValue (1))
							{
								ChatPacket (CHAT_TYPE_INFO, "[LS;284]");
								return false;
							}

							if (LearnSkillByBook (SKILL_LEADERSHIP))
							{
								ITEM_MANAGER::instance().RemoveItem (item);

								int iReadDelay = number (SKILLBOOK_DELAY_MIN, SKILLBOOK_DELAY_MAX);

								SetSkillNextReadTime (SKILL_LEADERSHIP, get_global_time() + iReadDelay);
							}
						}
						break;

						case 50304: // 연계기 수련서
						case 50305:
						case 50306:
						{
							if (IsPolymorphed())
							{
								ChatPacket (CHAT_TYPE_INFO, "[LS;1041]");
								return false;

							}
							if (GetSkillLevel (SKILL_COMBO) == 0 && GetLevel() < 30)
							{
								ChatPacket (CHAT_TYPE_INFO, "[LS;295]");
								return false;
							}

							if (GetSkillLevel (SKILL_COMBO) == 1 && GetLevel() < 50)
							{
								ChatPacket (CHAT_TYPE_INFO, "[LS;305]");
								return false;
							}

							if (GetSkillLevel (SKILL_COMBO) >= 2)
							{
								ChatPacket (CHAT_TYPE_INFO, "[LS;316]");
								return false;
							}

							int iPct = item->GetValue (0);

							if (LearnSkillByBook (SKILL_COMBO, iPct))
							{
								ITEM_MANAGER::instance().RemoveItem (item);

								int iReadDelay = number (SKILLBOOK_DELAY_MIN, SKILLBOOK_DELAY_MAX);

								SetSkillNextReadTime (SKILL_COMBO, get_global_time() + iReadDelay);
							}
						}
						break;
						case 50311: // 언어 수련서
						case 50312:
						case 50313:
						{
							if (IsPolymorphed())
							{
								ChatPacket (CHAT_TYPE_INFO, "[LS;1041]");
								return false;

							}
							DWORD dwSkillVnum = item->GetValue (0);
							int iPct = MINMAX (0, item->GetValue (1), 100);
							if (GetSkillLevel (dwSkillVnum)>=20 || dwSkillVnum-SKILL_LANGUAGE1+1 == GetEmpire())
							{
								ChatPacket (CHAT_TYPE_INFO, "[LS;328]");
								return false;
							}

							if (LearnSkillByBook (dwSkillVnum, iPct))
							{
								ITEM_MANAGER::instance().RemoveItem (item);

								int iReadDelay = number (SKILLBOOK_DELAY_MIN, SKILLBOOK_DELAY_MAX);

								SetSkillNextReadTime (dwSkillVnum, get_global_time() + iReadDelay);
							}
						}
						break;

						case 50061 : // 일본 말 소환 스킬 수련서
						{
							if (IsPolymorphed())
							{
								ChatPacket (CHAT_TYPE_INFO, "[LS;1041]");
								return false;

							}
							DWORD dwSkillVnum = item->GetValue (0);
							int iPct = MINMAX (0, item->GetValue (1), 100);

							if (GetSkillLevel (dwSkillVnum) >= 10)
							{
								ChatPacket (CHAT_TYPE_INFO, "[LS;339]");
								return false;
							}

							if (LearnSkillByBook (dwSkillVnum, iPct))
							{
								ITEM_MANAGER::instance().RemoveItem (item);

								int iReadDelay = number (SKILLBOOK_DELAY_MIN, SKILLBOOK_DELAY_MAX);

								SetSkillNextReadTime (dwSkillVnum, get_global_time() + iReadDelay);
							}
						}
						break;

						case 50314:
						case 50315:
						case 50316: // 변신 수련서
						case 50323:
						case 50324: // 증혈 수련서
						case 50325:
						case 50326: // 철통 수련서
						{
							if (IsPolymorphed() == true)
							{
								ChatPacket (CHAT_TYPE_INFO, "[LS;521]");
								return false;
							}

							int iSkillLevelLowLimit = item->GetValue (0);
							int iSkillLevelHighLimit = item->GetValue (1);
							int iPct = MINMAX (0, item->GetValue (2), 100);
							int iLevelLimit = item->GetValue (3);
							DWORD dwSkillVnum = 0;

							switch (item->GetVnum())
							{
								case 50314:
								case 50315:
								case 50316:
									dwSkillVnum = SKILL_POLYMORPH;
									break;

								case 50323:
								case 50324:
									dwSkillVnum = SKILL_ADD_HP;
									break;

								case 50325:
								case 50326:
									dwSkillVnum = SKILL_RESIST_PENETRATE;
									break;

								default:
									return false;
							}

							if (0 == dwSkillVnum)
							{
								return false;
							}

							if (GetLevel() < iLevelLimit)
							{
								ChatPacket (CHAT_TYPE_INFO, "[LS;350]");
								return false;
							}

							if (GetSkillLevel (dwSkillVnum) >= 40)
							{
								ChatPacket (CHAT_TYPE_INFO, "[LS;339]");
								return false;
							}

							if (GetSkillLevel (dwSkillVnum) < iSkillLevelLowLimit)
							{
								ChatPacket (CHAT_TYPE_INFO, "[LS;274]");
								return false;
							}

							if (GetSkillLevel (dwSkillVnum) >= iSkillLevelHighLimit)
							{
								ChatPacket (CHAT_TYPE_INFO, "[LS;371]");
								return false;
							}

							if (LearnSkillByBook (dwSkillVnum, iPct))
							{
								ITEM_MANAGER::instance().RemoveItem (item);

								int iReadDelay = number (SKILLBOOK_DELAY_MIN, SKILLBOOK_DELAY_MAX);

								SetSkillNextReadTime (dwSkillVnum, get_global_time() + iReadDelay);
							}
						}
						break;

						case 50902:
						case 50903:
						case 50904:
						{
							if (IsPolymorphed())
							{
								ChatPacket (CHAT_TYPE_INFO, "[LS;1041]");
								return false;

							}
							DWORD dwSkillVnum = SKILL_CREATE;
							int iPct = MINMAX (0, item->GetValue (1), 100);

							if (GetSkillLevel (dwSkillVnum)>=40)
							{
								ChatPacket (CHAT_TYPE_INFO, "[LS;339]");
								return false;
							}

							if (LearnSkillByBook (dwSkillVnum, iPct))
							{
								ITEM_MANAGER::instance().RemoveItem (item);

								int iReadDelay = number (SKILLBOOK_DELAY_MIN, SKILLBOOK_DELAY_MAX);

								SetSkillNextReadTime (dwSkillVnum, get_global_time() + iReadDelay);

								if (test_server)
								{
									ChatPacket (CHAT_TYPE_INFO, "[TEST_SERVER] Success to learn skill ");
								}
							}
							else
							{
								if (test_server)
								{
									ChatPacket (CHAT_TYPE_INFO, "[TEST_SERVER] Failed to learn skill ");
								}
							}
						}
						break;

						// MINING
						case ITEM_MINING_SKILL_TRAIN_BOOK:
						{
							if (IsPolymorphed())
							{
								ChatPacket (CHAT_TYPE_INFO, "[LS;1041]");
								return false;

							}
							DWORD dwSkillVnum = SKILL_MINING;
							int iPct = MINMAX (0, item->GetValue (1), 100);

							if (GetSkillLevel (dwSkillVnum)>=40)
							{
								ChatPacket (CHAT_TYPE_INFO, "[LS;339]");
								return false;
							}

							if (LearnSkillByBook (dwSkillVnum, iPct))
							{
								ITEM_MANAGER::instance().RemoveItem (item);

								int iReadDelay = number (SKILLBOOK_DELAY_MIN, SKILLBOOK_DELAY_MAX);

								SetSkillNextReadTime (dwSkillVnum, get_global_time() + iReadDelay);
							}
						}
						break;
						// END_OF_MINING

						case ITEM_HORSE_SKILL_TRAIN_BOOK:
						{
							if (IsPolymorphed())
							{
								ChatPacket (CHAT_TYPE_INFO, "[LS;1041]");
								return false;

							}
							DWORD dwSkillVnum = SKILL_HORSE;
							int iPct = MINMAX (0, item->GetValue (1), 100);

							if (GetLevel() < 50)
							{
								ChatPacket (CHAT_TYPE_INFO, "[LS;376]");
								return false;
							}

							if (!test_server && get_global_time() < GetSkillNextReadTime (dwSkillVnum))
							{
								if (FindAffect (AFFECT_SKILL_NO_BOOK_DELAY))
								{
									// 주안술서 사용중에는 시간 제한 무시
									RemoveAffect (AFFECT_SKILL_NO_BOOK_DELAY);
									ChatPacket (CHAT_TYPE_INFO, "[LS;377]");
								}
								else
								{
									SkillLearnWaitMoreTimeMessage (GetSkillNextReadTime (dwSkillVnum) - get_global_time());
									return false;
								}
							}

							if (GetPoint (POINT_HORSE_SKILL) >= 20 ||
									GetSkillLevel (SKILL_HORSE_WILDATTACK) + GetSkillLevel (SKILL_HORSE_CHARGE) + GetSkillLevel (SKILL_HORSE_ESCAPE) >= 60 ||
									GetSkillLevel (SKILL_HORSE_WILDATTACK_RANGE) + GetSkillLevel (SKILL_HORSE_CHARGE) + GetSkillLevel (SKILL_HORSE_ESCAPE) >= 60)
							{
								ChatPacket (CHAT_TYPE_INFO, "[LS;378]");
								return false;
							}

							if (number (1, 100) <= iPct)
							{
								ChatPacket (CHAT_TYPE_INFO, "[LS;379]");
								ChatPacket (CHAT_TYPE_INFO, "[LS;380]");
								PointChange (POINT_HORSE_SKILL, 1);

								int iReadDelay = number (SKILLBOOK_DELAY_MIN, SKILLBOOK_DELAY_MAX);

								if (!test_server)
								{
									SetSkillNextReadTime (dwSkillVnum, get_global_time() + iReadDelay);
								}
							}
							else
							{
								ChatPacket (CHAT_TYPE_INFO, "[LS;382]");
							}

							ITEM_MANAGER::instance().RemoveItem (item);
						}
						break;

						case 70102: // 선두
						case 70103: // 선두
						{
							if (GetAlignment() >= 0)
							{
								return false;
							}

							int delta = MIN (-GetAlignment(), item->GetValue (0));

							sys_log (0, "%s ALIGNMENT ITEM %d", GetName(), delta);

							UpdateAlignment (delta);
							item->SetCount (item->GetCount() - 1);

							if (delta / 10 > 0)
							{
								ChatPacket (CHAT_TYPE_TALKING, "[LS;383]");
								ChatPacket (CHAT_TYPE_INFO, "[LS;384;%d]", delta/10);
							}
						}
						break;

						case 71107: // 천도복숭아
						{
							int val = item->GetValue (0);
							int interval = item->GetValue (1);
							quest::PC* pPC = quest::CQuestManager::instance().GetPC (GetPlayerID());
							int last_use_time = pPC->GetFlag ("mythical_peach.last_use_time");

							if (get_global_time() - last_use_time < interval * 60 * 60)
							{
								if (test_server == false)
								{
									ChatPacket (CHAT_TYPE_INFO, "[LS;1033]");
									return false;
								}
								else
								{
									ChatPacket (CHAT_TYPE_INFO, "[LS;1034]");
								}
							}

							if (GetAlignment() == 200000)
							{
								ChatPacket (CHAT_TYPE_INFO, "[LS;1036]");
								return false;
							}

							if (200000 - GetAlignment() < val * 10)
							{
								val = (200000 - GetAlignment()) / 10;
							}

							int old_alignment = GetAlignment() / 10;

							UpdateAlignment (val*10);

							item->SetCount (item->GetCount()-1);
							pPC->SetFlag ("mythical_peach.last_use_time", get_global_time());

							ChatPacket (CHAT_TYPE_TALKING, "[LS;383]");
							ChatPacket (CHAT_TYPE_INFO, "[LS;384]", val);

							char buf[256 + 1];
							snprintf (buf, sizeof (buf), "%d %d", old_alignment, GetAlignment() / 10);
							LogManager::instance().CharLog (this, val, "MYTHICAL_PEACH", buf);
						}
						break;

						case 71109: // 탈석서
						case 72719:
						{
							LPITEM item2;

							if (!IsValidItemPosition (DestCell) || ! (item2 = GetItem (DestCell)))
							{
								return false;
							}

							if (item2->IsExchanging() == true)
							{
								return false;
							}

							if (item2->GetSocketCount() == 0)
							{
								return false;
							}

							switch (item2->GetType())
							{
								case ITEM_WEAPON:
									break;
								case ITEM_ARMOR:
									switch (item2->GetSubType())
									{
										case ARMOR_EAR:
										case ARMOR_WRIST:
										case ARMOR_NECK:
											ChatPacket (CHAT_TYPE_INFO, "[LS;395]");
											return false;
									}
									break;

								default:
									return false;
							}

							std::stack<long> socket;

							for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
							{
								socket.push (item2->GetSocket (i));
							}

							int idx = ITEM_SOCKET_MAX_NUM - 1;

							while (socket.size() > 0)
							{
								if (socket.top() > 2 && socket.top() != ITEM_BROKEN_METIN_VNUM)
								{
									break;
								}

								idx--;
								socket.pop();
							}

							if (socket.size() == 0)
							{
								ChatPacket (CHAT_TYPE_INFO, "[LS;395]");
								return false;
							}

							LPITEM pItemReward = AutoGiveItem (socket.top());

							if (pItemReward != NULL)
							{
								item2->SetSocket (idx, 1);

								char buf[256+1];
								snprintf (buf, sizeof (buf), "%s(%u) %s(%u)",
										  item2->GetName(), item2->GetID(), pItemReward->GetName(), pItemReward->GetID());
								LogManager::instance().ItemLog (this, item, "USE_DETACHMENT_ONE", buf);

								item->SetCount (item->GetCount() - 1);
							}
						}
						break;

						case 70201:   // 탈색제
						case 70202:   // 염색약(흰색)
						case 70203:   // 염색약(금색)
						case 70204:   // 염색약(빨간색)
						case 70205:   // 염색약(갈색)
						case 70206:   // 염색약(검은색)
						{
							// NEW_HAIR_STYLE_ADD
							if (GetPart (PART_HAIR) >= 1001)
							{
								ChatPacket (CHAT_TYPE_INFO, "[LS;385]");
							}
							// END_NEW_HAIR_STYLE_ADD
							else
							{
								quest::CQuestManager& q = quest::CQuestManager::instance();
								quest::PC* pPC = q.GetPC (GetPlayerID());

								if (pPC)
								{
									int last_dye_level = pPC->GetFlag ("dyeing_hair.last_dye_level");

									if (last_dye_level == 0 ||
											last_dye_level+3 <= GetLevel() ||
											item->GetVnum() == 70201)
									{
										SetPart (PART_HAIR, item->GetVnum() - 70201);

										if (item->GetVnum() == 70201)
										{
											pPC->SetFlag ("dyeing_hair.last_dye_level", 0);
										}
										else
										{
											pPC->SetFlag ("dyeing_hair.last_dye_level", GetLevel());
										}

										item->SetCount (item->GetCount() - 1);
										UpdatePacket();
									}
									else
									{
										ChatPacket (CHAT_TYPE_INFO, "[LS;386;%d]", last_dye_level+3);
									}
								}
							}
						}
						break;

						case ITEM_NEW_YEAR_GREETING_VNUM:
						{
							DWORD dwBoxVnum = ITEM_NEW_YEAR_GREETING_VNUM;
							std::vector <DWORD> dwVnums;
							std::vector <DWORD> dwCounts;
							std::vector <LPITEM> item_gets;
							int count = 0;

							if (GiveItemFromSpecialItemGroup (dwBoxVnum, dwVnums, dwCounts, item_gets, count))
							{
								for (int i = 0; i < count; i++)
								{
									if (dwVnums[i] == CSpecialItemGroup::GOLD)
									{
										ChatPacket (CHAT_TYPE_INFO, "[LS;1269;%d]", dwCounts[i]);
									}
								}

								item->SetCount (item->GetCount() - 1);
							}
						}
						break;

						case ITEM_VALENTINE_ROSE:
						case ITEM_VALENTINE_CHOCOLATE:
						{
							DWORD dwBoxVnum = item->GetVnum();
							std::vector <DWORD> dwVnums;
							std::vector <DWORD> dwCounts;
							std::vector <LPITEM> item_gets;
							int count = 0;


							if (item->GetVnum() == ITEM_VALENTINE_ROSE && SEX_MALE==GET_SEX (this) ||
									item->GetVnum() == ITEM_VALENTINE_CHOCOLATE && SEX_FEMALE==GET_SEX (this))
							{
								// 성별이 맞지않아 쓸 수 없다.
								ChatPacket (CHAT_TYPE_INFO, "[LS;387]");
								return false;
							}


							if (GiveItemFromSpecialItemGroup (dwBoxVnum, dwVnums, dwCounts, item_gets, count))
							{
								item->SetCount (item->GetCount()-1);
							}
						}
						break;

						case ITEM_WHITEDAY_CANDY:
						case ITEM_WHITEDAY_ROSE:
						{
							DWORD dwBoxVnum = item->GetVnum();
							std::vector <DWORD> dwVnums;
							std::vector <DWORD> dwCounts;
							std::vector <LPITEM> item_gets;
							int count = 0;


							if (item->GetVnum() == ITEM_WHITEDAY_CANDY && SEX_MALE==GET_SEX (this) ||
									item->GetVnum() == ITEM_WHITEDAY_ROSE && SEX_FEMALE==GET_SEX (this))
							{
								// 성별이 맞지않아 쓸 수 없다.
								ChatPacket (CHAT_TYPE_INFO, "[LS;387]");
								return false;
							}


							if (GiveItemFromSpecialItemGroup (dwBoxVnum, dwVnums, dwCounts, item_gets, count))
							{
								item->SetCount (item->GetCount()-1);
							}
						}
						break;

						case 50011: // 월광보합
						{
							DWORD dwBoxVnum = 50011;
							std::vector <DWORD> dwVnums;
							std::vector <DWORD> dwCounts;
							std::vector <LPITEM> item_gets;
							int count = 0;

							if (GiveItemFromSpecialItemGroup (dwBoxVnum, dwVnums, dwCounts, item_gets, count))
							{
								for (int i = 0; i < count; i++)
								{
									char buf[50 + 1];
									snprintf (buf, sizeof (buf), "%u %u", dwVnums[i], dwCounts[i]);
									LogManager::instance().ItemLog (this, item, "MOONLIGHT_GET", buf);

									//ITEM_MANAGER::instance().RemoveItem(item);
									item->SetCount (item->GetCount() - 1);

									switch (dwVnums[i])
									{
										case CSpecialItemGroup::GOLD:
											ChatPacket (CHAT_TYPE_INFO, "[LS;1269;%d]", dwCounts[i]);
											break;

										case CSpecialItemGroup::EXP:
											ChatPacket (CHAT_TYPE_INFO, "[LS;1279]");
											ChatPacket (CHAT_TYPE_INFO, "[LS;1290;%d]", dwCounts[i]);
											break;

										case CSpecialItemGroup::MOB:
											ChatPacket (CHAT_TYPE_INFO, "[LS;1299]");
											break;

										case CSpecialItemGroup::SLOW:
											ChatPacket (CHAT_TYPE_INFO, "[LS;1310]");
											break;

										case CSpecialItemGroup::DRAIN_HP:
											ChatPacket (CHAT_TYPE_INFO, "[LS;3]");
											break;

										case CSpecialItemGroup::POISON:
											ChatPacket (CHAT_TYPE_INFO, "[LS;13]");
											break;

										case CSpecialItemGroup::MOB_GROUP:
											ChatPacket (CHAT_TYPE_INFO, "[LS;1299]");
											break;

										default:
											if (item_gets[i])
											{
												if (dwCounts[i] > 1)
												{
													ChatPacket (CHAT_TYPE_INFO, "[LS;24;%s;%d]", item_gets[i]->GetName(), dwCounts[i]);
												}
												else
												{
													ChatPacket (CHAT_TYPE_INFO, "[LS;35;%s]", item_gets[i]->GetName());
												}
											}
											break;
									}
								}
							}
							else
							{
								ChatPacket (CHAT_TYPE_TALKING, "[LS;56]");
								return false;
							}
						}
						break;

						case ITEM_GIVE_STAT_RESET_COUNT_VNUM:
						{
							//PointChange(POINT_GOLD, -iCost);
							PointChange (POINT_STAT_RESET_COUNT, 1);
							item->SetCount (item->GetCount()-1);
						}
						break;

						case 50107:
						{
							EffectPacket (SE_CHINA_FIREWORK);
							// 스턴 공격을 올려준다
							AddAffect (AFFECT_CHINA_FIREWORK, POINT_STUN_PCT, 30, AFF_CHINA_FIREWORK, 5*60, 0, true);
							item->SetCount (item->GetCount()-1);
						}
						break;

						case 50108:
						{
							if (CArenaManager::instance().IsArenaMap (GetMapIndex()) == true)
							{
								ChatPacket (CHAT_TYPE_INFO, "[LS;1205]");
								return false;
							}

							EffectPacket (SE_SPIN_TOP);
							// 스턴 공격을 올려준다
							AddAffect (AFFECT_CHINA_FIREWORK, POINT_STUN_PCT, 30, AFF_CHINA_FIREWORK, 5*60, 0, true);
							item->SetCount (item->GetCount()-1);
						}
						break;

						case ITEM_WONSO_BEAN_VNUM:
							PointChange (POINT_HP, GetMaxHP() - GetHP());
							item->SetCount (item->GetCount()-1);
							break;

						case ITEM_WONSO_SUGAR_VNUM:
							PointChange (POINT_SP, GetMaxSP() - GetSP());
							item->SetCount (item->GetCount()-1);
							break;

						case ITEM_WONSO_FRUIT_VNUM:
							PointChange (POINT_STAMINA, GetMaxStamina()-GetStamina());
							item->SetCount (item->GetCount()-1);
							break;

						case ITEM_ELK_VNUM: // 돈꾸러미
						{
							int iGold = item->GetSocket (0);
							ITEM_MANAGER::instance().RemoveItem (item);
							ChatPacket (CHAT_TYPE_INFO, "[LS;1216;%d]", iGold);
							PointChange (POINT_GOLD, iGold);
						}
						break;

						case 27995:
						{
						}
						break;

						case 71092 : // 변신 해체부 임시
						{
							if (m_pkChrTarget != NULL)
							{
								if (m_pkChrTarget->IsPolymorphed())
								{
									m_pkChrTarget->SetPolymorph (0);
									m_pkChrTarget->RemoveAffect (AFFECT_POLYMORPH);
								}
							}
							else
							{
								if (IsPolymorphed())
								{
									SetPolymorph (0);
									RemoveAffect (AFFECT_POLYMORPH);
								}
							}
						}
						break;

#if !defined(__BL_67_ATTR__)
							case 71051 : // 진재가
								{
									// 유럽, 싱가폴, 베트남 진재가 사용금지
									if (LC_IsEurope() || LC_IsSingapore() || LC_IsVietnam())
										return false;

									LPITEM item2;

									if (!IsValidItemPosition(DestCell) || !(item2 = GetInventoryItem(wDestCell)))
										return false;

									if (item2->IsExchanging() == true)
										return false;

									if (item2->GetAttributeSetIndex() == -1)
									{
										ChatPacket (CHAT_TYPE_INFO, "[LS;2049]");
										
										return false;
									}

									if (item2->AddRareAttribute() == true)
									{
										ChatPacket (CHAT_TYPE_INFO, "[LS;2050]");

										int iAddedIdx = item2->GetRareAttrCount() + 4;
										char buf[21];
										snprintf(buf, sizeof(buf), "%u", item2->GetID());

										LogManager::instance().ItemLog(
												GetPlayerID(),
												item2->GetAttributeType(iAddedIdx),
												item2->GetAttributeValue(iAddedIdx),
												item->GetID(),
												"ADD_RARE_ATTR",
												buf,
												GetDesc()->GetHostName(),
												item->GetOriginalVnum());

										item->SetCount(item->GetCount() - 1);
									}
									else
									{
										ChatPacket (CHAT_TYPE_INFO, "[LS;2051]");
									}
								}
								break;
#endif

#if !defined(__BL_67_ATTR__)
							case 71052 : // 진재경
								{
									// 유럽, 싱가폴, 베트남 진재가 사용금지
									if (LC_IsEurope() || LC_IsSingapore() || LC_IsVietnam())
										return false;

									LPITEM item2;

									if (!IsValidItemPosition(DestCell) || !(item2 = GetItem(DestCell)))
										return false;

									if (item2->IsExchanging() == true)
										return false;

									if (item2->GetAttributeSetIndex() == -1)
									{
										ChatPacket (CHAT_TYPE_INFO, "[LS;2049]");
										return false;
									}

									if (item2->ChangeRareAttribute() == true)
									{
										char buf[21];
										snprintf(buf, sizeof(buf), "%u", item2->GetID());
										LogManager::instance().ItemLog(this, item, "CHANGE_RARE_ATTR", buf);

										item->SetCount(item->GetCount() - 1);
									}
									else
									{
										ChatPacket (CHAT_TYPE_INFO, "[LS;2052]");
									}
								}
								break;
#endif

						case ITEM_AUTO_HP_RECOVERY_S:
						case ITEM_AUTO_HP_RECOVERY_M:
						case ITEM_AUTO_HP_RECOVERY_L:
						case ITEM_AUTO_HP_RECOVERY_X:
						case ITEM_AUTO_SP_RECOVERY_S:
						case ITEM_AUTO_SP_RECOVERY_M:
						case ITEM_AUTO_SP_RECOVERY_L:
						case ITEM_AUTO_SP_RECOVERY_X:
						// 무시무시하지만 이전에 하던 걸 고치기는 무섭고...
						// 그래서 그냥 하드 코딩. 선물 상자용 자동물약 아이템들.
						case REWARD_BOX_ITEM_AUTO_SP_RECOVERY_XS:
						case REWARD_BOX_ITEM_AUTO_SP_RECOVERY_S:
						case REWARD_BOX_ITEM_AUTO_HP_RECOVERY_XS:
						case REWARD_BOX_ITEM_AUTO_HP_RECOVERY_S:
						{
							if (CArenaManager::instance().IsArenaMap (GetMapIndex()) == true)
							{
								ChatPacket (CHAT_TYPE_INFO, "[LS;403]");
								return false;
							}

							EAffectTypes type = AFFECT_NONE;
							bool isSpecialPotion = false;

							switch (item->GetVnum())
							{
								case ITEM_AUTO_HP_RECOVERY_X:
									isSpecialPotion = true;

								case ITEM_AUTO_HP_RECOVERY_S:
								case ITEM_AUTO_HP_RECOVERY_M:
								case ITEM_AUTO_HP_RECOVERY_L:
								case REWARD_BOX_ITEM_AUTO_HP_RECOVERY_XS:
								case REWARD_BOX_ITEM_AUTO_HP_RECOVERY_S:
									type = AFFECT_AUTO_HP_RECOVERY;
									break;

								case ITEM_AUTO_SP_RECOVERY_X:
									isSpecialPotion = true;

								case ITEM_AUTO_SP_RECOVERY_S:
								case ITEM_AUTO_SP_RECOVERY_M:
								case ITEM_AUTO_SP_RECOVERY_L:
								case REWARD_BOX_ITEM_AUTO_SP_RECOVERY_XS:
								case REWARD_BOX_ITEM_AUTO_SP_RECOVERY_S:
									type = AFFECT_AUTO_SP_RECOVERY;
									break;
							}

							if (AFFECT_NONE == type)
							{
								break;
							}

							if (item->GetCount() > 1)
							{
								int pos = GetEmptyInventory (item->GetSize());

								if (-1 == pos)
								{
									ChatPacket (CHAT_TYPE_INFO, "[LS;1130]");
									break;
								}

								item->SetCount (item->GetCount() - 1);

								LPITEM item2 = ITEM_MANAGER::instance().CreateItem (item->GetVnum(), 1);
								item2->AddToCharacter (this, TItemPos (INVENTORY, pos));

								if (item->GetSocket (1) != 0)
								{
									item2->SetSocket (1, item->GetSocket (1));
								}

								item = item2;
							}

							CAffect* pAffect = FindAffect (type);

							if (NULL == pAffect)
							{
								EPointTypes bonus = POINT_NONE;

								if (true == isSpecialPotion)
								{
									if (type == AFFECT_AUTO_HP_RECOVERY)
									{
										bonus = POINT_MAX_HP_PCT;
									}
									else if (type == AFFECT_AUTO_SP_RECOVERY)
									{
										bonus = POINT_MAX_SP_PCT;
									}
								}

								AddAffect (type, bonus, 4, item->GetID(), INFINITE_AFFECT_DURATION, 0, true, false);

								item->Lock (true);
								item->SetSocket (0, true);

								AutoRecoveryItemProcess (type);
							}
							else
							{
								if (item->GetID() == pAffect->dwFlag)
								{
									RemoveAffect (pAffect);

									item->Lock (false);
									item->SetSocket (0, false);
								}
								else
								{
									LPITEM old = FindItemByID (pAffect->dwFlag);

									if (NULL != old)
									{
										old->Lock (false);
										old->SetSocket (0, false);
									}

									RemoveAffect (pAffect);

									EPointTypes bonus = POINT_NONE;

									if (true == isSpecialPotion)
									{
										if (type == AFFECT_AUTO_HP_RECOVERY)
										{
											bonus = POINT_MAX_HP_PCT;
										}
										else if (type == AFFECT_AUTO_SP_RECOVERY)
										{
											bonus = POINT_MAX_SP_PCT;
										}
									}

									AddAffect (type, bonus, 4, item->GetID(), INFINITE_AFFECT_DURATION, 0, true, false);

									item->Lock (true);
									item->SetSocket (0, true);

									AutoRecoveryItemProcess (type);
								}
							}
						}
						break;
#ifdef __AURA_SYSTEM__
			case ITEM_AURA_BOOST_ITEM_VNUM_BASE + ITEM_AURA_BOOST_ERASER:
			{
				LPITEM item2;
				if (!IsValidItemPosition(DestCell) || !(item2 = GetItem(DestCell)))
					return false;

				if (item2->IsExchanging() || item2->IsEquipped())
					return false;

				if (item2->GetSocket(ITEM_SOCKET_AURA_BOOST) == 0)
				{
					ChatPacket (CHAT_TYPE_INFO, "[LS;2008]");
					return false;
				}

				if (IS_SET(item->GetFlag(), ITEM_FLAG_STACKABLE) && !IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_STACK) && item->GetCount() > 1)
					item->SetCount(item->GetCount() - 1);
				else
					ITEM_MANAGER::instance().RemoveItem(item);

				item2->SetSocket(ITEM_SOCKET_AURA_BOOST, 0);
			}
			break;
#endif
					}
					break;

				case USE_CLEAR:
				{
					RemoveBadAffect();
					item->SetCount (item->GetCount() - 1);
				}
				break;

				case USE_INVISIBILITY:
				{
					if (item->GetVnum() == 70026)
					{
						quest::CQuestManager& q = quest::CQuestManager::instance();
						quest::PC* pPC = q.GetPC (GetPlayerID());

						if (pPC != NULL)
						{
							int last_use_time = pPC->GetFlag ("mirror_of_disapper.last_use_time");

							if (get_global_time() - last_use_time < 10*60)
							{
								ChatPacket (CHAT_TYPE_INFO, "[LS;1033]");
								return false;
							}

							pPC->SetFlag ("mirror_of_disapper.last_use_time", get_global_time());
						}
					}

					AddAffect (AFFECT_INVISIBILITY, POINT_NONE, 0, AFF_INVISIBILITY, 300, 0, true);
					item->SetCount (item->GetCount() - 1);
				}
				break;

				case USE_POTION_NODELAY:
				{
					if (CArenaManager::instance().IsArenaMap (GetMapIndex()) == true)
					{
						if (quest::CQuestManager::instance().GetEventFlag ("arena_potion_limit") > 0)
						{
							ChatPacket (CHAT_TYPE_INFO, "[LS;403]");
							return false;
						}

						switch (item->GetVnum())
						{
							case 70020 :
							case 71018 :
							case 71019 :
							case 71020 :
								if (quest::CQuestManager::instance().GetEventFlag ("arena_potion_limit_count") < 10000)
								{
									if (m_nPotionLimit <= 0)
									{
										ChatPacket (CHAT_TYPE_INFO, "[LS;122]");
										return false;
									}
								}
								break;

							default :
								ChatPacket (CHAT_TYPE_INFO, "[LS;403]");
								return false;
						}
					}

					bool used = false;

					if (item->GetValue (0) != 0) // HP 절대값 회복
					{
						if (GetHP() < GetMaxHP())
						{
							PointChange (POINT_HP, item->GetValue (0) * (100 + GetPoint (POINT_POTION_BONUS)) / 100);
							EffectPacket (SE_HPUP_RED);
							used = TRUE;
						}
					}

					if (item->GetValue (1) != 0)	// SP 절대값 회복
					{
						if (GetSP() < GetMaxSP())
						{
							PointChange (POINT_SP, item->GetValue (1) * (100 + GetPoint (POINT_POTION_BONUS)) / 100);
							EffectPacket (SE_SPUP_BLUE);
							used = TRUE;
						}
					}

					if (item->GetValue (3) != 0) // HP % 회복
					{
						if (GetHP() < GetMaxHP())
						{
							PointChange (POINT_HP, item->GetValue (3) * GetMaxHP() / 100);
							EffectPacket (SE_HPUP_RED);
							used = TRUE;
						}
					}

					if (item->GetValue (4) != 0) // SP % 회복
					{
						if (GetSP() < GetMaxSP())
						{
							PointChange (POINT_SP, item->GetValue (4) * GetMaxSP() / 100);
							EffectPacket (SE_SPUP_BLUE);
							used = TRUE;
						}
					}

					if (used)
					{
						if (item->GetVnum() == 50085 || item->GetVnum() == 50086)
						{
							if (test_server)
							{
								ChatPacket (CHAT_TYPE_INFO, "[LS;132]");
							}
							SetUseSeedOrMoonBottleTime();
						}
						if (GetDungeon())
						{
							GetDungeon()->UsePotion (this);
						}

						if (GetWarMap())
						{
							GetWarMap()->UsePotion (this, item);
						}

						m_nPotionLimit--;

						//RESTRICT_USE_SEED_OR_MOONBOTTLE
						item->SetCount (item->GetCount() - 1);
						//END_RESTRICT_USE_SEED_OR_MOONBOTTLE
					}
				}
				break;

				case USE_POTION:
					if (CArenaManager::instance().IsArenaMap (GetMapIndex()) == true)
					{
						if (quest::CQuestManager::instance().GetEventFlag ("arena_potion_limit") > 0)
						{
							ChatPacket (CHAT_TYPE_INFO, "[LS;403]");
							return false;
						}

						switch (item->GetVnum())
						{
							case 27001 :
							case 27002 :
							case 27003 :
							case 27004 :
							case 27005 :
							case 27006 :
								if (quest::CQuestManager::instance().GetEventFlag ("arena_potion_limit_count") < 10000)
								{
									if (m_nPotionLimit <= 0)
									{
										ChatPacket (CHAT_TYPE_INFO, "[LS;122]");
										return false;
									}
								}
								break;

							default :
								ChatPacket (CHAT_TYPE_INFO, "[LS;403]");
								return false;
						}
					}

					if (item->GetValue (1) != 0)
					{
						if (GetPoint (POINT_SP_RECOVERY) + GetSP() >= GetMaxSP())
						{
							return false;
						}

						PointChange (POINT_SP_RECOVERY, item->GetValue (1) * MIN (200, (100 + GetPoint (POINT_POTION_BONUS))) / 100);
						StartAffectEvent();
						EffectPacket (SE_SPUP_BLUE);
					}

					if (item->GetValue (0) != 0)
					{
						if (GetPoint (POINT_HP_RECOVERY) + GetHP() >= GetMaxHP())
						{
							return false;
						}

						PointChange (POINT_HP_RECOVERY, item->GetValue (0) * MIN (200, (100 + GetPoint (POINT_POTION_BONUS))) / 100);
						StartAffectEvent();
						EffectPacket (SE_HPUP_RED);
					}

					if (GetDungeon())
					{
						GetDungeon()->UsePotion (this);
					}

					if (GetWarMap())
					{
						GetWarMap()->UsePotion (this, item);
					}

					item->SetCount (item->GetCount() - 1);
					m_nPotionLimit--;
					break;

				case USE_POTION_CONTINUE:
				{
					if (item->GetValue (0) != 0)
					{
						AddAffect (AFFECT_HP_RECOVER_CONTINUE, POINT_HP_RECOVER_CONTINUE, item->GetValue (0), 0, item->GetValue (2), 0, true);
					}
					else if (item->GetValue (1) != 0)
					{
						AddAffect (AFFECT_SP_RECOVER_CONTINUE, POINT_SP_RECOVER_CONTINUE, item->GetValue (1), 0, item->GetValue (2), 0, true);
					}
					else
					{
						return false;
					}
				}

				if (GetDungeon())
				{
					GetDungeon()->UsePotion (this);
				}

				if (GetWarMap())
				{
					GetWarMap()->UsePotion (this, item);
				}

				item->SetCount (item->GetCount() - 1);
				break;

				case USE_ABILITY_UP:
				{
					switch (item->GetValue (0))
					{
						case APPLY_MOV_SPEED:
							AddAffect (AFFECT_MOV_SPEED, POINT_MOV_SPEED, item->GetValue (2), AFF_MOV_SPEED_POTION, item->GetValue (1), 0, true);
							break;

						case APPLY_ATT_SPEED:
							AddAffect (AFFECT_ATT_SPEED, POINT_ATT_SPEED, item->GetValue (2), AFF_ATT_SPEED_POTION, item->GetValue (1), 0, true);
							break;

						case APPLY_STR:
							AddAffect (AFFECT_STR, POINT_ST, item->GetValue (2), 0, item->GetValue (1), 0, true);
							break;

						case APPLY_DEX:
							AddAffect (AFFECT_DEX, POINT_DX, item->GetValue (2), 0, item->GetValue (1), 0, true);
							break;

						case APPLY_CON:
							AddAffect (AFFECT_CON, POINT_HT, item->GetValue (2), 0, item->GetValue (1), 0, true);
							break;

						case APPLY_INT:
							AddAffect (AFFECT_INT, POINT_IQ, item->GetValue (2), 0, item->GetValue (1), 0, true);
							break;

						case APPLY_CAST_SPEED:
							AddAffect (AFFECT_CAST_SPEED, POINT_CASTING_SPEED, item->GetValue (2), 0, item->GetValue (1), 0, true);
							break;

						case APPLY_ATT_GRADE_BONUS:
							AddAffect (AFFECT_ATT_GRADE, POINT_ATT_GRADE_BONUS,
									   item->GetValue (2), 0, item->GetValue (1), 0, true);
							break;

						case APPLY_DEF_GRADE_BONUS:
							AddAffect (AFFECT_DEF_GRADE, POINT_DEF_GRADE_BONUS,
									   item->GetValue (2), 0, item->GetValue (1), 0, true);
							break;
					}
				}

				if (GetDungeon())
				{
					GetDungeon()->UsePotion (this);
				}

				if (GetWarMap())
				{
					GetWarMap()->UsePotion (this, item);
				}

				item->SetCount (item->GetCount() - 1);
				break;

				case USE_TALISMAN:
				{
					const int TOWN_PORTAL	= 1;
					const int MEMORY_PORTAL = 2;


					// gm_guild_build, oxevent 맵에서 귀환부 귀환기억부 를 사용못하게 막음
					if (GetMapIndex() == 200 || GetMapIndex() == 113)
					{
						ChatPacket (CHAT_TYPE_INFO, "[LS;388]");
						return false;
					}

					if (CArenaManager::instance().IsArenaMap (GetMapIndex()) == true)
					{
						ChatPacket (CHAT_TYPE_INFO, "[LS;1205]");
						return false;
					}

					if (m_pkWarpEvent)
					{
						ChatPacket (CHAT_TYPE_INFO, "[LS;389]");
						return false;
					}

					// CONSUME_LIFE_WHEN_USE_WARP_ITEM
					int consumeLife = CalculateConsume (this);

					if (consumeLife < 0)
					{
						return false;
					}
					// END_OF_CONSUME_LIFE_WHEN_USE_WARP_ITEM

					if (item->GetValue (0) == TOWN_PORTAL) // 귀환부
					{
						if (item->GetSocket (0) == 0)
						{
							if (!GetDungeon())
								if (!GiveRecallItem (item))
								{
									return false;
								}

							PIXEL_POSITION posWarp;

							if (SECTREE_MANAGER::instance().GetRecallPositionByEmpire (GetMapIndex(), GetEmpire(), posWarp))
							{
								// CONSUME_LIFE_WHEN_USE_WARP_ITEM
								PointChange (POINT_HP, -consumeLife, false);
								// END_OF_CONSUME_LIFE_WHEN_USE_WARP_ITEM

								WarpSet (posWarp.x, posWarp.y);
							}
							else
							{
								sys_err ("CHARACTER::UseItem : cannot find spawn position (name %s, %d x %d)", GetName(), GetX(), GetY());
							}
						}
						else
						{
							if (test_server)
							{
								ChatPacket (CHAT_TYPE_INFO, "[LS;390]");
							}

							ProcessRecallItem (item);
						}
					}
					else if (item->GetValue (0) == MEMORY_PORTAL) // 귀환기억부
					{
						if (item->GetSocket (0) == 0)
						{
							if (GetDungeon())
							{
								ChatPacket (CHAT_TYPE_INFO, "[LS;391;%s;%s]", item->GetName(), "");
								return false;
							}

							if (!GiveRecallItem (item))
							{
								return false;
							}
						}
						else
						{
							// CONSUME_LIFE_WHEN_USE_WARP_ITEM
							PointChange (POINT_HP, -consumeLife, false);
							// END_OF_CONSUME_LIFE_WHEN_USE_WARP_ITEM

							ProcessRecallItem (item);
						}
					}
				}
				break;

				case USE_TUNING:
				case USE_DETACHMENT:
				{
					LPITEM item2;

					if (!IsValidItemPosition (DestCell) || ! (item2 = GetItem (DestCell)))
					{
						return false;
					}

					if (item2->IsExchanging())
					{
						return false;
					}

					if (item2->GetVnum() >= 28330 && item2->GetVnum() <= 28343) // 영석+3
					{
						ChatPacket (CHAT_TYPE_INFO, LC_TEXT ("+3 영석은 이 아이템으로 개량할 수 없습니다"));
						return false;
					}
#ifdef ENABLE_ACCE_SYSTEM
					if (item->GetValue(0) == ACCE_CLEAN_ATTR_VALUE0)
					{
						if (!CleanAcceAttr(item, item2))
							return false;

						return true;
					}
#endif
					if (item2->GetVnum() >= 28430 && item2->GetVnum() <= 28443)  // 영석+4
					{
						if (item->GetVnum() == 71056) // 청룡의숨결
						{
							RefineItem (item, item2);
						}
						else
						{
							ChatPacket (CHAT_TYPE_INFO, LC_TEXT ("영석은 이 아이템으로 개량할 수 없습니다"));
						}
					}
					else
					{
						RefineItem (item, item2);
					}
				}
				break;

				//  ACCESSORY_REFINE & ADD/CHANGE_ATTRIBUTES
				case USE_PUT_INTO_BELT_SOCKET:
				case USE_PUT_INTO_RING_SOCKET:
				case USE_PUT_INTO_ACCESSORY_SOCKET:
				case USE_ADD_ACCESSORY_SOCKET:
				case USE_CLEAN_SOCKET:
				case USE_CHANGE_ATTRIBUTE:
				case USE_CHANGE_ATTRIBUTE2 :
				case USE_ADD_ATTRIBUTE:
				case USE_ADD_ATTRIBUTE2:
#ifdef __AURA_SYSTEM__
				case USE_PUT_INTO_AURA_SOCKET:
#endif
#if defined(__BL_MOVE_COSTUME_ATTR__)
				case USE_RESET_COSTUME_ATTR:
				case USE_CHANGE_COSTUME_ATTR:
#endif
				{
					LPITEM item2;
					if (!IsValidItemPosition (DestCell) || ! (item2 = GetItem (DestCell)))
					{
						return false;
					}

					if (item2->IsEquipped())
					{
						BuffOnAttr_RemoveBuffsFromItem (item2);
					}

					// [NOTE] 코스튬 아이템에는 아이템 최초 생성시 랜덤 속성을 부여하되, 재경재가 등등은 막아달라는 요청이 있었음.
					// 원래 ANTI_CHANGE_ATTRIBUTE 같은 아이템 Flag를 추가하여 기획 레벨에서 유연하게 컨트롤 할 수 있도록 할 예정이었으나
					// 그딴거 필요없으니 닥치고 빨리 해달래서 그냥 여기서 막음... -_-
/*					if (ITEM_COSTUME == item2->GetType() && item->GetSubType() != USE_CHANGE_ATTRIBUTE2)
#ifdef __AURA_SYSTEM__
						if (item->GetSubType() != USE_PUT_INTO_AURA_SOCKET)
#endif*/
#if defined(__BL_67_ATTR__)
// #if defined(__BL_MOVE_COSTUME_ATTR__)
					if (ITEM_COSTUME == item2->GetType() && item->GetSubType() != USE_RESET_COSTUME_ATTR && item->GetSubType() != USE_CHANGE_COSTUME_ATTR && item->GetSubType() != USE_CHANGE_ATTRIBUTE2)
#else
					if (ITEM_COSTUME == item2->GetType())
#endif
#ifdef __AURA_SYSTEM__
					if (item->GetSubType() != USE_PUT_INTO_AURA_SOCKET)
#endif
					{
						ChatPacket (CHAT_TYPE_INFO, "[LS;396]");
						return false;
					}

					if (item2->IsExchanging())
					{
						return false;
					}

					switch (item->GetSubType())
					{
						case USE_CLEAN_SOCKET:
						{
							int i;
							for (i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
							{
								if (item2->GetSocket (i) == ITEM_BROKEN_METIN_VNUM)
								{
									break;
								}
							}

							if (i == ITEM_SOCKET_MAX_NUM)
							{
								ChatPacket (CHAT_TYPE_INFO, "[LS;395]");
								return false;
							}

							int j = 0;

							for (i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
							{
								if (item2->GetSocket (i) != ITEM_BROKEN_METIN_VNUM && item2->GetSocket (i) != 0)
								{
									item2->SetSocket (j++, item2->GetSocket (i));
								}
							}

							for (; j < ITEM_SOCKET_MAX_NUM; ++j)
							{
								if (item2->GetSocket (j) > 0)
								{
									item2->SetSocket (j, 1);
								}
							}

							{
								char buf[21];
								snprintf (buf, sizeof (buf), "%u", item2->GetID());
								LogManager::instance().ItemLog (this, item, "CLEAN_SOCKET", buf);
							}

							item->SetCount (item->GetCount() - 1);

						}
						break;

#if defined(__BL_MOVE_COSTUME_ATTR__)
								case USE_RESET_COSTUME_ATTR:
								{
									if (item2->GetAttributeSetIndex() == -1)
									{
										ChatPacket(CHAT_TYPE_INFO, "[LS;396]");
										return false;
									}

									if (item2->IsEquipped())
										return false;

									if (item2->GetType() != EItemTypes::ITEM_COSTUME)
										return false;

									if (item2->GetAttributeCount() < 1) // (costume+ have at least 1 bonus)
										return false;

									item2->ClearAttribute();

									const int iRandCostume = number(1, 100);
									static BYTE arrCostumeAttrChance[] = { 100, 30, 5 };

									for (BYTE bl = 0; bl < _countof(arrCostumeAttrChance); bl++)
										if (iRandCostume <= arrCostumeAttrChance[bl])
											item2->AddAttribute();

									item->SetCount(item->GetCount() - 1);
									ChatPacket(CHAT_TYPE_INFO, "[LS;399]");
								}
								break;

								case USE_CHANGE_COSTUME_ATTR:
								{
									if (item2->GetAttributeSetIndex() == -1)
									{
										ChatPacket(CHAT_TYPE_INFO, "[LS;396]");
										return false;
									}

									if (item2->IsEquipped())
										return false;

									if (item2->GetType() != EItemTypes::ITEM_COSTUME)
										return false;

									if (item2->GetAttributeCount() < 1) // (costume+ have at least 1 bonus)
										return false;

									item2->ChangeAttribute();

									item->SetCount(item->GetCount() - 1);
									ChatPacket(CHAT_TYPE_INFO, "[LS;399]");
								}
								break;
#endif

						case USE_CHANGE_ATTRIBUTE :
							if (item2->GetAttributeSetIndex() == -1)
							{
								ChatPacket (CHAT_TYPE_INFO, "[LS;396]");
								return false;
							}

							if (item2->GetAttributeCount() == 0)
							{
								ChatPacket (CHAT_TYPE_INFO, "[LS;397]");
								return false;
							}

							if (GM_PLAYER == GetGMLevel() && false == test_server)
							{
								//
								// Event Flag 를 통해 이전에 아이템 속성 변경을 한 시간으로 부터 충분한 시간이 흘렀는지 검사하고
								// 시간이 충분히 흘렀다면 현재 속성변경에 대한 시간을 설정해 준다.
								//

								DWORD dwChangeItemAttrCycle = quest::CQuestManager::instance().GetEventFlag (msc_szChangeItemAttrCycleFlag);
								if (dwChangeItemAttrCycle < msc_dwDefaultChangeItemAttrCycle)
								{
									dwChangeItemAttrCycle = msc_dwDefaultChangeItemAttrCycle;
								}

								quest::PC* pPC = quest::CQuestManager::instance().GetPC (GetPlayerID());

								if (pPC)
								{
									DWORD dwNowMin = get_global_time() / 60;

									DWORD dwLastChangeItemAttrMin = pPC->GetFlag (msc_szLastChangeItemAttrFlag);

									if (dwLastChangeItemAttrMin + dwChangeItemAttrCycle > dwNowMin)
									{
										ChatPacket (CHAT_TYPE_INFO, "[LS;398]",
													dwChangeItemAttrCycle, dwChangeItemAttrCycle - (dwNowMin - dwLastChangeItemAttrMin));
										return false;
									}

									pPC->SetFlag (msc_szLastChangeItemAttrFlag, dwNowMin);
								}
							}

							if (item->GetSubType() == USE_CHANGE_ATTRIBUTE2)
							{
								int aiChangeProb[ITEM_ATTRIBUTE_MAX_LEVEL] =
								{
									0, 0, 30, 40, 3
								};

								item2->ChangeAttribute (aiChangeProb);
							}
							else if (item->GetVnum() == 76014)
							{
								int aiChangeProb[ITEM_ATTRIBUTE_MAX_LEVEL] =
								{
									0, 10, 50, 39, 1
								};

								item2->ChangeAttribute (aiChangeProb);
							}

							else
							{
								// 연재경 특수처리
								// 절대로 연재가 추가 안될거라 하여 하드 코딩함.
								if (item->GetVnum() == 71151 || item->GetVnum() == 76023)
								{
									if ((item2->GetType() == ITEM_WEAPON)
											|| (item2->GetType() == ITEM_ARMOR && item2->GetSubType() == ARMOR_BODY))
									{
										bool bCanUse = true;
										for (int i = 0; i < ITEM_LIMIT_MAX_NUM; ++i)
										{
											if (item2->GetLimitType (i) == LIMIT_LEVEL && item2->GetLimitValue (i) > 40)
											{
												bCanUse = false;
												break;
											}
										}
										if (false == bCanUse)
										{
											ChatPacket (CHAT_TYPE_INFO, "[LS;1064]");
											break;
										}
									}
									else
									{
										ChatPacket (CHAT_TYPE_INFO, "[LS;1065]");
										break;
									}
								}
								item2->ChangeAttribute();
							}

							ChatPacket (CHAT_TYPE_INFO, "[LS;399]");
							{
								char buf[21];
								snprintf (buf, sizeof (buf), "%u", item2->GetID());
								LogManager::instance().ItemLog (this, item, "CHANGE_ATTRIBUTE", buf);
							}

							item->SetCount (item->GetCount() - 1);
							break;

#if defined(__BL_67_ATTR__)
								case USE_CHANGE_ATTRIBUTE2:
									if (item2->GetAttributeSetIndex() == -1 || item2->GetRareAttrCount() == 0)
									{
										ChatPacket (CHAT_TYPE_INFO, "[LS;2048]");
										return false;
									}

									if (item2->IsEquipped())
										return false;

									if ((item->GetVnum() == SMALL_ORISON && number(1, 100) >= 10) == false 
										&& item2->ChangeRareAttribute() == true)
										ChatPacket (CHAT_TYPE_INFO, "[LS;2048]");
									else
										ChatPacket (CHAT_TYPE_INFO, "[LS;2051]");

									item->SetCount(item->GetCount() - 1);
									break;
#endif

						case USE_ADD_ATTRIBUTE :
							if (item2->GetAttributeSetIndex() == -1)
							{
								ChatPacket (CHAT_TYPE_INFO, "[LS;396]");
								return false;
							}

							if (item2->GetAttributeCount() < 4)
							{
								// 연재가 특수처리
								// 절대로 연재가 추가 안될거라 하여 하드 코딩함.
								if (item->GetVnum() == 71152 || item->GetVnum() == 76024)
								{
									if ((item2->GetType() == ITEM_WEAPON)
											|| (item2->GetType() == ITEM_ARMOR && item2->GetSubType() == ARMOR_BODY))
									{
										bool bCanUse = true;
										for (int i = 0; i < ITEM_LIMIT_MAX_NUM; ++i)
										{
											if (item2->GetLimitType (i) == LIMIT_LEVEL && item2->GetLimitValue (i) > 40)
											{
												bCanUse = false;
												break;
											}
										}
										if (false == bCanUse)
										{
											ChatPacket (CHAT_TYPE_INFO, "[LS;1064]");
											break;
										}
									}
									else
									{
										ChatPacket (CHAT_TYPE_INFO, "[LS;1065]");
										break;
									}
								}
								char buf[21];
								snprintf (buf, sizeof (buf), "%u", item2->GetID());

								if (number (1, 100) <= aiItemAttributeAddPercent[item2->GetAttributeCount()])
								{
									item2->AddAttribute();
									ChatPacket (CHAT_TYPE_INFO, "[LS;400]");

									int iAddedIdx = item2->GetAttributeCount() - 1;
									LogManager::instance().ItemLog (
										GetPlayerID(),
										item2->GetAttributeType (iAddedIdx),
										item2->GetAttributeValue (iAddedIdx),
										item->GetID(),
										"ADD_ATTRIBUTE_SUCCESS",
										buf,
										GetDesc()->GetHostName(),
										item->GetOriginalVnum());
								}
								else
								{
									ChatPacket (CHAT_TYPE_INFO, "[LS;401]");
									LogManager::instance().ItemLog (this, item, "ADD_ATTRIBUTE_FAIL", buf);
								}

								item->SetCount (item->GetCount() - 1);
							}
							else
							{
								ChatPacket (CHAT_TYPE_INFO, "[LS;402]");
							}
							break;

						case USE_ADD_ATTRIBUTE2 :
							// 축복의 구슬
							// 재가비서를 통해 속성을 4개 추가 시킨 아이템에 대해서 하나의 속성을 더 붙여준다.
							if (item2->GetAttributeSetIndex() == -1)
							{
								ChatPacket (CHAT_TYPE_INFO, "[LS;396]");
								return false;
							}

							// 속성이 이미 4개 추가 되었을 때만 속성을 추가 가능하다.
							if (item2->GetAttributeCount() == 4)
							{
								char buf[21];
								snprintf (buf, sizeof (buf), "%u", item2->GetID());

								if (number (1, 100) <= aiItemAttributeAddPercent[item2->GetAttributeCount()])
								{
									item2->AddAttribute();
									ChatPacket (CHAT_TYPE_INFO, "[LS;400]");

									int iAddedIdx = item2->GetAttributeCount() - 1;
									LogManager::instance().ItemLog (
										GetPlayerID(),
										item2->GetAttributeType (iAddedIdx),
										item2->GetAttributeValue (iAddedIdx),
										item->GetID(),
										"ADD_ATTRIBUTE2_SUCCESS",
										buf,
										GetDesc()->GetHostName(),
										item->GetOriginalVnum());
								}
								else
								{
									ChatPacket (CHAT_TYPE_INFO, "[LS;401]");
									LogManager::instance().ItemLog (this, item, "ADD_ATTRIBUTE2_FAIL", buf);
								}

								item->SetCount (item->GetCount() - 1);
							}
							else if (item2->GetAttributeCount() == 5)
							{
								ChatPacket (CHAT_TYPE_INFO, "[LS;404]");
							}
							else if (item2->GetAttributeCount() < 4)
							{
								ChatPacket (CHAT_TYPE_INFO, "[LS;405]");
							}
							else
							{
								// wtf ?!
								sys_err ("ADD_ATTRIBUTE2 : Item has wrong AttributeCount(%d)", item2->GetAttributeCount());
							}
							break;

						case USE_ADD_ACCESSORY_SOCKET:
						{
							char buf[21];
							snprintf (buf, sizeof (buf), "%u", item2->GetID());

							if (item2->IsAccessoryForSocket())
							{
								if (item2->GetAccessorySocketMaxGrade() < ITEM_ACCESSORY_SOCKET_MAX_NUM)
								{
									if (number (1, 100) <= 50)
									{
										item2->SetAccessorySocketMaxGrade (item2->GetAccessorySocketMaxGrade() + 1);
										ChatPacket (CHAT_TYPE_INFO, "[LS;406]");
										LogManager::instance().ItemLog (this, item, "ADD_SOCKET_SUCCESS", buf);
									}
									else
									{
										ChatPacket (CHAT_TYPE_INFO, "[LS;407]");
										LogManager::instance().ItemLog (this, item, "ADD_SOCKET_FAIL", buf);
									}

									item->SetCount (item->GetCount() - 1);
								}
								else
								{
									ChatPacket (CHAT_TYPE_INFO, "[LS;408]");
								}
							}
							else
							{
								ChatPacket (CHAT_TYPE_INFO, "[LS;409]");
							}
						}
						break;

						case USE_PUT_INTO_BELT_SOCKET:
						case USE_PUT_INTO_ACCESSORY_SOCKET:
							if (item2->IsAccessoryForSocket() && item->CanPutInto (item2))
							{
								char buf[21];
								snprintf (buf, sizeof (buf), "%u", item2->GetID());

								if (item2->GetAccessorySocketGrade() < item2->GetAccessorySocketMaxGrade())
								{
									if (number (1, 100) <= aiAccessorySocketPutPct[item2->GetAccessorySocketGrade()])
									{
										item2->SetAccessorySocketGrade (item2->GetAccessorySocketGrade() + 1);
										ChatPacket (CHAT_TYPE_INFO, "[LS;410]");
										LogManager::instance().ItemLog (this, item, "PUT_SOCKET_SUCCESS", buf);
									}
									else
									{
										ChatPacket (CHAT_TYPE_INFO, "[LS;411]");
										LogManager::instance().ItemLog (this, item, "PUT_SOCKET_FAIL", buf);
									}

									item->SetCount (item->GetCount() - 1);
								}
								else
								{
									if (item2->GetAccessorySocketMaxGrade() == 0)
									{
										ChatPacket (CHAT_TYPE_INFO, "[LS;412]");
									}
									else if (item2->GetAccessorySocketMaxGrade() < ITEM_ACCESSORY_SOCKET_MAX_NUM)
									{
										ChatPacket (CHAT_TYPE_INFO, "[LS;413]");
										ChatPacket (CHAT_TYPE_INFO, "[LS;415]");
									}
									else
									{
										ChatPacket (CHAT_TYPE_INFO, "[LS;416]");
									}
								}
							}
							else
							{
								ChatPacket (CHAT_TYPE_INFO, "[LS;417]");
							}
							break;
#ifdef __AURA_SYSTEM__
			case USE_PUT_INTO_AURA_SOCKET:
			{
				if (item2->IsAuraBoosterForSocket() && item->CanPutInto(item2))
				{
					char buf[21];
					snprintf(buf, sizeof(buf), "%d", item2->GetID());

					const BYTE c_bAuraBoostIndex = item->GetOriginalVnum() - ITEM_AURA_BOOST_ITEM_VNUM_BASE;
					item2->SetSocket(ITEM_SOCKET_AURA_BOOST, c_bAuraBoostIndex * 100000000 + item->GetValue(ITEM_AURA_BOOST_TIME_VALUE));

					ChatPacket (CHAT_TYPE_INFO, "[LS;2009]");

					LogManager::instance().ItemLog(this, item, "PUT_AURA_SOCKET", buf);

					if (IS_SET(item->GetFlag(), ITEM_FLAG_STACKABLE) && !IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_STACK) && item->GetCount() > 1)
						item->SetCount(item->GetCount() - 1);
					else
						ITEM_MANAGER::instance().RemoveItem(item, "PUT_AURA_SOCKET_REMOVE");
				}
				else
					ChatPacket (CHAT_TYPE_INFO, "[LS;2010]");
			}
			break;
#endif
					}
					if (item2->IsEquipped())
					{
						BuffOnAttr_AddBuffsFromItem (item2);
					}
				}
				break;
				//  END_OF_ACCESSORY_REFINE & END_OF_ADD_ATTRIBUTES & END_OF_CHANGE_ATTRIBUTES

				case USE_BAIT:
				{

					if (m_pkFishingEvent)
					{
						ChatPacket (CHAT_TYPE_INFO, "[LS;418]");
						return false;
					}

#ifdef ENABLE_ADDITIONAL_EQUIPMENT_PAGE
						LPITEM weapon = GetEquipWear(WEAR_WEAPON);
#else
						LPITEM weapon = GetWear(WEAR_WEAPON);
#endif

					if (!weapon || weapon->GetType() != ITEM_ROD)
					{
						return false;
					}

					if (weapon->GetSocket (2))
					{
						ChatPacket (CHAT_TYPE_INFO, "[LS;419;%s]", item->GetName());
					}
					else
					{
						ChatPacket (CHAT_TYPE_INFO, "[LS;420;%s]", item->GetName());
					}

					weapon->SetSocket (2, item->GetValue (0));
					item->SetCount (item->GetCount() - 1);
				}
				break;

				case USE_MOVE:
				case USE_TREASURE_BOX:
				case USE_MONEYBAG:
					break;

				case USE_AFFECT :
				{
#if defined(__BL_OFFICIAL_LOOT_FILTER__) && defined(__PREMIUM_LOOT_FILTER__)
							if (item->GetValue(0) == AFFECT_LOOTING_SYSTEM)
							{
								CAffect* pkAff = FindAffect(AFFECT_LOOTING_SYSTEM);
								long lDur = item->GetValue(3);

								if (pkAff)
								{
									if (lDur > 0)
									{
										if (pkAff->lDuration > LONG_MAX - lDur)
										{
											ChatPacket (CHAT_TYPE_INFO, "[LS;2037]");
											return false;
										}
									}
									else
									{
										sys_err("AFFECT_LOOTING_SYSTEM, lDur not a positive value.");
										return false;
									}

									lDur += pkAff->lDuration;

									RemoveAffect(pkAff);
									ChatPacket (CHAT_TYPE_INFO, "[LS;2038]");
								}
								else
								{
									ChatPacket (CHAT_TYPE_INFO, "[LS;2039]");
								}

								AddAffect(AFFECT_LOOTING_SYSTEM, POINT_NONE, 0, AFF_NONE, lDur, 0, false);
								item->SetCount(item->GetCount() - 1);
								return true;
							}
#endif
					if (FindAffect (item->GetValue (0), aApplyInfo[item->GetValue (1)].bPointType))
					{
						ChatPacket (CHAT_TYPE_INFO, "[LS;99]");
					}
					else
					{
						AddAffect (item->GetValue (0), aApplyInfo[item->GetValue (1)].bPointType, item->GetValue (2), 0, item->GetValue (3), 0, false);
						item->SetCount (item->GetCount() - 1);
					}
				}
				break;

				case USE_CREATE_STONE:
					AutoGiveItem (number (28000, 28013));
					item->SetCount (item->GetCount() - 1);
					break;

				// 물약 제조 스킬용 레시피 처리
				case USE_RECIPE :
				{
					LPITEM pSource1 = FindSpecifyItem (item->GetValue (1));
					DWORD dwSourceCount1 = item->GetValue (2);

					LPITEM pSource2 = FindSpecifyItem (item->GetValue (3));
					DWORD dwSourceCount2 = item->GetValue (4);

					if (dwSourceCount1 != 0)
					{
						if (pSource1 == NULL)
						{
							ChatPacket (CHAT_TYPE_INFO, "[LS;421]");
							return false;
						}
					}

					if (dwSourceCount2 != 0)
					{
						if (pSource2 == NULL)
						{
							ChatPacket (CHAT_TYPE_INFO, "[LS;421]");
							return false;
						}
					}

					if (pSource1 != NULL)
					{
						if (pSource1->GetCount() < dwSourceCount1)
						{
							ChatPacket (CHAT_TYPE_INFO, "[LS;422;%s]", pSource1->GetName());
							return false;
						}

						pSource1->SetCount (pSource1->GetCount() - dwSourceCount1);
					}

					if (pSource2 != NULL)
					{
						if (pSource2->GetCount() < dwSourceCount2)
						{
							ChatPacket (CHAT_TYPE_INFO, "[LS;422;%s]", pSource2->GetName());
							return false;
						}

						pSource2->SetCount (pSource2->GetCount() - dwSourceCount2);
					}

					LPITEM pBottle = FindSpecifyItem (50901);

					if (!pBottle || pBottle->GetCount() < 1)
					{
						ChatPacket (CHAT_TYPE_INFO, "[LS;423]");
						return false;
					}

					pBottle->SetCount (pBottle->GetCount() - 1);

					if (number (1, 100) > item->GetValue (5))
					{
						ChatPacket (CHAT_TYPE_INFO, "[LS;424]");
						return false;
					}

					AutoGiveItem (item->GetValue (0));
				}
				break;
			}
		}
		break;

		case ITEM_METIN:
		{
			LPITEM item2;

			if (!IsValidItemPosition (DestCell) || ! (item2 = GetItem (DestCell)))
			{
				return false;
			}

			if (item2->IsExchanging())
			{
				return false;
			}

			if (item2->GetType() == ITEM_PICK)
			{
				return false;
			}
			if (item2->GetType() == ITEM_ROD)
			{
				return false;
			}

			int i;

			for (i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
			{
				DWORD dwVnum;

				if ((dwVnum = item2->GetSocket (i)) <= 2)
				{
					continue;
				}

				TItemTable* p = ITEM_MANAGER::instance().GetTable (dwVnum);

				if (!p)
				{
					continue;
				}

				if (item->GetValue (5) == p->alValues[5])
				{
					ChatPacket (CHAT_TYPE_INFO, "[LS;426]");
					return false;
				}
			}

			if (item2->GetType() == ITEM_ARMOR)
			{
				if (!IS_SET (item->GetWearFlag(), WEARABLE_BODY) || !IS_SET (item2->GetWearFlag(), WEARABLE_BODY))
				{
					ChatPacket (CHAT_TYPE_INFO, "[LS;427]");
					return false;
				}
			}
			else if (item2->GetType() == ITEM_WEAPON)
			{
				if (!IS_SET (item->GetWearFlag(), WEARABLE_WEAPON))
				{
					ChatPacket (CHAT_TYPE_INFO, "[LS;428]");
					return false;
				}
			}
			else
			{
				ChatPacket (CHAT_TYPE_INFO, "[LS;431]");
				return false;
			}

			for (i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
				if (item2->GetSocket (i) >= 1 && item2->GetSocket (i) <= 2 && item2->GetSocket (i) >= item->GetValue (2))
				{
					// 석 확률
					if (number (1, 100) <= 30)
					{
						ChatPacket (CHAT_TYPE_INFO, "[LS;429]");
						item2->SetSocket (i, item->GetVnum());
					}
					else
					{
						ChatPacket (CHAT_TYPE_INFO, "[LS;430]");
						item2->SetSocket (i, ITEM_BROKEN_METIN_VNUM);
					}

					LogManager::instance().ItemLog (this, item2, "SOCKET", item->GetName());
					ITEM_MANAGER::instance().RemoveItem (item, "REMOVE (METIN)");
					break;
				}

			if (i == ITEM_SOCKET_MAX_NUM)
			{
				ChatPacket (CHAT_TYPE_INFO, "[LS;431]");
			}
		}
		break;

		case ITEM_AUTOUSE:
		case ITEM_MATERIAL:
		case ITEM_SPECIAL:
		case ITEM_TOOL:
		case ITEM_LOTTERY:
			break;

		case ITEM_TOTEM:
		{
			if (!item->IsEquipped())
			{
				EquipItem (item);
			}
		}
		break;

		case ITEM_BLEND:
			// 새로운 약초들
			sys_log (0, "ITEM_BLEND!!");
			if (Blend_Item_find (item->GetVnum()))
			{
				int		affect_type		= AFFECT_BLEND;
				if (item->GetSocket (0) >= _countof (aApplyInfo))
				{
					sys_err ("INVALID BLEND ITEM(id : %d, vnum : %d). APPLY TYPE IS %d.", item->GetID(), item->GetVnum(), item->GetSocket (0));
					return false;
				}
				int		apply_type		= aApplyInfo[item->GetSocket (0)].bPointType;
				int		apply_value		= item->GetSocket (1);
				int		apply_duration	= item->GetSocket (2);

				if (FindAffect (affect_type, apply_type))
				{
					ChatPacket (CHAT_TYPE_INFO, "[LS;99]");
				}
				else
				{
					if (FindAffect (AFFECT_EXP_BONUS_EURO_FREE, POINT_RESIST_MAGIC))
					{
						ChatPacket (CHAT_TYPE_INFO, "[LS;99]");
					}
					else
					{
						AddAffect (affect_type, apply_type, apply_value, 0, apply_duration, 0, false);
						item->SetCount (item->GetCount() - 1);
					}
				}
			}
			break;
		case ITEM_EXTRACT:
		{
			LPITEM pDestItem = GetItem (DestCell);
			if (NULL == pDestItem)
			{
				return false;
			}
			switch (item->GetSubType())
			{
				case EXTRACT_DRAGON_SOUL:
					if (pDestItem->IsDragonSoul())
					{
						return DSManager::instance().PullOut (this, NPOS, pDestItem, item);
					}
					return false;
				case EXTRACT_DRAGON_HEART:
					if (pDestItem->IsDragonSoul())
					{
						return DSManager::instance().ExtractDragonHeart (this, pDestItem, item);
					}
					return false;
				default:
					return false;
			}
		}
		break;

#if defined(ENABLE_PROTO_RENEWAL)
		case ITEM_PET:
		{
			switch (item->GetSubType())
			{
				case PET_PAY:
				{
					PetSummon(item);
				}
				break;

				default:
					quest::CQuestManager::Instance().UseItem(GetPlayerID(), item, false);
					break;
			}
		}
		break;
#endif

		case ITEM_NONE:
			sys_err ("Item type NONE %s", item->GetName());
			break;

		default:
			sys_log (0, "UseItemEx: Unknown type %s %d", item->GetName(), item->GetType());
			return false;
	}

	return true;
}

int g_nPortalLimitTime = 10;

bool CHARACTER::UseItem (TItemPos Cell, TItemPos DestCell)
{
	WORD wCell = Cell.cell;
	BYTE window_type = Cell.window_type;
	WORD wDestCell = DestCell.cell;
	BYTE bDestInven = DestCell.window_type;
	LPITEM item;

	if (!CanHandleItem())
	{
		return false;
	}

	if (!IsValidItemPosition (Cell) || ! (item = GetItem (Cell)))
	{
		return false;
	}

	sys_log (0, "%s: USE_ITEM %s (inven %d, cell: %d)", GetName(), item->GetName(), window_type, wCell);

	if (item->IsExchanging())
	{
		return false;
	}

	if (!item->CanUsedBy (this))
	{
		ChatPacket (CHAT_TYPE_INFO, "[LS;1004]");
		return false;
	}

	if (IsStun())
	{
		return false;
	}

	if (false == FN_check_item_sex (this, item))
	{
		ChatPacket (CHAT_TYPE_INFO, "[LS;1005]");
		return false;
	}
	
#if defined(__BL_TRANSMUTATION__) && defined(__COSTUME_SYSTEM__)
	DWORD dwTransmutationVnum = item->GetTransmutationVnum();
	if (dwTransmutationVnum != 0)
	{
		TItemTable* pItemTable = ITEM_MANAGER::instance().GetTable(dwTransmutationVnum);
		if (pItemTable && (pItemTable->GetType() == ITEM_COSTUME))
		{
			if ((IS_SET(pItemTable->GetAntiFlags(), ITEM_ANTIFLAG_MALE) && SEX_MALE == GET_SEX(this)) ||
				(IS_SET(pItemTable->GetAntiFlags(), ITEM_ANTIFLAG_FEMALE) && SEX_FEMALE == GET_SEX(this)))
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Transmutation] You cannot equip the transmuted item as it does not match your gender."));
				return false;
			}
		}
	}
#endif

	//PREVENT_TRADE_WINDOW
	if (IS_SUMMON_ITEM (item->GetVnum()))
	{
		// 경혼반지 사용지 상대방이 SUMMONABLE_ZONE에 있는가는 WarpToPC()에서 체크
		if (false == IS_SUMMONABLE_ZONE (GetMapIndex()))
		{
			ChatPacket (CHAT_TYPE_INFO, "[LS;432]");
			return false;
		}

		int iPulse = thecore_pulse();

		//창고 연후 체크
		if (iPulse - GetSafeboxLoadTime() < PASSES_PER_SEC (g_nPortalLimitTime))
		{
			ChatPacket (CHAT_TYPE_INFO, "[LS;434;%d]", g_nPortalLimitTime);

			if (test_server)
			{
				ChatPacket (CHAT_TYPE_INFO, "[TestOnly]Pulse %d LoadTime %d PASS %d", iPulse, GetSafeboxLoadTime(), PASSES_PER_SEC (g_nPortalLimitTime));
			}
			return false;
		}

		//거래관련 창 체크
		if (GetExchange() || GetMyShop() || GetShopOwner() || IsOpenSafebox() || IsCubeOpen()
#ifdef __AURA_SYSTEM__
			|| IsAuraRefineWindowOpen()
#endif
#if defined(__BL_67_ATTR__)
			|| Is67AttrOpen()
#endif
#if defined(__BL_MOVE_COSTUME_ATTR__)
			|| IsItemComb()
#endif
#if defined(__BL_TRANSMUTATION__)
			|| GetTransmutation()
#endif
			)
		{
			ChatPacket (CHAT_TYPE_INFO, "[LS;435]");
			return false;
		}

		//PREVENT_REFINE_HACK
		//개량후 시간체크
		{
			if (iPulse - GetRefineTime() < PASSES_PER_SEC (g_nPortalLimitTime))
			{
				ChatPacket (CHAT_TYPE_INFO, "[LS;437;%d]", g_nPortalLimitTime);
				return false;
			}
		}
		//END_PREVENT_REFINE_HACK

#ifdef ENABLE_ACCE_SYSTEM
		if (isAcceOpened(true) || isAcceOpened(false))
		{
			ChatPacket (CHAT_TYPE_INFO, "[LS;2053]");
			return false;
		}
#endif

		//PREVENT_ITEM_COPY
		{
			if (iPulse - GetMyShopTime() < PASSES_PER_SEC (g_nPortalLimitTime))
			{
				ChatPacket (CHAT_TYPE_INFO, "[LS;438;%d]", g_nPortalLimitTime);
				return false;
			}

		}
		//END_PREVENT_ITEM_COPY


		//귀환부 거리체크
		if (item->GetVnum() != 70302)
		{
			PIXEL_POSITION posWarp;

			int x = 0;
			int y = 0;

			double nDist = 0;
			const double nDistant = 5000.0;
			//귀환기억부
			if (item->GetVnum() == 22010)
			{
				x = item->GetSocket (0) - GetX();
				y = item->GetSocket (1) - GetY();
			}
			//귀환부
			else if (item->GetVnum() == 22000)
			{
				SECTREE_MANAGER::instance().GetRecallPositionByEmpire (GetMapIndex(), GetEmpire(), posWarp);

				if (item->GetSocket (0) == 0)
				{
					x = posWarp.x - GetX();
					y = posWarp.y - GetY();
				}
				else
				{
					x = item->GetSocket (0) - GetX();
					y = item->GetSocket (1) - GetY();
				}
			}

			nDist = sqrt (pow ((float)x, 2) + pow ((float)y, 2));

			if (nDistant > nDist)
			{
				ChatPacket (CHAT_TYPE_INFO, "[LS;439]");
				if (test_server)
				{
					ChatPacket (CHAT_TYPE_INFO, "PossibleDistant %f nNowDist %f", nDistant, nDist);
				}
				return false;
			}
		}

		//PREVENT_PORTAL_AFTER_EXCHANGE
		//교환 후 시간체크
		if (iPulse - GetExchangeTime()  < PASSES_PER_SEC (g_nPortalLimitTime))
		{
			ChatPacket (CHAT_TYPE_INFO, "[LS;440;%d]", g_nPortalLimitTime);
			return false;
		}
		//END_PREVENT_PORTAL_AFTER_EXCHANGE

	}

	//보따리 비단 사용시 거래창 제한 체크
	if (item->GetVnum() == 50200 | item->GetVnum() == 71049)
	{
		if (GetExchange() || GetMyShop() || GetShopOwner() || IsOpenSafebox() || IsCubeOpen()
#ifdef __AURA_SYSTEM__
			|| IsAuraRefineWindowOpen()
#endif
#if defined(__BL_67_ATTR__)
			|| Is67AttrOpen()
#endif
#if defined(__BL_MOVE_COSTUME_ATTR__)
			|| IsItemComb()
#endif
#if defined(__BL_TRANSMUTATION__)
			|| GetTransmutation()
#endif
			)
		{
			ChatPacket (CHAT_TYPE_INFO, "[LS;441]");
			return false;
		}

	}
	//END_PREVENT_TRADE_WINDOW

	if (IS_SET (item->GetFlag(), ITEM_FLAG_LOG)) // 사용 로그를 남기는 아이템 처리
	{
		DWORD vid = item->GetVID();
		DWORD oldCount = item->GetCount();
		DWORD vnum = item->GetVnum();

		char hint[ITEM_NAME_MAX_LEN + 32 + 1];
		int len = snprintf (hint, sizeof (hint) - 32, "%s", item->GetName());

		if (len < 0 || len >= (int) sizeof (hint) - 32)
		{
			len = (sizeof (hint) - 32) - 1;
		}

		bool ret = UseItemEx (item, DestCell);

		if (NULL == ITEM_MANAGER::instance().FindByVID (vid)) // UseItemEx에서 아이템이 삭제 되었다. 삭제 로그를 남김
		{
			LogManager::instance().ItemLog (this, vid, vnum, "REMOVE", hint);
		}
		else if (oldCount != item->GetCount())
		{
			snprintf (hint + len, sizeof (hint) - len, " %u", oldCount - 1);
			LogManager::instance().ItemLog (this, vid, vnum, "USE_ITEM", hint);
		}
		return (ret);
	}
	else
	{
		return UseItemEx (item, DestCell);
	}
}

bool CHARACTER::DropItem (TItemPos Cell, BYTE bCount)
{
	LPITEM item = NULL;

	if (!CanHandleItem())
	{
		if (NULL != DragonSoul_RefineWindow_GetOpener())
		{
			ChatPacket (CHAT_TYPE_INFO, "[LS;1069]");
		}
		return false;
	}

	if (IsDead())
	{
		return false;
	}

	if (!IsValidItemPosition (Cell) || ! (item = GetItem (Cell)))
	{
		return false;
	}

	if (item->IsExchanging())
	{
		return false;
	}

	if (true == item->isLocked())
	{
		return false;
	}

	if (quest::CQuestManager::instance().GetPCForce (GetPlayerID())->IsRunning() == true)
	{
		return false;
	}

	if (IS_SET (item->GetAntiFlag(), ITEM_ANTIFLAG_DROP | ITEM_ANTIFLAG_GIVE))
	{
		ChatPacket (CHAT_TYPE_INFO, "[LS;442]");
		return false;
	}

	if (bCount == 0 || bCount > item->GetCount())
	{
		bCount = item->GetCount();
	}

	SyncQuickslot (QUICKSLOT_TYPE_ITEM, Cell.cell, 255);	// Quickslot 에서 지움

	LPITEM pkItemToDrop;

	if (bCount == item->GetCount())
	{
		item->RemoveFromCharacter();
		pkItemToDrop = item;
	}
	else
	{
		if (bCount == 0)
		{
			if (test_server)
			{
				sys_log (0, "[DROP_ITEM] drop item count == 0");
			}
			return false;
		}

		item->SetCount (item->GetCount() - bCount);
		ITEM_MANAGER::instance().FlushDelayedSave (item);

		pkItemToDrop = ITEM_MANAGER::instance().CreateItem (item->GetVnum(), bCount);

		// copy item socket -- by mhh
		FN_copy_item_socket (pkItemToDrop, item);

		char szBuf[51 + 1];
		snprintf (szBuf, sizeof (szBuf), "%u %u", pkItemToDrop->GetID(), pkItemToDrop->GetCount());
		LogManager::instance().ItemLog (this, item, "ITEM_SPLIT", szBuf);
	}

	PIXEL_POSITION pxPos = GetXYZ();

	if (pkItemToDrop->AddToGround (GetMapIndex(), pxPos))
	{
		item->AttrLog();

		ChatPacket (CHAT_TYPE_INFO, "[LS;443]");
		pkItemToDrop->StartDestroyEvent();

		ITEM_MANAGER::instance().FlushDelayedSave (pkItemToDrop);

		char szHint[32 + 1];
		snprintf (szHint, sizeof (szHint), "%s %u %u", pkItemToDrop->GetName(), pkItemToDrop->GetCount(), pkItemToDrop->GetOriginalVnum());
		LogManager::instance().ItemLog (this, pkItemToDrop, "DROP", szHint);
		//Motion(MOTION_PICKUP);
	}

	return true;
}

#ifdef WJ_NEW_DROP_DIALOG
bool CHARACTER::DestroyItem(TItemPos Cell)
{
	LPITEM item = NULL;
	
	if (!CanHandleItem())
	{
		if (NULL != DragonSoul_RefineWindow_GetOpener())
			ChatPacket (CHAT_TYPE_INFO, "[LS;2029]");
		return false;
	}
	
	if (IsDead())
		return false;
	
	if (!IsValidItemPosition(Cell) || !(item = GetItem(Cell)))
		return false;
	
	if (item->IsExchanging())
		return false;
	
	if (true == item->isLocked())
		return false;

	if (quest::CQuestManager::instance().GetPCForce(GetPlayerID())->IsRunning() == true)
		return false;
	
	if (item->GetCount() <= 0)
		return false;

	SyncQuickslot(QUICKSLOT_TYPE_ITEM, Cell.cell, 1000);
	ITEM_MANAGER::instance().RemoveItem(item);
	ChatPacket (CHAT_TYPE_INFO, "[LS;2030]"), item->GetName();
	
	return true;
}

bool CHARACTER::SellItem(TItemPos Cell, BYTE bCount)
{
	LPITEM item = NULL;
	
	if (!CanHandleItem())
	{
		if (NULL != DragonSoul_RefineWindow_GetOpener())
			ChatPacket (CHAT_TYPE_INFO, "[LS;2029]");
		return false;
	}
	
	if (IsDead())
		return false;
	
	if (!IsValidItemPosition(Cell) || !(item = GetItem(Cell)))
		return false;
	
	if (item->IsExchanging())
		return false;
	
	if (true == item->isLocked())
		return false;

	if (IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_SELL))
	{
		ChatPacket (CHAT_TYPE_INFO, "[LS;2031]");
		return false;
	}
	
	if (quest::CQuestManager::instance().GetPCForce(GetPlayerID())->IsRunning() == true)
		return false;
	
	if (item->GetCount() <= 0)
		return false;

	DWORD dwPrice;
	
	if (bCount == 0 || bCount > item->GetCount())
		bCount = item->GetCount();
	
	dwPrice = item->GetShopBuyPrice();
	
	if (IS_SET(item->GetFlag(), ITEM_FLAG_COUNT_PER_1GOLD))
	{
		if (dwPrice == 0)
			dwPrice = bCount;
		else
			dwPrice = bCount / dwPrice;
	}
	else
		dwPrice *= bCount;
	
	dwPrice /= 5;
	
	DWORD dwTax = 0;

	dwPrice -= dwTax;

	const int64_t nTotalMoney = static_cast<int64_t>(GetGold()) + static_cast<int64_t>(dwPrice);
	if (GOLD_MAX <= nTotalMoney)
	{
		sys_err("[OVERFLOW_GOLD] id %u name %s gold %u", GetPlayerID(), GetName(), GetGold());
		ChatPacket (CHAT_TYPE_INFO, "[LS;2032]");
		
		return false;
	}
	
	sys_log(0, "SHOP: SELL: %s item name: %s(x%d):%u price: %u", GetName(), item->GetName(), bCount, item->GetID(), dwPrice);
	
	DBManager::instance().SendMoneyLog(MONEY_LOG_SHOP, item->GetVnum(), dwPrice);
	item->SetCount(item->GetCount() - bCount);
	PointChange(POINT_GOLD, dwPrice, false);
	ChatPacket (CHAT_TYPE_INFO, "[LS;2033]"), item->GetName();
	
	return true;
}
#endif

bool CHARACTER::DropGold (int gold)
{
	if (gold <= 0 || gold > GetGold())
	{
		return false;
	}

	if (!CanHandleItem())
	{
		return false;
	}

	if (0 != g_GoldDropTimeLimitValue)
	{
		if (get_dword_time() < m_dwLastGoldDropTime+g_GoldDropTimeLimitValue)
		{
			ChatPacket (CHAT_TYPE_INFO, "[LS;1062]");
			return false;
		}
	}

	m_dwLastGoldDropTime = get_dword_time();

	LPITEM item = ITEM_MANAGER::instance().CreateItem (1, gold);

	if (item)
	{
		PIXEL_POSITION pos = GetXYZ();

		if (item->AddToGround (GetMapIndex(), pos))
		{
			//Motion(MOTION_PICKUP);
			PointChange (POINT_GOLD, -gold, true);

			if (gold > 1000) // 천원 이상만 기록한다.
			{
				LogManager::instance().CharLog (this, gold, "DROP_GOLD", "");
			}

			item->StartDestroyEvent (150);
			ChatPacket (CHAT_TYPE_INFO, "[LS;1057;%d]", 150/60);
		}

		Save();
		return true;
	}

	return false;
}

bool CHARACTER::MoveItem (TItemPos Cell, TItemPos DestCell, BYTE count)
{
	LPITEM item = NULL;

	if (!IsValidItemPosition (Cell))
	{
		return false;
	}

	if (! (item = GetItem (Cell)))
	{
		return false;
	}

	if (item->IsExchanging())
	{
		return false;
	}

	if (item->GetCount() < count)
	{
		return false;
	}

	if (INVENTORY == Cell.window_type && Cell.cell >= INVENTORY_MAX_NUM && IS_SET (item->GetFlag(), ITEM_FLAG_IRREMOVABLE))
	{
		return false;
	}

	if (true == item->isLocked())
	{
		return false;
	}

	if (!IsValidItemPosition (DestCell))
	{
		return false;
	}

	if (!CanHandleItem())
	{
		if (NULL != DragonSoul_RefineWindow_GetOpener())
		{
			ChatPacket (CHAT_TYPE_INFO, "[LS;1069]");
		}
#ifdef __AURA_SYSTEM__
		if (IsAuraRefineWindowOpen())
			ChatPacket (CHAT_TYPE_INFO, "[LS;2011]");
#endif
		return false;
	}

	// 기획자의 요청으로 벨트 인벤토리에는 특정 타입의 아이템만 넣을 수 있다.
	if (DestCell.IsBeltInventoryPosition() && false == CBeltInventoryHelper::CanMoveIntoBeltInventory (item))
	{
		ChatPacket (CHAT_TYPE_INFO, "[LS;1097]");
		return false;
	}

	// 이미 착용중인 아이템을 다른 곳으로 옮기는 경우, '장책 해제' 가능한 지 확인하고 옮김
	if (Cell.IsEquipPosition())
	{
		if (!CanUnequipNow(item))
			return false;

#ifdef __WEAPON_COSTUME_SYSTEM__
		int iWearCell = item->FindEquipCell(this);
		if (iWearCell == WEAR_WEAPON || iWearCell == WEAR_SECOND_WEAPON)
		{
			LPITEM costumeWeapon = GetWear(WEAR_COSTUME_WEAPON);
			if (costumeWeapon && !UnequipItem(costumeWeapon))
			{
				ChatPacket (CHAT_TYPE_INFO, "[LS;1965]");
				return false;
			}

			if (!IsEmptyItemGrid(DestCell, item->GetSize(), Cell.cell))
				return UnequipItem(item);
		}

		if (iWearCell == WEAR_BODY || iWearCell == WEAR_SECOND_BODY)
		{
			LPITEM costumeBody = GetWear(WEAR_COSTUME_BODY);

			if (costumeBody && !UnequipItem(costumeBody)) 
			{
				ChatPacket(CHAT_TYPE_INFO, "[LS;1965]");
				return false;
			}
			if (!IsEmptyItemGrid(DestCell, item->GetSize(), Cell.cell))
				return UnequipItem(item);
		}
#endif
	}

	if (DestCell.IsEquipPosition())
	{
		if (GetItem (DestCell))	// 장비일 경우 한 곳만 검사해도 된다.
		{
			ChatPacket (CHAT_TYPE_INFO, "[LS;147]");

			return false;
		}

		EquipItem (item, DestCell.cell - INVENTORY_MAX_NUM);
	}
	else
	{
		if (item->IsDragonSoul())
		{
			if (item->IsEquipped())
			{
				return DSManager::instance().PullOut (this, DestCell, item);
			}
			else
			{
				if (DestCell.window_type != DRAGON_SOUL_INVENTORY)
				{
					return false;
				}

				if (!DSManager::instance().IsValidCellForThisItem (item, DestCell))
				{
					return false;
				}
			}
		}
		// 용혼석이 아닌 아이템은 용혼석 인벤에 들어갈 수 없다.
		else if (DRAGON_SOUL_INVENTORY == DestCell.window_type)
		{
			return false;
		}

		LPITEM item2;

		if ((item2 = GetItem (DestCell)) && item != item2 && item2->IsStackable() &&
				!IS_SET (item2->GetAntiFlag(), ITEM_ANTIFLAG_STACK) &&
				item2->GetVnum() == item->GetVnum()) // 합칠 수 있는 아이템의 경우
		{
			for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
				if (item2->GetSocket (i) != item->GetSocket (i))
				{
					return false;
				}

			if (count == 0)
			{
				count = item->GetCount();
			}

			sys_log (0, "%s: ITEM_STACK %s (window: %d, cell : %d) -> (window:%d, cell %d) count %d", GetName(), item->GetName(), Cell.window_type, Cell.cell,
					 DestCell.window_type, DestCell.cell, count);

			count = MIN (200 - item2->GetCount(), count);

			item->SetCount (item->GetCount() - count);
			item2->SetCount (item2->GetCount() + count);
			return true;
		}

		if (!IsEmptyItemGrid (DestCell, item->GetSize(), Cell.cell))
		{
			return false;
		}

		if (count == 0 || count >= item->GetCount() || !item->IsStackable() || IS_SET (item->GetAntiFlag(), ITEM_ANTIFLAG_STACK))
		{
			sys_log (0, "%s: ITEM_MOVE %s (window: %d, cell : %d) -> (window:%d, cell %d) count %d", GetName(), item->GetName(), Cell.window_type, Cell.cell,
					 DestCell.window_type, DestCell.cell, count);

			item->RemoveFromCharacter();
#if defined(__BL_ENABLE_PICKUP_ITEM_EFFECT__)
			SetItem(DestCell, item, false);
#else
			SetItem(DestCell, item);
#endif

			if (INVENTORY == Cell.window_type && INVENTORY == DestCell.window_type)
			{
				SyncQuickslot (QUICKSLOT_TYPE_ITEM, Cell.cell, DestCell.cell);
			}
		}
		else if (count < item->GetCount())
		{

			sys_log (0, "%s: ITEM_SPLIT %s (window: %d, cell : %d) -> (window:%d, cell %d) count %d", GetName(), item->GetName(), Cell.window_type, Cell.cell,
					 DestCell.window_type, DestCell.cell, count);

			item->SetCount (item->GetCount() - count);
			LPITEM item2 = ITEM_MANAGER::instance().CreateItem (item->GetVnum(), count);

			// copy socket -- by mhh
			FN_copy_item_socket (item2, item);

#if defined(__BL_ENABLE_PICKUP_ITEM_EFFECT__)
			item2->AddToCharacter(this, DestCell, false);
#else
			item2->AddToCharacter(this, DestCell);
#endif

			char szBuf[51+1];
			snprintf (szBuf, sizeof (szBuf), "%u %u %u %u ", item2->GetID(), item2->GetCount(), item->GetCount(), item->GetCount() + item2->GetCount());
			LogManager::instance().ItemLog (this, item, "ITEM_SPLIT", szBuf);
		}
	}

	return true;
}

namespace NPartyPickupDistribute
{
	struct FFindOwnership
	{
		LPITEM item;
		LPCHARACTER owner;

		FFindOwnership (LPITEM item)
			: item (item), owner (NULL)
		{
		}

		void operator() (LPCHARACTER ch)
		{
			if (item->IsOwnership (ch))
			{
				owner = ch;
			}
		}
	};

	struct FCountNearMember
	{
		int		total;
		int		x, y;

		FCountNearMember (LPCHARACTER center)
			: total (0), x (center->GetX()), y (center->GetY())
		{
		}

		void operator() (LPCHARACTER ch)
		{
			if (DISTANCE_APPROX (ch->GetX() - x, ch->GetY() - y) <= PARTY_DEFAULT_RANGE)
			{
				total += 1;
			}
		}
	};

	struct FMoneyDistributor
	{
		int		total;
		LPCHARACTER	c;
		int		x, y;
		int		iMoney;

		FMoneyDistributor (LPCHARACTER center, int iMoney)
			: total (0), c (center), x (center->GetX()), y (center->GetY()), iMoney (iMoney)
		{
		}

		void operator() (LPCHARACTER ch)
		{
			if (ch!=c)
				if (DISTANCE_APPROX (ch->GetX() - x, ch->GetY() - y) <= PARTY_DEFAULT_RANGE)
				{
					ch->PointChange (POINT_GOLD, iMoney, true);

					if (iMoney > 1000) // 천원 이상만 기록한다.
					{
						LogManager::instance().CharLog (ch, iMoney, "GET_GOLD", "");
					}
				}
		}
	};
}

void CHARACTER::GiveGold (int iAmount)
{
	if (iAmount <= 0)
	{
		return;
	}

	sys_log (0, "GIVE_GOLD: %s %d", GetName(), iAmount);

	if (GetParty())
	{
		LPPARTY pParty = GetParty();

		// 파티가 있는 경우 나누어 가진다.
		DWORD dwTotal = iAmount;
		DWORD dwMyAmount = dwTotal;

		NPartyPickupDistribute::FCountNearMember funcCountNearMember (this);
		pParty->ForEachOnlineMember (funcCountNearMember);

		if (funcCountNearMember.total > 1)
		{
			DWORD dwShare = dwTotal / funcCountNearMember.total;
			dwMyAmount -= dwShare * (funcCountNearMember.total - 1);

			NPartyPickupDistribute::FMoneyDistributor funcMoneyDist (this, dwShare);

			pParty->ForEachOnlineMember (funcMoneyDist);
		}

		PointChange (POINT_GOLD, dwMyAmount, true);

		if (dwMyAmount > 1000) // 천원 이상만 기록한다.
		{
			LogManager::instance().CharLog (this, dwMyAmount, "GET_GOLD", "");
		}
	}
	else
	{
		PointChange (POINT_GOLD, iAmount, true);

		if (iAmount > 1000) // 천원 이상만 기록한다.
		{
			LogManager::instance().CharLog (this, iAmount, "GET_GOLD", "");
		}
	}
}

bool CHARACTER::PickupItem (DWORD dwVID)
{
	LPITEM item = ITEM_MANAGER::instance().FindByVID (dwVID);

	if (IsObserverMode())
	{
		return false;
	}

	if (!item || !item->GetSectree())
	{
		return false;
	}

	if (item->DistanceValid (this))
	{
		if (item->IsOwnership (this))
		{
#if defined(__BL_OFFICIAL_LOOT_FILTER__)
			if (GetLootFilter() && !GetLootFilter()->CanPickUpItem(item))
			{
				if (!GetLootFilter()->IsLootFilteredItem(dwVID))
				{
					GetLootFilter()->InsertLootFilteredItem(dwVID);

					if (GetDesc())
					{
						TPacketGCLootFilter p;
						p.header = HEADER_GC_LOOT_FILTER;
						p.enable = false;
						p.vid = dwVID;
						GetDesc()->Packet(&p, sizeof(p));
					}
				}
				ChatPacket (CHAT_TYPE_INFO, "[LS;2040]");
				return false;
			}
#endif
			// 만약 주으려 하는 아이템이 엘크라면
			if (item->GetType() == ITEM_ELK)
			{
				GiveGold (item->GetCount());
				item->RemoveFromGround();

				M2_DESTROY_ITEM (item);

				Save();
			}
			// 평범한 아이템이라면
			else
			{
				if (item->IsStackable() && !IS_SET (item->GetAntiFlag(), ITEM_ANTIFLAG_STACK))
				{
					BYTE bCount = item->GetCount();

					for (int i = 0; i < INVENTORY_MAX_NUM; ++i)
					{
						LPITEM item2 = GetInventoryItem (i);

						if (!item2)
						{
							continue;
						}

						if (item2->GetVnum() == item->GetVnum())
						{
							int j;

							for (j = 0; j < ITEM_SOCKET_MAX_NUM; ++j)
								if (item2->GetSocket (j) != item->GetSocket (j))
								{
									break;
								}

							if (j != ITEM_SOCKET_MAX_NUM)
							{
								continue;
							}

							BYTE bCount2 = MIN (200 - item2->GetCount(), bCount);
							bCount -= bCount2;

							item2->SetCount (item2->GetCount() + bCount2);

							if (bCount == 0)
							{
								ChatPacket (CHAT_TYPE_INFO, "[LS;444;%s]", item2->GetName());
								M2_DESTROY_ITEM (item);
								if (item2->GetType() == ITEM_QUEST)
								{
									quest::CQuestManager::instance().PickupItem (GetPlayerID(), item2);
								}
								return true;
							}
						}
					}

					item->SetCount (bCount);
				}

				int iEmptyCell;
				if (item->IsDragonSoul())
				{
					if ((iEmptyCell = GetEmptyDragonSoulInventory (item)) == -1)
					{
						sys_log (0, "No empty ds inventory pid %u size %ud itemid %u", GetPlayerID(), item->GetSize(), item->GetID());
						ChatPacket (CHAT_TYPE_INFO, "[LS;445]");
						return false;
					}
				}
				else
				{
					if ((iEmptyCell = GetEmptyInventory (item->GetSize())) == -1)
					{
						sys_log (0, "No empty inventory pid %u size %ud itemid %u", GetPlayerID(), item->GetSize(), item->GetID());
						ChatPacket (CHAT_TYPE_INFO, "[LS;445]");
						return false;
					}
				}

				item->RemoveFromGround();

				if (item->IsDragonSoul())
				{
					item->AddToCharacter (this, TItemPos (DRAGON_SOUL_INVENTORY, iEmptyCell));
				}
				else
				{
					item->AddToCharacter (this, TItemPos (INVENTORY, iEmptyCell));
				}

				char szHint[32+1];
				snprintf (szHint, sizeof (szHint), "%s %u %u", item->GetName(), item->GetCount(), item->GetOriginalVnum());
				LogManager::instance().ItemLog (this, item, "GET", szHint);
				ChatPacket (CHAT_TYPE_INFO, "[LS;444;%s]", item->GetName());

				if (item->GetType() == ITEM_QUEST)
				{
					quest::CQuestManager::instance().PickupItem (GetPlayerID(), item);
				}
			}

			//Motion(MOTION_PICKUP);
			return true;
		}
		else if (!IS_SET (item->GetAntiFlag(), ITEM_ANTIFLAG_GIVE | ITEM_ANTIFLAG_DROP) && GetParty())
		{
			// 다른 파티원 소유권 아이템을 주으려고 한다면
			NPartyPickupDistribute::FFindOwnership funcFindOwnership (item);

			GetParty()->ForEachOnlineMember (funcFindOwnership);

			LPCHARACTER owner = funcFindOwnership.owner;

			int iEmptyCell;

			if (item->IsDragonSoul())
			{
				if (! (owner && (iEmptyCell = owner->GetEmptyDragonSoulInventory (item)) != -1))
				{
					owner = this;

					if ((iEmptyCell = GetEmptyDragonSoulInventory (item)) == -1)
					{
						owner->ChatPacket (CHAT_TYPE_INFO, "[LS;445]");
						return false;
					}
				}
			}
			else
			{
				if (! (owner && (iEmptyCell = owner->GetEmptyInventory (item->GetSize())) != -1))
				{
					owner = this;

					if ((iEmptyCell = GetEmptyInventory (item->GetSize())) == -1)
					{
						owner->ChatPacket (CHAT_TYPE_INFO, "[LS;445]");
						return false;
					}
				}
			}

#if defined(__BL_OFFICIAL_LOOT_FILTER__)
			if (owner->GetLootFilter() && !owner->GetLootFilter()->CanPickUpItem(item))
			{
				if (owner == this)
				{
					if (!owner->GetLootFilter()->IsLootFilteredItem(dwVID))
					{
						owner->GetLootFilter()->InsertLootFilteredItem(dwVID);

						if (owner->GetDesc())
						{
							TPacketGCLootFilter p;
							p.header = HEADER_GC_LOOT_FILTER;
							p.enable = false;
							p.vid = dwVID;
							owner->GetDesc()->Packet(&p, sizeof(p));
						}
					}
					ChatPacket (CHAT_TYPE_INFO, "[LS;2040]");
				}
				else
				{
					ChatPacket (CHAT_TYPE_INFO, "[LS;2041]");
				}

				return false;
			}
#endif

			item->RemoveFromGround();

			if (item->IsDragonSoul())
			{
				item->AddToCharacter (owner, TItemPos (DRAGON_SOUL_INVENTORY, iEmptyCell));
			}
			else
			{
				item->AddToCharacter (owner, TItemPos (INVENTORY, iEmptyCell));
			}

			char szHint[32+1];
			snprintf (szHint, sizeof (szHint), "%s %u %u", item->GetName(), item->GetCount(), item->GetOriginalVnum());
			LogManager::instance().ItemLog (owner, item, "GET", szHint);

			if (owner == this)
			{
				ChatPacket (CHAT_TYPE_INFO, "[LS;444;%s]", item->GetName());
			}
			else
			{
				owner->ChatPacket (CHAT_TYPE_INFO, "[LS;446;%s;%s]", GetName(), item->GetName());
				ChatPacket (CHAT_TYPE_INFO, "[LS;449;%s;%s]", owner->GetName(), item->GetName());
			}

			if (item->GetType() == ITEM_QUEST)
			{
				quest::CQuestManager::instance().PickupItem (owner->GetPlayerID(), item);
			}

			return true;
		}
	}

	return false;
}

bool CHARACTER::SwapItem (BYTE bCell, BYTE bDestCell)
{
	if (!CanHandleItem())
	{
		return false;
	}

	TItemPos srcCell (INVENTORY, bCell), destCell (INVENTORY, bDestCell);

	// 올바른 Cell 인지 검사
	// 용혼석은 Swap할 수 없으므로, 여기서 걸림.
	//if (bCell >= INVENTORY_MAX_NUM + WEAR_MAX_NUM || bDestCell >= INVENTORY_MAX_NUM + WEAR_MAX_NUM)
	if (srcCell.IsDragonSoulEquipPosition() || destCell.IsDragonSoulEquipPosition())
	{
		return false;
	}

	// 같은 CELL 인지 검사
	if (bCell == bDestCell)
	{
		return false;
	}

	// 둘 다 장비창 위치면 Swap 할 수 없다.
	if (srcCell.IsEquipPosition() && destCell.IsEquipPosition())
	{
		return false;
	}

	LPITEM item1, item2;

	// item2가 장비창에 있는 것이 되도록.
	if (srcCell.IsEquipPosition())
	{
		item1 = GetInventoryItem (bDestCell);
		item2 = GetInventoryItem (bCell);
	}
	else
	{
		item1 = GetInventoryItem (bCell);
		item2 = GetInventoryItem (bDestCell);
	}

	if (!item1 || !item2)
	{
		return false;
	}

	if (item1 == item2)
	{
		sys_log (0, "[WARNING][WARNING][HACK USER!] : %s %d %d", m_stName.c_str(), bCell, bDestCell);
		return false;
	}

	// item2가 bCell위치에 들어갈 수 있는지 확인한다.
	if (!IsEmptyItemGrid (TItemPos (INVENTORY, item1->GetCell()), item2->GetSize(), item1->GetCell()))
	{
		return false;
	}

	// 바꿀 아이템이 장비창에 있으면
	if (TItemPos (EQUIPMENT, item2->GetCell()).IsEquipPosition())
	{
		BYTE bEquipCell = item2->GetCell() - INVENTORY_MAX_NUM;
		BYTE bInvenCell = item1->GetCell();

		// 착용중인 아이템을 벗을 수 있고, 착용 예정 아이템이 착용 가능한 상태여야만 진행
		if (false == CanUnequipNow (item2) || false == CanEquipNow (item1))
		{
			return false;
		}

		if (bEquipCell != item1->FindEquipCell (this)) // 같은 위치일때만 허용
		{
			return false;
		}

		item2->RemoveFromCharacter();

		if (item1->EquipTo(this, bEquipCell))
#if defined(__BL_ENABLE_PICKUP_ITEM_EFFECT__)
			item2->AddToCharacter(this, TItemPos(INVENTORY, bInvenCell), false);
#else
			item2->AddToCharacter(this, TItemPos(INVENTORY, bInvenCell));
#endif
		else
		{
			sys_err ("SwapItem cannot equip %s! item1 %s", item2->GetName(), item1->GetName());
		}
	}
	else
	{
		BYTE bCell1 = item1->GetCell();
		BYTE bCell2 = item2->GetCell();

		item1->RemoveFromCharacter();
		item2->RemoveFromCharacter();

#if defined(__BL_ENABLE_PICKUP_ITEM_EFFECT__)
		item1->AddToCharacter(this, TItemPos(INVENTORY, bCell2), false);
		item2->AddToCharacter(this, TItemPos(INVENTORY, bCell1), false);
#else
		item1->AddToCharacter(this, TItemPos(INVENTORY, bCell2));
		item2->AddToCharacter(this, TItemPos(INVENTORY, bCell1));
#endif
	}

	return true;
}

bool CHARACTER::UnequipItem (LPITEM item)
{
	int pos;
#ifdef __WEAPON_COSTUME_SYSTEM__
	int iWearCell = item->FindEquipCell(this);
	if (iWearCell == WEAR_WEAPON || iWearCell == WEAR_SECOND_WEAPON)
	{
		LPITEM costumeWeaponn = GetWear(WEAR_COSTUME_WEAPON);
		
		// E?er kostum silahı varsa, cıkarılması gerekiyor
		if (costumeWeaponn && !UnequipItem(costumeWeaponn)) 
		{
			ChatPacket(CHAT_TYPE_INFO, "[LS;1965]");
			return false;
		}
	}
	if (iWearCell == WEAR_BODY || iWearCell == WEAR_SECOND_BODY)
	{
		LPITEM costumeBody = GetWear(WEAR_COSTUME_BODY);

		if (costumeBody && !UnequipItem(costumeBody)) 
		{
			ChatPacket(CHAT_TYPE_INFO, "[LS;1965]");
			return false;
		}
	}
#endif

	if (false == CanUnequipNow (item))
	{
		return false;
	}

	if (item->IsDragonSoul())
	{
		pos = GetEmptyDragonSoulInventory (item);
	}
	else
	{
		pos = GetEmptyInventory (item->GetSize());
	}

	// HARD CODING
	if (item->GetVnum() == UNIQUE_ITEM_HIDE_ALIGNMENT_TITLE)
	{
		ShowAlignment (true);
	}

	item->RemoveFromCharacter();
	if (item->IsDragonSoul())
	{
		item->AddToCharacter (this, TItemPos (DRAGON_SOUL_INVENTORY, pos));
	}
	else
	{
#if defined(__BL_ENABLE_PICKUP_ITEM_EFFECT__)
		item->AddToCharacter(this, TItemPos(INVENTORY, pos), false);
#else
		item->AddToCharacter(this, TItemPos(INVENTORY, pos));
#endif
	}

	CheckMaximumPoints();

	return true;
}

//
// @version	05/07/05 Bang2ni - Skill 사용후 1.5 초 이내에 장비 착용 금지
//
bool CHARACTER::EquipItem (LPITEM item, int iCandidateCell)
{
	if (item->IsExchanging())
	{
		return false;
	}

	if (false == item->IsEquipable())
	{
		return false;
	}

	if (false == CanEquipNow (item))
	{
		return false;
	}

	int iWearCell = item->FindEquipCell (this, iCandidateCell);

	if (iWearCell < 0)
	{
		return false;
	}

	// 무언가를 탄 상태에서 턱시도 입기 금지
#ifdef ENABLE_ADDITIONAL_EQUIPMENT_PAGE
	if ((iWearCell == WEAR_BODY || iWearCell == WEAR_SECOND_BODY) && IsRiding() && (item->GetVnum() >= 11901 && item->GetVnum() <= 11904))
#else
	if (iWearCell == WEAR_BODY && IsRiding() && (item->GetVnum() >= 11901 && item->GetVnum() <= 11904))
#endif
	{
		ChatPacket (CHAT_TYPE_INFO, "[LS;917]");
		return false;
	}

#ifdef ENABLE_ADDITIONAL_EQUIPMENT_PAGE
	if ((iWearCell != WEAR_ARROW || iWearCell != WEAR_SECOND_ARROW) && IsPolymorphed())
#else
	if (iWearCell != WEAR_ARROW && IsPolymorphed())
#endif
	{
		ChatPacket (CHAT_TYPE_INFO, "[LS;450]");
		return false;
	}

	if (FN_check_item_sex (this, item) == false)
	{
		ChatPacket (CHAT_TYPE_INFO, "[LS;1005]");
		return false;
	}

#if defined(__BL_TRANSMUTATION__) && defined(__COSTUME_SYSTEM__)
	DWORD dwTransmutationVnum = item->GetTransmutationVnum();
	if (dwTransmutationVnum != 0)
	{
		TItemTable* pItemTable = ITEM_MANAGER::instance().GetTable(dwTransmutationVnum);
		if (pItemTable && (pItemTable->GetType() == ITEM_COSTUME))
		{
			if ((IS_SET(pItemTable->GetAntiFlags(), ITEM_ANTIFLAG_MALE) && SEX_MALE == GET_SEX(this)) ||
				(IS_SET(pItemTable->GetAntiFlags(), ITEM_ANTIFLAG_FEMALE) && SEX_FEMALE == GET_SEX(this)))
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Transmutation] You cannot equip the transmuted item as it does not match your gender."));
				return false;
			}
		}
	}
#endif

	//신규 탈것 사용시 기존 말 사용여부 체크
	if (item->IsRideItem() && IsRiding())
	{
		ChatPacket (CHAT_TYPE_INFO, "[LS;1054]");
		return false;
	}

	// 화살 이외에는 마지막 공격 시간 또는 스킬 사용 1.5 후에 장비 교체가 가능
	DWORD dwCurTime = get_dword_time();

#ifdef ENABLE_ADDITIONAL_EQUIPMENT_PAGE
	if ((iWearCell != WEAR_ARROW || iWearCell != WEAR_SECOND_ARROW)
		&& (dwCurTime - GetLastAttackTime() <= 1500 || dwCurTime - m_dwLastSkillTime <= 1500))
#else
	if (iWearCell != WEAR_ARROW
		&& (dwCurTime - GetLastAttackTime() <= 1500 || dwCurTime - m_dwLastSkillTime <= 1500))
#endif
	{
		ChatPacket (CHAT_TYPE_INFO, "[LS;451]");
		return false;
	}

#ifdef __WEAPON_COSTUME_SYSTEM__
	if (iWearCell == WEAR_WEAPON)
	{
		if (item->GetType() == ITEM_WEAPON)
		{
			LPITEM costumeWeapon = GetWear(WEAR_COSTUME_WEAPON);
			if (costumeWeapon && costumeWeapon->GetValue(3) != item->GetSubType() && !UnequipItem(costumeWeapon))
			{
				ChatPacket (CHAT_TYPE_INFO, "[LS;1965]");
				return false;
			}
		}
		else //fishrod/pickaxe
		{
			LPITEM costumeWeapon = GetWear(WEAR_COSTUME_WEAPON);
			if (costumeWeapon && !UnequipItem(costumeWeapon))
			{
				ChatPacket (CHAT_TYPE_INFO, "[LS;1965]");
				return false;
			}
		}
	}
	else if (iWearCell == WEAR_COSTUME_WEAPON)
	{
		if (item->GetType() == ITEM_COSTUME && item->GetSubType() == COSTUME_WEAPON)
		{
			LPITEM pkWeapon = GetEquipWear(WEAR_WEAPON);
			if (!pkWeapon || pkWeapon->GetType() != ITEM_WEAPON || item->GetValue(3) != pkWeapon->GetSubType())
			{
				ChatPacket (CHAT_TYPE_INFO, "[LS;1966]");
				return false;
			}
		}
	}
#endif

	// 용혼석 특수 처리
	if (item->IsDragonSoul())
	{
		// 같은 타입의 용혼석이 이미 들어가 있다면 착용할 수 없다.
		// 용혼석은 swap을 지원하면 안됨.
		if (GetInventoryItem (INVENTORY_MAX_NUM + iWearCell))
		{
			ChatPacket (CHAT_TYPE_INFO, "[LS;1090]");
			return false;
		}

		if (!item->EquipTo (this, iWearCell))
		{
			return false;
		}
	}
	// 용혼석이 아님.
	else
	{
		// 착용할 곳에 아이템이 있다면,
		if (GetWear (iWearCell) && !IS_SET (GetWear (iWearCell)->GetFlag(), ITEM_FLAG_IRREMOVABLE))
		{
			// 이 아이템은 한번 박히면 변경 불가. swap 역시 완전 불가
#ifndef ENABLE_ADDITIONAL_EQUIPMENT_PAGE
			// Once this item is stuck, it cannot be changed. swap is completely impossible
			if (item->GetWearFlag() == WEARABLE_ABILITY)
				return false;
#endif

			if (false == SwapItem (item->GetCell(), INVENTORY_MAX_NUM + iWearCell))
			{
				return false;
			}
		}
		else
		{
			BYTE bOldCell = item->GetCell();

			if (item->EquipTo (this, iWearCell))
			{
				SyncQuickslot (QUICKSLOT_TYPE_ITEM, bOldCell, iWearCell);
			}
		}
	}

	if (true == item->IsEquipped())
	{
		// 아이템 최초 사용 이후부터는 사용하지 않아도 시간이 차감되는 방식 처리.
		if (-1 != item->GetProto()->cLimitRealTimeFirstUseIndex)
		{
			// 한 번이라도 사용한 아이템인지 여부는 Socket1을 보고 판단한다. (Socket1에 사용횟수 기록)
			if (0 == item->GetSocket (1))
			{
				// 사용가능시간은 Default 값으로 Limit Value 값을 사용하되, Socket0에 값이 있으면 그 값을 사용하도록 한다. (단위는 초)
				long duration = (0 != item->GetSocket (0)) ? item->GetSocket (0) : item->GetProto()->aLimits[item->GetProto()->cLimitRealTimeFirstUseIndex].lValue;

				if (0 == duration)
				{
					duration = 60 * 60 * 24 * 7;
				}

				item->SetSocket (0, time (0) + duration);
				item->StartRealTimeExpireEvent();
			}

			item->SetSocket (1, item->GetSocket (1) + 1);
		}

		if (item->GetVnum() == UNIQUE_ITEM_HIDE_ALIGNMENT_TITLE)
		{
			ShowAlignment (false);
		}

		const DWORD& dwVnum = item->GetVnum();

		// 라마단 이벤트 초승달의 반지(71135) 착용시 이펙트 발동
		if (true == CItemVnumHelper::IsRamadanMoonRing (dwVnum))
		{
			this->EffectPacket (SE_EQUIP_RAMADAN_RING);
		}
		// 할로윈 사탕(71136) 착용시 이펙트 발동
		else if (true == CItemVnumHelper::IsHalloweenCandy (dwVnum))
		{
			this->EffectPacket (SE_EQUIP_HALLOWEEN_CANDY);
		}
		// 행복의 반지(71143) 착용시 이펙트 발동
		else if (true == CItemVnumHelper::IsHappinessRing (dwVnum))
		{
			this->EffectPacket (SE_EQUIP_HAPPINESS_RING);
		}
		// 사랑의 팬던트(71145) 착용시 이펙트 발동
		else if (true == CItemVnumHelper::IsLovePendant (dwVnum))
		{
			this->EffectPacket (SE_EQUIP_LOVE_PENDANT);
		}
		// ITEM_UNIQUE의 경우, SpecialItemGroup에 정의되어 있고, (item->GetSIGVnum() != NULL)
		//
		else if (ITEM_UNIQUE == item->GetType() && 0 != item->GetSIGVnum())
		{
			const CSpecialItemGroup* pGroup = ITEM_MANAGER::instance().GetSpecialItemGroup (item->GetSIGVnum());
			if (NULL != pGroup)
			{
				const CSpecialAttrGroup* pAttrGroup = ITEM_MANAGER::instance().GetSpecialAttrGroup (pGroup->GetAttrVnum (item->GetVnum()));
				if (NULL != pAttrGroup)
				{
					const std::string& std = pAttrGroup->m_stEffectFileName;
					SpecificEffectPacket (std.c_str());
				}
			}
		}
#ifdef ENABLE_ACCE_SYSTEM
		else if ((item->GetType() == ITEM_COSTUME) && (item->GetSubType() == COSTUME_ACCE))
			this->EffectPacket(SE_EFFECT_ACCE_EQUIP);
#endif
		if (UNIQUE_SPECIAL_RIDE == item->GetSubType() && IS_SET (item->GetFlag(), ITEM_FLAG_QUEST_USE))
		{
			quest::CQuestManager::instance().UseItem (GetPlayerID(), item, false);
		}
	}

	return true;
}

void CHARACTER::BuffOnAttr_AddBuffsFromItem (LPITEM pItem)
{
	for (int i = 0; i < sizeof (g_aBuffOnAttrPoints)/sizeof (g_aBuffOnAttrPoints[0]); i++)
	{
		TMapBuffOnAttrs::iterator it = m_map_buff_on_attrs.find (g_aBuffOnAttrPoints[i]);
		if (it != m_map_buff_on_attrs.end())
		{
			it->second->AddBuffFromItem (pItem);
		}
	}
}

void CHARACTER::BuffOnAttr_RemoveBuffsFromItem (LPITEM pItem)
{
	for (int i = 0; i < sizeof (g_aBuffOnAttrPoints)/sizeof (g_aBuffOnAttrPoints[0]); i++)
	{
		TMapBuffOnAttrs::iterator it = m_map_buff_on_attrs.find (g_aBuffOnAttrPoints[i]);
		if (it != m_map_buff_on_attrs.end())
		{
			it->second->RemoveBuffFromItem (pItem);
		}
	}
}

void CHARACTER::BuffOnAttr_ClearAll()
{
	for (TMapBuffOnAttrs::iterator it = m_map_buff_on_attrs.begin(); it != m_map_buff_on_attrs.end(); it++)
	{
		CBuffOnAttributes* pBuff = it->second;
		if (pBuff)
		{
			pBuff->Initialize();
		}
	}
}

void CHARACTER::BuffOnAttr_ValueChange (BYTE bType, BYTE bOldValue, BYTE bNewValue)
{
	TMapBuffOnAttrs::iterator it = m_map_buff_on_attrs.find (bType);

	if (0 == bNewValue)
	{
		if (m_map_buff_on_attrs.end() == it)
		{
			return;
		}
		else
		{
			it->second->Off();
		}
	}
	else if (0 == bOldValue)
	{
		CBuffOnAttributes* pBuff;
		if (m_map_buff_on_attrs.end() == it)
		{
			switch (bType)
			{
				case POINT_ENERGY:
				{
#ifdef ENABLE_ADDITIONAL_EQUIPMENT_PAGE
					if (GetEquipIndex() == FIRST_TYPE_EQUIPMENT)
					{
						static BYTE abSlot[] = { WEAR_BODY, WEAR_HEAD, WEAR_FOOTS, WEAR_WRIST, WEAR_WEAPON, WEAR_NECK, WEAR_EAR, WEAR_SHIELD };
						static std::vector <BYTE> vec_slots(abSlot, abSlot + _countof(abSlot));
						pBuff = M2_NEW CBuffOnAttributes(this, bType, &vec_slots);
					}
					else
					{
						static BYTE abSlot[] = { WEAR_SECOND_BODY, WEAR_SECOND_HEAD, WEAR_SECOND_FOOTS, WEAR_SECOND_WRIST, WEAR_SECOND_WEAPON,
						WEAR_SECOND_NECK, WEAR_SECOND_EAR, WEAR_SECOND_SHIELD };
						static std::vector <BYTE> vec_slots(abSlot, abSlot + _countof(abSlot));
						pBuff = M2_NEW CBuffOnAttributes(this, bType, &vec_slots);
					}
#else
					static BYTE abSlot[] = { WEAR_BODY, WEAR_HEAD, WEAR_FOOTS, WEAR_WRIST, WEAR_WEAPON, WEAR_NECK, WEAR_EAR, WEAR_SHIELD };
					static std::vector <BYTE> vec_slots(abSlot, abSlot + _countof(abSlot));
					pBuff = M2_NEW CBuffOnAttributes(this, bType, &vec_slots);
#endif
				}
				break;
				case POINT_COSTUME_ATTR_BONUS:
				{
					static BYTE abSlot[] = { WEAR_COSTUME_BODY, WEAR_COSTUME_HAIR, WEAR_COSTUME_WEAPON, WEAR_COSTUME_MOUNT };
					static std::vector <BYTE> vec_slots (abSlot, abSlot + _countof (abSlot));
					pBuff = M2_NEW CBuffOnAttributes (this, bType, &vec_slots);
				}
				break;
				default:
					break;
			}
			m_map_buff_on_attrs.insert (TMapBuffOnAttrs::value_type (bType, pBuff));

		}
		else
		{
			pBuff = it->second;
		}

		pBuff->On (bNewValue);
	}
	else
	{
		if (m_map_buff_on_attrs.end() == it)
		{
			return;
		}
		else
		{
			it->second->ChangeBuffValue (bNewValue);
		}
	}
}


LPITEM CHARACTER::FindSpecifyItem (DWORD vnum) const
{
	for (int i = 0; i < INVENTORY_MAX_NUM; ++i)
		if (GetInventoryItem (i) && GetInventoryItem (i)->GetVnum() == vnum)
		{
			return GetInventoryItem (i);
		}

	return NULL;
}

LPITEM CHARACTER::FindItemByID (DWORD id) const
{
	for (int i=0 ; i < INVENTORY_MAX_NUM ; ++i)
	{
		if (NULL != GetInventoryItem (i) && GetInventoryItem (i)->GetID() == id)
		{
			return GetInventoryItem (i);
		}
	}

	for (int i=BELT_INVENTORY_SLOT_START; i < BELT_INVENTORY_SLOT_END ; ++i)
	{
		if (NULL != GetInventoryItem (i) && GetInventoryItem (i)->GetID() == id)
		{
			return GetInventoryItem (i);
		}
	}

	return NULL;
}

int CHARACTER::CountSpecifyItem (DWORD vnum) const
{
	int	count = 0;
	LPITEM item;

	for (int i = 0; i < INVENTORY_MAX_NUM; ++i)
	{
		item = GetInventoryItem (i);
		if (NULL != item && item->GetVnum() == vnum)
		{
			// 개인 상점에 등록된 물건이면 넘어간다.
			if (m_pkMyShop && m_pkMyShop->IsSellingItem (item->GetID()))
			{
				continue;
			}
			else
			{
				count += item->GetCount();
			}
		}
	}

	return count;
}

void CHARACTER::RemoveSpecifyItem (DWORD vnum, DWORD count)
{
	if (0 == count)
	{
		return;
	}

	for (UINT i = 0; i < INVENTORY_MAX_NUM; ++i)
	{
		if (NULL == GetInventoryItem (i))
		{
			continue;
		}

		if (GetInventoryItem (i)->GetVnum() != vnum)
		{
			continue;
		}

		//개인 상점에 등록된 물건이면 넘어간다. (개인 상점에서 판매될때 이 부분으로 들어올 경우 문제!)
		if (m_pkMyShop)
		{
			bool isItemSelling = m_pkMyShop->IsSellingItem (GetInventoryItem (i)->GetID());
			if (isItemSelling)
			{
				continue;
			}
		}

		if (vnum >= 80003 && vnum <= 80007)
		{
			LogManager::instance().GoldBarLog (GetPlayerID(), GetInventoryItem (i)->GetID(), QUEST, "RemoveSpecifyItem");
		}

		if (count >= GetInventoryItem (i)->GetCount())
		{
			count -= GetInventoryItem (i)->GetCount();
			GetInventoryItem (i)->SetCount (0);

			if (0 == count)
			{
				return;
			}
		}
		else
		{
			GetInventoryItem (i)->SetCount (GetInventoryItem (i)->GetCount() - count);
			return;
		}
	}

	// 예외처리가 약하다.
	if (count)
	{
		sys_log (0, "CHARACTER::RemoveSpecifyItem cannot remove enough item vnum %u, still remain %d", vnum, count);
	}
}

int CHARACTER::CountSpecifyTypeItem (BYTE type) const
{
	int	count = 0;

	for (int i = 0; i < INVENTORY_MAX_NUM; ++i)
	{
		LPITEM pItem = GetInventoryItem (i);
		if (pItem != NULL && pItem->GetType() == type)
		{
			count += pItem->GetCount();
		}
	}

	return count;
}

void CHARACTER::RemoveSpecifyTypeItem (BYTE type, DWORD count)
{
	if (0 == count)
	{
		return;
	}

	for (UINT i = 0; i < INVENTORY_MAX_NUM; ++i)
	{
		if (NULL == GetInventoryItem (i))
		{
			continue;
		}

		if (GetInventoryItem (i)->GetType() != type)
		{
			continue;
		}

		//개인 상점에 등록된 물건이면 넘어간다. (개인 상점에서 판매될때 이 부분으로 들어올 경우 문제!)
		if (m_pkMyShop)
		{
			bool isItemSelling = m_pkMyShop->IsSellingItem (GetInventoryItem (i)->GetID());
			if (isItemSelling)
			{
				continue;
			}
		}

		if (count >= GetInventoryItem (i)->GetCount())
		{
			count -= GetInventoryItem (i)->GetCount();
			GetInventoryItem (i)->SetCount (0);

			if (0 == count)
			{
				return;
			}
		}
		else
		{
			GetInventoryItem (i)->SetCount (GetInventoryItem (i)->GetCount() - count);
			return;
		}
	}
}

void CHARACTER::AutoGiveItem (LPITEM item, bool longOwnerShip)
{
	if (NULL == item)
	{
		sys_err ("NULL point.");
		return;
	}
	if (item->GetOwner())
	{
		sys_err ("item %d 's owner exists!", item->GetID());
		return;
	}

	int cell;
	if (item->IsDragonSoul())
	{
		cell = GetEmptyDragonSoulInventory (item);
	}
	else
	{
		cell = GetEmptyInventory (item->GetSize());
	}

	if (cell != -1)
	{
		if (item->IsDragonSoul())
		{
			item->AddToCharacter (this, TItemPos (DRAGON_SOUL_INVENTORY, cell));
		}
		else
		{
			item->AddToCharacter (this, TItemPos (INVENTORY, cell));
		}

		LogManager::instance().ItemLog (this, item, "SYSTEM", item->GetName());

		if (item->GetType() == ITEM_USE && item->GetSubType() == USE_POTION)
		{
			TQuickslot* pSlot;

			if (GetQuickslot (0, &pSlot) && pSlot->type == QUICKSLOT_TYPE_NONE)
			{
				TQuickslot slot;
				slot.type = QUICKSLOT_TYPE_ITEM;
				slot.pos = cell;
				SetQuickslot (0, slot);
			}
		}
	}
	else
	{
		item->AddToGround (GetMapIndex(), GetXYZ());
		item->StartDestroyEvent();

		if (longOwnerShip)
		{
			item->SetOwnership (this, 300);
		}
		else
		{
			item->SetOwnership (this, 60);
		}
		LogManager::instance().ItemLog (this, item, "SYSTEM_DROP", item->GetName());
	}
}

LPITEM CHARACTER::AutoGiveItem (DWORD dwItemVnum, BYTE bCount, int iRarePct, bool bMsg)
{
	TItemTable* p = ITEM_MANAGER::instance().GetTable (dwItemVnum);

	if (!p)
	{
		return NULL;
	}

	DBManager::instance().SendMoneyLog (MONEY_LOG_DROP, dwItemVnum, bCount);

	if (p->dwFlags & ITEM_FLAG_STACKABLE && p->bType != ITEM_BLEND)
	{
		for (int i = 0; i < INVENTORY_MAX_NUM; ++i)
		{
			LPITEM item = GetInventoryItem (i);

			if (!item)
			{
				continue;
			}

			if (item->GetVnum() == dwItemVnum && FN_check_item_socket (item))
			{
				if (IS_SET (p->dwFlags, ITEM_FLAG_MAKECOUNT))
				{
					if (bCount < p->alValues[1])
					{
						bCount = p->alValues[1];
					}
				}

				BYTE bCount2 = MIN (200 - item->GetCount(), bCount);
				bCount -= bCount2;

				item->SetCount (item->GetCount() + bCount2);

				if (bCount == 0)
				{
					if (bMsg)
					{
						ChatPacket (CHAT_TYPE_INFO, "[LS;444;%s]", item->GetName());
					}

					return item;
				}
			}
		}
	}

	LPITEM item = ITEM_MANAGER::instance().CreateItem (dwItemVnum, bCount, 0, true);

	if (!item)
	{
		sys_err ("cannot create item by vnum %u (name: %s)", dwItemVnum, GetName());
		return NULL;
	}

	if (item->GetType() == ITEM_BLEND)
	{
		for (int i=0; i < INVENTORY_MAX_NUM; i++)
		{
			LPITEM inv_item = GetInventoryItem (i);

			if (inv_item == NULL)
			{
				continue;
			}

			if (inv_item->GetType() == ITEM_BLEND)
			{
				if (inv_item->GetVnum() == item->GetVnum())
				{
					if (inv_item->GetSocket (0) == item->GetSocket (0) &&
							inv_item->GetSocket (1) == item->GetSocket (1) &&
							inv_item->GetSocket (2) == item->GetSocket (2) &&
							inv_item->GetCount() < ITEM_MAX_COUNT)
					{
						inv_item->SetCount (inv_item->GetCount() + item->GetCount());
						return inv_item;
					}
				}
			}
		}
	}

	int iEmptyCell;
	if (item->IsDragonSoul())
	{
		iEmptyCell = GetEmptyDragonSoulInventory (item);
	}
	else
	{
		iEmptyCell = GetEmptyInventory (item->GetSize());
	}

	if (iEmptyCell != -1)
	{
		if (bMsg)
		{
			ChatPacket (CHAT_TYPE_INFO, "[LS;444;%s]", item->GetName());
		}

		if (item->IsDragonSoul())
		{
			item->AddToCharacter (this, TItemPos (DRAGON_SOUL_INVENTORY, iEmptyCell));
		}
		else
		{
			item->AddToCharacter (this, TItemPos (INVENTORY, iEmptyCell));
		}
		LogManager::instance().ItemLog (this, item, "SYSTEM", item->GetName());

		if (item->GetType() == ITEM_USE && item->GetSubType() == USE_POTION)
		{
			TQuickslot* pSlot;

			if (GetQuickslot (0, &pSlot) && pSlot->type == QUICKSLOT_TYPE_NONE)
			{
				TQuickslot slot;
				slot.type = QUICKSLOT_TYPE_ITEM;
				slot.pos = iEmptyCell;
				SetQuickslot (0, slot);
			}
		}
	}
	else
	{
		item->AddToGround (GetMapIndex(), GetXYZ());
		item->StartDestroyEvent();
		// 안티 드랍 flag가 걸려있는 아이템의 경우,
		// 인벤에 빈 공간이 없어서 어쩔 수 없이 떨어트리게 되면,
		// ownership을 아이템이 사라질 때까지(300초) 유지한다.
		if (IS_SET (item->GetAntiFlag(), ITEM_ANTIFLAG_DROP))
		{
			item->SetOwnership (this, 300);
		}
		else
		{
			item->SetOwnership (this, 60);
		}
		LogManager::instance().ItemLog (this, item, "SYSTEM_DROP", item->GetName());
	}

	sys_log (0,
			 "7: %d %d", dwItemVnum, bCount);
	return item;
}

bool CHARACTER::GiveItem (LPCHARACTER victim, TItemPos Cell)
{
	if (!CanHandleItem())
	{
		return false;
	}

	LPITEM item = GetItem (Cell);

	if (item && !item->IsExchanging())
	{
		if (victim->CanReceiveItem (this, item))
		{
			victim->ReceiveItem (this, item);
			return true;
		}
	}

	return false;
}

bool CHARACTER::CanReceiveItem (LPCHARACTER from, LPITEM item) const
{
	if (IsPC())
	{
		return false;
	}

	// TOO_LONG_DISTANCE_EXCHANGE_BUG_FIX
	if (DISTANCE_APPROX (GetX() - from->GetX(), GetY() - from->GetY()) > 2000)
	{
		return false;
	}
	// END_OF_TOO_LONG_DISTANCE_EXCHANGE_BUG_FIX

	switch (GetRaceNum())
	{
		case fishing::CAMPFIRE_MOB:
			if (item->GetType() == ITEM_FISH &&
					(item->GetSubType() == FISH_ALIVE || item->GetSubType() == FISH_DEAD))
			{
				return true;
			}
			break;

		case fishing::FISHER_MOB:
			if (item->GetType() == ITEM_ROD)
			{
				return true;
			}
			break;

		// BUILDING_NPC
		case BLACKSMITH_WEAPON_MOB:
		case DEVILTOWER_BLACKSMITH_WEAPON_MOB:
			if (item->GetType() == ITEM_WEAPON &&
					item->GetRefinedVnum())
			{
				return true;
			}
			else
			{
				return false;
			}
			break;

		case BLACKSMITH_ARMOR_MOB:
		case DEVILTOWER_BLACKSMITH_ARMOR_MOB:
			if (item->GetType() == ITEM_ARMOR &&
					(item->GetSubType() == ARMOR_BODY || item->GetSubType() == ARMOR_SHIELD || item->GetSubType() == ARMOR_HEAD) &&
					item->GetRefinedVnum())
			{
				return true;
			}
			else
			{
				return false;
			}
			break;

		case BLACKSMITH_ACCESSORY_MOB:
		case DEVILTOWER_BLACKSMITH_ACCESSORY_MOB:
			if (item->GetType() == ITEM_ARMOR &&
					! (item->GetSubType() == ARMOR_BODY || item->GetSubType() == ARMOR_SHIELD || item->GetSubType() == ARMOR_HEAD) &&
					item->GetRefinedVnum())
			{
				return true;
			}
			else
			{
				return false;
			}
			break;
		// END_OF_BUILDING_NPC

		case BLACKSMITH_MOB:
			if (item->GetRefinedVnum() && item->GetRefineSet() < 500)
			{
				return true;
			}
			else
			{
				return false;
			}

		case BLACKSMITH2_MOB:
			if (item->GetRefineSet() >= 500)
			{
				return true;
			}
			else
			{
				return false;
			}

		case ALCHEMIST_MOB:
			if (item->GetRefinedVnum())
			{
				return true;
			}
			break;

		case 20101:
		case 20102:
		case 20103:
			// 초급 말
			if (item->GetVnum() == ITEM_REVIVE_HORSE_1)
			{
				if (!IsDead())
				{
					from->ChatPacket (CHAT_TYPE_INFO, "[LS;452]");
					return false;
				}
				return true;
			}
			else if (item->GetVnum() == ITEM_HORSE_FOOD_1)
			{
				if (IsDead())
				{
					from->ChatPacket (CHAT_TYPE_INFO, "[LS;453]");
					return false;
				}
				return true;
			}
			else if (item->GetVnum() == ITEM_HORSE_FOOD_2 || item->GetVnum() == ITEM_HORSE_FOOD_3)
			{
				return false;
			}
			break;
		case 20104:
		case 20105:
		case 20106:
			// 중급 말
			if (item->GetVnum() == ITEM_REVIVE_HORSE_2)
			{
				if (!IsDead())
				{
					from->ChatPacket (CHAT_TYPE_INFO, "[LS;452]");
					return false;
				}
				return true;
			}
			else if (item->GetVnum() == ITEM_HORSE_FOOD_2)
			{
				if (IsDead())
				{
					from->ChatPacket (CHAT_TYPE_INFO, "[LS;453]");
					return false;
				}
				return true;
			}
			else if (item->GetVnum() == ITEM_HORSE_FOOD_1 || item->GetVnum() == ITEM_HORSE_FOOD_3)
			{
				return false;
			}
			break;
		case 20107:
		case 20108:
		case 20109:
			// 고급 말
			if (item->GetVnum() == ITEM_REVIVE_HORSE_3)
			{
				if (!IsDead())
				{
					from->ChatPacket (CHAT_TYPE_INFO, "[LS;452]");
					return false;
				}
				return true;
			}
			else if (item->GetVnum() == ITEM_HORSE_FOOD_3)
			{
				if (IsDead())
				{
					from->ChatPacket (CHAT_TYPE_INFO, "[LS;453]");
					return false;
				}
				return true;
			}
			else if (item->GetVnum() == ITEM_HORSE_FOOD_1 || item->GetVnum() == ITEM_HORSE_FOOD_2)
			{
				return false;
			}
			break;
	}

	//if (IS_SET(item->GetFlag(), ITEM_FLAG_QUEST_GIVE))
	{
		return true;
	}

	return false;
}

void CHARACTER::ReceiveItem (LPCHARACTER from, LPITEM item)
{
	if (IsPC())
	{
		return;
	}

	switch (GetRaceNum())
	{
		case fishing::CAMPFIRE_MOB:
			if (item->GetType() == ITEM_FISH && (item->GetSubType() == FISH_ALIVE || item->GetSubType() == FISH_DEAD))
			{
				fishing::Grill (from, item);
			}
			else
			{
				// TAKE_ITEM_BUG_FIX
				from->SetQuestNPCID (GetVID());
				// END_OF_TAKE_ITEM_BUG_FIX
				quest::CQuestManager::instance().TakeItem (from->GetPlayerID(), GetRaceNum(), item);
			}
			break;

		// DEVILTOWER_NPC
		case DEVILTOWER_BLACKSMITH_WEAPON_MOB:
		case DEVILTOWER_BLACKSMITH_ARMOR_MOB:
		case DEVILTOWER_BLACKSMITH_ACCESSORY_MOB:
			if (item->GetRefinedVnum() != 0 && item->GetRefineSet() != 0 && item->GetRefineSet() < 500)
			{
				from->SetRefineNPC (this);
				from->RefineInformation (item->GetCell(), REFINE_TYPE_MONEY_ONLY);
			}
			else
			{
				from->ChatPacket (CHAT_TYPE_INFO, "[LS;1002]");
			}
			break;
		// END_OF_DEVILTOWER_NPC

		case BLACKSMITH_MOB:
		case BLACKSMITH2_MOB:
		case BLACKSMITH_WEAPON_MOB:
		case BLACKSMITH_ARMOR_MOB:
		case BLACKSMITH_ACCESSORY_MOB:
			if (item->GetRefinedVnum())
			{
				from->SetRefineNPC (this);
				from->RefineInformation (item->GetCell(), REFINE_TYPE_NORMAL);
			}
			else
			{
				from->ChatPacket (CHAT_TYPE_INFO, "[LS;1002]");
			}
			break;

		case 20101:
		case 20102:
		case 20103:
		case 20104:
		case 20105:
		case 20106:
		case 20107:
		case 20108:
		case 20109:
			if (item->GetVnum() == ITEM_REVIVE_HORSE_1 ||
					item->GetVnum() == ITEM_REVIVE_HORSE_2 ||
					item->GetVnum() == ITEM_REVIVE_HORSE_3)
			{
				from->ReviveHorse();
				item->SetCount (item->GetCount()-1);
				from->ChatPacket (CHAT_TYPE_INFO, "[LS;454]");
			}
			else if (item->GetVnum() == ITEM_HORSE_FOOD_1 ||
					 item->GetVnum() == ITEM_HORSE_FOOD_2 ||
					 item->GetVnum() == ITEM_HORSE_FOOD_3)
			{
				from->FeedHorse();
				from->ChatPacket (CHAT_TYPE_INFO, "[LS;455]");
				item->SetCount (item->GetCount()-1);
				EffectPacket (SE_HPUP_RED);
			}
			break;

		default:
			sys_log (0, "TakeItem %s %d %s", from->GetName(), GetRaceNum(), item->GetName());
			from->SetQuestNPCID (GetVID());
			quest::CQuestManager::instance().TakeItem (from->GetPlayerID(), GetRaceNum(), item);
			break;
	}
}

bool CHARACTER::IsEquipUniqueItem (DWORD dwItemVnum) const
{
	{
		LPITEM u = GetWear (WEAR_UNIQUE1);

		if (u && u->GetVnum() == dwItemVnum)
		{
			return true;
		}
	}

	{
		LPITEM u = GetWear (WEAR_UNIQUE2);

		if (u && u->GetVnum() == dwItemVnum)
		{
			return true;
		}
	}


#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
	{
		LPITEM u = GetWear(WEAR_COSTUME_MOUNT);
		if (u && u->GetVnum() == dwItemVnum)
			return true;
	}
#endif

	// 언어반지인 경우 언어반지(견본) 인지도 체크한다.
	if (dwItemVnum == UNIQUE_ITEM_RING_OF_LANGUAGE)
	{
		return IsEquipUniqueItem (UNIQUE_ITEM_RING_OF_LANGUAGE_SAMPLE);
	}

	return false;
}

// CHECK_UNIQUE_GROUP
bool CHARACTER::IsEquipUniqueGroup (DWORD dwGroupVnum) const
{
	{
		LPITEM u = GetWear (WEAR_UNIQUE1);

		if (u && u->GetSpecialGroup() == (int) dwGroupVnum)
		{
			return true;
		}
	}

	{
		LPITEM u = GetWear (WEAR_UNIQUE2);

		if (u && u->GetSpecialGroup() == (int) dwGroupVnum)
		{
			return true;
		}
	}

#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
	{
		LPITEM u = GetWear(WEAR_COSTUME_MOUNT);
		if (u && u->GetSpecialGroup() == (int)dwGroupVnum)
			return true;
	}
#endif

	return false;
}
// END_OF_CHECK_UNIQUE_GROUP

void CHARACTER::SetRefineMode (int iAdditionalCell)
{
	m_iRefineAdditionalCell = iAdditionalCell;
	m_bUnderRefine = true;
}

void CHARACTER::ClearRefineMode()
{
	m_bUnderRefine = false;
	SetRefineNPC (NULL);
}

bool CHARACTER::GiveItemFromSpecialItemGroup (DWORD dwGroupNum, std::vector<DWORD>& dwItemVnums,
		std::vector<DWORD>& dwItemCounts, std::vector <LPITEM>& item_gets, int& count)
{
	const CSpecialItemGroup* pGroup = ITEM_MANAGER::instance().GetSpecialItemGroup (dwGroupNum);

	if (!pGroup)
	{
		sys_err ("cannot find special item group %d", dwGroupNum);
		return false;
	}

	std::vector <int> idxes;
	int n = pGroup->GetMultiIndex (idxes);

	bool bSuccess;

	for (int i = 0; i < n; i++)
	{
		bSuccess = false;
		int idx = idxes[i];
		DWORD dwVnum = pGroup->GetVnum (idx);
		DWORD dwCount = pGroup->GetCount (idx);
		int	iRarePct = pGroup->GetRarePct (idx);
		LPITEM item_get = NULL;
		switch (dwVnum)
		{
			case CSpecialItemGroup::GOLD:
				PointChange (POINT_GOLD, dwCount);
				LogManager::instance().CharLog (this, dwCount, "TREASURE_GOLD", "");

				bSuccess = true;
				break;
			case CSpecialItemGroup::EXP:
			{
				PointChange (POINT_EXP, dwCount);
				LogManager::instance().CharLog (this, dwCount, "TREASURE_EXP", "");

				bSuccess = true;
			}
			break;

			case CSpecialItemGroup::MOB:
			{
				sys_log (0, "CSpecialItemGroup::MOB %d", dwCount);
				int x = GetX() + number (-500, 500);
				int y = GetY() + number (-500, 500);

				LPCHARACTER ch = CHARACTER_MANAGER::instance().SpawnMob (dwCount, GetMapIndex(), x, y, 0, true, -1);
				if (ch)
				{
					ch->SetAggressive();
				}
				bSuccess = true;
			}
			break;
			case CSpecialItemGroup::SLOW:
			{
				sys_log (0, "CSpecialItemGroup::SLOW %d", - (int)dwCount);
				AddAffect (AFFECT_SLOW, POINT_MOV_SPEED, - (int)dwCount, AFF_SLOW, 300, 0, true);
				bSuccess = true;
			}
			break;
			case CSpecialItemGroup::DRAIN_HP:
			{
				int iDropHP = GetMaxHP()*dwCount/100;
				sys_log (0, "CSpecialItemGroup::DRAIN_HP %d", -iDropHP);
				iDropHP = MIN (iDropHP, GetHP()-1);
				sys_log (0, "CSpecialItemGroup::DRAIN_HP %d", -iDropHP);
				PointChange (POINT_HP, -iDropHP);
				bSuccess = true;
			}
			break;
			case CSpecialItemGroup::POISON:
			{
				AttackedByPoison (NULL);
				bSuccess = true;
			}
			break;

			case CSpecialItemGroup::MOB_GROUP:
			{
				int sx = GetX() - number (300, 500);
				int sy = GetY() - number (300, 500);
				int ex = GetX() + number (300, 500);
				int ey = GetY() + number (300, 500);
				CHARACTER_MANAGER::instance().SpawnGroup (dwCount, GetMapIndex(), sx, sy, ex, ey, NULL, true);

				bSuccess = true;
			}
			break;
			default:
			{
				item_get = AutoGiveItem (dwVnum, dwCount, iRarePct);

				if (item_get)
				{
					bSuccess = true;
				}
			}
			break;
		}

		if (bSuccess)
		{
			dwItemVnums.push_back (dwVnum);
			dwItemCounts.push_back (dwCount);
			item_gets.push_back (item_get);
			count++;

		}
		else
		{
			return false;
		}
	}
	return bSuccess;
}

// NEW_HAIR_STYLE_ADD
bool CHARACTER::ItemProcess_Hair (LPITEM item, int iDestCell)
{
	if (item->CheckItemUseLevel (GetLevel()) == false)
	{
		// 레벨 제한에 걸림
		ChatPacket (CHAT_TYPE_INFO, "[LS;456]");
		return false;
	}

	DWORD hair = item->GetVnum();

	switch (GetJob())
	{
		case JOB_WARRIOR :
			hair -= 72000; // 73001 - 72000 = 1001 부터 헤어 번호 시작
			break;

		case JOB_ASSASSIN :
			hair -= 71250;
			break;

		case JOB_SURA :
			hair -= 70500;
			break;

		case JOB_SHAMAN :
			hair -= 69750;
			break;

		default :
			return false;
			break;
	}

	if (hair == GetPart (PART_HAIR))
	{
		ChatPacket (CHAT_TYPE_INFO, "[LS;457]");
		return true;
	}

	item->SetCount (item->GetCount() - 1);

	SetPart (PART_HAIR, hair);
	UpdatePacket();

	return true;
}
// END_NEW_HAIR_STYLE_ADD

bool CHARACTER::ItemProcess_Polymorph (LPITEM item)
{
	if (IsPolymorphed())
	{
		ChatPacket (CHAT_TYPE_INFO, "[LS;458]");
		return false;
	}

	if (true == IsRiding())
	{
		ChatPacket (CHAT_TYPE_INFO, "[LS;1053]");
		return false;
	}

	DWORD dwVnum = item->GetSocket (0);

	if (dwVnum == 0)
	{
		ChatPacket (CHAT_TYPE_INFO, "[LS;460]");
		item->SetCount (item->GetCount()-1);
		return false;
	}

	const CMob* pMob = CMobManager::instance().Get (dwVnum);

	if (pMob == NULL)
	{
		ChatPacket (CHAT_TYPE_INFO, "[LS;460]");
		item->SetCount (item->GetCount()-1);
		return false;
	}

	switch (item->GetVnum())
	{
		case 70104 :
		case 70105 :
		case 70106 :
		case 70107 :
		case 71093 :
		{
			// 둔갑구 처리
			sys_log (0, "USE_POLYMORPH_BALL PID(%d) vnum(%d)", GetPlayerID(), dwVnum);

			// 레벨 제한 체크
			int iPolymorphLevelLimit = MAX (0, 20 - GetLevel() * 3 / 10);
			if (pMob->m_table.bLevel >= GetLevel() + iPolymorphLevelLimit)
			{
				ChatPacket (CHAT_TYPE_INFO, "[LS;461]");
				return false;
			}

			int iDuration = GetSkillLevel (POLYMORPH_SKILL_ID) == 0 ? 5 : (5 + (5 + GetSkillLevel (POLYMORPH_SKILL_ID)/40 * 25));
			iDuration *= 60;

			DWORD dwBonus = 0;

			dwBonus = (2 + GetSkillLevel (POLYMORPH_SKILL_ID)/40) * 100;

			AddAffect (AFFECT_POLYMORPH, POINT_POLYMORPH, dwVnum, AFF_POLYMORPH, iDuration, 0, true);
			AddAffect (AFFECT_POLYMORPH, POINT_ATT_BONUS, dwBonus, AFF_POLYMORPH, iDuration, 0, false);

			item->SetCount (item->GetCount()-1);
		}
		break;

		case 50322:
		{
			// 보류

			// 둔갑서 처리
			// 소켓0                소켓1           소켓2
			// 둔갑할 몬스터 번호   수련정도        둔갑서 레벨
			sys_log (0, "USE_POLYMORPH_BOOK: %s(%u) vnum(%u)", GetName(), GetPlayerID(), dwVnum);

			if (CPolymorphUtils::instance().PolymorphCharacter (this, item, pMob) == true)
			{
				CPolymorphUtils::instance().UpdateBookPracticeGrade (this, item);
			}
			else
			{
			}
		}
		break;

		default :
			sys_err ("POLYMORPH invalid item passed PID(%d) vnum(%d)", GetPlayerID(), item->GetOriginalVnum());
			return false;
	}

	return true;
}

bool CHARACTER::CanDoCube() const
{
	if (m_bIsObserver)
	{
		return false;
	}
	if (GetShop())
	{
		return false;
	}
	if (GetMyShop())
	{
		return false;
	}
	if (m_bUnderRefine)
	{
		return false;
	}
	if (IsWarping())
	{
		return false;
	}
#ifdef __AURA_SYSTEM__
	if (IsAuraRefineWindowOpen())	return false;
#endif
	return true;
}

bool CHARACTER::UnEquipSpecialRideUniqueItem()
{
	LPITEM Unique1 = GetWear (WEAR_UNIQUE1);
	LPITEM Unique2 = GetWear (WEAR_UNIQUE2);
#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
	LPITEM Unique3 = GetWear(WEAR_COSTUME_MOUNT);
#endif
	if (NULL != Unique1)
	{
		if (UNIQUE_GROUP_SPECIAL_RIDE == Unique1->GetSpecialGroup())
		{
			return UnequipItem (Unique1);
		}
	}

	if (NULL != Unique2)
	{
		if (UNIQUE_GROUP_SPECIAL_RIDE == Unique2->GetSpecialGroup())
		{
			return UnequipItem (Unique2);
		}
	}

#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
	if (NULL != Unique3)
	{
		if (UNIQUE_GROUP_SPECIAL_RIDE == Unique3->GetSpecialGroup())
		{
			return UnequipItem(Unique3);
		}
	}
#endif

	return true;
}

void CHARACTER::AutoRecoveryItemProcess (const EAffectTypes type)
{
	if (true == IsDead() || true == IsStun())
	{
		return;
	}

	if (false == IsPC())
	{
		return;
	}

	if (AFFECT_AUTO_HP_RECOVERY != type && AFFECT_AUTO_SP_RECOVERY != type)
	{
		return;
	}

	if (NULL != FindAffect (AFFECT_STUN))
	{
		return;
	}

	{
		const DWORD stunSkills[] = { SKILL_TANHWAN, SKILL_GEOMPUNG, SKILL_BYEURAK, SKILL_GIGUNG };

		for (size_t i=0 ; i < sizeof (stunSkills)/sizeof (DWORD) ; ++i)
		{
			const CAffect* p = FindAffect (stunSkills[i]);

			if (NULL != p && AFF_STUN == p->dwFlag)
			{
				return;
			}
		}
	}

	const CAffect* pAffect = FindAffect (type);
	const size_t idx_of_amount_of_used = 1;
	const size_t idx_of_amount_of_full = 2;

	if (NULL != pAffect)
	{
		LPITEM pItem = FindItemByID (pAffect->dwFlag);

		if (NULL != pItem && true == pItem->GetSocket (0))
		{
			if (false == CArenaManager::instance().IsArenaMap (GetMapIndex()))
			{
				const long amount_of_used = pItem->GetSocket (idx_of_amount_of_used);
				const long amount_of_full = pItem->GetSocket (idx_of_amount_of_full);

				const int32_t avail = amount_of_full - amount_of_used;

				int32_t amount = 0;

				if (AFFECT_AUTO_HP_RECOVERY == type)
				{
					amount = GetMaxHP() - (GetHP() + GetPoint (POINT_HP_RECOVERY));
				}
				else if (AFFECT_AUTO_SP_RECOVERY == type)
				{
					amount = GetMaxSP() - (GetSP() + GetPoint (POINT_SP_RECOVERY));
				}

				if (amount > 0)
				{
					if (avail > amount)
					{
						const int pct_of_used = amount_of_used * 100 / amount_of_full;
						const int pct_of_will_used = (amount_of_used + amount) * 100 / amount_of_full;

						bool bLog = false;
						// 사용량의 10% 단위로 로그를 남김
						// (사용량의 %에서, 십의 자리가 바뀔 때마다 로그를 남김.)
						if ((pct_of_will_used / 10) - (pct_of_used / 10) >= 1)
						{
							bLog = true;
						}
						pItem->SetSocket (idx_of_amount_of_used, amount_of_used + amount, bLog);
					}
					else
					{
						amount = avail;

						ITEM_MANAGER::instance().RemoveItem (pItem);
					}

					if (AFFECT_AUTO_HP_RECOVERY == type)
					{
						PointChange (POINT_HP_RECOVERY, amount);
						EffectPacket (SE_AUTO_HPUP);
					}
					else if (AFFECT_AUTO_SP_RECOVERY == type)
					{
						PointChange (POINT_SP_RECOVERY, amount);
						EffectPacket (SE_AUTO_SPUP);
					}
				}
			}
			else
			{
				pItem->Lock (false);
				pItem->SetSocket (0, false);
				RemoveAffect (const_cast<CAffect*> (pAffect));
			}
		}
		else
		{
			RemoveAffect (const_cast<CAffect*> (pAffect));
		}
	}
}

bool CHARACTER::IsValidItemPosition (TItemPos Pos) const
{
	BYTE window_type = Pos.window_type;
	WORD cell = Pos.cell;

	switch (window_type)
	{
		case RESERVED_WINDOW:
			return false;

		case INVENTORY:
		case EQUIPMENT:
			return cell < (INVENTORY_AND_EQUIP_SLOT_MAX);

		case DRAGON_SOUL_INVENTORY:
			return cell < (DRAGON_SOUL_INVENTORY_MAX_NUM);

		case SAFEBOX:
			if (NULL != m_pkSafebox)
			{
				return m_pkSafebox->IsValidPosition (cell);
			}
			else
			{
				return false;
			}

		case MALL:
			if (NULL != m_pkMall)
			{
				return m_pkMall->IsValidPosition (cell);
			}
			else
			{
				return false;
			}
		default:
			return false;
	}
}


// 귀찮아서 만든 매크로.. exp가 true면 msg를 출력하고 return false 하는 매크로 (일반적인 verify 용도랑은 return 때문에 약간 반대라 이름때문에 헷갈릴 수도 있겠다..)
#define VERIFY_MSG(exp, msg)  \
	if (true == (exp)) { \
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT(msg)); \
			return false; \
	}


/// 현재 캐릭터의 상태를 바탕으로 주어진 item을 착용할 수 있는 지 확인하고, 불가능 하다면 캐릭터에게 이유를 알려주는 함수
bool CHARACTER::CanEquipNow (const LPITEM item, const TItemPos& srcCell, const TItemPos& destCell) /*const*/
{
	const TItemTable* itemTable = item->GetProto();
	BYTE itemType = item->GetType();
	BYTE itemSubType = item->GetSubType();

	switch (GetJob())
	{
		case JOB_WARRIOR:
			if (item->GetAntiFlag() & ITEM_ANTIFLAG_WARRIOR)
			{
				return false;
			}
			break;

		case JOB_ASSASSIN:
			if (item->GetAntiFlag() & ITEM_ANTIFLAG_ASSASSIN)
			{
				return false;
			}
			break;

		case JOB_SHAMAN:
			if (item->GetAntiFlag() & ITEM_ANTIFLAG_SHAMAN)
			{
				return false;
			}
			break;

		case JOB_SURA:
			if (item->GetAntiFlag() & ITEM_ANTIFLAG_SURA)
			{
				return false;
			}
			break;
	}

	for (int i = 0; i < ITEM_LIMIT_MAX_NUM; ++i)
	{
		long limit = itemTable->aLimits[i].lValue;
		switch (itemTable->aLimits[i].bType)
		{
			case LIMIT_LEVEL:
				if (GetLevel() < limit)
				{
					ChatPacket (CHAT_TYPE_INFO, "[LS;462]");
					return false;
				}
				break;

			case LIMIT_STR:
				if (GetPoint (POINT_ST) < limit)
				{
					ChatPacket (CHAT_TYPE_INFO, "[LS;463]");
					return false;
				}
				break;

			case LIMIT_INT:
				if (GetPoint (POINT_IQ) < limit)
				{
					ChatPacket (CHAT_TYPE_INFO, "[LS;464]");
					return false;
				}
				break;

			case LIMIT_DEX:
				if (GetPoint (POINT_DX) < limit)
				{
					ChatPacket (CHAT_TYPE_INFO, "[LS;465]");
					return false;
				}
				break;

			case LIMIT_CON:
				if (GetPoint (POINT_HT) < limit)
				{
					ChatPacket (CHAT_TYPE_INFO, "[LS;466]");
					return false;
				}
				break;
		}
	}

	if (item->GetWearFlag() & WEARABLE_UNIQUE)
	{
		if ((GetWear (WEAR_UNIQUE1) && GetWear (WEAR_UNIQUE1)->IsSameSpecialGroup (item)) ||
				(GetWear (WEAR_UNIQUE2) && GetWear (WEAR_UNIQUE2)->IsSameSpecialGroup (item)))
		{
			ChatPacket (CHAT_TYPE_INFO, "[LS;468]");
			return false;
		}

		if (marriage::CManager::instance().IsMarriageUniqueItem (item->GetVnum()) &&
				!marriage::CManager::instance().IsMarried (GetPlayerID()))
		{
			ChatPacket (CHAT_TYPE_INFO, "[LS;467]");
			return false;
		}

	}

#ifdef ENABLE_DS_SET
	if ((DragonSoul_IsDeckActivated()) && (item->IsDragonSoul())) {
		ChatPacket (CHAT_TYPE_INFO, "[LS;467]");
		return false;
	}
#endif

	return true;
}

/// 현재 캐릭터의 상태를 바탕으로 착용 중인 item을 벗을 수 있는 지 확인하고, 불가능 하다면 캐릭터에게 이유를 알려주는 함수
bool CHARACTER::CanUnequipNow(const LPITEM item, const TItemPos& srcCell, const TItemPos& destCell)
{
	if (ITEM_BELT == item->GetType())
		VERIFY_MSG(CBeltInventoryHelper::IsExistItemInBeltInventory(this), "벨트 인벤토리에 아이템이 존재하면 해제할 수 없습니다.");
#ifdef __AURA_SYSTEM__
	if (IsAuraRefineWindowOpen())
		return false;
#endif
	// 영원히 해제할 수 없는 아이템
	if (IS_SET(item->GetFlag(), ITEM_FLAG_IRREMOVABLE))
		return false;
#ifdef ENABLE_DS_SET
	if ((DragonSoul_IsDeckActivated()) && (item->IsDragonSoul())) {
		ChatPacket (CHAT_TYPE_INFO, "[LS;467]");
		return false;
	}
#endif
	// 아이템 unequip시 인벤토리로 옮길 때 빈 자리가 있는 지 확인
	{
		int pos = -1;
		if (item->IsDragonSoul())
			pos = GetEmptyDragonSoulInventory(item);
		else
			pos = GetEmptyInventory(item->GetSize());
		
		VERIFY_MSG( -1 == pos, "소지품에 빈 공간이 없습니다." );
	}
	return true;
}

#if defined(__BL_MOVE_COSTUME_ATTR__)
void CHARACTER::OpenItemComb()
{
	if (IsItemComb() || GetExchange() || IsOpenSafebox() || GetShopOwner() || GetMyShop() || IsCubeOpen())
	{
		ChatPacket(CHAT_TYPE_INFO, "You have to close other windows.");
		return;
	}
	
	const LPCHARACTER npc = GetQuestNPC();
	if (npc == NULL)
	{
		sys_err("Item Combination NPC is NULL (ch: %s)", GetName());
		return;
	}

	SetItemCombNpc(npc);
	ChatPacket(CHAT_TYPE_COMMAND, "ShowItemCombinationDialog");
}

void CHARACTER::ItemCombination(const short MediumIndex, const short BaseIndex, const short MaterialIndex)
{
	if (IsItemComb() == false)
		return;
	
	const LPITEM MediumItem		= GetItem(TItemPos(INVENTORY, MediumIndex));
	const LPITEM BaseItem		= GetItem(TItemPos(INVENTORY, BaseIndex));
	const LPITEM MaterialItem	= GetItem(TItemPos(INVENTORY, MaterialIndex));

	if (MediumItem == NULL || BaseItem == NULL || MaterialItem == NULL)
		return;

	switch (MediumItem->GetType())
	{
	case EItemTypes::ITEM_MEDIUM:
		switch (MediumItem->GetSubType())
		{
		case EMediumSubTypes::MEDIUM_MOVE_COSTUME_ATTR:
			break;
		/*case EMediumSubTypes::MEDIUM_MOVE_ACCE_ATTR:
			break;*/
		default:
			return;
		}
		break;
	default:
		return;
	}

	if (BaseItem->IsEquipped() || MaterialItem->IsEquipped())
		return;

	if (BaseItem->GetType() != EItemTypes::ITEM_COSTUME || MaterialItem->GetType() != EItemTypes::ITEM_COSTUME)
		return;
	
	if (BaseItem->GetSubType() != MaterialItem->GetSubType())
		return;

	if (BaseItem->GetAttributeCount() < 1 || MaterialItem->GetAttributeCount() < 1)
		return;

	BaseItem->SetAttributes(MaterialItem->GetAttributes());
	BaseItem->UpdatePacket();

	ITEM_MANAGER::instance().RemoveItem(MaterialItem, "REMOVE (Item Combination)");
	MediumItem->SetCount(MediumItem->GetCount() - 1);
}
#endif