#pragma once
#include "Base.h"
#include "Section_2D.h"

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
	/// <summary>
	/// 섹션 키로 섹션 객체를 직접 찾는다.
	/// </summary>
	/// <param name="_strSectionTag">섹션 키</param>
	CSection*						Find_Section(const _wstring& _strSectionTag);

	/// <summary>
	/// 섹션 키로 해당 섹션에 오브젝트를 추가한다.
	/// </summary>
	/// <param name="_strSectionTag">섹션 키</param>
	/// <param name="_pGameObject">오브젝트</param>
	HRESULT							Add_GameObject_ToSectionLayer(const _wstring& _strSectionTag, CGameObject* _pGameObject, _uint _iLayerIndex = SECTION_2D_PLAYMAP_OBJECT);

	/// <summary>
/// 섹션 키로 해당 섹션에 해당 오브젝트를 제거한다.
/// </summary>
/// <param name="_strSectionTag">섹션 키</param>
/// <param name="_pGameObject">오브젝트</param>
	HRESULT							Remove_GameObject_ToSectionLayer(const _wstring& _strSectionTag, CGameObject* _pGameObject);
	
	/// <summary>
	/// 활성화된 섹션에 오브젝트를 집어넣는다.
	/// </summary>
	/// <param name="_pGameObject">오브젝트</param>
	/// <param name="_iLayerIndex">레이어 지정 (ref. Section_2D::SECTION_2D_RENDERGROUP)</param>
	HRESULT							Add_GameObject_ToCurSectionLayer(CGameObject* _pGameObject, _uint _iLayerIndex = SECTION_2D_PLAYMAP_OBJECT);

	/// <summary>
	/// 활성화된 섹션에서 오브젝트를 제거한다.
	/// </summary>
	/// <param name="_pGameObject">오브젝트</param>
	HRESULT							Remove_GameObject_ToCurSectionLayer(CGameObject* _pGameObject);
	
	/// <summary>
	/// 해당 섹션의 활성화 여부를 결정한다.
	/// </summary>
	/// <param name="_strSectionTag">섹션 키</param>
	/// <param name="_isActive">활성화 여부</param>
	HRESULT							SetActive_Section(CSection* _pSection, _bool _isActive);
	HRESULT							SetActive_Section(const _wstring& _strSectionTag, _bool _isActive);

	/// <summary>
	/// 섹션 루프 - 현재 활성화된 섹션에게 준비 작업을 진행하고, 렌더그룹에 오브젝트를 삽입하게끔 한다.
	/// </summary>
	HRESULT							Section_AddRenderGroup_Process();

	/// <summary>
	/// 해당 섹션의 렌더타겟 사이즈를 가져온다.
	/// </summary>
	/// <param name="_strSectionTag">섹션 키</param>
	_float2							Get_Section_RenderTarget_Size(const _wstring _strSectionKey);

public :
#pragma region 	활성화 섹션 관련 함수.

	/// <summary>
	/// 해당 섹션으로 책 활성화 섹션을 변경한다..
	/// </summary>
	/// <param name="_strSectionTag">섹션 키</param>
	HRESULT							Change_CurSection(const _wstring _strSectionKey);

	/// <summary>
	/// 오브젝트가 현재 활성화된 책 섹션에 포함되어 있는가 확인한다.
	/// </summary>
	/// <param name="_pGameObject">오브젝트</param>
	_bool							Is_CurSection(CGameObject* _pGameObject);

	/// <summary>
	/// 오브젝트가 섹션에 포함되어있는지 확인하고, 있으면 섹션 키 포인트, 없으면 nullptr을 반환
	/// </summary>
	/// <param name="_pGameObject">오브젝트</param>
	const _wstring*					Get_SectionKey(CGameObject* _pGameObject);

	// 현재 섹션의 렌더그룹 키를 반환한다. 
	_bool							Get_CurSectionRenderGroupKey(_uint& _iOutputGroupID, _uint& _iOutputPriorityID)
	{
		if (nullptr == m_pCurSection)
			return false;
		return m_pCurSection->Get_RenderGroupKey(_iOutputGroupID, _iOutputPriorityID);
	}

	//  다음 / 이전 섹션이 잇는가? 검사 (책용)
	_bool							Has_Next_Section() 
	{
		return nullptr != m_pCurSection && m_pCurSection->Has_NextPage();
	}

	_bool							Has_Prev_Section() 
	{
		return nullptr != m_pCurSection && m_pCurSection->Has_PrePage();
	}

	// 현재 활성화된 섹션 기준으로 다음/이전 키 가져오기.(책용)
	const _wstring Get_Next_Section_Key() 
	{
		_wstring strTargetSectionTag = L"";
		if (nullptr == m_pCurSection)
			return strTargetSectionTag;
		m_pCurSection->Has_NextPage(strTargetSectionTag);
		return strTargetSectionTag;
	}
	const _wstring Get_Cur_Section_Key() 
	{
		_wstring strTargetSectionTag = L"";
		if (nullptr == m_pCurSection)
			return strTargetSectionTag;
		return m_pCurSection->Get_SectionName();
	}

	const _wstring Get_Prev_Section_Key() 
	{ 
		_wstring strTargetSectionTag = L"";
		if (nullptr == m_pCurSection)
			return strTargetSectionTag;
		m_pCurSection->Has_PrePage(strTargetSectionTag);
		return strTargetSectionTag;
	}


	// 현재 활성화된 섹션 기준으로 다음/이전 섹션으로 이동 (책용)
	HRESULT							Change_Next_Section()
	{
		_wstring strTargetSectionTag = L"";
		if (nullptr == m_pCurSection)
			return E_FAIL;
		if (!m_pCurSection->Has_NextPage(strTargetSectionTag))
			return E_FAIL;
		return Change_CurSection(strTargetSectionTag);
	};
	HRESULT							Change_Prev_Section()
	{
		_wstring strTargetSectionTag = L"";
		if (nullptr == m_pCurSection)
			return E_FAIL;
		if (!m_pCurSection->Has_PrePage(strTargetSectionTag))
			return E_FAIL;
		return Change_CurSection(strTargetSectionTag);
	};

	


