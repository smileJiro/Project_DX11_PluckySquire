#pragma once

#include "Camera_Tool_Defines.h"
#include "Camera.h"

BEGIN(Camera_Tool)

class CCamera_Target final : public CCamera
{
public:
	enum CAMERA_MODE { DEFAULT, TURN, CAMERA_MODE_END };

	typedef struct tagCameraTargetDesc : public CCamera::CAMERA_DESC
	{
		CAMERA_MODE				eCameraMode;
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
	virtual HRESULT				Render() override;

public:
	virtual void				Set_CameraMode(_uint _iCameraMode, _int iNextCameraMode = -1) override { m_eCameraMode = (CAMERA_MODE)_iCameraMode; m_iNextCameraMode = iNextCameraMode; }

private:
	CAMERA_MODE					m_eCameraMode = { CAMERA_MODE_END };
	_int						m_iNextCameraMode = { -1 };

	_float						m_fSmoothSpeed = {};

private:
	void						Action_Mode(_float fTimeDelta);
	void						Move(_float fTimeDelta);

public:
	static CCamera_Target*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg);
	virtual void				Free() override;
};

END