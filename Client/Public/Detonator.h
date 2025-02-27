#pragma once
#include "ModelObject.h"

BEGIN(Client)
class IBombable;
class CDetonator :
    public CModelObject
{
public:
	typedef struct tagDetonatorDesc : public CModelObject::MODELOBJECT_DESC
	{
	}DETONATOR_DESC;
private:
	explicit CDetonator(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CDetonator(const CDetonator& _Prototype);
	virtual ~CDetonator() = default;

public:
	virtual HRESULT Initialize(void* _pArg) override;

	void Detonate();
	void Set_Bombable(IBombable* _pBomb) { m_pBomb = _pBomb; }
	_bool Is_DetonationMode() { return nullptr != m_pBomb; }
private:
	IBombable* m_pBomb = nullptr;
public:
	static CDetonator* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void			Free() override;
};

END