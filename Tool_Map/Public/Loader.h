#pragma once
#include "Map_Tool_Defines.h"
#include "Base.h"

/* 서브 스레드를 생성한다. */
/* 다음 레벨에 필요한 자원들을 로드한다. */
BEGIN(Engine)
class CGameInstance;
END


BEGIN(Map_Tool)
class CLoader final : public CBase
{
private:
	CLoader(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CLoader() = default;

public:
	HRESULT Initialize(LEVEL_ID _eNextLevelID);
	HRESULT Loading();

#ifdef _DEBUG
public:
	void Show_Debug();
#endif
public:
	_bool isFinished() const { return m_isFinished; }

private:
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pContext = nullptr;
	LEVEL_ID				m_eNextLevelID = LEVEL_END;
	HANDLE					m_hThread = {};
	CRITICAL_SECTION		m_Critical_Section = { nullptr };
	CGameInstance*			m_pGameInstance = { nullptr };

private:
	_bool					m_isFinished = false;
	_tchar					m_szLoadingText[MAX_PATH] = {};
private:
	HRESULT Loading_Level_Static();
	HRESULT Loading_Level_Logo();
	HRESULT Loading_Level_Map_Tool();
	HRESULT Loading_Level_Trigger();

public:
	static CLoader* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, LEVEL_ID _eNextLevelID);
	virtual void Free() override;
};

END