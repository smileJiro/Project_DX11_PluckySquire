#include "stdafx.h"
#include "Narration_Anim.h"
#include "2DModel.h"
#include "GameInstance.h"

#include "UI_Manager.h"
#include "Narration.h"

CNarration_Anim::CNarration_Anim(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CUI(_pDevice, _pContext)
{
}

CNarration_Anim::CNarration_Anim(const CNarration_Anim& _Prototype)
	:CUI(_Prototype)
{
}



HRESULT CNarration_Anim::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CNarration_Anim::Initialize(void* _pArg)
{
	// TODO :: 해당 RT는 가변적이므로 추후 수정해야한다.
	_float2 vRTSize = _float2(RTSIZE_BOOK2D_X, RTSIZE_BOOK2D_Y);


	CNarration::NarrationData* pDesc = static_cast<CNarration::NarrationData*>(_pArg);


	//pDesc->eStartCoord = COORDINATE_2D;
	//pDesc->isCoordChangeEnable = false;

	m_iCurLevelID = pDesc->iCurLevelID = pDesc->eCurlevelId;

	if (0 < pDesc->lines[pDesc->LineCount].NarAnim.size())
	{
		m_strAnimationId = pDesc->lines[pDesc->LineCount].NarAnim[pDesc->AnimIndex].strAnimationid;

		if (true == pDesc->lines[pDesc->LineCount].isLeft)
		{
			pDesc->fX = m_vPos.x = pDesc->lines[pDesc->LineCount].NarAnim[0].vPos.x;
			pDesc->fY = m_vPos.y = pDesc->lines[pDesc->LineCount].NarAnim[0].vPos.y;
		}
		else if (false == pDesc->lines[pDesc->LineCount].isLeft)
		{
			pDesc->fX = m_vPos.x = pDesc->lines[pDesc->LineCount].NarAnim[pDesc->AnimIndex].vPos.x + vRTSize.x / 2.f;
			pDesc->fY = m_vPos.y = pDesc->lines[pDesc->LineCount].NarAnim[pDesc->AnimIndex].vPos.y;
		}



		pDesc->fSizeX = m_fAnimationScale.x = pDesc->lines[pDesc->LineCount].NarAnim[0].vAnimationScale.x;
		pDesc->fSizeY = m_fAnimationScale.y = pDesc->lines[pDesc->LineCount].NarAnim[0].vAnimationScale.y;


	}

	m_fWaitingTime = pDesc->lines[pDesc->LineCount].fwaitingTime;


	m_isRender = true;

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (true == pDesc->lines[pDesc->LineCount].NarAnim[0].isLoop)
	{
		m_pModelCom->Set_AnimationLoop(0, true);
	}
	
	m_pModelCom->Set_Animation(pDesc->lines[pDesc->LineCount].NarAnim[0].iAnimationIndex);
	//CSection_Manager::GetInstance()->Add_GameObject_ToCurSectionLayer(this, SECTION_2D_PLAYMAP_BACKGROUND);

	CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(pDesc->strSectionid, this);

	_float2 vPos = { 0.f, 0.f };

	vPos = m_vPos;

	vPos.x = vPos.x - vRTSize.x / 2.f;
	vPos.y = -vPos.y + vRTSize.y / 2.f;

	m_pControllerTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(vPos.x, vPos.y, 0.f, 1.f));
	m_pControllerTransform->Set_Scale(COORDINATE_2D, _float3(m_vOriginSize.x, m_vOriginSize.y, 1.f));
	//m_pControllerTransform->Set_Scale(m_fAnimationScale.x, m_fAnimationScale.y, 1.f);

	//m_pControllerTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));


	return S_OK;

}


void CNarration_Anim::Priority_Update(_float _fTimeDelta)
{
	__super::Priority_Update(_fTimeDelta);
}

