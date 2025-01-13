#include "Animation.h"

#include "Channel.h"
#include "Transform.h"
CAnimation::CAnimation()
{
}

CAnimation::CAnimation(const CAnimation& _Prototype)
	: m_iNumChannels(_Prototype.m_iNumChannels)
	, m_Channels{ _Prototype.m_Channels }
	, m_fDuration(_Prototype.m_fDuration)
	, m_fTickPerSecond(_Prototype.m_fTickPerSecond)
	, m_fCurTrackPosition(_Prototype.m_fCurTrackPosition)
	, m_CurKeyFrameIndices{ _Prototype.m_CurKeyFrameIndices }
{
	// 채널은 얕은복사한다 >>> 애니메이션쪽으로 어짜피 m_CurKeyFrameIndex를 빼냈음 >> 이게 애니메이션 재생을 개별적으로 돌리게하는 요소.
	// 각각의 애니메이션은 Initialize에서 CurKeyFrameIndices를 0으로 리셋하고 시작함.
	for (auto& pChannel : m_Channels)
		Safe_AddRef(pChannel);

	strcpy_s(m_szName, _Prototype.m_szName);
}

HRESULT CAnimation::Initialize(const aiAnimation* _pAnimation, const CModel* _pModel)
{
	strcpy_s(m_szName, _pAnimation->mName.data);

	/* 현재 애니메이션의 길이 */
	m_fDuration = _pAnimation->mDuration;
	/* 1초당 얼마만큼의 속도로 Track을 이동할지(애니메이션의 기본 재생속도), */
	m_fTickPerSecond = _pAnimation->mTicksPerSecond;

	/* 해당 애니메이션에서 컨트롤 해야하는 뼈들의 개수 */
	m_iNumChannels = _pAnimation->mNumChannels;

	m_CurKeyFrameIndices.resize(m_iNumChannels);
	for (size_t i = 0; i < m_iNumChannels; ++i)
	{
		/* 채널(컨트롤당하는뼈)의 수 만큼 루프 돌며 객체 생성하고 Channels 벡터에 저장. */
		/* 모든 채널들을 순회하며 일괄처리를 하기위한 목적 외에는 딱히 큰 목적이 없음. 중요한 것은 CChannel class의 기능임. */
		CChannel* pChannel = CChannel::Create(_pAnimation->mChannels[i], _pModel);
		if (nullptr == pChannel)
			return E_FAIL;

		m_Channels.push_back(pChannel);
	}
    return S_OK;
}

HRESULT CAnimation::Initialize(const FBX_ANIMATION& _tAnimDesc)
{
	strcpy_s(m_szName, _tAnimDesc.szName);
	m_fDuration = _tAnimDesc.fDuration;
	m_fTickPerSecond = _tAnimDesc.fTickPerSec;
	m_iNumChannels = _tAnimDesc.iNumChannels;

	m_CurKeyFrameIndices.resize(m_iNumChannels);

	for (size_t i = 0; i < m_iNumChannels; ++i)
	{
		/* 채널(컨트롤당하는뼈)의 수 만큼 루프 돌며 객체 생성하고 Channels 벡터에 저장. */
		/* 모든 채널들을 순회하며 일괄처리를 하기위한 목적 외에는 딱히 큰 목적이 없음. 중요한 것은 CChannel class의 기능임. */
		CChannel* pChannel = CChannel::Create(_tAnimDesc.vecChannels[i]);
		if (nullptr == pChannel)
			return E_FAIL;

		m_Channels.push_back(pChannel);
	}
	return S_OK;
}

_bool CAnimation::Update_TransformationMatrices(const vector<CBone*>& _vecBones, _bool _isLoop, _float _fTransitionReadyPercent, _bool* isTransition, _float _fTimeDelta)
{
	// 현재 애니메이션 재생이 완료되었다면 true return.

	m_fCurTrackPosition += m_fTickPerSecond * _fTimeDelta;
	if (m_fCurTrackPosition / m_fDuration >= _fTransitionReadyPercent)
	{
		if(false == _isLoop)
			*isTransition = true;
	}

	if (true == _isLoop && m_fCurTrackPosition >= m_fDuration)
	{
		m_fCurTrackPosition = 0.f;

	}
	else if (m_fCurTrackPosition >= m_fDuration)
			return true;

	for (size_t i = 0; i < m_iNumChannels; ++i)
	{
		m_Channels[i]->Update_TransformationMatrix(m_fCurTrackPosition, &m_CurKeyFrameIndices[i], _vecBones);
	}

	return false;
}

_bool CAnimation::Update_TransformationMatrices(const vector<CBone*>& _vecBones, _bool _isLoop, _float _fTimeDelta)
{
	// 현재 애니메이션 재생이 완료되었다면 true return.

	m_fCurTrackPosition += m_fTickPerSecond * _fTimeDelta;
	if (true == _isLoop && m_fCurTrackPosition >= m_fDuration)
	{
		m_fCurTrackPosition = 0.f;
	}
	else if (m_fCurTrackPosition >= m_fDuration)
		return true;

	for (size_t i = 0; i < m_iNumChannels; ++i)
	{
		m_Channels[i]->Update_TransformationMatrix(m_fCurTrackPosition, &m_CurKeyFrameIndices[i], _vecBones);
	}

	return false;
}

void CAnimation::Reset_AnimationProgress()
{
	m_fCurTrackPosition = 0.0f;
	ZeroMemory(&m_CurKeyFrameIndices.front(), sizeof(_uint) * m_CurKeyFrameIndices.size());
}

CAnimation* CAnimation::Create(const aiAnimation* _pAIAnimation, const CModel* _pModel)
{
	CAnimation* pInstance = new CAnimation();

	if (FAILED(pInstance->Initialize(_pAIAnimation, _pModel)))
	{
		MSG_BOX("Failed to Created : CAnimation");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CAnimation* CAnimation::Create(const FBX_ANIMATION& _tAnimDesc)
{
	CAnimation* pInstance = new CAnimation();

	if (FAILED(pInstance->Initialize(_tAnimDesc)))
	{
		MSG_BOX("Failed to Created : CAnimation");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CAnimation* CAnimation::Clone()
{
	return new CAnimation(*this);
}

void CAnimation::Free()
{
	for (auto& pChannel : m_Channels)
		Safe_Release(pChannel);
	m_Channels.clear();

	__super::Free();
}

HRESULT CAnimation::ConvertToBinary(HANDLE _hFile, DWORD* _dwByte)
{
	if (!WriteFile(_hFile, m_szName, sizeof(m_szName), _dwByte, nullptr))
		return E_FAIL;

	if (!WriteFile(_hFile, &m_iNumChannels, sizeof(_uint), _dwByte, nullptr))
		return E_FAIL;

	if (!WriteFile(_hFile, &m_fDuration, sizeof(_float), _dwByte, nullptr))
		return E_FAIL;

	if (!WriteFile(_hFile, &m_fTickPerSecond, sizeof(_float), _dwByte, nullptr))
		return E_FAIL;

	/* Channel 순회 */
	for (auto& pChannel : m_Channels)
	{
		pChannel->ConvertToBinary(_hFile, _dwByte);
	}
	return S_OK;
}
