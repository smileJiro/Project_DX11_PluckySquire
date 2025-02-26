#pragma once
#include "ModelObject.h"
#include "Player.h"
BEGIN(Engine)
class CCollider;
class CEffect_System;
END
BEGIN(Client)
class CPlayerSword :
    public CModelObject
{
public:
	enum SWORD_STATE
	{
		HANDLING,
		OUTING,
		RETRIEVING,
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
	virtual void	Update(_float _fTimeDelta) override;
	virtual void	Late_Update(_float _fTimeDelta) override;
	virtual HRESULT	Render() override;


	virtual void OnTrigger_Enter(const COLL_INFO& _My, const COLL_INFO& _Other) override;
	virtual void OnTrigger_Stay(const COLL_INFO& _My, const COLL_INFO& _Other)override;
	virtual void OnTrigger_Exit(const COLL_INFO& _My, const COLL_INFO& _Other)override;
	virtual void On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)override;
	virtual void On_Collision2D_Stay(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)override;
	virtual void On_Collision2D_Exit(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)override;
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
	_bool Is_Flying() { return OUTING == m_eCurrentState || RETRIEVING == m_eCurrentState; }
	_bool Is_ComingBack() { return RETRIEVING == m_eCurrentState;}
	_bool Is_Outing() { return OUTING == m_eCurrentState; }
	_bool Is_SwordHandling() { return HANDLING == m_eCurrentState; }
	_vector Get_LookDirection();
private:
	_bool m_bAttackEnable = false;
	_float m_f3DKnockBackPower = 12.f;
	_float m_f2DKnockBackPower = 50.f;
	set<CGameObject*> m_AttckedObjects;

	//칼 던지기 관련
	_float m_fOutingTime = 0.7f;
	_float m_fFlyingTimeAcc = 0.f;
	_vector m_vThrowDirection = {};
	_vector m_vStuckDirection = {};
	_vector m_vStuckPosition = {};
	_float m_fFlyingSpeed3D = 20.f;
	_float m_fFlyingSpeed2D = 1200.f;
	_float m_fRetriveRange3D = 0.5f;
	_float m_fRetriveRange2D = 30.f;
	_float m_fRotationForce3D = 100.f;

	//일반 공격 관련
	_float m_f3DNormalAttackRange = 0.5f;
	_float m_f3DNormalAttackZOffset = 0.5f;
	_float m_f3DJumpAttackRange = 1.5f;
	_float m_f3DJumpAttackZOffset = 0.0f;

	SWORD_STATE m_eCurrentState = HANDLING;
	SWORD_STATE m_ePastState = HANDLING;

	class CPlayer* m_pPlayer = nullptr;
	CCollider* m_pBody2DColliderCom = nullptr;

	// VFX
	class CEffect_Trail*	m_pTrailEffect = { nullptr };
	class CEffect_System*	m_pThrowTrailEffect = { nullptr };
	class CEffect_Beam*		m_pBeamEffect = { nullptr };
public:
	static CPlayerSword* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void			Free() override;
};

END