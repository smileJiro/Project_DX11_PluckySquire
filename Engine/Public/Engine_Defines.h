#pragma once

#pragma warning (disable : 4251) // 4251 경고는 비활성한다.

#include <d3d11.h>

#define DIRECTINPUT_VERSION	0x0800
#include <dinput.h>
#include <DirectXMath.h>
#include "DirectXCollision.h"
using namespace DirectX;

#include "Effects11\d3dx11effect.h"
#include "DirectXTK\DDSTextureLoader.h" // dds 확장자 전용
#include "DirectXTK\WICTextureLoader.h" // 그외 모든 확장자 전용(tga 지원 x)
#include "DirectXTK\ScreenGrab.h" // 마스크맵 만들때 사용했음, 일반적으로 스크린샷같은 기능 지원 이미지를 파일로 만드는데에.
#include "DirectXTK\PrimitiveBatch.h" // 콜라이더 디버그에서 렌더링할때 사용
#include "DirectXTK\VertexTypes.h" // 각종 기본 버텍스 타입을 마소에서 제공 했음. >>> 우린 콜라이더 그리기위해 
#include "DirectXTK\Effects.h" // dx9처럼 가장 기초적인 렌더링을 지원하는 헤더파일

#include "DirectXTK\SpriteBatch.h" // 콜라이더 그릴때 매개변수로 들어가는 Draw Color 관련 구조체가 이 헤더에 있어서 추가함.
#include "DirectXTK\SpriteFont.h"

#include "assimp\scene.h"
#include "assimp\Importer.hpp"
#include "assimp\postprocess.h"
#include <d3dcompiler.h>

/* Imgui */
#include "imguidll\imgui.h"
#include "imguidll\imgui_impl_win32.h"
#include "imguidll\imgui_impl_dx11.h"
// 코드
#pragma comment(lib, "imguidll.lib")


/* Sound */
#include <Windows.h>
#include <mmsystem.h>
#include <dsound.h>

// 코드
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "dsound.lib")




namespace Engine
{
	enum class PROTOTYPE { PROTO_GAMEOBJ, PROTO_COMPONENT };

	const unsigned int g_iShadowWidth = 8192;
	const unsigned int g_iShadowHeight = 4608;


}
#include <functional>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <iostream>
#include <typeinfo>
using namespace std;

#include "Engine_Typedef.h"
#include "Engine_Macro.h"
#include "Engine_Struct.h"
#include "Engine_Function.h"
#include "Engine_Enum.h"

#include <fstream>

#include <json.hpp>

using json = nlohmann::json;

using namespace Engine;

// Memory Leak Check Code

#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifndef DBG_NEW 

#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
#define new DBG_NEW 

#endif

#endif // _DEBUG
