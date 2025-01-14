#pragma once

#include <process.h>

namespace Camera_Tool
{
	const unsigned int			g_iWinSizeX = { 1600 };
	const unsigned int			g_iWinSizeY = { 900 };

	enum LEVEL_ID { LEVEL_STATIC, LEVEL_LOADING, LEVEL_TOOL, LEVEL_GAMEPLAY, LEVEL_END };
}

extern HINSTANCE g_hInst;
extern HWND g_hWnd;

using namespace Camera_Tool;