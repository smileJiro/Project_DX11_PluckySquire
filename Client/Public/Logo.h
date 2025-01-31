#pragma once
#include "UI.h"
#include "Section_Manager.h"

BEGIN(Engine)
class CShader;
class CModel;
class CVIBuffer_Collider;
END

BEGIN(Client)

class CLogo : public CUI
{


protected:
	explicit CLogo(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CLogo(const CLogo& _Prototype);
	virtual ~CLogo() = default;

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
	HRESULT					Ready_Props(LEVEL_ID _eCurLevel, const _wstring& _strLayerTag);

protected:
	virtual HRESULT			Ready_Components() override;

protected:
	CUI::SHOPPANEL			Get_ShopPanel() { return m_eShopPanel; }


public:
	static CLogo*			Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject*	Clone(void* _pArg);
	virtual void			Free() override;
	HRESULT					Cleanup_DeadReferences() { return S_OK; };

public:





protected:
	CUI::SHOPPANEL		m_eShopPanel = CUI::SHOPPANEL::SHOP_DEFAULT;
	SKILLSHOP			m_eSkillShopIcon = SKILLSHOP_END;
	_bool				m_isChooseItem = { false };


private:
	_bool				m_isOpenPanel = { false };
	_int				m_iPreindex = { 0 };
	_bool				m_isConfirmItem = { false };
	_uint				m_iConfirmItemIndex = { 0 };


};

END

