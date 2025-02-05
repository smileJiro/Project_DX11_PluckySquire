#include "stdafx.h"
#include "SampleBook.h"
#include "GameInstance.h"
#include "Controller_Model.h"
#include "3DModel.h"
#include "Section_Manager.h"


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
	pDesc->iPriorityID_3D = PR3D_NONBLEND;

	__super::Initialize(_pArg);
	Set_AnimationLoop(COORDINATE_3D, 0, true);
	Set_AnimationLoop(COORDINATE_3D, 8, false);
	Set_AnimationLoop(COORDINATE_3D, 9, true);
	Set_Animation(0);

	Register_OnAnimEndCallBack(bind(&CSampleBook::PageAction_End, this, placeholders::_1, placeholders::_2));

	return S_OK;
}

void CSampleBook::Priority_Update(_float _fTimeDelta)
{
	__super::Priority_Update(_fTimeDelta);
}

void CSampleBook::Update(_float _fTimeDelta)
{
	if (KEY_DOWN(KEY::M))
	{
		if (Book_Action(NEXT))
		{
			Set_ReverseAnimation(false);
			Set_Animation(8);
		}
	}
	if (KEY_DOWN(KEY::N))
	{
		if (Book_Action(PREVIOUS))
		{
			Set_ReverseAnimation(true);
			Set_Animation(8);
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

	// 
	_uint iMaxActiveSectionCount = CSection_Manager::GetInstance()->Get_MaxCurActiveSectionCount();
	_uint iMainPageIndex = iMaxActiveSectionCount / 2;

	for (_uint i = 0; i < (_uint)pModel->Get_Meshes().size(); ++i)
	{
		_uint iShaderPass = 0;
		auto pMesh = pModel->Get_Mesh(i);
		_uint iMaterialIndex = pMesh->Get_MaterialIndex();


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
					if (CUR_LEFT == i || CUR_RIGHT == i)
						pResourceView = SECTION_MGR->Get_SRV_FromRenderTarget(*(SECTION_MGR->Get_Prev_Section_Key()));
					else if (NEXT_LEFT == i || NEXT_RIGHT == i)
						pResourceView = SECTION_MGR->Get_SRV_FromRenderTarget();
					else 
						MSG_BOX("Book Mesh Binding Error - SampleBook.cpp");
					break;
					// 나머지 케이스는				다음 -> 다음 (다음이 없으면, 기본 마테리얼 디퓨즈.)
					//								현재 -> 현재 
				case Client::CSampleBook::NEXT:
				case Client::CSampleBook::ACTION_LAST:
				default:

					if (CUR_LEFT == i || CUR_RIGHT == i)
						pResourceView = SECTION_MGR->Get_SRV_FromRenderTarget();
					else if (NEXT_LEFT == i || NEXT_RIGHT == i)
					{
						if (SECTION_MGR->Has_Next_Section())
							pResourceView = SECTION_MGR->Get_SRV_FromRenderTarget(*(SECTION_MGR->Get_Next_Section_Key()));
						else
							if (FAILED(pModel->Bind_Material(pShader, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0)))
								continue;
					}
					else
						MSG_BOX("Book Mesh Binding Error - SampleBook.cpp");

					break;
				}
	#pragma endregion


				if (nullptr != pResourceView)
				{
					pShader->Bind_SRV("g_DiffuseTexture", pResourceView);
					iShaderPass = 4;
				}
		}
		break;
		default:
			if (FAILED(pModel->Bind_Material(pShader, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0)))
			{
				continue;
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

_bool CSampleBook::Book_Action(BOOK_PAGE_ACTION _eAction)
{

	if (ACTION_LAST != m_eCurAction)
		return false;

	switch (_eAction)
	{
	case Client::CSampleBook::PREVIOUS:
		if (!SECTION_MGR->Has_Prev_Section())
			return false;
		m_eCurAction = PREVIOUS;

		break;
	case Client::CSampleBook::NEXT:
		if (!SECTION_MGR->Has_Next_Section())
			return false;
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
			SECTION_MGR->Change_Next_Section();
		else if (PREVIOUS == m_eCurAction)
			SECTION_MGR->Change_Prev_Section();
		Set_Animation(0);
		m_eCurAction = ACTION_LAST;
	}
#pragma endregion
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

	__super::Free();
}
