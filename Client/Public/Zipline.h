#pragma once
#include "ModelObject.h"
#include "Interactable.h"

BEGIN(Client)

class CZipline : public CModelObject, public IInteractable
{
protected:
	CZipline(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CZipline(const CZipline& _Prototype);
	virtual ~CZipline() = default;

public:
	virtual HRESULT Initialize(void* _pArg) override;
	virtual HRESULT Render() override;

protected:
	_bool			m_isUserAround = { false };
	_bool			m_isUserContact = { false };

public:
	static CZipline*		Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject*	Clone(void* _pArg) override;
	virtual void			Free() override;
};

END
