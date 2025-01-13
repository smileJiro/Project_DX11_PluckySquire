#include "Collision_Manager.h"
#include "GameInstance.h"
#include "Layer.h"
#include "ContainerObject.h"

CCollision_Manager::CCollision_Manager()
	: m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CCollision_Manager::Initialize()
{

	return S_OK;
}

void CCollision_Manager::Update()
{
	Update_CollisionLayer();
}

void CCollision_Manager::Add_CollisionLayerCheckInfo(COLL_CHECK* _pCollCheckLayerData)
{
	if (nullptr == _pCollCheckLayerData)
		return;

	m_CollCheckLayerData.push_back(_pCollCheckLayerData);
}

void CCollision_Manager::Level_Enter(_int _iNextLevelID)
{
	m_iCurLevelID = _iNextLevelID;
}

void CCollision_Manager::Level_Exit()
{
	Clear();
}

void CCollision_Manager::Clear()
{
	for (auto& pCollCheckData : m_CollCheckLayerData)
		Safe_Delete(pCollCheckData);

	m_CollCheckLayerData.clear();

	m_mapPrevData.clear();
}

_bool CCollision_Manager::Intersect_RayCollision(_fvector _vRayStart, _fvector _vRayDir, const _wstring& _strLayerTag, _int _iOtherPartIndex, const CGameObject* _pThis)
{
	/* 레이의 전방 경로에 충돌체가 하나라도 존재한다면 즉시 탈출. */
	CLayer* pLayer = m_pGameInstance->Find_Layer(m_iCurLevelID, _strLayerTag);
	if (nullptr == pLayer)
		return false;
	const list<CGameObject*>& GameObjects = pLayer->Get_GameObjects();

	_bool bResult = false;
	_float fDst = 0.0f;
	for (auto& pGameObject : GameObjects)
	{
		if (_pThis == pGameObject)
			continue;
		CGameObject* pColCheckObject = Get_CollCheckObject(pGameObject, _iOtherPartIndex);
		const vector<CCollider*>& Colliders = pColCheckObject->Get_Colliders();
		if (Colliders.empty())
			continue;

		for (auto& pCollider : Colliders)
		{
			if (pCollider->Intersect_Ray(_vRayStart, _vRayDir, &fDst))
			{
				if (0.0f < fDst)
				{
					bResult = true;
					break;
					pCollider->Set_Collision(CCollider::ISCOLLISION::COLLISION_RAY);
				}
				else
				{
					pCollider->Set_Collision(CCollider::ISCOLLISION::NONE);
				}
			}
		}
		

	}
	return bResult;
}


_bool CCollision_Manager::Intersect_RayCollision_Nearest(_fvector _vRayStart, _fvector _vRayDir, const _wstring& _strLayerTag, _float* _pOutDst, CGameObject** _ppOutNearestObj, const CGameObject* _pThis, _uint _iCollidersIndex, _int _iOtherPartIndex)
{
	/* 매개변수로 들어온 Layer를 순회하며 Intersect_Ray 함수 호출. */
	CLayer* pLayer = m_pGameInstance->Find_Layer(m_iCurLevelID, _strLayerTag);
	if (nullptr == pLayer)
		return false;
	const list<CGameObject*>& GameObjects = pLayer->Get_GameObjects();
	_bool bResult = false;
	_float fMinDst = 9999.f;
	_float fDst = 10000.f;
	CCollider* pFinalCollider = nullptr;
	for (auto& pGameObject : GameObjects)
	{
		if (_pThis == pGameObject)
			continue;
		CGameObject* pColCheckObject = Get_CollCheckObject(pGameObject, _iOtherPartIndex);
		const vector<CCollider*>& Colliders = pColCheckObject->Get_Colliders();
		if (Colliders.empty())
			continue;
		/* 지정한 특정 콜라이더에 대해서 검사 */
		if (Colliders[_iCollidersIndex]->Intersect_Ray(_vRayStart, _vRayDir, &fDst))
		{
			/* 충돌이 발생 했다. 음수는 혹시모르니 예외처리. */
			if (fMinDst > fDst && 0.0f < fDst)
				fMinDst = fDst;

			bResult = true;
			*_ppOutNearestObj = pGameObject;
			Colliders[_iCollidersIndex]->Set_Collision(CCollider::ISCOLLISION::COLLISION_RAY);
		}
		else
		{
			Colliders[_iCollidersIndex]->Set_Collision(CCollider::ISCOLLISION::NONE);
		}

	}

	*_pOutDst = fMinDst;
	return bResult;
}

