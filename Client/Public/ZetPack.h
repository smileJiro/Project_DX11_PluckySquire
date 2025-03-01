#pragma once
#include "ModelObject.h"

BEGIN(Client)
class CZetPack :
    public CModelObject
{
public:
	typedef struct tagZetPackDesc : public CModelObject::MODELOBJECT_DESC
	{
		CPlayer* pPlayer = nullptr;
	}ZETPACK_DESC;
private:
	explicit CZetPack(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CZetPack(const CZetPack& _Prototype);
	virtual ~CZetPack() = default;

public:
	virtual HRESULT Initialize(void* _pArg) override;
	void Update(_float _fTimeDelta) override;
	void Late_Update(_float _fTimeDelta) override;
	virtual HRESULT			Render() override;

	void ReFuel();
	//추진
	void Propel(_float _fTimeDelta);
	//역추진
	void Retropropulsion();
private:
	CPlayer* m_pPlayer = nullptr;
	_float m_fFuel = 2.5f;
	_float m_fMaxFuel = 2.5f;
	_float m_fFuelConsumption = 1.0f;

	_float m_fMaxForeceRatio = 1.0f;
	_float m_fMinForeceRatio = 0.5f;
	_float m_fPropelForce = 1900.f;
public:
	static CZetPack* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void			Free() override;
};

END