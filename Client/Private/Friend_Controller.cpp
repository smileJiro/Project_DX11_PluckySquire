#include "stdafx.h"
#include "Friend_Controller.h"
#include "GameInstance.h"
#include "PlayerData_Manager.h"
#include "Friend.h"

IMPLEMENT_SINGLETON(CFriend_Controller)

CFriend_Controller::CFriend_Controller()
    :m_pGameInstance(CGameInstance::GetInstance())
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CFriend_Controller::Initialize(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    m_pDevice = _pDevice;
    Safe_AddRef(m_pDevice);
    m_pContext = _pContext;
    Safe_AddRef(m_pContext);

    return S_OK;
}

HRESULT CFriend_Controller::Register_Friend(const _wstring& _strFriendTag, CFriend* _pFriend)
{
    if (nullptr != Find_Friend(_strFriendTag))
    {
        MSG_BOX("같은 친구가 두명 등록되었어.");
        return E_FAIL;
    }

    m_Friends.emplace(_strFriendTag, _pFriend);
    Safe_AddRef(_pFriend);

    return S_OK;
}

HRESULT CFriend_Controller::Erase_Friend(const _wstring& _strFriendTag)
{
    CFriend* pFriend = Find_Friend(_strFriendTag);
    if (nullptr == pFriend)
    {
        MSG_BOX("있지도 않은 친구를 지우려한다고? 괘 씸 해");
        return E_FAIL;
    }

    Safe_Release(pFriend);
    m_Friends.erase(_strFriendTag);

    Erase_Friend_FromTrainList(_strFriendTag);
    return S_OK;
}

CFriend* CFriend_Controller::Find_Friend(const _wstring& _strFriendTag)
{
    auto& iter = m_Friends.find(_strFriendTag);

    if (iter == m_Friends.end())
        return nullptr;

    return iter->second;
}

HRESULT CFriend_Controller::Start_Train()
{
    for (_uint i = 0; i < m_TrainList.size(); ++i)
    {
        _wstring* pTag = Find_Friend_FromTrainList(i);
        CFriend* pFriend = Find_Friend(*pTag);
        
        if (i == 0) // 첫번째 녀석은 Player를 따라가.
        {
            CPlayer* pPlayer = CPlayerData_Manager::GetInstance()->Get_NormalPlayer_Ptr();
            if (nullptr == pPlayer)
                return E_FAIL;
            if (COORDINATE_3D == pPlayer->Get_CurCoord())
                return E_FAIL;

            pFriend->Set_ChaseTarget(pPlayer);
        }
        else // 그 다음 녀석부터는 자기 앞 index의 녀석을 따라가
        {
            _wstring* pPrevTag = Find_Friend_FromTrainList(i - 1);
            CFriend* pPrevFriend = Find_Friend(*pPrevTag);
            if (nullptr == pPrevFriend)
                return E_FAIL;
            if (COORDINATE_3D == pPrevFriend->Get_CurCoord())
                return E_FAIL;

            pFriend->Set_ChaseTarget(pPrevFriend);
        }
    }

}

HRESULT CFriend_Controller::End_Train()
{
    for (_uint i = 0; i < m_TrainList.size(); ++i)
    {
        _wstring* pTag = Find_Friend_FromTrainList(i);
        CFriend* pFriend = Find_Friend(*pTag);

        pFriend->Set_ChaseTarget(nullptr);
    }

    return S_OK;
}

HRESULT CFriend_Controller::Register_Friend_ToTrainList(const _wstring& _strFriendTag)
{
    // 친구목록에 없으면 Fail
    if (nullptr == Find_Friend(_strFriendTag))
        return E_FAIL;

    // 이미 TrainList에 있으면 Fail
    if (nullptr != Find_Friend_FromTrainList(_strFriendTag))
        return E_FAIL;

    m_TrainList.push_back(_strFriendTag);
    return S_OK;
}

HRESULT CFriend_Controller::Erase_Friend_FromTrainList(const _wstring& _strFriendTag)
{
    auto& iter = m_TrainList.begin();
    for(iter; iter != m_TrainList.end(); )
    {
        if (_strFriendTag == (*iter))
            break;
        else
            ++iter;
    }

    if(iter != m_TrainList.end())
    {
        m_TrainList.erase(iter);
        return S_OK;
    }
    
    return E_FAIL;
}

_wstring* CFriend_Controller::Find_Friend_FromTrainList(const _wstring& _strFriendTag)
{
    auto& iter = m_TrainList.begin();
    for (iter; iter != m_TrainList.end(); )
    {
        if (_strFriendTag == (*iter))
            break;
        else
            ++iter;
    }

    if (iter == m_TrainList.end())
        return nullptr;

    return &(*iter);
}

_wstring* CFriend_Controller::Find_Friend_FromTrainList(_uint _iIndex)
{
    auto& iter = m_TrainList.begin();
    for (_uint i = 0; i < _iIndex; ++i)
    {
        if (iter == m_TrainList.end())
            return nullptr;
        else
            ++iter;
    }

    return &(*iter);
}

void CFriend_Controller::Free()
{
    Safe_Release(m_pGameInstance);
    Safe_Release(m_pContext);
    Safe_Release(m_pDevice);

    for (auto& Pair : m_Friends)
        Safe_Release(Pair.second);
    m_Friends.clear();

    m_TrainList.clear();
    __super::Free();
}
