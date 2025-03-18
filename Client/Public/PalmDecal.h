#pragma once
#include "ModelObject.h"
#include "Interactable.h"
BEGIN(Client)
class IStoppable;
class CSection_Manager;
class CPalmDecal :
	public CModelObject, public IInteractable
{
private:
	explicit CPalmDecal(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CPalmDecal(const CPalmDecal& _Prototype);
	virtual ~CPalmDecal() = default;

public:
	virtual HRESULT Initialize(void* _pArg) override;
	virtual void Priority_Update(_float _fTimeDelta) override;
	virtual void Update(_float _fTimeDelta) override;
	virtual void Late_Update(_float _fTimeDelta) override;
	virtual HRESULT	Render() override;
public:
	virtual _matrix		Get_FinalWorldMatrix() override;
	virtual void On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)override;
	virtual void On_Collision2D_Stay(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)override;
	virtual void On_Collision2D_Exit(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)override;
	void Place(_fvector _v2DPos, _fvector _v2DDir);
	void Erase();
public:
	void Interact(CPlayer* _pUser) override;
	_bool Is_Interactable(CPlayer* _pUser) override;
	_float Get_Distance(COORDINATE _eCoord, CPlayer* _pUser) override;

public:
	virtual void Active_OnEnable();
	virtual void Active_OnDisable();

private:
	set<CGameObject*> m_StoppedObjects;
	CSection_Manager* m_pSectionMgr = nullptr;
	_bool m_bPlacedFrame = false;
public:
	static CPalmDecal* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void			Free() override;


};

END