_bool CCollision_Manager::Intersect_RayCollision_NearestDistance(_fvector _vRayStart, _fvector _vRayDir, const _wstring& _strLayerTag, _float* _pOutDst, const CGameObject* _pThis, _uint _iCollidersIndex, _int _iOtherPartIndex)
{
	/* 매개변수로 들어온 Layer를 순회하며 Intersect_Ray 함수 호출. */
	CLayer* pLayer = m_pGameInstance->Find_Layer(m_iCurLevelID, _strLayerTag);
	if (nullptr == pLayer)
		return false;
	const list<CGameObject*>& GameObjects = pLayer->Get_GameObjects();
	_bool bResult = false;
	_float fMinDst = 9999.f;
	_float fDst = 10000.f;
	CCollider* pFinalCollider = nullptr;
	for (auto& pGameObject : GameObjects)
	{
		if (_pThis == pGameObject)
			continue;
		CGameObject* pColCheckObject = Get_CollCheckObject(pGameObject, _iOtherPartIndex);
		const vector<CCollider*>& Colliders = pColCheckObject->Get_Colliders();
		if (Colliders.empty())
			continue;
		/* 지정한 특정 콜라이더에 대해서 검사 */
		if (Colliders[_iCollidersIndex]->Intersect_Ray(_vRayStart, _vRayDir, &fDst))
		{
			/* 충돌이 발생 했다. 음수는 혹시모르니 예외처리. */
			if (fMinDst > fDst && 0.0f < fDst)
				fMinDst = fDst;

			bResult = true;
			Colliders[_iCollidersIndex]->Set_Collision(CCollider::ISCOLLISION::COLLISION_RAY);
		}
		else
		{
			Colliders[_iCollidersIndex]->Set_Collision(CCollider::ISCOLLISION::NONE);
		}

	}
	
	*_pOutDst = fMinDst;
	return bResult;
}

CGameObject* CCollision_Manager::Get_CollCheckObject(CGameObject* _pGameObject, _int iIndex)
{
	if(iIndex == -1)
	{
		return _pGameObject;
	}

	return static_cast<CContainerObject*>(_pGameObject)->Get_PartObject(iIndex);
}

