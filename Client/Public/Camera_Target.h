#pragma once
#include "Camera.h"

BEGIN(Client)
class CCamera_Target  : public CCamera
{

public:
	typedef struct tagCameraTargetDesc : public CCamera::CAMERA_DESC
	{
		// Target Object 
		CGameObject*	pTarget;
		// 룩벡터를 어떤 축으로 회전시킬 것 인가? (기본값 : x 축 넣으면 됨)
		_float3			vArmRotAxis = {};
		// vArmRotAxis로 몇도 만큼 돌릴 것 인가.
		_float			fArmAngle = 0.0f;
		// TargetObject와의 거리
		_float			fDistance = 0.0f;
		// 마우스 감도.
		_float			fMouseSensor = 0.0f;
	}CAMERA_TARGET_DESC;
protected:
	CCamera_Target(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CCamera_Target(const CCamera_Target& _Prototype);
	virtual ~CCamera_Target() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* _pArg) override;
	virtual void Priority_Update(_float _fTimeDelta) override;
	virtual void Update(_float _fTimeDelta) override;
	virtual void Late_Update(_float _fTimeDelta) override;

public: /* Default */
	HRESULT Change_Target(CGameObject* _pNewTarget);
	HRESULT Change_SubTarget(CGameObject* _pNewSubTarget);
	virtual void MoveToTarget(_float _fTimeDelta);
	void MakeArm();
	void MoveToSubTarget(_float _fTimeDelta);
	void Release_SubTarget();
	void AutoRotaionCameraArm(_float _fTimeDelta);
	void MouseMove(_float _fTimeDelta);

public: /* Rotation */
	void TurnArmY(_float _fSpeed);
	void TurnArmX(_float _fSpeed);
	void RotationY(_float _fRadian);
	void Set_TurnTargetRadian_Y(_float _fRadian, _float _fTurnSpeed);
	void Turn_TargetY(_float _fTimeDelta); /* 목표 회전 각도까지 회전시키게*/
	void Set_TurnTargetRadian_X(_float _fRadian, _float _fTurnSpeed);
	void Turn_TargetX(_float _fTimeDelta); /* 목표 회전 각도까지 회전시키게*/


public: /* Assassin CutScene */
	void Start_Assassinate();
	void End_Assassinate();
	void Action_Assassinate(_float _fTimeDelta);

public:
	void Set_TargetOffset(const _float3& _vTargetOffset, _float _fTime);
	void Action_OffsetMove(_float _fTimeDelta);
public:
	// Get 
	const _vector Get_TargetCamLook() { return m_pControllerTransform->Get_State(CTransform::STATE_LOOK); }
protected:
	CGameObject*	m_pTarget = nullptr;
	CGameObject*	m_pSubTarget = nullptr;
	_float4x4		m_ArmRotMatrixY = {};
	_float4x4		m_ArmRotMatrixX = {};

	_float3			m_vArm = { 0.0f, 1.5f, 0.0f }; 
	_float3			m_vArmRotAxis = {};
	_float			m_fArmAngle = 0.0f;
	_float			m_fDistance = 0.0f;
	_float			m_fMouseSensor = 0.0f;

	/* SubTarget 과의 관계 */
	_float			m_fSubTargetMinDistance = 1.5f;
	_float			m_fSubTargetStandardDistance = 4.0f;
	_bool			m_isTurnSubTargetDistance = false;

	/* Smooth Camera */
	_float			m_fAtTime = 0.2f;
	_float			m_fAtTimeAcc = 0.0f;
	_float			m_fAtSpeed = 0.0f;
	_float3			m_vPrevAt = {};
	_float3			m_vCurAt = {};
	_float3			m_vTargetAt = {};

	/* Target Turn */
	_bool			m_isTurnFinishedY = true;
	_float2			m_vTargetRadianY = { 0.0f, 0.0f };
	_float			m_fTurnSpeedY = 1.0f;
	/* Target Turn */
	_bool			m_isTurnFinishedX = true;
	_float2			m_vTargetRadianX = { 0.0f, 0.0f };
	_float			m_fTurnSpeedX = 1.0f;

	/* Target Offset */
	_bool			m_isOffsetMoveFinished = true;
	_float3			m_vTargetOffset = { 0.0f, 0.0f, 0.0f };
	_float2			m_vOffsetMoveTime = { 0.0f, 0.0f };
	_float3			m_vMoveSpeed = { 0.0f, 0.0f, 0.0f };

	/* Assassinate */
	_int			m_iAssassinate = 0;


	/* First Check */
	_bool m_isFirst = true;
protected:
	void Compute_AtSpeed();
public:
	static CCamera_Target* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg);
	virtual void Free() override;
	virtual HRESULT Safe_Release_DeadObjects(); // 참조 중인 게임오브젝트들 중 죽은 Dead상태인 오브젝트를 체크해서 참조해제.(액티브 false인 애들때매 만듬)

};

END
