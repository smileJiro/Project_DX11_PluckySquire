#include "stdafx.h"
#include <intrin.h>
#include "Collision_Manager.h"
#include "Collider.h"
#include "GameObject.h"

IMPLEMENT_SINGLETON(CCollision_Manager)
CCollision_Manager::CCollision_Manager()
{
}

void CCollision_Manager::Update()
{
    for (auto& Pair : m_Colliders) // Section
    {
        for (_uint iRow = 0; iRow < MAX_GROUPID; ++iRow) // Object Group
        {
            for (_uint iCol = iRow; iCol < MAX_GROUPID; ++iCol)
            {
                if (m_iGroupFilter[iRow] & (1 << iCol))
                {
                    Collision_GroupUpdate(Pair.second, iRow, iCol);
                }
            }
        }

        // 충돌검사가 모두 끝났다면, 데이터 안의 Collider를 삭제하고 clear
        for (_uint i = 0; i < MAX_GROUPID; ++i)
        {
            for (auto& pCollider : Pair.second[i])
                Safe_Release(pCollider);

            Pair.second[i].clear();
        }
    }
}

void CCollision_Manager::Collision_GroupUpdate(const array<vector<CCollider*>, MAX_GROUPID>& _ColGroups, _uint _iLeft, _uint _iRight)
{
    auto& LeftColliders  = _ColGroups[_iLeft];
    auto& RightColliders  = _ColGroups[_iRight];

    // PrevCollInfo iterator;
    map<_ulonglong, _bool>::iterator iter;

    for (_uint i = 0; i < LeftColliders.size(); ++i)
    {
        // LeftCollider가 유효하지 않거나, 비활성 상태인 경우
        if (nullptr == LeftColliders[i] || false == LeftColliders[i]->Is_Active())
            continue;

        for (_uint j = 0; j < RightColliders.size(); ++j)
        {
            // RightCollider가 유효하지 않거나, 비활성 상태인 경우, 혹은 자기 자신과의 충돌검사인 경우 
            if (nullptr == RightColliders[j] || false == RightColliders[j]->Is_Active() || LeftColliders[i] == RightColliders[j])
                continue;

            CCollider* pLeftCollider = LeftColliders[i];
            CCollider* pRightCollider = RightColliders[j];

            COLLIDER_ID ID = {};
            ID.iLeftID = pLeftCollider->Get_ColliderID();
            ID.iRightID = pRightCollider->Get_ColliderID();
            iter = m_PrevCollInfo.find(ID.ID);
            if (m_PrevCollInfo.end() == iter)
            {
                // 첫 충돌검사 대상.
                m_PrevCollInfo.emplace(ID.ID, false);
                iter = m_PrevCollInfo.find(ID.ID);
            }

            // 본격적인 충돌 검사 수행.
            CGameObject* pLeftObject = pLeftCollider->Get_Owner();
            CGameObject* pRightObject = pRightCollider->Get_Owner();

            if (true == pLeftCollider->Is_Collision(pRightCollider))
            {
                if (true == iter->second)
                {
                    // 이전 프레임에도 충돌 중이었다.
                    if (true == pLeftObject->Is_Dead() || true == pRightObject->Is_Dead() || false == pLeftObject->Is_Active() || false == pRightObject->Is_Active())
                    {
                        // 만약 두 오브젝트 중 하나가 Dead Object라면, 충돌 탈출.
                        pLeftObject->On_Collision2D_Exit(pLeftCollider, pRightCollider, pRightObject);
                        pRightObject->On_Collision2D_Exit(pRightCollider, pLeftCollider, pLeftObject);
                        iter->second = false;
                    }
                    else
                    {
                        pLeftObject->On_Collision2D_Stay(pLeftCollider, pRightCollider, pRightObject);
                        pRightObject->On_Collision2D_Stay(pRightCollider, pLeftCollider, pLeftObject);

                        if (true == pLeftCollider->Is_Block() && false == pRightCollider->Is_Trigger())
                            pLeftCollider->Block(pRightCollider);
                        else if (true == pRightCollider->Is_Block() && false == pLeftCollider->Is_Trigger())
                            pRightCollider->Block(pLeftCollider);
                    }
                }
                else
                {
                    // 이전프레임에는 충돌하지 않았다.(첫 충돌)
                    if (false == pLeftObject->Is_Dead() && false == pRightObject->Is_Dead() && true == pLeftObject->Is_Active() && true == pRightObject->Is_Active())
                    {
                        pLeftObject->On_Collision2D_Enter(pLeftCollider, pRightCollider, pRightObject);
                        pRightObject->On_Collision2D_Enter(pRightCollider, pLeftCollider, pLeftObject);
                        iter->second = true; // 이전 프레임정보를 업데이트

                    }
                }
            }
            else
            {
                // 현재 충돌 X
                if (true == iter->second)
                {
                    // 이전 프레임에는 충돌 상태였다면,
                    pLeftObject->On_Collision2D_Exit(pLeftCollider, pRightCollider, pRightObject);
                    pRightObject->On_Collision2D_Exit(pRightCollider, pLeftCollider, pLeftObject);
                    iter->second = false;
                }
                else
                {
                    // 이전프레임도 충돌상태가 아니였다면.
                }
            }
        }
    }
}

