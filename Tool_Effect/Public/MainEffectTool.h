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
	HRESULT Ready_Prototype_Static();

	

public:
	static CMainEffectTool* Create();
	virtual void Free() override;

#ifdef _DEBUG
private:
	_float	m_fAccTime = 0.f;
	_int	m_iFrame = 0;
	_int	m_iRenderFrame = 0;
private:
	void	Debug_Default(_float _fTimeDelta);
#endif
};

