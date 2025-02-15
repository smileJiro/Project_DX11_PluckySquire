#pragma once
#include "Character.h"
#include "AnimEventReceiver.h"

BEGIN(Engine)
class CAnimEventGenerator;
class CCollider; // test
class CModelObject;
END
BEGIN(Client)
class CCarriableObject;
class CStateMachine;
enum PLAYER_INPUT
{
	PLAYER_INPUT_MOVE,
	PLAYER_INPUT_JUMP,
	PLAYER_INPUT_ATTACK,
	PLAYER_KEY_ROLL,
	PLAYER_KEY_THROWSWORD,
	PLAYER_KEY_INTERACT,
	PLAYER_KEY_THROWOBJECT,
	PLAYER_KEY_SNEAK,
	PLAYER_KEY_SPINATTACK,
	PLAYER_KEY_SPINCHARGING,
	PLAYER_KEY_SPINLAUNCH,
	PLAYER_KEY_LAST
};
typedef struct tagPlayerInputResult
{
	_vector vMoveDir = {0,0,0};
	_vector vDir = { 0,0,0 };
	_bool bInputStates[PLAYER_KEY_LAST] = {false,};

}PLAYER_INPUT_RESULT;
class CPlayer final : public CCharacter, public virtual  IAnimEventReceiver
{
public:
	typedef struct tagAttackTriggerDesc
	{
		_float fRadius;
		_float fRadianAngle;
		_float2 fOffset = {};
	}ATTACK_TRIGGER_DESC_2D;
	enum ATTACK_TYPE
	{
		ATTACK_TYPE_NORMAL1,
		ATTACK_TYPE_NORMAL2,
		ATTACK_TYPE_NORMAL3,
		ATTACK_TYPE_SPIN,
		ATTACK_TYPE_JUMPATTACK,
		ATTACK_TYPE_LAST
	};
	enum PLAYER_MODE
	{
		PLAYER_MODE_NORMAL,
		PLAYER_MODE_SWORD,
		PLAYER_MODE_SNEAK,
		PLAYER_MODE_LAST
	};

