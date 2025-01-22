#pragma once
#include <process.h>

namespace AnimTool
{
	const unsigned int g_iWinSizeX = 1600;
	const unsigned int g_iWinSizeY = 900;
}

extern HINSTANCE g_hInst;
extern HWND g_hWnd;

#define IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS_IMPLEMENTED

#include "imguidll\imgui.h"
#include "imguidll\imgui_internal.h"

#include <windows.h>
#include <commdlg.h> 
#include <string>
#include <shlobj.h>
#include <combaseapi.h>

#include "AnimTool_Enum.h"
#include "AnimTool_Function.h"

using namespace AnimTool;