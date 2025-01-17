#pragma once
#include "Base.h"


/* 객체들을 레이어 별로 묶어서 다수 보관한다. */

BEGIN(Engine)
class CComponent;
class CLayer final : public CBase
{
private:
	CLayer();
	virtual ~CLayer() = default;

public:
	HRESULT Add_GameObject(class CGameObject* pGameObject);
	void Priority_Update(_float fTimeDelta);
	void Update(_float fTimeDelta);
	void Late_Update(_float fTimeDelta);
public:
	CComponent* Find_Component(const _wstring& _strComponentTag, _uint _iObjectIndex = 0);
	CComponent* Find_Part_Component(_uint _iPartObjectIndex, const _wstring& _strPartComponentTag, _uint _iObjectIndex = 0);
public:
	// Get
	const list<class CGameObject*>& Get_GameObjects() { return m_GameObjects; }
	CGameObject* Get_GameObject_Ptr(_int _iObjectIndex);
private:
	list<class CGameObject*>			m_GameObjects{};

public:
	static CLayer* Create();
	virtual void Free() override;
};

END