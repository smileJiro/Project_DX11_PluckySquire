#pragma once
#include <process.h>

namespace AnimTool
{
	const unsigned int g_iWinSizeX = 1600;
	const unsigned int g_iWinSizeY = 900;

	enum RENDERGROUP
	{
		RG_2D = 1000,
		RG_3D = 2000,


		RG_WORLDPOSMAP = 9000,
		RG_LAST,
	};

	enum PRIORITY_3D
	{
		PR3D_PRIORITY = 10,
		PR3D_SHADOW = 20,
		PR3D_GEOMETRY = 30,
		PR3D_DIRECTLIGHTS = 40,
		PR3D_LIGHTNG = 50,
		PR3D_BLOOM = 52,
		PR3D_SSAO = 54,
		PR3D_COMBINE = 60,
		PR3D_BLEND = 70,
		PR3D_EFFECT = 80,
		PR3D_PARTICLE = 90,
		PR3D_AFTERPARTICLE = 100,
		PR3D_BLURDOWN = 110,
		PR3D_BLUR = 120,
		PR3D_AFTEREFFECT = 130,
		PR3D_UI = 140,

		PR3D_LAST = 999
	};


	enum PRIORITY_2D
	{
		// 전처리 그룹
		PR2D_PRIORITY = 0,
		PR2D_BOOK_SECTION = 5,
		PR2D_SECTION_START = 10,
		// 10 ~ 970 
		//-> 섹션 유동생성 발급
		PR2D_SECTION_END = 970,
		PR2D_UI = 980,
		// 후처리 그룹
		PR2D_AFTER = 990,
		PR2D_LAST = 999
	};


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