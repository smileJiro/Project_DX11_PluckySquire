#pragma once

#include "Camera_Tool_Defines.h"
#include "Camera.h"

BEGIN(Engine)
class CCameraArm;
END

BEGIN(Camera_Tool)

class CCamera_Target final : public CCamera
{
public:
	enum CAMERA_MODE { DEFAULT, MOVE_TO_NEXTARM, CAMERA_MODE_END };
	enum TARGET_STATE { TARGET_RIGHT, TARGET_UP, TARGET_LOOK, TARGET_POS, TARGET_STATE_END };

	typedef struct tagCameraTargetDesc : public CCamera::CAMERA_DESC
	{
		CAMERA_MODE				eCameraMode;
		_float3					vAtOffset;
		_float					fSmoothSpeed;

		const _float4x4* pTargetWorldMatrix = { nullptr };
	}CAMERA_TARGET_DESC;


private:
	CCamera_Target(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CCamera_Target(const CCamera_Target& _Prototype);
	virtual ~CCamera_Target() = default;

public:
	virtual HRESULT				Initialize_Prototype() override;
	virtual HRESULT				Initialize(void* _pArg) override;
	virtual void				Priority_Update(_float _fTimeDelta) override;
	virtual void				Update(_float _fTimeDelta) override;
	virtual void				Late_Update(_float _fTimeDelta) override;

#ifdef _DEBUG
	_float3						Get_ArmRotation();
#endif

public:
	void						Add_Arm(CCameraArm* _pCameraArm);
	void						Set_CameraMode(_uint _iCameraMode, _int _iNextCameraMode = -1) { m_eCameraMode = (CAMERA_MODE)_iCameraMode; m_iNextCameraMode = _iNextCameraMode; }
	virtual void				Change_Target(const _float4x4* _pTargetWorldMatrix) override;

public:
	void						Set_NextArmData(ARM_DATA* _pArmData, SUB_DATA* _pSubData);		// Event 처리 하면 사라지고 바로 Arm에 넣을 수도

private:
	const _float4x4*			m_pTargetWorldMatrix = { nullptr };

	CAMERA_MODE					m_eCameraMode = { CAMERA_MODE_END };
	_int						m_iNextCameraMode = { -1 };

	_float						m_fSmoothSpeed = {};

	// Arm
	CCameraArm*					m_pArm = { nullptr };

	// TargetPos
	_float3						m_vTargetPos = {};

	//// Sub Data
	//SUB_DATA*					m_pCurSubData = { nullptr };

private:
	void						Key_Input(_float _fTimeDelta);

	void						Action_Mode(_float _fTimeDelta);
	void						Defualt_Move(_float _fTimeDelta);
	void						Move_To_NextArm(_float _fTimeDelta);
	void						Look_Target(_float _fTimeDelta);

	_vector						Calculate_CameraPos();

public:
	static CCamera_Target*		Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject*		Clone(void* _pArg);
	virtual void				Free() override;
};

END