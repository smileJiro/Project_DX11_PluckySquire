#pragma once
#include "ContainerObject.h"
#include "DefenderMonster.h"
#include "DefenderSpawner.h"

BEGIN(Engine)
class CModelObject;
class CCustomFont;
END

BEGIN(Client)
class CDefenderSpawner;
class CDefenderPlayer;
class CDefenderPerson;
class CScrollModelObject;
class CSection_Manager;
class CDefenderCapsule;
class CDialog_Manager;
//포탈에 입장하면 포탈 사라짐. 
//BlockoInput 후 Dialog 시작
//lDialog 끝나면 캐릭터가 앞으로 걸어가다가 TransformIn


//다 구하면
//몬스터들 다 사라짐
//임무 완료 폰트가 지나감
//다지나가면 잠깐 페이드아웃&인
//BlockInput되면서 Trasnform OUt
//TransformOut 끝나면 관제탑으로 카메라타겟 전환 &  End Dialog 시작
//EndDIJalog 끝나면 아빠 머리 생성하고 카메라 타겟 전환
//아빠 머리까지 다가가서 충돌되면 GetItem 상태로 전환.
//GetItem 상태 끝나면 진짜 끝/
class CMiniGame_Defender :
    public CContainerObject
{
public:
	enum DEFENDER_PROGRESS_STATE
	{
		DEFENDER_PROG_NONE,
		DEFENDER_PROG_ENTERED,
		DEFENDER_PROG_BEGINNING_DIALOG,
		DEFENDER_PROG_TRANSFORM_IN,
		DEFENDER_PROG_GAME,
		DEFENDER_PROG_MISSION_COMPLETE_FONT,
		DEFENDER_PROG_MISSION_COMPLETE_FADEOUT,
		DEFENDER_PROG_MISSION_COMPLETE_FADEIN,
		DEFENDER_PROG_TRANSFORM_OUT,
		DEFENDER_PROG_ENDING_DIALOG,
		DEFENDER_PROG_REWARDING,
		DEFENDER_PROG_CLEAR,

	};
	enum DEFENDER_PART_ID
	{
		DEFENDER_PART_TOWER,
		DEFENDER_PART_COUNTER_BACK,
		DEFENDER_PART_COUNTER_0,
		DEFENDER_PART_COUNTER_1,
		DEFENDER_PART_COUNTER_2,
		DEFENDER_PART_COUNTER_3,
		DEFENDER_PART_COUNTER_4,
		DEFENDER_PART_COUNTER_5,
		DEFENDER_PART_COUNTER_6,
		DEFENDER_PART_COUNTER_7,
		DEFENDER_PART_COUNTER_8,
		DEFENDER_PART_COUNTER_9,
		DEFENDER_PART_LAST
	};
	typedef struct tagDefender_Desc : public CContainerObject::CONTAINEROBJ_DESC
	{
	}DEFENDER_DESC;
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
	void Mission_Complete();


	_bool Is_Cleared() { return m_bClear; }

	void Set_GameState(DEFENDER_PROGRESS_STATE _eState);
private:

private:
	HRESULT Ready_ControllTower();
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
	void Set_LeftPersonCount(_uint _iCount);

	void On_PlayerAnimEnd(COORDINATE _eCoord, _uint iAnimIdx);
private:
	CSection_Manager* m_pSectionManager = nullptr;
	CDefenderPlayer* m_pDefenderPlayer = nullptr;
	CDialog_Manager* m_pDialogManager = nullptr;
	DEFENDER_PROGRESS_STATE m_eGameState = DEFENDER_PROGRESS_STATE::DEFENDER_PROG_NONE;

	map<DEFENDER_MONSTER_ID, wstring> m_mapMonsterPrototypeTag;
	map< DEFENDER_MONSTER_ID, CDefenderSpawner*> m_Spawners;

	CScrollModelObject* m_pSidePersonUI = nullptr;
	_float m_fSideUIDIstance = 500.f;//사이드 UI의 플레이어와의 거리
	_float m_fHalfScreenRange = 600.f;//

	_float2 m_vSectionSize = {};
	_float m_fCenterHeight = 0.f;
	_float m_fFloorHeight = -350.f;
	_float m_fSpawnDistance = 600.f;//플레이어와의 거리
	_float m_fTimeAcc = 0.f;
	_float m_fLastCapsuleDestroyTime = 0.f;
	_float m_fCapsuleSpawnTerm = 7.5f;
	CDefenderCapsule* m_pCurrentCapsule = nullptr;

	_uint m_iPersonLeft = 9;
	_uint m_iMaxPersonCount = 9;
	_uint m_iCapsuleSpawnCount = 0;
	_uint m_iMaxCapsuleSpawnCount = 5;
	_uint m_iCapsulePersonCount[5] = { 1,1,2,2,3 };//캡슐에 들어있는 사람 수
	CModelObject* m_pPersonCounterBack = nullptr;
	CModelObject* m_pPersonCounter[10] = { nullptr, };


	_float m_fMissionClearFontTIme = 1.5f;
	_float m_fMissionClearFontTImeAcc= 0.f;
	CCustomFont* m_pMissionCompleteFont = nullptr;
	_bool m_bClear = false;

	_vector m_vTransformIn_Position = { -1820.f, -180.f, 0.f };
	_vector m_vStart_Position = { -2400.f, -290.f, 0.f };
	_float m_fCamTargetChangeTime = 1.f;
	_float m_fCamTargetChangeTimeAcc = 0.f;

public:
	static CMiniGame_Defender* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;
	virtual HRESULT	Cleanup_DeadReferences()override;
};

END