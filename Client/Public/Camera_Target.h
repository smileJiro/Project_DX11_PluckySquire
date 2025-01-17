#pragma once
#include "Camera.h"

BEGIN(Engine)
class CCameraArm;
END

BEGIN(Client)
class CCamera_Target  : public CCamera
{
public:
	enum CAMERA_MODE { DEFAULT, TURN, CAMERA_MODE_END };

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
	virtual void				Set_CameraMode(_uint _iCameraMode, _int iNextCameraMode = -1) override { m_eCameraMode = (CAMERA_MODE)_iCameraMode; m_iNextCameraMode = iNextCameraMode; }
	virtual void				Set_TargetPos(_vector	_vTargetPos) { XMStoreFloat3(&m_vTargetPos, _vTargetPos); }
	virtual void				Change_Target(const _float4x4* _pTargetWorldMatrix) override;

public:
	void						Add_Arm(CCameraArm* _pCameraArm);

private:
	CAMERA_MODE					m_eCameraMode = { CAMERA_MODE_END };
	_int						m_iNextCameraMode = { -1 };

	_float						m_fSmoothSpeed = {};
	_float3						m_vAtOffset = {};
	_float3						m_vTargetPos = {};

	// Arm
	CCameraArm*					m_pArm = { nullptr };

private:
	void						Key_Input(_float _fTimeDelta);

	void						Action_Mode(_float fTimeDelta);
	void						Defualt_Move(_float fTimeDelta);
	void						Look_Target(_float fTimeDelta);

public:
	static CCamera_Target*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg);
	virtual void				Free() override;
};

END
