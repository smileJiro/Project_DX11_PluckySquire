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


			RG_WORLDPOSMAP = 9000,
			RG_LAST,
		};

		enum PRIORITY_3D
		{
			PR3D_PRIORITY = 10,
			PR3D_SHADOW = 20,
			PR3D_GEOMETRY = 30,
			PR3D_DIRECTLIGHTS = 40,
			PR3D_LIGHTNG = 50, // >>> 리졸빙
			PR3D_POSTPROCESSING = 52, // PostProcessing
			PR3D_SSAO = 54, // PostProcessing
			PR3D_COMBINE = 60, // ToneMapping
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

		enum TRIGGER_TYPE
		{
			EVENT_TRIGGER,
			ARM_TRIGGER,
			CUTSCENE_TRIGGER,
			FREEZE_X_TRIGGER,
			FREEZE_Z_TRIGGER,
			TELEPORT_TRIGGER,
			SECTION_CHANGE_TRIGGER,
			DEFAULT_TRIGGER,
			ENABLE_LOOKAT_TRIGGER,

			TRIGGER_TYPE_END
		};

#define MAP_2D_DEFAULT_PATH L"../../Client/Bin/MapSaveFiles/2D/"
#define MAP_3D_DEFAULT_PATH L"../../Client/Bin/MapSaveFiles/3D/"
}

extern HINSTANCE g_hInst;
extern HWND g_hWnd;

#include "Camera_Tool_Function.h"

using namespace Camera_Tool;