#include "stdafx.h"
#include "ArrowForStamp.h"
#include "GameInstance.h"
#include "UI_Manager.h"
#include "Book.h"


CArrowForStamp::CArrowForStamp(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CUI (_pDevice, _pContext)
{
}

CArrowForStamp::CArrowForStamp(const CArrowForStamp& _Prototype)
	: CUI ( _Prototype )
{
}

HRESULT CArrowForStamp::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CArrowForStamp::Initialize(void* _pArg)
{
	UIOBJDESC* pDesc = static_cast<UIOBJDESC*>(_pArg);

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;





	//m_pControllerTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(g_iWinSizeX/ 2, g_iWinSizeY / 2, 1.f, 1.f));
	//m_pControllerTransform->Set_Scale(50.f, 50.f, 1.f);
	return S_OK;
}

void CArrowForStamp::Priority_Update(_float _fTimeDelta)
{
}

void CArrowForStamp::Update(_float _fTimeDelta)
{
}

void CArrowForStamp::Late_Update(_float _fTimeDelta)
{
	CBook* pBook = Uimgr->Get_Book();

	if (nullptr == pBook)
		return;

	if (true == pBook->Get_PlayerAbove())
	{
		if (true == Uimgr->Get_StampHave(0) && true == Uimgr->Get_StampHave(1))
			__super::Late_Update(_fTimeDelta);
	}
		
}

HRESULT CArrowForStamp::Render()
{
	CUI_Manager* pUIManager = CUI_Manager::GetInstance();
	if (false == pUIManager->Get_StampHave(0) || false == pUIManager->Get_StampHave(1))
		return S_OK;

	if (FAILED(m_pControllerTransform->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture")))
		return E_FAIL;


	m_pShaderCom->Begin((_uint)PASS_VTXPOSTEX::DEFAULT);
	m_pVIBufferCom->Bind_BufferDesc();
	m_pVIBufferCom->Render();

	//__super::Render();
	

	return S_OK;
}



HRESULT CArrowForStamp::Ready_Components()
{
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader_2D"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_Model_2D"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(Add_Component(m_iCurLevelID, TEXT("Prototype_Component_Texture_ArrowForStamp"),
		TEXT("Com_Texture_2D"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}


CArrowForStamp* CArrowForStamp::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CArrowForStamp* pInstance = new CArrowForStamp(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Created CArrowForStamp Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject* CArrowForStamp::Clone(void* _pArg)
{
	CArrowForStamp* pInstance = new CArrowForStamp(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Clone CArrowForStamp Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CArrowForStamp::Free()
{
	
	__super::Free();
	
}

HRESULT CArrowForStamp::Cleanup_DeadReferences()
{

	return S_OK;
}