	enum PLAYER_PART
	{
		PLAYER_PART_SWORD= 1,
		PLAYER_PART_GLOVE,
		PLAYER_PART_LAST
	};
	enum STATE
	{
		IDLE,
		RUN,
		JUMP_UP,
		JUMP_DOWN,
		ATTACK,
		JUMP_ATTACK,
		ROLL,
		THROWSWORD,
		PICKUPOBJECT,
		THROWOBJECT,
		CLAMBER,
		SPINATTACK,
		DIE,
		STATE_LAST
	};
	enum class ANIM_STATE_2D
	{
		JOT_GATEPASS,
		JOT_HANDEVICT,
		JOT_HANDEVICT_SMUSHLOOP,
		PLAYER_C06_END,
		PLAYER_C06_IDLE01,
		PLAYER_C06_IDLE01TRANSITION,
		PLAYER_C06_IDLE01_INTO,
		PLAYER_C06_IDLE02,
		PLAYER_C06_IDLE03,
		PLAYER_C06_LOOKUP,
		PLAYER_C08V02_END,
		PLAYER_C08V02_IDLE01,
		PLAYER_C08V02_IDLE02,
		PLAYER_C08V02_LOOKUPLEFT,
		PLAYER_C08V02_LOOKUPRIGHT,
		PLAYER_CLIFFCRUMBLE,
		PLAYER_CYBERJOTLITE_RUN_IDLE_TEMP,
		PLAYER_CYBERJOTLITE_RUN_RIGHT,
		PLAYER_CYBERJOTLITE_TRANSFORM,
		PLAYER_GIVEPOP,
		PLAYER_GIVEWAND,
		PLAYER_SLEEPING_LOOP,
		PLAYER_SLEEPING_WAKEUP,
		PLAYER_SLIPPINGV02_NOSWORD_DOWN,
		PLAYER_SLIPPINGV02_NOSWORD_RIGHT,
		PLAYER_SLIPPINGV02_NOSWORD_UP,
		PLAYER_SLIPPINGV02_SWORD_DOWN,
		PLAYER_SLIPPINGV02_SWORD_RIGHT,
		PLAYER_SLIPPINGV02_SWORD_UP,
		PLAYER_TUMBADANCEV03,
		PLAYER_MOJAM_MOJAM,
		PLAYER_MOJAM_MOJAM_C02_0910,
		PLAYER_MOJAM_MOJAM_C10_0910,
		JOT_GAMEOVER,
		JOT_GAMEOVER_LOOP,
		JOT_GAMEOVER_NOSWORD,
		JOT_GAMEOVER_NOSWORD_LOOP,
		JOT_LOOKUP_HOLD_RIGHT,
		JOT_LOOKUP_RIGHT,
		JOT_SURPRISE_HOLD_RIGHT,
		JOT_SURPRISE_RIGHT,
		JOT_TOIDLE_RIGHT,
		JUMP_IN_PLAYER_ZIPIN_DOWN,
		JUMP_IN_PLAYER_ZIPIN_RIGHT,
		JUMP_IN_PLAYER_ZIPIN_UP,
		JUMP_OUT_PLAYER_ZIPOUT_DOWN_EDIT,
		JUMP_OUT_PLAYER_ZIPOUT_RIGHT_EDIT,
		JUMP_OUT_PLAYER_ZIPOUT_UP_EDIT,
		PLAYER_ATTACK_DOWN,
		PLAYER_ATTACK_RIGHT,
		PLAYER_ATTACK_UP,
		PLAYER_ATTACKCOMBO_01_SS,
		PLAYER_ATTACKCOMBO_01_DOWN,
		PLAYER_ATTACKCOMBO_01_RIGHT,
		PLAYER_ATTACKCOMBO_01_UP,
		PLAYER_ATTACKCOMBO_03_SS,
		PLAYER_ATTACKCOMBO_03_DOWN,
		PLAYER_ATTACKCOMBO_03_RIGHT,
		PLAYER_ATTACKCOMBO_03_UP,
		PLAYER_ATTACKV02_SPIN_DOWN_LVL1,
		PLAYER_ATTACKV02_SPIN_DOWN_LVL2,
		PLAYER_ATTACKV02_SPIN_DOWN_LVL3,
		PLAYER_ATTACKV02_SPIN_DOWN_LVL4,
		PLAYER_ATTACKV02_SPIN_INTO_DOWN,
		PLAYER_ATTACKV02_SPIN_INTO_RIGHT,
		PLAYER_ATTACKV02_SPIN_INTO_UP,
		PLAYER_ATTACKV02_SPIN_RIGHT_LVL1,
		PLAYER_ATTACKV02_SPIN_RIGHT_LVL2,
		PLAYER_ATTACKV02_SPIN_RIGHT_LVL3,
		PLAYER_ATTACKV02_SPIN_RIGHT_LVL4,
		PLAYER_ATTACKV02_SPIN_UP_LVL1,
		PLAYER_ATTACKV02_SPIN_UP_LVL2,
		PLAYER_ATTACKV02_SPIN_UP_LVL3,
		PLAYER_ATTACKV02_SPIN_UP_LVL4,
		PLAYER_ATTACKV02_SS,
		PLAYER_BOOKENTER_DOWN,
		PLAYER_BOOKENTER_FALL_DOWN,
		PLAYER_BOOKENTER_FALL_RIGHT,
		PLAYER_BOOKENTER_FALL_UP,
		PLAYER_BOOKENTER_RIGHT,
		PLAYER_BOOKENTER_UP,
		PLAYER_BOOKEXIT_DOWN,
		PLAYER_BOOKEXIT_RIGHT,
		PLAYER_BOOKEXIT_UP,
		PLAYER_C06_CRUSHROOMGET,
		PLAYER_C06_CRUSHROOMGET_IDLE,
		PLAYER_C06_POWERUP_END,
		PLAYER_C06_RAISESWORD,
		PLAYER_C06_RAISESWORD_LOOP,
		PLAYER_C06_SWORDBEAM_LOOP,
		PLAYER_C06_SWORDBEAM_OUT,
		PLAYER_CANNON_CURL_CANCEL,
		PLAYER_CANNON_CURL_INTO,
		PLAYER_CANNON_CURL_SPIN,
		PLAYER_CANNONCHARGE_DOWN,
		PLAYER_CANNONCHARGE_RIGHT,
		PLAYER_CANNONCHARGE_UP,
		PLAYER_CARRY_IDLE_DOWN,
		PLAYER_CARRY_IDLE_RIGHT,
		PLAYER_CARRY_IDLE_RIGHT_SS,
		PLAYER_CARRY_IDLE_UP,
		PLAYER_CATCH_GLITCHBIRD,
		PLAYER_CATCH_GLITCHBIRD_OUTLINE,
		PLAYER_CYBERJOTLITE_BOOKEXIT_DOWN,
		PLAYER_DEATH_DOWN,
		PLAYER_DODGEROLL_DOWN_NOSWORD,
		PLAYER_DODGEROLL_DOWN_SINGLEROLLONLY,
		PLAYER_DODGEROLL_RIGHT_NOSWORD,
		PLAYER_DODGEROLL_RIGHT_SINGLEROLLONLY,
		PLAYER_DODGEROLL_UP_NOSWORD,
		PLAYER_DODGEROLL_UP_SINGLEROLLONLY,
		PLAYER_DROP_OBJECT_DOWN,
		PLAYER_DROP_OBJECT_RIGHT,
		PLAYER_DROP_OBJECT_SS,
		PLAYER_DROP_OBJECT_UP,
		PLAYER_ELECTROCUTE,
		PLAYER_HAPPY,
		PLAYER_IDLE_DOWN,
		PLAYER_IDLE_RIGHT,
		PLAYER_IDLE_SWORD_DOWN,
		PLAYER_IDLE_SWORD_RIGHT,
		PLAYER_IDLE_SWORD_UP,
		PLAYER_IDLE_UP,
		PLAYER_ITEM_RETRIEVE,
		PLAYER_ITEM_RETRIEVE_V02,
		PLAYER_JUMP_FALL_DOWN,
		PLAYER_JUMP_FALL_OBJECT_DOWN,
		PLAYER_JUMP_FALL_OBJECT_RIGHT,
		PLAYER_JUMP_FALL_OBJECT_UP,
		PLAYER_JUMP_FALL_RIGHT,
		PLAYER_JUMP_FALL_SWORD_DOWN,
		PLAYER_JUMP_FALL_SWORD_RIGHT,
		PLAYER_JUMP_FALL_SWORD_UP,
		PLAYER_JUMP_FALL_UP,
		PLAYER_JUMP_RISE_DOWN,
		PLAYER_JUMP_RISE_OBJECT_DOWN,
		PLAYER_JUMP_RISE_OBJECT_RIGHT,
		PLAYER_JUMP_RISE_OBJECT_UP,
		PLAYER_JUMP_RISE_RIGHT,
		PLAYER_JUMP_RISE_SWORD_DOWN,
		PLAYER_JUMP_RISE_SWORD_RIGHT,
		PLAYER_JUMP_RISE_SWORD_UP,
		PLAYER_JUMP_RISE_UP,
		PLAYER_JUMP_RISE_UP_,
		PLAYER_JUMPCARRY_RISE_DOWN,
		PLAYER_JUMPCARRY_RISE_UP,
		PLAYER_JUMPINGATTACK_FALL_DOWN,
		PLAYER_JUMPINGATTACK_FALL_RIGHT,
		PLAYER_JUMPINGATTACK_FALL_UP,
		PLAYER_JUMPINGATTACK_OUT_DOWN,
		PLAYER_JUMPINGATTACK_OUT_RIGHT,
		PLAYER_JUMPINGATTACK_OUT_UP,
		PLAYER_JUMPINGATTACK_RISE_DOWN,
		PLAYER_JUMPINGATTACK_RISE_RIGHT,
		PLAYER_JUMPINGATTACK_RISE_UP,
		PLAYER_KNOCKEDBACK_FLOOR,
		PLAYER_KNOCKEDBACK_INTO,
		PLAYER_KNOCKEDBACK_LOOP,
		PLAYER_KNOCKEDBACK_OUT,
		PLAYER_MOJAM_INTO,
		PLAYER_PICKUP_IDLE_DOWN,
		PLAYER_PICKUP_IDLE_RIGHT,
		PLAYER_PICKUP_IDLE_UP,
		PLAYER_PICKUP_OBJECT_DOWN,
		PLAYER_PICKUP_OBJECT_RIGHT,
		PLAYER_PICKUP_OBJECT_SS,
		PLAYER_PICKUP_OBJECT_UP,
		PLAYER_PULL_LOOP,
		PLAYER_PULL_OBJECT_DOWN,
		PLAYER_PULL_OBJECT_RIGHT,
		PLAYER_PULL_OBJECT_SS,
		PLAYER_PULL_OBJECT_UP,
		PLAYER_PUSH_OBJECT_DOWN,
		PLAYER_PUSH_OBJECT_RIGHT,
		PLAYER_PUSH_OBJECT_SS,
		PLAYER_PUSH_OBJECT_UP,
		PLAYER_ROLL_INTO,
		PLAYER_ROLL_LOOP,
		PLAYER_ROLL_OUT,
		PLAYER_RUN_DOWN,
		PLAYER_RUN_OBJECT_DOWN,
		PLAYER_RUN_OBJECT_RIGHT,
		PLAYER_RUN_OBJECT_UP,
		PLAYER_RUN_RIGHT,
		PLAYER_RUN_SWORD_DOWN,
		PLAYER_RUN_SWORD_RIGHT,
		PLAYER_RUN_SWORD_UP,
		PLAYER_RUN_UP,
		PLAYER_SWORD_RETRIEVE,
		PLAYER_SWORDTHROW_AIMLOOP_DOWN,
		PLAYER_SWORDTHROW_AIMLOOP_RIGHT,
		PLAYER_SWORDTHROW_AIMLOOP_UP,
		PLAYER_SWORDTHROW_INTO_DOWN,
		PLAYER_SWORDTHROW_INTO_RIGHT,
		PLAYER_SWORDTHROW_INTO_UP,
		PLAYER_SWORDTHROW_OUT_DOWN,
		PLAYER_SWORDTHROW_OUT_RIGHT,
		PLAYER_SWORDTHROW_OUT_UP,
		PLAYER_THROW_OBJECT_DOWN,
		PLAYER_THROW_OBJECT_RIGHT,
		PLAYER_THROW_OBJECT_UP,
		PLAYER_TRAPPED,
		PLAYER_ZIPLINE,
	};
	enum class ANIM_STATE_3D
	{
		LATCH_ANIM_BOOK_JUMP_FALL_RIGHT
		,LATCH_ANIM_BOOK_JUMP_LAND_RIGHT
		,LATCH_ANIM_IDLE_01
		,LATCH_ANIM_JUMP_LAND_02
		,LATCH_ANIM_JUMP_UP_02 =4
		,LATCH_KNOCKED_DOWN_AND_EATEN_FROM_BEHIND_LATCH
		,LATCH_KNOCKED_DOWN_AND_EATEN_FROM_BEHIND_LOOP_LATCH
		,LATCH_KNOCKED_DOWN_AND_EATEN_FROM_FRONT_LATCH
		,LATCH_KNOCKED_DOWN_AND_EATEN_FROM_FRONT_LOOP_LATCH
		,LATCH_TURN_LEFT
		,LATCH_TURN_MID
		,LATCH_TURN_RIGHT
		,LATCH_ANIM_ATTACK_02_GT
		,LATCH_ANIM_ATTACK_03_GT
		,LATCH_ANIM_BOOK_JUMP_FALL_BACK_NEWRIG = 14 //책 속으로 뛰어들기?
		,LATCH_ANIM_BOOK_JUMP_FALL_FRONT_NEWRIG
		,LATCH_ANIM_BOOK_JUMP_FALL_LEFT_NEWRIG
		,LATCH_ANIM_BOOK_JUMP_INTO_BACK_NEWRIG
		,LATCH_ANIM_BOOK_JUMP_INTO_FRONT_GT
		,LATCH_ANIM_BOOK_JUMP_INTO_FRONT_NEWRIG
		,LATCH_ANIM_BOOK_JUMP_INTO_LEFT_NEWRIG
		,LATCH_ANIM_BOOK_JUMP_INTO_RIGHT_GT
		,LATCH_ANIM_BOOK_JUMP_INTO_RIGHT_NEWRIG
		,LATCH_ANIM_BOOK_JUMP_LAND_BACK_NEWRIG
		,LATCH_ANIM_BOOK_JUMP_LAND_FRONT_NEWRIG
		,LATCH_ANIM_BOOK_JUMP_LAND_LEFT_NEWRIG = 25
		,LATCH_ANIM_BOOKOUT_01_GT
		,LATCH_ANIM_CALL_GT
		,LATCH_ANIM_CALL_NEWRIG
		,LATCH_ANIM_CANON_FLY_GT
		,LATCH_ANIM_CANON_FLY_LAND_GT
		,LATCH_ANIM_CAUGHT_GT
		,LATCH_ANIM_CLAMBER_01_EDIT_NEWRIG
		,LATCH_ANIM_CLAMBER_01_GT
		,LATCH_ANIM_DIE_GT
		,LATCH_ANIM_DIE_NEWRIG
		,LATCH_ANIM_DODGE_GT
		,LATCH_ANIM_DRAGGED_BWD_GT
		,LATCH_ANIM_DRAGGED_FWD_GT_ANIM
		,LATCH_ANIM_IDLE_01_GT
		,LATCH_ANIM_IDLE_NERVOUS_01_GT
		,LATCH_ANIM_IDLE_NERVOUS_01_NEWRIG
		,LATCH_ANIM_IDLE_NERVOUS_TURN_01_GT
		,LATCH_ANIM_IDLE_NERVOUS_TURN_01_NEWRIG
		,LATCH_ANIM_ITEM_GET_GT
		,LATCH_ANIM_ITEM_GET_NEWRIG
		,LATCH_ANIM_JUMP_DOWN_01_NEWRIG = 46
		,LATCH_ANIM_JUMP_DOWN_02_GT
		,LATCH_ANIM_JUMP_DOWN_02_NEWRIG
		,LATCH_ANIM_JUMP_LAND_02_GT
		,LATCH_ANIM_JUMP_START_SHORT_GT
		,LATCH_ANIM_JUMP_START_SHORT_NEWRIG
		,LATCH_ANIM_JUMP_UP_02_GT
		,LATCH_ANIM_JUMPATTACK_RISE_GT = 53
		,LATCH_ANIM_LUNCHBOX_BACKFLIP_GT
		,LATCH_ANIM_LUNCHBOX_POSE_01_LOOP_GT
		,LATCH_ANIM_LUNCHBOX_POSE_02_LOOP_GT
		,LATCH_ANIM_PULL_GT
		,LATCH_ANIM_PULL_LEFT_GT
		,LATCH_ANIM_PULL_RIGHT_GT
		,LATCH_ANIM_PUSH_GT
		,LATCH_ANIM_PUTDOWN_GT
		,LATCH_ANIM_PUTDOWN_NEWRIG
		,LATCH_ANIM_REMOTE_HOLD
		,LATCH_ANIM_REMOTE_PUSH_GT
		,LATCH_ANIM_RUN_01_GT
		,LATCH_ANIM_RUN_SWORD_01_GT
		,LATCH_ANIM_SPIN_ATTACK_INTO_GT
		,LATCH_ANIM_SPIN_ATTACK_OUT_GT
		,LATCH_ANIM_SPIN_ATTACK_SPIN_LOOP_GT
		,LATCH_ANIM_SPRINT_01_NEWRIG
		,LATCH_ANIM_STAMP_ATTACK_EDIT_NEWRIG
		,LATCH_ANIM_STAMP_ATTACK_GT
		,LATCH_ANIM_STAMP_ERASE_GT
		,LATCH_ANIM_STAMP_HOLD_GT
		,LATCH_ANIM_STAMP_HOLD_NEWRIG
		,LATCH_ANIM_STAMP_PULLOUT_SHORT_GT
		,LATCH_ANIM_STAMP_PULLOUT_SHORT_GT_EDIT
		,LATCH_ANIM_STAMP_PULLOUT_SHORT_NEWRIG
		,LATCH_ANIM_STEALTH_RUN_GT
		,LATCH_ANIM_SWORDTHROW_AIM_GT = 80
		,LATCH_ANIM_SWORDTHROW_AIM_NEWRIG
		,LATCH_ANIM_SWORDTHROW_INTO_GT
		,LATCH_ANIM_SWORDTHROW_INTO_NEWRIG
		,LATCH_ANIM_SWORDTHROW_THROW_GT
		,LATCH_ANIM_SWORDTHROW_THROW_NEWRIG
		,LATCH_ANIM_TORCH_STRAFE_BACK_01_GT
		,LATCH_ANIM_TORCH_STRAFE_FWD_01_GT
		,LATCH_ANIM_TORCH_STRAFE_LEFT_01_GT
		,LATCH_ANIM_TORCH_STRAFE_RIGHT_01_GT
		,LATCH_ANIM_TRAILER_OUTBOOK_01_ANIM = 90
		,LATCH_CINE_D09_LB_INTRO_SH_01
		,LATCH_CINE_D09_LB_INTRO_SH_02
		,LATCH_CINE_D09_LB_INTRO_SH_03
		,LATCH_CINE_GAUNTLET_EVENT_SHOT_01_GT
		,LATCH_PICKUP_GT
		,LATCH_PICKUP_IDLE_GT
		,LATCH_PICKUP_IDLE_NEWRIG
		,LATCH_PICKUP_NEWRIG
		,LATCH_STEALTH_IDLE_GT
		,LATCH_THROW_NEWRIG
		,LATCH_ANIM_ATTACK_01_BLOCKED_GT_EDIT
		,LATCH_ANIM_ATTACK_01_GT_EDIT
		,LATCH_ANIM_JUMPATTACK_FALL =103
		,LATCH_ANIM_JUMPATTACK_LAND_GT_EDIT
		,LATCH_ANIM_JUMPATTACK_RISE  = 105
		,LATCH_ANIM_LAST
	};
private:
	CPlayer(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CPlayer(const CPlayer& _Prototype);
	virtual ~CPlayer() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* _pArg) override;
	virtual void			Priority_Update(_float _fTimeDelta) override;
	virtual void			Update(_float _fTimeDelta) override;
	virtual void			Late_Update(_float _fTimeDelta) override;
	virtual HRESULT			Render() override;

public:
	virtual void OnContact_Enter(const COLL_INFO& _My, const COLL_INFO& _Other, const vector<PxContactPairPoint>& _ContactPointDatas) override;
	virtual void OnContact_Stay(const COLL_INFO& _My, const COLL_INFO& _Other, const vector<PxContactPairPoint>& _ContactPointDatas) override;
	virtual void OnContact_Exit(const COLL_INFO& _My, const COLL_INFO& _Other, const vector<PxContactPairPoint>& _ContactPointDatas) override;

