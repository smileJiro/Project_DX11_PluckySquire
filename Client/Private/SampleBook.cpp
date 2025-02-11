#include "stdafx.h"
#include "SampleBook.h"
#include "GameInstance.h"
#include "Controller_Model.h"
#include "3DModel.h"
#include "Section_Manager.h"
#include "AnimEventGenerator.h"
#include "RenderGroup_WorldPos.h"

#include "Camera_Manager.h"
#include "Camera_2D.h"

CSampleBook::CSampleBook(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CModelObject(_pDevice, _pContext)
{
}

CSampleBook::CSampleBook(const CSampleBook& _Prototype)
	: CModelObject(_Prototype)
{
}


HRESULT CSampleBook::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	return S_OK;
}

HRESULT CSampleBook::Initialize(void* _pArg)
{

	MODELOBJECT_DESC* pDesc = static_cast<MODELOBJECT_DESC*>(_pArg);

	pDesc->Build_3D_Model(pDesc->iCurLevelID,
		L"book",
		L"Prototype_Component_Shader_VtxAnimMesh",
		(_uint)PASS_VTXANIMMESH::RENDERTARGET_MAPP
	);
	pDesc->tTransform3DDesc.vInitialPosition = _float3(2.f, 0.f, -17.3f);
	pDesc->tTransform3DDesc.vInitialScaling = _float3(1.0f, 1.0f, 1.0f);
	pDesc->tTransform3DDesc.fRotationPerSec = XMConvertToRadians(180.f);
	pDesc->tTransform3DDesc.fSpeedPerSec = 0.f;

	pDesc->iRenderGroupID_3D = RG_3D;
	pDesc->iPriorityID_3D = PR3D_GEOMETRY;

	__super::Initialize(_pArg);
	Set_AnimationLoop(COORDINATE_3D, 0, true);
	Set_AnimationLoop(COORDINATE_3D, 8, false);
	Set_AnimationLoop(COORDINATE_3D, 9, true);
	Set_Animation(0);


	Bind_AnimEventFunc("Player_Change_Action", bind(&CSampleBook::PageAction_Call_PlayerEvent, this));

	Register_OnAnimEndCallBack(bind(&CSampleBook::PageAction_End, this, placeholders::_1, placeholders::_2));


	/* Com_AnimEventGenerator */
	CAnimEventGenerator::ANIMEVTGENERATOR_DESC tAnimEventDesc{};
	tAnimEventDesc.pReceiver = this;
	tAnimEventDesc.pSenderModel = Get_Model(COORDINATE_3D);
	m_pAnimEventGenerator = 
		static_cast<CAnimEventGenerator*> 
		(m_pGameInstance->
			Clone_Prototype(PROTOTYPE::PROTO_COMPONENT, m_iCurLevelID, TEXT("Prototype_Component_BookPageActionEvent"), &tAnimEventDesc));
	
	Add_Component(TEXT("AnimEventGenerator"), m_pAnimEventGenerator);



/* Start World Position Map Process */
#pragma region 1. Create RTV, MRT, DSV, RenderGroup
	{
		/* Target_WorldPosMap_Book */
		if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_WorldPosMap_Book"), (_uint)RTSIZE_BOOK2D_X, (_uint)RTSIZE_BOOK2D_Y, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 1.f))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_WorldPosMap_Book"), TEXT("Target_WorldPosMap_Book"))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_DSV_ToRenderer(TEXT("DSV_WorldPosMap_Book"), RTSIZE_BOOK2D_X, RTSIZE_BOOK2D_Y)))
			return E_FAIL;

		CRenderGroup_WorldPos::RG_MRT_DESC Desc;
		Desc.iRenderGroupID = RG_WORLDPOSMAP;
		Desc.iPriorityID = PRWORLD_MAINBOOK;
		Desc.isClear = false;
		Desc.isViewportSizeChange = true;
		Desc.pDSV = m_pGameInstance->Find_DSV(TEXT("DSV_WorldPosMap_Book"));
		Desc.strMRTTag = TEXT("MRT_WorldPosMap_Book");
		Desc.vViewportSize = { RTSIZE_BOOK2D_X , RTSIZE_BOOK2D_Y };
		CRenderGroup_WorldPos* pRenderGroup_WorldPos = CRenderGroup_WorldPos::Create(m_pDevice, m_pContext, &Desc);
		if (nullptr == pRenderGroup_WorldPos)
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_RenderGroup(pRenderGroup_WorldPos->Get_RenderGroupID(), pRenderGroup_WorldPos->Get_PriorityID(), pRenderGroup_WorldPos)))
			return E_FAIL;
		Safe_Release(pRenderGroup_WorldPos);
	}
