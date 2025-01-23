#pragma once
#include "State.h"

BEGIN(Client)

//공격 없이 애니메이션만 재생되는 상태
class CBossSceneState final : public CState
{
public:
	enum SCENE { FIRST, LAST };

private:
	CBossSceneState();
	virtual ~CBossSceneState() = default;

public:
	void Set_Scene(_uint _iSceneIdx) 
	{
		m_iSceneIdx = _iSceneIdx;
	}

public:
	virtual HRESULT Initialize(void* _pArg) override;

public:
	virtual void State_Enter() override;
	virtual void State_Update(_float _fTimeDelta) override;
	virtual void State_Exit() override;

private:
	//초기에는 첫 컷씬 애니메이션 재생토록 하고 FSM을 통해 set함수로 전환됨
	_uint m_iSceneIdx = { 0 };

public:
	static CBossSceneState* Create(void* _pArg);

public:
	virtual void Free() override;
};

END