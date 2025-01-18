#pragma once

#include "Camera_Tool_Defines.h"
#include "Transform.h"
#include "Base.h"

BEGIN(Engine)
class CGameInstance;
class CCamera;
class CCameraArm;
END

BEGIN(Camera_Tool)
class CCamera_Manager_Tool : public CBase
{
	DECLARE_SINGLETON(CCamera_Manager_Tool)

public:
	enum CAMERA_TYPE { FREE, TARGET, CAMERA_TYPE_END };

private:
	CCamera_Manager_Tool();
	virtual ~CCamera_Manager_Tool() = default;

public:
	HRESULT				Initialize();
	void				Update();
	void				Render();

public:
	CCamera*			Get_CurrentCamera() { return m_Cameras[m_eCurrentCameraType]; }
	CCamera*			Get_Camera(_uint _iCameraType) { return m_Cameras[_iCameraType]; }
	_vector				Get_CameraVector(CTransform::STATE _eState);						// 현재 카메라 Right, Up, Look, Pos 가져오는 함수
	_uint				Get_CameraType() { return m_eCurrentCameraType; }
	void				Get_ArmNames(vector<_wstring>* _vecArmNames);
	ARM_DATA*			Get_ArmData(_wstring _wszArmName);

public:
	void				Add_Camera(_uint _iCurrentCameraType, CCamera* _pCamera);			// Free Camera, Target Camera 셋팅(처음 한 번)

	void				Change_CameraMode(_uint _iCameraMode, _int _iNextMode = -1);		// 카메라 모드 전환(아마 Target Camera만 적용)							
	void				Change_CameraType(_uint _iCurrentCameraType);
	void				Change_CameraTarget(const _float4x4* _pTargetWorldMatrix);

	void				Set_NextArmData(_wstring _wszNextArmName);

	// Tool 작업 관련
public:
	void				Copy_Arm();
	void				Add_CopyArm(_wstring _wszArmTag, ARM_DATA _pData);
	void				Add_ArmData(_wstring wszArmTag, ARM_DATA _pData);
	void				Edit_ArmInfo(_wstring _wszArmTag);			// Copy Arm에 넣어서 초록색인 상태로 수정

	_float				Get_ArmLength(_bool _isCopyArm);

	void				Set_ArmRotation(_vector _vRotation, _bool _isCopyArm);
	void				Set_ArmLength(_float _fLength, _bool _isCopyArm);
	void				Set_CurrentArm(CCameraArm* _pCameraArm);	// Target Camera에 넣어 주는 애 받아서 함 Tool에서 Copy하기 위한 목적

private:
	CGameInstance*							m_pGameInstance = { nullptr };

private:
	CCamera*								m_Cameras[CAMERA_TYPE_END];
	_uint									m_eCurrentCameraType = { CAMERA_TYPE_END };

	map<_wstring, class CCameraArm*>		m_Arms;
	class CCameraArm*						m_pCurrentArm = { nullptr };
	CCameraArm*								m_pCopyArm = { nullptr };

	map<_wstring, ARM_DATA*>				m_ArmDatas;				// Data 저장

private:
	CCameraArm*			Find_Arm(_wstring _wszArmTag);
	ARM_DATA*			Find_ArmData(_wstring _wszArmTag);

public:
	virtual void		Free() override;
};

END