#pragma endregion // 1


#pragma region 2. Create Read Only Texture (Staging)
	{
		D3D11_TEXTURE2D_DESC desc = {};
		desc.Width = RTSIZE_BOOK2D_X; // 원본 텍스처 너비
		desc.Height = RTSIZE_BOOK2D_Y; // 원본 텍스처 높이
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT; // 원본 텍스처와 동일한 포맷
		desc.SampleDesc.Count = 1;
		desc.Usage = D3D11_USAGE_STAGING; // CPU 읽기 전용
		desc.BindFlags = 0; // 바인딩 없음
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		m_pDevice->CreateTexture2D(&desc, nullptr, &m_pStagingTexture);
	}
#pragma endregion // 2


#pragma region 3. Only One Time Render
	m_pGameInstance->Add_RenderObject_New(RG_WORLDPOSMAP, PRWORLD_MAINBOOK, this);
#pragma endregion // 3

	return S_OK;
}

void CSampleBook::Priority_Update(_float _fTimeDelta)
{
	

	__super::Priority_Update(_fTimeDelta);
}

void CSampleBook::Update(_float _fTimeDelta)
{
	_float3 fDefaultPos = { };

	if (KEY_DOWN(KEY::M))
	{
		Event_Book_Main_Section_Change_Start(1, &fDefaultPos);

	}
	if (KEY_DOWN(KEY::N))
	{
		Event_Book_Main_Section_Change_Start(0, &fDefaultPos);

	}

	if (CCamera_2D::FLIPPING_PAUSE == CCamera_Manager::GetInstance()->Get_CurCameraMode()) {
		if ((ACTION_LAST != m_eCurAction) && true == m_isAction) {

			if (m_eCurAction == NEXT)
			{
				Set_ReverseAnimation(false);
				Set_Animation(8);
			}

			if (m_eCurAction == PREVIOUS)
			{
				Set_ReverseAnimation(true);
				Set_Animation(8);
			}

			m_isAction = false;
		}
	}

	__super::Update(_fTimeDelta);

}

void CSampleBook::Late_Update(_float _fTimeDelta)
{
	//Register_RenderGroup(m_iRenderGroupID_3D, m_iPriorityID_3D);
	__super::Late_Update(_fTimeDelta);
}

