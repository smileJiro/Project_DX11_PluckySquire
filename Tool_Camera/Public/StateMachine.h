#pragma once
#include "Component.h"
#include "PlayerState.h"

BEGIN(Camera_Tool)
class CTest_Player;
class CTransition;
class CPlayerState;
class  CStateMachine :
    public CComponent
{
public:
	typedef struct tagStateMachineDesc
	{
		CTest_Player* pOwner = nullptr;
	}STATEMACHINE_DESC;
protected:
	CStateMachine(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CStateMachine(const CStateMachine& Prototype);
	virtual ~CStateMachine() = default;

public:
	HRESULT Initialize_Prototype();
	void Update(_float fTimeDelta);
	virtual HRESULT Initialize(void* pArg) override;


	void Register_OnStateChangeCallBack(const function<void(CPlayerState*)>& fCallback) { m_listStateChangeCallback.push_back(fCallback); }
	
	//Get
	CPlayerState* Get_CurrentState() { return m_pState; }
	//Set
	void Transition_To(CPlayerState* _pNewState);

private:
	CPlayerState* m_pState = nullptr;

	list<function<void(CPlayerState*)>> m_listStateChangeCallback;

	CTest_Player* m_pOwner = nullptr;
public:
	static CStateMachine* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);

	virtual CComponent* Clone(void* _pArg) override;
	virtual void Free() override;

};

END