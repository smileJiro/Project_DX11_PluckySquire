#pragma once
#include "Base.h"

/* 1. 해당 애니메이션(Idle, Attack, Run, etc...)이 컨트롤 해야하는 뼈(Channel)의 정보를 가진다. */

BEGIN(Engine)
class CModel;
class CChannel;
class CBone;
class CTransform;
class ENGINE_DLL CAnimation final : public CBase
{
private:
	CAnimation();
	CAnimation(const CAnimation& _Prototype); // 채널만 얕은복사해주는 복사생성자.
	virtual ~CAnimation() = default;

public:
	/* 모델 클래스가 있다는 것은, 이름 기준으로 본의 인덱스를 찾겠다는 것 */
	HRESULT Initialize(const aiAnimation* _pAnimation, const CModel* _pModel);
	HRESULT Initialize(const FBX_ANIMATION& _tAnimDesc);
	_bool Update_TransformationMatrices(const vector<CBone*>& _vecBones, _bool _isLoop, _float _fTransitionReadyPercent, _bool* isTransition, _float _fTimeDelta);
	_bool Update_TransformationMatrices(const vector<CBone*>& _vecBones, _bool _isLoop, _float _fTimeDelta);
public:
	// Get
	string Get_Name() { return string(m_szName); }
	_uint Get_NumChannels() { return m_iNumChannels; }

	_float Get_CurTrackPosition() const { return m_fCurTrackPosition; }
	_float Get_AnimationDuration() const { return m_fDuration; }
	_float Get_TickPerSec() const { return m_fTickPerSecond; }
	// Set
	void Set_Name(string _strName) { strcpy_s(m_szName, _strName.c_str()); }
	void Reset_AnimationProgress();
	void Set_CurTrackPosition(_float _fCurTrackPosition) { m_fCurTrackPosition = _fCurTrackPosition; }
	void Set_TickPerSec(_float _fTickPerSec) { m_fTickPerSecond = _fTickPerSec; }
	//void Set_CurKeyTrackPosition(_float _fCurKeyTrackPosition) { m_fCurTrackPosition = _fCurKeyTrackPosition; }
	//void Reset_CurKeyFrameIndices() { ZeroMemory(m_CurKeyFrameIndices.data(), sizeof(_uint) * m_CurKeyFrameIndices.size()); }
private:
	_char m_szName[MAX_PATH] = {};
	_uint m_iNumChannels = 0;
	vector<CChannel*> m_Channels;

	/* 애니메이션 재생 관련. */
	_float m_fDuration = 0.0f; /* Duration : 현재 재생 중인 애니메이션의 총 길이. if(m_fDuration > m_fTickPerSecond * _fTimeDelta) {} */
	_float m_fTickPerSecond = 0.0f; /* 1초당 얼마만큼 이동할건가. (여기서의 이동이란 말은 Track위에서의 애니메이션의 이동 거리를 이야기함.)*/
	_float m_fCurTrackPosition = 0.0f; /* 애니메이션 트랙 위에서의 현재 나의 포지션. */
	vector<_uint> m_CurKeyFrameIndices; // 원래 채널안에 있던 데이터였다. Channel 클래스를 얕은복사하게되면서 애니메이션쪽으로 뺀거.
public:
	static CAnimation* Create(const aiAnimation* _pAIAnimation, const CModel* _pModel);
	static CAnimation* Create(const FBX_ANIMATION& _tAnimDesc);
	virtual CAnimation* Clone();
	virtual void Free() override;

public:
	/* 바이너리 관련 */
	HRESULT ConvertToBinary(HANDLE _hFile, DWORD* _dwByte);


	friend class CAnimator;
};

END