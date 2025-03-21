#include "stdafx.h"
#include "Logo_BackGround.h"
#include "GameInstance.h"

#include "ModelObject.h"
#include "Logo_ColorObject.h"

CLogo_BackGround::CLogo_BackGround(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CUI(_pDevice, _pContext)
{
}

CLogo_BackGround::CLogo_BackGround(const CLogo_BackGround& _Prototype)
	: CUI(_Prototype)
{
}

HRESULT CLogo_BackGround::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CLogo_BackGround::Initialize(void* _pArg)
{
	CLogo_BackGround::MAIN_LOGO_DESC* pDesc = static_cast<MAIN_LOGO_DESC*>(_pArg);

	m_vColor = pDesc->vColor;
	m_eBackGroundType = pDesc->iBackGroundMainType;
	m_iCurLevelID = pDesc->iCurLevelID;

	pDesc->fX = g_iWinSizeX >> 1;
	pDesc->fY = g_iWinSizeY >> 1;
	pDesc->fSizeX = g_iWinSizeX;
	pDesc->fSizeY = g_iWinSizeY;

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Objects()))
		return E_FAIL;


	switch (m_eBackGroundType)
	{
	case MAIN_JOT :
	{
		m_pGameInstance->Start_BGM(_wstring(L"LCD_MUS_TITLEMUSIC_FULL"), 40.f);
		m_pGameInstance->Start_SFX_Delay(TEXT("A_sfx_jot_title_screen"), 0.3f, 30.f, false);
	}
	break;

	case MAIN_HUMGRUMP :
	{
		
		m_pGameInstance->Start_SFX_Delay(TEXT("A_sfx_C9_OnceUponATime"), 0.15f, 30.f, false);
		m_pGameInstance->Start_BGM(_wstring(L"LCD_MUS_C09_MAGNIFICENTHUMGRUMP_TITLESCREEN_FULL"), 60.f);
	}
	break;
	}


	return S_OK;
}

void CLogo_BackGround::Priority_Update(_float _fTimeDelta)
{
	for (auto& Part : m_pBackGroundParts) {
		if(nullptr != Part && true == Part->Is_Active())
			Part->Priority_Update(_fTimeDelta);
	}
}

void CLogo_BackGround::Update(_float _fTimeDelta)
{
	if (KEY_DOWN(KEY::ENTER)) {
		m_pBackGroundParts[LOGO_FADEUI]->Start_FadeAlphaIn(2.f);
		m_pBackGroundParts[LOGO_FADEUI]->Set_Active(true);
		m_isBGFadeOut = true;
	}

	Check_RenderTiming(_fTimeDelta);
	Caculate_FadeValue(_fTimeDelta);
	BackGround_FadeOut(_fTimeDelta);

	for (auto& Part : m_pBackGroundParts) {
		if (nullptr != Part && true == Part->Is_Active())
			Part->Update(_fTimeDelta);
	}
}

void CLogo_BackGround::Late_Update(_float _fTimeDelta)
{
	__super::Late_Update(_fTimeDelta);

	for (auto& Part : m_pBackGroundParts) {
		if (nullptr != Part && true == Part->Is_Active()) {
			Part->Register_RenderGroup(RENDERGROUP::RG_3D, PRIORITY_3D::PR3D_UI);
			Part->Late_Update(_fTimeDelta);
		}
	};
}

HRESULT CLogo_BackGround::Render()
{

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColors", &m_vColor, sizeof(_float4))))
		return E_FAIL;

	_float fAlphaRatio = 1.f;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fSprite2DFadeAlphaRatio", &fAlphaRatio, sizeof(_float))))
		return E_FAIL;

	__super::Render(0, PASS_VTXPOSTEX::COLOR_ALPHA);

	Render_Font();

	return S_OK;
}

