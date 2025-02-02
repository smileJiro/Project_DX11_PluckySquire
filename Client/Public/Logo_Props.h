#pragma once
#include "UI.h"

BEGIN(Engine)
class CShader;
class CModel;
class CVIBuffer_Collider;
END

BEGIN(Client)
class CLogo_Props final : public CUI
{

protected:
	explicit CLogo_Props(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CLogo_Props(const CLogo_Props& _Prototype);
	virtual ~CLogo_Props() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* _pArg) override;
	virtual void			Priority_Update(_float _fTimeDelta) override;
	virtual void			Update(_float _fTimeDelta) override;
	virtual void			Late_Update(_float _fTimeDelta) override;
	virtual HRESULT			Render() override;
	virtual void			Child_Update(_float _fTimeDelta) ;
	virtual void			Child_LateUpdate(_float _fTimeDelta) ;

private:
	_tchar					m_tTest[MAX_PATH] = {};
	_int					m_iTextureCount;

	

protected:
	virtual HRESULT			Ready_Components() override;
	void					isRender() { false == m_isRender ? m_isRender = true : m_isRender = false; }


public:
	static CLogo_Props*		Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject*	Clone(void* _pArg);
	virtual void			Free() override;
	HRESULT					Cleanup_DeadReferences() override;

};

END