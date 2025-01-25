#include "CutScene_Sector.h"

CCutScene_Sector::CCutScene_Sector(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: m_pDevice(_pDevice)
	, m_pContext(_pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

CCutScene_Sector::CCutScene_Sector(const CCutScene_Sector& _Prototype)
	: m_pDevice(_Prototype.m_pDevice)
	, m_pContext(_Prototype.m_pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CCutScene_Sector::Initialize(void* _pArg)
{
	CUTSCENE_SECTOR_DESC* pDesc = static_cast<CUTSCENE_SECTOR_DESC*>(_pArg);

	m_iSectorType = pDesc->iSectorType;

	return S_OK;
}

HRESULT CCutScene_Sector::Initialize_Clone()
{
	return S_OK;
}

void CCutScene_Sector::Priority_Update(_float _fTimeDelta)
{
}

void CCutScene_Sector::Update(_float _fTimeDelta)
{
}

void CCutScene_Sector::Late_Update(_float _fTimeDelta)
{
}

_float CCutScene_Sector::Get_TimeOffset()
{
	if (-1 == m_iCurKeyFrameIndex || m_iCurKeyFrameIndex >= m_KeyFrames.size() - 1)
		return -1;

	_float fRatioOffset = m_KeyFrames[m_iCurKeyFrameIndex + 1].fTimeStamp - m_KeyFrames[m_iCurKeyFrameIndex].fTimeStamp;
	
	return m_fCurrentTime * fRatioOffset;
}

_bool CCutScene_Sector::Get_IsLookAt()
{
	if (-1 == m_iCurKeyFrameIndex)
		return false;

	return m_KeyFrames[m_iCurKeyFrameIndex].bLookTarget;
}

_bool CCutScene_Sector::Get_IsChangeKeyFrame()
{
	if (m_iPreKeyFrameIndex != m_iCurKeyFrameIndex){
		m_iPreKeyFrameIndex = m_iCurKeyFrameIndex;
		return true;
	}
		
	return false;
}

_bool CCutScene_Sector::Play_Sector(_float _fTimeDelta, _vector* _pOutPos)
{
	m_fCurrentTime += _fTimeDelta;
	_float fRatio = m_fCurrentTime / m_fDuration;
	
	if (fRatio > 1.f) {
		m_fCurrentTime = 0.f;
		m_iPreKeyFrameIndex = -1;
		m_iCurKeyFrameIndex = -1;
		return true;
	}

	if(fRatio > m_KeyFrames[m_KeyFrames.size() - 5].fTimeStamp)	{
		m_fCurrentTime = 0.f;
		m_iPreKeyFrameIndex = -1;
		m_iCurKeyFrameIndex = -1;
		return true;
	}

	switch (m_iSectorType) {
	case SPLINE:
		*_pOutPos = Calculate_Position_Spline_Test(fRatio);
		break;
	case LINEAR:
		*_pOutPos = Calculate_Position_Linear(fRatio);
		break;
	}

	Calculate_Index();

	return false;
}

_vector CCutScene_Sector::Calculate_Position_Spline(_float _fRatio)
{
	_uint iSegment = (_uint)(_fRatio * (m_KeyFrames.size() - 3));
	//_float fRatio = (_fRatio - (float)iSegment / (m_KeyFrames.size() - 3)) * (m_KeyFrames.size() - 3);
	_vector vPos0 = XMLoadFloat3(&m_KeyFrames[iSegment].vPosition);
	_vector vPos1 = XMLoadFloat3(&m_KeyFrames[iSegment + 1].vPosition);
	_vector vPos2 = XMLoadFloat3(&m_KeyFrames[iSegment + 2].vPosition);
	_vector vPos3 = XMLoadFloat3(&m_KeyFrames[iSegment + 3].vPosition);

	_vector vLerp0_First = XMVectorLerp(vPos0, vPos1, _fRatio);
	_vector vLerp1_First = XMVectorLerp(vPos1, vPos2, _fRatio);
	_vector vLerp2_First = XMVectorLerp(vPos2, vPos3, _fRatio);

	_vector vLerp0_Second = XMVectorLerp(vLerp0_First, vLerp1_First, _fRatio);
	_vector vLerp1_Second = XMVectorLerp(vLerp1_First, vLerp2_First, _fRatio);

	_vector vResult = XMVectorLerp(vLerp0_Second, vLerp1_Second, _fRatio);

	return vResult;
}

_vector CCutScene_Sector::Calculate_Position_Linear(_float _fRatio)
{

	return _vector();
}

_uint CCutScene_Sector::Find_Span(_float _fRatio)
{
	for (_uint i = 3; i < m_KeyFrames.size(); ++i) {
		if (_fRatio <= m_Knots[i + 1])
			return i;
	}
	return m_KeyFrames.size() - 1;
}

_float CCutScene_Sector::BasisFunction(_uint _i, _uint _iDegree, _float _fRatio)
{
	if (_iDegree == 0) {
		return (_fRatio >= m_Knots[_i] && _fRatio < m_Knots[_i + 1]) ? 1.0f : 0.0f;
	}

	_float w1 = 0.0f, w2 = 0.0f;
	float denom1 = m_Knots[_i + _iDegree] - m_Knots[_i];
	float denom2 = m_Knots[_i + _iDegree + 1] - m_Knots[_i + 1];

	if (denom1 > 0)
		w1 = ((_fRatio - m_Knots[_i]) / denom1) * BasisFunction(_i, _iDegree - 1, _fRatio);

	if (denom2 > 0)
		w2 = ((m_Knots[_i + _iDegree + 1] - _fRatio) / denom2) * BasisFunction(_i + 1, _iDegree - 1, _fRatio);

	return w1 + w2;
}

void CCutScene_Sector::Calculate_Index()
{
	if (m_iCurKeyFrameIndex >= m_KeyFrames.size() - 1)
		return;

	if (m_fCurrentTime > m_KeyFrames[m_iCurKeyFrameIndex + 1].fTimeStamp) {
		m_iCurKeyFrameIndex++;
	}

}

_vector CCutScene_Sector::Calculate_Position_Spline_Test(_float _fRatio)
{
	_uint iSpan = Find_Span(_fRatio);
	_vector vPos = {  };

	for (_uint i = iSpan - 3; i <= iSpan; ++i) {
		_float fWeight = BasisFunction(i, 3, _fRatio);
		vPos += XMLoadFloat3(&m_KeyFrames[i].vPosition) * fWeight;

		cout << "제어점: " << i << "m_KeyFrames[i].vPosition" << m_KeyFrames[i].vPosition.x << "   " << m_KeyFrames[i].vPosition.y << "   " << m_KeyFrames[i].vPosition.z << ", fWieght: " << fWeight << endl;
		cout << "vPos: " << vPos.m128_f32[0] << "    " << vPos.m128_f32[1] << "    " << vPos.m128_f32[2] << endl;
	}
	cout << "!!!!!!!최종 Pos: " << vPos.m128_f32[0] << "    " << vPos.m128_f32[1] << "    " << vPos.m128_f32[2] << endl;
	cout << "=====================" << endl;

	return vPos;
}

void CCutScene_Sector::Sort_Sector()
{
	sort(m_KeyFrames.begin(), m_KeyFrames.end(), [](CUTSCENE_KEYFRAME& _src, CUTSCENE_KEYFRAME& _dst) {

		if (_src.fTimeStamp < _dst.fTimeStamp)
			return true;
		return false;
		});

	m_Knots.resize(m_KeyFrames.size() + 5 + 1);

	_int iCount = 0;

	for (auto& Knots : m_Knots) {
		if (0 == iCount || 1 == iCount || 2 == iCount) {
			Knots = 0.f;
			iCount++;
			continue;
		}
		else if (m_Knots.size() - 1 == iCount ) {
			Knots = 1.f;
			iCount++;
			continue;
		}
		else if (m_Knots.size() - 3 == iCount || m_Knots.size() - 2 == iCount) {
			Knots = m_KeyFrames.back().fTimeStamp;
			iCount++;
			continue;
		}

		Knots = m_KeyFrames[iCount - 3].fTimeStamp;

		iCount++;
	}
}

void CCutScene_Sector::Add_KeyFrame(CUTSCENE_KEYFRAME _tKeyFrame)
{
	for (auto& Frame : m_KeyFrames) {
		if (true == XMVector3Equal(XMLoadFloat3(&Frame.vPosition), XMLoadFloat3(&_tKeyFrame.vPosition)))
			return;
	}

	m_KeyFrames.push_back(_tKeyFrame);
}

CCutScene_Sector* CCutScene_Sector::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, void* pArg)
{
	CCutScene_Sector* pInstance = new CCutScene_Sector(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CCutScene_Sector");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCutScene_Sector* CCutScene_Sector::Clone()
{
	CCutScene_Sector* pInstance = new CCutScene_Sector(*this);

	if (FAILED(pInstance->Initialize_Clone()))
	{
		MSG_BOX("Failed to Cloned : CCutScene_Sector");
		Safe_Release(pInstance);
	}

	return pInstance;
}
void CCutScene_Sector::Free()
{
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);

	__super::Free();
}