	virtual void OnTrigger_Enter(const COLL_INFO& _My, const COLL_INFO& _Other);
	virtual void OnTrigger_Stay(const COLL_INFO& _My, const COLL_INFO& _Other);
	virtual void OnTrigger_Exit(const COLL_INFO& _My, const COLL_INFO& _Other);

public: /* 2D 충돌 */
	void						On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject);
	void						On_Collision2D_Stay(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject);
	void						On_Collision2D_Exit(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject);

	void						On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx);
	virtual void		On_Hit(CGameObject* _pHitter, _float _fDamg) override;
	virtual HRESULT				Change_Coordinate(COORDINATE _eCoordinate, _float3* _pNewPosition = nullptr) override;
	void Move_Attack_3D();
	void Attack(CGameObject* _pVictim);
	void Move(_fvector _vForce, _float _fTimeDelta);
	void Move_Forward(_float fVelocity, _float _fTImeDelta);
	void Jump();
	void	ThrowSword();
	void ThrowObject();
	PLAYER_INPUT_RESULT Player_KeyInput();
	//Get
	_bool Is_OnGround() {return m_bOnGround;}
	_bool Is_SneakMode() {return PLAYER_MODE_SNEAK == m_ePlayerMode;}
	_bool Is_Sneaking();//소리가 안나면 true 나면 false
	_bool Is_SwordMode() { return PLAYER_MODE_SWORD == m_ePlayerMode; }
	_bool Is_SwordHandling();
	_bool Is_CarryingObject(){ return nullptr != m_pCarryingObject; }
	_bool Is_AttackTriggerActive();
	_bool Is_PlatformerMode() { return m_bPlatformerMode; }
	_float Get_UpForce();
	_float Get_AnimProgress();
	_float Get_HeadHeight() { return m_fHeadHeight; }
	_float Get_StepSlopeThreshold() { return m_fStepSlopeThreshold; }
	_float Get_ArmHeight() { return m_fArmHeight; }
	_float Get_ArmLength() { return m_fArmLength; }
	_float Get_AirRotationSpeed() { return m_fAirRotateSpeed; }
	_float Get_AirRunSpeed() { return m_fAirRunSpeed; }
	_float Get_AirRunSpeed2D() { return m_f2DAirRunSpeed; }
	_float Get_MoveSpeed(COORDINATE _eCoord) { return COORDINATE_2D == _eCoord ? m_f2DMoveSpeed : m_f3DMoveSpeed; }
	_float Get_PickupRange(COORDINATE _eCoord) { return COORDINATE_2D == _eCoord ? m_f2DPickupRange : m_f3DPickupRange; }	_float Get_AttackDamg() { return m_tStat.fDamg; }
	_float Get_3DFloorDistance() { return m_f3DFloorDistance; }
	_float Get_2DFloorDistance() { return m_f2DFloorDistance; }
	_uint Get_SpinAttackLevel() { return m_iSpinAttackLevel; }
	_float Get_2DAttackForwardingSpeed() { return m_f2DAttackForwardSpeed; }
	_vector Get_CenterPosition();
	_vector  Get_HeadPosition();
	_vector Get_LookDirection();
	_vector Get_LookDirection(COORDINATE _eCoord);
	_vector Get_3DTargetDirection() { return m_v3DTargetDirection; }
	_vector Get_ClamberEndPosition() { return m_vClamberEndPosition; }
	_vector Get_WallNormal() { return m_vWallNormal; }

	const _float4x4* Get_CarryingOffset_Ptr(COORDINATE _eCoord) { return COORDINATE_2D == _eCoord ? &m_mat2DCarryingOffset : &m_mat3DCarryingOffset; }
	STATE Get_CurrentStateID();
	E_DIRECTION Get_2DDirection() { return m_e2DDirection_E; }
	PLAYER_MODE Get_PlayerMode() { return m_ePlayerMode; }
	CController_Transform* Get_Transform() { return m_pControllerTransform; }
	CCarriableObject* Get_CarryingObject();


	//Set
	void Switch_Animation(_uint _iAnimIndex);
	void Set_Animation(_uint _iAnimIndex);
	void Set_State(STATE _eState);
	void Set_Mode(PLAYER_MODE _eNewMode);
	void Set_2DDirection(E_DIRECTION _eEDir);
	void Set_3DTargetDirection(_fvector _vDir);
	void Set_WallNormal(_fvector _vNormal) { m_vWallNormal = _vNormal; }
	void Set_ClamberEndPosition(_fvector _vPos) { m_vClamberEndPosition = _vPos; }
	void Set_SwordGrip(_bool _bForehand);
	void Set_Kinematic(_bool _bKinematic);
	HRESULT Set_CarryingObject(CCarriableObject* _pCarryingObject);


	void Start_Attack(ATTACK_TYPE _eAttackType);
	void End_Attack();
	void Flush_AttckedSet() { m_AttckedObjects.clear(); }
	void Equip_Part(PLAYER_PART _ePartId);
	void UnEquip_Part(PLAYER_PART _ePartId);

