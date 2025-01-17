#pragma once

#include "Engine_Defines.h"
#include "Transform.h"
#include "Base.h"

BEGIN(Engine)
class CGameInstance;
class CController_Transform;
class CCamera;
class CCameraArm;
END

BEGIN(Engine)
class ENGINE_DLL CCamera_Manager : public CBase
{
public:
	enum CAMERA_TYPE { FREE, TARGET, CAMERA_TYPE_END };

	typedef struct tagArmDataDesc
	{
		_float3				vArm = { 0.f, 0.f, -1.f };
		_float				fLength = 1.f;
		_float				fTurnTime = {};
		_float				fLengthTime = {};
	} ARM_DATA;

private:
	CCamera_Manager();
	virtual ~CCamera_Manager() = default;

public:
	HRESULT				Initialize();
	void				Update(_float fTimeDelta);

#ifdef _DEBUG
	void				Render();
#endif

public:
	CCamera*			Get_CurrentCamera() { return m_Cameras[m_eCurrentCameraType]; }
	CCamera*			Get_Camera(_uint _iCameraType) { return m_Cameras[_iCameraType]; }
	_vector				Get_CameraVector(CTransform::STATE _eState);						// 현재 카메라 Right, Up, Look, Pos 가져오는 함수
	_uint				Get_CameraType() { return m_eCurrentCameraType; }					

public:
	void				Add_Camera(_uint _iCurrentCameraType, CCamera* _pCamera);			// Free Camera, Target Camera 셋팅(처음 한 번)

	void				Change_CameraMode(_uint _iCameraMode, _int _iNextMode = -1);		// 카메라 모드 전환(아마 Target Camera만 적용)							
	void				Change_CameraType(_uint _iCurrentCameraType);
	void				Change_CameraTarget(const _float4x4* _pTargetWorldMatrix);

	void				Set_CameraPos(_vector _vCameraPos, _vector _vTargetPos);			// CameraArm이 호출해서 Camera의 위치 설정

#ifdef _DEBUG
	void				Set_Rotation(_vector vRotation);
#endif
	
private:
	CGameInstance*							m_pGameInstance = { nullptr };

private:
	map<_wstring, ARM_DATA*>				m_ArmDatas;

	CCamera*								m_Cameras[CAMERA_TYPE_END];
	_uint									m_eCurrentCameraType = { CAMERA_TYPE_END };

public:
	static CCamera_Manager* Create();
	virtual void			Free();
};

END