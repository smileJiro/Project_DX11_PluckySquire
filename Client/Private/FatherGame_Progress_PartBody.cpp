#include "stdafx.h"
#include "FatherGame_Progress_PartBody.h"
#include "GameInstance.h"
#include "FatherGame.h"
#include "Section_Manager.h"
#include "Section_2D_PlayMap.h"

#include "Portal_Default.h"
#include "Mug_Alien.h"
#include "Player.h"
#include "ZetPack_Child.h"

CFatherGame_Progress_PartBody::CFatherGame_Progress_PartBody(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CFatherGame_Progress(_pDevice, _pContext)
{
}

HRESULT CFatherGame_Progress_PartBody::Initialize(void* _pArg)
{
    FATHERGAME_PROGRESS_PARTBODY_DESC* pDesc = static_cast<CFatherGame_Progress_PartBody::FATHERGAME_PROGRESS_PARTBODY_DESC*>(_pArg);

    
    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

   

    return S_OK;
}

HRESULT CFatherGame_Progress_PartBody::Progress_Enter()
{
    if (true == m_isClearProgress)
    {
        MSG_BOX("이미 클리어 된 프로그레스입니다.");
        return E_FAIL;
    }

    /* Create Mug_Alien */
    CMug_Alien::MUG_ALIEN_DESC MugDesc = {};
    MugDesc.iCurLevelID = LEVEL_CHAPTER_6;
    MugDesc.Build_2D_Transform(_float2(-225.f, -100.f), _float2(1.0f, 1.0f));
    CGameObject* pGameObject = nullptr;
    if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_CHAPTER_6, TEXT("Prototype_GameObject_Mug_Alien"), LEVEL_CHAPTER_6, TEXT("Layer_Mug_Alien"), &pGameObject, &MugDesc)))
        return E_FAIL;
    m_pMugAlien = static_cast<CMug_Alien*>(pGameObject);
    Safe_AddRef(m_pMugAlien);
    
    if (FAILED(CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(TEXT("Chapter6_SKSP_03"), pGameObject, SECTION_2D_PLAYMAP_OBJECT)))
        return E_FAIL;


    CFatherGame::GetInstance()->Set_Mug_Alien(m_pMugAlien);
#ifdef _DEBUG
    cout << "FatherGame Progress_ZetPack Start" << endl;
#endif // _DEBUG

    return S_OK;
}

void CFatherGame_Progress_PartBody::Progress_Update()
{
    if (true == m_isClearProgress)
        return;

    Progress_Clear();

}

HRESULT CFatherGame_Progress_PartBody::Progress_Clear()
{
    /* 1. 찌리의 대화 가 종료되었고,  */
    CZetPack_Child* pZetPack_Child = CFatherGame::GetInstance()->Get_ZetPack_Child();
    if (0 < m_pMugAlien->Get_DialogueIndex() && CZetPack_Child::STATE_PORTALOUT == pZetPack_Child->Get_CurState())
    {
        m_isClearProgress = true;

        /* 1. Claer 이벤트 호출 >>> 현재 보유중인 아빠 파츠의 개수에 따라 다른 이벤트 호출. */
        if (true)
        {
            Event_Register_Trigger(TEXT("Chapter6_FatherGame_Progress_PartHead"));
        }
        else /* 2. 그게 아니라면, 일반 이벤트 호출. */
        {
            Event_Register_Trigger(TEXT("Chapter6_FatherGame_Progress_PartBody_Clear"));
        }

        
        /* 2. 아빠부품 ui on */
        CFatherGame::GetInstance()->Set_Active_FatherParts_UIs(true);
        /* 3. 자기 자신의 Active 상태를 False로 변경 */
        Event_SetActive(this, false);
#ifdef _DEBUG
        cout << "FatherGame Progress_ZetPack Clear" << endl;
#endif // _DEBUG
    }

    return S_OK;
}

HRESULT CFatherGame_Progress_PartBody::Progress_Exit()
{
    if (false == m_isClearProgress)
        return S_OK;

    /* Clear 후 Exit 호출이 정상적인 로직으로 판단 함. */
    {

    }
#ifdef _DEBUG
    cout << "FatherGame Progress_ZetPack Exit" << endl;
#endif // _DEBUG
    return S_OK;
}

CFatherGame_Progress_PartBody* CFatherGame_Progress_PartBody::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, void* _pArg)
{
    CFatherGame_Progress_PartBody* pInstance = new CFatherGame_Progress_PartBody(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed Create : CFatherGame_Progress_PartBody");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CFatherGame_Progress_PartBody::Free()
{
    Safe_Release(m_pMugAlien);

    __super::Free();
}