#include "stdafx.h"
#include "Logo_ColorObject.h"

#include "Shader.h"

CLogo_ColorObject::CLogo_ColorObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CModelObject(_pDevice, _pContext)
{
}

CLogo_ColorObject::CLogo_ColorObject(const CLogo_ColorObject& _Prototype)
	: CModelObject(_Prototype)
{
}

HRESULT CLogo_ColorObject::Initialize(void* _pArg)
{
	CLogo_ColorObject::COLOROBJECT_DESC* pDesc = static_cast<CLogo_ColorObject::COLOROBJECT_DESC*>(_pArg);

	pDesc->eStartCoord = COORDINATE_2D;
	pDesc->isCoordChangeEnable = false;
	pDesc->strShaderPrototypeTag_2D = TEXT("Prototype_Component_Shader_VtxPosTex");
	pDesc->iShaderPass_2D = (_uint)PASS_VTXPOSTEX::COLOR_ALPHA;
	pDesc->iModelPrototypeLevelID_2D = LEVEL_STATIC;

	m_bPlayingAnim = false;
	m_vPosTexColor = pDesc->vColor;
	m_eColorObjectType = pDesc->iColorObjectType;
	m_isBlinking = pDesc->isBlinking;

	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	

	return S_OK;
}

void CLogo_ColorObject::Update(_float _fTimeDelta)
{
	Blinking();

	__super::Update(_fTimeDelta);
}

void CLogo_ColorObject::Late_Update(_float _fTimeDelta)
{
	__super::Late_Update(_fTimeDelta);
}

HRESULT CLogo_ColorObject::Render()
{
	__super::Render();

	_float fAlphaRatio = 1.f;

	if (FAILED(m_pShaderComs[COORDINATE_2D]->Bind_RawValue("g_fSprite2DFadeAlphaRatio", &fAlphaRatio, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

void CLogo_ColorObject::Active_OnEnable()
{
	if(DEFAULT != m_eColorObjectType)
		Start_FadeAlphaIn(3.f);

	__super::Active_OnEnable();
}

void CLogo_ColorObject::Active_OnDisable()
{
	__super::Active_OnDisable();
}

void CLogo_ColorObject::Blinking()
{
	if (false == m_isBlinking)
		return;

	if (SPRITE2D_FADEALPHA_STATE::FADEALPHA_OUT == m_eFadeAlphaState &&
		m_vFadeAlpha.x == m_vFadeAlpha.y) {
		m_iPreFadeAlphaState = SPRITE2D_FADEALPHA_STATE::FADEALPHA_DEFAULT;
	}

	if (m_iPreFadeAlphaState != m_eFadeAlphaState ) {
		
		m_iPreFadeAlphaState = m_eFadeAlphaState;

		switch (m_eFadeAlphaState) {
		case SPRITE2D_FADEALPHA_STATE::FADEALPHA_DEFAULT:
			Start_FadeAlphaOut(2.5f);
			m_iPreFadeAlphaState = m_eFadeAlphaState;
			break;
		case SPRITE2D_FADEALPHA_STATE::FADEALPHA_OUT:
			Start_FadeAlphaIn(2.5f);
			break;
		}
	}
}

CLogo_ColorObject* CLogo_ColorObject::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CLogo_ColorObject* pInstance = new CLogo_ColorObject(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CLogo_ColorObject");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CLogo_ColorObject::Clone(void* _pArg)
{
	CLogo_ColorObject* pInstance = new CLogo_ColorObject(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : CLogo_ColorObject");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLogo_ColorObject::Free()
{
	__super::Free();
}
