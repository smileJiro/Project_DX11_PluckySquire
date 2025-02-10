#pragma once
#include "Section.h"
BEGIN(Client)
class CMap_2D;
class CSection_2D final : public CSection
{

public:
	enum SECTION_2D_RENDER_TYPE
	{
		NONE_SECTION,
		ON_SECTION_BOOK,
		ON_SECTION_3D_IN_2D,
		SECTION_LAST
	};

	enum SECTION_2D_RENDERGROUP
	{
		SECTION_2D_BACKGROUND,
		SECTION_2D_OBJECT,
		SECTION_2D_TRIGGER,
		SECTION_2D_UI,
		SECTION_2D_RENDERGROUP_LAST,
	};


public :
	typedef struct tagSection2DDesc : public CSection::SECTION_DESC
	{
		_wstring				strFileName;
		_wstring				strMapName;
		_float2					fRenderTarget_Size;
		SECTION_2D_RENDER_TYPE	eSectionRenderType = {};

		void Book_2D_Build(const _wstring _strBookSectionName)
		{
			eSectionRenderType = ON_SECTION_BOOK;
			strSectionName = _strBookSectionName;
			fRenderTarget_Size = { (_float)RTSIZE_BOOK2D_X, (_float)RTSIZE_BOOK2D_Y };
		}

	}SECTION_2D_DESC;

private:
	CSection_2D(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CSection_2D() = default;

public:
	HRESULT								Initialize(SECTION_2D_DESC* _pDesc, _uint _iPriorityKey);
	HRESULT								Import(json _SectionJson, _uint _iPriorityKey);

public:	/* Section_2D 기능. */
	// 1. Map_2D에 있는 RTV에 자기 자신의 Map Texture를 Copy하는 기능.
	// 2. Section의 Map_2D에 있는 SRV를 return하는 기능. (혹은 직접 바인딩을 하는 기능.)
	HRESULT								Register_RTV_ToTargetManager();
	HRESULT								Register_RenderGroup_ToRenderer();

public:
	virtual HRESULT Add_GameObject_ToSectionLayer(CGameObject* _pGameObject, _uint _iLayerIndex = SECTION_2D_OBJECT) override;

	ID3D11RenderTargetView*				Get_RTV_FromRenderTarget();
	ID3D11ShaderResourceView*			Get_SRV_FromRenderTarget();
	ID3D11ShaderResourceView*			Get_SRV_FromTexture(_uint _iTextureIndex);

	_float2								Get_RenderTarget_Size();

	virtual HRESULT Section_AddRenderGroup_Process() override;


	// 맵 연결 생각안해놨는데, 일단 해보자.

	_bool					Is_NextPage(_wstring& _strNextPageTag) { _strNextPageTag = m_strNextPageTag; return L"" != m_strNextPageTag; };
	_bool					Is_PrePage(_wstring& _strPrePageTag) { _strPrePageTag = m_strPrePageTag; return L"" != m_strPrePageTag; };
private:
	CMap_2D* m_pMap = nullptr;

	_wstring		m_strNextPageTag = L"";
	_wstring		m_strPrePageTag = L"";

private:
	HRESULT Ready_Map_2D(SECTION_2D_DESC* _pDesc, _uint _iPriorityKey);

public:
	static CSection_2D* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, _uint _iPriorityKey, SECTION_2D_DESC* _pDesc);
	static CSection_2D* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, _uint _iPriorityKey, json _SectionJson);
	void Free() override;
};

END