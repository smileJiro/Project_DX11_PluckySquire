#include "stdafx.h"
#include "Simple_UI.h"
#include "GameInstance.h"

CSimple_UI::CSimple_UI(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CGameObject(_pDevice, _pContext)
{
}

CSimple_UI::CSimple_UI(const CSimple_UI& _Prototype)
	:CGameObject(_Prototype)
	,m_pTextureCom(_Prototype.m_pTextureCom)
	,m_pShaderCom(_Prototype.m_pShaderCom)
	,m_pVIBufferCom(_Prototype.m_pVIBufferCom)
{
	Safe_AddRef(m_pTextureCom);
	Safe_AddRef(m_pShaderCom);
	Safe_AddRef(m_pVIBufferCom);
}

HRESULT CSimple_UI::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSimple_UI::Initialize(void* _pArg)
{
	CSimple_UI::SIMPLE_UI_DESC* pDesc = static_cast<CSimple_UI::SIMPLE_UI_DESC*>(_pArg);

	// Save
	m_fX = pDesc->vUIInfo.x;
	m_fY = pDesc->vUIInfo.y;
	m_fSizeX = pDesc->vUIInfo.z;
	m_fSizeY = pDesc->vUIInfo.w;

	m_ePassIndex = pDesc->ePassIndex;
	m_iTextureIndex = pDesc->iTextureIndex;

	// Add
	pDesc->eStartCoord = COORDINATE_2D;
	pDesc->isCoordChangeEnable = false;

	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;
	/* 직교튀영을 위한 뷰ㅡ, 투영행르을 만들었다. */
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.f, 1.f));

	/* 던져준 fX, fY,  fSizeX, fSizeY로 그려질 수 있도록 월드행렬의 상태를 제어해둔다. */
	Apply_UIScaling();
	Apply_UIPosition();

	return S_OK;
}

void CSimple_UI::Priority_Update(_float _fTimeDelta)
{

	__super::Priority_Update(_fTimeDelta);
}

void CSimple_UI::Update(_float _fTimeDelta)
{

	__super::Update(_fTimeDelta);
}

void CSimple_UI::Late_Update(_float _fTimeDelta)
{
	//if (KEY_DOWN(KEY::K)) // 좌표 맞추기 테스트 용
	//{
	//	_float fY = g_iWinSizeX * 0.05f;
	//	_float fSizeX = 45.f;
	//	_float fSizeY = 45.f;
	//	_float fInterval = 20.f;
	//	Set_UIPosition(_float2(g_iWinSizeX * 0.5f - fSizeX - fInterval, fY));
	//	Set_UISize(_float2(fSizeX, fSizeY));
	//}

	Apply_UIScaling();
	Apply_UIPosition();
	Register_RenderGroup((_uint)RENDERGROUP::RG_3D, PR3D_UI);
	__super::Late_Update(_fTimeDelta);
}

HRESULT CSimple_UI::Render()
{
	if (FAILED(Bind_ShaderResources_WVP()))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", m_iTextureIndex)))
		return E_FAIL;

	m_pShaderCom->Begin((_uint)m_ePassIndex);

	// Buffer정보 전달
	m_pVIBufferCom->Bind_BufferDesc();

	// 그리기 연산 수행
	m_pVIBufferCom->Render();

	return S_OK;
}

void CSimple_UI::Set_TextureIndex(_uint _iTextureIndex)
{
	if (_iTextureIndex >= m_pTextureCom->Get_NumSRVs())
		return;

	m_iTextureIndex = _iTextureIndex;
}


HRESULT CSimple_UI::Bind_ShaderResources_WVP()
{

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pControllerTransform->Get_WorldMatrix_Ptr())))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	return S_OK;
}

void CSimple_UI::Apply_UIPosition()
{
	m_pControllerTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f, 1.f));
}

void CSimple_UI::Apply_UIScaling()
{
	m_pControllerTransform->Set_Scale(m_fSizeX, m_fSizeY, 1.f);
}

HRESULT CSimple_UI::Ready_Components(SIMPLE_UI_DESC* _pDesc)
{
	/* Com_Shader */
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	if (false == _pDesc->strTexturePrototypeTag.empty())
	{
		/* Com_Texture */
		if (FAILED(Add_Component(m_iCurLevelID, _pDesc->strTexturePrototypeTag,
			TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		{
			/* Level_Static */
			_int iStaticLevelID = m_pGameInstance->Get_StaticLevelID();
			if (FAILED(Add_Component(iStaticLevelID, _pDesc->strTexturePrototypeTag,
				TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
				return E_FAIL;
		}
	}

	return S_OK;
}

CSimple_UI* CSimple_UI::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CSimple_UI* pInstance = new CSimple_UI(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed Create : CSimple_UI");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject* CSimple_UI::Clone(void* _pArg)
{
	CSimple_UI* pInstance = new CSimple_UI(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed Clone : CSimple_UI ");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CSimple_UI::Free()
{
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pVIBufferCom);

	__super::Free();
}

HRESULT CSimple_UI::Cleanup_DeadReferences()
{
	return S_OK;
}
