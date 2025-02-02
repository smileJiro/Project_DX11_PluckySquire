#pragma once
#include "UI.h"

BEGIN(Engine)
class CShader;
class CModel;
class CVIBuffer_Collider;
END

BEGIN(Client)
class CLogo_BG final : public CUI
{

protected:
	explicit CLogo_BG(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CLogo_BG(const CLogo_BG& _Prototype);
	virtual ~CLogo_BG() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* _pArg) override;
	virtual void			Priority_Update(_float _fTimeDelta) override;
	virtual void			Update(_float _fTimeDelta) override;
	virtual void			Late_Update(_float _fTimeDelta) override;
	virtual HRESULT			Render() override;


private:
	_tchar					m_tTest[MAX_PATH] = {};
	_bool					m_isLogoChooseStage = { false };

protected:
	_bool					m_isRender = { false };

private:
	void					ProcesssByIndex(_int _iIndex );


protected:
	virtual HRESULT			Ready_Components() override;
	void					isRender() { false == m_isRender ? m_isRender = true : m_isRender = false; }

public:
	static CLogo_BG*		Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject*	Clone(void* _pArg);
	virtual void			Free() override;
	HRESULT					Cleanup_DeadReferences() override;

};

END

