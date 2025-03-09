#pragma once
#include "UI.h"
#include "Section_Manager.h"

BEGIN(Engine)
class CShader;
class CModel;
class CVIBuffer_Collider;
END

BEGIN(Client)

class CShopPanel : public CUI
{
public:
	struct ShopUI : public tagUIDesc
	{
		_int	iSkillLevel = { 0 };
		_int	iPrice = { 0 };
		_wstring	strName = TEXT(" ");
		_wstring	strDialogue = TEXT(" ");
	};


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
	void					isRender() { false == m_isRender ? m_isRender = true : m_isRender = false; };
	void					isFontPrint() { false == m_isOpenPanel ? m_isOpenPanel = true : m_isOpenPanel = false; };
	_bool					isInPanel(_float2 _vMousePos);
	_int					isInPanelItem(_float2 _vMousePos);
	void					Update_KeyInput(_float _fTimeDelta, _int _index = -1);
	void					ChangeState_Panel(_float _fTimeDelta, _bool _isOpenState);
	HRESULT					Ready_ShopPannel(LEVEL_ID _eCurLevel, const _wstring& _strLayerTag, _float2 _vRTSize);
	HRESULT					Ready_Item(LEVEL_ID _eCurLevel, const _wstring& _strLayerTag);
	void					Cal_ShopPartPos(_float2 _vRTSize, _float2 _vBGPos);
	void					instruct_ChildUpdate(_float _fTimeDelta);
	

protected:
	virtual HRESULT			Ready_Components() override;

protected:
	CUI::SHOPPANEL			Get_ShopPanel() { return m_eShopPanel; }

public:
	static CShopPanel*		Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject*	Clone(void* _pArg);
	virtual void			Free() override;
	HRESULT					Cleanup_DeadReferences() { return S_OK; };

public:

	_bool					Get_isChooseItem() { return m_isChooseItem; }
	//void					Set_isChooseItem(_bool _Choose) { m_isChooseItem = _Choose; }




protected:

	SKILLSHOP			m_eSkillShopIcon = SKILLSHOP_END;
	_bool				m_isChooseItem = { false };
	_bool				m_isPreState = { false };


private:
	_bool				m_isOpenPanel = { false };
	_int				m_iPreindex = { 0 };
	_bool				m_isConfirmItem = { false };
	_uint				m_iConfirmItemIndex = { 0 };





};

END

