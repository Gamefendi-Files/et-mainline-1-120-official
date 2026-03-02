#pragma once

#include "AbstractSingleton.h"

class IAbstractChat : public TAbstractSingleton<IAbstractChat>
{
	public:
		IAbstractChat() {}
		virtual ~IAbstractChat() {}

		#if defined(CLIENT_LOCALE_STRING)
		enum ESpecialColorType
		{
			CHAT_SPECIAL_COLOR_NORMAL,
			CHAT_SPECIAL_COLOR_DICE_0,
			CHAT_SPECIAL_COLOR_DICE_1,
		};

		virtual void AppendChat (int iType, const char* c_szChat, BYTE bSpecialColorType = ESpecialColorType::CHAT_SPECIAL_COLOR_NORMAL) = 0;
		#endif
};