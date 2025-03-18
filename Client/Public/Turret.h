#pragma once
#include "ModelObject.h"

BEGIN(Client)
class CTurret final : public CModelObject
{
public:
	enum STATE { STATE_CLOSE, STATE_LOWER, STATE_OPEN, STATE_RISE, STATE_FIRE, STATE_FIRE_INTO, STATE_LAST };
	typedef struct tagTurretDesc : public CModelObject::MODELOBJECT_DESC
	{

	}TURRET_DESC;
private:
	CTurret(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CTurret(const CTurret& _Prototype);
	virtual ~CTurret() = default;

public:
	virtual HRESULT Initialize_Prototype(); // 프로토 타입 전용 Initialize
	virtual HRESULT Initialize(void* _pArg); // 초기화 시 필요한 매개변수를 void* 타입으로 넘겨준다.
	virtual void	Priority_Update(_float _fTimeDelta) override;
	virtual void	Update(_float _fTimeDelta) override;
	virtual void	Late_Update(_float _fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Open_Turret() { m_eCurState = STATE_RISE; State_Change(); }
	void Close_Turret() { m_eCurState = STATE_LOWER; State_Change(); }

private:
	STATE		m_ePreState = STATE::STATE_LAST;
	STATE		m_eCurState = STATE::STATE_LAST;

private:
	_float2		m_vBombCoolTime = { 4.0f, 0.0f };

private:
	void State_Change(); 
	void State_Change_Close();		// 항상 닫혀있는
	void State_Change_Lower();		// 닫히는 모션
	void State_Change_Open();		// 항상 열려있는
	void State_Change_Rise();		// 열리는 모션
	void State_Change_Fire();		// 발사 
	void State_Change_Fire_Into();	// 긴 발사 준비 

private:
	void Action_State(_float _fTimeDelta);
	void Action_State_Close(_float _fTimeDelta); // 아무것도 안함.
	void Action_State_Lower(_float _fTimeDelta); 
	void Action_State_Open(_float _fTimeDelta); // 열려있는 상태 >>> 대포 발사 쿨타임 계산후 fire into로 변경
	void Action_State_Rise(_float _fTimeDelta); 
	void Action_State_Fire(_float _fTimeDelta); // 대포 생성 
	void Action_State_Fire_Into(_float _fTimeDelta); 

private:
	void					On_AnimEnd(COORDINATE _eCoord, _uint _iAnimIdx);

public:
	static CTurret* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CGameObject* Clone(void* _pArg);
	void Free() override;
};

END