#pragma once
#include "ContainerObject.h"


BEGIN(Client)
class CSlipperyObject :
    public CContainerObject
{
public:
	typedef struct tagSlipperyDesc :public CContainerObject::CONTAINEROBJ_DESC
	{
		_uint iImpactCollisionFilter = 0;
	}SLIPPERY_DESC;
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


public:
	void Start_Slip(_vector _vDirection);
private:
	virtual void On_StartSlip(_vector _vDirection) {};
	virtual void On_Impact(CGameObject* _pOtherObject) {};
protected:
	_bool m_bSlip = false;
	_vector m_vSlipDirection = {};
	_uint m_iImpactCollisionFilter = 0;
public:
	static CSlipperyObject* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;
};

END