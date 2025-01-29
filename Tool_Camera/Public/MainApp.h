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

public:
	void				Set_OneFrameDeltaTime(_float _iDeltaTime) { m_iOneFrameDeltaTime = _iDeltaTime; }

private:
	CGameInstance*			m_pGameInstance = nullptr;
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pContext = nullptr;

private:
	class CCamera_Manager_Tool*		m_pCamera_Manager = { nullptr };

private:
	_float2					m_vFPSRenderTime = { 1.0f, 0.0f };
	_float					m_iOneFrameDeltaTime = 0.0f;

private:
	void				Imgui_FPS(_float _fTimeDelta);

private:
	HRESULT				SetUp_StartLevel(LEVEL_ID _eLevelID);

private:
	HRESULT				Ready_RenderTargets();
	HRESULT				Ready_RenderGroup();

public:
	static CMainApp* Create();
	virtual void Free() override;
};

END
