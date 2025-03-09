#include "stdafx.h"
#include "2DDefault_RenderObject.h"

#include "GameInstance.h"


C2DDefault_RenderObject::C2DDefault_RenderObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject(pDevice, pContext)
{
}

C2DDefault_RenderObject::C2DDefault_RenderObject(const C2DDefault_RenderObject& Prototype)
	: CUIObject(Prototype)
{
}

HRESULT C2DDefault_RenderObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT C2DDefault_RenderObject::Initialize(void* pArg)
{
	DEFAULT_RENDER_OBJECT_DESC* pDesc = nullptr;
	
	DEFAULT_RENDER_OBJECT_DESC Desc = {};
	pDesc = &Desc;




	if (pArg != nullptr)
		pDesc = static_cast<DEFAULT_RENDER_OBJECT_DESC*>(pArg);
	
	pDesc->eStartCoord = COORDINATE_2D;
	pDesc->iCurLevelID = m_iCurLevelID = LEVEL_TOOL_2D_MAP;



	if (FAILED(CGameObject::Initialize(pDesc)))
		return E_FAIL;


	Change_RenderGroup(_float2((_float)RTSIZE_BOOK2D_X, (_float)RTSIZE_BOOK2D_Y), RG_2D, PR2D_BOOK_SECTION);

	


	if (FAILED(Ready_Components()))
		return E_FAIL;


	return S_OK;
}

void C2DDefault_RenderObject::Priority_Update(_float fTimeDelta)
{
	Register_RenderGroup(m_iRenderGroupID, m_iPriorityID);
	m_isBackColorRender = false;
}

void C2DDefault_RenderObject::Update(_float fTimeDelta)
{

}

void C2DDefault_RenderObject::Late_Update(_float fTimeDelta)
{
	Register_RenderGroup(RG_3D, PR3D_UI);
}

HRESULT C2DDefault_RenderObject::Render()
{

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

		//ColorAlpha 3
		//Default 0
	if (!m_isBackColorRender)
	{

		if (m_isDefaultRenderMode)
		{
			m_pShader->Begin((_uint)PASS_VTXPOSTEX::COLOR_ALPHA);
		}
		else 
		{
			m_pShader->Begin(0);

		}
		m_isBackColorRender = !m_isBackColorRender;

	}
	else
	{
		if (!m_is2DMode)
			return S_OK;
		m_pShader->Begin(0);
	}


	m_pVIBufferCom->Bind_BufferDesc();

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT C2DDefault_RenderObject::Change_RenderGroup(_float2 _fRenderTargetSize, _int _iRenderGroupID, _int _iPriorityID)
{
	m_fTargetSize = _fRenderTargetSize;



	if (_fRenderTargetSize.x > _fRenderTargetSize.y)
	{
		_float fBookY = _fRenderTargetSize.y * ((_float)g_iWinSizeX / (_float)_fRenderTargetSize.x);
		m_fX = g_iWinSizeX >> 1;
		m_fY = (_float)((_uint)fBookY >> 1);
		m_fSizeX = g_iWinSizeX;
		m_fSizeY = fBookY;
	}
	else 
	{
		_float fBookX = _fRenderTargetSize.x * ((_float)g_iWinSizeY / (_float)_fRenderTargetSize.y);
		m_fX = (_float)((_uint)fBookX >> 1);
		m_fY = g_iWinSizeY >> 1;
		m_fSizeX = fBookX;
		m_fSizeY = g_iWinSizeY;
	}


	/* 직교튀영을 위한 뷰ㅡ, 투영행르을 만들었다. */
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f));


	/* 던져준 fX, fY,  fSizeX, fSizeY로 그려질 수 있도록 월드행렬의 상태를 제어해둔다. */
	m_pControllerTransform->Set_Scale(m_fSizeX, m_fSizeY, 1.f);
	m_pControllerTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, 0.f, 1.f));


	return S_OK;
}

void C2DDefault_RenderObject::Texture_Output(const _wstring& _strTexturePath)
{
	if (nullptr != m_pTextureCom)
	{
		ID3D11ShaderResourceView* pSRV = m_pTextureCom->Get_SRV(0);

		ID3D11Texture2D* pTexture2D = nullptr;
		ID3D11Resource* pResource = nullptr;

		pSRV->GetResource(&pResource);
		if (pResource)
		{
			HRESULT hr = pResource->QueryInterface(__uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pTexture2D));
			pResource->Release();

			if (SUCCEEDED(hr) && pTexture2D)
			{
				if (FAILED(SaveDDSTextureToFile(m_pContext, pTexture2D, _strTexturePath.c_str())))
					return;
			}
		}
	}
}

