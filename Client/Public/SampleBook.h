#pragma once
#include "ModelObject.h"
BEGIN(Client)

class CSampleBook final : public CModelObject
{
public :
	enum BOOK_PAGE_ACTION
	{
		PREVIOUS,
		NEXT,
		LAST
	};
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


	_bool					Book_Action(BOOK_PAGE_ACTION _eAction);

	vector<_uint>			m_ActivePageIndexs;
	_float					m_fTestOffset = -1.f;

	BOOK_PAGE_ACTION		m_eCurAction = LAST;
	

public:
	static CSampleBook* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void			Free() override;

};

END