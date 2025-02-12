#pragma once
#include "ModelObject.h"
#include "Interactable.h"

BEGIN(Client)
class CPlayer;
class CCarriableObject :
    public CModelObject
{
private:
	explicit CCarriableObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CCarriableObject(const CCarriableObject& _Prototype);
	virtual ~CCarriableObject() = default;

public:
	virtual HRESULT			Render() override;

	HRESULT Initialize(void* _pArg);

public:
	HRESULT Set_Carrier(CPlayer* _pCarrier);
private:
	CPlayer* m_pCarrier = nullptr;
public:
	static CCarriableObject* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void			Free() override;
};

END