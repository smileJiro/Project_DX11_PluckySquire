#pragma once
#include "ModelObject.h"
#include "Bombable.h"

BEGIN(Client)
class CMeteor :
	public CModelObject
{
public :
	typedef struct tagMeteor : CModelObject::MODELOBJECT_DESC
	{
		F_DIRECTION eDir = F_DIRECTION::RIGHT;

	}METEOR_DESC;
public:
	enum ANIMATION_STATE
	{
		IDLE,
	};
private:
	explicit CMeteor(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CMeteor(const CMeteor& _Prototype);
	virtual ~CMeteor() = default;

public:
	virtual HRESULT Initialize(void* _pArg) override;
	virtual HRESULT					Render() override;

	void Active_OnEnable() override;

public:
	virtual void On_Collision2D_Stay(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)override;
private:
	F_DIRECTION		m_eDir = F_DIRECTION::RIGHT;
	CCollider*		m_pExplosionCollider = nullptr;
	_float			m_f2DKnockBackPower = 700.f;
	_int			m_iAttackDamg = 2;
public:
	static CMeteor* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void			Free() override;

};

END