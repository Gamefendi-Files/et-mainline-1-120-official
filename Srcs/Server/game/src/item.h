#ifndef __INC_METIN_II_GAME_ITEM_H__
#define __INC_METIN_II_GAME_ITEM_H__

#include "entity.h"

class CItem : public CEntity
{
	protected:
		// override methods from ENTITY class
		virtual void	EncodeInsertPacket (LPENTITY entity);
		virtual void	EncodeRemovePacket (LPENTITY entity);

	public:
		CItem (DWORD dwVnum);
		virtual ~CItem();

		int			GetLevelLimit();

		bool		CheckItemUseLevel (int nLevel);

		long		FindApplyValue (BYTE bApplyType);

		bool		IsStackable()
		{
			return (GetFlag() & ITEM_FLAG_STACKABLE)?true:false;
		}

		void		Initialize();
		void		Destroy();

		void		Save();

		void		SetWindow (BYTE b)
		{
			m_bWindow = b;
		}
		BYTE		GetWindow()
		{
			return m_bWindow;
		}

		void		SetID (DWORD id)
		{
			m_dwID = id;
		}
		DWORD		GetID()
		{
			return m_dwID;
		}

		void			SetProto (const TItemTable* table);
		TItemTable const* 	GetProto()
		{
			return m_pProto;
		}

		int		GetGold();
		int		GetShopBuyPrice();
		const char* 	GetName()
		{
			return m_pProto ? m_pProto->szLocaleName : NULL;
		}
		const char* 	GetBaseName()
		{
			return m_pProto ? m_pProto->szName : NULL;
		}
		BYTE		GetSize()
		{
			return m_pProto ? m_pProto->bSize : 0;
		}

		void		SetFlag (long flag)
		{
			m_lFlag = flag;
		}
		long		GetFlag()
		{
			return m_lFlag;
		}

		void		AddFlag (long bit);
		void		RemoveFlag (long bit);

		DWORD		GetWearFlag()
		{
			return m_pProto ? m_pProto->dwWearFlags : 0;
		}
		DWORD		GetAntiFlag()
		{
			return m_pProto ? m_pProto->dwAntiFlags : 0;
		}
		DWORD		GetImmuneFlag()
		{
			return m_pProto ? m_pProto->dwImmuneFlag : 0;
		}

		void		SetVID (DWORD vid)
		{
			m_dwVID = vid;
		}
		DWORD		GetVID()
		{
			return m_dwVID;
		}

		bool		SetCount (DWORD count);
		DWORD		GetCount();

		// GetVnum과 GetOriginalVnum에 대한 comment
		// GetVnum은 Masking 된 Vnum이다. 이를 사용함으로써, 아이템의 실제 Vnum은 10이지만, Vnum이 20인 것처럼 동작할 수 있는 것이다.
		// Masking 값은 ori_to_new.txt에서 정의된 값이다.
		// GetOriginalVnum은 아이템 고유의 Vnum으로, 로그 남길 때, 클라이언트에 아이템 정보 보낼 때, 저장할 때는 이 Vnum을 사용하여야 한다.
		//
		DWORD		GetVnum() const
		{
			return m_dwMaskVnum ? m_dwMaskVnum : m_dwVnum;
		}
		DWORD		GetOriginalVnum() const
		{
			return m_dwVnum;
		}
		BYTE		GetType() const
		{
			return m_pProto ? m_pProto->bType : 0;
		}
		BYTE		GetSubType() const
		{
			return m_pProto ? m_pProto->bSubType : 0;
		}
		BYTE		GetLimitType (DWORD idx) const
		{
			return m_pProto ? m_pProto->aLimits[idx].bType : 0;
		}
		long		GetLimitValue (DWORD idx) const
		{
			return m_pProto ? m_pProto->aLimits[idx].lValue : 0;
		}

#if defined(__BL_TRANSMUTATION__)
	public:
		// Weapon
		bool IsWeapon() { return GetType() == ITEM_WEAPON; }
		bool IsMainWeapon()
		{
			return GetType() == ITEM_WEAPON && (
				GetSubType() == WEAPON_SWORD
				|| GetSubType() == WEAPON_DAGGER
				|| GetSubType() == WEAPON_BOW
				|| GetSubType() == WEAPON_TWO_HANDED
				|| GetSubType() == WEAPON_BELL
				|| GetSubType() == WEAPON_FAN
				);
		}
		bool IsSword() { return GetType() == ITEM_WEAPON && GetSubType() == WEAPON_SWORD; }
		bool IsDagger() { return GetType() == ITEM_WEAPON && GetSubType() == WEAPON_DAGGER; }
		bool IsBow() { return GetType() == ITEM_WEAPON && GetSubType() == WEAPON_BOW; }
		bool IsTwoHandSword() { return GetType() == ITEM_WEAPON && GetSubType() == WEAPON_TWO_HANDED; }
		bool IsBell() { return GetType() == ITEM_WEAPON && GetSubType() == WEAPON_BELL; }
		bool IsFan() { return GetType() == ITEM_WEAPON && GetSubType() == WEAPON_FAN; }
		bool IsArrow() { return GetType() == ITEM_WEAPON && GetSubType() == WEAPON_ARROW; }
		bool IsMountSpear() { return GetType() == ITEM_WEAPON && GetSubType() == WEAPON_MOUNT_SPEAR; }

