#pragma once
#include "Camera_Tool_Defines.h"

#include "Base.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Camera_Tool)

class CMainApp final : public CBase
{
private:
	CMainApp();
	virtual ~CMainApp() = default;
public:
	HRESULT				Initialize();
	void				Progress(_float _fTimeDelta);

	HRESULT				Render();


private:
	CGameInstance*			m_pGameInstance = nullptr;
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pContext = nullptr;

private:
	HRESULT SetUp_StartLevel(LEVEL_ID _eLevelID);

public:
	static CMainApp* Create();
	virtual void Free() override;
};

END
