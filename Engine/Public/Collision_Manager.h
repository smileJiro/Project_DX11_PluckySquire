#pragma once
#include "Base.h"

BEGIN(Engine)

#pragma region Collider_ID(Union)
union COLLIDER_ID
{
	struct
	{
		_uint iLeft_ID;
		_uint iRight_ID;
	};
	_ulonglong ID;
};
#pragma endregion // Collider_ID(Union)


#pragma region Collision_CheckInfo_Struct
typedef struct tagCollCheckInfo
{
	struct Left
	{
		_tchar szLayerTag[MAX_PATH];
		vector<_int> PartObjectIndices;
	} left;
	struct Right
	{
		_tchar szLayerTag[MAX_PATH];
		vector<_int> PartObjectIndices;
	} right;

}COLL_CHECK;
#pragma endregion // Collision_CheckInfo_Struct


class CCollider;
class CGameObject;
class CGameInstance;
class CCollision_Manager final : public CBase
{
private:
	CCollision_Manager();
	virtual ~CCollision_Manager() = default;

public:
	HRESULT Initialize();
	void Update();							/* Update 내에서 충돌 검사 로직 수행. */

public: /* Ray Collision : 1회성 호출 함수. */
	_bool Intersect_RayCollision(_fvector _vRayStart, _fvector _vRayDir, const _wstring& _strLayerTag, _int _iOtherPartIndex, const CGameObject* _pThis);
	_bool Intersect_RayCollision_Nearest(_fvector _vRayStart, _fvector _vRayDir, const _wstring& _strLayerTag, _float* _pOutDst, CGameObject** _ppOutNearestObj, const CGameObject* _pThis, _uint _iCollidersIndex, _int _iOtherPartIndex);
	_bool Intersect_RayCollision_NearestDistance(_fvector _vRayStart, _fvector _vRayDir, const _wstring& _strLayerTag, _float* _pOutDst, const CGameObject* _pThis, _uint _iCollidersIndex, _int _iOtherPartIndex);

public:
	void Add_CollisionLayerCheckInfo(COLL_CHECK* _pCollCheckLayerData);	/* 충돌 검사 수행 대상 정보 수집. */
	void Level_Enter(_int _iNextLevelID);
	void Level_Exit();


	void Set_CurLevelID(_uint _iLevelID) { m_iCurLevelID = _iLevelID; }
private:
	CGameInstance* m_pGameInstance = nullptr;
	map<_ulonglong, _bool> m_mapPrevData;	/* 이전 프레임 충돌 상태에 대한 정보. */
	vector<COLL_CHECK*> m_CollCheckLayerData;

	_uint m_iCurLevelID = 0;

private:
	void Update_CollisionLayer();			/* 예외처리, 레이어 조건 판별 후 Loop */
	CGameObject* Get_CollCheckObject(CGameObject* _pGameObject, _int iIndex); /* 인덱스 확인 후, */

private:
	void Clear();							/* 레벨 전환 시, 데이터 리셋. */

public:
	static CCollision_Manager* Create();
	virtual void Free() override;
};
END

/* 충돌 검사 함수를 분리 >>> 검사 대상 Object 타입에 따라. */