HRESULT CSampleBook::Render()
{
	if (FAILED(Bind_ShaderResources_WVP()))
		return E_FAIL;
	COORDINATE eCoord = m_pControllerTransform->Get_CurCoord();
	CShader* pShader = m_pShaderComs[eCoord];
	_uint iShaderPass = m_iShaderPasses[eCoord];
	C3DModel* pModel = static_cast<C3DModel*>(m_pControllerModel->Get_Model(Get_CurCoord()));

	_float2 fLeftStartUV = { 0.f, 0.f };
	_float2 fLeftEndUV = { 0.5f, 1.f };
	_float2 fRightStartUV = { 0.5f, 0.f };
	_float2 fRightEndUV = { 1.f, 1.f };

	for (_uint i = 0; i < (_uint)pModel->Get_Meshes().size(); ++i)
	{
		_uint iShaderPass = 0;
		auto pMesh = pModel->Get_Mesh(i);
		_uint iMaterialIndex = pMesh->Get_MaterialIndex();

		
		if (FAILED(pModel->Bind_Material_PixelConstBuffer(iMaterialIndex, pShader)))
			return E_FAIL;

		switch (iMaterialIndex)
		{
			// 렌더타겟 메쉬들. 
			case Client::CSampleBook::NEXT_RIGHT:
			case Client::CSampleBook::NEXT_LEFT:
			case Client::CSampleBook::CUR_RIGHT:
			case Client::CSampleBook::CUR_LEFT:
		{
	#pragma region uv 매핑하기. (오른쪽 왼쪽 확인)
			if (CUR_LEFT == i || NEXT_LEFT == i)
			{
				if (FAILED(pShader->Bind_RawValue("g_fStartUV", &fLeftStartUV, sizeof(_float2))))
					return E_FAIL;
				if (FAILED(pShader->Bind_RawValue("g_fEndUV", &fLeftEndUV, sizeof(_float2))))
					return E_FAIL;
			}
			else if(CUR_RIGHT == i || NEXT_RIGHT == i)
			{
				if (FAILED(pShader->Bind_RawValue("g_fStartUV", &fRightStartUV, sizeof(_float2))))
					return E_FAIL;
				if (FAILED(pShader->Bind_RawValue("g_fEndUV", &fRightEndUV, sizeof(_float2))))
					return E_FAIL;
			}
			else 
				MSG_BOX("Book Mesh Binding Error - SampleBook.cpp");
	#pragma endregion

	#pragma region 현재 페이지인지, 다음 페이지인지 확인하고 SRV 가져오기.

			ID3D11ShaderResourceView* pResourceView = nullptr;
				switch (m_eCurAction)
				{
					// 이전 페이지로 진행 중이라면, 현재 페이지 -> 바뀌어야 할 페이지(이전 페이지)
					//								다음 페이지 -> 바뀌기 전 페이지(현재 페이지)
				case Client::CSampleBook::PREVIOUS:
					if (!m_isAction)
					{
						if (CUR_LEFT == i || CUR_RIGHT == i)
							pResourceView = SECTION_MGR->Get_SRV_FromRenderTarget(SECTION_MGR->Get_Prev_Section_Key());
						else if (NEXT_LEFT == i || NEXT_RIGHT == i)
							pResourceView = SECTION_MGR->Get_SRV_FromRenderTarget();
						else 
							MSG_BOX("Book Mesh Binding Error - SampleBook.cpp");
						break;
					}
					// 나머지 케이스는				다음 -> 다음 (다음이 없으면, 기본 마테리얼 디퓨즈.)
					//								현재 -> 현재 
					// PREVIOUS 의 경우 m_isAction 전까찌는 일반 렌더(break 안태움 ! )
				case Client::CSampleBook::NEXT:
				case Client::CSampleBook::ACTION_LAST:
				default:

					if (CUR_LEFT == i || CUR_RIGHT == i)
						pResourceView = SECTION_MGR->Get_SRV_FromRenderTarget();
					else if (NEXT_LEFT == i || NEXT_RIGHT == i)
					{
						if (SECTION_MGR->Has_Next_Section())
							pResourceView = SECTION_MGR->Get_SRV_FromRenderTarget(SECTION_MGR->Get_Next_Section_Key());
						else
							if (FAILED(pModel->Bind_Material(pShader, "g_AlbedoTexture", i, aiTextureType_DIFFUSE, 0)))
								continue;
					}
					else
						MSG_BOX("Book Mesh Binding Error - SampleBook.cpp");

					break;
				}
	#pragma endregion


				if (nullptr != pResourceView)
				{
					pShader->Bind_SRV("g_AlbedoTexture", pResourceView);
					iShaderPass = 4;
				}
		}
		break;
		default:
			if (FAILED(pModel->Bind_Material(pShader, "g_AlbedoTexture", i, aiTextureType_DIFFUSE, 0)))
			{
				continue;
			}
			if (FAILED(pModel->Bind_Material(pShader, "g_NormalTexture", i, aiTextureType_NORMALS, 0)))
			{
				int a = 0;
			}
			if (FAILED(pModel->Bind_Material(pShader, "g_ORMHTexture", i, aiTextureType_BASE_COLOR, 0)))
			{
				int a = 0;
			}
			if (FAILED(pModel->Bind_Material(pShader, "g_MetallicTexture", i, aiTextureType_METALNESS, 0)))
			{
				int a = 0;
			}
			if (FAILED(pModel->Bind_Material(pShader, "g_RoughnessTexture", i, aiTextureType_DIFFUSE_ROUGHNESS, 0)))
			{
				int a = 0;
			}
			if (FAILED(pModel->Bind_Material(pShader, "g_AOTexture", i, aiTextureType_AMBIENT_OCCLUSION, 0)))
			{
				int a = 0;
			}
			
			break;
		}

		if (FAILED(pModel->Bind_Matrices(pShader, "g_BoneMatrices", i)))
			return E_FAIL;

		pShader->Begin(iShaderPass);

		pMesh->Bind_BufferDesc();
		pMesh->Render();
	}

	return S_OK;
}