#pragma endregion

public:
	// 간접사용 *(추후 통합)
	_vector Get_WorldPosition_FromWorldPosMap(const _wstring& _strSectionTag,  _float2 _v2DTransformPosition);
	_vector Get_WorldPosition_FromWorldPosMap(_float2 _v2DTransformPosition);
	
	// 실사용
	_vector Get_WorldPosition_FromWorldPosMap(ID3D11Texture2D* m_pTargetTexture, _float2 _v2DTransformPosition);
public:
	// Get
	_int							Get_SectionLeveID() { return m_iCurLevelID; }
	/// <summary>
	/// 해당 섹션을 Section_2D로 간주하고, 2DMap의 RTV을 가져온다.
	/// </summary>
	/// <param name="_strSectionTag">섹션 키</param>
	ID3D11RenderTargetView*			Get_RTV_FromRenderTarget(const _wstring& _strSectionTag);

	/// <summary>
	/// 해당 섹션을 Section_2D로 간주하고, 2DMap의 SRV을 가져온다.
	/// </summary>
	/// <param name="_strSectionTag">섹션 키</param>
	ID3D11ShaderResourceView*		Get_SRV_FromRenderTarget(const _wstring& _strSectionTag);

	/// <summary>
	/// 현재 책에 활성화된 메인 섹션을 Section_2D로 간주하고, 2DMap의 SRV을 가져온다.
	/// </summary>
	ID3D11ShaderResourceView*		Get_SRV_FromRenderTarget();
	ID3D11ShaderResourceView*		Get_SRV_FromTexture(const _wstring& _strSectionTag, _uint _iTextureIndex = 0);

	// Set
	void							Set_BookWorldPosMapTexture(ID3D11Texture2D* _pBookWorldPosMap) 
	{ 
		if (nullptr != m_pBookWorldPosMap)
			Safe_Release(m_pBookWorldPosMap);

		m_pBookWorldPosMap = _pBookWorldPosMap; 
	}

	_uint							Generate_WorldPos_Priority_ID() { return m_iWorldPriorityGenKey++; }


private:
	/// <summary>
	/// 해당 섹션을 활성화 섹션 리스트에 추가하고, 앞뒤 페이지가 있을 경우 카피를 한번 돌려줌.
	/// </summary>
	/// <param name="_strSectionTag">기준 활성화 섹션 키</param>
	void							Main_Section_Active_Process(const _wstring& _strSectionTag);

private:
	ID3D11Device*					m_pDevice = nullptr;
	ID3D11DeviceContext*			m_pContext = nullptr;
	CGameInstance*					m_pGameInstance = nullptr;
private:
	
	// 발급할 Prority Key Start 지점.
	_uint							m_iPriorityGenKey = PR2D_SECTION_START;
	_uint							m_iWorldPriorityGenKey = PRWORLD_SKETCH_START;
	// 전체 섹션 목록
	map<_wstring, CSection*>		m_CurLevelSections;

	// 현재 레벨 
	LEVEL_ID						m_iCurLevelID = LEVEL_END;
	
	// 현재 책에서 재생중인 섹션
	CSection_2D*					m_pCurSection = nullptr;

private: 
	ID3D11Texture2D*				m_pBookWorldPosMap = nullptr; // 더 좋은 위치가 있다면 추후 스케치 스페이스까지 추가하고 변경해주삼.(02.06 태웅)
																  // 없는듯
#pragma region 임시, 2D모델 Info


HRESULT Ready_CurLevelSectionModels(const _wstring& _strJsonPath);

public :
	typedef struct tag2DModelInfo
	{

		_bool	isActive		= false;
		_bool	isSorting		= false;
		_bool	isBackGround	= false;
		_bool	isCollider	= false;
		_string strModelName;
		_uint	eModelType;
		_uint	eActiveType;
		_uint	eColliderType;
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
