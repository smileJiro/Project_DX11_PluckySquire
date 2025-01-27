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
public:
	typedef struct tagSectionDesc
	{
		_wstring strSectionName;
	}SECTION_DESC;
protected:
	CSection(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CSection() = default;

public:
	// 1. Section 자체는 마땅히 할게 없다. Section_2D는 별도의 추가작업이 있겠지. 
	virtual HRESULT Initialize();

public: /* Object Layer와의 상호 작용 */
	// 1. Section Layer에 Object를 추가하는 기능. (o)
	HRESULT Add_GameObject_ToSectionLayer(CGameObject* _pGameObject);

private:
	// 2. Section Layer에 Object의 Active를 변경하는 기능. (o)
	HRESULT SetActive_GameObjects(_bool _isActive);

public:
	// 3. Section Layer에 Object를 Renderer의 자신의 그룹에 Add 하는 기능.(x)
	HRESULT Add_RenderGroup_GameObjects();
	// 4. Section Layer에 Object의 DeadCheck를 하는 기능. (o)
	HRESULT Cleanup_DeadReferences();
	// 5. Section Layer를 Clear 하는 기능. (o)
	void	Clear_GameObjects();


protected:
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pContext = nullptr;
	CGameInstance*			m_pGameInstance = nullptr;

protected:
	_wstring				m_strName;
	CLayer*					m_pLayer = nullptr;

private:
	/* Section은 update 기반이 아니다. 즉 본인이 Active False여도 기능수행 가능. */
	virtual void Active_OnEnable();
	virtual void Active_OnDisable();

public:
	static CSection* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	void Free() override;
};

END