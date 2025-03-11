#include "stdafx.h"
#include "FatherGame.h"
#include "GameInstance.h"
#include "PortalLocker.h"
#include "Section_Manager.h"
#include "Section_2D_PlayMap.h"
#include "Portal_Default.h"
#include "ZetPack_Child.h"
#include "Simple_UI.h"
#include "Mug_Alien.h"
#include "JellyKing.h"
#include "ZetPack_Father.h"
#include "Mat.h"
#include "PlayerData_Manager.h"
#include "PlayerItem.h"

/* Progress */
#include "FatherGame_Progress_Start.h"
#include "FatherGame_Progress_ZetPack.h"
#include "FatherGame_Progress_PartBody.h"
#include "FatherGame_Progress_PartWing.h"
#include "FatherGame_Progress_PartHead.h"
#include "FatherGame_Progress_MakeFather.h"

IMPLEMENT_SINGLETON(CFatherGame)
CFatherGame::CFatherGame()
{
}

HRESULT CFatherGame::Start_Game(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	/* 1. 초기화 */
	m_pDevice = _pDevice;
	Safe_AddRef(m_pDevice);
	m_pContext = _pContext;
	Safe_AddRef(m_pContext);
	m_pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(m_pGameInstance);

	m_Progress.reserve((size_t)FATHER_PROGRESS::FATHER_PROGRESS_LAST);

	/* 2. Progress 생성 후 Vector에 저장. */

	
	{/* Progress Start */
		CFatherGame_Progress_Start::FATHERGAME_PROGRESS_START_DESC ProgressDesc = {};
		ProgressDesc.iNumMonsters = 3;
		ProgressDesc.strMonsterPrototypeTag = TEXT("Prototype_GameObject_Goblin");
		CFatherGame_Progress* pProgress = CFatherGame_Progress_Start::Create(m_pDevice, m_pContext, &ProgressDesc);
		if (nullptr == pProgress)
			return E_FAIL;
		pProgress->Set_Active(true);
		m_Progress.push_back(pProgress); // 여기가 원본임. AddRef x
		m_ProgressClear.push_back(false);
	}/* Progress Start */

	
	{/* Progress ZetPack */
		CFatherGame_Progress_ZetPack::FATHERGAME_PROGRESS_ZETPACK_DESC ProgressDesc = {};
		CFatherGame_Progress* pProgress = CFatherGame_Progress_ZetPack::Create(m_pDevice, m_pContext, &ProgressDesc);
		if (nullptr == pProgress)
			return E_FAIL;
		pProgress->Set_Active(false);
		m_Progress.push_back(pProgress); // 여기가 원본임. AddRef x
		m_ProgressClear.push_back(false);
	}/* Progress ZetPack */

	{/* Progress PartBody */
		CFatherGame_Progress_PartBody::FATHERGAME_PROGRESS_PARTBODY_DESC ProgressDesc = {};
		CFatherGame_Progress* pProgress = CFatherGame_Progress_PartBody::Create(m_pDevice, m_pContext, &ProgressDesc);
		if (nullptr == pProgress)
			return E_FAIL;
		pProgress->Set_Active(false);
		m_Progress.push_back(pProgress); // 여기가 원본임. AddRef x
		m_ProgressClear.push_back(false);
	}/* Progress PartBody */

	{/* Progress PartWing */
		CFatherGame_Progress_PartWing::FATHERGAME_PROGRESS_PARTWING_DESC ProgressDesc = {};
		CFatherGame_Progress* pProgress = CFatherGame_Progress_PartWing::Create(m_pDevice, m_pContext, &ProgressDesc);
		if (nullptr == pProgress)
			return E_FAIL;
		pProgress->Set_Active(false);
		m_Progress.push_back(pProgress); // 여기가 원본임. AddRef x
		m_ProgressClear.push_back(false);
	}/* Progress PartWing */

	{/* Progress PartHead */
		CFatherGame_Progress_PartHead::FATHERGAME_PROGRESS_PARTHEAD_DESC ProgressDesc = {};
		CFatherGame_Progress* pProgress = CFatherGame_Progress_PartHead::Create(m_pDevice, m_pContext, &ProgressDesc);
		if (nullptr == pProgress)
			return E_FAIL;
		pProgress->Set_Active(false);
		m_Progress.push_back(pProgress); // 여기가 원본임. AddRef x
		m_ProgressClear.push_back(false);
	}/* Progress PartHead */


	{/* Progress MakeFather */
		CFatherGame_Progress_MakeFather::FATHERGAME_PROGRESS_MAKEFATHER_DESC ProgressDesc = {};
		CFatherGame_Progress* pProgress = CFatherGame_Progress_MakeFather::Create(m_pDevice, m_pContext, &ProgressDesc);
		if (nullptr == pProgress)
			return E_FAIL;
		pProgress->Set_Active(false);
		m_Progress.push_back(pProgress); // 여기가 원본임. AddRef x
		m_ProgressClear.push_back(false);
	}/* Progress MakeFather */
	
	/* 2. PortalDefender 3곳에 생성 */
	m_PortalLockers.resize((size_t)LOCKER_LAST);

	{/* PortalLocker ZetPack */
		CGameObject* pGameObject = nullptr;
		CPortalLocker::PORTALLOCKER_DESC PortalLockerDesc;
		CPortal* pTargetPortal = static_cast<CPortal_Default*>(static_cast<CSection_2D_PlayMap*>(CSection_Manager::GetInstance()->Find_Section(TEXT("Chapter6_SKSP_01")))->Get_Portal(0));
		if (nullptr == pTargetPortal)
			return E_FAIL;
		PortalLockerDesc.pTargetPortal = pTargetPortal;
		PortalLockerDesc.ePortalLockerType = CPortalLocker::TYPE_PURPLE;
		PortalLockerDesc.strSectionKey = TEXT("Chapter6_SKSP_01");

		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_PortalLocker"), LEVEL_CHAPTER_6, TEXT("Layer_PortalLocker"), &pGameObject, &PortalLockerDesc)))
			return E_FAIL;

		m_PortalLockers[LOCKER_ZETPACK] = static_cast<CPortalLocker*>(pGameObject);
		Safe_AddRef(m_PortalLockers[LOCKER_ZETPACK]);
		if (FAILED(CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(PortalLockerDesc.strSectionKey, pGameObject, SECTION_2D_PLAYMAP_OBJECT)))
			return E_FAIL;		
	}/* PortalLocker ZetPack */


	{/* PortalLocker PartHead */
		CGameObject* pGameObject = nullptr;
		CPortalLocker::PORTALLOCKER_DESC PortalLockerDesc;
		CPortal* pTargetPortal = static_cast<CPortal_Default*>(static_cast<CSection_2D_PlayMap*>(CSection_Manager::GetInstance()->Find_Section(TEXT("Chapter6_SKSP_04")))->Get_Portal(0));
		if (nullptr == pTargetPortal)
			return E_FAIL;
		PortalLockerDesc.pTargetPortal = pTargetPortal;
		PortalLockerDesc.ePortalLockerType = CPortalLocker::TYPE_YELLOW;
		PortalLockerDesc.strSectionKey = TEXT("Chapter6_SKSP_04");
	
		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_PortalLocker"), LEVEL_CHAPTER_6, TEXT("Layer_PortalLocker"), &pGameObject, &PortalLockerDesc)))
			return E_FAIL;
	
		m_PortalLockers[LOCKER_PARTHEAD] = static_cast<CPortalLocker*>(pGameObject);
		Safe_AddRef(m_PortalLockers[LOCKER_PARTHEAD]);
	
		if (FAILED(CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(PortalLockerDesc.strSectionKey, pGameObject, SECTION_2D_PLAYMAP_OBJECT)))
			return E_FAIL;
	}/* PortalLocker PartHead */

	/* FatherParts Condition */
	m_FatherPartsCondition.resize((size_t)FATHER_PART::FATHER_LAST);
	for (_uint i = 0; i < (_uint)FATHER_PART::FATHER_LAST; ++i)
		m_FatherPartsCondition[i] = false;

	m_FatherParts_UIs.resize((size_t)FATHER_PART::FATHER_LAST);
	/* FatherParts UI */
	{
		
		CGameObject* pGameObject = nullptr;
		CSimple_UI::SIMPLE_UI_DESC SimpleUI_Desc{};
		_float fY = g_iWinSizeX * 0.05f;
		_float fSizeX = 48.f;
		_float fSizeY = 48.f;
		_float fInterval = 10.f;
		_float fStartPosX = g_iWinSizeX * 0.5f - fSizeX - fInterval;
		_float fTextureAspect = 112.f / 116.f;
		SimpleUI_Desc.vUIInfo = { fStartPosX, fY , fSizeX * fTextureAspect, fSizeY };
		SimpleUI_Desc.strTexturePrototypeTag = TEXT("Prototype_Component_Texture_FatherParts_UI");
		SimpleUI_Desc.ePassIndex = PASS_VTXPOSTEX::DEFAULT;
		SimpleUI_Desc.iCurLevelID = LEVEL_CHAPTER_6;
		SimpleUI_Desc.iTextureIndex = FATHER_PART::FATHER_BODY * 2;
		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_Simple_UI"), LEVEL_CHAPTER_6, TEXT("Layer_FatherParts_UI"), &pGameObject, &SimpleUI_Desc)))
			return E_FAIL;
		Safe_AddRef(pGameObject);
		m_FatherParts_UIs[(_uint)FATHER_PART::FATHER_BODY] = static_cast<CSimple_UI*>(pGameObject);
		m_FatherParts_UIs[(_uint)FATHER_PART::FATHER_BODY]->Set_Active(false);
		pGameObject = nullptr;
		fTextureAspect = 88.f / 136.f;
		SimpleUI_Desc.vUIInfo = { fStartPosX + 1 * (fSizeX + fInterval), fY , fSizeX * fTextureAspect , fSizeY };
		SimpleUI_Desc.iTextureIndex = FATHER_PART::FATHER_WING * 2;
		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_Simple_UI"), LEVEL_CHAPTER_6, TEXT("Layer_FatherParts_UI"), &pGameObject, &SimpleUI_Desc)))
			return E_FAIL;
		Safe_AddRef(pGameObject);
		m_FatherParts_UIs[(_uint)FATHER_PART::FATHER_WING] = static_cast<CSimple_UI*>(pGameObject);
		m_FatherParts_UIs[(_uint)FATHER_PART::FATHER_WING]->Set_Active(false);

		pGameObject = nullptr;
		fTextureAspect = 120.f / 120.f;
		SimpleUI_Desc.vUIInfo = { fStartPosX + 2 * (fSizeX + fInterval), fY , fSizeX * fTextureAspect, fSizeY };
		SimpleUI_Desc.iTextureIndex = FATHER_PART::FATER_HEAD * 2;
		if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_STATIC, TEXT("Prototype_GameObject_Simple_UI"), LEVEL_CHAPTER_6, TEXT("Layer_FatherParts_UI"), &pGameObject, &SimpleUI_Desc)))
			return E_FAIL;
		Safe_AddRef(pGameObject);
		m_FatherParts_UIs[(_uint)FATHER_PART::FATER_HEAD] = static_cast<CSimple_UI*>(pGameObject);
		m_FatherParts_UIs[(_uint)FATHER_PART::FATER_HEAD]->Set_Active(false);
	}

	{ /* Mat */
		CMat::MODELOBJECT_DESC Desc;
		Desc.Build_3D_Transform(_float3(2.11f, 5.87f, -0.39f), _float3(0.61f, 1.0f, 0.61f));
		Desc.iCurLevelID = LEVEL_CHAPTER_6;

		CGameObject* pGameObject = nullptr;
		if(FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_CHAPTER_6, TEXT("Prototype_GameObject_Mat"), LEVEL_CHAPTER_6, TEXT("Layer_Mat"), &pGameObject, &Desc)))
			return E_FAIL;
		m_pMat = static_cast<CMat*>(pGameObject);
		Safe_AddRef(m_pMat);
	}/* Mat */

	{ /* Item StopStamp */
		if(FAILED(CPlayerData_Manager::GetInstance()->Spawn_PlayerItem(LEVEL_STATIC, (LEVEL_ID)LEVEL_CHAPTER_6, TEXT("Stop_Stamp"), _float3(2.11f, 3.87f, -0.39f))))
			return E_FAIL;

	}/* Item StopStamp */
	m_eGameState = GAME_PLAYING;

	return S_OK;
}

