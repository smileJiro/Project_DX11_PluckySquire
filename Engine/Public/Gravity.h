#pragma once
#include "Component.h"
BEGIN(Engine)
class CGameObject;
class ENGINE_DLL CGravity final: public CComponent
{
public:
	typedef struct tagGravityDesc
	{
		CGameObject* pOwner = nullptr;
		_float3 vGravityDirection = { 0.0f, -1.0f, 0.0f };
		_float fGravity = 9.8f;
		_float fMaxGravityAcc = 1000.f;
	}GRAVITY_DESC;
public:
	enum STATE { STATE_FLOOR, STATE_FALLDOWN ,STATE_LAST };
private:
	CGravity(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CGravity(const CGravity& _Prototype);
	virtual ~CGravity() = default;

public:
	virtual HRESULT				Initialize_Prototype();
	virtual HRESULT				Initialize(void* _pArg);
	virtual void				Priority_Update(_float _fTimeDelta);

public:
	void						Change_State(STATE _eState);

private:
	void						State_Change();

private:
	void						Action_State(_float _fTimeDelta);
	void						Action_Floor(_float _fTimeDelta);
	void						Action_FallDown(_float _fTimeDelta);

public:
	// Get
	STATE						Get_CurState() const { return m_eCurState; }
	const _float3&				Get_GravityDirection() const { return m_vDirection; }
	_float						Get_Gravity() const { return m_fGravity; }
	_float						Get_GravityAcc() const { return m_fGravityAcc; }

	// Set
	void						Set_GravityDirection(const _float3& _vDirection) { m_vDirection = _vDirection; }
	void						Set_Gravity(_float _fGravity) { m_fGravity = _fGravity; }
	void						Set_GravityAcc(_float _fGravityAcc) { m_fGravityAcc = _fGravityAcc; }
	void						Set_GravityOffset(_float _fGravityOffset) { m_fGravityOffset = _fGravityOffset; }


private:
	/* Active 변경시 호출되는 함수 추가. */
	virtual void Active_OnEnable();
	virtual void Active_OnDisable();

private:
	CGameObject*				m_pOwner = nullptr;
	STATE						m_ePreState = STATE::STATE_LAST;
	STATE						m_eCurState = STATE::STATE_FLOOR;
private:
	_float3						m_vDirection = {};
	_float						m_fGravity = 9.8f; // 중력가속도
	_float						m_fGravityAcc = 0.0f;
	_float						m_fMaxGravityAcc = 1000.f;
	_float						m_fGravityOffset = {};
public:
	static CGravity*			Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CComponent*					Clone(void* _pArg) override;
	void						Free() override;

};

END