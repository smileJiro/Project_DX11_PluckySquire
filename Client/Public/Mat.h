#pragma once
#include "ModelObject.h"

BEGIN(Client)
class CMat final: public CModelObject
{
private:
	CMat(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CMat(const CMat& _Prototype);
	virtual ~CMat() = default;

public:
	HRESULT Initialize_Prototype();
	HRESULT Initialize(void* _pArg);
	virtual void	Late_Update(_float _fTimeDelta) override;
private:

public:
	static CMat* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CGameObject* Clone(void* _pArg) override;
	void Free()override;

};

END