void CFatherGame::Update()
{
	if (GAME_PLAYING != m_eGameState)
		return;

	if (KEY_DOWN(KEY::F))
	{
		OpenPortalLocker(PORTALLOCKER::LOCKER_PARTHEAD);
	}
	if (KEY_DOWN(KEY::U))
	{
		Start_StopStampMoveWork();
	}

	for (_uint i = 0; i < m_Progress.size(); ++i)
	{
		/* Update */
		if(true == m_Progress[i]->Is_Active()) 
		{
			m_Progress[i]->Progress_Update();

			/* Clear Check */
			if (true == m_Progress[i]->Is_Clear())
			{
				/* True로 기록 , Active는 내부적으로 Progress가 관리. */
				m_ProgressClear[i] = true;
			}
		}
	}

	/* Game End Check */
	m_iClearCount = 0;
	for (_uint i = 0; i < m_Progress.size(); ++i)
	{
		if (true == m_ProgressClear[i] && false == m_Progress[i]->Is_Active())
		{
			++m_iClearCount;
		}

	}

	if (m_Progress.size() == m_iClearCount)
	{
		End_Game();
	}

}

HRESULT CFatherGame::End_Game()
{
	m_eGameState = GAME_END;

	Safe_Release(m_pMat);
	m_pMat = nullptr;
	Safe_Release(m_pMugAlien);
	m_pMugAlien = nullptr;
	Safe_Release(m_pZetPack_Child);
	m_pZetPack_Child = nullptr;
	Safe_Release(m_pJellyKing);
	m_pJellyKing = nullptr;
	Safe_Release(m_pZetPack_Father);
	m_pZetPack_Father = nullptr;

	for (auto& pPortalLocker : m_PortalLockers)
	{
		Safe_Release(pPortalLocker);
	}
	m_PortalLockers.clear();

	for (_uint i = 0; i < (_uint)FATHER_PART::FATHER_LAST; ++i)
	{
		Safe_Release(m_FatherParts_UIs[i]);
		Event_DeleteObject(m_FatherParts_UIs[i]);
	}
	m_FatherParts_UIs.clear();

	for (auto& pProgress : m_Progress)
		Safe_Release(pProgress);
	m_Progress.clear();

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);

