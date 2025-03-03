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
	m_isAddSectionRender = false;
	//CSection_Manager::GetInstance()->Add_GameObject_ToCurSectionLayer(this, SECTION_2D_PLAYMAP_UI);

	return S_OK;
}


void CPortrait::Update(_float _fTimeDelta)
{
	if (m_isPortraitRender)
	{
		_float2 RTSize = _float2(RTSIZE_BOOK2D_X, RTSIZE_BOOK2D_Y);
		ChangePosition(m_isPortraitRender, RTSize);
	}
	


}

void CPortrait::Late_Update(_float _fTimeDelta)
{
	if (m_isPortraitRender)
	{
		if (!m_is2D)
		{
			Register_RenderGroup(RENDERGROUP::RG_3D, PRIORITY_3D::PR3D_UI);
		}

		if (!m_isAddSectionRender)
		{
			CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(m_strSection, this, SECTION_2D_PLAYMAP_UI);
			m_isAddSectionRender = true;
		}
		
		
		

	}
	//else
	//{
	//	if (!m_isAddSectionRender)
	//	{
	//		CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(m_strSection, this, SECTION_2D_PLAYMAP_UI);
	//		m_isAddSectionRender = true;
	//	}
	//}
}

HRESULT CPortrait::Render()
{
	if (m_isPortraitRender)
	{
		// 렌더 타깃 사이즈 (예: 상수값 사용)
		
		__super::Render((int)m_ePortraitFace, PASS_VTXPOSTEX::DEFAULT);
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

	//_float3 vTexPos = Uimgr->Get_CalDialoguePos();
	//
	//const auto& currentLine = Uimgr->Get_DialogueLine(m_tDialogIndex, Uimgr->Get_DialogueLineIndex());
	//
	//if (true == Uimgr->Get_Dialogue(Uimgr->Get_DialogId())[0].lines[Uimgr->Get_DialogueLineIndex()].is2D)
	//{
	//	m_pControllerTransform->Get_Transform(COORDINATE_2D)->Set_Scale(m_vDisplay2DSize.x, m_vDisplay2DSize.y, 1.f);
	//}
	//else if (false == Uimgr->Get_Dialogue(Uimgr->Get_DialogId())[0].lines[Uimgr->Get_DialogueLineIndex()].is2D)
	//{
	//	m_pControllerTransform->Get_Transform(COORDINATE_2D)->Set_Scale(m_vDisplay3DSize.x, m_vDisplay3DSize.y, 1.f);
	//}

	//_float3 pos = m_vDialoguePos;

	//if (!m_isAddSectionRender)
	//{
	//
	//	wstring CurrentDialog(Uimgr->Get_Dialogue(Uimgr->Get_DialogId())[0].Section);
	//	CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(CurrentDialog, this, SECTION_2D_PLAYMAP_UI);
	//	Get_Transform()->Set_State(CTransform::STATE_POSITION, XMVectorSet(vTexPos.x, vTexPos.y, 0.f, 1.f));
	//	//CSection_Manager::GetInstance()->Add_GameObject_ToCurSectionLayer(this, SECTION_2D_PLAYMAP_UI);
	//	m_isAddSectionRender = true;
	//}

	if (!_isRender)
		return;

	// 변경됨: 외부로부터 전달받은 대화 기준 위치 사용
	_float3 pos = m_vDialoguePos;

	// 2D/3D 여부에 따라 스케일 설정 (여기서는 동일하게 처리하지만 필요하면 분기 추가)
	if (m_is2D)
	{
		m_pControllerTransform->Get_Transform(COORDINATE_2D)->Set_Scale(m_vDisplay2DSize.x, m_vDisplay2DSize.y, 1.f);
	}
	else
	{
		m_pControllerTransform->Get_Transform(COORDINATE_2D)->Set_Scale(m_vDisplay2DSize.x, m_vDisplay2DSize.y, 1.f);
	}

	if (!m_isAddSectionRender)
	{
		CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(m_strSection, this, SECTION_2D_PLAYMAP_UI);
		m_isAddSectionRender = true;
	}

	Get_Transform()->Set_State(CTransform::STATE_POSITION, XMVectorSet(pos.x -_RTSize.x * 0.12f, pos.y, pos.z, 1.f));

}

void CPortrait::SetPortraitState(_bool _bPortraitRender, _bool _is2D, PORT _eFace,  const _float3& _vDialoguePos, const wstring& _strSection)
{
	m_isPortraitRender	= _bPortraitRender;
	m_is2D				= _is2D;
	m_ePortraitFace		= _eFace;
	m_vDialoguePos		= _vDialoguePos;
	m_strSection		= _strSection;
}

HRESULT CPortrait::Ready_Components()
{
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader_2D"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
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
