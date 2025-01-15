#pragma once
//
//#include "Camera_Tool_Defines.h"
//#include "Base.h"
//
//BEGIN(Engine)
//class CGameInstance;
//class CGameObject;
//class CTransform;
//END
//
//BEGIN(Camera_Tool)
//class CCamera_Manager : public CBase
//{
//private:
//	CCamera_Manager();
//	virtual ~CCamera_Manager() = default;
//
//public:
//	HRESULT				Initialize();
//	void				Update();
//
//public:
//	void				clear();
//	
//	void				Set_LevelCamera(class CCamera_Main* pCamera);
//	void				Set_CameraMode(_uint iCameraMode, _int iNextMode = -1);
//
//private:
//	CGameInstance*		m_pGameInstance = { nullptr };
//
//private:
//	map<_wstring, class CCameraArm*>		m_Arms;
//	class CCameraArm*						m_pCurrentArm = { nullptr };
//
//	class CCamera_Main*						m_pMainCamera = { nullptr };
//	CTransform*								m_pTransform = { nullptr };
//
//public:
//	virtual void Free() override;
//
//};
//
//END