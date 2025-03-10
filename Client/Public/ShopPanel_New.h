#pragma once
#include "UI.h"
#include "Section_Manager.h"

BEGIN(Engine)
class CShader;
class CModel;
class CVIBuffer_Collider;
END

BEGIN(Client)

class CShopPanel_New : public CUI
{
public:
	struct ShopUI : public tagUIDesc
	{
		_int		iSkillLevel = { 0 };
		_int		iPrice = { -1 };
		_wstring	strName = TEXT(" ");
		_wstring	strDialogue = TEXT(" ");
	};


protected:
	explicit CShopPanel_New(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CShopPanel_New(const CShopPanel_New& _Prototype);
	virtual ~CShopPanel_New() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* _pArg) override;
	virtual void			Priority_Update(_float _fTimeDelta) override;
	virtual void			Update(_float _fTimeDelta) override;
	virtual void			Late_Update(_float _fTimeDelta) override;
	virtual HRESULT			Render(_int _iTextureindex = 0, PASS_VTXPOSTEX _eShaderPass = PASS_VTXPOSTEX::DEFAULT) override;
	virtual HRESULT			ShopRender(_int _iTextureindex = 0, PASS_VTXPOSTEX _eShaderPass = PASS_VTXPOSTEX::DEFAULT);

private:
	//_uint					m_iPanelUI[SETTINGPANEL::SETTING_END];

private:
	void					isRender() { false == m_isRender ? m_isRender = true : m_isRender = false; };
	void					isFontPrint() { false == m_isOpenPanel ? m_isOpenPanel = true : m_isOpenPanel = false; };
	//_bool					isInPanel(_float2 _vMousePos);
	//_int					isInPanelItem(_float2 _vMousePos);
	void					Update_KeyInput(_float _fTimeDelta);
	void					ChangeState_Panel(_float _fTimeDelta, _bool _isOpenState);

protected:
	void					Cal_ShopPartPos(_float2 _vRTSize, _float2 _vBGPos);

	
public:
	CUI::SHOPPANEL			Get_ShopPanel() { return m_eShopPanel; }

protected:
	virtual HRESULT			Ready_Components() override;

protected:
	

public:
	static CShopPanel_New*		Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject*	Clone(void* _pArg);
	virtual void			Free() override;
	HRESULT					Cleanup_DeadReferences() { return S_OK; };

public:
	_bool					Get_isChooseItem() { return m_isChooseItem; }





protected:

	SKILLSHOP			m_eSkillShopIcon = SKILLSHOP_END;
	_bool				m_isChooseItem = { false };
	_bool				m_isPreState = { false };


private:
	_bool				m_isOpenPanel = { false };
	_int				m_iPreindex = { 0 };
	_bool				m_isConfirmItem = { false };
	_uint				m_iConfirmItemIndex = { 0 };

	_uint				m_iChooseIndex = { 0 };
	_bool               m_isOpenConfirmUI = { false };



	/* 테스트 용도*/
protected:
	_int				m_iItemCount = { -1 };

};

END

