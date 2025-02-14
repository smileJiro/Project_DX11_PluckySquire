#include "stdafx.h"
#include "Portrait.h"
#include "GameInstance.h"
#include "UI_Manager.h"
#include "Dialogue.h"
#include "Section_Manager.h"

CPortrait::CPortrait(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CUI (_pDevice, _pContext)
{
}

CPortrait::CPortrait(const CPortrait& _Prototype)
	: CUI( _Prototype )
{
}

HRESULT CPortrait::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPortrait::Initialize(void* _pArg)
{
	UIOBJDESC* pDesc = static_cast<UIOBJDESC*>(_pArg);
	m_vDisplay3DSize = _float2(pDesc->fSizeX * 0.5f, pDesc->fSizeY * 0.5f);

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_isRender = false;

	//_float2 vSize;
	//vSize = m_pTextureCom->Get_Size();

	//m_fSizeX = vSize.x;
	//m_fSizeY = vSize.y;
	
	_float2 vCalScale = { 0.f, 0.f };
	vCalScale.x = m_vOriginSize.x * RATIO_BOOK2D_X;
	vCalScale.y = m_vOriginSize.y * RATIO_BOOK2D_Y;
	m_vDisplay2DSize = vCalScale;
	m_pControllerTransform->Set_Scale(m_vDisplay2DSize.x, m_vDisplay2DSize.y, 1.f);

	//CSection_Manager::GetInstance()->Add_GameObject_ToCurSectionLayer(this, SECTION_2D_PLAYMAP_UI);

	return S_OK;
}


void CPortrait::Update(_float _fTimeDelta)
{




}

void CPortrait::Late_Update(_float _fTimeDelta)
{
	if (true == Uimgr->Get_DisplayDialogue() && COORDINATE_3D == Uimgr->Get_Player()->Get_CurCoord())
	{
		Register_RenderGroup(RENDERGROUP::RG_3D, PRIORITY_3D::PR3D_UI);
	}
	else if (true == Uimgr->Get_DisplayDialogue() && COORDINATE_2D == Uimgr->Get_Player()->Get_CurCoord())
	{
		if (!m_isAddSectionRender)
		{
			wstring CurrentDialog(Uimgr->Get_Dialogue(Uimgr->Get_DialogId())[0].Section);
				CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(CurrentDialog, this);


			m_isAddSectionRender = true;
			//CSection_Manager::GetInstance()->Add_GameObject_ToCurSectionLayer(this, SECTION_2D_PLAYMAP_UI);
		}
	}
}

HRESULT CPortrait::Render()
{
	if (true == Uimgr->Get_PortraitRender())
	{
		wsprintf(m_tDialogIndex, Uimgr->Get_DialogId());
		m_isRender = Uimgr->Get_DialogueLine(m_tDialogIndex, Uimgr->Get_DialogueLineIndex()).isPortrait;
	}
	else
	{
		m_isRender = false;
	}

	if (true == m_isRender)
	{
		wsprintf(m_tDialogIndex, Uimgr->Get_DialogId());

		if (Uimgr->Get_DialogueLineIndex() >= Uimgr->Get_Dialogue(m_tDialogIndex)[0].lines.size())
			return E_FAIL;

		wsprintf(m_tDialogIndex, Uimgr->Get_DialogId());
		m_ePortraitFace = (CDialog::PORTRAITNAME)Uimgr->Get_Dialogue(m_tDialogIndex)[0].lines[Uimgr->Get_DialogueLineIndex()].portrait;


		/* 추후 진행에따라 변경을 해야한다. */
		_float2 RTSize = _float2(RTSIZE_BOOK2D_X, RTSIZE_BOOK2D_Y);
		ChangePosition(m_isRender, RTSize);

		__super::Render((_int)m_ePortraitFace, PASS_VTXPOSTEX::DEFAULT);
	}
	return S_OK;
}

HRESULT CPortrait::DisplayPortrait()
{
	return S_OK;
}

void CPortrait::ChangePosition(_bool _isRender, _float2 _RTSize)
{
	if (false == _isRender)
		return;

	_float3 vTexPos = Uimgr->Get_CalDialoguePos();

	const auto& currentLine = Uimgr->Get_DialogueLine(m_tDialogIndex, Uimgr->Get_DialogueLineIndex());

	if (COORDINATE_2D == Uimgr->Get_Player()->Get_CurCoord())
	{
		m_pControllerTransform->Get_Transform(COORDINATE_2D)->Set_Scale(m_vDisplay2DSize.x, m_vDisplay2DSize.y, 1.f);
	}
	else if (COORDINATE_3D == Uimgr->Get_Player()->Get_CurCoord())
	{
		m_pControllerTransform->Get_Transform(COORDINATE_2D)->Set_Scale(m_vDisplay3DSize.x, m_vDisplay3DSize.y, 1.f);
	}

	switch (currentLine.location)
	{
	case CDialog::LOC_MIDDOWN:  // 가운데 아래
	{



		if (COORDINATE_2D == Uimgr->Get_Player()->Get_CurCoord())
		{
			if (!m_isAddSectionRender)
			{
				wstring CurrentDialog(Uimgr->Get_Dialogue(Uimgr->Get_DialogId())[0].Section);
				CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(CurrentDialog, this);
				//CSection_Manager::GetInstance()->Add_GameObject_ToCurSectionLayer(this, SECTION_2D_PLAYMAP_UI);
				m_isAddSectionRender = true;
			}

			vTexPos.x = vTexPos.x - _RTSize.x * 0.12f;
			vTexPos.y = vTexPos.y;

			//vPos.y -= _RTSize.y * 0.13f;
		}
		else if (COORDINATE_3D == Uimgr->Get_Player()->Get_CurCoord())
		{
			if (true == m_isAddSectionRender)
			{
				CSection_Manager::GetInstance()->Remove_GameObject_ToCurSectionLayer(this);
				m_isAddSectionRender = false;
			}

			vTexPos = _float3(g_iWinSizeX / 5.3f, g_iWinSizeY - g_iWinSizeY / 6.85f, 0.f);

			m_pControllerTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(vTexPos.x - g_iWinSizeX * 0.5f, -vTexPos.y + g_iWinSizeY * 0.5f, 0.f, 1.f));

			return;

		}





		// 3D 세팅
		//if (3D)
		//{
		//	_float2 vPos = { 0.f, 0.f };
		//
		//	vPos = _float2(g_iWinSizeX / 4.1f, g_iWinSizeY - g_iWinSizeY / 3.25f);
		//}
		//vTextPos = _float2(g_iWinSizeX / 4.1f, g_iWinSizeY - g_iWinSizeY / 3.25f);

		//2D 
	//else
	//{
		//vTexPos.x = vTexPos.x - _RTSize.x * 0.12f;
		//vTexPos.y = vTexPos.y;
	//
	// }
		
	}
	break;

	case CDialog::LOC_MIDHIGH:   // 정 가운데
	{
		vTexPos.x = vTexPos.x - _RTSize.x * 0.12f;
		vTexPos.y = vTexPos.y;
	}
	break;

	case CDialog::LOC_MIDLEFT:  // 가운데 좌측
	{
		vTexPos.x = vTexPos.x - _RTSize.x * 0.12f;
		vTexPos.y = vTexPos.y;
	}
	break;

	case CDialog::LOC_MIDRIGHT: // 가운데 우측
	{
		vTexPos.x = vTexPos.x - _RTSize.x * 0.12f;
		vTexPos.y = vTexPos.y;
	}
	break;
	case CDialog::LOC_LEFTDOWN: // 가운데 우측
	{
		vTexPos.x = vTexPos.x - _RTSize.x * 0.12f;
		vTexPos.y = vTexPos.y;

	}
	break;

	case CDialog::LOC_LEFTHIGH: // 가운데 우측
	{
		vTexPos.x = vTexPos.x - _RTSize.x * 0.12f;
		vTexPos.y = vTexPos.y;
	}
	break;

	case CDialog::LOC_RIGHTHIGH: // 가운데 우측
	{
		vTexPos.x = vTexPos.x - _RTSize.x * 0.12f;
		vTexPos.y = vTexPos.y;
	}
	break;

	case CDialog::LOC_RIGHDOWN: // 가운데 우측
	{
		vTexPos.x = vTexPos.x - _RTSize.x * 0.12f;
		vTexPos.y = vTexPos.y;
	}
	break;
	}
	Get_Transform()->Set_State(CTransform::STATE_POSITION, XMVectorSet(vTexPos.x, vTexPos.y, 0.f, 1.f));
}

HRESULT CPortrait::Ready_Components()
{
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader_2D"), reinterpret_cast<CComponent**>(&m_pShaderComs[COORDINATE_2D]))))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_Model_2D"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(Add_Component(m_iCurLevelID, TEXT("Prototype_Component_Texture_DialoguePortrait"),
		TEXT("Com_Texture_2D"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}


CPortrait* CPortrait::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CPortrait* pInstance = new CPortrait(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Created CPortrait Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject* CPortrait::Clone(void* _pArg)
{
	CPortrait* pInstance = new CPortrait(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Clone CPortrait Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CPortrait::Free()
{

	//CSection_Manager::GetInstance()->Remove_GameObject_ToCurSectionLayer(this);
	__super::Free();
	
}

HRESULT CPortrait::Cleanup_DeadReferences()
{

	return S_OK;
}
