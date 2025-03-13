#pragma once
#include "ContainerObject.h"
#include "Stoppable.h"

BEGIN(Client)
typedef struct tagCharacterStat
{
	_int iDamg = 1;
	_int iHP = 12;
	_int iMaxHP = 12;
}STAT;
enum class AUTOMOVE_TYPE
{
	//임의의 지점까지 이동하기
	MOVE_TO,
	//특정 방향으로 이동하기
	MOVE_TOWARD,
	//보는 방향만 바꾸기
	LOOK_DIRECTION,
	//애니메이션만 바꾸기
	CHANGE_ANIMATION,
	//그냥 대기하기
	WAIT,
	LAST
};
typedef struct tagAutoMoveCommand
{
	friend class CCharacter;
	enum STATE
	{
		STATE_START,
		STATE_PREDELAY,
		STATE_RUN,
		STATE_POSTDELAY,
		STATE_END,
		STATE_LAST
	};
	AUTOMOVE_TYPE eType;
	_uint iAnimIndex;
	//보는 방향, 이동 지점
	_vector vTarget;
	//선딜레이
	_float fPreDelayTime = 0.f;
	//후딜레이
	_float fPostDelayTime = 0.f;

private:
	void Update(_float _fTimeDelta) 
	{
		fTimeAcc += _fTimeDelta; 
		if (Is_Start())
			eState = STATE_PREDELAY;
		else if (Is_PreDelayTime() && fTimeAcc >= fPreDelayTime)
		{
			eState = STATE_RUN;
			fTimeAcc = 0.f;
		}
		else if (Is_RunTime())
		{
		}
		else if (Is_PostDelayTime() && fTimeAcc >= fPostDelayTime)
		{
			eState = STATE_END;
		}
	}
	_bool Is_Start() { return STATE_START == eState;  }
	_bool Is_PreDelayTime() { return STATE_PREDELAY== eState;}
	_bool Is_RunTime() {return STATE_RUN == eState;}
	_bool Is_PostDelayTime() { return STATE_POSTDELAY == eState; }
	_bool Is_End() { return STATE_END == eState; }
	void Complete_Command() {eState = STATE_POSTDELAY; fTimeAcc = 0.f;}

private: 
	STATE eState = STATE_START;
	_float fTimeAcc = 0.f;
}AUTOMOVE_COMMAND;

