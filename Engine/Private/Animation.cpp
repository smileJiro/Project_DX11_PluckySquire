#include "..\Public\Animation.h"

#include "Channel.h"
#include "Bone.h"

namespace Engine
{
	KEYFRAME Lerp_Frame(const KEYFRAME& Frame1, const KEYFRAME& Frame2, _float fRatio)
	{
		KEYFRAME KeyFrame = {};
		XMStoreFloat3(&KeyFrame.vScale, XMVectorLerp(XMLoadFloat3(&Frame1.vScale), XMLoadFloat3(&Frame2.vScale), fRatio));
		XMStoreFloat4(&KeyFrame.vRotation, XMQuaternionSlerp(XMLoadFloat4(&Frame1.vRotation), XMLoadFloat4(&Frame2.vRotation), fRatio));
		XMStoreFloat3(&KeyFrame.vPosition, XMVectorLerp(XMLoadFloat3(&Frame1.vPosition), XMLoadFloat3(&Frame2.vPosition), fRatio));

		return KeyFrame;
	}
}


CAnimation::CAnimation()
{
}

CAnimation::CAnimation(const CAnimation& Prototype)
	: m_iNumChannels{ Prototype.m_iNumChannels }
	, m_vecChannel{ Prototype.m_vecChannel }
	, m_fDuration{ Prototype.m_fDuration }
	, m_fTickPerSecond{ Prototype.m_fTickPerSecond }
	, m_fCurrentTrackPosition{ Prototype.m_fCurrentTrackPosition }
	, m_CurrentKeyFrameIndices{ Prototype.m_CurrentKeyFrameIndices }
{
	for (auto& pChannel : m_vecChannel)
		Safe_AddRef(pChannel);

	strcpy_s(m_szName, Prototype.m_szName);
}

HRESULT CAnimation::Initialize(ifstream& inFile, const C3DModel* pModel)
{
	_uint iNameLength = 0;
	inFile.read(reinterpret_cast<char*>(&iNameLength), sizeof(_uint));
	inFile.read(m_szName, iNameLength);
	m_szName[iNameLength] = '\0';
	cout << m_szName << endl;


	double dValue = 0.0;
	inFile.read(reinterpret_cast<char*>(&dValue), sizeof(double));
	m_fDuration = (_float)dValue;
	inFile.read(reinterpret_cast<char*>(&dValue), sizeof(double));
	m_fTickPerSecond = (_float)dValue;

	inFile.read(reinterpret_cast<char*>(&m_iNumChannels), sizeof(_uint));

	m_CurrentKeyFrameIndices.resize(m_iNumChannels);

	for (size_t i = 0; i < m_iNumChannels; i++)
	{
		CChannel* pChannel = CChannel::Create(inFile, pModel);
		if (nullptr == pChannel)
			return E_FAIL;

		m_vecChannel.push_back(pChannel);
	}
	return S_OK;
}

bool CAnimation::Update_TransformationMatrices(const vector<class CBone*>& Bones, _float fTimeDelta)
{

	if (m_fCurrentTrackPosition >= m_fDuration)
	{
		if (true == m_bLoop)
			m_fCurrentTrackPosition = 0.f;
		else
			return true;
	}


	for (size_t i = 0; i < m_iNumChannels; i++)
	{
		m_vecChannel[i]->Update_TransformationMatrix(m_fCurrentTrackPosition, &m_CurrentKeyFrameIndices[i], Bones);
	}
	m_fCurrentTrackPosition += m_fTickPerSecond * fTimeDelta;

	for (auto& tEvnt : m_listAnimEvent)
	{
		if (tEvnt.bIsTriggered)
			continue;
		if (Get_Progress() >= tEvnt.fTime)
		{
			tEvnt.pFunc();
			tEvnt.bIsTriggered = true;
		}
	}
	return false;
}

bool CAnimation::Update_AnimTransition(const vector<class CBone*>& Bones, _float fTimeDelta, const map<_uint, KEYFRAME>& mapAnimTransLeftFrame)
{
	float _fAnimTransitionTrackPos = m_fAnimTransitionTime * m_fTickPerSecond;
	if (m_fCurrentTrackPosition >= _fAnimTransitionTrackPos)
	{
		Reset_CurrentTrackPosition();
		return true;
	}
	m_fCurrentTrackPosition += m_fTickPerSecond * fTimeDelta;

	_float			fRatio = m_fCurrentTrackPosition / _fAnimTransitionTrackPos;
	KEYFRAME tFrame;
	for (_uint channel = 0; channel < m_iNumChannels; channel++)
	{
		_uint iBoneIdx = m_vecChannel[channel]->Get_BoneIndex();
		KEYFRAME tLeftKey;
		if (mapAnimTransLeftFrame.find(iBoneIdx) == mapAnimTransLeftFrame.end())
			tLeftKey = m_vecChannel[channel]->Get_KeyFrame(0);
		else
			tLeftKey = mapAnimTransLeftFrame.at(iBoneIdx);
		tFrame = Lerp_Frame(tLeftKey, m_vecChannel[channel]->Get_KeyFrame(0), fRatio);
		Bones[iBoneIdx]->Set_TransformationMatrix(XMMatrixAffineTransformation(XMLoadFloat3(&tFrame.vScale), XMVectorSet(0.f, 0.f, 0.f, 1.f), XMLoadFloat4(&tFrame.vRotation), XMVectorSetW(XMLoadFloat3(&tFrame.vPosition), 1.f)));
	}

	return false;
}

void CAnimation::Reset_CurrentTrackPosition()
{
	m_fCurrentTrackPosition = 0.f;
	ZeroMemory(m_CurrentKeyFrameIndices.data(), 0);
	for (auto& tEvnt : m_listAnimEvent)
		tEvnt.bIsTriggered = false;
}

void CAnimation::Get_CurrentFrame(map<_uint,KEYFRAME>* pOutKeyFrames) const
{
	for (auto& channel : m_vecChannel)
		channel->Get_Frame(m_fCurrentTrackPosition, pOutKeyFrames);
}

_float CAnimation::Get_Progress()
{
	return m_fCurrentTrackPosition / m_fDuration;
}

bool CAnimation::Is_AnimChangeable()
{
	return m_fDuration * m_fPostDelayPercentage <= m_fCurrentTrackPosition;
}

void CAnimation::Get_Frame(_float fTrackPos, map<_uint, KEYFRAME>* pOutKeyFrames) const
{
	for (auto& channel : m_vecChannel)
		channel->Get_Frame(fTrackPos, pOutKeyFrames);
}


void CAnimation::Register_AnimEvent(ANIM_EVENT tAnimEvent)
{
	m_listAnimEvent.push_back(tAnimEvent);
}

void CAnimation::Ready_AnimTransition()
{
	Reset_CurrentTrackPosition();
}

CAnimation* CAnimation::Create(ifstream& inFile, const C3DModel* pModel)
{
	CAnimation* pInstance = new CAnimation();

	if (FAILED(pInstance->Initialize(inFile, pModel)))
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
	for (auto& pChannel : m_vecChannel)
		Safe_Release(pChannel);

	m_vecChannel.clear();

	__super::Free();
}