HRESULT CLogo_BackGround::Ready_Components()
{
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader_2D"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_Model_2D"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Logo_BG"),
		TEXT("Com_Texture_2D"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLogo_BackGround::Ready_Objects()
{
	// Character 积己
	CGameObject* pObject = nullptr;

	CModelObject::MODELOBJECT_DESC Desc = {};

	Desc.eStartCoord = COORDINATE_2D;
	Desc.isCoordChangeEnable = false;
	Desc.strShaderPrototypeTag_2D = TEXT("Prototype_Component_Shader_VtxPosTex");
	Desc.iShaderPass_2D = (_uint)PASS_VTXPOSTEX::SPRITE2D;

	Desc.tTransform2DDesc.vInitialScaling = _float3(1.3f, 1.3f, 1.f);

	Desc.iModelPrototypeLevelID_2D = LEVEL_STATIC;

	Desc.tTransform2DDesc.vInitialPosition = _float3(-210.f, 1.f, 0.f);
	Desc.iCurLevelID = m_iCurLevelID;

	switch (m_eBackGroundType) {
	case MAIN_JOT:
	{
		Desc.tTransform2DDesc.vInitialScaling = _float3(1.28f, 1.28f, 1.f);
		Desc.tTransform2DDesc.vInitialPosition = _float3(330.f, -45.f, 0.f);
		Desc.strModelPrototypeTag_2D = TEXT("Prototype_Component_Main_Jot");
	}
		
		break;
	case MAIN_HUMGRUMP:
		Desc.strModelPrototypeTag_2D = TEXT("Prototype_Component_Main_Humgrump");
		break;
	}

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_ModelObject"),
		m_iCurLevelID, TEXT("Layer_UI"), &pObject, &Desc)))
		return E_FAIL;



		


	m_pBackGroundParts[LOGO_CHARACTER] = static_cast<CModelObject*>(pObject);
	m_pBackGroundParts[LOGO_CHARACTER]->Register_OnAnimEndCallBack(bind(&CLogo_BackGround::On_End_Animation, this, placeholders::_1, placeholders::_2));
	Safe_AddRef(pObject);

	if (MAIN_JOT == m_eBackGroundType)
	{
		_vector vRight = m_pBackGroundParts[LOGO_CHARACTER]->Get_ControllerTransform()->Get_State(CTransform::STATE_RIGHT);
		m_pBackGroundParts[LOGO_CHARACTER]->Get_ControllerTransform()->Set_State(CTransform::STATE_RIGHT, -XMVectorAbs(vRight));
	}


	// Logo Text 积己
	CLogo_ColorObject::COLOROBJECT_DESC TextObjDesc = {};
	TextObjDesc.iCurLevelID = m_iCurLevelID;
	TextObjDesc.iColorObjectType = CLogo_ColorObject::LOGO_TEXTOBJECT;

	switch (m_eBackGroundType) 
	{
	case MAIN_JOT:
	{
		TextObjDesc.tTransform2DDesc.vInitialPosition = _float3(-15.f, 310.f, 0.f);
		TextObjDesc.tTransform2DDesc.vInitialScaling = _float3(g_iWinSizeX * 2.2f, g_iWinSizeY * 0.7f, 2.2f);
		TextObjDesc.strModelPrototypeTag_2D = TEXT("Prototype_Logo_TextObject_Jot");
		TextObjDesc.vColor = _float4(0.f, 0.f, 0.f, 1.f);

	}
		
		break;
	case MAIN_HUMGRUMP:
	{
		TextObjDesc.tTransform2DDesc.vInitialPosition = _float3(-30.f, 50.f, 0.f);
		TextObjDesc.tTransform2DDesc.vInitialScaling = _float3(g_iWinSizeX * 3.f, g_iWinSizeY, 3.2f);
		TextObjDesc.strModelPrototypeTag_2D = TEXT("Prototype_Logo_TextObject_Humgrump");
		TextObjDesc.vColor = _float4(1.f, 1.f, 0.f, 1.f);
	}
		
		break;
	}

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_Logo_ColorObject"),
		m_iCurLevelID, TEXT("Layer_UI"), &pObject, &TextObjDesc)))
		return E_FAIL;

	m_pBackGroundParts[LOGO_TEXT_OBJECT] = static_cast<CModelObject*>(pObject);
	pObject->Set_Active(false);
	Safe_AddRef(pObject);

	// Logo Text Button 积己
	TextObjDesc.iCurLevelID = m_iCurLevelID;
	TextObjDesc.tTransform2DDesc.vInitialPosition = _float3(480.f, -320.f, 1.f);
	TextObjDesc.tTransform2DDesc.vInitialScaling = _float3(g_iWinSizeX * 0.9f, g_iWinSizeY * 0.3f, 1.f);
	TextObjDesc.iColorObjectType = CLogo_ColorObject::LOGO_TEXTOBJECT_BUTTON;

	TextObjDesc.strModelPrototypeTag_2D = TEXT("Prototype_Logo_TextObject_Button");
	TextObjDesc.vColor = _float4(1.f, 1.f, 1.f, 1.f);
	TextObjDesc.isBlinking = true;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_Logo_ColorObject"),
		m_iCurLevelID, TEXT("Layer_UI"), &pObject, &TextObjDesc)))
		return E_FAIL;

	m_pBackGroundParts[LOGO_TEXT_OBJECT_BUTTON] = static_cast<CModelObject*>(pObject);
	pObject->Set_Active(false);
	Safe_AddRef(pObject);

	// Logo FadeUI 积己
	TextObjDesc.iCurLevelID = m_iCurLevelID;
	TextObjDesc.tTransform2DDesc.vInitialPosition = _float3(0.f, 0.f, 0.f);
	TextObjDesc.tTransform2DDesc.vInitialScaling = _float3(g_iWinSizeX * 3.f, g_iWinSizeY * 3.f, 1.f);
	TextObjDesc.iColorObjectType = CLogo_ColorObject::DEFAULT;

	TextObjDesc.strModelPrototypeTag_2D = TEXT("Prototype_Logo_FadeUI");
	TextObjDesc.vColor = _float4(0.f, 0.f, 0.f, 1.f);
	TextObjDesc.isBlinking = false;

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_Logo_ColorObject"),
		m_iCurLevelID, TEXT("Layer_UI"), &pObject, &TextObjDesc)))
		return E_FAIL;

	m_pBackGroundParts[LOGO_FADEUI] = static_cast<CModelObject*>(pObject);
	pObject->Set_Active(false);
	Safe_AddRef(pObject);

	return S_OK;
}

