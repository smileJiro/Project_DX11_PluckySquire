#include "stdafx.h"
#include "ToolAnimation3D.h"
#include "3DModel.h"

CToolAnimation3D::CToolAnimation3D()
	:CAnimation3D()
{
}

CToolAnimation3D::CToolAnimation3D(const CToolAnimation3D& Prototype)
	: CAnimation3D(Prototype)
{
}


HRESULT CToolAnimation3D::Initialize(ifstream& inFile, const C3DModel* pModel)
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

	inFile.read(reinterpret_cast<char*>(&m_iNumChannels), sizeof(_uint));

	m_CurrentKeyFrameIndices.resize(m_iNumChannels);

	for (size_t i = 0; i < m_iNumChannels; i++)
	{
		CToolChannel* pChannel = CToolChannel::Create(inFile, pModel);
		if (nullptr == pChannel)
			return E_FAIL;

		m_vecChannel.push_back(pChannel);
	}
	return S_OK;
}

void CToolAnimation3D::Export(ofstream& _outfile)
{
	_uint iCount = 0;
	//Name
	_outfile.write(reinterpret_cast<const char*>(&m_iNameLength), sizeof(_uint));
	_outfile.write(m_szName, m_iNameLength);
	//Duration
	_double dValue = m_fDuration;
	_outfile.write(reinterpret_cast<const char*>(&dValue), sizeof(_double));
	//TickPerSecond
	dValue = m_fTickPerSecond;
	_outfile.write(reinterpret_cast<const char*>(&dValue), sizeof(_double));
	//ChannelCount
	_outfile.write(reinterpret_cast<const char*>(&m_iNumChannels), sizeof(_uint));
	for (size_t i = 0; i < m_iNumChannels; i++)
	{
		static_cast<CToolChannel*>(m_vecChannel[i])->Export(_outfile);
	}
}

_float CToolAnimation3D::Get_TrackPosition()
{
	return m_fCurrentTrackPosition;
}

void CToolAnimation3D::Set_TrackPosition(_float _fTrackPos)
{
	m_fCurrentTrackPosition = _fTrackPos;
}

CToolAnimation3D* CToolAnimation3D::Create(ifstream& inFile, const C3DModel* pModel)
{
	CToolAnimation3D* pInstance = new CToolAnimation3D();

	if (FAILED(pInstance->Initialize(inFile, pModel)))
	{
		MSG_BOX("Failed to Created : ToolAnimation3D");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CToolAnimation3D* CToolAnimation3D::Clone()
{
	return new CToolAnimation3D(*this);
}

void CToolAnimation3D::Free()
{
	__super::Free();
}

CToolChannel::CToolChannel()
	:CChannel()
{
}

CToolChannel::CToolChannel(const CToolChannel& Prototype)
	:CChannel(Prototype)
{
}

HRESULT CToolChannel::Initialize(ifstream& inFile, const C3DModel* pModel)
{

	inFile.read(reinterpret_cast<char*>(&m_iNameLength), sizeof(_uint));
	inFile.read(m_szName, m_iNameLength);
	m_szName[m_iNameLength] = '\0';
	m_iBoneIndex = pModel->Get_BoneIndex(m_szName);

	inFile.read(reinterpret_cast<char*>(&m_iNumKeyFrames), sizeof(_uint));

	for (_uint i = 0; i < m_iNumKeyFrames; i++)
	{
		KEYFRAME		tKeyFrame = {};

		inFile.read(reinterpret_cast<char*>(&tKeyFrame), sizeof(KEYFRAME));

		m_KeyFrames.push_back(tKeyFrame);
	}
	return S_OK;
}

void CToolChannel::Export(ofstream& _outfile)
{
	//Name
	_outfile.write(reinterpret_cast<const char*>(&m_iNameLength), sizeof(_uint));
	_outfile.write(m_szName, m_iNameLength);
	//KeyFrames
	_uint iCount = m_KeyFrames.size();
	_outfile.write(reinterpret_cast<const char*>(&iCount), sizeof(_uint));
	for (auto& tKeyFrame : m_KeyFrames)
	{
		_outfile.write(reinterpret_cast<const char*>(&tKeyFrame), sizeof(KEYFRAME));
	}
}

CToolChannel* CToolChannel::Create(ifstream& inFile, const C3DModel* pModel)
{
	CToolChannel* pInstance = new CToolChannel();

	if (FAILED(pInstance->Initialize(inFile, pModel)))
	{
		MSG_BOX("Failed to Created : ToolChannel");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CToolChannel::Free()
{
	__super::Free();
}
