#include "stdafx.h"
#include "FatherGame_Progress_MakeFather.h"
#include "GameInstance.h"
#include "FatherGame.h"
#include "Section_Manager.h"
#include "Section_2D_PlayMap.h"
#include "PlayerData_Manager.h"

#include "Portal_Default.h"
#include "Player.h"
#include "ZetPack_Child.h"
#include "MiniGame_Defender.h"
#include "ZetPack_Father.h"


CFatherGame_Progress_MakeFather::CFatherGame_Progress_MakeFather(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CFatherGame_Progress(_pDevice, _pContext)
{
}

HRESULT CFatherGame_Progress_MakeFather::Initialize(void* _pArg)
{
    FATHERGAME_PROGRESS_MAKEFATHER_DESC* pDesc = static_cast<CFatherGame_Progress_MakeFather::FATHERGAME_PROGRESS_MAKEFATHER_DESC*>(_pArg);

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CFatherGame_Progress_MakeFather::Progress_Enter()
{
    if (true == m_isClearProgress)
    {
        MSG_BOX("이미 클리어 된 프로그레스입니다.");
        return E_FAIL;
    }

    
    {/* Create ZetPack_Father */
    /* Create ZetPackChild */
        CZetPack_Father::ZETPACK_FATHER_DESC ZetPackDesc = {};
        ZetPackDesc.iCurLevelID = LEVEL_CHAPTER_6;
        ZetPackDesc.Build_2D_Transform(_float2(-142.f, -75.f), _float2(1.0f, 1.0f), 200.f);
        CGameObject* pGameObject = nullptr;
        if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_CHAPTER_6, TEXT("Prototype_GameObject_ZetPack_Father"), LEVEL_CHAPTER_6, TEXT("Layer_ZetPack_Father"), &pGameObject, &ZetPackDesc)))
            return E_FAIL;
        m_pZetPack_Father = static_cast<CZetPack_Father*>(pGameObject);
        Safe_AddRef(m_pZetPack_Father); // 해당 프로그레스가 종료되어도 제트팩은 6챕터 레이어에 여전히 존재할 것임.

        if (FAILED(CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(TEXT("Chapter6_SKSP_01"), pGameObject, SECTION_2D_PLAYMAP_OBJECT)))
            return E_FAIL;

        CFatherGame::GetInstance()->Set_ZetPack_Father(m_pZetPack_Father);

    }/* Create ZetPack_Father */

#ifdef _DEBUG
    cout << "FatherGame Progress_MakeFather Start" << endl;
#endif // _DEBUG

    return S_OK;
}

void CFatherGame_Progress_MakeFather::Progress_Update()
{
    if (true == m_isClearProgress)
        return;


    Progress_Clear();

}

HRESULT CFatherGame_Progress_MakeFather::Progress_Clear()
{
    
    CZetPack_Child* pZetPack_Child = CFatherGame::GetInstance()->Get_ZetPack_Child();
    if (CZetPack_Father::STATE_IDLE == m_pZetPack_Father->Get_CurState() && CZetPack_Child::STATE_PORTALOUT == pZetPack_Child->Get_CurState())
    {
        m_isClearProgress = true;
        /* 3. 자기 자신의 Active 상태를 False로 변경 */
        Event_SetActive(this, false);
#ifdef _DEBUG
        cout << "FatherGame Progress_MakeFather Clear" << endl;
#endif // _DEBUG
    }

    return S_OK;
}

HRESULT CFatherGame_Progress_MakeFather::Progress_Exit()
{
    if (false == m_isClearProgress)
        return S_OK;

    {/* Clear 후 Exit 호출이 정상적인 로직으로 판단 함. */

    }
#ifdef _DEBUG
    cout << "FatherGame Progress_MakeFather Exit" << endl;
#endif // _DEBUG
    return S_OK;
}

CFatherGame_Progress_MakeFather* CFatherGame_Progress_MakeFather::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, void* _pArg)
{
    CFatherGame_Progress_MakeFather* pInstance = new CFatherGame_Progress_MakeFather(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed Create : CFatherGame_Progress_MakeFather");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CFatherGame_Progress_MakeFather::Free()
{
    Safe_Release(m_pZetPack_Father);
    __super::Free();
}