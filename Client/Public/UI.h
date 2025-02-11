#pragma once
#include "UIObject.h"
#include "GameInstance.h"
#include "Player.h"
#include "2DModel.h"
//


BEGIN(Engine)
class CTexture;
class CVIBuffer_Rect;
class CModelObject;
class C2DModel;
END

BEGIN(Client)
class CUI : public Engine::CUIObject
{
public:
	enum PANEL { PANEL_SETTING, PANEL_SHOP, PANEL_LOGO, PANEL_END };
	enum SKILLSHOPBGS {SHOPBG_BG, SHOPBG_BADGE, SHOPBG_BULB, SHOPBG_END};

	enum SETTINGPANEL {
		SETTING_ESCGOBLIN,
		SETTING_BOOKMARK,
		SETTING_BG,
		SETTING_HEART,
		SETTING_BULB,
		SETTING_ESCBG,
		SETTING_BACKESC,
		SETTING_BACKARROW,
		SETTING_ENTERBG,
		SETTING_ESCENTER,
		SETTING_END
	};

	enum SHOPPANEL {
		SHOP_BG,
		SHOP_DIALOGUEBG,
		SHOP_CHOOSEBG,
		SHOP_BULB,
		SHOP_ESCBG,
		SHOP_BACKESC,
		SHOP_BACKARROW,
		SHOP_ENTERBG,
		SHOP_ENTER,
		SHOP_DEFAULT,
		SHOP_END
	};

	enum SKILLSHOP
	{
		SKILLSHOP_BG,
		SKILLSHOP_JUMPATTACKBADGE,
		SKILLSHOP_SPINATTACKBADGE,
		SKILLSHOP_ATTACKPLUSBADGE,
		SKILLSHOP_THROWATTBADGE,
		SKILLSHOP_SCROLLITEM,
		SKILLSHOP_BULB,
		SKILLSHOP_END,
		
	};

	enum LOGOPROP
	{
		LOGOPROP_BORDER,
		LOGOPROP_TREEINK0,
		LOGOPROP_TREEINK1,
		LOGOPROP_TREE,
		LOGOPROP_WHITEFLOWER0,
		LOGOPROP_WHITEFLOWER1,
		LOGOPROP_ENTER,

		LOGOPROP_END
	};


public:
	typedef struct tagUIDesc : public CUIObject::UIOBJECT_DESC
	{
		 _wstring strLayerTag;
		 _uint			iTextureCount = { 0 };					// 어떤 스킬의 어떤 레벨의 텍스쳐 이니?
		SETTINGPANEL	eSettingPanelKind = { SETTING_END };				// 세팅 패널에서 어떤 파츠(종류)이니
		SHOPPANEL		eShopPanelKind = { SHOP_DEFAULT };					// 상점 패널에서 어떤 파츠(종류)이니

		_uint			iSkillLevel = { 0 };				// 스킬의 레벨
		_uint			iShopItemCount = { 0 }; 			// 텍스쳐를 연결하기 위한
		SKILLSHOP		eShopSkillKind = { SKILLSHOP_END }; // 어떤 스킬이니?
		_bool			isChooseItem = { false };			// 너가 선택되어있니?

	}UIOBJDESC;

public:

	


public:
	explicit CUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CUI(const CUI& Prototype);
	virtual ~CUI() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* _pArg);
	virtual void Priority_Update(_float _fTimeDelta);
	virtual void Update(_float _fTimeDelta);
	virtual void Late_Update(_float _fTimeDelta);
	virtual HRESULT Render(_int _iTextureindex = 0, PASS_VTXPOSTEX _eShaderPass = PASS_VTXPOSTEX::DEFAULT);
	CController_Transform* Get_Transform() { return m_pControllerTransform; }




public:
	_float	Get_FY() { return m_fY; }
	_float	Get_FX() { return m_fX; }
	void	Set_FX(_float _fX) { m_fY = _fX; }
	void	Set_FY(_float _fY) { m_fY = _fY; }


protected:
	virtual HRESULT			Ready_Components();
	HRESULT					Bind_ShaderResources();
	void					Change_BookScale_ForShop(_float2 _vRTSize);


public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;

protected:
	CShader*		m_pShaderComs[COORDINATE_LAST] = {nullptr};
	CTexture*		m_pTextureCom = {nullptr};
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	C2DModel*		m_pModelCom = { nullptr };
	_uint			m_iShaderPasses[COORDINATE_LAST] = {};
	CUI::SHOPPANEL	m_eShopPanel = CUI::SHOPPANEL::SHOP_DEFAULT;

	_bool			m_isRender = { true };
	
	_float4			m_vColor = { 0.f, 0.f, 0.f, 1.f };
	_float			m_fRed = { 0.f };
	_float			m_fGreen = { 0.f };
	_float			m_fBlue = { 0.f };
	_tchar			m_tFont[MAX_PATH] = {};
	_float2			m_vOriginSize = {0.f, 0.f};




};

END