void CLogo_BackGround::On_End_Animation(COORDINATE _eCoordinate, _uint _iAnimIndex)
{
	if (COORDINATE_3D == _eCoordinate)
		return;

	if (CLogo_BackGround::INTO == _iAnimIndex) {

		if (nullptr == m_pBackGroundParts[LOGO_TEXT_OBJECT])
			return;

		if (MAIN_HUMGRUMP != m_eBackGroundType)
		{
			if (false == m_pBackGroundParts[LOGO_TEXT_OBJECT]->CBase::Is_Active())
			{
				m_pBackGroundParts[LOGO_TEXT_OBJECT]->Set_Active(true);
				m_isRenderTextObject = true;
			}

			return;
		}
			

		m_pBackGroundParts[LOGO_CHARACTER]->Switch_Animation(BG_CHARACTER_ANIM::LOOP);
		m_pBackGroundParts[LOGO_TEXT_OBJECT]->Set_Active(true);
		m_isRenderTextObject = true;
	}
}

void CLogo_BackGround::Caculate_FadeValue(_float _fTimeDelta)
{
	if (false == m_isRenderFont)
		return;

	if (false == m_isEndFadeFont) {
		_float fRatio = m_pGameInstance->Calculate_Ratio(&m_fFontFadeTime, _fTimeDelta, LERP);

		if (fRatio >= (1.f - EPSILON)) {
			fRatio = 1.f;
			m_isEndFadeFont = true;
			m_fFontFadeTime.y = m_fFontFadeTime.x;
		}
	}
}

void CLogo_BackGround::BackGround_FadeOut(_float fTimeDelta)
{
	if (false == m_isBGFadeOut)
		return;

	if (CModelObject::FADEALPHA_DEFAULT == m_pBackGroundParts[LOGO_FADEUI]->Get_FadeAlphaState()) {
		m_isEndBGFadeOut = true;
	}
}

void CLogo_BackGround::Render_Font()
{
	if (false == m_isRenderFont)
		return;

	_float fRatio = m_fFontFadeTime.y / m_fFontFadeTime.x;
	
	if (MAIN_JOT == m_eBackGroundType)
	{
		m_pGameInstance->Render_Font(TEXT("Font40"), TEXT("陵脚 霸烙 酒墨单固 147扁 力累"), _float2(g_iWinSizeX - (g_iWinSizeX * 0.95f), g_iWinSizeY - (g_iWinSizeY * 0.12f)), XMVectorSet(0.0f, 0.0f, 0.0f, fRatio));
	}
	else
	m_pGameInstance->Render_Font(TEXT("Font40"), TEXT("One Possible Future 力累"), _float2(g_iWinSizeX - (g_iWinSizeX * 0.95f), g_iWinSizeY - (g_iWinSizeY * 0.12f)), XMVectorSet(0.0f, 0.0f, 0.0f, fRatio));
}

void CLogo_BackGround::Check_RenderTiming(_float _fTimeDelta)
{
	if (false == m_isRenderTextObject)
		return;

	if (CModelObject::FADEALPHA_DEFAULT == m_pBackGroundParts[LOGO_TEXT_OBJECT]->Get_FadeAlphaState()
		&& false == m_isRenderFont) {
		m_isRenderFont = true;
	}

	if (true == m_isRenderFont && 1.f == (_float)(m_fFontFadeTime.y / m_fFontFadeTime.x)
		&& false == m_isRenderTextObject_Button) {
		m_pBackGroundParts[LOGO_TEXT_OBJECT_BUTTON]->Set_Active(true);
		m_isRenderTextObject_Button = true;
	}
}

CLogo_BackGround* CLogo_BackGround::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CLogo_BackGround* pInstance = new CLogo_BackGround(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Created CLogo_BackGround Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject* CLogo_BackGround::Clone(void* _pArg)
{
	CLogo_BackGround* pInstance = new CLogo_BackGround(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Clone CLogo_BackGround Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CLogo_BackGround::Free()
{
	for (auto& Object : m_pBackGroundParts) {
		Safe_Release(Object);
	}

	__super::Free();
}

HRESULT CLogo_BackGround::Cleanup_DeadReferences()
{
	return S_OK;
}