#ifdef _DEBUG
	cout << "FatherGame End" << endl;
#endif // _DEBUG

	return S_OK;
}

void CFatherGame::DeadCheck_ReferenceObject()
{
	for (_uint i = 0; i < PORTALLOCKER::LOCKER_LAST; ++i)
	{
		if (nullptr == m_PortalLockers[i])
			continue;

		if (true == m_PortalLockers[i]->Is_Dead())
		{
			Safe_Release(m_PortalLockers[i]);
			m_PortalLockers[i] = nullptr;
		}
	}
}

void CFatherGame::Start_Progress(FATHER_PROGRESS _eStartProgress)
{
	if (true == m_ProgressClear[_eStartProgress])
		assert(nullptr);
	if (nullptr == m_Progress[_eStartProgress])
		assert(nullptr);

	Event_SetActive(m_Progress[_eStartProgress], true);
}

void CFatherGame::OpenPortalLocker(PORTALLOCKER _ePortalLockerIndex)
{
	if (PORTALLOCKER::LOCKER_LAST <= _ePortalLockerIndex)
		return;

	if (nullptr == m_PortalLockers[_ePortalLockerIndex])
		return;

	m_PortalLockers[_ePortalLockerIndex]->Open_Locker();
	Safe_Release(m_PortalLockers[_ePortalLockerIndex]);
	m_PortalLockers[_ePortalLockerIndex] = nullptr;
}

