#pragma once

#include <process.h>

namespace Camera_Tool
{
	const unsigned int			g_iWinSizeX = { 1600 };
	const unsigned int			g_iWinSizeY = { 900 };

	enum LEVEL_ID { LEVEL_LOADING, LEVEL_STATIC, LEVEL_CAMERA_TOOL, LEVEL_TRIGGER_TOOL, LEVEL_END };
	enum class EVENT_TYPE
	{
		CREATE_OBJECT,
		DELETE_OBJECT,
		LEVEL_CHANGE,
		SET_ACTIVE,
		HR,
		CHANGE_COORDINATE,
		LAST,
	};

	enum class F_DIRECTION
	{
		LEFT,
		RIGHT,
		UP,
		DOWN,
		F_DIR_LAST
	};

	enum class CYCLE_TYPE
	{
		TIME,
		COUNT,
		CYCLE_TYPE_END
	};

		enum OBJECT_GROUP // 아래 GROUP은 예시임. 편한대로 사용하시면 됨.
	{
		NONE =						0x00, 
		PLAYER =					0x02,
		MONSTER =					0x04,
		MAPOBJECT =					0x08,
		INTERACTION_OBEJCT =		0x10,
		PLAYER_PROJECTILE =			0x20,
		MONSTER_PROJECTILE =		0x40,
		TRIGGER_OBJECT =		    0x80,
								 // 0x100
								 // 0x200
								 // 0x400 

		LAST
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
			PR3D_AFTEREFFECT = 80,
			PR3D_UI = 90,

			PR3D_LAST = 999
		};

#define MAP_2D_DEFAULT_PATH L"../../Client/Bin/MapSaveFiles/2D/"
#define MAP_3D_DEFAULT_PATH L"../../Client/Bin/MapSaveFiles/3D/"
}

extern HINSTANCE g_hInst;
extern HWND g_hWnd;

#include "Camera_Tool_Function.h"

using namespace Camera_Tool;