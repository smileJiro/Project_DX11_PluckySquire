#pragma once
#include "ModelObject.h"
/* 1. Candle Body 는 자기 자신의 Animation 상태를 변경하는 함수를 제공(이는 모델오브젝트에서) */
/* 2. 생성 코드를 모아두는 정도. */
BEGIN(Client)
class CCandle_Body final: public CModelObject
{
private:
	CCandle_Body(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CCandle_Body(const CCandle_Body& _Prototype);
	virtual ~CCandle_Body() = default;

public:
	virtual HRESULT			Initialize_Prototype(); // 프로토 타입 전용 Initialize
	virtual HRESULT			Initialize(void* _pArg); // 초기화 시 필요한 매개변수를 void* 타입으로 넘겨준다.

private:
	HRESULT					Ready_Components();

public:
	void					State_Change_Idle();		// 자체적인 State 관리용이 아닌 컨테이너 호출용 
	void					State_Change_TurnOn();		// 자체적인 State 관리용이 아닌 컨테이너 호출용 
	void					State_Change_FlameLoop();	// 자체적인 State 관리용이 아닌 컨테이너 호출용 
	void					State_Change_TurnOff();		// 자체적인 State 관리용이 아닌 컨테이너 호출용 

public:
	static CCandle_Body*	Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CCandle_Body*	Clone(void* _pArg) override;
	virtual void			Free() override;

};
END
