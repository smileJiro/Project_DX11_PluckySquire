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

_bool CCutScene_Sector::Play_Sector(_float _fTimeDelta, _vector* _pOutPos)
{
	m_fCurrentTime += _fTimeDelta;
	_float fRatio = m_fCurrentTime / m_KeyFrames.back().fTimeStamp;

	if (fRatio > 1.f) {
		m_fCurrentTime = 0.f;
		return true;
	}

	switch (m_iSectorType) {
	case SPLINE:
		*_pOutPos = Calculate_Position_Spline(fRatio);
		break;
	case LINEAR:
		*_pOutPos = Calculate_Position_Linear(fRatio);
		break;
	}

	return false;
}

_vector CCutScene_Sector::Calculate_Position_Spline(_float _fRatio)
{
	_uint iSegment = (_uint)(_fRatio * (m_KeyFrames.size() - 3));
	_float fRatio = _fRatio * (m_KeyFrames.size() - 3) - iSegment;

	_vector vPos0 = XMLoadFloat3(&m_KeyFrames[iSegment].vPosition);
	_vector vPos1 = XMLoadFloat3(&m_KeyFrames[iSegment + 1].vPosition);
	_vector vPos2 = XMLoadFloat3(&m_KeyFrames[iSegment + 2].vPosition);
	_vector vPos3 = XMLoadFloat3(&m_KeyFrames[iSegment + 3].vPosition);

	_vector vLerp0_First = XMVectorLerp(vPos0, vPos1, fRatio);
	_vector vLerp1_First = XMVectorLerp(vPos1, vPos2, fRatio);
	_vector vLerp2_First = XMVectorLerp(vPos2, vPos3, fRatio);

	_vector vLerp0_Second = XMVectorLerp(vLerp0_First, vLerp1_First, fRatio);
	_vector vLerp1_Second = XMVectorLerp(vLerp1_First, vLerp2_First, fRatio);

	_vector vResult = XMVectorLerp(vLerp0_Second, vLerp1_Second, fRatio);

	return vResult;
}

_vector CCutScene_Sector::Calculate_Position_Linear(_float _fRatio)
{
	return _vector();
}

void CCutScene_Sector::Sort_Sector()
{
	sort(m_KeyFrames.begin(), m_KeyFrames.end(), [](CUTSCENE_KEYFRAME& _src, CUTSCENE_KEYFRAME& _dst) {

		if (_src.fTimeStamp < _dst.fTimeStamp)
			return true;
		return false;
		});
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
