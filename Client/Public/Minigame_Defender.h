#pragma once
#include "ModelObject.h"
#include "DefenderMonster.h"
#include "DefenderSpawner.h"
BEGIN(Client)
class CDefenderSpawner;
class CDefenderPlayer;
class CDefenderPerson;
class CScrollModelObject;
class CSection_Manager;
class CDefenderCapsule;
class CMiniGame_Defender :
    public CModelObject
{
public:

	typedef struct tagDefender_ControllTowerDesc : public CModelObject::MODELOBJECT_DESC
	{
	}DEFENDER_CONTROLLTOWER_DESC;
private:
	explicit CMiniGame_Defender(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CMiniGame_Defender(const CMiniGame_Defender& _Prototype);
	virtual ~CMiniGame_Defender() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* _pArg) override;
	virtual void Priority_Update(_float _fTimeDelta) override;
	virtual void Update(_float _fTimeDelta) override;
	virtual void Late_Update(_float _fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	virtual void On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)override;
	virtual void On_Collision2D_Stay(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)override;
	virtual void On_Collision2D_Exit(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)override;

public:
	void Start_Gamde();
	void Restart_Game();
	void Rescue_Person(CDefenderPerson* _pPerson);
	void Clear_Game();

	_bool Is_Cleared() { return m_bClear; }
private:
	HRESULT Ready_Spanwer();
	HRESULT Ready_UI();
	virtual void Enter_Section(const _wstring _strIncludeSectionName)override;
	_vector Get_SidePosition(T_DIRECTION _eDirection);
	_vector Get_ScrolledPosition(_vector _vPosition);
	_bool Is_LeftSide(_vector _vPos);
	_bool Is_InsideScreen(_vector _vPos);
	_float Get_ScrolledDistance(_vector _vPos);
	_float Get_RightScreenDistance(_vector _vPos);
	_float Get_LeftScreenDistance(_vector _vPos);
private:
	CSection_Manager* m_pSectionManager = nullptr;
	CDefenderPlayer* m_pDefenderPlayer = nullptr;
	_bool m_bGameStart = false;

	map<DEFENDER_MONSTER_ID, wstring> m_mapMonsterPrototypeTag;
	map< DEFENDER_MONSTER_ID, CDefenderSpawner*> m_Spawners;

	CScrollModelObject* m_pSidePersonUI = nullptr;
	_float m_fSideUIDIstance = 500.f;//사이드 UI의 플레이어와의 거리
	_float m_fHalfScreenRange = 600.f;//

	_float2 m_vSectionSize = {};
	_float m_fCenterHeight = 0.f;
	_float m_fFloorHeight = -350.f;
	_float m_fSpawnDistance = 1000.f;//플레이어와의 거리
	_float m_fTimeAcc = 0.f;
	_float m_fLastCapsuleDestroyTime = 0.f;
	_float m_fCapsuleSpawnTerm = 15.f;
	_uint m_iCapsuleSpawnedCount = 0;
	CDefenderCapsule* m_pCurrentCapsule = nullptr;

	_uint m_iPersonCount = 0;
	_uint m_iMaxPersonCount = 7;
	_uint m_iSpawnedPersonCount = 0;

	_bool m_bClear = false;
public:
	static CMiniGame_Defender* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;
	virtual HRESULT	Cleanup_DeadReferences()override;
};

END