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
	HRESULT							Add_GameObject_ToCurSectionLayer(CGameObject* _pGameObject, _uint _iLayerIndex = 1);
	HRESULT							Remove_GameObject_ToCurSectionLayer(CGameObject* _pGameObject);
	
	// 3. 원하는 Section 활성, 비활성 
	HRESULT							SetActive_Section(const _wstring& _strSectionTag, _bool _isActive);
	// 4. 외부에서도 Section을 추가하게 할지 말지 

	HRESULT							Section_AddRenderGroup_Process();

	_float2							Get_Section_RenderTarget_Size(const _wstring _strSectionKey);

public :
#pragma region 	활성화 섹션 관련 함수.

	// 활성화 섹션 변경. (자동으로 이전, 다음 섹션을 연결짓는다.)
	HRESULT							Change_CurSection(const _wstring _strSectionKey);
	// 현재 활성화된 메인 섹션에 있는지 검사한다. 
	_bool							Is_CurSection(CGameObject* _pGameObject);
	// 해당 객체가 등록된 섹션을 찾고, 있다면 그 섹션 키를 반환한다.
	const _wstring*					Get_SectionKey(CGameObject* _pGameObject);
	// 현재 섹션의 렌더그룹 키를 반환한다. 
	_bool							Get_CurSectionRenderGroupKey(_uint& _iOutputGroupID, _uint& _iOutputPriorityID)
	{
		if (nullptr == m_pCurSection)
			return false;
		return m_pCurSection->Get_RenderGroupKey(_iOutputGroupID, _iOutputPriorityID);
	}

	// 현재 활성화된 섹션에 다음 / 이전 섹션이 잇는가? 검사 (책용)
	_bool							Has_Next_Section() { 
		if (m_iMaxCurActiveSectionCount <= m_iCurActiveSectionIndex + 1) 
			return false;
		return nullptr != m_CurActiveSections[(m_iCurActiveSectionIndex) + 1]; };
	_bool							Has_Prev_Section() { 
		if (0 >= m_iCurActiveSectionIndex - 1)
			return false;
		return nullptr != m_CurActiveSections[(m_iCurActiveSectionIndex) - 1]; };


	// 현재 활성화된 섹션 기준으로 다음/이전 키 가져오기.(책용)
	const _wstring* Get_Next_Section_Key() {
		if (!Has_Next_Section())
			return nullptr;
		return &m_CurActiveSections[m_iCurActiveSectionIndex + 1]->Get_SectionName();
	}
	const _wstring* Get_Prev_Section_Key() { 
		if (!Has_Prev_Section())
			return nullptr;
		return &m_CurActiveSections[m_iCurActiveSectionIndex - 1]->Get_SectionName(); }


	// 현재 활성화된 섹션 기준으로 다음/이전 섹션으로 이동 (책용)
	HRESULT							Change_Next_Section()
	{
		if (!Has_Next_Section())
			return E_FAIL;
		return Change_CurSection(m_CurActiveSections[m_iCurActiveSectionIndex + 1]->Get_SectionName());
	};
	HRESULT							Change_Prev_Section()
	{
		if (!Has_Prev_Section())
			return E_FAIL;
		return Change_CurSection(m_CurActiveSections[m_iCurActiveSectionIndex - 1]->Get_SectionName());
		
	};

	


#pragma endregion

public:
	_vector Get_WorldPosition_FromWorldPosMap(_float2 _v2DTransformPosition);
public:
	// Get
	_int							Get_SectionLeveID() { return m_iCurLevelID; }
	ID3D11RenderTargetView*			Get_RTV_FromRenderTarget(const _wstring& _strSectionTag);
	ID3D11ShaderResourceView*		Get_SRV_FromRenderTarget(const _wstring& _strSectionTag);
	ID3D11ShaderResourceView*		Get_SRV_FromRenderTarget(_uint _iCurActiveIndex);
	ID3D11ShaderResourceView*		Get_SRV_FromRenderTarget();
	ID3D11ShaderResourceView*		Get_SRV_FromTexture(const _wstring& _strSectionTag, _uint _iTextureIndex = 0);
	_uint							Get_MaxCurActiveSectionCount() { return m_iMaxCurActiveSectionCount; }

	// Set
	void							Set_BookWorldPosMapTexture(ID3D11Texture2D* _pBookWorldPosMap) { 
		if (nullptr != m_pBookWorldPosMap)
			Safe_Release(m_pBookWorldPosMap);

		m_pBookWorldPosMap = _pBookWorldPosMap; }

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
	
	// index  m_iMaxCurActiveSectionCount / 2 -> Main Section(CurSection)
	vector<CSection*>				m_CurActiveSections;
	_uint							m_iMaxCurActiveSectionCount = 5;
	_uint							m_iCurActiveSectionIndex;

private: /* For. WorldPosMap */
	ID3D11Texture2D*				m_pBookWorldPosMap = nullptr; // 더 좋은 위치가 있다면 추후 스케치 스페이스까지 추가하고 변경해주삼.(02.06 태웅)
#pragma region 임시, 2D모델 Info


HRESULT Ready_CurLevelSectionModels(const _wstring& _strJsonPath);

public :
	typedef struct tag2DModelInfo
	{

		_bool isActive		= false;
		_bool isSorting		= false;
		_bool isBackGround	= false;
		_bool isCollider	= false;
		_string strModelName;
		_string strModelType;
		_string strActiveType;
		_string strColliderType;
		_float2 fSorting_Offset_Pos = {};
		_float2 fCollider_Offset_Pos = {};
		_float2 fCollider_Extent = {};
		_float	fCollider_Radius = 0.f;

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
