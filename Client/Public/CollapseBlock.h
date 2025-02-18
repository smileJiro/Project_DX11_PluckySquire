#pragma once
#include "2DMapObject.h"

BEGIN(Client)
class CCollapseBlock final : public C2DMapObject
{

public:
	enum STATE { STATE_IDLE, STATE_SHAKE, STATE_DROP, STATE_LAST };
private:
	CCollapseBlock(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CCollapseBlock(const CCollapseBlock& _Prototype);
	virtual ~CCollapseBlock() = default;

public:
	virtual HRESULT					Initialize_Prototype() override;
	virtual HRESULT					Initialize(void* _pArg) override;
	virtual void					Priority_Update(_float _fTimeDelta) override;
	virtual void					Update(_float _fTimeDelta) override;
	virtual void					Late_Update(_float _fTimeDelta) override;
	virtual HRESULT					Render() override;

public:
	virtual void					On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject);
	virtual void					On_Collision2D_Stay(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject);
	virtual void					On_Collision2D_Exit(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject);

private:
	STATE							m_ePreState = STATE::STATE_LAST;
	STATE							m_eCurState = STATE::STATE_LAST;

private: /* Origin Position */
	_float2							m_vOriginPostion = {};	/* Shake 시작 시, OriginPosition 저장 후, OriginPos + ShakeOffset 으로 최종 위치 결정. */

private: /* Shake Parameters */
	_float2							m_vShakeTime = { 0.75f, 0.0f };	/* 중력 컴포넌트를 켜는 시간 이면서, 부들부들 거리는 시간. */
	_float2							m_vShakeCycleTime = { 0.02f, 0.0f };	/* 중력 컴포넌트를 켜는 시간 이면서, 부들부들 거리는 시간. */
	_float2							m_vShakeStrength = { 10.0f, 5.0f};
	_float2							m_vShakeOffset = {};	/* 랜덤 값으로 카메라 셰이크처럼 좌표 흔들기. */

private: /* Drop Parameters */
	_float2							m_vDropTime = { 3.0f, 0.0f };

private: /* 상태 변경 루틴 */
	void							State_Change();
	void							State_Change_Idle();		// 기본 상태
	void							State_Change_Shake();	// 부숴지는 상태
	void							State_Change_Drop();		// 떨어지는 상태 >> 일정이상 떨어지면, 사라지고, 일정 시간 뒤 다시 Idle로 가고 OriginPos로 이동. 

private: /* 상태 실행 루틴 */
	void							Action_State(_float _fTimeDelta);
	void							Action_Idle(_float _fTimeDelta);
	void							Action_Shake(_float _fTimeDelta);
	void							Action_Drop(_float _fTimeDelta);

private:
	HRESULT							Ready_Components();

public:
	static CCollapseBlock*			Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CGameObject*					Clone(void* _pArg) override;
	void							Free() override;
};
END
