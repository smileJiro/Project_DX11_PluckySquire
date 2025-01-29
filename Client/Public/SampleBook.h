#pragma once
#include "ModelObject.h"
BEGIN(Client)

class CSampleBook final : public CModelObject
{
private:
	CSampleBook(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CSampleBook(const CSampleBook& _Prototype);
	virtual ~CSampleBook() = default;
public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* _pArg) override;
	virtual void			Priority_Update(_float _fTimeDelta) override;
	virtual void			Update(_float _fTimeDelta) override;
	virtual void			Late_Update(_float _fTimeDelta) override;
	virtual HRESULT			Render() override;
	virtual HRESULT			Render_Shadow() override;

	vector<_uint>			m_ActivePageIndexs;
	_float					m_fTestOffset = -1.f;
public:
	static CSampleBook* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void			Free() override;

};

END