		// Armor
		bool IsArmor() { return GetType() == ITEM_ARMOR; }
		bool IsArmorBody() { return GetType() == ITEM_ARMOR && GetSubType() == ARMOR_BODY; }
		bool IsHelmet() { return GetType() == ITEM_ARMOR && GetSubType() == ARMOR_HEAD; }
		bool IsShield() { return GetType() == ITEM_ARMOR && GetSubType() == ARMOR_SHIELD; }
		bool IsWrist() { return GetType() == ITEM_ARMOR && GetSubType() == ARMOR_WRIST; }
		bool IsShoe() { return GetType() == ITEM_ARMOR && GetSubType() == ARMOR_FOOTS; }
		bool IsNecklace() { return GetType() == ITEM_ARMOR && GetSubType() == ARMOR_NECK; }
		bool IsEarRing() { return GetType() == ITEM_ARMOR && GetSubType() == ARMOR_EAR; }

		// Costume
		bool IsCostume() { return GetType() == ITEM_COSTUME; }
		bool IsCostumeHair() { return GetType() == ITEM_COSTUME && GetSubType() == COSTUME_HAIR; }
		bool IsCostumeBody() { return GetType() == ITEM_COSTUME && GetSubType() == COSTUME_BODY; }
		bool IsCostumeMount() { return GetType() == ITEM_COSTUME && GetSubType() == COSTUME_MOUNT; }
		bool IsCostumeWeapon() { return GetType() == ITEM_COSTUME && GetSubType() == COSTUME_WEAPON; }

#endif

		long		GetValue (DWORD idx);

		void		SetCell (LPCHARACTER ch, WORD pos)
		{
			m_pOwner = ch, m_wCell = pos;
		}
		WORD		GetCell()
		{
			return m_wCell;
		}
#ifdef ENABLE_ADDITIONAL_EQUIPMENT_PAGE
		void		StartEquipEvent();
		void		StopEquipEvent();
#endif
		LPITEM		RemoveFromCharacter();
#if defined(__BL_ENABLE_PICKUP_ITEM_EFFECT__)
		bool	AddToCharacter(LPCHARACTER ch, TItemPos Cell, bool bHighlight = true);
#else
		bool	AddToCharacter(LPCHARACTER ch, TItemPos Cell);
#endif
		LPCHARACTER	GetOwner()
		{
			return m_pOwner;
		}

		LPITEM		RemoveFromGround();
		bool		AddToGround (long lMapIndex, const PIXEL_POSITION& pos, bool skipOwnerCheck = false);

		int			FindEquipCell (LPCHARACTER ch, int bCandidateCell = -1);
		bool		IsEquipped() const
		{
			return m_bEquipped;
		}
		bool		EquipTo (LPCHARACTER ch, BYTE bWearCell);
		bool		IsEquipable() const;

		bool		CanUsedBy (LPCHARACTER ch);

		bool		DistanceValid (LPCHARACTER ch);

