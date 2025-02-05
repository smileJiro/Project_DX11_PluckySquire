#include "..\Public\Animation3D.h"

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


CAnimation3D::CAnimation3D()
	:CAnimation()
{
}

CAnimation3D::CAnimation3D(const CAnimation3D& Prototype)
	:CAnimation(Prototype)
	, m_iNumChannels{ Prototype.m_iNumChannels }
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

HRESULT CAnimation3D::Initialize(ifstream& inFile, const C3DModel* pModel)
{
	_uint iNameLength = 0;
	inFile.read(reinterpret_cast<char*>(&iNameLength), sizeof(_uint));
	inFile.read(m_szName, iNameLength);
	m_szName[iNameLength] = '\0';
	//cout << m_szName << endl;


	double dValue = 0.0;
	inFile.read(reinterpret_cast<char*>(&dValue), sizeof(double));
	m_fDuration = (_float)dValue;
	inFile.read(reinterpret_cast<char*>(&dValue), sizeof(double));
	m_fTickPerSecond = (_float)dValue;
	//Loop
	inFile.read(reinterpret_cast<char*>(&m_bLoop), sizeof(_bool));
	//SpeedMagnifier
	inFile.read(reinterpret_cast<char*>(&m_fSpeedMagnifier), sizeof(_float));


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

bool CAnimation3D::Update_TransformationMatrices(const vector<class CBone*>& Bones, _float fTimeDelta, _bool _bReverse)
{

	if ((_bReverse && m_fCurrentTrackPosition < 0)
		|| (false == _bReverse && m_fCurrentTrackPosition > m_fDuration))
	{
		if (true == m_bLoop)
		{
			Reset(_bReverse);
		}
		else
		{
			m_fCurrentTrackPosition =  _bReverse ? 0 : m_fDuration;
			return true;
		}
	}


	for (size_t i = 0; i < m_iNumChannels; i++)
	{
		m_vecChannel[i]->Update_TransformationMatrix(m_fCurrentTrackPosition, &m_CurrentKeyFrameIndices[i], Bones, _bReverse);
	}
	if (_bReverse)
		m_fCurrentTrackPosition -= m_fTickPerSecond * fTimeDelta * m_fSpeedMagnifier;
	else
		m_fCurrentTrackPosition += m_fTickPerSecond * fTimeDelta * m_fSpeedMagnifier;


	return false;
}
bool CAnimation3D::Update_AnimTransition(const vector<class CBone*>& Bones, _float fTimeDelta, const map<_uint, KEYFRAME>& mapAnimTransLeftFrame, _bool _bReverse)
{
	float _fAnimTransitionTrackPos = m_fAnimTransitionTime * m_fTickPerSecond;
	if (m_fCurrentTrackPosition >= _fAnimTransitionTrackPos)
	{
		Reset(_bReverse);
		return true;
	}
	m_fCurrentTrackPosition += m_fTickPerSecond * fTimeDelta * m_fSpeedMagnifier;

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

void CAnimation3D::Reset(_bool _bReverse)
{
	__super::Reset(_bReverse);
	//m_fCurrentTrackPosition = _bReverse ? m_fDuration : 0;
}


void CAnimation3D::Get_CurrentFrame(map<_uint,KEYFRAME>* pOutKeyFrames) const
{
	for (auto& channel : m_vecChannel)
		channel->Get_Frame(m_fCurrentTrackPosition, pOutKeyFrames);
}

_float CAnimation3D::Get_Progress()
{
	return m_fCurrentTrackPosition / m_fDuration;
}

bool CAnimation3D::Is_AnimChangeable()
{
	return m_fDuration * m_fPostDelayPercentage <= m_fCurrentTrackPosition;
}

void CAnimation3D::Set_Progress(_float _fProgerss)
{
	m_fCurrentTrackPosition = _fProgerss * m_fDuration;
	//현재 TrackPosition이 되기 전의 키 프레임으로
	for (_uint i = 0; i < m_iNumChannels; i++)
	{
		m_CurrentKeyFrameIndices[i] = m_vecChannel[i]->Get_KeyFrameIndex(m_fCurrentTrackPosition);
	}
}

void CAnimation3D::Get_Frame(_float fTrackPos, map<_uint, KEYFRAME>* pOutKeyFrames) const
{
	for (auto& channel : m_vecChannel)
		channel->Get_Frame(fTrackPos, pOutKeyFrames);
}


void CAnimation3D::Ready_AnimTransition()
{
	Reset();
}

CAnimation3D* CAnimation3D::Create(ifstream& inFile, const C3DModel* pModel)
{
	CAnimation3D* pInstance = new CAnimation3D();

	if (FAILED(pInstance->Initialize(inFile, pModel)))
	{
		MSG_BOX("Failed to Created : CAnimation");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CAnimation3D* CAnimation3D::Clone()
{
	return new CAnimation3D(*this);
}

void CAnimation3D::Free()
{
	for (auto& pChannel : m_vecChannel)
		Safe_Release(pChannel);

	m_vecChannel.clear();

	__super::Free();
}