void CNarration_Anim::Update(_float _fTimeDelta)
{
	__super::Update(_fTimeDelta);

	if (true == m_isPlayAnimation)
	{
		m_pModelCom->Play_Animation(_fTimeDelta, false);
	}
	



	//// TODO :: TEST용도
	//_float2 vRTSize = _float2(RTSIZE_BOOK2D_X, RTSIZE_BOOK2D_Y);
	//_float2 vPos = { 2000.f, 400.f };
	//
	//
	//
	//vPos.x = vPos.x - vRTSize.x / 2.f;
	//vPos.y = -vPos.y + vRTSize.y / 2.f;
	//
	//m_pControllerTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(vPos.x, vPos.y, 0.f, 1.f));

}

void CNarration_Anim::Late_Update(_float _fTimeDelta)
{
	//Register_RenderGroup(RENDERGROUP::RG_3D, PRIORITY_3D::PR3D_UI);
	//__super::Late_Update(_fTimeDelta);

}

HRESULT CNarration_Anim::Render()
{	

	_matrix matLocal = *static_cast<C2DModel*>(m_pModelCom)->Get_CurrentSpriteTransform();
	_matrix matRatioScalling = XMMatrixScaling((_float)RATIO_BOOK2D_X, (_float)RATIO_BOOK2D_Y, 1.f);
	matLocal *= matRatioScalling;

	//_matrix matWorld = matLocal * XMLoadFloat4x4(&m_WorldMatrices[COORDINATE_2D]);

	_matrix matWorld = matLocal * XMLoadFloat4x4(m_pControllerTransform->Get_Transform(COORDINATE_2D)->Get_WorldMatrix_Ptr());

	_float4x4 matWorld4x4;
	XMStoreFloat4x4(&matWorld4x4, matWorld);
	if (FAILED(m_pShaderComs[COORDINATE_2D]->Bind_Matrix("g_WorldMatrix", &matWorld4x4)))
		return E_FAIL;


	//__super::Render();c
	//if (FAILED(m_pControllerTransform->Bind_ShaderResource(m_pShaderComs[COORDINATE_2D], "g_WorldMatrix")))
	//	return E_FAIL;
	//
	//
	//if (FAILED(m_pShaderComs[COORDINATE_2D]->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
	//	return E_FAIL;
	//
	//if (FAILED(m_pShaderComs[COORDINATE_2D]->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
	//	return E_FAIL;
	//__super::Render(m_pModelCom);

	if (m_isPlayAnimation == true)
	{
		

		m_pModelCom->Render(m_pShaderComs[COORDINATE_2D], (_uint)PASS_VTXPOSTEX::SPRITE2D);
		int a = 0;
		//Register_RenderGroup()
	}

	

	return S_OK;
}




void CNarration_Anim::On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx)
{

}

void CNarration_Anim::StartAnimation()
{
	m_isPlayAnimation = true;
}


HRESULT CNarration_Anim::Ready_Components()
{	
	// 셰이더를 만든다.
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader_2D"), reinterpret_cast<CComponent**>(&m_pShaderComs[COORDINATE_2D]))))
		return E_FAIL;

	// 모델쪽 생성하는 관련 부분 // 모델 게임레벨
	CComponent* pComponent = static_cast<CComponent*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_COMPONENT, m_iCurLevelID, m_strAnimationId, nullptr));

	if (nullptr == pComponent)
		return E_FAIL;

	m_pModelCom = static_cast<C2DModel*>(pComponent);

	return S_OK;
}



CNarration_Anim* CNarration_Anim::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CNarration_Anim* pInstance = new CNarration_Anim(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Created CPick_Bulb Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject* CNarration_Anim::Clone(void* _pArg)
{
	CNarration_Anim* pInstance = new CNarration_Anim(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Clone CNarration_Anim Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CNarration_Anim::Free()
{
	__super::Free();
}

HRESULT CNarration_Anim::Cleanup_DeadReferences()
{
	return S_OK;
}
