#pragma once
#include "Section.h"
#include "Map_2D.h"

BEGIN(Engine)
class CModelObject;
END

BEGIN(Client)
class CMap_2D;
class CSection_2D abstract : public CSection
{
public:
	enum SECTION_2D_RENDER_TYPE
	{
		SECTION_2D_BOOK,
		SECTION_2D_SKSP,
		SECTION_2D_RENDER_TYPE_LAST,
	};


	enum SECTION_2D_PLAY_TYPE
	{
		PLAYMAP, //0
		NARRAION, //1
		SKSP, //2
		MINIGAME, //3
		WORLDMAP, //4
		SECTION_2D_PLAY_TYPE_LAST
	};

public:
	typedef struct tagSection2DDesc : public CSection::SECTION_DESC
	{
		_wstring				strFileName;
		_wstring				strMapName;
		_float2					fRenderResolution;
		_float2					fLevelSizePixels;

		_bool					isImport;
		json					SectionJson;

		void Book_2D_Build(const _wstring _strBookSectionName)
		{
			strSectionName = _strBookSectionName;
			fRenderResolution = { (_float)RTSIZE_BOOK2D_X, (_float)RTSIZE_BOOK2D_Y };
			fLevelSizePixels = { (_float)RTSIZE_BOOK2D_X, (_float)RTSIZE_BOOK2D_Y };
		}

	}SECTION_2D_DESC;

protected:
	CSection_2D(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, SECTION_2D_PLAY_TYPE _ePlayType, SECTION_2D_RENDER_TYPE _eRenderType);
	virtual ~CSection_2D() = default;

public:
	/// <summary>
	/// 부모의 Initialize를 호출한다.
	/// </summary>
	/// <param name="_pDesc">SECTION_2D_DESC, nullptr은 처리 불가능</param>
	/// <param name="_iPriorityKey">PriorityKey</param>
	virtual HRESULT						Initialize(void* _pDesc);

	/// <summary>
	/// <paramref name="_SectionJson"/>으로부터 Section_2D의 공통정보를 불러온 뒤 각 객체 Initialize를 순서대로 호출한다.
	/// </summary>
	/// <param name="_SectionJson">Ref : Client/Bin/MapSaveFiles/2D/Chapter1_Data.json 참조</param>
	/// <param name="_iPriorityKey">PriorityKey</param>
	virtual HRESULT						Import(void* _pDesc);

public:
	/// <summary>
	/// Map2D의 Render Group에 바인딩되어 있는 Render Target View를 가져온다.
	/// </summary>
	ID3D11RenderTargetView* Get_RTV_FromRenderTarget();

	/// <summary>
	/// Map2D의 Render Group에 바인딩되어 있는 Shader Resource View를 가져온다.
	/// </summary>
	ID3D11ShaderResourceView* Get_SRV_FromRenderTarget();

	/// <summary>
	/// Map2D 가 가지고 있는 BackGround 텍스쳐를 불러온다
	/// </summary>
	ID3D11ShaderResourceView* Get_SRV_FromTexture(_uint _iTextureIndex);

	/// <summary>
	/// Map2D가 BackGround 텍스쳐를 본인 Render Group에 바인딩되어 있는 Render Target에 복사한다.
	/// </summary>
	HRESULT								Copy_DefaultMap_ToRenderTarget();

	/// <summary>
	/// Map2D가의 Render Group에 바인딩되어 있는 View들의 공통 Size를 가져온다.
	/// </summary>
	_float2								Get_RenderTarget_Size();

	/// <summary>
	/// 섹션의 주요 기능(렌더그룹 관리, 각 객체 렌더그룹 Insert, etc... 을 수행한다.
	/// </summary>
	virtual HRESULT						Section_AddRenderGroup_Process() abstract;

	/// <summary>
	/// 이전 페이지 정보가 존재하는가?
	/// </summary>
	/// <param name="_strPrePageTag">현재 섹션 객체에 등록되어 있는 Tag를 반환한다.</param>
	/// <returns>현재 섹션 객체에 등록되어 있는 해당 페이지 정보 Tag가 빈 String이라면, false를 반환한다.</returns>
	_bool								Has_PrePage(_wstring& _strPrePageTag) { _strPrePageTag = m_strPrePageTag; return L"" != m_strPrePageTag; };
	_bool								Has_PrePage() { return L"" != m_strPrePageTag; };

