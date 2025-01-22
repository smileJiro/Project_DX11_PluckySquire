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

	*_pOutPos = Calculate_Position(fRatio);

	return false;
}

_vector CCutScene_Sector::Calculate_Position(_float _fRatio)
{
	_uint iSegment = (_uint)(_fRatio * (m_KeyFrames.size() - 3));
	_float fRatio = _fRatio * (m_KeyFrames.size() - 3) - iSegment;

	_vector vPos0 = m_KeyFrames[iSegment].vPosition;
	_vector vPos1 = m_KeyFrames[iSegment + 1].vPosition;
	_vector vPos2 = m_KeyFrames[iSegment + 2].vPosition;
	_vector vPos3 = m_KeyFrames[iSegment + 3].vPosition;

	_vector vLerp0_First = XMVectorLerp(vPos0, vPos1, fRatio);
	_vector vLerp1_First = XMVectorLerp(vPos1, vPos2, fRatio);
	_vector vLerp2_First = XMVectorLerp(vPos2, vPos3, fRatio);

	_vector vLerp0_Second = XMVectorLerp(vLerp0_First, vLerp1_First, fRatio);
	_vector vLerp1_Second = XMVectorLerp(vLerp1_First, vLerp2_First, fRatio);

	_vector vResult = XMVectorLerp(vLerp0_Second, vLerp1_Second, fRatio);

	return vResult;
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
