#pragma once
#include <process.h>

namespace Client
{
	const unsigned int g_iWinSizeX = 1600;
	const unsigned int g_iWinSizeY = 900;


    // 신규 레벨 추가 시 등록
    inline constexpr const char* LevelNames[] =
    {
        nullptr,            // LOADING
        nullptr,            // STATIC
        "Level_Logo",       // LOGO
        "Level_Chapter_2",  // CH2
        "Level_Chapter_4",  // CH4
        "Level_Chapter_6",  // CH6
        "Level_Chapter_8",  // CH8
        "Level_Test",       // CH_TEST
        nullptr,            // CAMERA_TOOL
        nullptr             // END
    };


}
extern float g_BGMVolume;
extern float g_SFXVolume;
extern HINSTANCE g_hInst;
extern HWND g_hWnd;


#include "Client_Macro.h"
#include "Client_Enum.h"
#include "Client_Function.h"
#include "Client_Struct.h"

using namespace Client;