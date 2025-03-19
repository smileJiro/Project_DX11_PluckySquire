#pragma once
#include "ModelObject.h"

BEGIN(Client)
class CZip_C8 :
    public CModelObject
{
public:
	CZip_C8(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CZip_C8(const CZip_C8& _Prototype);
	virtual ~CZip_C8() = default;

public:
	virtual HRESULT Initialize(void* _pArg);
	virtual void Update(_float _fTimeDelta);
	virtual void Late_Update(_float _fTimeDelta);
	virtual HRESULT Render();

public:
	virtual void On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)override;
	virtual void On_Collision2D_Stay(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)override;
	virtual void On_Collision2D_Exit(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)override;

private:
	CPlayer* m_pPlayer = nullptr;
public:
	static CZip_C8* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg);
	virtual void Free();
\
};

END