#pragma once

#include "Base.h"
#include "AnimTool_Defines.h"

BEGIN(Engine)
class CGameInstance;
END
BEGIN(AnimTool)
class CAnimTool_MainApp final : public CBase
{
private:
	CAnimTool_MainApp();
	virtual ~CAnimTool_MainApp() = default;

public:
	HRESULT Initialize();
	void	Progress(_float _fTimeDelta);
	HRESULT Render();

private:
	CGameInstance* m_pGameInstance = nullptr;

	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pContext = nullptr;

private:
	void	Set_EffectRG();

private:
	//HRESULT Loading_ShaderMaterials_PathFind(const _wstring& strDirectoryPath);
	//HRESULT Loading_BGM_PathFind(const _wstring& strDirectoryPath);
	//HRESULT Loading_SFX_PathFind(const _wstring& strDirectoryPath);
	HRESULT SetUp_StartLevel(LEVEL_ID _eLevelID);

	HRESULT Ready_RenderTargets();
	HRESULT Ready_RenderGroup();

public:
	static CAnimTool_MainApp* Create();
	virtual void Free() override;
};

END