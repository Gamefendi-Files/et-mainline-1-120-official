#pragma once

#include "PythonWindow.h"
#include "../UserInterface/Locale_inc.h"

namespace UI
{
	enum
	{
		ITEM_WIDTH = 32,
		ITEM_HEIGHT = 32,
#if defined(__BL_ENABLE_PICKUP_ITEM_EFFECT__)
		SLOT_ACTIVE_EFFECT_COUNT = 3,
#endif
		SLOT_NUMBER_NONE = 0xffffffff,
	};

	enum ESlotStyle
	{
		SLOT_STYLE_NONE,
		SLOT_STYLE_PICK_UP,
		SLOT_STYLE_SELECT,
	};

#if defined(__BL_ENABLE_PICKUP_ITEM_EFFECT__)
	enum ESlotColorType
	{
		COLOR_TYPE_ORANGE,
		COLOR_TYPE_WHITE,
		COLOR_TYPE_RED,
		COLOR_TYPE_GREEN,
		COLOR_TYPE_YELLOW,
		COLOR_TYPE_SKY,
		COLOR_TYPE_PINK,
	};
	
	enum ESlotHilight
	{
		HILIGHTSLOT_ACCE,
		HILIGHTSLOT_CHANGE_LOOK,
		HILIGHTSLOT_AURA,
		HILIGHTSLOT_CUBE,

		HILIGHTSLOT_MAX
	};
#endif

	enum ESlotState
	{
		SLOT_STATE_LOCK		= (1 << 0),
		SLOT_STATE_CANT_USE	= (1 << 1),
		SLOT_STATE_DISABLE	= (1 << 2),
		SLOT_STATE_ALWAYS_RENDER_COVER = (1 << 3),			// 현재 Cover 버튼은 슬롯에 무언가 들어와 있을 때에만 렌더링 하는데, 이 flag가 있으면 빈 슬롯이어도 커버 렌더링
#ifdef WJ_ENABLE_TRADABLE_ICON
		SLOT_STATE_CANT_MOUSE_EVENT		= (1 << 4),
		SLOT_STATE_UNUSABLE				= (1 << 5),
#endif
	};

	class CSlotWindow : public CWindow
	{
		public:
			static DWORD Type();

		public:
			class CSlotButton;
			class CCoverButton;
			class CCoolTimeFinishEffect;

			friend class CSlotButton;
			friend class CCoverButton;

			typedef struct SSlot
			{
				DWORD	dwState;
				DWORD	dwSlotNumber;
				DWORD	dwCenterSlotNumber;		// NOTE : 사이즈가 큰 아이템의 경우 아이템의 실제 위치 번호
				DWORD	dwItemIndex;			// NOTE : 여기서 사용되는 Item이라는 단어는 좁은 개념의 것이 아닌,
				BOOL	isItem;					//        "슬롯의 내용물"이라는 포괄적인 개념어. 더 좋은 것이 있을까? - [levites]

				// CoolTime
				float	fCoolTime;
				float	fStartCoolTime;

				// Toggle
				BOOL	bActive;

				int		ixPosition;
				int		iyPosition;

				int		ixCellSize;
				int		iyCellSize;

				BYTE	byxPlacedItemSize;
				BYTE	byyPlacedItemSize;

				CGraphicImageInstance* pInstance;
				CNumberLine* pNumberLine;

				bool	bRenderBaseSlotImage;
				CCoverButton* pCoverButton;
				CSlotButton* pSlotButton;
				CImageBox* pSignImage;
				CAniImageBox* pFinishCoolTimeEffect;
#ifdef ENABLE_ACCE_SYSTEM
				CAniImageBox*	pActiveSlotEffect[3];
#endif
#if defined(__BL_ENABLE_PICKUP_ITEM_EFFECT__)
				D3DXCOLOR d3Color;
#endif
#if defined(__BL_TRANSMUTATION__)
				std::shared_ptr< CImageBox > pCoverImage;
#endif
			} TSlot;
			typedef std::list<TSlot> TSlotList;
			typedef TSlotList::iterator TSlotListIterator;

