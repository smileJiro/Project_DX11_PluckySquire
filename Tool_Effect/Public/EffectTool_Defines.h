#pragma once
#include <process.h>
#include "Engine_Defines.h"

namespace EffectTool
{
	const unsigned int g_iWinSizeX = 1600;
	const unsigned int g_iWinSizeY = 900;

	enum LEVEL_ID
	{
		LEVEL_STATIC,
		LEVEL_LOADING,
		LEVEL_TOOL,


		LEVEL_END
	};

	enum class EVENT_TYPE
	{
		CREATE_OBJECT,
		DELETE_OBJECT,
		LEVEL_CHANGE,
		SET_ACTIVE,

		LAST,
	};

}



extern HINSTANCE g_hInst;
extern HWND g_hWnd;


using namespace EffectTool;
