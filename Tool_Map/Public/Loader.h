#pragma once
#include "Map_Tool_Defines.h"
#include "Base.h"

/* 서브 스레드를 생성한다. */
/* 다음 레벨에 필요한 자원들을 로드한다. */
BEGIN(Engine)
class CGameInstance;
END




BEGIN(Map_Tool)



class CImguiLogger;
	

class CLoader final : public CBase
{
private:
	CLoader(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CLoader() = default;

public:
	HRESULT Initialize(LEVEL_ID _eNextLevelID, CImguiLogger* _pLogger);
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
	CImguiLogger*			m_pLogger = { nullptr };

private:
	
	_bool					m_isFinished = false;
	_tchar					m_szLoadingText[MAX_PATH] = {};


private:
	HRESULT Loading_Level_Static();
	HRESULT Loading_Level_Logo();
	HRESULT Loading_Level_2D_Map_Tool();
	HRESULT Loading_Level_3D_Map_Tool();
	HRESULT Loading_Level_Trigger();

	HRESULT Load_Models_FromJson(LEVEL_ID _iLevId, const _tchar* _szJsonFilePath, _fmatrix _PreTransformMatrix);
	HRESULT Load_Dirctory_Models(_uint _iLevId, const _tchar* _szDirPath, _fmatrix _PreTransformMatrix);
	HRESULT Load_Dirctory_Models_Recursive(_uint _iLevId, const _tchar* _szDirPath, _fmatrix _PreTransformMatrix);


public:
	static CLoader* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, LEVEL_ID _eNextLevelID, CImguiLogger* _pLogger);
	virtual void Free() override;
};

END