private:


	void					Key_Input(_float _fTimeDelta);

private:
	HRESULT					Ready_Components();
	HRESULT					Ready_PartObjects();
private:
	//Variables
	_float m_f3DCenterYOffset = 0.5;
	_float m_fHeadHeight = 1.0;
	_float m_fArmHeight = 0.5f; // 벽타기 기준 높이
	_float m_fArmLength = 0.325f;// 벽 타기 범위
	_float m_fFootLength = 0.25;
	_float m_fAttackForwardingForce = 12.f;
	_float m_fGroundRotateSpeed = 360.f;
	_float m_fStepSlopeThreshold = 0.1f;
	_float m_f3DLandAnimHeightThreshold= 0.6f;
	//_float m_fFootHeightThreshold = 0.1f;
	_float m_f3DJumpPower = 11.f;
	_float m_fAirRotateSpeed = 40.f;
	_float m_fAirRunSpeed = 10.f;
	_float m_f3DMoveSpeed= 10.f;
	_float m_f3DFloorDistance = 0;
	_float m_f3DThrowObjectPower = 20.f;
	_float m_f3DPickupRange = 1.3f;
	_float m_f3DKnockBackPower = 100.f;
	_bool m_bOnGround = false;
	_bool m_bAttackTrigger = false;
	_uint m_iSpinAttackLevel = 4;
	_vector m_vClamberEndPosition = { 0,0,0,1 };//벽타기 끝날 위치
	_vector m_vWallNormal= { 0,0,1,0 };//접촉한 벽의 법선
	_vector m_v3DTargetDirection = { 0,0,-1 };
	_float4x4 m_mat3DCarryingOffset = {};
	PLAYER_MODE m_ePlayerMode = PLAYER_MODE_NORMAL;

	//2D전용
	_float m_f2DAttackForwardSpeed = 700.f;
	_float m_f2DUpForce = 0;
	_float m_f2DFloorDistance = 0;
	_float m_f2DMoveSpeed= 400.f;
	_float m_f2DJumpPower = 800.f;
	_float m_f2DCenterYOffset= 36.f;
	_float m_f2DInteractRange = 93.f;
	_float m_f2DThrowObjectPower = 100.f;
	_float m_f2DPickupRange = 93.f;
	_float m_f2DKnockBackPower = 100.f;
	_float4x4 m_mat2DCarryingOffset = {};
	_bool m_bPlatformerMode = false;
	ATTACK_TYPE m_eCurAttackType = ATTACK_TYPE_NORMAL1;
	ATTACK_TRIGGER_DESC_2D m_f2DAttackTriggerDesc[ATTACK_TYPE_LAST];// = { 93.f, 93.f, 120.f };
	//ATTACK_TRIGGER_DESC_2D m_f2DAttackAngle[ATTACK_TYPE_LAST];// = { 110.f, 110.f,45.f };
	_float m_f2DAirRunSpeed = 300.f;
	E_DIRECTION m_e2DDirection_E = E_DIRECTION::E_DIR_LAST;
	//Components
	CStateMachine* m_pStateMachine = nullptr;
	CAnimEventGenerator* m_pAnimEventGenerator = nullptr;
	CCollider* m_pBody2DColliderCom = nullptr;
	CCollider* m_pBody2DTriggerCom = nullptr;
	CCollider* m_pAttack2DTriggerCom = nullptr;

	//Parts
	class CPlayerSword* m_pSword = nullptr;
	CModelObject* m_pBody = nullptr;
	CModelObject* m_pGlove= nullptr;
	CCarriableObject* m_pCarryingObject = { nullptr, };

	set<CGameObject*> m_AttckedObjects;
public:
	static CPlayer*		Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject*	Clone(void* _pArg) override;
	virtual void			Free() override;

};
END