		void		UpdatePacket();
		void		UsePacketEncode (LPCHARACTER ch, LPCHARACTER victim, struct packet_item_use* packet);

		void		SetExchanging (bool isOn = true);
		bool		IsExchanging()
		{
			return m_bExchanging;
		}

		bool		IsTwohanded();

		bool		IsPolymorphItem();
#if defined(__BL_OFFICIAL_LOOT_FILTER__)
		long		GetWearingLevelLimit() const;
		bool		IsHairDye() const;
#endif
		void		ModifyPoints (bool bAdd);	// 아이템의 효과를 캐릭터에 부여 한다. bAdd가 false이면 제거함

		bool		CreateSocket (BYTE bSlot, BYTE bGold);
		const long* 	GetSockets()
		{
			return &m_alSockets[0];
		}
		long		GetSocket (int i)
		{
			return m_alSockets[i];
		}

		void		SetSockets (const long* al);
		void		SetSocket (int i, long v, bool bLog = true);

		int		GetSocketCount();
		bool		AddSocket();

		const TPlayerItemAttribute* GetAttributes()
		{
			return m_aAttr;
		}
		const TPlayerItemAttribute& GetAttribute (int i)
		{
			return m_aAttr[i];
		}

#if defined(__ITEM_APPLY_RANDOM__)
		const TPlayerItemAttribute* GetRandomApplies() { return m_aApplyRandom; }
		const TPlayerItemAttribute& GetRandomApply(BYTE i) { return m_aApplyRandom[i]; }
		TPlayerItemAttribute* GetNextRandomApplies();

		void SetRandomApplies(const TPlayerItemAttribute* c_pApplyRandom);

		BYTE GetRandomApplyType(BYTE i) { return m_aApplyRandom[i].bType; }
		short GetRandomApplyValue(BYTE i) { return m_aApplyRandom[i].sValue; }
		BYTE GetRandomApplyPath(BYTE i) { return m_aApplyRandom[i].bPath; }
#endif

		BYTE		GetAttributeType (int i)
		{
			return m_aAttr[i].bType;
		}
		short		GetAttributeValue (int i)
		{
			return m_aAttr[i].sValue;
		}

		void		SetAttributes (const TPlayerItemAttribute* c_pAttribute);

		int		FindAttribute (BYTE bType);
		bool		RemoveAttributeAt (int index);
		bool		RemoveAttributeType (BYTE bType);

		bool		HasAttr (BYTE bApply);
		bool		HasRareAttr (BYTE bApply);

		void		SetDestroyEvent (LPEVENT pkEvent);
		void		StartDestroyEvent (int iSec=300);

		DWORD		GetRefinedVnum()
		{
			return m_pProto ? m_pProto->dwRefinedVnum : 0;
		}
		DWORD		GetRefineFromVnum();
		int		GetRefineLevel();

		void		SetSkipSave (bool b)
		{
			m_bSkipSave = b;
		}
		bool		GetSkipSave()
		{
			return m_bSkipSave;
		}

		bool		IsOwnership (LPCHARACTER ch);
		void		SetOwnership (LPCHARACTER ch, int iSec = 10);
		void		SetOwnershipEvent (LPEVENT pkEvent);

		DWORD		GetLastOwnerPID()
		{
			return m_dwLastOwnerPID;
		}
#ifdef __AURA_SYSTEM__
		bool	IsAuraBoosterForSocket();

		void	StartAuraBoosterSocketExpireEvent();
		void	StopAuraBoosterSocketExpireEvent();
		void	SetAuraBoosterSocketExpireEvent(LPEVENT pkEvent);
#endif
		int		GetAttributeSetIndex(); // 속성 붙는것을 지정한 배열의 어느 인덱스를 사용하는지 돌려준다.
		void		AlterToMagicItem();
		void		AlterToSocketItem (int iSocketCount);

		WORD		GetRefineSet()
		{
			return m_pProto ? m_pProto->wRefineSet : 0;
		}

		void		StartUniqueExpireEvent();
		void		SetUniqueExpireEvent (LPEVENT pkEvent);

