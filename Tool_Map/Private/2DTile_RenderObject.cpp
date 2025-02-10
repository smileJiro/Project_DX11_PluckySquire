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
	TILE_RENDEROBJECT_DESC* pDesc = static_cast<TILE_RENDEROBJECT_DESC*>(pArg);

	// UI Transform size 초기화. 1로 채워주지 않으면 이후 Scailing이 모두 0.f로 들어간다.
	pDesc->fSizeX = 1.f;
	pDesc->fSizeY = 1.f;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	// 지정해둔 텍스쳐 사이즈대로 수동 직교투영 행렬 생성.

	if (FAILED(Ready_Components()))
		return E_FAIL;
	

	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)pDesc->iMapSizeWidth, (_float)pDesc->iMapSizeHeight, 0.0f, 1.0f));


	// 타일 기본정보 업데이트.
	m_iWidthSIze = pDesc->iIndexSIzeX;
	m_iHeightSIze = pDesc->iIndexSIzeY;
	m_iWidthCount = pDesc->iIndexCountX;
	m_iHeightCount = pDesc->iIndexCountY;

	m_iIndexCount = m_iWidthCount * m_iHeightCount;

	m_fSizeX = (_float)m_iWidthSIze;
	m_fSizeY = (_float)m_iHeightSIze;

	// 타일 스케일 대로 트랜스폼 업데이트 진행.
	m_pControllerTransform->Set_Scale(m_fSizeX, m_fSizeY, 1.f);



	// 타일 인덱스 갯수대로 리사이즈 후, -1로 채우기
	m_TextureIndexs.resize(m_iIndexCount);
	
	
	
	return S_OK;
}

void C2DTile_RenderObject::Priority_Update(_float fTimeDelta)
{
}

void C2DTile_RenderObject::Update(_float fTimeDelta)
{
	Register_RenderGroup(RG_2D, PR2D_BOOK_SECTION);
}

void C2DTile_RenderObject::Late_Update(_float fTimeDelta)
{
}

HRESULT C2DTile_RenderObject::Render()
{
	for (_uint i = 0; i < m_iIndexCount; i++)
	{
		// 해당 타일 Index에 지정된 Texture Index 대로, 타일 UV 설정과 타일 위치를 배치 후, 렌더
		
		_float2 fPos = {};
		_float2 fStartUV = {};
		_float2 fEndUV = {};
		// 유효성 검사 failed시, 렌더를 진행하지 않는다.
		if (FAILED(Compute_UV(i, fStartUV, fEndUV)))
			continue;
		
		if (FAILED(Compute_Tile_Pos(i, fPos)))
			continue;

		// UV 좌표 바인딩.
		if (FAILED(m_pShader->Bind_RawValue("g_vSpriteStartUV", &fStartUV, sizeof(_float2))))
			return E_FAIL;
		if (FAILED(m_pShader->Bind_RawValue("g_vSpriteEndUV", &fEndUV, sizeof(_float2))))
			return E_FAIL;
		m_fX = fPos.x;
		m_fY = fPos.y;
		// 타일 위치 업데이트.
		m_pControllerTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_fX - RTSIZE_BOOK2D_X * 0.5f, -m_fY + RTSIZE_BOOK2D_Y * 0.5f, 0.f, 1.f));
		
		if (FAILED(Bind_ShaderResources()))
			return E_FAIL;

		// 4 == Client Pos Shader의 UV Mapping Pass
		m_pShader->Begin(4);

		m_pVIBufferCom->Bind_BufferDesc();

		m_pVIBufferCom->Render();
	}


	if (m_strOutputPath != L"")
	{
		ID3D11ShaderResourceView* pSRV = m_pGameInstance->Get_RT_SRV(L"Target_Book_2D");
		ID3D11Texture2D* pTexture2D =  nullptr ;
		ID3D11Resource* pResource = nullptr;

		pSRV->GetResource(&pResource);
		if (pResource)
		{
			HRESULT hr = pResource->QueryInterface(__uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pTexture2D));
			pResource->Release();

			if (SUCCEEDED(hr) && pTexture2D)
			{
				if (FAILED(SaveDDSTextureToFile(m_pContext, pTexture2D, m_strOutputPath.c_str())))
					return E_FAIL;
			}
		}
		m_strOutputPath = L"";
	}
	


	return S_OK;
}


