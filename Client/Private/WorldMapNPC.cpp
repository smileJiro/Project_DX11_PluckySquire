#include "stdafx.h"
#include "WorldMapNPC.h"
#include "WorldMapNpc_Jot.h"
#include "WorldMapNpc_Thrash.h"
#include "WorldMapNpc_Violet.h"
#include "GameInstance.h"
#include "Section_Manager.h"
#include "Camera_Manager.h"
#include "GameInstance.h"




CWorldMapNPC::CWorldMapNPC(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CCharacter(_pDevice, _pContext)
{
}

CWorldMapNPC::CWorldMapNPC(const CWorldMapNPC& _Prototype)
	: CCharacter( _Prototype )
{
}

HRESULT CWorldMapNPC::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CWorldMapNPC::Initialize(void* _pArg)
{
	//tagContainerObjectDesc* pDesc = static_cast<tagContainerObjectDesc*>(_pArg);


	CONTAINEROBJ_DESC* pDesc = static_cast<CONTAINEROBJ_DESC*>(_pArg);

	pDesc->eStartCoord = COORDINATE_2D;
	pDesc->iNumPartObjects = WORLDMAP_NPC::NPC_LAST;
	pDesc->isCoordChangeEnable = false;
	pDesc->tTransform2DDesc.fRotationPerSec = XMConvertToRadians(180.f);
	pDesc->tTransform2DDesc.fSpeedPerSec = 100.f;

	// 이게 맞나?
	pDesc->iObjectGroupID = OBJECT_GROUP::MAPOBJECT;
	
	if (FAILED(__super::Initialize(pDesc)))
	{
		MSG_BOX("WorldMap_NPC SuperIntialize Failed");
		return E_FAIL;
	}
		
	if (FAILED(Ready_PartObjects()))
	{
		MSG_BOX("WorldMap_Npc ReadyPartObjects Failed");
		return E_FAIL;
	}
		
	Pos_Ready();

	switch (pDesc->iCurLevelID)
	{
	case LEVEL_CHAPTER_2:
	{
		CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(TEXT("Chapter2_Worldmap_0"), this);
		m_iStartIndex = POS_HONEYBEE;
	}
	break;

	case LEVEL_CHAPTER_4:
	{
		CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(TEXT("Chapter4_Worldmap_0"), this);
		m_iStartIndex = POS_TOWER;
	}
		break;

	case LEVEL_CHAPTER_6:
	{
		CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(TEXT("Chapter6_Worldmap_0"), this);
		m_iStartIndex = POS_SWAMPEND;
	}
	break;
	}

	m_vStartPos = m_PosMoves[m_iStartIndex];
	m_pControllerTransform->Set_State(CTransform::STATE_POSITION, _float4(m_vStartPos.x, m_vStartPos.y, 0.f, 1.f));

	return S_OK;
}

void CWorldMapNPC::Priority_Update(_float _fTimeDelta)
{
}

void CWorldMapNPC::Update(_float _fTimeDelta)
{
	Progress(_fTimeDelta);
	__super::Update(_fTimeDelta);
	
	

}

void CWorldMapNPC::Late_Update(_float _fTimeDelta)
{
	__super::Late_Update(_fTimeDelta);
}

HRESULT CWorldMapNPC::Render()
{
	DisplayLocationName();

	return S_OK;
}

HRESULT CWorldMapNPC::Ready_PartObjects()
{
	/////////////// 조트 ///////////////////
	CWorldMapNpc_Jot::WORLDMAP_PART_JOT_DESC JotDesc{};
	JotDesc.pParent = this;
	JotDesc.eStartCoord = COORDINATE_2D;
	JotDesc.iCurLevelID = m_iCurLevelID;
	JotDesc.isCoordChangeEnable = false;
	JotDesc.iModelPrototypeLevelID_2D = LEVEL_STATIC;
	JotDesc.strShaderPrototypeTag_2D = TEXT("Prototype_Component_Shader_VtxPosTex");
	JotDesc.iShaderPass_2D = (_uint)PASS_VTXPOSTEX::SPRITE2D;
	JotDesc.pParentMatrices[COORDINATE_2D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_2D);
	JotDesc.tTransform2DDesc.vInitialPosition = _float3(0.0f, 0.0f, 0.0f);
	JotDesc.tTransform2DDesc.vInitialScaling = _float3(1.f, 1.f, 1.f);
	JotDesc.tTransform2DDesc.fRotationPerSec = XMConvertToRadians(180.f);

	m_PartObjects[NPC_JOT] = static_cast<CWorldMapNpc_Jot*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_WorldMapNPC_Jot"), &JotDesc));

	if (nullptr == m_PartObjects[NPC_JOT])
	{
		MSG_BOX("Create WorldMapNPC_Jot Failed");
		return E_FAIL;
	}

	static_cast<CModelObject*>(m_PartObjects[NPC_JOT])->Switch_Animation(0);
	/////////////// 조트 ///////////////////

	/////////////// 쓰레쉬 /////////////////
	CWorldMapNpc_Thrash::WORLDMAP_PART_Thrash_DESC ThrashDesc{};
	ThrashDesc.pParent = this;
	ThrashDesc.eStartCoord = COORDINATE_2D;
	ThrashDesc.iCurLevelID = m_iCurLevelID;
	ThrashDesc.isCoordChangeEnable = false;
	ThrashDesc.iModelPrototypeLevelID_2D = LEVEL_STATIC;
	ThrashDesc.strShaderPrototypeTag_2D = TEXT("Prototype_Component_Shader_VtxPosTex");
	ThrashDesc.iShaderPass_2D = (_uint)PASS_VTXPOSTEX::SPRITE2D;
	ThrashDesc.pParentMatrices[COORDINATE_2D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_2D);
	ThrashDesc.tTransform2DDesc.vInitialPosition = _float3(0.0f, 0.0f, 0.0f);
	ThrashDesc.tTransform2DDesc.vInitialScaling = _float3(1.f, 1.f, 1.f);
	ThrashDesc.tTransform2DDesc.fRotationPerSec = XMConvertToRadians(180.f);

	m_PartObjects[NPC_THRASH] = static_cast<CWorldMapNpc_Thrash*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_WorldMapNPC_Thrash"), &JotDesc));

	if (nullptr == m_PartObjects[NPC_THRASH])
	{
		MSG_BOX("Create WorldMapNPC_Thrash Failed");
		return E_FAIL;
	}

	static_cast<CModelObject*>(m_PartObjects[NPC_THRASH])->Switch_Animation(7);
	/////////////// 쓰레쉬 /////////////////

	////////////// 바이올렛 ////////////////
	CWorldMapNpc_Violet::WORLDMAP_PART_Violet_DESC ViolethDesc{};
	ThrashDesc.pParent = this;
	ThrashDesc.eStartCoord = COORDINATE_2D;
	ThrashDesc.iCurLevelID = m_iCurLevelID;
	ThrashDesc.isCoordChangeEnable = false;
	ThrashDesc.iModelPrototypeLevelID_2D = LEVEL_STATIC;
	ThrashDesc.strShaderPrototypeTag_2D = TEXT("Prototype_Component_Shader_VtxPosTex");
	ThrashDesc.iShaderPass_2D = (_uint)PASS_VTXPOSTEX::SPRITE2D;
	ThrashDesc.pParentMatrices[COORDINATE_2D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_2D);
	ThrashDesc.tTransform2DDesc.vInitialPosition = _float3(0.0f, 0.0f, 0.0f);
	ThrashDesc.tTransform2DDesc.vInitialScaling = _float3(1.f, 1.f, 1.f);
	ThrashDesc.tTransform2DDesc.fRotationPerSec = XMConvertToRadians(180.f);

	m_PartObjects[NPC_VIOLET] = static_cast<CWorldMapNpc_Violet*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_WorldMapNPC_Violet"), &JotDesc));

	if (nullptr == m_PartObjects[NPC_VIOLET])
	{
		MSG_BOX("Create WorldMapNPC_Violet Failed");
		return E_FAIL;
	}

	static_cast<CModelObject*>(m_PartObjects[NPC_VIOLET])->Switch_Animation(11);
	////////////// 바이올렛 ////////////////


	return S_OK;
}

HRESULT CWorldMapNPC::Progress(_float _fTimeDelta)
{

	CSection_Manager* pSectionManager = CSection_Manager::GetInstance();

	if (TEXT("Chapter2_Worldmap_0") != pSectionManager->Get_Cur_Section_Key() &&
		TEXT("Chapter4_Worldmap_0") != pSectionManager->Get_Cur_Section_Key() &&
		TEXT("Chapter6_Worldmap_0") != pSectionManager->Get_Cur_Section_Key() &&
		TEXT("Chapter6_Worldmap_01") != pSectionManager->Get_Cur_Section_Key())
	{
		return S_OK;
	}

	if (STATE_READY == m_eState)
	{

		if (1.f > m_fReadyTime)
		{
			m_fReadyTime += _fTimeDelta;
		}
		else
		{
			m_eState = STATE_WAIT;
		}
	}
	else if (STATE_WAIT == m_eState)
	{
		if (false == m_isChangeCameraTarget)
		{
			m_isChangeCameraTarget = true;
		}
			
		if (1.5f > m_fWaitTime)
		{
			m_fWaitTime += _fTimeDelta;
		}
		else
		{
			m_eState = CWorldMapNPC::STATE_WALK;
		}
	}
	else if (STATE_WALK == m_eState)
	{
		if (false == m_isUpdatePos)
		{
			if (m_iStartIndex == 6)
			{
				assert(TEXT("WorldMapNpc -> Out of array"));
				return E_FAIL;
			}

			m_vStartPos = m_PosMoves[m_iStartIndex];
			m_vArrivePos = m_PosMoves[m_iStartIndex + 1];
			
			m_isUpdatePos = true;
		}

		if (false == m_isWalkAnim)
		{
			if (POS_HONEYBEE == m_iStartIndex)
			{
				_vector vRight = m_PartObjects[NPC_JOT]->Get_ControllerTransform()->Get_State(CTransform::STATE_RIGHT);
				m_PartObjects[NPC_JOT]->Get_ControllerTransform()->Set_State(CTransform::STATE_RIGHT, -XMVectorAbs(vRight));

				vRight = m_PartObjects[NPC_THRASH]->Get_ControllerTransform()->Get_State(CTransform::STATE_RIGHT);
				m_PartObjects[NPC_THRASH]->Get_ControllerTransform()->Set_State(CTransform::STATE_RIGHT, -XMVectorAbs(vRight));

				vRight = m_PartObjects[NPC_VIOLET]->Get_ControllerTransform()->Get_State(CTransform::STATE_RIGHT);
				m_PartObjects[NPC_VIOLET]->Get_ControllerTransform()->Set_State(CTransform::STATE_RIGHT, -XMVectorAbs(vRight));

				static_cast<CModelObject*>(m_PartObjects[NPC_JOT])->Switch_Animation(2);
				static_cast<CModelObject*>(m_PartObjects[NPC_THRASH])->Switch_Animation(9);
				static_cast<CModelObject*>(m_PartObjects[NPC_VIOLET])->Switch_Animation(13);
			}
			else
			{
				static_cast<CModelObject*>(m_PartObjects[NPC_JOT])->Switch_Animation(2);
				static_cast<CModelObject*>(m_PartObjects[NPC_THRASH])->Switch_Animation(9);
				static_cast<CModelObject*>(m_PartObjects[NPC_VIOLET])->Switch_Animation(13);
			}

			m_isWalkAnim = true;
		}
		
		if (true == Move_To(XMVectorSet(m_vArrivePos.x, m_vArrivePos.y, 0.f, 1.f), _fTimeDelta))
		{
			m_eState = STATE_ARRIVE;
		}
	}
	else if (STATE_ARRIVE == m_eState)
	{
		if (false == m_isArriveAnim)
		{
			if (POS_TOWER == m_iStartIndex || POS_ATRIA == m_iStartIndex)
			{
				static_cast<CModelObject*>(m_PartObjects[NPC_JOT])->Switch_Animation(1);
				static_cast<CModelObject*>(m_PartObjects[NPC_THRASH])->Switch_Animation(8);
				static_cast<CModelObject*>(m_PartObjects[NPC_VIOLET])->Switch_Animation(12);

				m_isArriveAnim = true;
			}
			else
			{
				static_cast<CModelObject*>(m_PartObjects[NPC_JOT])->Switch_Animation(3);
				static_cast<CModelObject*>(m_PartObjects[NPC_THRASH])->Switch_Animation(10);
				static_cast<CModelObject*>(m_PartObjects[NPC_VIOLET])->Switch_Animation(14);

				m_isArriveAnim = true;
			}
		}

		if (3.f > m_fWaitTime)
		{
			m_fWaitTime += _fTimeDelta;
		}
		else
		{
			_float2 vPos = Change_PlayerPos();
			_float3 vPlayerPos = _float3(vPos.x, vPos.y, 0.f);

			m_fReadyTime = 0;
			m_fWaitTime = 0;
			m_fWaitTime = 0;
			m_eState = STATE_READY;
			m_isWalkAnim = false;
			m_isArriveAnim = false;
			m_isUpdatePos = false;
			m_isChangeCameraTarget = false;

			_int CurIndex = (_uint)m_iStartIndex;
			CurIndex += 1;
			m_iStartIndex = (MOVEPOS)CurIndex;

			_vector vRight = m_PartObjects[NPC_JOT]->Get_ControllerTransform()->Get_State(CTransform::STATE_RIGHT);
			m_PartObjects[NPC_JOT]->Get_ControllerTransform()->Set_State(CTransform::STATE_RIGHT, XMVectorAbs(vRight));

			vRight = m_PartObjects[NPC_THRASH]->Get_ControllerTransform()->Get_State(CTransform::STATE_RIGHT);
			m_PartObjects[NPC_THRASH]->Get_ControllerTransform()->Set_State(CTransform::STATE_RIGHT, XMVectorAbs(vRight));

			vRight = m_PartObjects[NPC_VIOLET]->Get_ControllerTransform()->Get_State(CTransform::STATE_RIGHT);
			m_PartObjects[NPC_VIOLET]->Get_ControllerTransform()->Set_State(CTransform::STATE_RIGHT, XMVectorAbs(vRight));
			

			Change_BookOrder();
			Event_Book_Main_Section_Change_Start(1, &vPlayerPos);
			CSection_Manager::GetInstance()->Remove_GameObject_ToCurSectionLayer(this);

			if (4 == m_iStartIndex)
			{
				m_PartObjects[NPC_VIOLET]->Set_Render(false);
				m_PartObjects[NPC_JOT]->Get_ControllerTransform()->Set_State(CTransform::STATE_POSITION, XMVectorSet(30.f, 0.f, 0.f, 1.f));
				m_PartObjects[NPC_THRASH]->Get_ControllerTransform()->Set_State(CTransform::STATE_POSITION, XMVectorSet(-25.f, 0.f, 0.f, 1.f));
				CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(TEXT("Chapter6_Worldmap_01"), this);
			}
		}
	}

	return S_OK;
}
 
HRESULT CWorldMapNPC::Pos_Ready()
{
	m_PosMoves[0] = _float2(-243.0f, -60.0f);
	m_PosMoves[1] = _float2(-450.f, 13.53f);
	m_PosMoves[2] = _float2(-354.f, 86.19f);
	m_PosMoves[3] = _float2(-110.f, -91.3f);
	m_PosMoves[4] = _float2(2.76f, -71.3f);
	m_PosMoves[5] = _float2(227.6f, -162.f);

	return S_OK;
}

void CWorldMapNPC::Change_BookOrder()
{
	wstring strPreSection = SECTION_MGR->Get_Prev_Section_Key();
	wstring strNextSection = SECTION_MGR->Get_Next_Section_Key();

	if (L"" == strPreSection)
		return;

	CSection* pSection = SECTION_MGR->Find_Section(strPreSection);
	if (nullptr != pSection)
		static_cast<CSection_2D*>(pSection)->Set_NextPageTag(strNextSection);
}

_float2 CWorldMapNPC::Change_PlayerPos()
{
	_float2 ChangPlayerPos = { 0.f, 0.f };

	if (POS_HONEYBEE == m_iStartIndex)
		ChangPlayerPos = _float2(-687.f, 182.4f);
	else if (POS_TOWER == m_iStartIndex)
		ChangPlayerPos = _float2(-1968.f, -685.f);
	else if (POS_SWAMPEND == m_iStartIndex)
		ChangPlayerPos = _float2(-14.3f, -664.f);
	else if (POS_ATRIA == m_iStartIndex)
		ChangPlayerPos = _float2(-1195.f, -160.f);

	return ChangPlayerPos;
}

										// 하드 코딩 //
HRESULT CWorldMapNPC::DisplayLocationName()
{
	_float2 vMidPoint = { RTSIZE_BOOK2D_X / 2.f, RTSIZE_BOOK2D_Y / 2.f };

	_float2 vCalPos = { 0.f, 0.f };
	_float2 vTextPos = { 0.f, 0.f };

	switch (m_iStartIndex)
	{
	case POS_HONEYBEE:
	{
		DisplayHoneyBee(vMidPoint);
	}
	break;

	case POS_TOWER:
	{
		DisplayHoneyBee(vMidPoint);
		vTextPos = _float2(-272.f, 188.1f);
		m_strLocationName = TEXT("시의 늪");

		vCalPos.x = vMidPoint.x + vTextPos.x;
		vCalPos.y = vMidPoint.y - vTextPos.y;

		if (FAILED(m_pGameInstance->Render_Font(TEXT("Font18"), m_strLocationName.c_str(), _float2((vCalPos.x), (vCalPos.y + 25.f)), XMVectorSet(0.f, 0.f, 0.f, 1.f))))
			return E_FAIL;
	}
	break;

	case POS_SWAMPEND:
	{
		DisplayHoneyBee(vMidPoint);
		vTextPos = _float2(-272.f, 188.1f);
		m_strLocationName = TEXT("시의 늪");

		vCalPos.x = vMidPoint.x + vTextPos.x;
		vCalPos.y = vMidPoint.y - vTextPos.y;

		if (FAILED(m_pGameInstance->Render_Font(TEXT("Font18"), m_strLocationName.c_str(), _float2((vCalPos.x), (vCalPos.y + 25.f)), XMVectorSet(0.f, 0.f, 0.f, 1.f))))
			return E_FAIL;

		vTextPos = _float2(99.1f, 70.5f);
		m_strLocationName = TEXT("아르티아");

		vCalPos.x = vMidPoint.x + vTextPos.x;
		vCalPos.y = vMidPoint.y - vTextPos.y;

		if (FAILED(m_pGameInstance->Render_Font(TEXT("Font30"), m_strLocationName.c_str(), _float2((vCalPos.x), (vCalPos.y + 25.f)), XMVectorSet(0.f, 0.f, 0.f, 1.f))))
			return E_FAIL;
	}
	break;

	case POS_ATRIA:
	{
		DisplayHoneyBee(vMidPoint);
		vTextPos = _float2(-272.f, 188.1f);
		m_strLocationName = TEXT("시의 늪");

		vCalPos.x = vMidPoint.x + vTextPos.x;
		vCalPos.y = vMidPoint.y - vTextPos.y;

		if (FAILED(m_pGameInstance->Render_Font(TEXT("Font18"), m_strLocationName.c_str(), _float2((vCalPos.x), (vCalPos.y + 25.f)), XMVectorSet(0.f, 0.f, 0.f, 1.f))))
			return E_FAIL;
		
		vTextPos = _float2(99.1f, 70.5f);
		m_strLocationName = TEXT("아르티아");

		vCalPos.x = vMidPoint.x + vTextPos.x;
		vCalPos.y = vMidPoint.y - vTextPos.y;

		if (FAILED(m_pGameInstance->Render_Font(TEXT("Font30"), m_strLocationName.c_str(), _float2((vCalPos.x), (vCalPos.y + 25.f)), XMVectorSet(0.f, 0.f, 0.f, 1.f))))
			return E_FAIL;
		
		vTextPos = _float2(282.9f, -200.f);
		m_strLocationName = TEXT("트라르그 산");

		vCalPos.x = vMidPoint.x + vTextPos.x;
		vCalPos.y = vMidPoint.y - vTextPos.y;

		if (FAILED(m_pGameInstance->Render_Font(TEXT("Font24"), m_strLocationName.c_str(), _float2((vCalPos.x), (vCalPos.y + 25.f)), XMVectorSet(0.f, 0.f, 0.f, 1.f))))
			return E_FAIL;
	}
	break;

	}

	return S_OK;
}

HRESULT CWorldMapNPC::DisplayHoneyBee(_float2 _MidPoint)
{
	_float2 vText = { 0.f, 0.f };
	_float2 vCal = { 0.f, 0.f };

	vText = _float2(-507.f, -206.f);
	m_strLocationName = TEXT("먹물숲");

	vCal.x = _MidPoint.x + vText.x;
	vCal.y = _MidPoint.y - vText.y;

	if (FAILED(m_pGameInstance->Render_Font(TEXT("Font18"), m_strLocationName.c_str(), _float2((vCal.x), (vCal.y + 25.f)), XMVectorSet(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	vText = _float2(-474.f, -66.f);
	m_strLocationName = TEXT("벌꿀 봉우리");

	vCal.x = _MidPoint.x + vText.x;
	vCal.y = _MidPoint.y - vText.y;

	if (FAILED(m_pGameInstance->Render_Font(TEXT("Font18"), m_strLocationName.c_str(), _float2((vCal.x), (vCal.y + 25.f)), XMVectorSet(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	vText = _float2(-560.f, 175.9f);
	m_strLocationName = TEXT("책의 탑");

	vCal.x = _MidPoint.x + vText.x;
	vCal.y = _MidPoint.y - vText.y;

	if (FAILED(m_pGameInstance->Render_Font(TEXT("Font18"), m_strLocationName.c_str(), _float2((vCal.x), (vCal.y + 25.f)), XMVectorSet(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	return S_OK;
}

CWorldMapNPC* CWorldMapNPC::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CWorldMapNPC* pInstance = new CWorldMapNPC(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Created CWorldMapNPC Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject* CWorldMapNPC::Clone(void* _pArg)
{
	CWorldMapNPC* pInstance = new CWorldMapNPC(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Clone CWorldMapNPC Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CWorldMapNPC::Free()
{
	
	__super::Free();
	
}

HRESULT CWorldMapNPC::Cleanup_DeadReferences()
{

	return S_OK;
}
