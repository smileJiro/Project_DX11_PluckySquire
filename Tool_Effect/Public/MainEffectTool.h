#pragma once
#include "EffectTool_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CGameInstance;
END

class CMainEffectTool : public CBase
{
private:
	CMainEffectTool();
	virtual ~CMainEffectTool() = default;

public:
	HRESULT Initialize();
	void	Progress(_float _fTimeDelta);
	HRESULT Render();

private:
	CGameInstance* m_pGameInstance = nullptr;
	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pContext = nullptr;

private:
	HRESULT SetUp_StartLevel(LEVEL_ID _eLevelID);


public:
	static CMainEffectTool* Create();
	virtual void Free() override;
};

