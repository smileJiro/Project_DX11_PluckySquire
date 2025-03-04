#pragma once
#include "ModelObject.h"
#include "Interactable.h"

BEGIN(Client)
class CPlayer;
class CDraggableObject :
    public CModelObject, public IInteractable
{
public:
	typedef struct tagDraggableDesc : public CModelObject::MODELOBJECT_DESC
	{
		_float3 vBoxHalfExtents = { 0.f,0.f,0.f };
		_float3 vBoxOffset = { 0.f,0.f,0.f };
	}DRAGGABLE_DESC;
protected:
	explicit CDraggableObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CDraggableObject(const CDraggableObject& _Prototype);
	virtual ~CDraggableObject() = default;

	HRESULT Initialize(void* _pArg);
	virtual void Update(_float _fTimeDelta) override;
	virtual void Late_Update(_float _fTimeDelta) override;
	virtual void OnContact_Modify(const COLL_INFO& _0, const COLL_INFO& _1, CModifiableContacts& _ModifiableContacts, _bool _bIm0)override;
	virtual void OnContact_Enter(const COLL_INFO& _My, const COLL_INFO& _Other, const vector<PxContactPairPoint>& _ContactPointDatas) override;
	virtual void OnContact_Stay(const COLL_INFO& _My, const COLL_INFO& _Other, const vector<PxContactPairPoint>& _ContactPointDatas) override;
	virtual void OnContact_Exit(const COLL_INFO& _My, const COLL_INFO& _Other, const vector<PxContactPairPoint>& _ContactPointDatas) override;
	virtual void OnTrigger_Enter(const COLL_INFO& _My, const COLL_INFO& _Other)override;
	virtual void OnTrigger_Stay(const COLL_INFO& _My, const COLL_INFO& _Other)override;
	virtual void OnTrigger_Exit(const COLL_INFO& _My, const COLL_INFO& _Other)override;

public:
	void Interact(CPlayer* _pUser) override;
	_bool Is_Interactable(CPlayer* _pUser) override;
	_float Get_Distance(COORDINATE _eCoord, CPlayer* _pUser) override;
	virtual void On_InteractionStart(CPlayer* _pPlayer) override;
	virtual void On_InteractionEnd(CPlayer* _pPlayer) override;
	void Set_Dragger(CPlayer* _pPlayer) { m_pDragger = _pPlayer; }
	void Move(_fvector _vForce, _float _fTimeDelta);

private:
	_bool m_bUserAround = false;
	_bool m_bUserContact = false;
	CPlayer* m_pDragger = nullptr;


public:
	static CDraggableObject* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;
};

END