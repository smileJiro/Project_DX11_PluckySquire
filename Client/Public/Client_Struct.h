#pragma once

namespace Client
{
	typedef struct tagPoolingData
	{
		_int iPrototypeLevelID = -1;
		_wstring strPrototypeTag;
		_wstring strLayerTag;

		_wstring strSectionKey;
		SECTION_PLAYMAP_2D_RENDERGROUP eSection2DRenderGroup;
	}Pooling_DESC;
}