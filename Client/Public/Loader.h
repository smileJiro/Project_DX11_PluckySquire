#pragma once
#include "Client_Defines.h"
#include "Base.h"

/* 서브 스레드를 생성한다. */
/* 다음 레벨에 필요한 자원들을 로드한다. */
BEGIN(Engine)
class CGameInstance;
END


BEGIN(Client)
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
	HRESULT Loading_Level_Chapter_TEST();
	HRESULT Loading_Level_Chapter_2();
	HRESULT Loading_Level_Chapter_4();

private: /* Sound */
	HRESULT Loading_BGM_PathFind(const _wstring& strDirectoryPath);
	HRESULT Loading_SFX_PathFind(const _wstring& strDirectoryPath);

private:
	HRESULT Load_Dirctory_Models(_uint _iLevId, const _tchar* _szDirPath, _fmatrix _PreTransformMatrix);
	HRESULT Load_Dirctory_2DModels(_uint _iLevId, const _tchar* _szDirPath);
	// 경로에 있는 3D 모델파일을 전부 불러온다.
	HRESULT Load_Dirctory_Models_Recursive(_uint _iLevId, const _tchar* _szDirPath, _fmatrix _PreTransformMatrix);
	// 경로에 있는 2D 모델파일을 전부 불러온다.
	HRESULT Load_Dirctory_2DModels_Recursive(_uint _iLevId, const _tchar* _szDirPath);
	// Json에 적혀있는 모델 파일만 불러온다.
	HRESULT Load_Models_FromJson(LEVEL_ID _iLevId, const _tchar* _szJsonFilePath, _fmatrix _PreTransformMatrix, _bool _isCollider = false);
	HRESULT Load_Models_FromJson(LEVEL_ID _iLevId, const _tchar* _szJsonFilePath, const _tchar* _szJsonFileName, _fmatrix _PreTransformMatrix, _bool _isCollider = false)
	{
		return Load_Models_FromJson(_iLevId, (_wstring(_szJsonFilePath) + _wstring(_szJsonFileName)).c_str(), _PreTransformMatrix, _isCollider);
	}
	HRESULT Load_Directory_Effects(LEVEL_ID _iLevID, const _tchar* _szJsonFilePath);

	HRESULT Map_Object_Create(LEVEL_ID _eProtoLevelId, LEVEL_ID _eObjectLevelId, _wstring _strFileName);

public:
	static CLoader* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, LEVEL_ID _eNextLevelID);
	virtual void Free() override;
};

END