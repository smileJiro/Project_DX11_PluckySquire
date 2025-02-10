#pragma once
#include <process.h>
#pragma warning (disable : 4273)
#pragma warning (disable : 4099)
namespace Map_Tool
{
	const unsigned int g_iWinSizeX = 1600;
	const unsigned int g_iWinSizeY = 900;

	enum LEVEL_ID { LEVEL_STATIC, LEVEL_LOADING, LEVEL_TOOL_3D_MAP, LEVEL_TOOL_3D_MODEL, LEVEL_TOOL_2D_MAP, LEVEL_END };
	enum LOG_TYPE
	{
		LOG_DEFAULT,
		LOG_LOADING,
		LOG_ERROR,
		LOG_SAVE,
		LOG_BINARY,
		LOG_LOAD,
		LOG_CREATE,
		LOG_DELETE,
		LOG_END
	};
	enum class EVENT_TYPE
	{
		CREATE_OBJECT,
		DELETE_OBJECT,
		LEVEL_CHANGE,
		SET_ACTIVE,
		CAPCHER,

		LAST,
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
		PR3D_NONBLEND = 30,
		PR3D_LIGHTS = 40,
		PR3D_FINAL = 50,
		PR3D_BLEND = 60,
		PR3D_EFFECT = 70,
		PR3D_AFTEREFFECT = 80,
		PR3D_UI = 90,

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

#define STATIC_3D_MODEL_FILE_PATH L"../../Client/Bin/resources/Models/"
#define MAP_2D_DEFAULT_PATH L"../../Client/Bin/MapSaveFiles/2D/"
#define MAP_3D_DEFAULT_PATH L"../../Client/Bin/MapSaveFiles/3D/"
#include "imguidll\imgui.h"
#include "imguidll\imgui_internal.h"



#include "Map_Tool_Function.h"
#include "ImGuiStyle_MapTool_Function.h"
#include "ImguiLogger.h"




using namespace Map_Tool;

#define LOG_TYPE(msg, type)		m_pLogger->Add_Log(msg,type)
#define LOG(msg)		m_pLogger->Add_Log(msg)

#define OBJECT_DESTROY(Obj) (Event_DeleteObject(Obj));

