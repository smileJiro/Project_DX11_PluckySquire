#pragma once
#include "Base.h"


/* 객체들을 레이어 별로 묶어서 다수 보관한다. */

BEGIN(Engine)
class CComponent;
class CGameInstance;
class ENGINE_DLL CLayer final : public CBase
{
private:
	CLayer();
	virtual ~CLayer() = default;

public:
	HRESULT		Add_GameObject(class CGameObject* _pGameObject);
	HRESULT		Remove_GameObject(class CGameObject* _pGameObject);
	CComponent* Find_Component(const _wstring& _strComponentTag, _uint _iObjectIndex = 0);
	CComponent* Find_Part_Component(_uint _iPartObjectIndex, const _wstring& _strPartComponentTag, _uint _iObjectIndex = 0);

public:
	// 자기 자신 Layer를 순회하며 Dead Object에 대해 Erase 하는 기능. 
	void		Cleanup_DeadReferences(); 
	void		SetActive_GameObjects(_bool _isActive);
	void		Clear_GameObjects();
	void		Check_FrustumCulling();
	template <typename Comparator>
	void		Sort_Objects(Comparator _funcCompair)
	{
		m_GameObjects.sort(_funcCompair);
	}
public:
	// Get
	const list<class CGameObject*>& Get_GameObjects() { return m_GameObjects; }
	CGameObject* Get_GameObject_Ptr(_int _iObjectIndex);
private:
	CGameInstance* m_pGameInstance = nullptr;
private:
	_wstring							m_strName{}; // 이름추가하고, 디버깅좀하자. TODO:: 0224
	list<class CGameObject*>			m_GameObjects{};

private: /* private : CObject_Manager 에서만 접근 가능하게 제한. */
	void Priority_Update(_float fTimeDelta);
	void Update(_float fTimeDelta);
	void Late_Update(_float fTimeDelta);

public:
	static CLayer* Create();
	virtual void Free() override;

	friend class CObject_Manager;
};

END