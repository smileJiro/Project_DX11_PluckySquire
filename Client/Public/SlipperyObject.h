#pragma once
#include "ModelObject.h"


BEGIN(Client)
class CSlipperyObject :
    public CModelObject
{
public:
	CSlipperyObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CSlipperyObject(const CSlipperyObject& _Prototype);
	virtual ~CSlipperyObject() = default;

public:
	virtual HRESULT Initialize(void* _pArg) override;
	virtual void Update(_float _fTimeDelta) override;
	virtual HRESULT Render() override;
public:
	virtual void On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)override;
	virtual void On_Collision2D_Stay(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)override;
	virtual void On_Collision2D_Exit(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)override;

	void Start_Slip(_vector _vDirection);
private:
	_bool m_bSlip = false;
	_vector m_vSlipDirection = {};
public:
	static CSlipperyObject* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;
};

END