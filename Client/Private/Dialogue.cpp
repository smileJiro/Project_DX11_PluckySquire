#include "stdafx.h"
#include "Dialogue.h"
#include "GameInstance.h"

CDialogue::CDialogue(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CUI (_pDevice, _pContext)
{
}

CDialogue::CDialogue(const CDialogue& _Prototype)
	: CUI ( _Prototype )
{
}

HRESULT CDialogue::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDialogue::Initialize(void* _pArg)
{
	UIOBJDESC* pDesc = static_cast<UIOBJDESC*>(_pArg);

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_isRender = true;

	return S_OK;
}

void CDialogue::Priority_Update(_float _fTimeDelta)
{
}

void CDialogue::Update(_float _fTimeDelta)
{
}

void CDialogue::Late_Update(_float _fTimeDelta)
{
	__super::Late_Update(_fTimeDelta);
}

HRESULT CDialogue::Render()
{
	if (true == m_isRender)
		__super::Render(0, PASS_VTXPOSTEX::DEFAULT);

	return S_OK;
}

void CDialogue::Display_Dialogues()
{
	
}





HRESULT CDialogue::Ready_Components()
{
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader_2D"), reinterpret_cast<CComponent**>(&m_pShaderComs[COORDINATE_2D]))))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_Model_2D"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(Add_Component(m_iCurLevelID, TEXT("Prototype_Component_Texture_PickBulb"),
		TEXT("Com_Texture_2D"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}


CDialogue* CDialogue::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CDialogue* pInstance = new CDialogue(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Created CDialogue Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject* CDialogue::Clone(void* _pArg)
{
	CDialogue* pInstance = new CDialogue(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Clone CDialogue Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CDialogue::Free()
{
	//for (_int i = 0 ; i < m_Dialogues.size(); ++i)
	//{
	//	for (int j = 0; j < m_Dialogues[i].vDialogue.size(); ++j)
	//	{
	//		Safe_Delete_Array(m_Dialogues[i].vDialogue[j]);
	//
	//	}
	//}


	__super::Free();
	
}

HRESULT CDialogue::Cleanup_DeadReferences()
{

	return S_OK;
}
