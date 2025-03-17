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
	m_PrePlayerHP = 12;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}

void CInteraction_Heart::Priority_Update(_float _fTimeDelta)
{

}

void CInteraction_Heart::Update(_float _fTimeDelta)
{
	// 캐릭터를 따라다니는 하트 // 타격받았을때만 노출되게 변경해야한다.
	__super::Update(_fTimeDelta);
}

void CInteraction_Heart::Late_Update(_float _fTimeDelta)
{
	CPlayer* pPlayer = Uimgr->Get_Player();

	if (true == m_isRender && COORDINATE_3D == pPlayer->Get_CurCoord())
	{
		__super::Late_Update(_fTimeDelta);
	}

	//if (true == Uimgr->Get_PlayerOnHit())
	//{
		
		
		m_PlayerHP = pPlayer->Get_Stat().iHP;

		if (m_PrePlayerHP != m_PlayerHP)
		{
			m_isRender = true;
			m_PrePlayerHP = m_PlayerHP;
			m_fRenderTime = 0.f;

			if (COORDINATE_2D == pPlayer->Get_CurCoord())
			{
				if (true == m_isDeleteRender)
					m_isDeleteRender = false;


				if (nullptr != CSection_Manager::GetInstance()->Get_SectionKey(pPlayer))
				{
					auto CurSection = CSection_Manager::GetInstance()->Get_SectionKey(pPlayer);
					m_strSectionName = *(CurSection);
					if (m_preSectionName != m_strSectionName)
					{

						CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(*(CurSection), this, SECTION_2D_PLAYMAP_UI);
						m_preSectionName = m_strSectionName;
					}
				}

			}
			else if (COORDINATE_3D == pPlayer->Get_CurCoord())
			{
				if (false == m_isDeleteRender)
				{
					SECTION_MGR->Remove_GameObject_FromSectionLayer(m_preSectionName, this);

					m_preSectionName = TEXT(" ");
					m_isDeleteRender = true;
				}
			}

		}

		Cal_HeartPos();
		Cal_RenderTime(_fTimeDelta, m_PlayerHP);
	//}
} 

HRESULT CInteraction_Heart::Render()
{
	
	if (true == m_isRender && COORDINATE_2D == Uimgr->Get_Player()->Get_CurCoord())
	{
		// TODO :: 일단은...

		__super::Render(m_PlayerHP);
	}
	else if (true == m_isRender  && COORDINATE_3D == Uimgr->Get_Player()->Get_CurCoord())
	{
		if (FAILED(m_pControllerTransform->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
			return E_FAIL;


		if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
			return E_FAIL;

		if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", m_PlayerHP)))
			return E_FAIL;

		m_pShaderCom->Begin((_uint)0);
		m_pVIBufferCom->Bind_BufferDesc();
		m_pVIBufferCom->Render();
	}


	return S_OK;
}



HRESULT CInteraction_Heart::Ready_Components()
{
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader_2D"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
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
	CPlayer* pPlayer = Uimgr->Get_Player();


	if (COORDINATE_2D == pPlayer->Get_CurCoord())
	{


		// TODO :: 해당 부분은 가변적이다. 추후 변경해야한다.
		_float2 RTSize = _float2(RTSIZE_BOOK2D_X, RTSIZE_BOOK2D_Y);
		_float2 vPlayerPos = _float2(pPlayer->Get_BodyPosition().m128_f32[0], pPlayer->Get_BodyPosition().m128_f32[1]);
		_float2 vCalPos = { 0.f, 0.f };

		vCalPos.x = vPlayerPos.x;
		vCalPos.y = vPlayerPos.y + RTSize.y * 0.175f;

		m_pControllerTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(vCalPos.x, vCalPos.y, 0.f, 1.f));
	}
	else if (COORDINATE_3D == pPlayer->Get_CurCoord())
	{
		//TODO :: 3D 어떻게 표현할것인가?
		_float2 vCalx = __super::WorldToSceen(pPlayer->Get_WorldMatrix());
		_float CalX = vCalx.x - g_iWinSizeX / 2.f;
		_float CalY = -(vCalx.y - g_iWinSizeY / 1.55f);
		m_pControllerTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(CalX, CalY, 0.f, 1.f));
		m_pControllerTransform->Set_Scale(COORDINATE_2D, _float3(m_fSizeX, m_fSizeY, 1.f));
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
	}
}

