#pragma once
#include "Character.h"

/* 1. 타겟위치로 가라 >> 매번 가야 할 위치를 지정해줄거야 */
/* 2. 타겟 오브젝트를 따라다녀라 >> Chase */
/* 3. 전투 해라 */
/* 4. 대화 해라 */
/* 5. Friend 는 Idle 상태일때 Friend Queue에 명령들을 확인하고, 미리 추가된 행동들에 대해 작업을 수행한다. */
/* Friend 는 해당 기능만을 제공하고, 바이올렛, 쓰레시에서는 각각의 상태에 맞는 애니메이션 재생을 한다. */
/* Friend 매니저 클래스를 만들고, 해당 클래스를 통해서 Friend를 조작한다. */
// 6. Finght 로직 >>> 목표 레이어를 체크하며 가장 가까운 적과의 전투. >> Layer가 Active 상태를 체크하며 모두 정리 되었다면 Idle로 전환

BEGIN(Client)
class CFriend abstract : public CCharacter
{
public:
	enum FRIEND_MODE { MODE_DEFAULT, MODE_FIGHT, MODE_BOSS };
	enum FRIEND_STATE { FRIEND_IDLE, FRIEND_MOVE, FRIEND_CHASE, FRIEND_ATTACK, FRIEND_TALK, FRIEND_MOJAM, FRIEND_HIT, FRIEND_PULL, FRIEND_BACKROLL, FRIEND_ANY, FRIEND_LAST };
	enum COLLIDER_USE { COL_BODY, COL_ATTACK, COL_INTERACT, COL_LAST };
	enum DIRECTION 
	{
		DIR_DOWN,
		DIR_RIGHT,
		DIR_UP,
		DIR_LEFT,

		DIR_LAST
	};
public:
	typedef struct tagFriendDesc : CCharacter::CHARACTER_DESC
	{
		FRIEND_STATE eStartState = FRIEND_IDLE;
		DIRECTION	eStartDirection = DIRECTION::DIR_DOWN;
		_wstring	 strFightLayerTag; // 전투 대상 레이어
		_wstring	 strDialogueTag; // 다이얼로그 태그
		_int		 iStartDialogueIndex = -1;
		_int		 iNumDialoguesIndices = 0;

		_wstring	strPartBodyModelTag; // 렌더하고자하는 Body 모델 태그.
		_uint		iModelTagLevelID = 0;
	}FRIEND_DESC;

protected:
	CFriend(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CFriend(const CFriend& _Prototype);
	virtual ~CFriend() = default;

public:
	HRESULT					Initialize(void* _pArg) override; 
	void					Priority_Update(_float _fTimeDelta)override; 
	void					Update(_float _fTimeDelta) override;
	void					Late_Update(_float _fTimeDelta) override;
	HRESULT					Render() override;

public:
	virtual void			On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject);
	virtual void			On_Collision2D_Stay(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject);
	virtual void			On_Collision2D_Exit(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject);
	virtual void			On_Hit(CGameObject* _pHitter, _int _fDamg, _fvector _vForce);

public:
	virtual void			Switch_PartAnim(_uint _iPartIndex, _uint _iAnimIndex, _bool _isLoop);
	void					Move_Position(_float2 _vTargetPosition, CFriend::DIRECTION _eEndDirection);
	void					Set_Direction(DIRECTION _eDir) { m_eDirection = _eDir; }

public:
	// Get
	FRIEND_MODE				Get_CurMode() const { return m_eCurMode; }
	// Set
	void					Change_Mode(FRIEND_MODE _eMode) { 
		if(FAILED(Mode_Exit()))
			return;
		if(FAILED(Mode_Enter(_eMode)))
			return;
		m_eCurMode = _eMode;
	}
	virtual HRESULT			Mode_Enter(FRIEND_MODE _eNextMode);
	virtual HRESULT			Mode_Exit();
	void					Change_CurState(FRIEND_STATE _eState) { m_eCurState = _eState; State_Change(); } // 상태 강제 전환.
	void					Change_AnyState(_uint _iAnimIndex, _bool _isLoop, DIRECTION _eDirectionLR) {
		m_eCurState = FRIEND_ANY; 
		m_iAnyAnimIndex = _iAnimIndex;
		m_isAnyAnimLoop = _isLoop;
		m_eDirection = _eDirectionLR;
		m_ePreState = FRIEND_LAST;
		State_Change();
	}

