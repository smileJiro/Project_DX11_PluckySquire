#pragma once
#include "ModelObject.h"
#include "Interactable.h"

BEGIN(Client)
class CLunchBox :
    public CModelObject, public IInteractable
{
private:
	explicit CLunchBox(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CLunchBox (CLunchBox& _Prototype);
	virtual ~CLunchBox() = default;
public:
	HRESULT Initialize(void* _pArg);

	virtual void		Update(_float _fTimeDelta) override;
	virtual void		Late_Update(_float _fTimeDelta) override;
	virtual HRESULT			Render() override;

	virtual void OnTrigger_Enter(const COLL_INFO& _My, const COLL_INFO& _Other)override;
	virtual void OnTrigger_Stay(const COLL_INFO& _My, const COLL_INFO& _Other)override;
	virtual void OnTrigger_Exit(const COLL_INFO& _My, const COLL_INFO& _Other)override;

	// IInteractable을(를) 통해 상속됨
	virtual void Interact(CPlayer* _pUser) override;
	virtual _bool Is_Interactable(CPlayer* _pUser) override;
	virtual _float Get_Distance(COORDINATE _eCoord, CPlayer* _pUser) override;
	virtual void	On_Touched(CPlayer* _pPlayer) override;

private:
	_bool m_bPlayerInRange = false;
public:
	static CLunchBox* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void	Free() override;
};

END