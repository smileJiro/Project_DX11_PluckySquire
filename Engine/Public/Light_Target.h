#pragma once
#include "Light.h"
/* 1. Target Light는 Owner의 위치값을 기준을 자기 자신의 위치를 업데이트한다. */
/* 2. Offset을 가지며 Target Postion + Offset Postion == 광원의 최종 위치 */

BEGIN(Engine)
class CGameObject;
class ENGINE_DLL CLight_Target final : public CLight
{
private:
	CLight_Target(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, LIGHT_TYPE _eLightType);
	virtual ~CLight_Target() = default;

public:
	virtual HRESULT Initialize(const CONST_LIGHT& _LightDesc, CGameObject* _pTargetOwner, const _float3& _vOffsetPos, _bool _isNotClear);
	virtual void	Update(_float _fTimeDelta) override;

private:
	void			Chase_Target();

private:
	CGameObject*	m_pTargetOwner = nullptr;
	_float3			m_vOffsetPostion = {};

public:
	static CLight_Target* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const CONST_LIGHT& _LightDesc, LIGHT_TYPE _eLightType, CGameObject* _pTargetOwner, const _float3& _vOffsetPos, _bool _isNotClear = true);
	virtual void Free() override;
};
END
