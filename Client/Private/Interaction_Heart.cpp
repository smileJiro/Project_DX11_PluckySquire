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

	m_isRender = false;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	//CSection_Manager::GetInstance()->Add_GameObject_ToCurSectionLayer(this, SECTION_2D_PLAYMAP_UI);

	return S_OK;
}

void CInteraction_Heart::Priority_Update(_float _fTimeDelta)
{

}

void CInteraction_Heart::Update(_float _fTimeDelta)
{
	// 캐릭터를 따라다니는 하트 // 타격받았을때만 노출되게 변경해야한다.

	if (true == Uimgr->Get_PlayerOnHit())
	{
		m_PlayerHP = Uimgr->Get_Player()->Get_Stat().iHP;

		if (m_PrePlayerHP != m_PlayerHP)
		{
			m_isRender = true;
			m_PrePlayerHP = m_PlayerHP;

			if (false == CSection_Manager::GetInstance()->Is_CurSection(this))
				CSection_Manager::GetInstance()->Add_GameObject_ToCurSectionLayer(this);

		}

		Cal_HeartPos();
		Cal_RenderTime(_fTimeDelta, m_PlayerHP);

	}

	__super::Update(_fTimeDelta);

}

void CInteraction_Heart::Late_Update(_float _fTimeDelta)
{

} 

HRESULT CInteraction_Heart::Render()
{
	
	if (true == m_isRender && COORDINATE_2D == Uimgr->Get_Player()->Get_CurCoord())
	{
		// TODO :: 일단은...

		__super::Render(m_PlayerHP);
	}


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

void CInteraction_Heart::Cal_HeartPos()
{
	if (COORDINATE_2D == Uimgr->Get_Player()->Get_CurCoord())
	{
		// TODO :: 해당 부분은 가변적이다. 추후 변경해야한다.
		_float2 RTSize = _float2(RTSIZE_BOOK2D_X, RTSIZE_BOOK2D_Y);

		

		_float2 vPlayerPos = _float2(Uimgr->Get_Player()->Get_BodyPosition().m128_f32[0], Uimgr->Get_Player()->Get_BodyPosition().m128_f32[1]);

		_float2 vCalPos = { 0.f, 0.f };

		vCalPos.x = vPlayerPos.x;
		vCalPos.y = vPlayerPos.y + RTSize.y * 0.175f;

		m_pControllerTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(vCalPos.x, vCalPos.y, 0.f, 1.f));
	}
}

void CInteraction_Heart::Cal_RenderTime(_float _fTimeDelta, _int _iPlayerHP)
{
	m_fRenderTime += _fTimeDelta;

	if (m_fRenderTime >= 3.f)
	{
		m_isRender = false;
		m_fRenderTime = 0.f;
		Uimgr->Set_PlayerOnHit(false);
		CSection_Manager::GetInstance()->Remove_GameObject_ToCurSectionLayer(this);
	}
}