HRESULT C2DTile_RenderObject::Set_Texture(TILE_TEXTURE_INFO& _tTextureInfo, ID3D11ShaderResourceView* _pTexture)
{
	if (nullptr == _pTexture )
		return E_FAIL;
	if (nullptr == m_pTextureCom)
		m_pTextureCom = CTexture::Create(m_pDevice, m_pContext);

	// 추가전 Texture Info 정보 갯수와 SRVs 갯수가 일치하지 않으면 매우 곤란해지니 리턴
	if ((_uint)m_TIleTextureInfos.size() != m_pTextureCom->Get_NumSRVs())
		return E_FAIL;

	m_pTextureCom->Add_Texture(_pTexture, _tTextureInfo.strTextureTag);
	m_TIleTextureInfos.push_back(_tTextureInfo);
	
	return S_OK;
}

HRESULT C2DTile_RenderObject::Set_Index(_uint _iTileIndex, _int _iTextureInfoIndex, _int _iTextureUVIndex)
{
	//if (_iTileIndex >= (_uint)m_TextureIndexs.size())
	//	return E_FAIL;

	//if (_iTextureInfoIndex >= (_uint)m_TIleTextureInfos.size())
	//	return E_FAIL;

	m_TextureIndexs[_iTileIndex] = { _iTextureInfoIndex, _iTextureUVIndex };

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
	//if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
	//	return E_FAIL;
	//if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
	//	return E_FAIL;

	return S_OK;
}

HRESULT C2DTile_RenderObject::Compute_UV(_uint _iIndex, _float2& _fStartUV, _float2& _fEndUV)
{
	//유효성 검사.
	if (_iIndex >= (_uint)m_TextureIndexs.size() || // 타일 인덱스
		-1 == m_TextureIndexs[_iIndex].x || // 텍스쳐 정보
		-1 == m_TextureIndexs[_iIndex].y ) // 텍스쳐 UV 인덱스
		return E_FAIL;



	_int TextureInfoIndex = m_TextureIndexs[_iIndex].x;
	_int TextureUVIndex = m_TextureIndexs[_iIndex].y;

	TILE_TEXTURE_INFO& tTextureInfo = m_TIleTextureInfos[TextureInfoIndex];

	
	_uint iY = TextureUVIndex / tTextureInfo.iTileIndexCountX;
	_uint iX = TextureUVIndex - (iY * tTextureInfo.iTileIndexCountX);
	_float fUvWidth = (_float)tTextureInfo.iTileIndexSizeX / (_float)tTextureInfo.iTileTextureSizeX;
	_float fUvHeight = (_float)tTextureInfo.iTileIndexSizeY / (_float)tTextureInfo.iTileTextureSizeY;


	_fStartUV.x = iX * fUvWidth;
	_fStartUV.y = iY * fUvHeight;

	_fEndUV.x = _fStartUV.x + fUvWidth;
	_fEndUV.y = _fStartUV.y + fUvHeight;

	return S_OK;
}

HRESULT C2DTile_RenderObject::Compute_Tile_Pos(_uint _iIndex, _float2& _fPos)
{
	//유효성 검사.
	if (_iIndex >= (_uint)m_TextureIndexs.size() || // 타일 인덱스
		-1 == m_TextureIndexs[_iIndex].x || // 텍스쳐 정보
		-1 == m_TextureIndexs[_iIndex].y) // 텍스쳐 UV 인덱스
		return E_FAIL;

	_uint iX = 0;
	_uint iY = 0;
	iY = _iIndex / m_iWidthCount;
	iX = _iIndex - (iY * m_iWidthCount);

	_fPos.x = (_float)(m_iWidthSIze * iX + (_uint)(m_iWidthSIze / 2));
	_fPos.y = (_float)(m_iHeightSIze * iY + (_uint)(m_iHeightSIze / 2));

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
	Safe_Release(m_pTextureCom);
}

HRESULT C2DTile_RenderObject::Cleanup_DeadReferences()
{
	return S_OK;
}