		public:
			CSlotWindow (PyObject* ppyObject);
			virtual ~CSlotWindow();

			void Destroy();

			// Manage Slot
			void SetSlotType (DWORD dwType);
			void SetSlotStyle (DWORD dwStyle);

			void AppendSlot (DWORD dwIndex, int ixPosition, int iyPosition, int ixCellSize, int iyCellSize);
			void SetCoverButton (DWORD dwIndex, const char* c_szUpImageName, const char* c_szOverImageName, const char* c_szDownImageName, const char* c_szDisableImageName, BOOL bLeftButtonEnable, BOOL bRightButtonEnable);
			void SetSlotBaseImage (const char* c_szFileName, float fr, float fg, float fb, float fa);
			void AppendSlotButton (const char* c_szUpImageName, const char* c_szOverImageName, const char* c_szDownImageName);
			void AppendRequirementSignImage (const char* c_szImageName);

			void EnableCoverButton (DWORD dwIndex);
			void DisableCoverButton (DWORD dwIndex);
			void SetAlwaysRenderCoverButton (DWORD dwIndex, bool bAlwaysRender = false);

			void ShowSlotBaseImage (DWORD dwIndex);
			void HideSlotBaseImage (DWORD dwIndex);
			BOOL IsDisableCoverButton (DWORD dwIndex);
			BOOL HasSlot (DWORD dwIndex);

			void ClearAllSlot();
			void ClearSlot (DWORD dwIndex);
			void SetSlot (DWORD dwIndex, DWORD dwVirtualNumber, BYTE byWidth, BYTE byHeight, CGraphicImage* pImage, D3DXCOLOR& diffuseColor);
			void SetSlotCount (DWORD dwIndex, DWORD dwCount);
			void SetSlotCountNew (DWORD dwIndex, DWORD dwGrade, DWORD dwCount);
			void SetSlotCoolTime (DWORD dwIndex, float fCoolTime, float fElapsedTime = 0.0f);
			void ActivateSlot (DWORD dwIndex);
			void DeactivateSlot (DWORD dwIndex);
			void RefreshSlot();
#if defined(__BL_TRANSMUTATION__)
			void SetSlotCoverImage(const DWORD dwIndex, const char* FileName);
			void EnableSlotCoverImage(const DWORD dwIndex, const bool bEnable);
#endif
			DWORD GetSlotCount();

			void LockSlot (DWORD dwIndex);
			void UnlockSlot (DWORD dwIndex);
			BOOL IsLockSlot (DWORD dwIndex);
			void SetCantUseSlot (DWORD dwIndex);
			void SetUseSlot (DWORD dwIndex);
			BOOL IsCantUseSlot (DWORD dwIndex);
			void EnableSlot (DWORD dwIndex);
			void DisableSlot (DWORD dwIndex);
			BOOL IsEnableSlot (DWORD dwIndex);
#ifdef WJ_ENABLE_TRADABLE_ICON
			void SetCanMouseEventSlot(DWORD dwIndex);
			void SetCantMouseEventSlot(DWORD dwIndex);
			void SetUsableSlotOnTopWnd(DWORD dwIndex);
			void SetUnusableSlotOnTopWnd(DWORD dwIndex);
#endif
			// Select
			void ClearSelected();
			void SelectSlot (DWORD dwSelectingIndex);
			BOOL isSelectedSlot (DWORD dwIndex);
			DWORD GetSelectedSlotCount();
			DWORD GetSelectedSlotNumber (DWORD dwIndex);

			// Slot Button
			void ShowSlotButton (DWORD dwSlotNumber);
			void HideAllSlotButton();
			void OnPressedSlotButton (DWORD dwType, DWORD dwSlotNumber, BOOL isLeft = TRUE);

