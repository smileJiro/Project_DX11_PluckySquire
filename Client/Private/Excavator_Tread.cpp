#include "stdafx.h"
#include "Excavator_Tread.h"
#include "GameInstance.h"
#include "ModelObject.h"

CExcavator_Tread::CExcavator_Tread(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CCharacter(_pDevice, _pContext)
{
}

CExcavator_Tread::CExcavator_Tread(const CExcavator_Tread& _Prototype)
    :CCharacter(_Prototype)
{
}

HRESULT CExcavator_Tread::Initialize(void* _pArg)
{
	TREAD_DESC* pDesc = static_cast<TREAD_DESC*>(_pArg);

	// Save Desc
	
	// Add Desc
	pDesc->eStartCoord = COORDINATE_2D;
	pDesc->iNumPartObjects = TREAD_LAST;
	pDesc->isCoordChangeEnable = false;
	pDesc->iObjectGroupID = OBJECT_GROUP::MONSTER;
    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    if (FAILED(Ready_PartObjects(pDesc)))
        return E_FAIL;

    return S_OK;
}

void CExcavator_Tread::Priority_Update(_float _fTimeDelta)
{

	__super::Priority_Update(_fTimeDelta);
}

void CExcavator_Tread::Update(_float _fTimeDelta)
{
	__super::Update(_fTimeDelta);
}

void CExcavator_Tread::Late_Update(_float _fTimeDelta)
{
	__super::Late_Update(_fTimeDelta);
}

void CExcavator_Tread::Start_Wheel()
{
	static_cast<CModelObject*>(m_PartObjects[TREAD_LUGNUTS_L])->Set_PlayingAnim(true);
	static_cast<CModelObject*>(m_PartObjects[TREAD_LUGNUTS_M])->Set_PlayingAnim(true);
	static_cast<CModelObject*>(m_PartObjects[TREAD_LUGNUTS_R])->Set_PlayingAnim(true);

	static_cast<CModelObject*>(m_PartObjects[TREAD_LINES])->Set_PlayingAnim(true);
}

void CExcavator_Tread::Stop_Wheel()
{
	static_cast<CModelObject*>(m_PartObjects[TREAD_LUGNUTS_L])->Set_PlayingAnim(false);
	static_cast<CModelObject*>(m_PartObjects[TREAD_LUGNUTS_M])->Set_PlayingAnim(false);
	static_cast<CModelObject*>(m_PartObjects[TREAD_LUGNUTS_R])->Set_PlayingAnim(false);

	static_cast<CModelObject*>(m_PartObjects[TREAD_LINES])->Set_PlayingAnim(false);
}

void CExcavator_Tread::Start_Part_HitRender()
{
	for (_uint i = 0; i < TREAD_LAST; ++i)
	{
		static_cast<CModelObject*>(m_PartObjects[i])->Start_HitRender();
	}
}

HRESULT CExcavator_Tread::Ready_PartObjects(TREAD_DESC* _pDesc)
{
	{/* Part Inner */
		CModelObject::MODELOBJECT_DESC Desc{};
		Desc.iCurLevelID = m_iCurLevelID;
		Desc.pParentMatrices[COORDINATE_2D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_2D);
		Desc.Build_2D_Transform(_float2(0.0f, 0.0f), _float2(1.0f, 1.0f));
		Desc.Build_2D_Model(m_iCurLevelID, TEXT("Excavator_Treads"), TEXT("Prototype_Component_Shader_VtxPosTex"));
		Desc.eStartCoord = COORDINATE_2D;

		m_PartObjects[TREAD_INNER] = static_cast<CPartObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_ModelObject"), &Desc));
		if (nullptr == m_PartObjects[TREAD_INNER])
			return E_FAIL;
		static_cast<CModelObject*>(m_PartObjects[TREAD_INNER])->Switch_Animation(TREAD_INNER);
	}/* Part Inner */

	{/* Part Outer */
		CModelObject::MODELOBJECT_DESC Desc{};
		Desc.iCurLevelID = m_iCurLevelID;
		Desc.pParentMatrices[COORDINATE_2D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_2D);
		Desc.Build_2D_Transform(_float2(0.0f, 0.0f), _float2(1.0f, 1.0f));
		Desc.Build_2D_Model(m_iCurLevelID, TEXT("Excavator_Treads"), TEXT("Prototype_Component_Shader_VtxPosTex"));
		Desc.eStartCoord = COORDINATE_2D;

		m_PartObjects[TREAD_OUTER] = static_cast<CPartObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_ModelObject"), &Desc));
		if (nullptr == m_PartObjects[TREAD_OUTER])
			return E_FAIL;
		static_cast<CModelObject*>(m_PartObjects[TREAD_OUTER])->Switch_Animation(TREAD_OUTER);
	}/* Part Outer */

	{/* Part Lines */
		CModelObject::MODELOBJECT_DESC Desc{};
		Desc.iCurLevelID = m_iCurLevelID;
		Desc.pParentMatrices[COORDINATE_2D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_2D);
		Desc.Build_2D_Transform(_float2(0.0f, -80.0f), _float2(1.0f, 1.0f));
		Desc.Build_2D_Model(m_iCurLevelID, TEXT("Excavator_Treads"), TEXT("Prototype_Component_Shader_VtxPosTex"));
		Desc.eStartCoord = COORDINATE_2D;

		m_PartObjects[TREAD_LINES] = static_cast<CPartObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_ModelObject"), &Desc));
		if (nullptr == m_PartObjects[TREAD_LINES])
			return E_FAIL;
		static_cast<CModelObject*>(m_PartObjects[TREAD_LINES])->Switch_Animation(TREAD_LINES);
		static_cast<CModelObject*>(m_PartObjects[TREAD_LINES])->Set_PlayingAnim(false);
	}/* Part Lines */

	{/* Part Wheel_L */
		CModelObject::MODELOBJECT_DESC Desc{};
		Desc.iCurLevelID = m_iCurLevelID;
		Desc.pParentMatrices[COORDINATE_2D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_2D);
		Desc.Build_2D_Transform(_float2(-230.f, 0.0f), _float2(1.0f, 1.0f));
		Desc.Build_2D_Model(m_iCurLevelID, TEXT("Excavator_Treads"), TEXT("Prototype_Component_Shader_VtxPosTex"));
		Desc.eStartCoord = COORDINATE_2D;

		m_PartObjects[TREAD_WHEEL_L] = static_cast<CPartObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_ModelObject"), &Desc));
		if (nullptr == m_PartObjects[TREAD_WHEEL_L])
			return E_FAIL;
		static_cast<CModelObject*>(m_PartObjects[TREAD_WHEEL_L])->Switch_Animation(TREAD_WHEEL_L);
	}/* Part Wheel_L */

	{/* Part Wheel_M */
		CModelObject::MODELOBJECT_DESC Desc{};
		Desc.iCurLevelID = m_iCurLevelID;
		Desc.pParentMatrices[COORDINATE_2D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_2D);
		Desc.Build_2D_Transform(_float2(0.0f, 0.0f), _float2(1.0f, 1.0f));
		Desc.Build_2D_Model(m_iCurLevelID, TEXT("Excavator_Treads"), TEXT("Prototype_Component_Shader_VtxPosTex"));
		Desc.eStartCoord = COORDINATE_2D;

		m_PartObjects[TREAD_WHEEL_M] = static_cast<CPartObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_ModelObject"), &Desc));
		if (nullptr == m_PartObjects[TREAD_WHEEL_M])
			return E_FAIL;
		static_cast<CModelObject*>(m_PartObjects[TREAD_WHEEL_M])->Switch_Animation(TREAD_WHEEL_M);
	}/* Part Wheel_M */

	{/* Part Wheel_R */
		CModelObject::MODELOBJECT_DESC Desc{};
		Desc.iCurLevelID = m_iCurLevelID;
		Desc.pParentMatrices[COORDINATE_2D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_2D);
		Desc.Build_2D_Transform(_float2(238.f, 0.0f), _float2(1.0f, 1.0f));
		Desc.Build_2D_Model(m_iCurLevelID, TEXT("Excavator_Treads"), TEXT("Prototype_Component_Shader_VtxPosTex"));
		Desc.eStartCoord = COORDINATE_2D;

		m_PartObjects[TREAD_WHEEL_R] = static_cast<CPartObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_ModelObject"), &Desc));
		if (nullptr == m_PartObjects[TREAD_WHEEL_R])
			return E_FAIL;
		static_cast<CModelObject*>(m_PartObjects[TREAD_WHEEL_R])->Switch_Animation(TREAD_WHEEL_R);
	}/* Part Wheel_R */

	{/* Part Lugnuts_L */
		CModelObject::MODELOBJECT_DESC Desc{};
		Desc.iCurLevelID = m_iCurLevelID;
		Desc.pParentMatrices[COORDINATE_2D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_2D);
		Desc.Build_2D_Transform(_float2(-230.f, -65.0f), _float2(1.0f, 1.0f));
		Desc.Build_2D_Model(m_iCurLevelID, TEXT("Excavator_Treads"), TEXT("Prototype_Component_Shader_VtxPosTex"));
		Desc.eStartCoord = COORDINATE_2D;

		m_PartObjects[TREAD_LUGNUTS_L] = static_cast<CPartObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_ModelObject"), &Desc));
		if (nullptr == m_PartObjects[TREAD_LUGNUTS_L])
			return E_FAIL;
		static_cast<CModelObject*>(m_PartObjects[TREAD_LUGNUTS_L])->Switch_Animation(TREAD_LUGNUTS_L);
		static_cast<CModelObject*>(m_PartObjects[TREAD_LUGNUTS_L])->Set_PlayingAnim(false);
	}/* Part Lugnuts_L */

	{/* Part Lugnuts_M */
		CModelObject::MODELOBJECT_DESC Desc{};
		Desc.iCurLevelID = m_iCurLevelID;
		Desc.pParentMatrices[COORDINATE_2D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_2D);
		Desc.Build_2D_Transform(_float2(0.0f, -60.0f), _float2(1.0f, 1.0f));
		Desc.Build_2D_Model(m_iCurLevelID, TEXT("Excavator_Treads"), TEXT("Prototype_Component_Shader_VtxPosTex"));
		Desc.eStartCoord = COORDINATE_2D;

		m_PartObjects[TREAD_LUGNUTS_M] = static_cast<CPartObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_ModelObject"), &Desc));
		if (nullptr == m_PartObjects[TREAD_LUGNUTS_M])
			return E_FAIL;
		static_cast<CModelObject*>(m_PartObjects[TREAD_LUGNUTS_M])->Switch_Animation(TREAD_LUGNUTS_L);
		static_cast<CModelObject*>(m_PartObjects[TREAD_LUGNUTS_M])->Set_PlayingAnim(false);
	}/* Part Lugnuts_M */

	{/* Part Lugnuts_R */
		CModelObject::MODELOBJECT_DESC Desc{};
		Desc.iCurLevelID = m_iCurLevelID;
		Desc.pParentMatrices[COORDINATE_2D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_2D);
		Desc.Build_2D_Transform(_float2(238.f, -65.0f), _float2(1.0f, 1.0f));
		Desc.Build_2D_Model(m_iCurLevelID, TEXT("Excavator_Treads"), TEXT("Prototype_Component_Shader_VtxPosTex"));
		Desc.eStartCoord = COORDINATE_2D;

		m_PartObjects[TREAD_LUGNUTS_R] = static_cast<CPartObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_ModelObject"), &Desc));
		if (nullptr == m_PartObjects[TREAD_LUGNUTS_R])
			return E_FAIL;
		static_cast<CModelObject*>(m_PartObjects[TREAD_LUGNUTS_R])->Switch_Animation(TREAD_LUGNUTS_L);
		static_cast<CModelObject*>(m_PartObjects[TREAD_LUGNUTS_R])->Set_PlayingAnim(false);
	}/* Part Lugnuts_R */
	

    return S_OK;
}

CExcavator_Tread* CExcavator_Tread::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CExcavator_Tread* pInstance = new CExcavator_Tread(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CExcavator_Tread");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CExcavator_Tread::Clone(void* _pArg)
{
	CExcavator_Tread* pInstance = new CExcavator_Tread(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : CExcavator_Tread");
		Safe_Release(pInstance);
	}

	return pInstance;
}
void CExcavator_Tread::Free()
{

	__super::Free();
}