void CCollision_Manager::Update_CollisionLayer()
{
	for (auto& pCollCheckData : m_CollCheckLayerData)
	{
		if (pCollCheckData->left.PartObjectIndices.empty() || pCollCheckData->right.PartObjectIndices.empty())
			continue;
		CLayer* pLeftLayer = m_pGameInstance->Find_Layer(m_iCurLevelID, wstring(pCollCheckData->left.szLayerTag));
		CLayer* pRightLayer = m_pGameInstance->Find_Layer(m_iCurLevelID, wstring(pCollCheckData->right.szLayerTag));
		if (nullptr == pLeftLayer || nullptr == pRightLayer)
			continue;
		const list<CGameObject*>& LeftObjList = pLeftLayer->Get_GameObjects();
		const list<CGameObject*>& RightObjList = pRightLayer->Get_GameObjects();

		map<_ulonglong, _bool>::iterator iter;
		CGameObject* pLeftCollObj = nullptr;
		CGameObject* pRightCollObj = nullptr;
		CCollider* pLeftColl = nullptr;
		CCollider* pRightColl = nullptr;
		for (auto& pLeft : LeftObjList)
		{
			for (size_t i = 0; i < pCollCheckData->left.PartObjectIndices.size(); ++i)
			{
				/* 인덱스에 담긴 실제 충돌검사 대상 오브젝트 리턴. */
				pLeftCollObj = Get_CollCheckObject(pLeft, pCollCheckData->left.PartObjectIndices[i]);
				const vector<CCollider*>& LeftColliders = pLeftCollObj->Get_Colliders();
				if (LeftColliders.empty())
					continue;

				for (auto& pLeftColl : LeftColliders)
				{					
					for (auto& pRight : RightObjList)
					{
						for (size_t j = 0; j < pCollCheckData->right.PartObjectIndices.size(); ++j)
						{
							pRightCollObj = Get_CollCheckObject(pRight, pCollCheckData->right.PartObjectIndices[j]);

							const vector<CCollider*>& RightColliders = pRightCollObj->Get_Colliders();
							if (RightColliders.empty())
								continue;
							/* 예외처리 : Collider가 없거나, 자기 자신과 충돌하는 경우. */
							if (pLeftCollObj == pRightCollObj)
								continue;
							for (auto& pRightColl : RightColliders)
							{
								/* Collider ID 탐색, 없으면 Insert */
								COLLIDER_ID ID = {};
								ID.iLeft_ID = pLeftColl->Get_ColliderID();
								ID.iRight_ID = pRightColl->Get_ColliderID();

								iter = m_mapPrevData.find(ID.ID);
								if (m_mapPrevData.end() == iter)
								{
									m_mapPrevData.emplace(ID.ID, false);
									iter = m_mapPrevData.find(ID.ID);
								}

								/* 충돌 검사 후, 이전 프레임 정보와 합산하여 현재 충돌 상태를 결정. */
								if (pLeftColl->Intersect(pRightColl))
								{
									/* 충돌 검사 True */

									if (true == iter->second)
									{
										/* 이전 프레임에도 충돌 상태였다 >>> OnCollision()*/

										if (pLeftCollObj->Is_Dead() || pRightCollObj->Is_Dead() ||
											!pLeftCollObj->Is_Active() || !pRightCollObj->Is_Active() ||
											!pLeftColl->Is_Active() || !pRightColl->Is_Active())
										{
											/* 예외처리 : 두 오브젝트 중 하나라도 Is_Dead() 거나 Is_Active() == false 라면, */
											pLeftCollObj->OnCollision_Exit(pLeftColl, pRightCollObj, pRightColl);
											pRightCollObj->OnCollision_Exit(pRightColl, pLeftCollObj, pLeftColl);

											iter->second = false;
											pLeftColl->Set_Collision(CCollider::ISCOLLISION::NONE);
											pRightColl->Set_Collision(CCollider::ISCOLLISION::NONE);
										}
										else
										{
											pLeftCollObj->OnCollision(pLeftColl, pRightCollObj, pRightColl);
											pRightCollObj->OnCollision(pRightColl, pLeftCollObj, pLeftColl);
										}


									}
									else
									{
										if (pLeftCollObj->Is_Dead() || pRightCollObj->Is_Dead() ||
											!pLeftCollObj->Is_Active() || !pRightCollObj->Is_Active()||
											!pLeftColl->Is_Active() || !pRightColl->Is_Active())
										{
											continue;
										}
										else
										{
											pLeftCollObj->OnCollision_Enter(pLeftColl, pRightCollObj, pRightColl);
											pRightCollObj->OnCollision_Enter(pRightColl, pLeftCollObj, pLeftColl);

											iter->second = true;
											pLeftColl->Set_Collision(CCollider::ISCOLLISION::COLLISION);
											pRightColl->Set_Collision(CCollider::ISCOLLISION::COLLISION);
										}

									}
								}
								else
								{
									/* 이번 프레임에 충돌하지 않았다. */
									if (iter->second)
									{
										/* 하지만 이전 프레임에는 충돌 상태였다. */
										pLeftCollObj->OnCollision_Exit(pLeftColl, pRightCollObj, pRightColl);
										pRightCollObj->OnCollision_Exit(pRightColl, pLeftCollObj, pLeftColl);

										iter->second = false;
										pLeftColl->Set_Collision(CCollider::ISCOLLISION::NONE);
										pRightColl->Set_Collision(CCollider::ISCOLLISION::NONE);
									}

								}
							}


						}

					}
					
				}

				

			}

		}

	}

}

CCollision_Manager* CCollision_Manager::Create()
{
	CCollision_Manager* pInstance = new CCollision_Manager();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CCollision_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCollision_Manager::Free()
{
	Clear();
	Safe_Release(m_pGameInstance);

	__super::Free();
}