void CFatherGame::Start_StopStampMoveWork()
{
	m_pMat->Get_ActorCom()->Add_Impulse(_float3(0.0f, 20.0f, 0.0f));
	m_pMat->Get_ActorCom()->Set_AngularVelocity(_float3(10.f, 0.0f, 0.0f));

	CPlayerItem* pPlayerItem = CPlayerData_Manager::GetInstance()->Get_PlayerItem_Ptr(TEXT("Stop_Stamp"));
	pPlayerItem->Get_ActorCom()->Add_Impulse(_float3(0.0f, 20.0f, 5.0f));
}

void CFatherGame::Set_ZetPack_Child(CZetPack_Child* _pZetPack_Child)
{
	assert(_pZetPack_Child);
	assert(!m_pZetPack_Child);
	m_pZetPack_Child = _pZetPack_Child;
	Safe_AddRef(m_pZetPack_Child);
}
void CFatherGame::Set_Mug_Alien(CMug_Alien* _pMugAlien)
{
	assert(_pMugAlien);
	assert(!m_pMugAlien);
	m_pMugAlien = _pMugAlien;
	Safe_AddRef(m_pMugAlien);
}

void CFatherGame::Set_JellyKing(CJellyKing* _pJellyKing)
{
	assert(_pJellyKing);
	assert(!m_pJellyKing);
	m_pJellyKing = _pJellyKing;
	Safe_AddRef(m_pJellyKing);
}

