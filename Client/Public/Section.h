#pragma once
#include "Base.h"

BEGIN(Engine)
class CGameObject;
class CLayer;
class CGameInstance;
END

BEGIN(Client)
class CSection : public CBase
{
protected:
	CSection();
	virtual ~CSection() = default;

public:
	// 1. Section 자체는 마땅히 할게 없다. Section_2D는 별도의 추가작업이 있겠지. 
	virtual HRESULT Initialize();

public:
	// 1. Section Layer에 Object를 추가하는 기능.
	HRESULT Add_GameObject_ToSectionLayer(CGameObject* _pGameObject);
	// 2. Section Layer에 Object의 Active를 변경하는 기능.
	HRESULT SetActive_GameObjects(_bool _isActive);
	// 3. Section Layer에 Object를 Renderer의 자신의 그룹에 Add 하는 기능.
	HRESULT Add_RenderGroup_GameObjects();
	// 4. Section Layer에 Object의 DeadCheck를 하는 기능.
	HRESULT Cleanup_DeadReferences();
	// 5. Section Layer를 Clear 하는 기능.
	void	Clear_GameObjects();


private:
	CGameInstance* m_pGameInstance = nullptr;

private:
	_wstring m_strName;
	CLayer* m_pLayer = nullptr;

private: /* Initialize Method */

public:
	static CSection* Create();
	void Free() override;
};

END