class CCharacter abstract : public CContainerObject, public IStoppable
{
public:
	typedef struct tagCharacterDesc : public CContainerObject::CONTAINEROBJ_DESC
	{
		STAT _tStat;
		_float _fStepSlopeThreshold = 0.35f;
		_float _fStepHeightThreshold =0.35f;
		_bool _isIgnoreGround = false;
	}CHARACTER_DESC;

protected:
	CCharacter(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CCharacter(const CCharacter& _Prototype);
	virtual ~CCharacter() = default;

public:
	virtual HRESULT			Initialize(void* _pArg) override;
	virtual void			Priority_Update(_float _fTimeDelta)override;
	virtual void			Update(_float _fTimeDelta) override;
	virtual void			Late_Update(_float _fTimeDelta) override;

	virtual void OnContact_Modify(const COLL_INFO& _My, const COLL_INFO& _Other, CModifiableContacts& _ModifiableContacts, _bool _bIm0)override;	virtual void OnContact_Enter(const COLL_INFO& _My, const COLL_INFO& _Other, const vector<PxContactPairPoint>& _ContactPointDatas) override;
	virtual void OnContact_Stay(const COLL_INFO& _My, const COLL_INFO& _Other, const vector<PxContactPairPoint>& _ContactPointDatas) override;
	virtual void OnContact_Exit(const COLL_INFO& _My, const COLL_INFO& _Other, const vector<PxContactPairPoint>& _ContactPointDatas) override;
	virtual void Enter_Section(const _wstring _strIncludeSectionName) override;
public:

	void Stop_Rotate();
	void Stop_Move();
	void Stop_MoveXZ();

	virtual void Move(_fvector _vVelocity, _float _fTimeDelta);
	_bool Move_To_3D(_fvector _vPosition, _float _fEpsilon = 0.5f, _bool _FreezeY = true);
	_bool Move_To(_fvector _vPosition, _float _fTimeDelta);
	_bool Check_Arrival(_fvector _vPosition, _float _fEpsilon = 0.5f);
	_bool Check_Arrival(_fvector _vPrevPosition, _fvector _vNextPosition, _fvector _vTargetPosition);
	//캐릭터 기준 _vDir 방향을 바라보게 하는 함수. Y축으로만 회전함.
	void LookDirectionXZ_Kinematic(_fvector _vDir);
	void LookDirectionXZ_Dynamic(_fvector _vDir);
	//속도는 degree로 줘야함 (초당), 목표 방향에 도달시 true 리턴
	_bool Rotate_To(_fvector _vDirection, _float _fSpeed);
	_bool Rotate_To_Radians(_fvector _vDirection, _float _fSpeed);

	void KnockBack(_fvector _vForce);

	virtual void On_Land() {};
	virtual void On_Change2DDirection(E_DIRECTION _eCurrentDir) {};

public:
	//GET
	_bool Is_Dynamic();
	_bool Is_OnGround();
	_bool Is_PlatformerMode() { return m_bPlatformerMode; }
	STAT& Get_Stat() { return m_tStat; }
	_float Get_StepSlopeThreshold() { return m_fStepSlopeThreshold; }
	_vector Get_ScrolledPosition();
	_vector Get_ScrolledPosition(_vector _vPosition);
	E_DIRECTION Get_2DDirection() { return m_e2DDirection_E; }
	//SET
	void Set_ScrollingMode(_bool _bScrollingMode);
	virtual void Set_2DDirection(E_DIRECTION _eEDir, _bool _isOnChange = true);
	virtual void Set_2DDirection(F_DIRECTION _eFDir, _bool _isOnChange = true);

	
protected:
	_float Measure_FloorDistance();
	_vector StepAssist(_fvector _vVelocity, _float _fTimeDelta);

public:
	void Go_Straight_F_Dir(_float _fTimeDelta);

//AUTO MOVE
public:
	//COMMAND를 넣은 순서대로 실행됨.
	void Add_AutoMoveCommand(AUTOMOVE_COMMAND _pCommand);
	/// <param name="_bAutoClearQue"> : 큐에 든 모든 AutoMove가 완료되면 자동으로 큐를 비우기</param>
	void Start_AutoMove(_bool _bAutoClearQue);
	void Stop_AutoMove();
	void Clear_AutoMove();
	_bool Is_AutoMoving() { return m_bAutoMoving; }
	virtual void On_StartAutoMove() {};
	virtual void On_EndAutoMove() {};
private:

	_bool Process_AutoMove(_float _fTimeDelta);
	_bool Process_AutoMove_MoveTo(const AUTOMOVE_COMMAND& _pCommand, _float _fTimeDelta);
	_bool Process_AutoMove_MoveToward(const AUTOMOVE_COMMAND& _pCommand, _float _fTimeDelta);
	_bool Process_AutoMove_LookDirection(const AUTOMOVE_COMMAND& _pCommand, _float _fTimeDelta);
	_bool Process_AutoMove_ChangeAnimation(const AUTOMOVE_COMMAND& _pCommand, _float _fTimeDelta);
	_bool Process_AutoMove_Wait(const AUTOMOVE_COMMAND& _pCommand, _float _fTimeDelta);
protected:
	STAT m_tStat;

	_float m_fKnockBackForce = {};
	_float m_fKnockBackAccTime = {};

	_float m_fStepSlopeThreshold = 0.45f;
	_float m_fStepHeightThreshold = 0.15f;
	_bool	m_isKnockBack = {};
	_bool m_bOnGround = false;
	_bool m_bPlatformerMode = false;
	_bool m_bScrollingMode = false;
	_float3 m_vKnockBackDirection = {};
	_vector m_vLookBefore = { 0.f,0.f,-1.f };
	_vector m_v3DTargetDirection = { 0.f,0.f,-1.f };
	_float m_f3DFloorDistance = 0.f;
	_float m_f2DFloorDistance = 0.f;
	_float m_f2DUpForce = 0.f;
	_float4x4 m_matQueryShapeOffset;
	_vector m_vFloorNormal;
	_float m_fGravity = 0.f;

	_bool m_isIgnoreGround = { false };

	E_DIRECTION m_e2DDirection_E = E_DIRECTION::E_DIR_LAST;
	//MOVE TO

	//AUTOMOVE
	queue<AUTOMOVE_COMMAND> m_AutoMoveQue;
	_bool m_bAutoMoving = false;
	_bool m_bAutoClearAutoMoves = false;
public:	
	virtual void Free() override;


};

END