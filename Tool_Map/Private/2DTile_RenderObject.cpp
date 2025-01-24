#include "stdafx.h"
#include "2DTile_RenderObject.h"
#include "Engine_Defines.h"
#include "GameInstance.h"


C2DTile_RenderObject::C2DTile_RenderObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject(pDevice, pContext)
{
}

C2DTile_RenderObject::C2DTile_RenderObject(const C2DTile_RenderObject& Prototype)
	: CUIObject(Prototype)
{
}

HRESULT C2DTile_RenderObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT C2DTile_RenderObject::Initialize(void* pArg)
{
	UIOBJECT_DESC Desc = {};


	Desc.fX = 0.f;
	Desc.fY = 0.f;
	Desc.fSizeX = m_iWidthSIze;
	Desc.fSizeY = m_iHeightSIze;

	if (FAILED(__super::Initialize(&Desc)))
		return E_FAIL;
	        XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)RTSIZE_BOOK2D_X, (_float)RTSIZE_BOOK2D_Y, 0.0f, 1.0f));

	if (FAILED(Ready_Components()))
		return E_FAIL;
	return S_OK;
}

void C2DTile_RenderObject::Priority_Update(_float fTimeDelta)
{
	int a = 10;
}

void C2DTile_RenderObject::Update(_float fTimeDelta)
{

}

void C2DTile_RenderObject::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RG_BOOK_2D, this);
}

HRESULT C2DTile_RenderObject::Render()
{

	for (_uint i = 0; i < m_iIndexCount; i++)
	{
		_uint iPosX = 0;
		_uint iPosY = 0;
		_uint iX = 0;
		_uint iY = 0;
		
		if (m_TextureIndexs[i] != -1)
		{
			iY = i / m_iWidthCount;
			iX = i - (iY * m_iWidthCount);
			
			iPosX = m_iWidthSIze * iX + (m_iWidthSIze / 2);
			iPosY = m_iHeightSIze * iY + (m_iHeightSIze / 2);
			m_fX = iPosX;
			m_fY = iPosY;

			_float fUvWidth = (_float)m_iWidthSIze / (_float)(m_iWidthSIze * m_iWidthCount);
			_float fUvHeight = (_float)m_iHeightSIze / (_float)(m_iHeightSIze * m_iHeightCount);

			_float2 fStartUV = {};
			_float2 fEndUV = {};
			fStartUV.x = iX * fUvWidth;
			fStartUV.y = iY * fUvHeight;

			fEndUV.x = fStartUV.x + fUvWidth;
			fEndUV.y = fStartUV.y + fUvHeight;


			if (FAILED(m_pShader->Bind_RawValue("g_vSpriteStartUV", &fStartUV, sizeof(_float2))))
				return E_FAIL;
			if (FAILED(m_pShader->Bind_RawValue("g_vSpriteEndUV", &fEndUV, sizeof(_float2))))
				return E_FAIL;
			m_pControllerTransform->Set_Scale(m_fSizeX, m_fSizeY, 1.f);
			//m_pControllerTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_fX - RTSIZE_BOOK2D_X * 0.5f, -m_fY + RTSIZE_BOOK2D_Y * 0.5f, 0.f, 1.f));
			m_pControllerTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f,0.f,0.f,1.f));
			
			if (FAILED(Bind_ShaderResources()))
				return E_FAIL;

			m_pShader->Begin(0);

			m_pVIBufferCom->Bind_BufferDesc();

			m_pVIBufferCom->Render();
		}
	}

	return S_OK;
}

HRESULT C2DTile_RenderObject::Set_TileTexture(const TILE_INFO& _tTile_Info)
{
	m_iWidthSIze = _tTile_Info.iWidthSIze;
	m_iHeightSIze = _tTile_Info.iHeightSIze;
	m_iWidthCount = _tTile_Info.iWidthCount;
	m_iHeightCount = _tTile_Info.iHeightCount;
	m_iIndexCount = m_iWidthCount * m_iHeightCount;
	m_fSizeX = m_iWidthSIze;
	m_fSizeY = m_iHeightSIze;
	if (nullptr != m_pTextureCom)
		Safe_Release(m_pTextureCom);
	m_pTextureCom = CTexture::Create(m_pDevice, m_pContext);
	m_pTextureCom->Add_Texture(_tTile_Info.pTexture,_tTile_Info.strTextureName);

	m_TextureIndexs.resize(m_iIndexCount);
	std::iota(m_TextureIndexs.begin(), m_TextureIndexs.end(), -1);

	return S_OK;
}

HRESULT C2DTile_RenderObject::Set_Index(_uint _iTileIndex, _int _iTextureIndex)
{
	if (_iTileIndex >= (_uint)m_TextureIndexs.size())
		return E_FAIL;
	m_TextureIndexs[_iTileIndex] = _iTextureIndex;
	return S_OK;
}

HRESULT C2DTile_RenderObject::Ready_Components()
{
	/* Com_Shader */
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShader))))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;


	return S_OK;


}

HRESULT C2DTile_RenderObject::Bind_ShaderResources()
{
	if (FAILED(m_pControllerTransform->Bind_ShaderResource(m_pShader, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShader, "g_DiffuseTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	return S_OK;
}

C2DTile_RenderObject* C2DTile_RenderObject::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	C2DTile_RenderObject* pInstance = new C2DTile_RenderObject(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : C2DTile_RenderObject");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* C2DTile_RenderObject::Clone(void* pArg)
{
	C2DTile_RenderObject* pInstance = new C2DTile_RenderObject(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : C2DTile_RenderObject");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void C2DTile_RenderObject::Free()
{
	__super::Free();

	Safe_Release(m_pShader);
	Safe_Release(m_pVIBufferCom);
}

HRESULT C2DTile_RenderObject::Cleanup_DeadReferences()
{
	return S_OK;
}
