#pragma once
#include "Blocker.h"
BEGIN(Client)
class CPlayerBlocker :
    public CBlocker
{
protected:
	CPlayerBlocker(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, COORDINATE _eCoord);
	CPlayerBlocker(const CPlayerBlocker& _Prototype);
	virtual ~CPlayerBlocker() = default;

public:
	virtual HRESULT			Initialize(void* _pArg) override;
	virtual void			Update(_float _fTimeDelta) override;
	virtual HRESULT Render()override;
public:
	/* 2d용 3d용 desc 따로만들고 둘중 하나 넣기? 프로토타입 시점에 2D 3D 프로토타입 각자 만들고, */
	static CPlayerBlocker* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, COORDINATE _eCoord);
	CGameObject* Clone(void* _pArg) override;
	void				Free() override;
};

END