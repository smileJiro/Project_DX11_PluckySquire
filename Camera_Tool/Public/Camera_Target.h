#pragma once

#include "Camera_Tool_Defines.h"
#include "Camera.h"

BEGIN(Camera_Tool)

class CCamera_Target final : public CCamera
{
	typedef struct tagCameraTargetDesc : public CCamera::CAMERA_DESC
	{
		//CAMERA_MODE			eCameraMode;
		_float3				vArm;
		_float				fSmoothSpeed;
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
	static CCamera_Target*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg);
	virtual void				Free() override;
};

END