#pragma once
#include "ModelObject.h"
BEGIN(Map_Tool)

class CBook final : public CModelObject
{
private:
	CBook(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CBook(const CBook& _Prototype);
	virtual ~CBook() = default;
public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* _pArg) override;
	virtual void			Priority_Update(_float _fTimeDelta) override;
	virtual void			Update(_float _fTimeDelta) override;
	virtual void			Late_Update(_float _fTimeDelta) override;
	virtual HRESULT			Render() override;

public:
	static CBook* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void			Free() override;

};

END