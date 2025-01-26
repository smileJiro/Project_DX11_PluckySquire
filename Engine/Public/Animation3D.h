#pragma once
#include "Animation.h"
#include "Base.h"



BEGIN(Engine)
KEYFRAME Lerp_Frame(const KEYFRAME& Frame1, const KEYFRAME& Frame2, _float fRatio);

class ENGINE_DLL CAnimation3D : public CAnimation
{
protected:
	CAnimation3D();
	CAnimation3D(const CAnimation3D& Prototype);
	virtual ~CAnimation3D() = default;

public:
	virtual HRESULT Initialize(ifstream& inFile, const class C3DModel* pModel);
	bool Update_TransformationMatrices(const vector<class CBone*>& Bones, _float fTimeDelta);
	bool Update_AnimTransition(const vector<class CBone*>& Bones, _float fTimeDelta, const map<_uint, KEYFRAME>& m_mapAnimTransLeftFrame);

	virtual void Reset()override;

	float Get_AnimTime() { return m_fDuration; }
	void Get_Frame(_float fTrackPos, map<_uint, KEYFRAME>* pOutKeyFrames) const;
	void Get_CurrentFrame(map<_uint, KEYFRAME>* pOutKeyFrames) const;
	virtual _float Get_Progress() override;
	bool Is_AnimChangeable();

	virtual void Set_Progress(_float _fProgerss)override;

	void Set_PostDealyPercent(_float fPercentage) { m_fPostDelayPercentage = fPercentage; }
	void Set_AnimTransitionTime(_float fTime) { m_fAnimTransitionTime = fTime; }

	void Ready_AnimTransition();
protected:
	_char					m_szName[MAX_PATH] = {};
	_uint					m_iNumChannels = {};
	vector<class CChannel*> m_vecChannel;


	_float					m_fDuration = {};
	_float					m_fPostDelayPercentage = { 0.f };
	_float					m_fTickPerSecond = {};
	_float					m_fCurrentTrackPosition = {};
	_float					m_fAnimTransitionTime = { 0.1f };

	//Channel마다 현재 KeyFrameIndex를 저장하는 배열
	vector<_uint>			m_CurrentKeyFrameIndices;

public:
	static CAnimation3D* Create(ifstream& inFile, const class C3DModel* pModel);
	virtual CAnimation3D* Clone();
	virtual void Free() override;


};

END