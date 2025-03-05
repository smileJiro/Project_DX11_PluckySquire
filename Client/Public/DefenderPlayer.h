#pragma once
#include "Character.h"
#include "AnimEventReceiver.h"

BEGIN(Engine)
class CCollider;

class CModelObject;
END

BEGIN(Client)
class CSection_Manager;
class CDefenderPlayer :
    public CCharacter
{
public:
	enum class ANIM_STATE_CYBERJOT2D
	{
		CYBER2D_FLY_DOWN,
		CYBER2D_FLY_LEFT,
		CYBER2D_FLY_RIGHT,
		CYBER2D_FLY_UP,
		CYBER2D_FLYSHOT_DOWN,
		CYBER2D_FLYSHOT_LEFT,
		CYBER2D_FLYSHOT_RIGHT,
		CYBER2D_FLYSHOT_UP,
		CYBER2D_IDLE,
		CYBER2D_TRANSFORM_IN,
		CYBER2D_TRANSFORM_OUT,
		CYBER2D_SHOT,
		CYBER2D_DEATH,
		CYBVER2D_LAST,
	};
private:
	CDefenderPlayer(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CDefenderPlayer(const CDefenderPlayer& _Prototype);
	virtual ~CDefenderPlayer() = default;

public:
	virtual HRESULT		Initialize_Prototype() override;
	virtual HRESULT		Initialize(void* _pArg) override;
	virtual void		Priority_Update(_float _fTimeDelta) override;
	virtual void		Update(_float _fTimeDelta) override;
	virtual void		Late_Update(_float _fTimeDelta) override;
	virtual HRESULT		Render() override;

	void Key_Input(_float _fTimeDelta);
private:
	HRESULT					Ready_Components();
	HRESULT					Ready_PartObjects();
public:
	virtual void OnContact_Enter(const COLL_INFO& _My, const COLL_INFO& _Other, const vector<PxContactPairPoint>& _ContactPointDatas) override;
	virtual void OnContact_Stay(const COLL_INFO& _My, const COLL_INFO& _Other, const vector<PxContactPairPoint>& _ContactPointDatas) override;
	virtual void OnContact_Exit(const COLL_INFO& _My, const COLL_INFO& _Other, const vector<PxContactPairPoint>& _ContactPointDatas) override;
	virtual void OnTrigger_Enter(const COLL_INFO& _My, const COLL_INFO& _Other)override;
	virtual void OnTrigger_Stay(const COLL_INFO& _My, const COLL_INFO& _Other)override;
	virtual void OnTrigger_Exit(const COLL_INFO& _My, const COLL_INFO& _Other)override;
	virtual void On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)override;
	virtual void On_Collision2D_Stay(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)override;
	virtual void On_Collision2D_Exit(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)override;

public:
	void Shoot();
private:
	CSection_Manager* m_pSection_Manager = nullptr;
	//PARTS
	CModelObject* m_pBody = nullptr;
	CModelObject* m_pCrossHair = nullptr;

	//COMPONENTS
	CCollider* m_pBody2DColliderCom = nullptr;

	//STATS
	_float m_fMoveSpeed = 500.f;
	_float m_fShootDelay = 0.2f;
	_float m_fShootTimeAcc = 0.f;

	//CONTROL
	_float m_fCrossHairMoveRange = 1000.f;
	_float m_fCrossHairMoveSpeed = 300.f;
	_float m_fCrossHairPosition = 0.f;
public:
	static CDefenderPlayer* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;
};

END