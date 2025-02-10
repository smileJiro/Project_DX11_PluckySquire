#pragma once
#include "Section_2D.h"
BEGIN(Client)
class CSection_2D_PlayMap abstract: public CSection_2D
{
public :
	enum SECTION_2D_PLAYMAP_STYLE
	{
		NONE = 0x00,
		COMBAT = 0x01,
		STORY = 0x02,
		EVENT = 0x04,
	};

public :
	typedef struct tagSection2DDesc : public CSection_2D::SECTION_2D_DESC
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

	}SECTION_2D_PLAYMAP_DESC;

protected:
	CSection_2D_PlayMap(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, SECTION_2D_RENDER_TYPE _eRenderType);
	virtual ~CSection_2D_PlayMap() = default;

public:
	HRESULT								Initialize(SECTION_2D_PLAYMAP_DESC* _pDesc, _uint _iPriorityKey);
	HRESULT								Import(json _SectionJson, _uint _iPriorityKey);

public:
	virtual HRESULT						Add_GameObject_ToSectionLayer(CGameObject* _pGameObject, _uint _iLayerIndex) override;
	virtual HRESULT						Section_AddRenderGroup_Process() override;

public:
	void Free() override;
};

END