void CFatherGame::Set_ZetPack_Father(CZetPack_Father* _pZetPack_Father)
{
	assert(_pZetPack_Father);
	assert(!m_pZetPack_Father);
	m_pZetPack_Father = _pZetPack_Father;
	Safe_AddRef(m_pZetPack_Father);
}

void CFatherGame::Set_Active_FatherParts_UIs(_bool _isActive)
{
	if (false == m_ProgressClear[FATHER_PROGRESS_START])
		return;

	for (_uint i = 0; i < FATHER_PART::FATHER_LAST; ++i)
	{
		Event_SetActive(m_FatherParts_UIs[i], _isActive);
	}
}

void CFatherGame::Pickup_FatherPart(FATHER_PART _eFatherPart)
{
	if (true == m_FatherPartsCondition[_eFatherPart])
		return;
	m_FatherPartsCondition[_eFatherPart] = true;
	m_FatherParts_UIs[_eFatherPart]->Set_TextureIndex((_uint)_eFatherPart * 2 + 1);
}

_uint CFatherGame::Check_FatherPartsCondition_Count()
{
	_uint iCount = 0;
	for (_bool isCondition : m_FatherPartsCondition)
	{
		if (true == isCondition)
			++iCount;
	}
	return iCount;
}

_bool CFatherGame::Check_FatherPartsCondition(CFatherGame::FATHER_PART _ePartIndex)
{
	return m_FatherPartsCondition[_ePartIndex];
}

void CFatherGame::Free()
{
	for (auto& pPortalLocker : m_PortalLockers)
	{
		Safe_Release(pPortalLocker);
	}
	m_PortalLockers.clear();

	if (GAME_END != m_eGameState)
	{
		/* 정상적으로 게임 엔드가 호출되지 않은 경우에만 릴리즈 */
		Safe_Release(m_pMat);
		Safe_Release(m_pMugAlien);
		Safe_Release(m_pZetPack_Child);
		Safe_Release(m_pJellyKing);

		for (_uint i = 0; i < (_uint)FATHER_PART::FATHER_LAST; ++i)
		{
			Safe_Release(m_FatherParts_UIs[i]);
		}
		m_FatherParts_UIs.clear();

		for (auto& pProgress : m_Progress)
			Safe_Release(pProgress);
		m_Progress.clear();

		Safe_Release(m_pGameInstance);
		Safe_Release(m_pContext);
		Safe_Release(m_pDevice);
	}


	__super::Free();
}