	/// <summary>
	/// 다음 페이지 정보가 존재하는가?
	/// </summary>
	/// <param name="_strPrePageTag">현재 섹션 객체에 등록되어 있는 Tag를 반환한다.</param>
	/// <returns>현재 섹션 객체에 등록되어 있는 해당 페이지 정보 Tag가 빈 String이라면, false를 반환한다.</returns>
	_bool								Has_NextPage(_wstring& _strNextPageTag) { _strNextPageTag = m_strNextPageTag; return L"" != m_strNextPageTag; };
	_bool								Has_NextPage() { return L"" != m_strNextPageTag; };

	SECTION_2D_RENDER_TYPE				Get_Section_2D_RenderType() { return m_eMySectionRenderType; }
	SECTION_2D_PLAY_TYPE				Get_Section_2D_PlayType() { return m_eMySectionPlayType; }

	_bool								Is_Rotation() { return m_isRotation; }
	_bool								Is_Override_WorldTex() { return m_isOverride_WorldTex; }
	_bool								Is_Platformer() { return m_isPlatformer; }
	_bool								Is_Scrolling() { return m_isScrolling; }
	_uint								Get_Override_Normal() { return m_iOverride_Normal; }

	HRESULT								Word_Action_To_Section(const _wstring& _strSectionTag, _uint _iControllerIndex, _uint _iContainerIndex, _uint _iWordType, _bool _isRegistered);


	_float								Get_CameraRatio() { return m_fCameraRatio;  }

	CLayer*								Get_Section_Layer(SECTION_PLAYMAP_2D_RENDERGROUP _eType) { return m_Layers[_eType]; }

	void								Set_NextPageTag(const _wstring _strNextTag) {m_strNextPageTag = _strNextTag;}
	void								Set_PrevPageTag(const _wstring _strPrevTag) {m_strPrePageTag = _strPrevTag;}


	virtual void						Set_WorldTexture(ID3D11Texture2D* _pTexture) 
	{
		if (nullptr == m_pMap)
			return ;
		return m_pMap->Set_WorldTexture(_pTexture);
	}
	ID3D11Texture2D*					Get_WorldTexture()
	{
		if (nullptr == m_pMap) 
			return nullptr;
		return m_pMap->Get_WorldTexture();
	}

	/// <summary>
	/// 이 섹션이 책의 메인 섹션이 되었을 때 호출. 해당 섹션의 Object가 모두 활성화되기 전에 호출된다
	/// 호출시점 CSection_Manager::Change_CurSection() 참조
	/// </summary>
	/// <param name="_strPreSectionTag">활성화 전 책의 Main Section Tag</param>
	virtual HRESULT						Section_Enter(const _wstring& _strPreSectionTag) { return S_OK; }
	/// <summary>
	/// 이 섹션이 책의 메인 섹션이 아니게 되었을때 . 해당 섹션의 Object가 모두 비활성화되기 전에 호출된다
	/// 호출시점 CSection_Manager::Change_CurSection() 참조
	/// </summary>
	/// <param name="_strPreSectionTag">다음 활성화할 책의 Main Section Tag</param>
	virtual HRESULT						Section_Exit(const _wstring& _strNextSectionTag) { return S_OK; }


public:
	HRESULT					Register_WorldCapture(CModelObject* _pModel);


protected:
	SECTION_2D_RENDER_TYPE	m_eMySectionRenderType;
	SECTION_2D_PLAY_TYPE	m_eMySectionPlayType;


	// BackGround & RenderGroup(RT,DSV) 관리객체
	CMap_2D*				m_pMap = nullptr;

	// 실제 렌더 타겟 사이즈
	_float2					m_fRenderResolution = {};

	// 맵 크기
	_float2					m_fLevelSizePixels = {};

	_wstring				m_strNextPageTag = L"";
	_wstring				m_strPrePageTag = L"";

	_bool					m_isScrolling = false;// 컵, tub - 6챕터 도는거.
	_bool					m_isPlatformer = false;
	_bool					m_isRotation = false;
	_bool					m_isOverride_WorldTex = false;
	_uint					m_iOverride_Normal = { 0 };

	_float					m_fCameraRatio = 1.f;

protected:
	/// <summary>
	/// SECTION_2D_DESC의 정보를 사용하여 Map 2D를 생성한다.
	/// </summary>
	HRESULT Ready_Map_2D(_wstring _strMapFIleName);

	void Free() override;
};

END