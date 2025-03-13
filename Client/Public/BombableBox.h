#pragma once
#include "2DMapWordObject.h"
#include "Bombable.h"


BEGIN(Client)
class CBombableBox :
	public C2DMapWordObject, public IBombable
{
public:
	enum ANIM_STATE
	{
		CRACKED,
		BOMB_APPEAR,
		BOMB_LOOP,
		EXPLODE,
		SOLID,
	};
	enum STATE
	{
		STATE_BOMBIDLE,
		STATE_DROP_START,
		STATE_DROPED,
		STATE_EXPLODE,
		STATE_IDLE,
		STATE_LAST,

	};
public:
	CBombableBox(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CBombableBox(const CBombableBox& _Prototype);
	virtual ~CBombableBox() = default;
public:
	virtual HRESULT Initialize(void* _pArg) override;
	virtual void Update(_float _fTimeDelta)override;
public:
	void On_Detonated();
	void On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx);
	virtual void On_Collision2D_Stay(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)override;
	virtual _bool	Action_Execute(_uint _iControllerIndex, _uint _iContainerIndex, _uint _iWordIndex) override;

	virtual HRESULT	Render() override;

private:
	void Set_State(STATE _eState);

private:
	STATE m_eState = STATE::STATE_IDLE;
public:
	static CBombableBox* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;


	// IBombable을(를) 통해 상속됨
	void Detonate() override;

};

END