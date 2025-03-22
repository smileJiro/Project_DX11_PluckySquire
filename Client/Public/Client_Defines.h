#pragma once
#include <process.h>

namespace Client
{
	const unsigned int g_iWinSizeX = 1600;
	const unsigned int g_iWinSizeY = 900;


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