#pragma once
#include "Camera.h"

BEGIN(Engine)
class CCameraArm;
END

BEGIN(Client)
class CCamera_Target  : public CCamera
{
public:
	enum CAMERA_MODE { DEFAULT, MOVE_TO_NEXTARM, RETURN_TO_PREARM, RETURN_TO_DEFUALT, CAMERA_MODE_END };

	typedef struct tagCameraTargetDesc : public CCamera::CAMERA_DESC
	{
		CAMERA_MODE				eCameraMode;
		_float3					vAtOffset;
		_float					fSmoothSpeed;
	}CAMERA_TARGET_DESC;

private:
	CCamera_Target(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera_Target(const CCamera_Target& Prototype);
	virtual ~CCamera_Target() = default;

public:
	virtual HRESULT				Initialize_Prototype() override;
	virtual HRESULT				Initialize(void* pArg) override;
	virtual void				Priority_Update(_float fTimeDelta) override;
	virtual void				Update(_float fTimeDelta) override;
	virtual void				Late_Update(_float fTimeDelta) override;

#ifdef _DEBUG
	_float3						Get_ArmRotation();
#endif

public:
	void						Add_CurArm(CCameraArm* _pCameraArm);
	void						Add_ArmData(_wstring _wszArmTag, ARM_DATA* _pArmData, SUB_DATA* _pSubData);
	
	_bool						Set_NextArmData(_wstring _wszNextArmName, _int _iTriggerID);
	void						Set_PreArmDataState(_int _iTriggerID, _bool _isReturn);
	void						Set_CameraMode(_uint _iCameraMode, _int iNextCameraMode = -1) { m_eCameraMode = (CAMERA_MODE)_iCameraMode; m_iNextCameraMode = iNextCameraMode; }
	void						Change_Target(const _float4x4* _pTargetWorldMatrix) override;

private:
	CAMERA_MODE					m_eCameraMode = { CAMERA_MODE_END };
	_int						m_iNextCameraMode = { -1 };

	_float						m_fSmoothSpeed = {};

	// Arm
	map<_wstring, pair<ARM_DATA*, SUB_DATA*>>	m_ArmDatas;
	CCameraArm*					m_pCurArm = { nullptr };

private:
	void						Key_Input(_float _fTimeDelta);

	void						Action_Mode(_float _fTimeDelta);
	void						Defualt_Move(_float fTimeDelta);
	void						Move_To_NextArm(_float _fTimeDelta);
	void						Look_Target(_float fTimeDelta);
	void						Move_To_PreArm(_float _fTimeDelta);

private:
	pair<ARM_DATA*, SUB_DATA*>* Find_ArmData(_wstring _wszArmTag);

public:
	static CCamera_Target*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg);
	virtual void				Free() override;
};

END