		void		StartTimerBasedOnWearExpireEvent();
		void		SetTimerBasedOnWearExpireEvent (LPEVENT pkEvent);

		void		StartRealTimeExpireEvent();
		bool		IsRealTimeItem();

		void		StopUniqueExpireEvent();
		void		StopTimerBasedOnWearExpireEvent();
		void		StopAccessorySocketExpireEvent();

		//			일단 REAL_TIME과 TIMER_BASED_ON_WEAR 아이템에 대해서만 제대로 동작함.
		int			GetDuration();

		int		GetAttributeCount();
		void		ClearAttribute();
		void ClearAllAttribute();
		void		ChangeAttribute (const int* aiChangeProb=NULL);
		void		AddAttribute();
		void		AddAttribute (BYTE bType, short sValue);
#if defined(__BL_TRANSMUTATION__)
		void		SetTransmutationVnum(DWORD blVnum);
		DWORD		GetTransmutationVnum() const;
#endif
		void		ApplyAddon (int iAddonType);

		int		GetSpecialGroup() const;
		bool	IsSameSpecialGroup (const LPITEM item) const;

		// ACCESSORY_REFINE
		// 액세서리에 광산을 통해 소켓을 추가
		bool		IsAccessoryForSocket();

		int		GetAccessorySocketGrade();
		int		GetAccessorySocketMaxGrade();
		int		GetAccessorySocketDownGradeTime();

		void		SetAccessorySocketGrade (int iGrade);
		void		SetAccessorySocketMaxGrade (int iMaxGrade);
		void		SetAccessorySocketDownGradeTime (DWORD time);

		void		AccessorySocketDegrade();

		// 악세사리 를 아이템에 밖았을때 타이머 돌아가는것( 구리, 등 )
		void		StartAccessorySocketExpireEvent();
		void		SetAccessorySocketExpireEvent (LPEVENT pkEvent);

		bool		CanPutInto (LPITEM item);
		// END_OF_ACCESSORY_REFINE

		void		CopyAttributeTo (LPITEM pItem);
		void		CopySocketTo (LPITEM pItem);

		int			GetRareAttrCount();
		bool		AddRareAttribute();
		bool		ChangeRareAttribute();

		void		AttrLog();

		void		Lock (bool f)
		{
			m_isLocked = f;
		}
		bool		isLocked() const
		{
			return m_isLocked;
		}

	private :
		void		SetAttribute (int i, BYTE bType, short sValue);
#if defined(__ITEM_APPLY_RANDOM__)
		void SetRandomApply(BYTE bIndex, BYTE bType, short sValue, BYTE bPath);
#endif
	public:
		void		SetForceAttribute (int i, BYTE bType, short sValue);
#if defined(__ITEM_APPLY_RANDOM__)
		void SetForceRandomApply(BYTE bIndex, BYTE bType, short sValue, BYTE bPath);
#endif
	protected:
		bool		EquipEx (bool is_equip);
		bool		Unequip();

		void		AddAttr (BYTE bApply, BYTE bLevel);
		void		PutAttribute (const int* aiAttrPercentTable);
		void		PutAttributeWithLevel (BYTE bLevel);

	protected:
		friend class CInputDB;
		bool		OnAfterCreatedItem();			// 서버상에 아이템이 모든 정보와 함께 완전히 생성(로드)된 후 불리우는 함수.

	public:
		bool		IsRideItem();
		bool		IsRamadanRing();

		void		ClearMountAttributeAndAffect();
		bool		IsNewMountItem();
#ifdef ENABLE_MOUNT_COSTUME_SYSTEM
		bool		IsMountItem();
#endif
#if defined(__PENDANT_SYSTEM__)
		bool IsPendant() { return GetType() == ITEM_ARMOR && GetSubType() == ARMOR_PENDANT; }
#endif
#if defined(__GLOVE_SYSTEM__)
		bool IsGlove() { return GetType() == ITEM_ARMOR && GetSubType() == ARMOR_GLOVE; }
#endif
		// 독일에서 기존 캐시 아이템과 같지만, 교환 가능한 캐시 아이템을 만든다고 하여,
		// 오리지널 아이템에, 교환 금지 플래그만 삭제한 새로운 아이템들을 새로운 아이템 대역에 할당하였다.
		// 문제는 새로운 아이템도 오리지널 아이템과 같은 효과를 내야하는데,
		// 서버건, 클라건, vnum 기반으로 되어있어
		// 새로운 vnum을 죄다 서버에 새로 다 박아야하는 안타까운 상황에 맞닿았다.
		// 그래서 새 vnum의 아이템이면, 서버에서 돌아갈 때는 오리지널 아이템 vnum으로 바꿔서 돌고 하고,
		// 저장할 때에 본래 vnum으로 바꿔주도록 한다.

