#include "stdafx.h"
#include "Npc_Humgrump.h"

#include "Section_Manager.h"

CNpc_Humgrump::CNpc_Humgrump(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CModelObject(_pDevice, _pContext)
{
}

CNpc_Humgrump::CNpc_Humgrump(const CNpc_Humgrump& _Prototype)
    : CModelObject(_Prototype)
{
}

HRESULT CNpc_Humgrump::Initialize(void* _pArg)
{
	if (nullptr == _pArg)
		return E_FAIL;

	HUMGRUMP_DESC* pDesc = static_cast<HUMGRUMP_DESC*>(_pArg);

	pDesc->eStartCoord = COORDINATE_2D;
	pDesc->isCoordChangeEnable = false;
	pDesc->strShaderPrototypeTag_2D = TEXT("Prototype_Component_Shader_VtxPosTex");
	pDesc->iShaderPass_2D = (_uint)PASS_VTXPOSTEX::SPRITE2D;

	pDesc->tTransform2DDesc.vInitialScaling = _float3(1.f, 1.f, 1.f);

	pDesc->iModelPrototypeLevelID_2D = LEVEL_STATIC;
	pDesc->strModelPrototypeTag_2D = TEXT("Humgrump");
	//pDesc->iObjectGroupID = OBJECT_GROUP::NP;

	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	Switch_Animation(pDesc->iStartAnimIndex);
	Set_AnimationLoop(COORDINATE_2D, pDesc->iStartAnimIndex, pDesc->isLoop);

	if (true == pDesc->isOppositeSide)
		Set_Opposite_Side();

	if (FAILED(CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(pDesc->strSectionTag, this)))
		return E_FAIL;

    return S_OK;
}

void CNpc_Humgrump::Update(_float _fTimeDelta)
{
    __super::Update(_fTimeDelta);
}

void CNpc_Humgrump::Late_Update(_float _fTimeDelta)
{
	__super::Late_Update(_fTimeDelta);
}

HRESULT CNpc_Humgrump::Render()
{
	__super::Render();
#ifdef _DEBUG
	//for (auto& p2DCollider : m_p2DColliderComs)
	//{
	//	p2DCollider->Render(SECTION_MGR->Get_Section_RenderTarget_Size(m_strSectionName));
	//}
#endif // _DEBUG

	return S_OK;
}

void CNpc_Humgrump::Set_Opposite_Side()
{
	_vector vRight = m_pControllerTransform->Get_State(CTransform::STATE_RIGHT);
	Get_ControllerTransform()->Set_State(CTransform::STATE_RIGHT, -1.f * XMVectorAbs(vRight));
	vRight = m_pControllerTransform->Get_State(CTransform::STATE_RIGHT);
}

CNpc_Humgrump* CNpc_Humgrump::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CNpc_Humgrump* pInstance = new CNpc_Humgrump(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CNpc_Humgrump");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CNpc_Humgrump::Clone(void* _pArg)
{
	CNpc_Humgrump* pInstance = new CNpc_Humgrump(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : CNpc_Humgrump");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNpc_Humgrump::Free()
{
	__super::Free();
}
