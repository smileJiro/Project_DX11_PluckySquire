#include "stdafx.h"
#include "Interaction_E.h"
#include "UI_Manager.h"
#include "Interactable.h"
#include "Section_Manager.h"
#include "Dialog_Manager.h"



CInteraction_E::CInteraction_E(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CUI(_pDevice, _pContext)
{
}

CInteraction_E::CInteraction_E(const CInteraction_E& _Prototype)
	: CUI(_Prototype)
{
}

HRESULT CInteraction_E::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CInteraction_E::Initialize(void* _pArg)
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

void CInteraction_E::Priority_Update(_float _fTimeDelta)
{

}

void CInteraction_E::Update(_float _fTimeDelta)
{
	if (nullptr == Uimgr->Get_Player()->Get_InteractableObject())
	{
		if (true == m_isRender)
			m_isRender = false;

		return;
	}
	else
	{
		if (false == m_isRender)
			m_isRender = true;

		CGameObject* pGameObejct = dynamic_cast<CGameObject*>(Uimgr->Get_Player()->Get_InteractableObject());


		if (false == CSection_Manager::GetInstance()->Is_CurSection(pGameObejct))
			return;

		if (true == Uimgr->Get_Player()->Get_InteractableObject()->Is_UIPlayerHeadUp())
		{
			Cal_PlayerHighPos();
		}
		else if (false == Uimgr->Get_Player()->Get_InteractableObject()->Is_UIPlayerHeadUp())
		{
			Cal_ObjectPos(pGameObejct);
		}
	}
	

}

void CInteraction_E::Late_Update(_float _fTimeDelta)
{

} 

HRESULT CInteraction_E::Render()
{	
	// 다이얼로그 중일땐 랜더 끄기
	if (true == CDialog_Manager::GetInstance()->Get_DisPlayDialogue())
		return S_OK;


	// 인터렉션 진행 중일 때 랜더 끄기



	if (true == m_isRender && COORDINATE_2D == Uimgr->Get_Player()->Get_CurCoord())
	{
		// TODO :: 일단은...

		__super::Render();


		
		_float2 RTSize = _float2(CSection_Manager::GetInstance()->Get_Section_RenderTarget_Size(CSection_Manager::GetInstance()->Get_Cur_Section_Key()));
		//Display_Text(RTSize);

		
		Display_Text(_float3(0.f,0.f,0.f), RTSize);
	}


	return S_OK;
}



HRESULT CInteraction_E::Ready_Components()
{
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader_2D"), reinterpret_cast<CComponent**>(&m_pShaderComs[COORDINATE_2D]))))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_Model_2D"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Interact_E"),
		TEXT("Com_Texture_2D"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;


	return S_OK;
}


CInteraction_E* CInteraction_E::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CInteraction_E* pInstance = new CInteraction_E(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Created CInteraction_E Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject* CInteraction_E::Clone(void* _pArg)
{
	CInteraction_E* pInstance = new CInteraction_E(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Clone CInteraction_E Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CInteraction_E::Free()
{
	__super::Free();
}

HRESULT CInteraction_E::Cleanup_DeadReferences()
{
	return S_OK;
}

void CInteraction_E::Cal_PlayerHighPos()
{
	// 포탈 및 점프

	if (COORDINATE_2D == Uimgr->Get_Player()->Get_CurCoord())
	{
		// TODO :: 해당 부분은 가변적이다. 추후 변경해야한다.
		_float2 RTSize = _float2(CSection_Manager::GetInstance()->Get_Section_RenderTarget_Size(CSection_Manager::GetInstance()->Get_Cur_Section_Key()));

		_float2 vPlayerPos = _float2(Uimgr->Get_Player()->Get_BodyPosition().m128_f32[0], Uimgr->Get_Player()->Get_BodyPosition().m128_f32[1]);

		_float2 vCalPos = { 0.f, 0.f };

		vCalPos.x = vPlayerPos.x;
		vCalPos.y = vPlayerPos.y + RTSize.y * 0.3f;

		m_vObejctPos = _float3(vCalPos.x, vCalPos.y, 1.f);

		m_pControllerTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(vCalPos.x, vCalPos.y, 0.f, 1.f));

		if (false == CSection_Manager::GetInstance()->Is_CurSection(this))
			CSection_Manager::GetInstance()->Add_GameObject_ToCurSectionLayer(this, SECTION_2D_PLAYMAP_UI);
	}
	else if (COORDINATE_3D == Uimgr->Get_Player()->Get_CurCoord())
	{
		//TODO :: 3D 어떻게 표현할것인가?
	}
}

void CInteraction_E::Cal_ObjectPos(CGameObject* _pGameObject)
{
	// 각종 오브젝트
	if (nullptr == Uimgr->Get_Player()->Get_InteractableObject())
		return;

	_float3 vObjectPos = { 0.f, 0.f, 0.f };

	if (COORDINATE_2D == Uimgr->Get_Player()->Get_CurCoord())
	{
		_float2 RTSize = _float2(CSection_Manager::GetInstance()->Get_Section_RenderTarget_Size(CSection_Manager::GetInstance()->Get_Cur_Section_Key()));


		_bool isColumn = { false };

		// 세로가 더 기나요?
		if (RTSize.x < RTSize.y)
		{
			// 세로가 더 길어요!
			isColumn = true;
		}

		// 오브젝트의 위치


		if (false == isColumn)
		{
			vObjectPos.x = _pGameObject->Get_ControllerTransform()->Get_Transform(COORDINATE_2D)->Get_State(CTransform::STATE_POSITION).m128_f32[0];
			vObjectPos.y = _pGameObject->Get_ControllerTransform()->Get_Transform(COORDINATE_2D)->Get_State(CTransform::STATE_POSITION).m128_f32[1];

			// 노출되는  UI의 최종 위치
			m_vObejctPos = _float3(vObjectPos.x, vObjectPos.y + RTSize.y * 0.25f, 1.f);

			if (m_vObejctPos.y <= 300.f)
				m_pControllerTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(vObjectPos.x, m_vObejctPos.y, 0.f, 1.f));
			else
			{
				m_pControllerTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(vObjectPos.x, m_vObejctPos.y - RTSize.y * 0.35f, 0.f, 1.f));
				m_vObejctPos.y = m_vObejctPos.y - RTSize.y * 0.35f;
			}
				
		
		}
		else
		{
			vObjectPos.x = _pGameObject->Get_ControllerTransform()->Get_Transform(COORDINATE_2D)->Get_State(CTransform::STATE_POSITION).m128_f32[0];
			vObjectPos.y = _pGameObject->Get_ControllerTransform()->Get_Transform(COORDINATE_2D)->Get_State(CTransform::STATE_POSITION).m128_f32[1];

			// 노출되는  UI의 최종 위치
			m_vObejctPos = _float3(vObjectPos.x, vObjectPos.y + RTSize.y * 0.025f, 1.f);
			m_pControllerTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(vObjectPos.x, m_vObejctPos.y, 0.f, 1.f));

		}

		

		m_strIntaractName = Uimgr->Get_Player()->Get_InteractableObject()->Get_InteractName();

		if (false == CSection_Manager::GetInstance()->Is_CurSection(this))
			CSection_Manager::GetInstance()->Add_GameObject_ToCurSectionLayer(this, SECTION_2D_PLAYMAP_UI);

		//Display_Text(m_vObejctPos, RTSize);
	}
	


	int a = 0;


}

void CInteraction_E::Cal_DisplayPos(_float2 _vRTSize, CGameObject* _pGameObject)
{
	IInteractable* InteractObject = Uimgr->Get_Player()->Get_InteractableObject();
	CGameObject* pGameObject = dynamic_cast<CGameObject*>(InteractObject);

	int a = 0;

	// 플레이어 위로 노출되는 포탈, 
	// if ()
	//
}

void CInteraction_E::Display_Text(_float3 _vPos, _float2 _vRTSize)
{

	// 글자 어떤걸로 띄울 것인가?
	if (m_strIntaractName == TEXT("NPC"))
	{
		m_strDisplayText = TEXT("대화하기");
	}

	// 위치로 어디로 띄울껀가요?
	// 이건 2D에요
	if (COORDINATE_2D == Uimgr->Get_Player()->Get_CurCoord())
	{
		_float2 vTextPos = { 0.f, 0.f };
		_float2 vMiddlePos = { _vRTSize.x / 2.f, _vRTSize.y / 2.f };

		_bool isColumn = { false };

		if (_vRTSize.x < _vRTSize.y)
		{
			isColumn = true;
		}

		if (false == isColumn)
		{
			vTextPos.x = m_vObejctPos.x + vMiddlePos.x - vMiddlePos.x * 0.01f;
			vTextPos.y = vMiddlePos.y - m_vObejctPos.y - _vRTSize.y * 0.02f;
		}
		else
		{
			vTextPos.x = m_vObejctPos.x + vMiddlePos.x - vMiddlePos.x * 0.01f;
			vTextPos.y = vMiddlePos.y - m_vObejctPos.y - _vRTSize.y * 0.003f;
		}

		m_pGameInstance->Render_Font(TEXT("Font24"), m_strDisplayText.c_str(), _float2(vTextPos.x, vTextPos.y), XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f));
	}
	// 이건 3D에요
	else if (COORDINATE_3D == Uimgr->Get_Player()->Get_CurCoord())
	{

	}

}
