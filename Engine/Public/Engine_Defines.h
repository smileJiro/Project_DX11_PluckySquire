#pragma once

#pragma warning (disable : 4251) // 4251 경고는 비활성한다.
#pragma warning (disable : 4099)
#include <d3d11.h>

#define DIRECTINPUT_VERSION	0x0800
#include <dinput.h>
#include <DirectXMath.h>
#include "DirectXCollision.h"
#include <dxgi1_4.h> // DXGI 1.4 헤더 필요
#include <d3d11sdklayers.h> // 디버그 기능 관련
#include <dxgidebug.h>

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



/* Assimp */
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
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
/* Sound */
#include <Windows.h>
#include <mmsystem.h>
#include <dsound.h>
///* DirectXTex */
//#include "DirectXTex\DirectXTex.h"
//#include "DirectXTex\DirectXTex.inl"
// 코드
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "dsound.lib")

//#pragma warning    (disable : 4514)
//#pragma warning    (disable : 4820)
//#pragma warning    (disable : 4127)
//#pragma warning    (disable : 4710)
//#pragma warning    (disable : 4711)
//#pragma warning    (disable : 4577)
//#pragma warning    (disable : 4996)
//#pragma warning    (disable : 4251)
#pragma warning    (disable : 6297)
#pragma warning    (disable : 33010)
#pragma warning (disable : 26495)

#include "physx/PxPhysicsAPI.h"
//#pragma comment    (lib, "physx/LowLevel_static_64.lib")
//#pragma comment    (lib, "physx/LowLevelAABB_static_64.lib")
//#pragma comment    (lib, "physx/LowLevelDynamics_static_64.lib")
//#pragma comment    (lib, "physx/PhysX_64.lib")
//#pragma comment    (lib, "physx/PhysXCharacterKinematic_static_64.lib")
//#pragma comment    (lib, "physx/PhysXCommon_64.lib")
//#pragma comment    (lib, "physx/PhysXCooking_64.lib")
//#pragma comment    (lib, "physx/PhysXExtensions_static_64.lib")
//#pragma comment    (lib, "physx/PhysXFoundation_64.lib")
//#pragma comment    (lib, "physx/PhysXPvdSDK_static_64.lib")
//#pragma comment    (lib, "physx/PhysXTask_static_64.lib")
//#pragma comment    (lib, "physx/PhysXVehicle_static_64.lib")
//#pragma comment    (lib, "physx/PhysXVehicle2_static_64.lib")
//#pragma comment    (lib, "physx/SceneQuery_static_64.lib")
//#pragma comment    (lib, "physx/SimulationController_static_64.lib")
using namespace physx;

#pragma warning (default : 26495)
#pragma warning    (default : 33010)
#pragma warning    (default : 6297)
//#pragma warning    (default : 4514)
//#pragma warning    (default : 4820)
//#pragma warning    (default : 4127)
//#pragma warning    (default : 4710)
//#pragma warning    (default : 4711)
//#pragma warning    (default : 4577)
//#pragma warning    (default : 4996)
//#pragma warning    (default : 4251)

 



namespace Engine
{

	enum class PROTOTYPE { PROTO_GAMEOBJ, PROTO_COMPONENT };




	//extern IMPORT_MODE	g_eImportMode = IMPORT_MODE::NONE_IMPORT;
	const unsigned int g_iShadowWidth = 8192;
	const unsigned int g_iShadowHeight = 4608;

}
#include <functional>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <iostream>
#include <typeinfo>
#include <stack>
#include <any>

#include <chrono>
#include <condition_variable>
#include <cstdio>
#include <future>
#include <mutex>
#include <thread>
using namespace std;

#include "Engine_Typedef.h"
#include "Engine_Macro.h"
#include "Engine_Struct.h"
#include "Engine_Function.h"
#include "Engine_Enum.h"

#include <fstream>
using namespace Engine;

#include <json.hpp>

using json = nlohmann::json;


// Memory Leak Check Code

#ifdef NDEBUG

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifndef DBG_NEW 

#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
#define new DBG_NEW 

#endif

#endif // _DEBUG
