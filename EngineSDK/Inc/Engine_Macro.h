#ifndef Engine_Macro_h__
#define Engine_Macro_h__


namespace Engine
{
#define			PI 3.1415926f
#define			MATERIAL_TEXTURE_MAX	25


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

}


#endif // Engine_Macro_h__
