//  --------------------------------------------
// |	Locale Services							|
//  --------------------------------------------
//#define LOCALE_SERVICE_GLOBAL					// GLOBAL version
#define LOCALE_SERVICE_EUROPE					// 유럽

//#define LSS_SECURITY_KEY		"1234abcd5678efgh"

//#define CHECK_LATEST_DATA_FILES

//#define USE_RELATIVE_PATH

//  --------------------------------------------
// |	New Defines							|
//  --------------------------------------------
#define ENABLE_COSTUME_SYSTEM
#define ENABLE_ENERGY_SYSTEM
#define ENABLE_DRAGON_SOUL_SYSTEM
#define ENABLE_NEW_EQUIPMENT_SYSTEM
//#define ENABLE_DISCORD_RPC					// Discord Rich Presence			* Author: Mali		*
#define CLIENT_LOCALE_STRING					// [REVERSED] Client Locale String	* Author: Mali		*
#define SAVE_CAMERA_MODE						// [REVERSED] Save Camera Mode		* Author: Mali		*
#define ENABLE_COSTUME_WEAPON_SYSTEM			// kostüm silah sistemi
#define ENABLE_ADDITIONAL_EQUIPMENT_PAGE		// Official Additional Equipment Page
#define ENABLE_MOUNT_COSTUME_SYSTEM				// costüm binek sistemi (yanında gezmelisinden)
#define ENABLE_OBJ_SCALLING						// Obje Boyutlandırma Sistemi
#define ENABLE_ACCE_SYSTEM						// Kuşak Sistemi
#define ENABLE_AURA_SYSTEM						// Official Aura Sistemi
#define ENABLE_NEW_GAMEOPTION					// dracarys sistem seçenekleri sistemi
#define ENABLE_SHOW_NIGHT_SYSTEM				// snow enviroment system
#define ENABLE_FOV_OPTION						// Geniş Görüş Açısı sistemi
#define __BL_CLIP_MASK__						// Kaydırmalı pencerelerde, yazıların, butonların ve görsellerin, pencere dışına taşmamasını sağlayan bir sistem.
#define __BL_MOUSE_WHEEL_TOP_WINDOW__			// Fare tekeri kullanarak kaydırmayı, en son üzerine tıklanılan pencerede tutan sistem.
#define __BL_SMOOTH_SCROLL__					// Kaydırma çubuğunun pürüzsüz hareket etmesini sağlayan sistem.
#define ENABLE_QUEST_RENEWAL					// Quest Page Renewal
#define __BL__DETAILS_UI__						// Karakter detayları sayfası sistemi.
#define ENABLE_CPP_PSM							// fast loading
#define ENABLE_Storing_Affects					// Karakter üzerindeki her affecti kontrol edebilmenize olanak sağlayan sistem.
#define __BL_MOVE_CHANNEL__						// hızlı kanal değiştirme sistemi.
#define WJ_NEW_DROP_DIALOG						// yere sil sat sistemi.
#define ENABLE_ITEM_DELETE_SYSTEM				// Toplu Sil-Sat modülü
#define __BL_OFFICIAL_LOOT_FILTER__
#if defined(__BL_OFFICIAL_LOOT_FILTER__)
#	define ENABLE_PREMIUM_LOOT_FILTER			// Enable Premium Usage of the Loot Filter System
#endif
#define ENABLE_SHOW_CHEST_DROP					// Sandık aynası sistemi
#define ENABLE_LARGE_DYNAMIC_PACKET				// Large dynamic packet Utility
#define ENABLE_TARGET_INFORMATION_SYSTEM		// Moblardan düşenleri görme
#define ENABLE_CONQUEROR_LEVEL					// Official Yohara Güncellemesi
#define ENABLE_APPLY_RANDOM						// Apply Random Individual Attributes
#define ENABLE_PENDANT							// Talisman Elements
#define ENABLE_GLOVE_SYSTEM						// Glove Equipement
#define ENABLE_ELEMENT_ADD						// Official new Element system
#define __BL_67_ATTR__							// offcial 6-7 efsun system
#define WJ_ENABLE_TRADABLE_ICON					// trade item efekti
#define __BL_ENABLE_PICKUP_ITEM_EFFECT__		// new item efekt
#define VIEW_TARGET_PLAYER_HP					// Player HP Bar on TargetBoard		* Author: N/A		*
#define VIEW_TARGET_DECIMAL_HP					// Monster HP Bar on TargetBoard	* Author: N/A		*
#define ENABLE_DS_GRADE_MYTH					// mitsi simya güncellemesi
#define ENABLE_DS_SET							// simya set bonus sistemi
#define ENABLE_DSS_ACTIVE_EFFECT_BUTTON			// Enable dragon soul effect button when enabled
#define ENABLE_DS_CHANGE_ATTR					// simya efsun değiştirme sistemi
#define ENABLE_REFINE_RENEWAL					// artı basarken pencere açık kalsın
#define ENABLE_ELEMENTAL_TARGET					// element target system
#define WJ_SHOW_MOB_INFO						//
#ifdef WJ_SHOW_MOB_INFO							//
#define ENABLE_SHOW_MOBAIFLAG					// mob agresif göster gizle
#define ENABLE_SHOW_MOBLEVEL					// mob lwl göster gizle
#define WJ_SHOW_MOB_INFO_EX						//
#endif
#define ENABLE_EXTENDED_DS_INVENTORY			// simya envanter sistemi
#define ENABLE_HIDE_COSTUME_SYSTEM				// Kostüm Gizleme
#define ITEM_ANTIFLAG_TOOLTIP					// item antiflag tooltip
#define ENABLE_7AND8TH_SKILLS					// Official 7-8 Skill Güncellemesi
#define ENABLE_SHIP_DEFENSE						// Official Ship Defense (Hydra Dungeon)
#define __BL_MOVE_COSTUME_ATTR__				// official kostüm bonus aktarımı
#define __BL_TRANSMUTATION__					// Official item yansıtma sistemi








//  --------------------------------------------
// |	Fixes									|
//  --------------------------------------------
#define FOG_FIX									// [REVERSED] Fog Fix				* Author: Mali		*
#define LEVEL_FIX								// [REVERSED] Level Update Fix		* Author: Mali		*
#define ENABLE_FIX_MOBS_LAG						// theadmin33