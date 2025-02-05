#include "stdafx.h"
#include "PrintFloorWord.h"
#include "GameInstance.h"
#include "UI_Manager.h"



CPrintFloorWord::CPrintFloorWord(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CFloorWord(_pDevice, _pContext)
{
}

CPrintFloorWord::CPrintFloorWord(const CPrintFloorWord& _Prototype)
	: CFloorWord(_Prototype)
{
}

HRESULT CPrintFloorWord::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CPrintFloorWord::Initialize(void* _pArg)
{
	UIOBJDESC* pDesc = static_cast<UIOBJDESC*>(_pArg);

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	

	return S_OK;
}

void CPrintFloorWord::Priority_Update(_float _fTimeDelta)
{
}

void CPrintFloorWord::Update(_float _fTimeDelta)
{

}

void CPrintFloorWord::Late_Update(_float _fTimeDelta)
{
}

HRESULT CPrintFloorWord::Render()
{
	if (true == m_isRender && false == CUI_Manager::GetInstance()->Get_isESC())
		__super::Render();

	return S_OK;
}



HRESULT CPrintFloorWord::Ready_Components()
{
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader_2D"), reinterpret_cast<CComponent**>(&m_pShaderComs[COORDINATE_2D]))))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_Model_2D"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(Add_Component(m_iCurLevelID, TEXT("Prototype_Component_Texture_KEYQ"),
		TEXT("Com_Texture_2D"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;


	return S_OK;
}


CPrintFloorWord* CPrintFloorWord::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CPrintFloorWord* pInstance = new CPrintFloorWord(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Created CPrintFloorWord Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject* CPrintFloorWord::Clone(void* _pArg)
{
	CPrintFloorWord* pInstance = new CPrintFloorWord(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Clone CPrintFloorWord Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CPrintFloorWord::Free()
{
	__super::Free();
}

HRESULT CPrintFloorWord::Cleanup_DeadReferences()
{
	return S_OK;
}

//HRESULT CPrintFloorWord::Cleanup_DeadReferences()
//{
//	return S_OK;
//}

