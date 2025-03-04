#pragma once
#include "BombableObject.h"


BEGIN(Client)
class CSlipperyObject;
class CTiltSwapPusher :
    public CBombableObject
{
public:
	enum ANIM_STATE
	{
		ANIM_IDLE_RIGHT,
		ANIM_DROP_DOWN,
		ANIM_DROP_RIGHT,
		ANIM_EXPLODE_DOWN,
		ANIM_EXPLODE_RIGHT,
		ANIM_BOMBIDLE_DOWN,
		ANIM_BOMBIDLE_RIGHT,
		ANIM_IDLE_DOWN,
	};
	enum STATE
	{
		STATE_IDLE,
		STATE_DROP,
		STATE_EXPLODE,
		STATE_BOMBIDLE,
		STATE_LAST,

	};
	typedef struct tagTiltSwapPusherDesc : public CModelObject::MODELOBJECT_DESC
	{
		F_DIRECTION eLookDirection = F_DIRECTION::DOWN;
	}TILTSWAPPUSHER_DESC;
public:
	CTiltSwapPusher(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CTiltSwapPusher(const CTiltSwapPusher& _Prototype);
	virtual ~CTiltSwapPusher() = default;
public:
	virtual HRESULT Initialize(void* _pArg) override;
	virtual void Update(_float _fTimeDelta)override;
public:
	void On_Detonated() override;
	void On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx);
	virtual void On_Collision2D_Stay(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)override;

	virtual HRESULT	Render() override;

private	:
	void Set_State(STATE _eState);
	void Set_Direction(F_DIRECTION _eDirection);

protected:
	virtual void On_BombInstalled();
private:
	_float m_f2DKnockBackPower = 1500.f;
	F_DIRECTION m_eDirection = F_DIRECTION::DOWN;
	STATE m_eState = STATE::STATE_IDLE;
	set<CSlipperyObject*> m_PushedObjects;
public:
	static CTiltSwapPusher* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;

};

END