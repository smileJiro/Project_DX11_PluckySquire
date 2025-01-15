#pragma once

#include <process.h>

namespace Camera_Tool
{
	const unsigned int			g_iWinSizeX = { 1600 };
	const unsigned int			g_iWinSizeY = { 900 };

	enum LEVEL_ID { LEVEL_LOADING, LEVEL_STATIC, LEVEL_CAMERA_TOOL, LEVEL_GAMEPLAY, LEVEL_END };
	enum class EVENT_TYPE
	{
		CREATE_OBJECT,
		DELETE_OBJECT,
		LEVEL_CHANGE,
		SET_ACTIVE,
		HR,

		LAST,
	};
}

extern HINSTANCE g_hInst;
extern HWND g_hWnd;

#include "Camera_Tool_Function.h"

using namespace Camera_Tool;