HRESULT CSampleBook::Render_Shadow()
{
	return S_OK;
}

HRESULT CSampleBook::Render_WorldPosMap()
{
#pragma region 1. 책의 메인 페이지 좌, 우 메쉬를 찾아서 렌더.
	if (FAILED(m_pShaderComs[COORDINATE_3D]->Bind_Matrix("g_WorldMatrix", m_pControllerTransform->Get_WorldMatrix_Ptr())))
		return E_FAIL;

	C3DModel* p3DModel = static_cast<C3DModel*>(m_pControllerModel->Get_Model(COORDINATE_3D));
	if (nullptr == p3DModel)
		return E_FAIL;

	CMesh* pLeftMesh = p3DModel->Get_Mesh(CUR_LEFT);
	CMesh* pRightMesh = p3DModel->Get_Mesh(CUR_RIGHT);

	_float2 vStartCoord = { 0.0f, 0.0f };
	m_pShaderComs[COORDINATE_3D]->Bind_RawValue("g_fStartUV", &vStartCoord, sizeof(_float2));
	p3DModel->Bind_Matrices(m_pShaderComs[COORDINATE_3D], "g_BoneMatrices", CUR_LEFT);
	m_pShaderComs[COORDINATE_3D]->Begin((_uint)PASS_VTXANIMMESH::WORLDPOSMAP_BOOK);
	pLeftMesh->Bind_BufferDesc();
	pLeftMesh->Render();

	vStartCoord = { 0.5f, 0.0f };
	m_pShaderComs[COORDINATE_3D]->Bind_RawValue("g_fStartUV", &vStartCoord, sizeof(_float2));
	p3DModel->Bind_Matrices(m_pShaderComs[COORDINATE_3D], "g_BoneMatrices", CUR_RIGHT);
	m_pShaderComs[COORDINATE_3D]->Begin((_uint)PASS_VTXANIMMESH::WORLDPOSMAP_BOOK);
	pRightMesh->Bind_BufferDesc();
	pRightMesh->Render();
#pragma endregion // 1


#pragma region 2. 읽기 전용 텍스쳐에 WorldMapPos를 복사.
	ID3D11ShaderResourceView* pSRV = m_pGameInstance->Get_RT_SRV(TEXT("Target_WorldPosMap_Book"));
	ID3D11Resource* pResource = nullptr;
	pSRV->GetResource(&pResource);
	m_pContext->CopyResource(m_pStagingTexture, pResource);
	Safe_Release(pResource);
#pragma endregion // 2

	CSection_Manager::GetInstance()->Set_BookWorldPosMapTexture(m_pStagingTexture);
    return S_OK;
}

