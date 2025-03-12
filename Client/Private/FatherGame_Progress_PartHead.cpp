#include "stdafx.h"
#include "FatherGame_Progress_PartHead.h"
#include "GameInstance.h"
#include "FatherGame.h"
#include "Section_Manager.h"
#include "Section_2D_PlayMap.h"

#include "Portal_Default.h"
#include "Player.h"
#include "ZetPack_Child.h"
#include "MiniGame_Defender.h"
#include "DefenderPlayer.h"


CFatherGame_Progress_PartHead::CFatherGame_Progress_PartHead(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CFatherGame_Progress(_pDevice, _pContext)
{
}

HRESULT CFatherGame_Progress_PartHead::Initialize(void* _pArg)
{
    FATHERGAME_PROGRESS_PARTHEAD_DESC* pDesc = static_cast<CFatherGame_Progress_PartHead::FATHERGAME_PROGRESS_PARTHEAD_DESC*>(_pArg);

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CFatherGame_Progress_PartHead::Progress_Enter()
{
    if (true == m_isClearProgress)
    {
        MSG_BOX("이미 클리어 된 프로그레스입니다.");
        return E_FAIL;
    }

    
    {/* Create MiniGameDefender */

        CSection_Manager* pSectionMgr = CSection_Manager::GetInstance();

        CMiniGame_Defender::DEFENDER_DESC tDesc = {};
        tDesc.iCurLevelID = LEVEL_CHAPTER_6;
        tDesc.tTransform2DDesc.vInitialPosition = { -2020.f, -80.f, 0.f };   // TODO ::임시 위치
        m_pMiniGameDefender = static_cast<CMiniGame_Defender*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_CHAPTER_6, TEXT("Prototype_GameObject_Minigame_Defender"), &tDesc));;
        m_pGameInstance->Add_GameObject_ToLayer(LEVEL_CHAPTER_6, TEXT("Layer_Defender"), m_pMiniGameDefender);
        pSectionMgr->Add_GameObject_ToSectionLayer(TEXT("Chapter6_SKSP_04"), m_pMiniGameDefender, SECTION_2D_PLAYMAP_OBJECT);

        CDefenderPlayer::DEFENDERPLAYER_DESC tDeffenderPlayerDesc = {};
        tDeffenderPlayerDesc.iCurLevelID = LEVEL_CHAPTER_6;
        tDeffenderPlayerDesc.pMinigame = m_pMiniGameDefender;
        CDefenderPlayer* pPlayer = static_cast<CDefenderPlayer*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_CHAPTER_6, TEXT("Prototype_GameObject_DefenderPlayer"), &tDeffenderPlayerDesc));
        m_pGameInstance->Add_GameObject_ToLayer(LEVEL_CHAPTER_6, TEXT("Layer_Defender"), pPlayer);
        pSectionMgr->Add_GameObject_ToSectionLayer(TEXT("Chapter6_SKSP_04"), pPlayer, SECTION_2D_PLAYMAP_OBJECT);
        pPlayer->Set_Active(false);


        m_pMiniGameDefender;

    }/* Create MiniGameDefender */

#ifdef _DEBUG
    cout << "FatherGame Progress_PartHead Start" << endl;
#endif // _DEBUG

    return S_OK;
}

void CFatherGame_Progress_PartHead::Progress_Update()
{
    if (true == m_isClearProgress)
        return;

    if (KEY_DOWN(KEY::K))
    {
        // Test Code : 머리 먹은걸로 치기
        CFatherGame::GetInstance()->Pickup_FatherPart(CFatherGame::FATER_HEAD);
    }

    Progress_Clear();

}

HRESULT CFatherGame_Progress_PartHead::Progress_Clear()
{
    
    CZetPack_Child* pZetPack_Child = CFatherGame::GetInstance()->Get_ZetPack_Child();
    if (CFatherGame::GetInstance()->Check_FatherPartsCondition(CFatherGame::FATER_HEAD) && CZetPack_Child::STATE_PORTALOUT == pZetPack_Child->Get_CurState())
    {
        m_isClearProgress = true;

        /* 1. Claer 이벤트 호출 >>> 현재 보유중인 아빠 파츠의 개수에 따라 다른 이벤트 호출. */
        Event_Register_Trigger(TEXT("Chapter6_FatherGame_Progress_Fatherpart_3"));
        CFatherGame::GetInstance()->Start_Progress(CFatherGame::FATHER_PROGRESS_MAKEFATHER);
        /* 3. 자기 자신의 Active 상태를 False로 변경 */
        Event_SetActive(this, false);
#ifdef _DEBUG
        cout << "FatherGame Progress_PartHead Clear" << endl;
#endif // _DEBUG
    }

    return S_OK;
}

HRESULT CFatherGame_Progress_PartHead::Progress_Exit()
{
    if (false == m_isClearProgress)
        return S_OK;

    {/* Clear 후 Exit 호출이 정상적인 로직으로 판단 함. */

    }
#ifdef _DEBUG
    cout << "FatherGame Progress_PartWing Exit" << endl;
#endif // _DEBUG
    return S_OK;
}

CFatherGame_Progress_PartHead* CFatherGame_Progress_PartHead::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, void* _pArg)
{
    CFatherGame_Progress_PartHead* pInstance = new CFatherGame_Progress_PartHead(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed Create : CFatherGame_Progress_PartHead");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CFatherGame_Progress_PartHead::Free()
{
    Safe_Release(m_pMiniGameDefender);

    __super::Free();
}