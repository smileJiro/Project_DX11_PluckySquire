#pragma once
#include "Base.h"

#define MAX_GROUPID 32

BEGIN(Engine)
class CCollider;
END


BEGIN(Client)
union COLLIDER_ID
{
	struct
	{
		_uint iLeftID;
		_uint iRightID;
	};
	_ulonglong ID;
};
class CCollision_Manager final : public CBase
{
	DECLARE_SINGLETON(CCollision_Manager)
private:
	CCollision_Manager();
	virtual ~CCollision_Manager() = default;
public:
	void					Update();

private: /* Collision Group Update (그룹별 충돌검사 루틴) */
	void					Collision_GroupUpdate(const array<vector<CCollider*>, MAX_GROUPID>& _ColGroups, _uint _iLeft, _uint _iRight);

public: /* Section 등록 */
	HRESULT					Register_Section(_uint _iSectionKey);

public: /* Group Filter 관리 */
	void					Check_GroupFilter(OBJECT_GROUP _eLeft, OBJECT_GROUP _eRight);						 // 그룹필터를 추가
	void					Erase_GroupFilter(OBJECT_GROUP _eLeft, OBJECT_GROUP _eRight);						 // 그룹필터를 삭제
	void					Clear_GroupFilter() { memset(m_iGroupFilter, 0, sizeof(_uint) * 32); }

public: /* Group에 Collider 등록 */
	HRESULT					Add_Collider(_uint _iSectionKey, OBJECT_GROUP _eGroupFilter, CCollider* _pCollider); // 콜라이더를 추가.

private:					
	_uint					Convert_GroupID_ToIndex(_uint _iGroupID);											 // 16진수 enum을 0부터 시작하는 1간격의 정수형태로 변환.
	array<vector<CCollider*>, MAX_GROUPID>* Find_Colliders(_uint _iSectionKey);

private: /* Colliders, GroupFilter */
	map<_uint, array<vector<CCollider*>, MAX_GROUPID>> m_Colliders;// map<섹션키, vector<collider>[32]>
	_uint					m_iGroupFilter[MAX_GROUPID] = {};

private: /* 이전프레임의 충돌 결과를 저장하는 map */
	map<_ulonglong, _bool> m_PrevCollInfo;

public:
	 virtual void			Free() override;
};
END
