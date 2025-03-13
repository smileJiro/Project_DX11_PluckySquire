#pragma once
#include "ModelObject.h"
#include "Interactable.h"
BEGIN(Client)
class CEvictedSword :
    public CModelObject, public IInteractable
{
public:
	enum ANIMATION_STATE
	{
		EVICT = 0,
		IDLE,
	};
private:
	explicit CEvictedSword(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CEvictedSword(const CEvictedSword& _Prototype);
	virtual ~CEvictedSword() = default;

public:
	virtual HRESULT Initialize(void* _pArg) override;
	virtual void Update(_float _fTimeDelta)override;
	virtual HRESULT Render()override;
public:
	void On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx);
private:
	_float2 m_vSwordOffset = { -250.f, 64.f };

public:
	static CEvictedSword* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void			Free() override;

	// IInteractable을(를) 통해 상속됨
	void Interact(CPlayer* _pUser) override;
	_bool Is_Interactable(CPlayer* _pUser) override;
	_float Get_Distance(COORDINATE _eCoord, CPlayer* _pUser) override;
};

END