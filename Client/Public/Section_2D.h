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
		PLAYMAP,
		NARRAION,
		SKSP,
		SECTION_2D_PLAY_TYPE_LAST
	};
	// 섹션 내부 렌더그룹.

public:
	typedef struct tagSection2DDesc : public CSection::SECTION_DESC
	{
		_wstring				strFileName;
		_wstring				strMapName;
		_float2					fRenderResolution;
		_float2					fLevelSizePixels;

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
	virtual HRESULT						Initialize(SECTION_2D_DESC* _pDesc, _uint _iPriorityKey);

	/// <summary>
	/// <paramref name="_SectionJson"/>으로부터 Section_2D의 공통정보를 불러온 뒤 각 객체 Initialize를 순서대로 호출한다.
	/// </summary>
	/// <param name="_SectionJson">Ref : Client/Bin/MapSaveFiles/2D/Chapter1_Data.json 참조</param>
	/// <param name="_iPriorityKey">PriorityKey</param>
	virtual HRESULT						Import(json _SectionJson, _uint _iPriorityKey);

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

	void								Set_WorldTexture(ID3D11Texture2D* _pTexture) 
	{
		if (nullptr != m_pMap)
			return ;
		return m_pMap->Set_WorldTexture(_pTexture);
	}
	ID3D11Texture2D*					Get_WorldTexture()
	{
		if (nullptr == m_pMap) 
			return nullptr;
		return m_pMap->Get_WorldTexture();
	}
	const _wstring						Get_WorldRenderTarget_Tag();
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

	_bool					m_isRotation = false;

protected:
	/// <summary>
	/// SECTION_2D_DESC의 정보를 사용하여 Map 2D를 생성한다.
	/// </summary>
	HRESULT Ready_Map_2D(_wstring _strMapFIleName);

	void Free() override;
};

END