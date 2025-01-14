#pragma once

#include "Base.h"


BEGIN(Engine)

class CChannel final : public CBase
{
private:
	CChannel();
	virtual ~CChannel() = default;

public:
	HRESULT Initialize(ifstream& inFile, const class CModel* pModel);
	void Update_TransformationMatrix(_float fCurrentTrackPosition, _uint* pKeyFrameIndex, const vector<class CBone*>& Bones);

	void Get_Frame(_float fTrackPos, map<_uint, KEYFRAME>* pOutKeyFrames) const;
	KEYFRAME Get_Frame(_float fTrackPos, _uint* pOutCurrentKeyFrameIdx) const;
	_uint Get_KeyFrameIndex(_float fTrackPos, _uint iStartKeyFrameIdx = 0) const;
	_uint Get_BoneIndex() const { return m_iBoneIndex; }
	KEYFRAME Get_KeyFrame(_uint iKeyFrameIndex) { return m_KeyFrames[iKeyFrameIndex]; }
private:
	_char						m_szName[MAX_PATH] = {};
	_uint						m_iNumKeyFrames = {};
	vector<KEYFRAME>			m_KeyFrames;
	_uint						m_iBoneIndex = {};

public:
	static CChannel* Create(ifstream& inFile, const class CModel* pModel);
	virtual void Free() override;
};

END