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
	pDesc->iShaderPass_2D = (_uint)PASS_VTXPOSTEX::DEFAULT;
	pDesc->tTransform2DDesc.vInitialScaling = _float3(g_iWinSizeX * 1.0f, g_iWinSizeY * 0.333f, 1.f);
	pDesc->iModelPrototypeLevelID_2D = LEVEL_STATIC;

	m_bPlayingAnim = false;
	m_vColor = pDesc->vColor;

	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	Set_Active(false);

	return S_OK;
}

void CLogo_ColorObject::Update(_float _fTimeDelta)
{
	__super::Update(_fTimeDelta);
}

void CLogo_ColorObject::Late_Update(_float _fTimeDelta)
{
	__super::Late_Update(_fTimeDelta);
}

HRESULT CLogo_ColorObject::Render()
{
	if (FAILED(m_pShaderComs[COORDINATE_2D]->Bind_RawValue("g_vColors", &m_vColor, sizeof(_float4))))
		return E_FAIL;

	__super::Render();

	_float4 vColor = _float4(1.0f, 1.0f, 1.0f, 1.0f);
	if (FAILED(m_pShaderComs[COORDINATE_2D]->Bind_RawValue("g_vColors", &vColor, sizeof(_float4))))
		return E_FAIL;
	return S_OK;
}

void CLogo_ColorObject::Active_OnEnable()
{
	Start_FadeAlphaIn(5.f);

	__super::Active_OnEnable();
}

void CLogo_ColorObject::Active_OnDisable()
{
	__super::Active_OnDisable();
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