_bool CSampleBook::Book_Action(BOOK_PAGE_ACTION _eAction)
{

	if (ACTION_LAST != m_eCurAction)
		return false;

	switch (_eAction)
	{
	case Client::CSampleBook::PREVIOUS:
		if (!SECTION_MGR->Has_Prev_Section())
			return false;
		SECTION_MGR->SetActive_Section(SECTION_MGR->Get_Prev_Section_Key(),true);
		m_eCurAction = PREVIOUS;
		break;
	case Client::CSampleBook::NEXT:
		if (!SECTION_MGR->Has_Next_Section())
			return false;
		SECTION_MGR->SetActive_Section(SECTION_MGR->Get_Next_Section_Key(),true);
		m_eCurAction = NEXT;
		break;
	case Client::CSampleBook::ACTION_LAST:
		break;
	default:
		break;
	}



	return true;
}

void CSampleBook::PageAction_End(COORDINATE _eCoord, _uint iAnimIdx)
{
#pragma region 책 넘기는 Action

	if (ACTION_LAST != m_eCurAction)
	{
		if (NEXT == m_eCurAction)
		{
			if (SECTION_MGR->Has_Next_Section())
				Event_Book_Main_Section_Change_End(SECTION_MGR->Get_Next_Section_Key());
		}
		else if (PREVIOUS == m_eCurAction)
		{
			// 이거 애니메이션 마지막프레임이 실행되는것 때문에, 앞으로가기와 뒤로가기의
			// 실제 섹션 변경시점을 다르게 해줘야 할거같음 지금상황에선 어쩔수없이.
			// 애니메이션이 못생긴걸 탓합니다. 
			//if (SECTION_MGR->Has_Prev_Section())
			//	Event_Book_Main_Section_Change(SECTION_MGR->Get_Prev_Section_Key()->c_str());
			SECTION_MGR->Change_Prev_Section();
		}
		
		Event_Book_Main_Change(CCamera_Manager::GetInstance()->Get_CameraType());

		Set_Animation(0);
		m_eCurAction = ACTION_LAST;
	}
#pragma endregion
}

void CSampleBook::PageAction_Call_PlayerEvent()
{
	CGameObject* pGameObject = m_pGameInstance->Get_GameObject_Ptr(m_iCurLevelID, L"Layer_Player", 0);

	if (nullptr != pGameObject)
	{
		_wstring strMoveSectionName = L"";
		if (FAILED(SECTION_MGR->Remove_GameObject_ToCurSectionLayer(pGameObject)))
			return;

		if (NEXT == m_eCurAction)
		{
			if (SECTION_MGR->Has_Next_Section())
				strMoveSectionName = SECTION_MGR->Get_Next_Section_Key();
		}
		else if (PREVIOUS == m_eCurAction)
		{
			if (SECTION_MGR->Has_Prev_Section())
				strMoveSectionName = SECTION_MGR->Get_Prev_Section_Key();
		}
		
		if (L"" != strMoveSectionName)
		{
			_vector vNewPos = XMVectorSet(m_fNextPos.x, m_fNextPos.y, 0.f, 1.f);

			pGameObject->Set_Position(vNewPos);

			if (FAILED(SECTION_MGR->Add_GameObject_ToSectionLayer(strMoveSectionName, pGameObject)))
				return;
		}
	}
}

HRESULT CSampleBook::Execute_Action(BOOK_PAGE_ACTION _eAction, _float3 _fNextPosition)
{
	if (Book_Action(_eAction))
	{
		m_fNextPos = _fNextPosition;
		m_isAction = true;
		CCamera_Manager::GetInstance()->Change_CameraMode(CCamera_2D::FLIPPING_UP);
	}
	return S_OK;
}

CSampleBook* CSampleBook::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CSampleBook* pInstance = new CSampleBook(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSampleBook");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSampleBook::Clone(void* _pArg)
{
	CSampleBook* pInstance = new CSampleBook(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : CSampleBook");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSampleBook::Free()
{
	Safe_Release(m_pStagingTexture);

	__super::Free();
}