			// Requirement Sign
			void ShowRequirementSign (DWORD dwSlotNumber);
			void HideRequirementSign (DWORD dwSlotNumber);

			// ToolTip
			BOOL OnOverInItem (DWORD dwSlotNumber);
			void OnOverOutItem();
#if defined(__BL_ENABLE_PICKUP_ITEM_EFFECT__)
			void SetSlotDiffuseColor(DWORD dwIndex, int iColorType);
#endif
			// For Usable Item
			void SetUseMode (BOOL bFlag);
			void SetUsableItem (BOOL bFlag);

			// CallBack
			void ReserveDestroyCoolTimeFinishEffect (DWORD dwSlotIndex);
#ifdef ENABLE_ADDITIONAL_EQUIPMENT_PAGE
			void	SetSlotChange(DWORD oldIndex, DWORD newIndex);
#endif
#ifdef ENABLE_ACCE_SYSTEM
			void ActivateEffect(DWORD dwSlotIndex, float r, float g, float b, float a);
			void DeactivateEffect(DWORD dwSlotIndex);
#endif
		protected:
			void __Initialize();
			void __CreateToggleSlotImage();
#if defined(__BL_ENABLE_PICKUP_ITEM_EFFECT__)
			void __CreateSlotEnableEffect(int index);
#else
			void __CreateSlotEnableEffect();
#endif
			void __CreateFinishCoolTimeEffect (TSlot* pSlot);
			void __CreateBaseImage (const char* c_szFileName, float fr, float fg, float fb, float fa);

			void __DestroyToggleSlotImage();
#if defined(__BL_ENABLE_PICKUP_ITEM_EFFECT__)
			void __DestroySlotEnableEffect(int index);
#else
			void __DestroySlotEnableEffect();
#endif
			void __DestroyFinishCoolTimeEffect (TSlot* pSlot);
			void __DestroyBaseImage();

			// Event
			void OnUpdate();
			void OnRender();
			BOOL OnMouseLeftButtonDown();
			BOOL OnMouseLeftButtonUp();
			BOOL OnMouseRightButtonDown();
			BOOL OnMouseLeftButtonDoubleClick();
			void OnMouseOverOut();
			void OnMouseOver();
			void RenderSlotBaseImage();
			void RenderLockedSlot();
			virtual void OnRenderPickingSlot();
			virtual void OnRenderSelectedSlot();

			// Select
			void OnSelectEmptySlot (int iSlotNumber);
			void OnSelectItemSlot (int iSlotNumber);
			void OnUnselectEmptySlot (int iSlotNumber);
			void OnUnselectItemSlot (int iSlotNumber);
			void OnUseSlot();

			// Manage Slot
			BOOL GetSlotPointer (DWORD dwIndex, TSlot** ppSlot);
			BOOL GetSelectedSlotPointer (TSlot** ppSlot);
			virtual BOOL GetPickedSlotPointer (TSlot** ppSlot);
			void ClearSlot (TSlot* pSlot);
			virtual void OnRefreshSlot();

			// ETC
			BOOL OnIsType (DWORD dwType);

		protected:
			DWORD m_dwSlotType;
			DWORD m_dwSlotStyle;
			std::list<DWORD> m_dwSelectedSlotIndexList;
			TSlotList m_SlotList;
			DWORD m_dwToolTipSlotNumber;

			BOOL m_isUseMode;
			BOOL m_isUsableItem;

			CGraphicImageInstance* m_pBaseImageInstance;
			CImageBox* m_pToggleSlotImage;
#if defined(__BL_ENABLE_PICKUP_ITEM_EFFECT__)
			CAniImageBox * m_pSlotActiveEffect[SLOT_ACTIVE_EFFECT_COUNT];
#else
			CAniImageBox* m_pSlotActiveEffect;
#endif
			std::deque<DWORD> m_ReserveDestroyEffectDeque;
	};
};