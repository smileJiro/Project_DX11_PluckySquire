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
	//Check_Character_Anim();

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

	__super::Render(0, PASS_VTXPOSTEX::COLOR_ALPHA);

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
	// Character 持失
	CGameObject* pObject = nullptr;

	CModelObject::MODELOBJECT_DESC Desc = {};

	Desc.eStartCoord = COORDINATE_2D;
	Desc.isCoordChangeEnable = false;
	Desc.strShaderPrototypeTag_2D = TEXT("Prototype_Component_Shader_VtxPosTex");
	Desc.iShaderPass_2D = (_uint)PASS_VTXPOSTEX::SPRITE2D;

	Desc.tTransform2DDesc.vInitialScaling = _float3(1.f, 1.f, 1.f);

	Desc.iModelPrototypeLevelID_2D = LEVEL_STATIC;

	Desc.tTransform2DDesc.vInitialPosition = _float3(0.f, 0.f, 0.f);
	Desc.iCurLevelID = m_iCurLevelID;

	switch (m_eBackGroundType) {
	case MAIN_JOT:
		Desc.strModelPrototypeTag_2D = TEXT("Main_Jot");
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
	
	// Logo Text 持失
	CLogo_ColorObject::COLOROBJECT_DESC TextObjDesc = {};
	TextObjDesc.iCurLevelID = m_iCurLevelID;
	TextObjDesc.tTransform2DDesc.vInitialPosition = _float3(0.f, 0.f, 0.f);

	switch (m_eBackGroundType) {
	case MAIN_JOT:
		TextObjDesc.strModelPrototypeTag_2D = TEXT("Main_Jot");
		break;
	case MAIN_HUMGRUMP:
		TextObjDesc.strModelPrototypeTag_2D = TEXT("Prototype_Logo_TextObject_Humgrump");
		TextObjDesc.vColor = _float4(1.f, 1.f, 0.f, 1.f);
		break;
	}

	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_Logo_ColorObject"),
		m_iCurLevelID, TEXT("Layer_UI"), &pObject, &TextObjDesc)))
		return E_FAIL;

	m_pBackGroundParts[LOGO_TEXT_OBJECT] = static_cast<CModelObject*>(pObject);
	return S_OK;
}

void CLogo_BackGround::On_End_Animation(COORDINATE _eCoordinate, _uint _iAnimIndex)
{
	if (COORDINATE_3D == _eCoordinate)
		return;

	if (CLogo_BackGround::INTO == _iAnimIndex) {

		if (nullptr == m_pBackGroundParts[LOGO_TEXT_OBJECT])
			return;

		m_pBackGroundParts[LOGO_CHARACTER]->Switch_Animation(BG_CHARACTER_ANIM::LOOP);
		m_pBackGroundParts[LOGO_TEXT_OBJECT]->Set_Active(true);
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
	__super::Free();
}

HRESULT CLogo_BackGround::Cleanup_DeadReferences()
{
	return S_OK;
}
