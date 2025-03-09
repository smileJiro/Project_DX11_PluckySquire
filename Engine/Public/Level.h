#pragma once
#include "Base.h"

BEGIN(Engine)
class CUI;
class ENGINE_DLL CLevel abstract : public CBase
{

protected:
	CLevel(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CLevel() = default; 

public:
	virtual HRESULT Initialize();
	virtual void Update(_float _fTimeDelta);
	// 실질적인 Render는 Renderer 클래스에서 수행함. 
	// Debug 용 코드 작성.
	virtual HRESULT Render(); 
	
protected:
	// Device, Context는 여기저기 뿌려줘야하기에 해당 포인터를 소유.
	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pContext = nullptr;
	class CGameInstance* m_pGameInstance = nullptr; // class 를 붙이는 이유, 일종의 전방선언.
protected:
	HRESULT Ready_DeafultCameraSet();
public:
	virtual void Free() override; // virtual : 가상 함수다, override : 재정의 한다.
	// virtual : Base class 에서 함수를 가상 함수로 선언하기 위함.
	// Derived class 에서 함수를 재정의 할 수 있도록 하는 역할을 한다. 
	
	// override : Derived class 에서 Base class 의 가상 함수를 올바르게 재정의 하고 있다는 것을 명시적으로 나타냄.
	// 개발자가 재정의 하고자 하는 함수가 실제로 기반 클래스의 가상 함수와 일치하는지 컴파일러가 확인하도록 도와준다.
	// 해당 키워드를 통해 실수를 방지할 수 있다.
};

END