		// Mask vnum은 어떤 이유(ex. 위의 상황)로 인해 vnum이 바뀌어 돌아가는 아이템을 위해 있다.
		void		SetMaskVnum (DWORD vnum)
		{
			m_dwMaskVnum = vnum;
		}
		DWORD		GetMaskVnum()
		{
			return m_dwMaskVnum;
		}
		bool		IsMaskedItem()
		{
			return m_dwMaskVnum != 0;
		}
#ifdef ENABLE_DS_CHANGE_ATTR
		bool		IsMythDragonSoul();
#endif
		// 용혼석
		bool		IsDragonSoul();
		int		GiveMoreTime_Per (float fPercent);
		int		GiveMoreTime_Fix (DWORD dwTime);

	private:
		TItemTable const* m_pProto;		// 프로토 타잎

		DWORD		m_dwVnum;
#if defined(__BL_TRANSMUTATION__)
		DWORD		m_dwTransmutationVnum;
#endif
		LPCHARACTER	m_pOwner;

		BYTE		m_bWindow;		// 현재 아이템이 위치한 윈도우
		DWORD		m_dwID;			// 고유번호
		bool		m_bEquipped;	// 장착 되었는가?
		DWORD		m_dwVID;		// VID
		WORD		m_wCell;		// 위치
		DWORD		m_dwCount;		// 개수
		long		m_lFlag;		// 추가 flag
		DWORD		m_dwLastOwnerPID;	// 마지막 가지고 있었던 사람의 PID

		bool		m_bExchanging;	///< 현재 교환중 상태

		long		m_alSockets[ITEM_SOCKET_MAX_NUM];	// 아이템 소캣
#if defined(__ITEM_APPLY_RANDOM__)
		TPlayerItemAttribute m_aApplyRandom[ITEM_APPLY_MAX_NUM];
#endif
		TPlayerItemAttribute	m_aAttr[ITEM_ATTRIBUTE_MAX_NUM];

		LPEVENT		m_pkDestroyEvent;
		LPEVENT		m_pkExpireEvent;
		LPEVENT		m_pkUniqueExpireEvent;
		LPEVENT		m_pkTimerBasedOnWearExpireEvent;
		LPEVENT		m_pkRealTimeExpireEvent;
		LPEVENT		m_pkAccessorySocketExpireEvent;
		LPEVENT		m_pkOwnershipEvent;
#ifdef __AURA_SYSTEM__
		LPEVENT		m_pkAuraBoostSocketExpireEvent;
#endif
		DWORD		m_dwOwnershipPID;

		bool		m_bSkipSave;

		bool		m_isLocked;

		DWORD		m_dwMaskVnum;
		DWORD		m_dwSIGVnum;
#ifdef ENABLE_ADDITIONAL_EQUIPMENT_PAGE
		bool		m_bEquipAlreadyStopped;
#endif
	public:
		void SetSIGVnum (DWORD dwSIG)
		{
			m_dwSIGVnum = dwSIG;
		}
		DWORD	GetSIGVnum() const
		{
			return m_dwSIGVnum;
		}
};

EVENTINFO (item_event_info)
{
	LPITEM item;
	char szOwnerName[CHARACTER_NAME_MAX_LEN];

	item_event_info()
		: item (0)
	{
		::memset (szOwnerName, 0, CHARACTER_NAME_MAX_LEN);
	}
};

EVENTINFO (item_vid_event_info)
{
	DWORD item_vid;

	item_vid_event_info()
		: item_vid (0)
	{
	}
};

#endif
