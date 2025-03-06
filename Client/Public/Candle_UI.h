#pragma once
#include "ModelObject.h"

BEGIN(Client)
class CCandle_UI : public CModelObject
{
public:
	typedef struct tagCandleUiDesc : CModelObject::MODELOBJECT_DESC
	{
		_float fX = 0.0f;
		_float fY = 0.0f;
		_float fSizeX = 1.0f;
		_float fSizeY = 1.0f;
	}CANDLE_UI_DESC;
	enum STATE { STATE_FLAMELOOP, STATE_TURNOFF, STATE_TURNON, STATE_IDLE, STATE_LAST };
private:
	CCandle_UI(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CCandle_UI(const CCandle_UI& _Prototype);
	virtual ~CCandle_UI() = default;

public:
	virtual HRESULT			Initialize_Prototype(); // 프로토 타입 전용 Initialize
	virtual HRESULT			Initialize(void* _pArg); // 초기화 시 필요한 매개변수를 void* 타입으로 넘겨준다.
	virtual void			Priority_Update(_float _fTimeDelta) override;
	virtual void			Update(_float _fTimeDelta) override;
	virtual void			Late_Update(_float _fTimeDelta) override;
	virtual HRESULT			Render() override;

public:
	void					State_Change_Idle();		// 자체적인 State 관리용이 아닌 컨테이너 호출용 
	void					State_Change_TurnOn();		// 자체적인 State 관리용이 아닌 컨테이너 호출용 
	void					State_Change_FlameLoop();	// 자체적인 State 관리용이 아닌 컨테이너 호출용 
	void					State_Change_TurnOff();		// 자체적인 State 관리용이 아닌 컨테이너 호출용 

private:
	void					On_AnimEnd(COORDINATE _eCoord, _uint _iAnimIdx);

	
private: /* UI 처럼 렌더링을 해야해 직교투영으로 */
	_float					m_fX = 0.0f;
	_float					m_fY = 0.0f;
	_float					m_fSizeX = 100.f;
	_float					m_fSizeY = 100.f;

	_float4x4				m_ViewMatrix = {};
	_float4x4				m_ProjMatrix = {};

private:
	HRESULT					Bind_ShaderResources_WVP() override;
	void					Apply_UIPosition();
	void					Apply_UIScaling();
public:
	static CCandle_UI*		Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CCandle_UI*				Clone(void* _pArg);
	virtual void			Free();

};

END