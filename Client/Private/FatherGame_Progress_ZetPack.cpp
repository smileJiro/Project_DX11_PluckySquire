#include "stdafx.h"
#include "FatherGame_Progress_ZetPack.h"
#include "GameInstance.h"
#include "FatherGame.h"
#include "Section_Manager.h"
#include "Section_2D_PlayMap.h"
#include "Goblin.h"
#include "PortalLocker.h"
#include "Portal_Default.h"
#include "ZetPack_Child.h"
#include "Player.h"

CFatherGame_Progress_ZetPack::CFatherGame_Progress_ZetPack(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CFatherGame_Progress(_pDevice, _pContext)
{
}

HRESULT CFatherGame_Progress_ZetPack::Initialize(void* _pArg)
{
    FATHERGAME_PROGRESS_ZETPACK_DESC* pDesc = static_cast<CFatherGame_Progress_ZetPack::FATHERGAME_PROGRESS_ZETPACK_DESC*>(_pArg);

    
    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

   

    return S_OK;
}

HRESULT CFatherGame_Progress_ZetPack::Progress_Enter()
{
    if (true == m_isClearProgress)
    {
        MSG_BOX("이미 클리어 된 프로그레스입니다.");
        return E_FAIL;
    }

    /* Create ZetPackChild */
    CZetPack_Child::ZETPACK_CHILD_DESC ZetPackDesc = {};
    ZetPackDesc.pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Find_Layer(LEVEL_CHAPTER_6, TEXT("Layer_Player"))->Get_GameObject_Ptr(0));
    assert(ZetPackDesc.pPlayer);

    ZetPackDesc.iCurLevelID = LEVEL_CHAPTER_6;
    ZetPackDesc.Build_2D_Transform(_float2(45.f, -100.f), _float2(1.0f, 1.0f), 200.f);
    CGameObject* pGameObject = nullptr;
    if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_CHAPTER_6, TEXT("Prototype_GameObject_ZetPack_Child"), LEVEL_CHAPTER_6, TEXT("Layer_ZetPack_Child"), &pGameObject, &ZetPackDesc)))
        return E_FAIL;
    m_pZetPack_Child = static_cast<CZetPack_Child*>(pGameObject);
    Safe_AddRef(m_pZetPack_Child); // 해당 프로그레스가 종료되어도 제트팩은 6챕터 레이어에 여전히 존재할 것임.
    
    if (FAILED(CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(TEXT("Chapter6_SKSP_01"), pGameObject, SECTION_2D_PLAYMAP_OBJECT)))
        return E_FAIL;

    CFatherGame::GetInstance()->Set_ZetPack_Child(m_pZetPack_Child);
#ifdef _DEBUG
    cout << "FatherGame Progress_ZetPack Start" << endl;
#endif // _DEBUG
    return S_OK;
}

void CFatherGame_Progress_ZetPack::Progress_Update()
{
    if (true == m_isClearProgress)
        return;

    Progress_Clear();

}

HRESULT CFatherGame_Progress_ZetPack::Progress_Clear()
{
    /* 1. 찌리의 대화 가 종료되었고,  */
    if (0 < m_pZetPack_Child->Get_DialogueIndex() && CZetPack_Child::STATE_PORTALOUT == m_pZetPack_Child->Get_CurState())
    {
        m_isClearProgress = true;

        /* 1. Claer 이벤트 호출 */
        Event_Register_Trigger(TEXT("Chapter6_FatherGame_Progress_ZetPack_Clear"));
        /* 2. 아빠부품 ui on */

        /* 3. 자기 자신의 Active 상태를 False로 변경 */
        Event_SetActive(this, false);
#ifdef _DEBUG
        cout << "FatherGame Progress_ZetPack Clear" << endl;
#endif // _DEBUG
    }

    return S_OK;
}

HRESULT CFatherGame_Progress_ZetPack::Progress_Exit()
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

CFatherGame_Progress_ZetPack* CFatherGame_Progress_ZetPack::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, void* _pArg)
{
    CFatherGame_Progress_ZetPack* pInstance = new CFatherGame_Progress_ZetPack(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed Create : CFatherGame_Progress_ZetPack");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CFatherGame_Progress_ZetPack::Free()
{
    Safe_Release(m_pZetPack_Child);

    __super::Free();
}