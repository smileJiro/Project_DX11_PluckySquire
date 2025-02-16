#pragma once
#include "ModelObject.h"
#include "Player.h"
BEGIN(Engine)
class CCollider;
END
BEGIN(Client)
class CPlayerSword :
    public CModelObject
{
public:
	enum SWORD_STATE
	{
		HANDLING,
		FLYING,
		STUCK,
		SWORD_STATE_LAST
	};
public:
	typedef struct tagPlayerSwordDesc : public CModelObject::MODELOBJECT_DESC
	{
		_float fThrowForce = 10.f;
		_float fHomingForce = 100.f;
		_float fRotationForce = 50.f;
		class CPlayer* pParent = nullptr;
	}PLAYER_SWORD_DESC;
private:
	CPlayerSword(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CPlayerSword(const CPlayerSword& _Prototype);
	virtual ~CPlayerSword() = default;

public:

	virtual HRESULT	Initialize_Prototype() override;
	virtual HRESULT Initialize(void* _pArg) override;
	virtual void			Update(_float _fTimeDelta) override;
	virtual void			Late_Update(_float _fTimeDelta) override;
	virtual HRESULT	Render() override;


	virtual void OnTrigger_Enter(const COLL_INFO& _My, const COLL_INFO& _Other) override;
	virtual void OnTrigger_Stay(const COLL_INFO& _My, const COLL_INFO& _Other)override;
	virtual void OnTrigger_Exit(const COLL_INFO& _My, const COLL_INFO& _Other)override;
	virtual void	On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)override;
	virtual void	On_Collision2D_Stay(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)override;
	virtual void	On_Collision2D_Exit(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)override;
	virtual void Active_OnEnable() override;
	virtual void Active_OnDisable() override;

	virtual HRESULT	 Change_Coordinate(COORDINATE _eCoordinate, _float3* _pNewPosition = nullptr) override;

	void Throw(_fvector _vDirection);
	void Switch_Grip(_bool _bForehand);

	void Set_State(SWORD_STATE _eNewState);
	void On_StateChange();
	void Attack(CGameObject* _pVictim);
	//Set
	void Set_AttackEnable(_bool _bOn, CPlayer::ATTACK_TYPE _eAttackType = CPlayer::ATTACK_TYPE::ATTACK_TYPE_NORMAL1);
	//Get
	_bool Is_AttackEnable();
	_bool Is_Flying() { return FLYING == m_eCurrentState; }
	_bool Is_ComingBack() { return Is_Flying() && 0 > m_fOutingForce; }
	_bool Is_Outing() { return Is_Flying() && 0 < m_fOutingForce; }
	_bool Is_SwordHandling() { return HANDLING == m_eCurrentState; }
	_vector Get_LookDirection();
private:
	_bool m_bAttackEnable = false;
	_float m_fThrowingPower3D = 30.f;
	_float m_fThrowingPower2D = 1200.f;
	_vector m_vThrowDirection = {};
	_vector m_vStuckDirection = {};
	//¹ÛÀ¸·Î ³ª°¡·Á´Â Èû
	_float m_fOutingForce = 1.f;
	//²ø¾îµéÀÌ´Â Èû
	_float m_fCentripetalForce3D = 30.f;
	_float m_fCentripetalForce2D = 1000.f;
	_float m_fRotationForce3D = 50.f;
	_float m_f3DKnockBackPower = 12.f;
	_float m_f2DKnockBackPower = 50.f;

	_float m_f3DNormalAttackRange = 0.5f;
	_float m_f3DNormalAttackZOffset = 0.5f;
	_float m_f3DJumpAttackRange = 1.5f;
	_float m_f3DJumpAttackZOffset = 0.0f;

	SWORD_STATE m_eCurrentState = HANDLING;
	SWORD_STATE m_ePastState = HANDLING;

	class CPlayer* m_pPlayer = nullptr;
	CCollider* m_pBody2DColliderCom = nullptr;

	set<CGameObject*> m_AttckedObjects;
public:
	static CPlayerSword* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void			Free() override;
};

END