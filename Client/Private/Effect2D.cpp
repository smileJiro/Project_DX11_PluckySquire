#include "stdafx.h"
#include "Effect2D.h"
#include "GameInstance.h"
#include "Section_Manager.h"

CEffect2D::CEffect2D(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CModelObject(_pDevice, _pContext)
{
}

CEffect2D::CEffect2D(const CEffect2D& _Prototype)
	:CModelObject(_Prototype)
{
}

HRESULT CEffect2D::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CEffect2D::Initialize(void* _pArg)
{
	EFFECT2D_DESC* pDesc = static_cast<EFFECT2D_DESC*>(_pArg);
	
	// Save Desc
	m_strEffectModelPrototypeTag = pDesc->strModelPrototypeTag_2D;
	m_eEffectModelPrototypeLevelID = (LEVEL_ID)pDesc->iModelPrototypeLevelID_2D;
	// Add Desc 
	pDesc->eStartCoord = COORDINATE_2D;
	pDesc->iObjectGroupID = OBJECT_GROUP::EFFECT2D;
	pDesc->isCoordChangeEnable = false;
	pDesc->iShaderPass_2D = (_uint)PASS_VTXPOSTEX::SPRITE2D;
	pDesc->pParentMatrices[COORDINATE_2D] = nullptr;
	pDesc->pParentMatrices[COORDINATE_3D] = nullptr;
	pDesc->strShaderPrototypeTag_2D = TEXT("Prototype_Component_Shader_VtxPosTex");

	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;

	/* Set Anim End */
	Register_OnAnimEndCallBack(bind(&CEffect2D::On_AnimEnd, this, placeholders::_1, placeholders::_2));

	m_isPooling = true;
	
	return S_OK;
}

void CEffect2D::Priority_Update(_float _fTimeDelta)
{
	/* Delay Time */
	if (false == m_isPlay)
	{
		m_vDelayTime.y += _fTimeDelta;

		if (m_vDelayTime.x <= m_vDelayTime.y)
		{
			m_vDelayTime.y = 0.0f;
			m_isPlay = true;
			Switch_Animation(m_iCurAnimIndex);
		}
		return;
	}

	/* Play Time */
	if (true == m_isLoop)
	{
		m_vLifeTime.y += _fTimeDelta;
		if (m_vLifeTime.x <= m_vLifeTime.y)
		{
			m_vLifeTime.y = 0.0f;
			Event_DeleteObject(this);
			CSection_Manager::GetInstance()->Remove_GameObject_FromSectionLayer(Get_Include_Section_Name(), this);
		}
	}


	__super::Priority_Update(_fTimeDelta);
}

void CEffect2D::Update(_float _fTimeDelta)
{
	if (false == m_isPlay)
		return;

	/* Play Animation */
	__super::Update(_fTimeDelta);
}

void CEffect2D::Late_Update(_float _fTimeDelta)
{
	if (false == m_isPlay)
		return;

	__super::Late_Update(_fTimeDelta);
}

HRESULT CEffect2D::Render()
{
	if (false == m_isPlay)
		return S_OK;

	__super::Render();
	return S_OK;
}

HRESULT CEffect2D::Play_Effect(const _wstring& _strSectionKey, _fmatrix _2DWorldMatrix, _float _fDelayTime, _uint _iAnimIndex, _bool _isLoop, _float _fLifeTime)
{
	if (true == _strSectionKey.empty())
		return E_FAIL;

	/* Set World Matrix */
	m_pControllerTransform->Set_WorldMatrix(_2DWorldMatrix);

	/* Set Delay Time */
	m_vDelayTime.x = _fDelayTime;
	m_vDelayTime.y = 0.0f;
	m_isLoop = _isLoop;
	m_iCurAnimIndex = _iAnimIndex;
	m_isPlay = false;
	m_vLifeTime.x = _fLifeTime;

	Set_AnimationLoop(COORDINATE_2D, m_iCurAnimIndex, m_isLoop);
	Event_SetActive(this, true);
	CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(_strSectionKey, this, SECTION_2D_PLAYMAP_EFFECT);
}
//
//HRESULT CEffect2D::Play_Effect(const _wstring& _strSectionKey, _fvector _vPos, _uint _iAnimIndex, _float _fDelayTime, _bool _isLoop, _float2 _vScale, _float _fRadianZ)
//{
//	if (true == _strSectionKey.empty())
//		return E_FAIL;
//
//	/* Set Transform */
//	Set_Scale(_vScale.x, _vScale.y, 0.0f);
//	Set_Position(_vPos);
//	m_pControllerTransform->Rotation(_fRadianZ, XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f));
//
//	/* Set Delay Time */
//	m_vDelayTime.x = _fDelayTime;
//	m_vDelayTime.y = 0.0f;
//	
//	/* Set Anim Data */
//	m_isLoop = _isLoop;
//	m_iCurAnimIndex = _iAnimIndex;
//	m_isPlay = false;
//	
//	Event_SetActive(this, true);
//}

void CEffect2D::On_AnimEnd(COORDINATE _eCoord, _uint _iAnimIdx)
{
	if (m_iCurAnimIndex == _iAnimIdx && false == m_isLoop)
	{
		Event_DeleteObject(this);
		CSection_Manager::GetInstance()->Remove_GameObject_FromSectionLayer(Get_Include_Section_Name(), this);
	}
}

HRESULT CEffect2D::Ready_Components(EFFECT2D_DESC* _pDesc)
{

	return S_OK;
}

void CEffect2D::Active_OnEnable()
{
	/* 액티브 활성화 시 */
	__super::Active_OnEnable();
}

void CEffect2D::Active_OnDisable()
{
	/* 액티브 활성화 시 */
	__super::Active_OnDisable();
}

CEffect2D* CEffect2D::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CEffect2D* pInstance = new CEffect2D(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed Create : CEffect2D");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEffect2D::Clone(void* _pArg)
{
	CEffect2D* pInstance = new CEffect2D(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed Clone : CEffect2D");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffect2D::Free()
{

	__super::Free();
}
