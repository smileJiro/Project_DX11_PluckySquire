#pragma once
#include "GameObject.h"
#include "Client_Defines.h"

BEGIN(Client)

class CMonster;

class CFormation : public CGameObject
{
public:
	typedef struct tagFormationDesc : public GAMEOBJECT_DESC
	{
		_wstring strMemberPrototypeTag;
		_wstring strMemberLayerTag;
		_uint iRow = {};
		_uint iColumn = {};
		_float fRow_Interval = {};
		_float fColumn_Interval = {};
		_float fDelayTime = { 0.f };
	}FORMATIONDESC;

private:
	CFormation(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CFormation(const CGameObject& Prototype);
	virtual ~CFormation() = default;

public:
	_bool Has_EmptySlot();

public:
	virtual HRESULT				Initialize_Prototype();
	virtual HRESULT				Initialize(void* _pArg);
	virtual void				Update(_float _fTimeDelta);

public:
	HRESULT Initialize_Members(void* _pArg);
	HRESULT Initialize_OffSets();
	void Add_Formation_PatrolPoints(_float3& _vPatrolPoint);

	//비어있는 위치로 추가
	_bool Add_To_Formation(CMonster* _pMember, CFormation** _pFormation);

	_bool Remove_From_Formation(CMonster* _pMember);

	_bool Get_Formation_Position(CMonster* _pMember, _float3* _vPosition);


private:
	vector<CMonster*> m_Members;
	set<_uint> m_EmptySlots;
	vector<_float3> m_PatrolPoints;
	vector<_float3> m_OffSets;
	_uint m_iRow = { 3 };
	_uint m_iColumn = { 4 };
	_float m_fRow_Interval = { 1.5f };
	_float m_fColumn_Interval = { 1.5f };

	_float m_fAccTime = { 0.f };
	_float m_fDelayTime = { 0.f };

public:
	HRESULT Cleanup_DeadReferences() override;

public:
	static CFormation* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;
};

END