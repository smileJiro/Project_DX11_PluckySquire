#pragma once
#include <process.h>
#pragma warning (disable : 5208)
#pragma comment(lib,"Engine.lib")
#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING

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

#include "Map_Tool_Function.h"

using namespace Map_Tool;