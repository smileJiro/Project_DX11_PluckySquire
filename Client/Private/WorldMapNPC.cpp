#include "stdafx.h"
#include "WorldMapNPC.h"
#include "WorldMapNpc_Jot.h"
#include "WorldMapNpc_Thrash.h"
#include "WorldMapNpc_Violet.h"
#include "GameInstance.h"
#include "Section_Manager.h"
#include "Camera_Manager.h"




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
		m_iStartIndex = 0;
	}
	break;

	case LEVEL_CHAPTER_4:
	{
		CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(TEXT("Chapter4_Worldmap_0"), this);
		m_iStartIndex = 1;
	}
		break;

	case LEVEL_CHAPTER_6:
	{
		CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(TEXT("Chapter6_Worldmap_0"), this);
		m_iStartIndex = 3;
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
	__super::Update(_fTimeDelta);
	Progress(_fTimeDelta);

}

void CWorldMapNPC::Late_Update(_float _fTimeDelta)
{
	__super::Late_Update(_fTimeDelta);
}

HRESULT CWorldMapNPC::Render()
{
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

void CWorldMapNPC::Progress(_float _fTimeDelta)
{
	if (4 == m_iStartIndex)
	{
		CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(TEXT("Chapter6_Worldmap_01"), this);
	}

	CSection_Manager* pSectionManager = CSection_Manager::GetInstance();

	if (TEXT("Chapter2_Worldmap_0") != pSectionManager->Get_Cur_Section_Key() &&
		TEXT("Chapter4_Worldmap_0") != pSectionManager->Get_Cur_Section_Key() &&
		TEXT("Chapter6_Worldmap_0") != pSectionManager->Get_Cur_Section_Key() &&
		TEXT("Chapter6_Worldmap_01") != pSectionManager->Get_Cur_Section_Key())
	{
		return;
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
			//CCamera_Manager::GetInstance()->Change_CameraTarget(this);
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
				return;
			}

			m_vStartPos = m_PosMoves[m_iStartIndex];
			m_vArrivePos = m_PosMoves[m_iStartIndex + 1];
			
			m_isUpdatePos = true;
		}

		if (false == m_isWalkAnim)
		{
			if (0 == m_iStartIndex)
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
			if (1 == m_iStartIndex || 4 == m_iStartIndex)
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

		if (2.5f > m_fWaitTime)
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
			++m_iStartIndex;

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
				m_PartObjects[NPC_JOT]->Get_ControllerTransform()->Set_State(CTransform::STATE_POSITION, XMVectorSet(40.f, 0.f, 0.f, 1.f));
			}
		}
	}
}
 
void CWorldMapNPC::Pos_Ready()
{
	m_PosMoves[0] = _float2(-303.0f, -90.0f);
	m_PosMoves[1] = _float2(-450.f, 13.53f);
	m_PosMoves[2] = _float2(-354.f, 86.19f);
	m_PosMoves[3] = _float2(-110.f, -91.3f);
	m_PosMoves[4] = _float2(2.76f, -71.3f);
	m_PosMoves[5] = _float2(227.6f, -162.f);
}

void CWorldMapNPC::Change_BookOrder()
{
	//CSection* strCursection = SECTION_MGR->Find_Section(SECTION_MGR->Get_Cur_Section_Key());
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

	if (0 == m_iStartIndex)
		ChangPlayerPos = _float2(-687.f, 182.4f);
	else if (1 == m_iStartIndex)
		ChangPlayerPos = _float2(-1968.f, -685.f);
	else if (3 == m_iStartIndex)
		ChangPlayerPos = _float2(-14.3f, -664.f);
	else if (4 == m_iStartIndex)
		ChangPlayerPos = _float2(-1195.f, -160.f);

	return ChangPlayerPos;
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
