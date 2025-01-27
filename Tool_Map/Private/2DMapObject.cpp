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
	//_float fScale = 1.f / max(fImageSize.x, fImageSize.y);
	//fImageSize.x *= fScale;
	//fImageSize.y *= fScale;
	//m_fX += fImageSize.x * 0.5f;
	m_fY -= fImageSize.y * 0.5f;
	m_pControllerTransform->Set_Scale(fImageSize.x , fImageSize.y , 1.f );
	m_pControllerTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_fX, -m_fY, 0.f, 1.f));

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
	_wstring strProtoTag = Desc->strProtoTag;
	// TODO :: 텍스쳐 매핑방식 구조 구성후 추후 수정, 0125 박예슬
	//_wstring TestTag[] = {
	//	L"Flower",
	//	L"bush",
	//	L"TD_Tree",
	//	L"ButterflyBlue",
	//	L"barrel",
	//	L"townsign",
	//	L"actionsignpost",
	//	L"treegreenfallen",
	//	L"treegreenbrokentrunk",
	//	L"largemossrock",
	//};

	//_int iTagIndex = -1;

	//for (_uint i = 0; i < (_uint)size(TestTag); i++)
	//{
	//	if (ContainWstring(strProtoTag, TestTag[i]))
	//	{
	//		iTagIndex = i;
	//		break;
	//	}
	//}
	//if (iTagIndex == -1)
	//{
		return E_FAIL;
	//}

	/* Com_Texture */
	//if (FAILED(Add_Component(m_iCurLevelID, TestTag[iTagIndex],
	//	TEXT("Com_Texture_2D"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
	//	return E_FAIL;



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
