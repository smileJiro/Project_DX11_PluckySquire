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

	ChangePosition(m_isRender);


}

void CPortrait::Late_Update(_float _fTimeDelta)
{
}

HRESULT CPortrait::Render()
{
	if (true == m_isRender)
	{
		__super::Render((_int)m_ePortraitFace, PASS_VTXPOSTEX::UI_POINTSAMPLE);
	}
		
	return S_OK;
}

HRESULT CPortrait::DisplayPortrait()
{
	return S_OK;
}

void CPortrait::ChangePosition(_bool _isRender)
{
	if (false == _isRender)
		return;

	_float2 vTexPos = _float2(0.f, 0.f);
	const auto& currentLine = Uimgr->Get_DialogueLine(m_tDialogIndex, Uimgr->Get_DialogueLineIndex());

	switch (currentLine.location)
	{
	case CDialog::LOC_DEFAULT:  // 啊款单 酒贰
	{
		// 3D rlwns
		//vTextPos = _float2(g_iWinSizeX / 4.1f, g_iWinSizeY - g_iWinSizeY / 3.25f);

		//2D 
		vTexPos = _float2(g_iWinSizeX / 2.f / 3.9f, g_iWinSizeY - g_iWinSizeY / 5.4f);
		vTexPos.x = vTexPos.x - g_iWinSizeX * 0.5f;
		vTexPos.y = -vTexPos.y + g_iWinSizeY * 0.5f;
	}
	break;

	case CDialog::LOC_MIDDLE:   // 沥 啊款单
	{
		vTexPos = _float2(g_iWinSizeX / 2.f, g_iWinSizeY / 2.f);
	}
	break;

	case CDialog::LOC_MIDLEFT:  // 啊款单 谅螟
	{
		vTexPos = _float2(g_iWinSizeX / 2.f, g_iWinSizeY / 2.f);
	}
	break;

	case CDialog::LOC_MIDRIGHT: // 啊款单 快螟
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
