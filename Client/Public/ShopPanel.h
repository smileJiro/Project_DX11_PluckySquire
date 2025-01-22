#pragma once
#include "UI.h"

BEGIN(Engine)
class CShader;
class CModel;
class CVIBuffer_Collider;
END

BEGIN(Client)

class CShopPanel : public CUI
{


protected:
	explicit CShopPanel(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CShopPanel(const CShopPanel& _Prototype);
	virtual ~CShopPanel() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* _pArg) override;
	virtual void			Priority_Update(_float _fTimeDelta) override;
	virtual void			Update(_float _fTimeDelta) override;
	virtual void			Late_Update(_float _fTimeDelta) override;
	virtual void			Child_Update(_float _fTimeDelta) {}
	virtual void			Child_LateUpdate(_float _fTimeDelta) {}
	virtual HRESULT			Render(_int _iTextureindex = 0, PASS_VTXPOSTEX _eShaderPass = PASS_VTXPOSTEX::DEFAULT) override;


private:
	//_uint					m_iPanelUI[SETTINGPANEL::SETTING_END];

private:
	void					isRender();
	void					isFontPrint();

protected:
	virtual HRESULT			Ready_Components() override;

protected:
	CUI::SHOPPANEL			Get_ShopPanel() { return m_eShopPanel; }

public:
	static CShopPanel*		Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject*	Clone(void* _pArg);
	virtual void			Free() override;
	HRESULT					Cleanup_DeadReferences() { return S_OK; };




protected:
	CUI::SHOPPANEL		m_eShopPanel = CUI::SHOPPANEL::SHOP_DEFAULT;

private:
	_bool				m_isOpenPanel = { false };


};

END