	void					Set_CurState(FRIEND_STATE _eState) { m_eCurState = _eState; State_Change(); }
protected:
	FRIEND_MODE				m_eCurMode = FRIEND_MODE::MODE_DEFAULT;
	FRIEND_STATE			m_eCurState = FRIEND_STATE::FRIEND_LAST;
	FRIEND_STATE			m_ePreState = FRIEND_STATE::FRIEND_LAST;
	DIRECTION				m_eDirection = DIRECTION::DIR_DOWN;

protected: /* State_Idle */
	list<FRIEND_STATE>		m_StateIdleQueue;

protected: /* State_Any */
	_uint					m_iAnyAnimIndex = 0;
	_bool					m_isAnyAnimLoop = false;
public:
	void					Add_IdleQueue(FRIEND_STATE _eState); // 추가
	void					Pop_IdleQueue();					 // 삭제
	FRIEND_STATE			Check_IdleQueue();					 // 다음 상태 확인
	void					Clear_IdleQueue();					 // Idle Queue 비우기

protected: /* State_Move */
	_float2					m_vMoveTargetPosition = {};
	DIRECTION				m_eMoveEndDirection = DIRECTION::DIR_LAST;

protected: /* State_ Chase */
	CGameObject*			m_pChaseTarget = nullptr;
	_float					m_fChaseInterval = 50.f;

public:
	void					Set_ChaseTarget(CGameObject* _pChaseTarget);
	void					Delete_ChaseTarget() { Safe_Release(m_pChaseTarget); m_pChaseTarget = nullptr; }
	void					Find_NearestTargetFromLayer();
protected:
	void					Update_ChaseTargetDirection();		 // Target 위치 기반으로 방향 결정. 
	void					Update_MoveTargetDirection(_fvector _vTargetPos); // 이동 목표 위치 기반으로 방향 결정. 
	virtual void			Change_AnimIndex_CurDirection() = 0; // Target의 위치 기반 결정된 방향으로 애니메이션 교체.
	void					ChaseToTarget(_float _fTimeDelta);
	void					ChaseToTargetPosition(_float2 _vTargetPosition, _float _fTimeDelta);

protected: /* State_Attack */
	_wstring				m_strFightLayerTag; // 전투 대상 레이어 태그.
	_float2					m_vAttackCoolTime = { 2.0f, 0.0f };
	_float					m_fAttackRangeFactor = 1.3f; // chase range의 1.3배
	_int					m_iAttackActionCount = 0;
public:
	void					Set_FightLayerTag(const _wstring _strFightLayerTag) { m_strFightLayerTag = _strFightLayerTag; }

protected: /* State_Talk */
	_wstring				m_strDialogueTag;
	_int					m_iDialogueIndex = -1;
	_int					m_iNumDialogueIndices = 0;

protected:
	_float2					m_vPullTime = { 1.5f, 0.0f };
	_bool					m_isBackRollLoop = false;
	_float2					m_vBackRollTime = { 0.75f, 0.0f };
protected:
	virtual void			State_Change();
	virtual void			State_Change_Idle();
	virtual void			State_Change_Move();
	virtual void			State_Change_Chase();
	virtual void			State_Change_Attack();
	virtual void			State_Change_Talk();
	virtual void			State_Change_Mojam();
	virtual void			State_Change_Hit();
	virtual void			State_Change_Pull();
	virtual void			State_Change_BackRoll();
	virtual void			State_Change_Any();

protected:
	virtual void			Action_State(_float _fTimeDelta);
	virtual void			Action_State_Idle(_float _fTimeDelta);
	virtual void			Action_State_Move(_float _fTimeDelta);
	virtual void			Action_State_Chase(_float _fTimeDelta);
	virtual void			Action_State_Attack(_float _fTimeDelta);
	virtual void			Action_State_Talk(_float _fTimeDelta);
	virtual void			Action_State_Mojam(_float _fTimeDelta);
	virtual void			Action_State_Hit(_float _fTimeDelta);
	virtual void			Action_State_Pull(_float _fTimeDelta);
	virtual void			Action_State_BackRoll(_float _fTimeDelta);
	virtual void			Action_State_Any(_float _fTimeDelta);

protected:
	virtual void			Action_State_Boss(_float _fTimeDelta) {};
	virtual void			State_Change_Boss() {};


protected:
	virtual void			Finished_DialogueAction() = 0;
	virtual void			On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx) = 0;
	virtual void			Switch_AnimIndex_State() = 0;
	virtual void			Move_Arrival();
private:
	HRESULT					Ready_Components(); // 콜라이더, 중력 컴포넌트 필요
	HRESULT					Ready_PartObjects(FRIEND_DESC* _pDesc); // Body 필요.

public:
	void Free() override;
};

END