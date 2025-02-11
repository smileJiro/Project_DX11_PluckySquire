#pragma once
#include "Map_Tool_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Map_Tool)

class CMap_Tool_MainApp final : public CBase
{
private:
	CMap_Tool_MainApp();
	virtual ~CMap_Tool_MainApp() = default;
public:
	HRESULT				Initialize();
	void				Progress(_float _fTimeDelta);

	HRESULT				Render();

private :
	void	Set_EffectRG();

private:
	CGameInstance*			m_pGameInstance = nullptr;
	CImguiLogger*			m_pLogger =  nullptr;

	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pContext = nullptr;

private:
	HRESULT SetUp_StartLevel(LEVEL_ID _eLevelID);

	HRESULT Ready_RenderTargets();
	HRESULT Ready_RenderGroup();
	void Imgui_FPS(_float _fTimeDelta);

public:
	static CMap_Tool_MainApp* Create();
	virtual void Free() override;
};

END
