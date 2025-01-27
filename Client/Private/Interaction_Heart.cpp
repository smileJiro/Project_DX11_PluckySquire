#include "stdafx.h"
#include "Interaction_Heart.h"
#include "UI_Manager.h"



CInteraction_Heart::CInteraction_Heart(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CUI(_pDevice, _pContext)
{
}

CInteraction_Heart::CInteraction_Heart(const CInteraction_Heart& _Prototype)
	: CUI(_Prototype)
{
}

HRESULT CInteraction_Heart::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CInteraction_Heart::Initialize(void* _pArg)
{
	UIOBJDESC* pDesc = static_cast<UIOBJDESC*>(_pArg);

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	
	m_isRender = true;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	

	return S_OK;
}

void CInteraction_Heart::Priority_Update(_float _fTimeDelta)
{
	// 캐릭터를 따라다니는 하트으
}

void CInteraction_Heart::Update(_float _fTimeDelta)
{
	m_vPlayerPos = CUI_Manager::GetInstance()->Get_Player()->Get_Transform()->Get_State(CTransform::STATE_POSITION);
	_float fPlayerPosX = m_vPlayerPos.m128_f32[1];
	_float fPlayerPosY = m_vPlayerPos.m128_f32[2];


	//fPlayerPosX = g_iWinSizeX / 2.f;
	//fPlayerPosY = g_iWinSizeY / 2.f;

	fPlayerPosX = fPlayerPosX + g_iWinSizeX * 0.5f;
	fPlayerPosY = -fPlayerPosY + g_iWinSizeY * 0.5f;


	m_pControllerTransform->Set_State(CTransform::STATE_POSITION, 
		XMVectorSet(fPlayerPosX, fPlayerPosY, 0.f, 1.f));


	__super::Update(_fTimeDelta);

}

void CInteraction_Heart::Late_Update(_float _fTimeDelta)
{
	__super::Late_Update(_fTimeDelta);
} 

HRESULT CInteraction_Heart::Render()
{
	if (true == m_isRender)
		__super::Render(m_PlayerHP);

	return S_OK;
}



HRESULT CInteraction_Heart::Ready_Components()
{
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader_2D"), reinterpret_cast<CComponent**>(&m_pShaderComs[COORDINATE_2D]))))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_Model_2D"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(Add_Component(m_iCurLevelID, TEXT("Prototype_Component_Texture_HeartPoint"),
		TEXT("Com_Texture_2D"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;


	return S_OK;
}


CInteraction_Heart* CInteraction_Heart::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CInteraction_Heart* pInstance = new CInteraction_Heart(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Created CInteraction_Heart Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject* CInteraction_Heart::Clone(void* _pArg)
{
	CInteraction_Heart* pInstance = new CInteraction_Heart(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Clone CInteraction_Heart Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CInteraction_Heart::Free()
{
	__super::Free();
}

HRESULT CInteraction_Heart::Cleanup_DeadReferences()
{
	return S_OK;
}

