#include "stdafx.h"
#include "ScrollModelObject.h"

CScrollModelObject::CScrollModelObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CModelObject(_pDevice, _pContext)
{
}

CScrollModelObject::CScrollModelObject(const CScrollModelObject& _Prototype)
	:CModelObject(_Prototype)
{
}

HRESULT CScrollModelObject::Initialize(void* _pArg)
{
	SCROLLMODELOBJ_DESC* pDesc = static_cast<SCROLLMODELOBJ_DESC*>(_pArg);
	m_fSectionSize = pDesc->vSectionSize;

	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;
	return S_OK;
}

void CScrollModelObject::Update(_float _fTimeDelta)
{
	__super::Update(_fTimeDelta);

	Scroll(_fTimeDelta);
}

void CScrollModelObject::Scroll(_float _fTimeDelta)
{
	_vector vPos = Get_FinalPosition();
	_float2 fPlayerPos = { XMVectorGetX(vPos), XMVectorGetY(vPos) };
	_float fDefaultWitdh = (m_fSectionSize.x * 0.5f);
	if (-fDefaultWitdh > fPlayerPos.x)
	{
		Set_Position(XMVectorSetX(vPos, fPlayerPos.x + m_fSectionSize.x));
	}
	if (fDefaultWitdh < fPlayerPos.x)
	{
		Set_Position(XMVectorSetX(vPos, fPlayerPos.x - m_fSectionSize.x));
	}
}

void CScrollModelObject::Set_Direction(T_DIRECTION _eTDir)
{
	switch (_eTDir)
	{
	case Client::T_DIRECTION::LEFT:
	{
		_vector vRight = m_pControllerTransform->Get_State(CTransform::STATE_RIGHT);
		Get_ControllerTransform()->Set_State(CTransform::STATE_RIGHT, -XMVectorAbs(vRight));
		break;
	}
	case Client::T_DIRECTION::RIGHT:
	{
		_vector vRight = m_pControllerTransform->Get_State(CTransform::STATE_RIGHT);
		Get_ControllerTransform()->Set_State(CTransform::STATE_RIGHT, XMVectorAbs(vRight));
		break;
	}
	default:
		break;
	}
}
