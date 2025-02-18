#pragma once
#include "ModelObject.h"
BEGIN(Client)
class CPlayerBody :
    public CModelObject
{
private:
	explicit CPlayerBody(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CPlayerBody(const CPlayerBody& _Prototype);
	virtual ~CPlayerBody() = default;

public:
	void Update(_float _fTimeDelta) override;
	void Late_Update(_float _fTimeDelta) override;
	virtual HRESULT			Render() override;
public:
	static CModelObject* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void			Free() override;
};

END