#pragma once
#include "Playable.h"
#include "AnimEventReceiver.h"

BEGIN(Engine)
class CCollider;

class CModelObject;
END

BEGIN(Client)
class CSection_Manager;
class CPlayer;
class CDefenderPlayer :
    public CPlayable
{
public:
	enum DEFENDER_PART
	{
		DEFENDER_PART_CROSSHAIR= PART_LAST,
		DEFENDER_PART_LAST,
	};
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
	typedef struct tagDefenderPlayerDesc : public CCharacter::CHARACTER_DESC
	{
		CPlayer* pOriginalPlayer = nullptr;
	}DEFENDERPLAYER_DESC;
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

	void Set_Direction(T_DIRECTION _eTDir);
	void Key_Input(_float _fTimeDelta);
	void Start_Game();
private:
	HRESULT	Ready_Components();
	HRESULT	Ready_PartObjects();
	HRESULT	Ready_BulletPool();
public:
	void On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx);
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
	T_DIRECTION m_eTDirection = T_DIRECTION::RIGHT;
	CPlayer* m_pOriginalPlayer = nullptr;
	CSection_Manager* m_pSection_Manager = nullptr;
	class CPooling_Manager* m_pPoolMgr = nullptr;
	_float4 m_vRightShootQuaternion = { 0.f,0.f,0.f,1.f };
	_float4 m_vLeftShootQuaternion = { 0.f,0.f,0.f,1.f };
	//PARTS
	CModelObject* m_pBody = nullptr;
	CModelObject* m_pCrossHair = nullptr;

	//COMPONENTS
	CCollider* m_pBody2DColliderCom = nullptr;

	//STATS
	_float m_fMoveSpeed = 500.f;
	_float m_fShootDelay = 0.1f;
	_float m_fShootTimeAcc = 0.f;

	//CONTROL
	_float m_fCrossHairMoveRange = 300.f;
	_float m_fCrossHairMoveSpeed = 100.f;
	_float m_fCrossHairPosition = 0.f;
	_float m_fCrossHairYOffset = 20.f;
	_vector m_fBarrelOffset = { 80.f, 20.f };
public:
	static CDefenderPlayer* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;
};

END