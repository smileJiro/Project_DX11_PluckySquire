#pragma once
#include "ModelObject.h"
#include "Stoppable.h"

BEGIN(Client)

class CSaw final : public CModelObject, public IStoppable
{
public:
	enum STATE { STATE_HIDE, STATE_ATTACK, STATE_STOP, STATE_LAST };
	enum ANIM 
	{ 
		SAW_SPIN, 
		SAW_STOPPABLE, 
		// EFFECT
		SAW_DUST_1, 
		SAW_DUST_2,
		SAW_DUST_3,
		SAW_DUST_4,
		SAW_DUST_5,
		SAW_EFFECT_LOOP,
	};
	typedef struct tagSawDesc : public CModelObject::MODELOBJECT_DESC
	{

	}SAW_DESC;
private:
	CSaw(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CSaw(const CSaw& _Prototype);
	virtual ~CSaw() = default;

public:
	virtual HRESULT Initialize_Prototype(); // 프로토 타입 전용 Initialize
	virtual HRESULT Initialize(void* _pArg); // 초기화 시 필요한 매개변수를 void* 타입으로 넘겨준다.
	virtual void	Priority_Update(_float _fTimeDelta) override;
	virtual void	Update(_float _fTimeDelta) override;
	virtual void	Late_Update(_float _fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	virtual void On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)override;
	virtual void On_Collision2D_Stay(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)override;
	virtual void On_Collision2D_Exit(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)override;
public:
	_int Get_AttackCount() const { return m_iAttackCount; }
	void Set_CurState(STATE _eState) { m_eCurState = _eState; State_Change(); }

public:
	_bool Is_Stoppable() { return m_eCurState == STATE_STOP; }
private:
	STATE m_ePreState = STATE::STATE_LAST;
	STATE m_eCurState = STATE::STATE_LAST;

private:
	_float2 m_vStartPos = {};
	_float2 m_vMinMaxOffset_X = { -500.f, 500.f };
	_float2 m_vMinMaxOffset_Y = { -70.f, 170.f };
	_float2 m_vOffset = { 0.0f, 170.f };
	_float2 m_vMoveSpeed = { 300.f, 50.f };
	_int m_iAttackCount = 0;
private:
	void State_Change();
	void State_Chage_Hide();
	void State_Change_Attack();
	void State_Change_Stop();

private:
	void Action_State(_float _fTimeDelta);
	void Action_State_Hide(_float _fTimeDelta);
	void Action_State_Attack(_float _fTimeDelta);
	void Action_State_Stop(_float _fTimeDelta);

private:
	void				On_Stop() override;	   // 자기 partobject
	void				On_UnStop() override;  // 자기 partobject

private:
	HRESULT	Ready_Components(SAW_DESC* _pDesc);
public:
	static CSaw* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CGameObject* Clone(void* _pArg);
	void Free() override;
};

END