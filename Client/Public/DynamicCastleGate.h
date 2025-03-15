#pragma once
#include "ContainerObject.h"

BEGIN(Client)
class CDynamicCastleGate :
    public CContainerObject
{
public:
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

	void Collapse();

	virtual void WorldMap_ReCapture();

private:
	HRESULT Ready_PartObjects();

private:
	_float m_fGateHalfHeight = 3.f;
	_float m_fGateHalfWidth = 2.5;
	_float m_fGateHalfThick = 0.2f;
public:
	static CDynamicCastleGate* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void			Free() override;
};

END