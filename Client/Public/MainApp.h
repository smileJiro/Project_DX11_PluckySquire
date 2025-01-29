#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)

class CMainApp final : public CBase
{
private:
	CMainApp();
	virtual ~CMainApp() = default;

public:
	HRESULT Initialize();
	void	Progress(_float _fTimeDelta);
	HRESULT Render();

public:
	void Set_OneFrameDeltaTime(_float _iDeltaTime) { m_iOneFrameDeltaTime = _iDeltaTime; }
private:
	CGameInstance* m_pGameInstance = nullptr;
	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pContext = nullptr;

private:
	void Imgui_FPS(_float _fTimeDelta);
private:
	_float2 m_vFPSRenderTime = { 1.0f, 0.0f };
	_float m_iOneFrameDeltaTime = 0.0f;
private:
	//HRESULT Loading_ShaderMaterials_PathFind(const _wstring& strDirectoryPath);
	//HRESULT Loading_BGM_PathFind(const _wstring& strDirectoryPath);
	//HRESULT Loading_SFX_PathFind(const _wstring& strDirectoryPath);
	HRESULT SetUp_StartLevel(LEVEL_ID _eLevelID);
	
private:
	HRESULT Initialize_Client_Manager();
	HRESULT Ready_Font();
	HRESULT Ready_RenderTargets();
	HRESULT Ready_RenderGroup();

public:
	static CMainApp* Create();
	virtual void Free() override;
};

END

/*
class C이름
{
private: or protected:
생성자, 소멸자.

public:
함수
protected:
변수
protected:
함수
private:
변수
private:
함수

public:
생성관련된 함수(Create, Clone)
삭제관련된 함수(Free)
};

*/