#pragma once
#include "Base.h"
#include "EffectTool_Defines.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(EffectTool)

class CEffectToolLoader : public CBase
{
private:
	CEffectToolLoader(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CEffectToolLoader() = default;

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
	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pContext = nullptr;
	LEVEL_ID				m_eNextLevelID = LEVEL_END;
	HANDLE					m_hThread = {};
	CRITICAL_SECTION		m_Critical_Section = { nullptr };
	CGameInstance* m_pGameInstance = { nullptr };

private:
	_bool					m_isFinished = false;
	_tchar					m_szLoadingText[MAX_PATH] = {};
private:
	HRESULT Loading_Level_Tool();


public:
	static CEffectToolLoader* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, LEVEL_ID _eNextLevelID);
	virtual void Free() override;
};

END