HRESULT CCollision_Manager::Register_Section(const _wstring& _strSectionKey)
{
    array<vector<CCollider*>, MAX_GROUPID> Colliders;
    if (nullptr != Find_Colliders(_strSectionKey))
        return E_FAIL;

    m_Colliders.emplace(_strSectionKey, Colliders);
    return S_OK;
}

void CCollision_Manager::Check_GroupFilter(OBJECT_GROUP _eLeft, OBJECT_GROUP _eRight)
{
    // 이미 비트연산자 전용 데이터야
    _uint iRow = (_uint)_eLeft;
    _uint iCol = (_uint)_eRight;

    if (iCol < iRow)
    {
        // 더 큰 GroupID가 열에 와야한다.
        iRow = (_uint)_eRight;
        iCol = (_uint)_eLeft;
    }

    // 16진수를 Index로 사용가능하게 변경.
    iRow = Convert_GroupID_ToIndex(iRow);
    // 이미 체크가 되어있더라도 그냥 추가한다. >>> 삭제하는 것은 별도로 만들거야.
    m_iGroupFilter[iRow] |= iCol;
}

void CCollision_Manager::Erase_GroupFilter(OBJECT_GROUP _eLeft, OBJECT_GROUP _eRight)
{
    // 이미 비트연산자 전용 데이터야
    _uint iRow = (_uint)_eLeft;
    _uint iCol = (_uint)_eRight;

    if (iCol < iRow)
    {
        // 더 큰 GroupID가 열에 와야한다.
        iRow = (_uint)_eRight;
        iCol = (_uint)_eLeft;
    }

    // 16진수를 Index로 사용가능하게 변경.
    iRow = Convert_GroupID_ToIndex(iRow);
    // 어떠한 비트 상태여도 icol을 제거한다.
    m_iGroupFilter[iRow] &= ~iCol;
}

HRESULT CCollision_Manager::Add_Collider(const _wstring& _strSectionKey, OBJECT_GROUP _eGroupFilter, CCollider* _pCollider)
{
    array<vector<CCollider*>, MAX_GROUPID>* pColliders = Find_Colliders(_strSectionKey);
    if (nullptr == pColliders)
        return E_FAIL;

    _uint iGroupIndex = Convert_GroupID_ToIndex(_eGroupFilter);
    if (MAX_GROUPID <= iGroupIndex)
        return E_FAIL;

    (*pColliders)[iGroupIndex].push_back(_pCollider);
    Safe_AddRef(_pCollider);
    return S_OK;
}

_uint CCollision_Manager::Convert_GroupID_ToIndex(_uint _iGroupID)
{
    if (_iGroupID == 0)
        return 0;

    DWORD dwIndex; // 결과를 저장할 변수
    _BitScanForward(&dwIndex, (DWORD)_iGroupID); // 최하위 비트의 위치를 계산
    return (_uint)dwIndex;
}

array<vector<CCollider*>, MAX_GROUPID>* CCollision_Manager::Find_Colliders(const _wstring& _strSectionKey)
{
    auto iter = m_Colliders.find(_strSectionKey);

    if (iter == m_Colliders.end())
        return nullptr;

    return &iter->second;
}

void CCollision_Manager::Free()
{
    for (auto& Pair : m_Colliders)
    {
        for (_uint i = 0; i < MAX_GROUPID; ++i)
        {
            for (auto& pCollider : Pair.second[i])
                Safe_Release(pCollider);

            Pair.second[i].clear();
        }
    }
    m_Colliders.clear();

	__super::Free();
}
