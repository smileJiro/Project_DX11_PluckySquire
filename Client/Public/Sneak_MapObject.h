#pragma once
#include "Sneak_FlipObject.h"
BEGIN(Client)

class CSneak_MapObject :public CSneak_FlipObject
{
protected:
	CSneak_MapObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CSneak_MapObject(const CSneak_MapObject& _Prototype);
	virtual ~CSneak_MapObject() = default;

public:
	virtual HRESULT Initialize(void* _pArg) override;
	virtual void	Move() {};

public:
	static CSneak_MapObject* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;
};

END