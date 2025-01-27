#pragma once

#include "Engine_Defines.h"
#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CCutScene_Sector final : public CBase
{
public:
	enum SECTOR_TYPE { SPLINE, LINEAR, SECTOR_TYPE_END };

	typedef struct tagCutSceneSectorDesc
	{
		_uint			iSectorType = { SECTOR_TYPE_END };
		_float			fSectorDuration = { 5.f };
	}CUTSCENE_SECTOR_DESC;

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
#ifdef _DEBUG
	vector<CUTSCENE_KEYFRAME>*	Get_KeyFrames() { return &m_KeyFrames; };
	_uint						Get_CurKeyFrameIndex() { return m_iCurKeyFrameIndex; };
	_float						Get_TimeOffset();
	_bool						Get_IsLookAt();
	CUTSCENE_KEYFRAME			Get_KeyFrame(_uint _iKeyFrameIndex) { return m_KeyFrames[_iKeyFrameIndex]; }
	_uint						Get_SectorType() { return m_iSectorType; }
	_float						Get_SectorDuration() { return m_fDuration; }
#endif

	_bool						Get_IsChangeKeyFrame();

public:
	_bool						Play_Sector(_float _fTimeDelta, _vector* _pOutPos);
	void						Sort_Sector();
	void						Add_KeyFrame(CUTSCENE_KEYFRAME _tKeyFrame);

private:
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };

private:
	vector<CUTSCENE_KEYFRAME>	m_KeyFrames;
	vector<_float>				m_Knots;

	_float						m_fCurrentTime = {};
	_float						m_fDuration = { 5.f };

	_uint						m_iSectorType = { SECTOR_TYPE_END };

	_int						m_iPreKeyFrameIndex = { 0 };
	_int						m_iCurKeyFrameIndex = { 1 };
	_int						m_iCurSegmentIndex = { 0 };

private:
	_vector						Calculate_Position_Spline(_float _fRatio);
	_vector						Calculate_Position_Spline_Test(_float _fTimeDelta);
	_vector						Calculate_Position_Catmull_Rom(_float _fRatio);
	_vector						Calculate_Position_Linear(_float _fRatio);

	_uint						Find_Span(_float _fRatio);
	_float						BasisFunction(_uint _i, _uint _iDegree, _float _fRatio);
	
	void						Calculate_Index();
public:
	static CCutScene_Sector*	Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, void* pArg);
	CCutScene_Sector*			Clone();
	virtual void				Free() override;

};

END