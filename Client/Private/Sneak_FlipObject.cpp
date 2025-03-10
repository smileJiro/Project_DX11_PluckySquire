#include "stdafx.h"
#include "Sneak_FlipObject.h"
#include "GameInstance.h"

CSneak_FlipObject::CSneak_FlipObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CModelObject(_pDevice, _pContext)
{
}

CSneak_FlipObject::CSneak_FlipObject(const CSneak_FlipObject& _Prototype)
	: CModelObject(_Prototype)
{
}

HRESULT CSneak_FlipObject::Initialize(void* _pArg)
{
	FLIPOBJECT_DESC* pDesc = static_cast<FLIPOBJECT_DESC*>(_pArg);

	pDesc->eStartCoord = COORDINATE_2D;
	pDesc->isCoordChangeEnable = false;
	pDesc->strShaderPrototypeTag_2D = TEXT("Prototype_Component_Shader_VtxPosTex");
	pDesc->iShaderPass_2D = (_uint)PASS_VTXPOSTEX::SPRITE2D;

	pDesc->tTransform2DDesc.vInitialScaling = _float3(1.f, 1.f, 1.f);

	pDesc->eActorType = ACTOR_TYPE::LAST;
	pDesc->pActorDesc = nullptr;
	pDesc->iModelPrototypeLevelID_2D = pDesc->iCurLevelID;

	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	
	m_iInitAnim = m_iCurAnim = pDesc->_iInitAnim;
	m_iFlipAnim1 = pDesc->_iFlipAnim1;
	m_iFlipAnim2 = pDesc->_iFlipAnim2;
	m_iFlipAnim1End = pDesc->_iFlipAnim1End;
	m_iFlipAnim2End = pDesc->_iFlipAnim2End;

	if (0 <= m_iCurAnim)
		Switch_Animation(m_iCurAnim);

	return S_OK;
}

void CSneak_FlipObject::On_AnimEnd(COORDINATE _eCoord, _uint _iAnimIdx)
{
	if (0 > m_iCurAnim)
		return;

	if (_iAnimIdx == m_iFlipAnim1)
	{
		//m_isFlipped = !m_isFlipped;

		Switch_Animation(m_iFlipAnim1End);
		m_iCurAnim = m_iFlipAnim1End;
	}
	if (_iAnimIdx == m_iFlipAnim2)
	{
		//m_isFlipped = !m_isFlipped;

		Switch_Animation(m_iFlipAnim2End);
		m_iCurAnim = m_iFlipAnim2End;

	}


}

void CSneak_FlipObject::Flip()
{
	if (0 > m_iCurAnim)
		return;

	if (m_iFlipAnim1 == m_iCurAnim || m_iFlipAnim1End == m_iCurAnim)
	{
		m_iCurAnim = m_iFlipAnim2;
	}
	else if (m_iFlipAnim2 == m_iCurAnim || m_iFlipAnim2End == m_iCurAnim)
	{
		m_iCurAnim = m_iFlipAnim1;
	}
	
	m_isFlipped = !m_isFlipped;
	Switch_Animation(m_iCurAnim);
}

CSneak_FlipObject* CSneak_FlipObject::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CSneak_FlipObject* pInstance = new CSneak_FlipObject(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSneak_FlipObject");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSneak_FlipObject::Clone(void* _pArg)
{
	CSneak_FlipObject* pInstance = new CSneak_FlipObject(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : CSneak_FlipObject");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSneak_FlipObject::Free()
{
	__super::Free();
}
