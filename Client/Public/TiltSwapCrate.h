#pragma once
#include "ModelObject.h"
#include "Stoppable.h"
#include "SlipperyObject.h"

BEGIN(Client)
class CTiltSwapCrate :
	public CSlipperyObject
{
public:
	CTiltSwapCrate(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CTiltSwapCrate(const CTiltSwapCrate& _Prototype);
	virtual ~CTiltSwapCrate() = default;

public:
	virtual HRESULT Initialize(void* _pArg) override;
	virtual HRESULT Render() override;
private:
	HRESULT Ready_Parts();

private:
	_float2 m_vColliderSize = { 125.f, 120.f };
public:
	static CTiltSwapCrate* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;

};

END