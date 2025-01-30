#include "RenderGroup.h"
#include "GameInstance.h"

CRenderGroup::CRenderGroup(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: m_pDevice(_pDevice)
	, m_pContext(_pContext)
	, m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}

HRESULT CRenderGroup::Initialize(void* _pArg)
{
	RG_DESC* pDesc = static_cast<RG_DESC*>(_pArg);

	// Save Desc 
	m_iRenderGroupID = pDesc->iRenderGroupID;
	m_iPriorityID = pDesc->iPriorityID;
	m_iFinalID = m_iRenderGroupID + m_iPriorityID;

	return S_OK;
}

HRESULT CRenderGroup::Add_RenderObject(CGameObject* _pGameObject)
{
	if (nullptr == _pGameObject)
		return E_FAIL;

	m_GroupObjects.push_back(_pGameObject);
	Safe_AddRef(_pGameObject);
	return S_OK;
}

HRESULT CRenderGroup::Render(CShader* _pRTShader, CVIBuffer_Rect* _pRTBuffer)
{
	for (auto& pGroupObject : m_GroupObjects)
	{
		if (nullptr != pGroupObject && true == pGroupObject->Is_Render())
			pGroupObject->Render();

		Safe_Release(pGroupObject);
	}
	m_GroupObjects.clear();

	return S_OK;
}

HRESULT CRenderGroup::Setup_Viewport(_float2 _vViewportSize)
{
	D3D11_VIEWPORT			ViewPortDesc;
	ZeroMemory(&ViewPortDesc, sizeof(D3D11_VIEWPORT));
	ViewPortDesc.TopLeftX = 0;
	ViewPortDesc.TopLeftY = 0;
	ViewPortDesc.Width = _vViewportSize.x;
	ViewPortDesc.Height = _vViewportSize.y;
	ViewPortDesc.MinDepth = 0.f;
	ViewPortDesc.MaxDepth = 1.f;

	m_pContext->RSSetViewports(1, &ViewPortDesc);

	return S_OK;
}

CRenderGroup* CRenderGroup::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, void* _pDesc)
{
	CRenderGroup* pInstance = new CRenderGroup(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize(_pDesc)))
	{
		MSG_BOX("Failed Create CRenderGroup");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRenderGroup::Free()
{
	for (auto& pGroupObject : m_GroupObjects)
		Safe_Release(pGroupObject);
	m_GroupObjects.clear();

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);

	__super::Free();
}
