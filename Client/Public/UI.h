#pragma once
#include "UIObject.h"
#include "GameInstance.h"
//


BEGIN(Engine)
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)


class CUI : public Engine::CUIObject
{
public:
	enum PANEL { PANEL_SETTING, PANEL_SHOP, PANEL_LOGO, PANEL_END };


	enum SETTINGPANEL {
		SETTING_BG,
		SETTING_HEART,
		SETTING_BULB,
		SETTING_BACKESC,
		SETTING_BACKARROW,
		SETTING_ESCENTER,
		SETTING_END
	};

	enum SHOPPANEL {
		SHOP_BG,
		SHOP_DIALOGUEBG,
		SHOP_CHOOSEBG,
		SHOP_BULB,
		SHOP_BACKESC,
		SHOP_BACKARROW,
		SHOP_ENTER,
		SHOP_DEFAULT,
		SHOP_END
	};


public:
	typedef struct tagUIDesc : public CUIObject::UIOBJECT_DESC
	{
		const _wstring strProtoTypeTag;
		_uint			iTextureCount;
		SETTINGPANEL	eSettingPanelKind;
		SHOPPANEL		eShopPanelKind;

	}UIOBJDESC;

public:

	


public:
	explicit CUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CUI(const CUI& Prototype);
	virtual ~CUI() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render(_int _index = 0);

public:



protected:
	virtual HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();


public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;

protected:
	CShader*		m_pShaderComs[COORDINATE_LAST] = {nullptr};
	CTexture*		m_pTextureCom = {nullptr};
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
	_uint			m_iShaderPasses[COORDINATE_LAST] = {};


	_bool			m_isRender = { true };
	
	_float4			m_vColor = { 0.f, 0.f, 0.f, 1.f };
	_float			m_fRed = { 0.f };
	_float			m_fGreen = { 0.f };
	_float			m_fBlue = { 0.f };





};

END