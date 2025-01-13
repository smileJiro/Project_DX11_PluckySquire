#pragma once
#include "Base.h"

BEGIN(Engine)
class CModel;
class CBone;
class CChannel final : public CBase
{
private:
	CChannel();
	virtual ~CChannel() = default;

public:
	HRESULT Initialize(const aiNodeAnim* _pAIChannel, const CModel* _pModel);
	HRESULT Initialize(const FBX_CHANNEL& _ChannelDesc);
	void Update_TransformationMatrix(_float _fCurTrackPosition, _uint* _pKeyFrameIndex, const vector<CBone*>& _vecBones);

public:
	
public:
	// Get
	_uint Get_BoneIndex() { return m_iBoneIndex; }
	const KEYFRAME& Get_KeyFrame(_uint _iKeyFrameIndex) { return m_KeyFrames[_iKeyFrameIndex]; }
	// Set
private:
	_char					m_szName[MAX_PATH] = {};
	_uint					m_iNumKeyFrames = 0;
	_uint					m_iBoneIndex = 0; // CModel 클래스가 가진 m_Bones 벡터 컨테이너중 현재 나에게 해당되는 CBone의 인덱스.
	vector<KEYFRAME>		m_KeyFrames;
public:
	static CChannel* Create(const aiNodeAnim* _pAIChannel, const CModel* _pModel);
	static CChannel* Create(const FBX_CHANNEL& _ChannelDesc);
	virtual void Free() override;

public:
	/* 바이너리 관련 */
	HRESULT ConvertToBinary(HANDLE _hFile, DWORD* _dwByte);
};
END
