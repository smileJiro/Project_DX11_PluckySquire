#include "stdafx.h"
#include "2DMapObjectInfo.h"
#include "Engine_Defines.h"
#include "GameInstance.h"
#include "Animation2D.h"
#include "Collider.h"

C2DMapObjectInfo::C2DMapObjectInfo(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:
	m_pDevice(_pDevice),
	m_pContext(_pContext),
	m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

C2DMapObjectInfo::C2DMapObjectInfo(const C2DMapObjectInfo& Prototype)
	: CBase(Prototype)
{
}


HRESULT C2DMapObjectInfo::Initialize(json _InfoJson)
{
	m_strSearchTag = _InfoJson["SearchTag"];
	m_strModelName = _InfoJson["TextureName"];
	m_isActive = _InfoJson["Active"];
	m_isCollider = _InfoJson["Collider"];
	m_isSorting = _InfoJson["Sorting"];
	m_isBackGround = _InfoJson["BackGround"];

	m_eModelType = _InfoJson["ModelType"];;

	if (m_isActive)
	{
		if (
			_InfoJson.contains("ActivePropertis")
			&&
			_InfoJson["ActivePropertis"].contains("ActiveType")
			)
		{
			m_eActiveType = _InfoJson["ActivePropertis"]["ActiveType"];
		}
	}
	if (m_isCollider)
	{
		if (
			_InfoJson.contains("ColliderPropertis")
			)
		{
			auto ColliderPropertis = _InfoJson["ColliderPropertis"];
			if (ColliderPropertis.contains("ColliderType"))
			{
				m_eColliderType = ColliderPropertis["ColliderType"];
			}
			if (ColliderPropertis.contains("ColliderInfo"))
			{
				auto ColliderInfo = ColliderPropertis["ColliderInfo"];

				if (ColliderInfo.contains("Collider_Offset_Pos"))
					m_fColliderOffsetPos = { ColliderInfo["Collider_Offset_Pos"]["X"],ColliderInfo["Collider_Offset_Pos"]["Y"] };

				switch (m_eColliderType)
				{
				case CCollider::AABB_2D:
					if (ColliderInfo.contains("Collider_Extent"))
						m_fColliderExtent = { ColliderInfo["Collider_Extent"]["X"],ColliderInfo["Collider_Extent"]["Y"] };
					break;
				case CCollider::CIRCLE_2D:
					if (ColliderInfo.contains("Collider_Radius"))
						m_fColliderRadius = ColliderInfo["Collider_Radius"];
					break;
				}
			}
		}
	}
	if (m_isCollider)
	{
		if (
			_InfoJson.contains("SortingPropertis")
			)
		{
			auto SortingPropertis = _InfoJson["SortingPropertis"];
			if (SortingPropertis.contains("Sorting_Offset_Pos"))
				m_fSortingPosition = { SortingPropertis["Sorting_Offset_Pos"]["X"],SortingPropertis["Sorting_Offset_Pos"]["Y"] };

		}
	}

#ifdef _DEBUG
	if (m_strSearchTag != "")
	{
		CBase* pModel = m_pGameInstance->Find_Prototype(LEVEL_TOOL_2D_MAP, StringToWstring(m_strModelName));
		if (pModel != nullptr)
		{
			m_isModelCreate = true;
			m_pModel = static_cast<C2DModel*>(pModel);

			if (SUCCEEDED(Create_PreviewModel_Texutre()) && nullptr != m_pPreviewSRV)
			{
				m_isToolRendering = true;
			}
		}
	}
#endif // _DEBUG


	return S_OK;
}

HRESULT C2DMapObjectInfo::Export(json& _OutputJson)
{
	_OutputJson["SearchTag"] = m_strSearchTag;
	_OutputJson["TextureName"] = m_strModelName;
	_OutputJson["ModelType"] = m_eModelType;
	_OutputJson["Active"] = m_isActive;
	_OutputJson["Collider"] = m_isCollider;
	_OutputJson["Sorting"] = m_isSorting;
	_OutputJson["BackGround"] = m_isBackGround;
	if (m_isActive)
	{
		_OutputJson["ActivePropertis"]["ActiveType"] = m_eActiveType;
	}
	if (m_isCollider)
	{
		_OutputJson["ColliderPropertis"]["ColliderType"] = m_eColliderType;
		if (m_eColliderType != CCollider::TYPE_LAST)
		{

			_OutputJson["ColliderPropertis"]["ColliderInfo"]["Collider_Offset_Pos"]["X"] = m_fColliderOffsetPos.x;
			_OutputJson["ColliderPropertis"]["ColliderInfo"]["Collider_Offset_Pos"]["Y"] = m_fColliderOffsetPos.y;
			switch (m_eColliderType)
			{
			case CCollider::AABB_2D:
				_OutputJson["ColliderPropertis"]["ColliderInfo"]["Collider_Extent"]["X"] = m_fColliderExtent.y;
				_OutputJson["ColliderPropertis"]["ColliderInfo"]["Collider_Extent"]["Y"] = m_fColliderExtent.y;
				break;
			case CCollider::CIRCLE_2D:
				_OutputJson["ColliderPropertis"]["ColliderInfo"]["Collider_Radius"] = m_fColliderRadius;
				break;
			}
		}
	}
	if (m_isSorting)
	{
		_OutputJson["SortingPropertis"]["Sorting_Offset_Pos"]["X"] = m_fSortingPosition.x;
		_OutputJson["SortingPropertis"]["Sorting_Offset_Pos"]["Y"] = m_fSortingPosition.y;
	}
	return S_OK;
}


ID3D11ShaderResourceView* C2DMapObjectInfo::Get_SRV(_float2* _pReturnSize)
{
	if (!m_isToolRendering || nullptr == m_pPreviewSRV)
		return nullptr;

	if (nullptr != _pReturnSize)
		*_pReturnSize = m_fPreviewSize;
	
	return m_pPreviewSRV;
}

void C2DMapObjectInfo::Set_Model(C2DModel* _pModel)
{
	if (_pModel != nullptr)
	{
		m_isModelCreate = true;
		m_pModel = static_cast<C2DModel*>(_pModel);
		m_eModelType = _pModel->Get_AnimType();

		if (SUCCEEDED(Create_PreviewModel_Texutre()) && nullptr !=  m_pPreviewSRV)
		{
			m_isToolRendering = true;
		}
	}
}

HRESULT C2DMapObjectInfo::Create_PreviewModel_Texutre()
{

	if (nullptr == m_pModel)
		return E_FAIL;


	const CSpriteFrame* pTargetSprite = nullptr;

	pTargetSprite = m_pModel->Get_SpriteFrame();



	if (nullptr == pTargetSprite)
		return E_FAIL;

	_float2 fStartUV = pTargetSprite->Get_StartUV();
	_float2 fEndUV = pTargetSprite->Get_EndUV();
	CTexture* pTexture = pTargetSprite->Get_Texture();

	if (nullptr == pTexture || 0 >= pTexture->Get_NumSRVs())
		return E_FAIL;

	ID3D11ShaderResourceView* pTargetSRV = pTexture->Get_SRV(0);


	ID3D11Texture2D* pSrcTexture2D = nullptr;
	ID3D11Resource* pResource = nullptr;

	pTargetSRV->GetResource(&pResource);
	if (nullptr == pResource)
		return E_FAIL;

	HRESULT hr = pResource->QueryInterface(__uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pSrcTexture2D));

	Safe_Release(pResource);

	if (FAILED(hr) || nullptr == pSrcTexture2D)
		return E_FAIL;

	D3D11_TEXTURE2D_DESC srcDesc;
	pSrcTexture2D->GetDesc(&srcDesc);

	_uint iX = static_cast<_uint>(srcDesc.Width * fStartUV.x);
	_uint iY = static_cast<_uint>(srcDesc.Height * fStartUV.y);
	_uint iWidth = static_cast<_uint>((srcDesc.Width * fEndUV.x) - iX);
	_uint iHeight = static_cast<_uint>((srcDesc.Height * fEndUV.y) - iY);

	m_fPreviewSize = { (_float)iWidth,(_float)iHeight };

	// 货肺款 咆胶贸 积己
	D3D11_TEXTURE2D_DESC dstDesc = srcDesc;
	dstDesc.Width = iWidth;
	dstDesc.Height = iHeight;
	dstDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	dstDesc.MiscFlags = 0;

	ID3D11Texture2D* pDstTexture = nullptr;
	m_pDevice->CreateTexture2D(&dstDesc, nullptr, &pDstTexture);

	// 漂沥 康开父 汗荤
	D3D11_BOX srcBox = { iX, iY, 0, iX + iWidth, iY + iHeight, 1 };
	m_pContext->CopySubresourceRegion(pDstTexture, 0, 0, 0, 0, pSrcTexture2D, 0, &srcBox);

	// 货肺款 SRV 积己
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = dstDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;

	m_pDevice->CreateShaderResourceView(pDstTexture, &srvDesc, &m_pPreviewSRV);

	Safe_Release(pSrcTexture2D);
	Safe_Release(pDstTexture);

	return S_OK;
}


C2DMapObjectInfo* C2DMapObjectInfo::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, json _InfoJson)
{
	C2DMapObjectInfo* pInstance = new C2DMapObjectInfo(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize(_InfoJson)))
	{
		MSG_BOX("Failed to Created : C2DMapObjectInfo");
		Safe_Release(pInstance);
	}

	return pInstance;
}

C2DMapObjectInfo* C2DMapObjectInfo::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	C2DMapObjectInfo* pInstance = new C2DMapObjectInfo(_pDevice, _pContext);

	return pInstance;
}

void C2DMapObjectInfo::Free()
{
	Safe_Release(m_pPreviewSRV);
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	__super::Free();
}
