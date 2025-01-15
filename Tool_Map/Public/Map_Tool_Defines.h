#pragma once
#include <process.h>
#pragma warning (disable : 4273)
namespace Map_Tool
{
	const unsigned int g_iWinSizeX = 1600;
	const unsigned int g_iWinSizeY = 900;

	enum LEVEL_ID { LEVEL_STATIC, LEVEL_LOADING, LEVEL_TOOL_MAP, LEVEL_TOOL_TRIGGER, LEVEL_END };
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
		HR,

		LAST,
	};

}

extern HINSTANCE g_hInst;
extern HWND g_hWnd;



#define IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS_IMPLEMENTED

#include "imguidll\imgui.h"
#include "imguidll\imgui_internal.h"



#include "Map_Tool_Function.h"
#include "ImguiLogger.h"




using namespace Map_Tool;

#define LOG_TYPE(msg, type)		m_pLogger->Add_Log(msg,type)
#define LOG(msg)		m_pLogger->Add_Log(msg)

#define OBJECT_DESTROY(Obj) (Event_DeleteObject(Obj));

