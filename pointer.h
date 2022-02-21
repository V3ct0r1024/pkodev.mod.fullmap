#pragma once
#include "address.h"
#include "structure.h"

namespace pkodev
{
	namespace pointer
	{
		// CPlayer::CPlayer()
		typedef void( __thiscall* CPlayer__CPlayer__Ptr)(void*);
		CPlayer__CPlayer__Ptr CPlayer__CPlayer = (CPlayer__CPlayer__Ptr)(void*)(pkodev::address::MOD_EXE_VERSION::CPlayer__CPlayer);

		// void	CMaskData::AddMap(char* pszName, int w, int h)
		typedef void(__thiscall* CMaskData__AddMap__Ptr)(void*, const char *, int, int);
		CMaskData__AddMap__Ptr CMaskData__AddMap = (CMaskData__AddMap__Ptr)(void*)(pkodev::address::MOD_EXE_VERSION::CMaskData__AddMap);
	}
}