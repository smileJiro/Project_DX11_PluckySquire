#pragma once

#include "Engine_Defines.h"
#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CCutScene_Sector final : public CBase
{
public:
	enum SECTOR_TYPE { SPLINE, LINEAR, SECTOR_TYPE_END };
private:
	CCutScene_Sector(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CCutScene_Sector(const CCutScene_Sector& _Prototype);
	virtual ~CCutScene_Sector() = default;

public:
	HRESULT						Initialize(void* _pArg);
	HRESULT						Initialize_Clone();
	void						Priority_Update(_float _fTimeDelta);
	void						Update(_float _fTimeDelta);
	void						Late_Update(_float _fTimeDelta);

public:
	_bool						Play_Sector(_float _fTimeDelta, _vector* _pOutPos);

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

private:
	vector<CUTSCENE_KEYFRAME>	m_KeyFrames;
	_float						m_fCurrentTime = {};
	
	SECTOR_TYPE					m_eSectorType = { SECTOR_TYPE_END };

private:
	_vector						Calculate_Position(_float _fRatio);

public:
	static CCutScene_Sector*	Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, void* pArg);
	CCutScene_Sector*			Clone();
	virtual void				Free() override;

};

END