#pragma once

#include "Base.h"



BEGIN(Engine)
KEYFRAME Lerp_Frame(const KEYFRAME& Frame1, const KEYFRAME& Frame2, _float fRatio);

typedef struct AnimEvent
{
	_float fTime;
	_bool bIsTriggered = false;
	function<void()> pFunc;
}ANIM_EVENT;
class CAnimation final : public CBase
{
private:
	CAnimation();
	CAnimation(const CAnimation& Prototype);
	virtual ~CAnimation() = default;

public:
	HRESULT Initialize(ifstream& inFile, const class CModel* pModel);
	bool Update_TransformationMatrices(const vector<class CBone*>& Bones, _float fTimeDelta);
	bool Update_AnimTransition(const vector<class CBone*>& Bones, _float fTimeDelta, const map<_uint, KEYFRAME>& m_mapAnimTransLeftFrame);

	void Reset_CurrentTrackPosition();

	float Get_AnimTime() { return m_fDuration; }
	void Get_Frame(_float fTrackPos, map<_uint, KEYFRAME>* pOutKeyFrames) const;
	void Get_CurrentFrame(map<_uint, KEYFRAME>* pOutKeyFrames) const;
	_float Get_Progress();
	bool Is_AnimChangeable();

	void Set_Loop(bool bLoop) { m_bLoop = bLoop; }
	void Set_PostDealyPercent(_float fPercentage) { m_fPostDelayPercentage = fPercentage; }
	void Set_AnimTransitionTime(_float fTime) { m_fAnimTransitionTime = fTime; }
	void Register_AnimEvent(ANIM_EVENT tAnimEvent);

	void Ready_AnimTransition();
private:
	_char					m_szName[MAX_PATH] = {};
	_uint					m_iNumChannels = {};
	vector<class CChannel*> m_vecChannel;


	_float					m_fDuration = {};
	_float					m_fPostDelayPercentage = { 0.f };
	_float					m_fTickPerSecond = {};
	_float					m_fCurrentTrackPosition = {};
	_float					m_fAnimTransitionTime = { 0.1f };

	bool						m_bLoop = { false };

	//Channel마다 현재 KeyFrameIndex를 저장하는 배열
	vector<_uint>			m_CurrentKeyFrameIndices;
	list< ANIM_EVENT>m_listAnimEvent;
public:
	static CAnimation* Create(ifstream& inFile, const class CModel* pModel);
	virtual CAnimation* Clone();
	virtual void Free() override;
};

END