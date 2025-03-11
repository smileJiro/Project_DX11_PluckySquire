#include "stdafx.h"
#include "FatherGame_Progress_PartWing.h"
#include "GameInstance.h"
#include "FatherGame.h"
#include "Section_Manager.h"
#include "Section_2D_PlayMap.h"

#include "Portal_Default.h"
#include "Player.h"
#include "ZetPack_Child.h"
#include "JellyKing.h"

CFatherGame_Progress_PartWing::CFatherGame_Progress_PartWing(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CFatherGame_Progress(_pDevice, _pContext)
{
}

HRESULT CFatherGame_Progress_PartWing::Initialize(void* _pArg)
{
    FATHERGAME_PROGRESS_PARTWING_DESC* pDesc = static_cast<CFatherGame_Progress_PartWing::FATHERGAME_PROGRESS_PARTWING_DESC*>(_pArg);

    
    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;



    return S_OK;
}

HRESULT CFatherGame_Progress_PartWing::Progress_Enter()
{
    if (true == m_isClearProgress)
    {
        MSG_BOX("이미 클리어 된 프로그레스입니다.");
        return E_FAIL;
    }

    /* Create JellyKing */
    {
        CJellyKing::JELLYKING_DESC Desc = {};
        Desc.iCurLevelID = LEVEL_CHAPTER_6;
        Desc.Build_2D_Transform(_float2(314.f, -54.f), _float2(1.0f, 1.0f));
        CGameObject* pGameObject = nullptr;
        if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_CHAPTER_6, TEXT("Prototype_GameObject_JellyKing"), LEVEL_CHAPTER_6, TEXT("Layer_JellyKing"), &pGameObject, &Desc)))
            return E_FAIL;
        m_pJellyKing = static_cast<CJellyKing*>(pGameObject);
        Safe_AddRef(m_pJellyKing);

        if (FAILED(CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(TEXT("Chapter6_SKSP_06"), pGameObject, SECTION_2D_PLAYMAP_OBJECT)))
            return E_FAIL;
    }


    //CFatherGame::GetInstance()->Set_Mug_Alien(m_pMugAlien);
#ifdef _DEBUG
    cout << "FatherGame Progress_PartWing Start" << endl;
#endif // _DEBUG

    return S_OK;
}

void CFatherGame_Progress_PartWing::Progress_Update()
{
    if (true == m_isClearProgress)
        return;

    Progress_Clear();

}

HRESULT CFatherGame_Progress_PartWing::Progress_Clear()
{
    /* 1. 찌리의 대화 가 종료되었고,  */
    CZetPack_Child* pZetPack_Child = CFatherGame::GetInstance()->Get_ZetPack_Child();
    if (1 < m_pJellyKing->Get_DialogueIndex() && CFatherGame::GetInstance()->Check_FatherPartsCondition(CFatherGame::FATHER_WING) && CZetPack_Child::STATE_PORTALOUT == pZetPack_Child->Get_CurState())
    {
        m_isClearProgress = true;

        /* 1. Claer 이벤트 호출 >>> 현재 보유중인 아빠 파츠의 개수에 따라 다른 이벤트 호출. */
        _uint iConditionCount = CFatherGame::GetInstance()->Check_FatherPartsCondition_Count();
        switch (iConditionCount)
        {
        case 0:
            assert(nullptr);
            break;
        case 1:
            Event_Register_Trigger(TEXT("Chapter6_FatherGame_Progress_Fatherpart_1"));
            break;
        case 2:
            Event_Register_Trigger(TEXT("Chapter6_FatherGame_Progress_Fatherpart_2"));
            CFatherGame::GetInstance()->Start_Progress(CFatherGame::FATHER_PROGRESS_PARTHEAD);
            break;
        case 3:
            assert(nullptr);
            break;
        default:
            break;
        }

        /* 3. 자기 자신의 Active 상태를 False로 변경 */
        Event_SetActive(this, false);
#ifdef _DEBUG
        cout << "FatherGame Progress_PartWing Clear" << endl;
#endif // _DEBUG
    }

    return S_OK;
}

HRESULT CFatherGame_Progress_PartWing::Progress_Exit()
{
    if (false == m_isClearProgress)
        return S_OK;

    /* Clear 후 Exit 호출이 정상적인 로직으로 판단 함. */
    {

    }
#ifdef _DEBUG
    cout << "FatherGame Progress_PartWing Exit" << endl;
#endif // _DEBUG
    return S_OK;
}

CFatherGame_Progress_PartWing* CFatherGame_Progress_PartWing::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, void* _pArg)
{
    CFatherGame_Progress_PartWing* pInstance = new CFatherGame_Progress_PartWing(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed Create : CFatherGame_Progress_PartWing");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CFatherGame_Progress_PartWing::Free()
{
    Safe_Release(m_pJellyKing);

    __super::Free();
}