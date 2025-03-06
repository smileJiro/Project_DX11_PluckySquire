#pragma once
#include "ModelObject.h"
BEGIN(Client)
class CSneak_DefaultObject :public CModelObject
{
protected:
	CSneak_DefaultObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CSneak_DefaultObject(const CSneak_DefaultObject& _Prototype);
	virtual ~CSneak_DefaultObject() = default;

public:
	virtual HRESULT Initialize(void* _pArg) override;
	virtual void	Move() {};

public:
	static CSneak_DefaultObject* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;
};

END