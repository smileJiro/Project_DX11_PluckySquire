#pragma once
#include "Portal.h"
#include "Interactable.h"

BEGIN(Engine)
class CEffect_System;
class CModelObject;
class CCollider_Circle;
END

BEGIN(Client)
class CPlayerData_Manager;
class CPortal_Cannon final : public CPortal
{
public:
	enum ANIM_STATE
	{
		ANIM_ACTIVE_LOOP, //차징완료된 상태.
		ANIM_IDLE_LOOP, // 플레이어가 2D에 있는 상태
		ANIM_INACTIVE_LOOP,//플레이어가 3D에 있는 상태
		ANIM_INTO_ACTIVE, //플레이거 차지완료한 시점에 불림 -> 완료 시 ACTIVE_LOOP로 넘어감
		ALNIM_INTO_IDLE, //플레이어가 차징 취소. 완료 시 -> IDLE_LOOP
		ANIM_LAUNCH, // 발사. 완료 시 -> INTO_IDLE
		ANIM_LANDING_JIGGLE, // 포탈에 착지했을 때. 완료 시 -> IDLE_LOOP
		ANIM_LANDING_LOOP, // 착지할 포탈로 지정됐을 때
		ANIM_LAST
	};
	enum PORTAL_STATE
	{
		IDLE,
		CHARGE,
		INACTIVE,
		TARGETEED,
		GOAL_IN,
		PORTAL_STATE_LAST
	};
private:
	CPortal_Cannon(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CPortal_Cannon(const CPortal_Cannon& _Prototype);
	virtual ~CPortal_Cannon() = default;

public:
	virtual HRESULT			Initialize_Prototype();
	virtual HRESULT			Initialize(void* _pArg);
	virtual HRESULT			Init_Actor();
	virtual void 			Update(_float _fTimeDelta) override;
	HRESULT					Ready_PartObjects(PORTAL_DESC* _pDesc);

public:
	virtual void			Interact(CPlayer* _pUser);
	HRESULT					Change_Coordinate(COORDINATE _eCoordinate, _float3* _pNewPosition) override;
	void					Shoot_Ground(CPlayer* _pUser, E_DIRECTION _e2DLookDir);
	void					Shoot_Target(CPlayer* _pUser, CPortal_Cannon* _vTarget);
	//GET
	virtual _bool			Is_Interactable(CPlayer* _pUser);
	virtual _float			Get_Distance(COORDINATE _eCoord, CPlayer* _pUser);
	_vector					Get_ShootDirection() { return m_vShootDirection; }
	//SET
	virtual HRESULT			Setup_3D_Postion() override;
	void					Set_PortalState(PORTAL_STATE _eState);
	void					Set_ShootDirection(_fvector _vDirection);
	CPortal_Cannon*			Find_TargetPortal(_fvector _vDirection);
protected:
	void					On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx);

	virtual void			Active_OnDisable() override;
	virtual void			Active_OnEnable() override;
	virtual void			On_InteractionStart(CPlayer* _pPlayer)override;
	virtual void			On_Pressing(CPlayer* _pPlayer, _float _fTimeDelta) override;
	virtual void			On_ChargeComplete(CPlayer* _pPlayer) override;
	virtual void			On_InteractionEnd(CPlayer* _pPlayer) override;
	virtual void			On_InteractionCancel(CPlayer* _pPlayer) override;

	virtual void			On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)override;
	virtual void			On_Collision2D_Stay(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)override;
	virtual void			On_Collision2D_Exit(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)override;

private:
	CPlayerData_Manager* m_pPlayerDataMgr = nullptr;
	CCollider_Circle* m_pDetectCollider = nullptr;
	CModelObject* m_pBody = nullptr;
	_float m_fShootDistance = 6.f;
	_float m_fTargetDetectRange = 3000.f;
	_vector m_vShootDirection = {0.f,0.f,0.f};
	PORTAL_STATE m_ePortalState = PORTAL_STATE::PORTAL_STATE_LAST;
	COORDINATE m_ePlayerCoordiante = COORDINATE::COORDINATE_LAST;

	set<CPortal_Cannon*> m_AdjCannonPortals;
public:
	static CPortal_Cannon* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CGameObject*			Clone(void* _pArg) override;
	void					Free() override;
};

END