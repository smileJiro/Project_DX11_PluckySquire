#pragma once
#include "Base.h"

BEGIN(Engine)
class CGameInstance;
END
BEGIN(Client)
class CSection;
class CSection_Manager final : public CBase
{
	DECLARE_SINGLETON(CSection_Manager)
private:
	CSection_Manager();
	virtual ~CSection_Manager() = default;

public:
	HRESULT							Initialize(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);

public:
	HRESULT							Level_Exit(_int _iChangeLevelID, _int _iNextChangeLevelID);	// Clear Current Level Section;
	HRESULT							Level_Enter(_int _iChangeLevelID);	// Create Next Level Section;

public:
	// 0. 원하는 Section Find
	CSection*						Find_Section(const _wstring& _strSectionTag);
	// 1. 원하는 Section에 Object 추가.
	HRESULT							Add_GameObject_ToSectionLayer(const _wstring& _strSectionTag, CGameObject* _pGameObject);
	// 2. 원하는 Section 활성, 비활성 
	HRESULT							SetActive_Section(const _wstring& _strSectionTag, _bool _isActive);
	// 3. 외부에서도 Section을 추가하게 할지 말지 

public:
	// Get
	ID3D11RenderTargetView*			Get_RTV_FromRenderTarget(const _wstring& _strSectionTag);
	ID3D11ShaderResourceView*		Get_SRV_FromRenderTarget(const _wstring& _strSectionTag);
	ID3D11ShaderResourceView*		Get_SRV_FromTexture(const _wstring& _strSectionTag, _uint _iTextureIndex = 0);

private:
	ID3D11Device*					m_pDevice = nullptr;
	ID3D11DeviceContext*			m_pContext = nullptr;
	CGameInstance*					m_pGameInstance = nullptr;
private:
	_int							m_iCurLevelID = -1;
	map<_wstring, CSection*>		m_CurLevelSections;

private:
	void							Clear_Sections();
	HRESULT							Ready_CurLevelSections(const _string& _strJsonPath);
public:
	void Free() override;
};
END


// 25.01.26 TODO :: 기초 기능 만든 후 Section 생성해보고 public, private 정리하기
// Renderer 작업하면서 실질적인 Section 생성 해보기. 