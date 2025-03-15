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

	typedef struct tagFresnel
	{
		_float4 vColor = { 1.f, 1.f, 1.f, 1.f };

		_float	fExp = 2.f;
		_float	fBaseReflect = 1.0f;
		_float	fStrength = 1.0f;
		_float	fDummy;

	} FRESNEL_INFO;

	typedef struct tagColorsInfo
	{
		_float4 vDiffuseColor = { 1.f, 1.f, 1.f, 1.f };
		_float4 vBloomColor = { 1.f, 1.f ,1.f, 1.f };
		_float4 vSubColor = { 1.f, 1.f, 1.f, 1.f };
	} COLORS_INFO;
}