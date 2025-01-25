#pragma once
#include "UI.h"

BEGIN(Engine)
class CShader;
class CModel;
class CVIBuffer_Collider;
END

BEGIN(Client)
class CLogo_Choose final : public CUI
{

protected:
	explicit CLogo_Choose(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CLogo_Choose(const CLogo_Choose& _Prototype);
	virtual ~CLogo_Choose() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* _pArg) override;
	virtual void			Priority_Update(_float _fTimeDelta) override;
	virtual void			Update(_float _fTimeDelta) override;
	virtual void			Late_Update(_float _fTimeDelta) override;
	virtual HRESULT			Render(_int _iTextureindex = 0, PASS_VTXPOSTEX _eShaderPass = PASS_VTXPOSTEX::DEFAULT) override;


private:
	_tchar					m_tTest[MAX_PATH] = {};

	

protected:
	virtual HRESULT			Ready_Components() override;
	void					isRender() { false == m_isRender ? m_isRender = true : m_isRender = false; }


public:
	static CLogo_Choose*		Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject*	Clone(void* _pArg);
	virtual void			Free() override;
	HRESULT					Cleanup_DeadReferences() override;

};

END