void C2DDefault_RenderObject::Set_Texture_Mode(const _string& _strTextureName)
{
	m_isDefaultRenderMode = false;
	// 텍스쳐 불러오는 코드
	_wstring strPath = L"../../Client/Bin/Resources/Textures/Map/" + StringToWstring(_strTextureName);
	//m_pTextureCom = CTexture::Create(m_pDevice, m_pContext, strPath.c_str(), 1, true, false);
	m_pTextureCom = CTexture::Create(m_pDevice, m_pContext, strPath.c_str());

	if (nullptr == m_pTextureCom)
	{ 
		MSG_BOX("Map Load Texture Create Fail !");
	}

}

void C2DDefault_RenderObject::Set_Default_Render_Mode()
{
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTextureCom);
	m_isDefaultRenderMode = true;
}

_float2 C2DDefault_RenderObject::Get_Texture_Size()
{
	if (nullptr != m_pTextureCom)
		return m_pTextureCom->Get_Size();
	return _float2(-1.f,-1.f);
}

_bool C2DDefault_RenderObject::IsCursor_In(_float2 _fCursorPos)
{
	//_float fOffX = m_fSizeX - (_float)g_iWinSizeX;
	//_float fOffY = m_fSizeY - (_float)g_iWinSizeY;
	//fOffX *= 0.5f;
	//fOffY *= 0.5f;
	//if (
	//	fOffX < _fCursorPos.x &&
	//	g_iWinSizeX - fOffX  > _fCursorPos.x && 
	//	fOffY < _fCursorPos.y && 
	//	g_iWinSizeY - fOffY  > _fCursorPos.y 
	//	)
		return true;
	//else 
	//	return false;
}

HRESULT C2DDefault_RenderObject::Ready_Components()
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

HRESULT C2DDefault_RenderObject::Bind_ShaderResources()
{

	Update_RenderWorld();
	if (FAILED(m_pControllerTransform->Bind_ShaderResource(m_pShader, "g_WorldMatrix")))
		return E_FAIL;

	//if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
	//	return E_FAIL;

	if (!m_isBackColorRender)
	{ 
		//XMStoreFloat4x4(&m_TargetProjMatrix, XMMatrixOrthographicLH((_float)m_fTargetSize.x, (_float)m_fTargetSize.y, 0.0f, 1.0f));
		if (FAILED(m_pShader->Bind_RawValue("g_vColors", &m_fBackColor, sizeof(_float4))))
			return E_FAIL;
		//if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_TargetProjMatrix)))
		//	return E_FAIL;

		if (nullptr != m_pTextureCom)
		{
			if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShader, "g_DiffuseTexture")))
				return E_FAIL;
		}
	}
	else
	{
		if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
			return E_FAIL;

		if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(m_pShader, "g_DiffuseTexture", m_DefaultRenderTargetName)))
			return E_FAIL;
	}
	




	return S_OK;
}

void C2DDefault_RenderObject::Update_RenderWorld()
{
	if (!m_isBackColorRender)
	{
		m_pControllerTransform->Set_Scale(m_fTargetSize.x, m_fTargetSize.y, 1.f);
		m_pControllerTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_fTargetSize.x * 0.5f - m_fTargetSize.x * 0.5f, -(m_fTargetSize.y * 0.5f) + m_fTargetSize.y * 0.5f, 0.f, 1.f));
	}
	else 
	{
		m_pControllerTransform->Set_Scale(m_fSizeX, m_fSizeY, 1.f);
		m_pControllerTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_fX - m_fSizeX * 0.5f, 0, 0.f, 1.f));

	}


}

C2DDefault_RenderObject* C2DDefault_RenderObject::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	C2DDefault_RenderObject* pInstance = new C2DDefault_RenderObject(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : C2DDefault_RenderObject");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* C2DDefault_RenderObject::Clone(void* pArg)
{
	C2DDefault_RenderObject* pInstance = new C2DDefault_RenderObject(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : C2DDefault_RenderObject");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void C2DDefault_RenderObject::Free()
{
	__super::Free();

	Safe_Release(m_pShader);
	Safe_Release(m_pVIBufferCom);
}

HRESULT C2DDefault_RenderObject::Cleanup_DeadReferences()
{
	return S_OK;
}
