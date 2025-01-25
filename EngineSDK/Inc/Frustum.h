#pragma once
#include "Base.h"

BEGIN(Engine)
class CGameInstance;
class CFrustum : public CBase
{
private:
	CFrustum();
	virtual ~CFrustum() = default;

public:
	HRESULT			Initialize();
	void			Update();	/* 매 프레임 ㅎ호출하여 공통영역인 월드까지 변환 */
	_bool			isIn_InWorldSpace(_fvector _vWorldPos, _float _fRange = 0.0f);

private:
	CGameInstance*				m_pGameInstance = nullptr;

	_float3								m_vPoints[8] = {};
	_float3								m_vPoints_InWorld[8] = {};

	_float4								m_vPlanes_InWorld[6] = {};
	_float4								m_vPlanes_InLocal[6] = {};

private:
	void Make_Planes(const _float3* pPoints, _float4* pOutPlanes);

public:
	static CFrustum* Create();
	virtual void Free() override;
};
END
