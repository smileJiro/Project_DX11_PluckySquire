#pragma once
#include "ContainerObject.h"
#include "Interactable.h"

BEGIN(Engine)
class CCollider;
class CEffect_System;
END

BEGIN(Client)
class CPortal final : public CContainerObject, public virtual IInteractable
{
	enum PORTAL_PART
	{
		PORTAL_PART_2D = 1,
		PORTAL_PART_3D, // ¾È¾µµí !? 
		PORTAL_PART_EFFECT,
		PORTAL_PART_LAST
	};

public :
	typedef struct tagPortalDESC : public CContainerObject::CONTAINEROBJ_DESC
	{
		_float	fTriggerRadius;
		_uint	iPortalIndex = {};

	}PORTAL_DESC;
private:
	CPortal(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CPortal(const CPortal& _Prototype);
	virtual ~CPortal() = default;

public:
	virtual HRESULT			Initialize_Prototype(); 
	virtual HRESULT			Initialize(void* _pArg); 
	virtual void			Priority_Update(_float _fTimeDelta) override;
	virtual void			Update(_float fTimeDelta) override;
	virtual void			Late_Update(_float _fTimeDelta) override;
	virtual HRESULT			Render() override;

	HRESULT					Init_Actor();
	void					Use_Portal(CPlayer* _pUser, NORMAL_DIRECTION* _pOutNormal);
private:
	HRESULT					Ready_Components(PORTAL_DESC* _pDesc);
	HRESULT					Ready_PartObjects(PORTAL_DESC* _pDesc);
public:
	void						On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject) override;
	virtual void				OnTrigger_Enter(const COLL_INFO& _My, const COLL_INFO& _Other) override;
	virtual void				OnTrigger_Stay(const COLL_INFO& _My, const COLL_INFO& _Other) override;
	HRESULT					Change_Coordinate(COORDINATE _eCoordinate, _float3* _pNewPosition) override;

	virtual void			Interact(CPlayer* _pUser);
	virtual _bool			Is_Interactable(CPlayer* _pUser);
	virtual _float			Get_Distance(COORDINATE _eCoord, CPlayer* _pUser);

	virtual void Active_OnDisable() override;
	virtual void Active_OnEnable() override;

protected:
	virtual void	On_InteractionStart(CPlayer* _pPlayer);

private :
	CCollider*	m_pColliderCom = { nullptr };

	_float		m_fTriggerRadius = {};
	_uint		m_iPortalIndex = {};
	CEffect_System* m_pEffectSystem = { nullptr };


public:
	static CPortal* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CGameObject* Clone(void* _pArg) override;
	void					Free() override;
};

END