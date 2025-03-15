#pragma once
#include "ContainerObject.h"

BEGIN(Client)
class CDynamicCastleGate :
    public CContainerObject
{
public:
	enum COLLAPSE_STATE
	{
		NONE,
		COLLAPSING,
		COLLAPSED,
	};
	enum CASTLEDOOR_PARTS
	{
		CASTL_PART_GATE,
		CASTL_PART_SKSP,
		CASTL_PART_LAST
	};
protected:
	explicit CDynamicCastleGate(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CDynamicCastleGate(const CDynamicCastleGate& _Prototype);
	virtual ~CDynamicCastleGate() = default;
public:
	HRESULT Initialize(void* _pArg);
	virtual void			Update(_float _fTimeDelta) override;
	virtual HRESULT			Render() override;

	virtual void OnContact_Modify(const COLL_INFO& _0, const COLL_INFO& _1, CModifiableContacts& _ModifiableContacts, _bool _bIm0)override;
	virtual void OnContact_Enter(const COLL_INFO& _My, const COLL_INFO& _Other, const vector<PxContactPairPoint>& _ContactPointDatas) override;

	void Collapse();

	virtual void WorldMap_ReCapture();

private:
	HRESULT Ready_PartObjects();
	void Set_State(COLLAPSE_STATE _eState);
	void Check_StateChange();
private:
	COLLAPSE_STATE m_eCurState = NONE;
	COLLAPSE_STATE m_ePrevState = NONE;

	_float m_fGateHalfHeight = 2.5f;
	_float m_fGateHalfWidth = 1.5;
	_float m_fGateHalfThick = 0.2f;
	_float3 m_fMassLocal = { 0.f,1.f,0.f };
	_vector m_vCollapseTorque = { 0.f,0.f,0.f };

	_float m_fPushTimeAcc = 0.f;
	_float m_fPushTime = 0.35f;
public:
	static CDynamicCastleGate* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void			Free() override;
};

END