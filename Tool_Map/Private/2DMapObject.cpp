#include "stdafx.h"
#include "2DMapObject.h"
#include "Engine_Defines.h"
#include "GameInstance.h"


C2DMapObject::C2DMapObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject(pDevice, pContext)
{
}

C2DMapObject::C2DMapObject(const C2DMapObject& Prototype)
	: CUIObject(Prototype)
{
}

HRESULT C2DMapObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT C2DMapObject::Initialize(void* pArg)
{
	if (nullptr == pArg)
		return E_FAIL;
	MAPOBJ_2D_DESC* pDesc = static_cast<MAPOBJ_2D_DESC*>(pArg);

	// UI Transform size 초기화. 1로 채워주지 않으면 이후 Scailing이 모두 0.f로 들어간다.



	pDesc->fSizeX = 1.f ;
	pDesc->fSizeY = 1.f ;
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	// 지정해둔 텍스쳐 사이즈대로 수동 직교투영 행렬 생성.

	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;

	_float2 fImageSize = m_pTextureCom->Get_Size();
	m_fY -= fImageSize.y * 0.5f;
	m_pControllerTransform->Set_Scale(fImageSize.x , fImageSize.y , 1.f );
	m_pControllerTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_fX, -m_fY, 0.f, 1.f));
	m_fRenderTargetSize = pDesc->fRenderTargetSize;
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)pDesc->fRenderTargetSize.x, (_float)pDesc->fRenderTargetSize.y, 0.0f, 1.0f));

	return S_OK;
}

void C2DMapObject::Priority_Update(_float fTimeDelta)
{
}

void C2DMapObject::Update(_float fTimeDelta)
{

}

void C2DMapObject::Late_Update(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(CRenderer::RG_BOOK_2D, this);
}

HRESULT C2DMapObject::Render()
{
	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	m_pShader->Begin(0);

	m_pVIBufferCom->Bind_BufferDesc();

	m_pVIBufferCom->Render();
	return S_OK;
}

void C2DMapObject::Set_OffsetPos(_float2 _fPos)
{
	m_pControllerTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_fX  + (_fPos.x), -m_fY + (-_fPos.y), 0.f, 1.f));
}

_bool C2DMapObject::IsCursor_In(_float2 _fCursorPos)
{
	_float2 fConvertRenderTargetSize = {};
	fConvertRenderTargetSize.x = g_iWinSizeX;
	fConvertRenderTargetSize.y = m_fRenderTargetSize.y * ((_float)g_iWinSizeX / (_float)m_fRenderTargetSize.x);

	_float fOffX = (g_iWinSizeX - fConvertRenderTargetSize.x) * 0.5f; 
	_float fOffY =	(g_iWinSizeY - fConvertRenderTargetSize.y) * 0.5f;

	_float fRelativeX = (_fCursorPos.x - fOffX) / fConvertRenderTargetSize.x;
	_float fRelativeY = (_fCursorPos.y - fOffY) / fConvertRenderTargetSize.y;

	_float2 fRealPos = { fRelativeX * m_fRenderTargetSize.x , fRelativeY * m_fRenderTargetSize.y };

	_vector fPosition = Get_Position();
	_float fPosX = XMVectorGetX(fPosition) + (m_fRenderTargetSize.x * 0.5f);
	_float fPosY = (XMVectorGetY(fPosition) * -1.f) + (m_fRenderTargetSize.y * 0.5f);

	_float3 fScale = Get_Scale();

	_float fLeft = fPosX - fScale.x / 2;
	_float fRight = fPosX + fScale.x / 2;
	_float fTop = fPosY - fScale.y / 2;
	_float fBottom = fPosY + fScale.y / 2;
	if (ContainWstring(m_strKey, L"rock"))
	{
		int a=1;
	}
	return (fRealPos.x >= fLeft && fRealPos.x <= fRight &&
		fRealPos.y >= fTop && fRealPos.y <= fBottom);
}

HRESULT C2DMapObject::Export(HANDLE hFile)
{

	_vector vPos = Get_Position();

	DWORD		dwByte(0);
	_uint		iModelIndex = nullptr != m_pModelInfo ? m_pModelInfo->Get_ModelIndex() : 0;
	_float2		fPos = {XMVectorGetX(vPos),XMVectorGetY(vPos) };
	_bool		isOverride = false;

	WriteFile(hFile, &iModelIndex, sizeof(_uint), &dwByte, nullptr);
	WriteFile(hFile, &fPos, sizeof(_float2), &dwByte, nullptr);
	WriteFile(hFile, &isOverride, sizeof(_bool), &dwByte, nullptr);

	return S_OK;
}

_vector C2DMapObject::Get_Position() const
{
	_vector vPos = __super::Get_Position();

	_float fPosX = XMVectorGetX(vPos) + (m_fRenderTargetSize.x * 0.5f);
	_float fPosY = (XMVectorGetY(vPos) * -1.f) + (m_fRenderTargetSize.y * 0.5f);
	XMVectorSetX(vPos, fPosX);
	XMVectorSetX(vPos, fPosY);
	return vPos;
}

HRESULT C2DMapObject::Update_Model_Index()
{
	if (nullptr == m_pModelInfo)
		return S_OK;


	if (m_pModelInfo->Is_Delete())
	{
		m_pModelInfo = nullptr;
		m_isModelLoad = false;
		/* Com_Texture */
		Safe_Release(m_pTextureCom);

		if (FAILED(Add_Component(m_iCurLevelID, L"Prototype_Component_Texture_None_Model",
			TEXT("Com_Texture_2D"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;
	}


	return S_OK;
}


HRESULT C2DMapObject::Ready_Components(MAPOBJ_2D_DESC* Desc)
{
	/* Com_Shader */
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShader))))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;
	m_strKey = Desc->strProtoTag;

	m_pModelInfo = Desc->pInfo;
	m_isModelLoad = nullptr != m_pModelInfo;


	if (m_isModelLoad)
	{
		m_pTextureCom = m_pModelInfo->Get_Texture();
		Safe_AddRef(m_pTextureCom);
	}
	if (m_pTextureCom == nullptr)
	{
		/* Com_Texture */
		if (FAILED(Add_Component(m_iCurLevelID, L"Prototype_Component_Texture_None_Model",
			TEXT("Com_Texture_2D"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
			return E_FAIL;
	}
	

	return S_OK;
}

HRESULT C2DMapObject::Bind_ShaderResources()
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

C2DMapObject* C2DMapObject::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	C2DMapObject* pInstance = new C2DMapObject(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		//MSG_BOX("Failed to Created : C2DMapObject");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* C2DMapObject::Clone(void* pArg)
{
	C2DMapObject* pInstance = new C2DMapObject(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		//MSG_BOX("Failed to Cloned : C2DMapObject");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void C2DMapObject::Free()
{
	__super::Free();

	Safe_Release(m_pShader);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pTextureCom);
}

HRESULT C2DMapObject::Cleanup_DeadReferences()
{
	return S_OK;
}
