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

	enum RENDERGROUP
	{
		RG_2D = 1000,
		RG_3D = 2000,

		RG_LAST,
	};

	enum PRIORITY_3D
	{
		PR3D_BOOK2D = 0,
		PR3D_PRIORITY = 10,
		PR3D_SHADOW = 20,
		PR3D_NONBLEND = 30,
		PR3D_LIGHTS = 40,
		PR3D_FINAL = 50,
		PR3D_BLEND = 60,
		PR3D_EFFECT = 70,
		PR3D_PARTICLE = 80,
		PR3D_AFTERPARTICLE = 90,
		PR3D_BLURDOWN = 100,
		PR3D_BLUR = 110,
		PR3D_AFTEREFFECT = 120,
		PR3D_UI = 130,

		PR3D_LAST = 999
	};


	enum PRIORITY_2D
	{
		PR2D_PRIORITY = 0,
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


using namespace EffectTool;
