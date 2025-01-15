#pragma once

#include "Engine_Defines.h"
#include "Camera.h"

BEGIN(Engine)

class CCameraArm final : public CBase
{
public:
	typedef struct
	{
		_float3				vArm = { 0.f, 0.f, -1.f };
		_float3				vPosOffset = { 0.f, 0.f, 0.f };
		_float3				vRotation;
		_float				fLength = 1.f;
		_wstring			wszArmTag = {};

		const _float4x4*	pTargetWorldMatrix = { nullptr };
	}CAMERA_ARM_DESC;

private:
	CCameraArm(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCameraArm(const CCameraArm& Prototype);
	virtual ~CCameraArm() = default;


public:
	HRESULT				Initialize_Prototype();
	HRESULT				Initialize(void* pArg);
	void				Priority_Update(_float fTimeDelta);
	void				Update(_float fTimeDelta);
	void				Late_Update(_float fTimeDelta);
	HRESULT				Render();

public:
	_wstring			Get_ArmTag() { return m_wszArmTag; }

private:
	_wstring			m_wszArmTag = {};
	_float3				m_vArm = {};
	_float3				m_vPosOffset = {};
	_float3				m_vRotation = {};
	_float				m_fLength = {};

	const _float4x4*	m_pTargetWorldMatrix = { nullptr };

private:
	void				Set_CameraPos(_float fTimeDelta);

private:
	static CCameraArm*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void		Free() override;
};

END