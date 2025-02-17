#pragma once
#include "ModelObject.h"

BEGIN(Engine)
class C2DModel;
END

BEGIN(Client)
class CFallingRock final : public CModelObject
{
public:
	typedef struct tagFallingRockDesc : CModelObject::MODELOBJECT_DESC
	{
		_float fFallDownEndY = {};
	}FALLINGROCK_DESC;

public:
	enum STATE { STATE_FALLDOWN, STATE_BOUND_2D, STATE_BOUND_3D, STATE_LAST };

private:
	CFallingRock(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CFallingRock(const CFallingRock& _Prototype);
	virtual ~CFallingRock() = default;

public:
	virtual HRESULT			Initialize_Prototype(); // 프로토 타입 전용 Initialize
	virtual HRESULT			Initialize(void* _pArg); // 초기화 시 필요한 매개변수를 void* 타입으로 넘겨준다.
	virtual void			Priority_Update(_float _fTimeDelta) override;
	virtual void			Update(_float _fTimeDelta) override;
	virtual void			Late_Update(_float _fTimeDelta) override;
	virtual HRESULT			Render() override;

public:
	virtual void			On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject);
	virtual void			On_Collision2D_Stay(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject);
	virtual void			On_Collision2D_Exit(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject);

private:
	STATE					m_ePreState = STATE::STATE_LAST;
	STATE					m_eCurState = STATE::STATE_FALLDOWN;

private: /* FallDown */
	_float					m_fFallDownEndY;

private: /* Bound */
	_float2					m_vJumpTime = { 0.5f, 0.0f };
	_float					m_fJumpForcePerSec = 500.f; // 처음엔 강하다가 점점 약해지겠지; 중력때문에

private: /* Coord3D Change */
	_float					m_fCoordChangePosY = -200.f;
	_float					m_fForce3D = 7.5f;
	_float2					m_fDeadTime = { 3.0f, 0.0f };
private:/* Shadow Position Data : 설계미스로 좀 별로인 방식 채택함 참고하지말 것*/
	C2DModel*				m_p2DShadowModelCom = nullptr;
	_float3					m_vShadowYDesc = {}; // x: StartY, y : CurrentY, z : TargetY

private:
	void					State_Change();
	void					State_Change_FallDown();
	void					State_Change_Bound_2D();
	void					State_Change_Bound_3D();

private:
	void					Action_State(_float _fTimeDelta);
	void					Action_FallDown(_float _fTimeDelta);
	void					Action_Bound_2D(_float _fTimeDelta);
	void					Action_Bound_3D(_float _fTimeDelta);
	
private:
	HRESULT					Ready_Components(FALLINGROCK_DESC* _pDesc);

private:
	/* Active 변경시 호출되는 함수 추가. */
	virtual void Active_OnEnable();
	virtual void Active_OnDisable();
public:
	static CFallingRock*	Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CGameObject*			Clone(void* _pArg) override;
	void					Free() override;
};

END