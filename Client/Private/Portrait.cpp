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

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_isRender = true;

	//_float2 vSize;
	//vSize = m_pTextureCom->Get_Size();

	//m_fSizeX = vSize.x;
	//m_fSizeY = vSize.y;
	
	_float2 vCalScale = { 0.f, 0.f };
	vCalScale.x = m_vOriginSize.x * RATIO_BOOK2D_X;
	vCalScale.y = m_vOriginSize.y * RATIO_BOOK2D_Y;

	m_pControllerTransform->Set_Scale(vCalScale.x, vCalScale.y, 1.f);

	CSection_Manager::GetInstance()->Add_GameObject_ToCurSectionLayer(this);

	return S_OK;
}


void CPortrait::Update(_float _fTimeDelta)
{
	m_isRender = Uimgr->Get_PortraitRender();

	Uimgr->Set_DialogId(TEXT("dialog_01"));
	wsprintf(m_tDialogIndex, Uimgr->Get_DialogId());

	//Uimgr->Get_DialogueLine()
	if (Uimgr->Get_DialogueLineIndex() >= Uimgr->Get_Dialogue(m_tDialogIndex)[0].lines.size())
		return;

	wsprintf(m_tDialogIndex, Uimgr->Get_DialogId());
	m_ePortraitFace = (CDialog::PORTRAITNAME)Uimgr->Get_Dialogue(m_tDialogIndex)[0].lines[Uimgr->Get_DialogueLineIndex()].portrait;


	/* 추후 진행에따라 변경을 해야한다. */
	_float2 RTSize = _float2(RTSIZE_BOOK2D_X, RTSIZE_BOOK2D_Y);
	ChangePosition(m_isRender, RTSize);


}

void CPortrait::Late_Update(_float _fTimeDelta)
{
}

HRESULT CPortrait::Render()
{
	if (true == m_isRender)
	{
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

	_float2 vTexPos = Uimgr->Get_DialoguePos();
	
	const auto& currentLine = Uimgr->Get_DialogueLine(m_tDialogIndex, Uimgr->Get_DialogueLineIndex());

	switch (currentLine.location)
	{
	case CDialog::LOC_DEFAULT:  // 가운데 아래
	{
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
		vTexPos.x = vTexPos.x - _RTSize.x * 0.15f;
		vTexPos.y = vTexPos.y;
	//
	// }
		
	}
	break;

	case CDialog::LOC_MIDDLE:   // 정 가운데
	{
		vTexPos = _float2(g_iWinSizeX / 2.f, g_iWinSizeY / 2.f);
	}
	break;

	case CDialog::LOC_MIDLEFT:  // 가운데 좌측
	{
		vTexPos = _float2(g_iWinSizeX / 2.f, g_iWinSizeY / 2.f);
	}
	break;

	case CDialog::LOC_MIDRIGHT: // 가운데 우측
	{
		vTexPos = _float2(g_iWinSizeX / 2.f, g_iWinSizeY / 2.f);
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
