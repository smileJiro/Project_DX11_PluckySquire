#pragma once
#include "ModelObject.h"
BEGIN(Client)
class CPlayerSword :
    public CModelObject
{
public:
	enum SWORD_STATE
	{
		HANDLING,
		FLYING,
		STUCK,
		SWORD_STATE_LAST
	};
public:
	typedef struct tagPlayerSwordDesc : public CModelObject::MODELOBJECT_DESC
	{
		_float fThrowForce = 10.f;
		_float fHomingForce = 100.f;
		_float fRotationForce = 50.f;
		class CPlayer* pParent = nullptr;
	}PLAYER_SWORD_DESC;
private:
	CPlayerSword(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CPlayerSword(const CPlayerSword& _Prototype);
	virtual ~CPlayerSword() = default;

public:
	virtual HRESULT Initialize(void* _pArg) override;
	virtual void			Update(_float _fTimeDelta) override;
	virtual void			Late_Update(_float _fTimeDelta) override;

	virtual void OnTrigger_Enter(const COLL_INFO& _My, const COLL_INFO& _Other) override;
	virtual void OnTrigger_Stay(const COLL_INFO& _My, const COLL_INFO& _Other)override;
	virtual void OnTrigger_Exit(const COLL_INFO& _My, const COLL_INFO& _Other)override;

	void Throw(_fvector _vDirection);
	_bool Is_Flying() { return FLYING == m_eCurrentState; }
	_bool Is_ComingBack() { return Is_Flying() && 0 > m_fOutingForce; }
	_bool Is_Outing() { return Is_Flying() && 0 < m_fOutingForce; }
	_bool Is_SwordHandling() { return HANDLING == m_eCurrentState; }
	void Switch_Grip(_bool _bForehand);

	void Set_State(SWORD_STATE _eNewState);
	void On_StateChange();

	//Set
	void Set_AttackEnable(_bool _bOn);
private:
	_float m_fThrowingPower = 15.f;
	_vector m_vThrowDirection = {};
	_vector m_vStuckDirection = {};
	//밖으로 나가려는 힘
	_float m_fOutingForce = 1.f;
	//끌어들이는 힘
	_float m_fCentripetalForce = 30.f;
	_float m_fRotationForce = 50.f;
	class CPlayer* m_pPlayer = nullptr;

	SWORD_STATE m_eCurrentState = HANDLING;
	SWORD_STATE m_ePastState = HANDLING;
	//무기가 날아가는 중에만 true가 됨.
	//_bool m_IsFlying = false;
	//던진 무기를 받은 프레임에만 true가 됨.
	//_bool m_IsReceive = false;
	//무기가 벽에 박혔을 때 true
	//_bool m_IsStuck = false;
public:
	static CPlayerSword* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void			Free() override;
};

END