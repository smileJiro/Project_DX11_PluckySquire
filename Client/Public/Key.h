#pragma once
#include "CarriableObject.h"

BEGIN(Client)

class CKey : public CCarriableObject
{
public:
	enum KEY_ANIMSTATE {OPEN, IDLE};

private:
	CKey(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CKey(const CKey& _Prototype);
	virtual ~CKey() = default;

public:
	HRESULT Initialize(void* _pArg);
	virtual void		Update(_float _fTimeDelta) override;
	virtual void		Late_Update(_float _fTimeDelta) override;
	virtual HRESULT		Render() override;

public:
	virtual void	On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject) override;

public:
	void			On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx);

public:
	static CKey* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void			Free() override;
};

END