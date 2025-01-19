#ifndef Engine_Macro_h__
#define Engine_Macro_h__

#define			PI 3.1415926f
#define			MATERIAL_TEXTURE_MAX	25


namespace Engine // Engine NameSpace를 사용하는 클래스의 기능을 호출하는 것이 아닌 상수 정의와 같은 Define은 namespace 안에 넣을 필요없다.
{

#ifndef			MSG_BOX
#define			MSG_BOX(_message)			MessageBox(NULL, TEXT(_message), L"System Message", MB_OK)
#endif

#define			BEGIN(NAMESPACE)		namespace NAMESPACE {
#define			END						}

#define			USING(NAMESPACE)	using namespace NAMESPACE;

#ifdef	ENGINE_EXPORTS
#define ENGINE_DLL		_declspec(dllexport)
#else
#define ENGINE_DLL		_declspec(dllimport)
#endif	

#define NO_COPY(CLASSNAME)											\
			private:												\
			CLASSNAME(const CLASSNAME&) = delete;					\
			CLASSNAME& operator = (const CLASSNAME&) = delete;		

#define DECLARE_SINGLETON(CLASSNAME)								\
			NO_COPY(CLASSNAME)										\
			private:												\
			static CLASSNAME*	m_pInstance;						\
			public:													\
			static CLASSNAME*	GetInstance( void );				\
			static unsigned int DestroyInstance( void );			

#define IMPLEMENT_SINGLETON(CLASSNAME)								\
			CLASSNAME*	CLASSNAME::m_pInstance = nullptr;			\
			CLASSNAME*	CLASSNAME::GetInstance( void )	{			\
			if(nullptr == m_pInstance) {							\
				m_pInstance = new CLASSNAME;						\
				}													\
				return m_pInstance;									\
			}														\
			unsigned int CLASSNAME::DestroyInstance( void ) {		\
				unsigned int iRefCnt = {0};							\
				if (nullptr != m_pInstance) {						\
				iRefCnt = m_pInstance->Release();					\
				if(0 == iRefCnt)									\
					m_pInstance = nullptr;							\
				}													\
				return iRefCnt;										\
			}

	// KeyManager
#define KEY_CHECK(key, state)   m_pGameInstance->GetKeyState(key) == state
#define KEY_PRESSING(key)       KEY_CHECK(key, KEY_STATE::PRESSING)
#define KEY_DOWN(key)           KEY_CHECK(key, KEY_STATE::DOWN)
#define KEY_UP(key)				KEY_CHECK(key, KEY_STATE::UP)
#define KEY_NONE(key)           KEY_CHECK(key, KEY_STATE::NONE)

#define MOUSE_CHECK(key, state)	m_pGameInstance->GetMouseKeyState(key) == state
#define MOUSE_PRESSING(key)		MOUSE_CHECK(key, KEY_STATE::PRESSING)
#define MOUSE_DOWN(key)			MOUSE_CHECK(key, KEY_STATE::DOWN)
#define MOUSE_UP(key)			MOUSE_CHECK(key, KEY_STATE::UP)
#define MOUSE_NONE(key)			MOUSE_CHECK(key, KEY_STATE::NONE)

#define MOUSE_MOVE(key)			m_pGameInstance->GetDIMouseMove(key)

	// Sound_Manager
	/* BGM */
#define LOAD_BGM(strBGMTag, strBGMFilePath) m_pGameInstance->Load_BGM(strBGMTag, strBGMFilePath)
#define START_BGM(strBGMTag, fVolume) m_pGameInstance->Start_BGM(strBGMTag, fVolume)
#define STOP_BGM() m_pGameInstance->Stop_BGM()
#define END_BGM() m_pGameInstance->End_BGM()
#define GET_VOLUME_BGM() m_pGameInstance->Get_BGMVolume()
#define SET_VOLUME_BGM(fVolume) m_pGameInstance->Set_BGMVolume(fVolume)

	/* SFX */
#define LOAD_SFX(strSFXTag, strSFXFilePath) m_pGameInstance->Load_SFX(strSFXTag, strSFXFilePath)
#define START_SFX(strSFXTag, fVolume, isLoop) m_pGameInstance->Start_SFX(strSFXTag, fVolume , isLoop)
#define START_SFX_DELAY(strSFXTag, fDelaytime, fVolume, isLoop) m_pGameInstance->Start_SFX_Delay(strSFXTag, fDelaytime, fVolume , isLoop)
#define STOP_SFX(strSFXTag) m_pGameInstance->Stop_SFX(strSFXTag)
#define END_SFX(strSFXTag) m_pGameInstance->End_SFX(strSFXTag)
#define GET_VOLUME_SFX(strSFXTag) m_pGameInstance->Get_SFXVolume(strSFXTag)
#define SET_VOLUME_SFX(strSFXTag, fVolume) m_pGameInstance->Set_SFXVolume(strSFXTag, fVolume)

	// GlobalFunction_Manager

#define STRINGTOWSTRING(str) m_pGameInstance->StringToWString(str)
#define WSTRINGTOSTRING(wstr) m_pGameInstance->WStringToString(wstr)

#define GET_CURSOR_POS() m_pGameInstance->Get_CursorPos()
#define GET_CURSOR_CLICKPOS() m_pGameInstance->Get_OnClickPos()
#define LERP(fLeft, fRight, fRatio) m_pGameInstance->Lerp(fLeft, fRight, fRatio)

}


#define PHYSX_RELEASE(pPxInstance) if(nullptr != pPxInstance){pPxInstance->release(); pPxInstance = nullptr;}


#endif // Engine_Macro_h__
