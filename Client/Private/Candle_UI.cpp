#include "stdafx.h"
#include "Candle_UI.h"
#include "GameInstance.h"


CCandle_UI::CCandle_UI(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CModelObject(_pDevice, _pContext)
{
}

CCandle_UI::CCandle_UI(const CCandle_UI& _Prototype)
	:CModelObject(_Prototype)
{
}

HRESULT CCandle_UI::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCandle_UI::Initialize(void* _pArg)
{
	CCandle_UI::CANDLE_UI_DESC* pDesc = static_cast<CCandle_UI::CANDLE_UI_DESC*>(_pArg);

	// Save
	m_fX = pDesc->fX;
	m_fY = pDesc->fY;
	m_fSizeX = pDesc->fSizeX;
	m_fSizeY = pDesc->fSizeY;

	// Add
	pDesc->Build_2D_Model(pDesc->iCurLevelID, TEXT("UI_Candle"), TEXT("Prototype_Component_Shader_VtxPosTex"), (_uint)PASS_VTXPOSTEX::SPRITE2D, false);
	pDesc->eStartCoord = COORDINATE_2D;

	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	/* 직교튀영을 위한 뷰ㅡ, 투영행르을 만들었다. */
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.f, 1.f));

	/* 던져준 fX, fY,  fSizeX, fSizeY로 그려질 수 있도록 월드행렬의 상태를 제어해둔다. */
	Apply_UIScaling();
	Apply_UIPosition();

	/* Set Anim End */
	Register_OnAnimEndCallBack(bind(&CCandle_UI::On_AnimEnd, this, placeholders::_1, placeholders::_2));

	m_pControllerModel->Switch_Animation((_uint)STATE_IDLE, false);
	return S_OK;
}

void CCandle_UI::Priority_Update(_float _fTimeDelta)
{
	m_fSizeX = 0.5f;
	m_fSizeY = 0.4f;
	Apply_UIScaling();
	Apply_UIPosition();
	__super::Priority_Update(_fTimeDelta);
}

void CCandle_UI::Update(_float _fTimeDelta)
{
	__super::Update(_fTimeDelta);
}

void CCandle_UI::Late_Update(_float _fTimeDelta)
{
	Register_RenderGroup(RENDERGROUP::RG_3D, PR3D_UI);

	__super::Late_Update(_fTimeDelta);
}

HRESULT CCandle_UI::Render()
{
	if (FAILED(Bind_ShaderResources_WVP()))
		return E_FAIL;
	COORDINATE eCoord = m_pControllerTransform->Get_CurCoord();
	_float fFadeAlphaRatio = m_vFadeAlpha.y / m_vFadeAlpha.x;
	switch (m_eFadeAlphaState)
	{
	case Engine::CModelObject::FADEALPHA_DEFAULT:
		fFadeAlphaRatio = 1.0f;
		break;
	case Engine::CModelObject::FADEALPHA_IN:
		fFadeAlphaRatio = m_vFadeAlpha.y / m_vFadeAlpha.x;
		break;
	case Engine::CModelObject::FADEALPHA_OUT:
		fFadeAlphaRatio = 1.0f - (m_vFadeAlpha.y / m_vFadeAlpha.x);
		break;
	default:
		break;
	}

	m_pShaderComs[eCoord]->Bind_RawValue("g_fSprite2DFadeAlphaRatio", &fFadeAlphaRatio, sizeof(_float));

	m_pControllerModel->Render(m_pShaderComs[eCoord], m_iShaderPasses[eCoord]);

	return S_OK;
}

void CCandle_UI::State_Change_Idle()
{
	m_pControllerModel->Switch_Animation((_uint)STATE_IDLE, false);
}

void CCandle_UI::State_Change_TurnOn()
{
	m_pControllerModel->Switch_Animation((_uint)STATE_TURNON, false);
}

void CCandle_UI::State_Change_FlameLoop()
{
	m_pControllerModel->Switch_Animation((_uint)STATE_FLAMELOOP, false);
}

void CCandle_UI::State_Change_TurnOff()
{
	m_pControllerModel->Switch_Animation((_uint)STATE_TURNOFF, false);
}

void CCandle_UI::OnOff_CandleUI(_bool _isOnOff)
{
	_isOnOff == true ? Start_FadeAlphaIn() : Start_FadeAlphaOut();
}

void CCandle_UI::On_AnimEnd(COORDINATE _eCoord, _uint _iAnimIdx)
{
	switch ((STATE)_iAnimIdx)
	{
	case Client::CCandle_UI::STATE_FLAMELOOP:
		break;
	case Client::CCandle_UI::STATE_TURNOFF:
		m_pControllerModel->Switch_Animation((_uint)STATE_IDLE);
		break;
	case Client::CCandle_UI::STATE_TURNON:
		m_pControllerModel->Switch_Animation((_uint)STATE_FLAMELOOP);
		break;
	case Client::CCandle_UI::STATE_IDLE:
		break;
	default:
		break;
	}
}

HRESULT CCandle_UI::Bind_ShaderResources_WVP()
{
	_matrix SpriteTransformMatrix = *static_cast<C2DModel*>(m_pControllerModel->Get_Model(COORDINATE_2D))->Get_CurrentSpriteTransform();

	_matrix WorldMatrix = m_pControllerTransform->Get_WorldMatrix(COORDINATE_2D);
	_float4x4 FinalWorldMatrix = {};
	XMStoreFloat4x4(&FinalWorldMatrix, SpriteTransformMatrix * WorldMatrix);
	
	if (FAILED(m_pShaderComs[COORDINATE_2D]->Bind_Matrix("g_WorldMatrix", &FinalWorldMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderComs[COORDINATE_2D]->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderComs[COORDINATE_2D]->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	return S_OK;
}

void CCandle_UI::Apply_UIPosition()
{
	m_pControllerTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f, 1.f));
}

void CCandle_UI::Apply_UIScaling()
{
	m_pControllerTransform->Set_Scale(m_fSizeX, m_fSizeY, 1.f);
}

CCandle_UI* CCandle_UI::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CCandle_UI* pInstance = new CCandle_UI(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCandle_UI");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCandle_UI* CCandle_UI::Clone(void* _pArg)
{
	CCandle_UI* pInstance = new CCandle_UI(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : CCandle_UI");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCandle_UI::Free()
{

	__super::Free();
}