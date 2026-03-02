#ifndef __INC_SERVICE_H__
#define __INC_SERVICE_H__
#pragma once
//  ============================================

//  --------------------------------------------
// |	Stock Defines							|
//  --------------------------------------------
#define _IMPROVED_PACKET_ENCRYPTION_			// 패킷 암호화 개선
#define __PET_SYSTEM__
#define __UDP_BLOCK__

//  --------------------------------------------
// |	New Defines								|
#define CLIENT_LOCALE_STRING					// [REVERSED] Client Locale String	* Author: Mali		*
#define __WEAPON_COSTUME_SYSTEM__				// kostum silah sistemi
#define ENABLE_ADDITIONAL_EQUIPMENT_PAGE 		// Official Additional Equipment Page
#define ENABLE_MOUNT_COSTUME_SYSTEM				// kostum binek sistemi
#define ENABLE_ACCE_SYSTEM						// kusak sistemi
#define __AURA_SYSTEM__							// Aura Sistemi
#define __QUEST_RENEWAL__						// Quest Page Renewal
#define __BL_MOVE_CHANNEL__						// hızlı kanal degistirme sistemi.
#define WJ_NEW_DROP_DIALOG						// yere item sil sat du?ur.
#define __BL_OFFICIAL_LOOT_FILTER__
#if defined(__BL_OFFICIAL_LOOT_FILTER__)
#	define __PREMIUM_LOOT_FILTER__				// Enable Premium Usage of the Loot Filter System
#endif
#define ENABLE_SHOW_CHEST_DROP					// sandık aynası modulu
#define ENABLE_LARGE_DYNAMIC_PACKET				// Large dynamic packet Utility
#define ENABLE_TARGET_INFORMATION_SYSTEM		// Moblardan dusenleri gorme
#define ENABLE_CONQUEROR_LEVEL					// Official Yohara Update
#define __ITEM_APPLY_RANDOM__					// Apply Random Individual Attributes
#define __PENDANT_SYSTEM__						// Talisman Elements
#define __GLOVE_SYSTEM__						// Glove Equipement
#define __ELEMENT_SYSTEM__						// official Element System
#define __BL_67_ATTR__							// offcial 6-7 efsun ekleme
#define WJ_ENABLE_TRADABLE_ICON					// --yapılamayan item efekti
#define __BL_ENABLE_PICKUP_ITEM_EFFECT__		// new item slot efekt
#define VIEW_TARGET_PLAYER_HP					// Player HP Bar on TargetBoard		* Author: N/A		*
#define VIEW_TARGET_DECIMAL_HP					// Monster HP Bar on TargetBoard	* Author: N/A		*
#define ENABLE_DS_GRADE_MYTH					// mitsi simya guncellemesi
#define ENABLE_DS_SET							// simya set bonus sistemi
#define ENABLE_DS_CHANGE_ATTR					// simya efsun de?i?tirme
#define ENABLE_EXTENDED_DS_INVENTORY			// simya envanter doluluk kontrolu (max 32 suan doldu?unda sandık acmaya izin vermez)
#define ENABLE_DRAGONSOUL_INVENTORY_BOX_SIZE	// simya envanter sistemi
#define __HIDE_COSTUME_SYSTEM__					// Kostum Gizleme
#define ENABLE_PET_SUMMON_AFTER_LOGIN			// pet giris kontrol
#define ENABLE_PROTO_RENEWAL					// pet c++ tasima
#define ENABLE_7AND8TH_SKILLS					// Official 7-8 Skill Sistemi
#define __SHIP_DEFENSE__						// Official Ship Defense (Hydra Dungeon)
#define __BL_MOVE_COSTUME_ATTR__				// official kostum bonus aktarımı
#define ENABLE_ITEM_ATTR_COSTUME				// official kostum efsunlama
#define __BL_TRANSMUTATION__					// Official item yansıtma sistemi


//  --------------------------------------------
// |	Fixes									|
//  --------------------------------------------
#define LEVEL_FIX								// [REVERSED] Level Update Fix	* Author: Mali			*

//  ============================================
#endif