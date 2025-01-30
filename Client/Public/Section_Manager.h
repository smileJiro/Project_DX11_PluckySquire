#pragma once
#include "Base.h"
#include "Section.h"

BEGIN(Engine)
class CGameInstance;

END

BEGIN(Client)

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
	// 1. 원하는 Section에 Object 추가 / 삭제.
	HRESULT							Add_GameObject_ToSectionLayer(const _wstring& _strSectionTag, CGameObject* _pGameObject);
	HRESULT							Remove_GameObject_ToSectionLayer(const _wstring& _strSectionTag, CGameObject* _pGameObject);
	
	// 2. 현재 활성화된 Section에 Object 추가/삭제
	HRESULT							Add_GameObject_ToCurSectionLayer(CGameObject* _pGameObject, _uint _iLayerIndex = 0);
	HRESULT							Remove_GameObject_ToCurSectionLayer(CGameObject* _pGameObject);
	
	// 3. 원하는 Section 활성, 비활성 
	HRESULT							SetActive_Section(const _wstring& _strSectionTag, _bool _isActive);
	// 4. 외부에서도 Section을 추가하게 할지 말지 

	HRESULT							Section_AddRenderGroup_Process();

public :
	HRESULT							Change_CurSection(const _wstring _strSectionKey);
	_bool							Is_CurSection	(CGameObject* _pGameObject);
	const _wstring*					Get_SectionKey	(CGameObject* _pGameObject);
	_int							Get_SectionLeveID() { return m_iCurLevelID; }

	_bool							Get_CurSectionRenderGroupKey(_uint& _iOutputGroupID, _uint& _iOutputPriorityID)
	{
		if (nullptr == m_pCurSection)
			return false;
		return m_pCurSection->Get_RenderGroupKey(_iOutputGroupID, _iOutputPriorityID);
	}

public:
	// Get
	ID3D11RenderTargetView*			Get_RTV_FromRenderTarget(const _wstring& _strSectionTag);

	ID3D11ShaderResourceView*		Get_SRV_FromRenderTarget(const _wstring& _strSectionTag);
	ID3D11ShaderResourceView*		Get_SRV_FromRenderTarget(_uint _iCurActiveIndex);
	ID3D11ShaderResourceView*		Get_SRV_FromRenderTarget();

	ID3D11ShaderResourceView*		Get_SRV_FromTexture(const _wstring& _strSectionTag, _uint _iTextureIndex = 0);


	_uint							Get_MaxCurActiveSectionCount() { return m_iMaxCurActiveSectionCount; }

private:
	void							Section_Active(const _wstring& _strSectionTag, _uint iCurSectionIndex);
private:
	ID3D11Device*					m_pDevice = nullptr;
	ID3D11DeviceContext*			m_pContext = nullptr;
	CGameInstance*					m_pGameInstance = nullptr;
private:


	_uint							m_iPriorityGenKey = PR2D_SECTION_START;

	_int							m_iCurLevelID = -1;
	map<_wstring, CSection*>		m_CurLevelSections;
	CSection*						m_pCurSection = nullptr;
	vector<CSection*>				m_CurActiveSections;

	_uint							m_iMaxCurActiveSectionCount = 5;


#pragma region 임시, 2D모델 Info


HRESULT Ready_CurLevelSectionModels(const _wstring& _strJsonPath);

public :
	typedef struct tag2DModelInfo
	{
		_string strModelName;
		_string strModelType;
		_string strActiveType;
		_string strColliderType;
		_bool isActive = false;
		_bool isSorting = false;
		_bool isCollider = false;
		_float2 fSorting_Offset_Pos = {};
		_float2 fCollider_Offset_Pos = {};
		_float	fCollider_Radius = 0.f;
		_float2 fCollider_Extent = {};
	}MODEL_2D_INFO;
private  :
	vector<MODEL_2D_INFO> m_2DModelInfos;
public:
	const MODEL_2D_INFO& Get_2DModel_Info(_uint _iIndex) { return m_2DModelInfos[_iIndex]; }
#pragma endregion


private:
	void							Clear_Sections();
	HRESULT							Ready_CurLevelSections(const _wstring& _strJsonPath);
public:
	void Free() override;
};
END


// 25.01.26 TODO :: 기초 기능 만든 후 Section 생성해보고 public, private 정리하기
// Renderer 작업하면서 실질적인 Section 생성 해보기. 