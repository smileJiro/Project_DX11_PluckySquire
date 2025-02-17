#include "Controller_Model.h"
#include "GameInstance.h"
#include "2DModel.h"
#include "3DModel.h"

CController_Model::CController_Model(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :m_pDevice(_pDevice)
    , m_pContext(_pContext)
    , m_pGameInstance(CGameInstance::GetInstance())
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
    Safe_AddRef(m_pGameInstance);
}

HRESULT CController_Model::Initialize(CON_MODEL_DESC* _pDesc)
{
    m_eCurCoord = _pDesc->eStartCoord;
    m_isCoordChangeEnable = _pDesc->isCoordChangeEnable;

    if (FAILED(Ready_Models(_pDesc)))
        return E_FAIL;


    return S_OK;
}
//3d: model 파일이 있음 -> Loader에서 model파일을 읽고 3DModel의 Prototype을 만듦 -> 여기서 PrototypeTag로 ClonePrototype을 함
//2D : 
HRESULT CController_Model::Ready_Models(CON_MODEL_DESC* _pDesc)
{
    switch (m_eCurCoord)
    {
    case Engine::COORDINATE_2D:
    {
 
        CComponent* pComponent = static_cast<CComponent*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_COMPONENT, _pDesc->i2DModelPrototypeLevelID, _pDesc->wstr2DModelPrototypeTag, nullptr));
        if (nullptr == pComponent)
            return E_FAIL;

        m_ModelComs[COORDINATE_2D] = static_cast<C2DModel*>(pComponent);
        if (true == m_isCoordChangeEnable)
        {

            pComponent = static_cast<CComponent*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_COMPONENT, _pDesc->i3DModelPrototypeLevelID, _pDesc->wstr3DModelPrototypeTag, nullptr));
            if (nullptr == pComponent)
                return E_FAIL;

            m_ModelComs[COORDINATE_3D] = static_cast<C3DModel*>(pComponent);
        }
    }
    break;
    case Engine::COORDINATE_3D:
    {

        CComponent* pComponent = static_cast<CComponent*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_COMPONENT, _pDesc->i3DModelPrototypeLevelID, _pDesc->wstr3DModelPrototypeTag, nullptr));
        if (nullptr == pComponent)
            return E_FAIL;

        m_ModelComs[COORDINATE_3D] = static_cast<C3DModel*>(pComponent);

        if (true == m_isCoordChangeEnable)
        {
            pComponent = static_cast<CComponent*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_COMPONENT, _pDesc->i2DModelPrototypeLevelID, _pDesc->wstr2DModelPrototypeTag, nullptr));
            if (nullptr == pComponent)
                return E_FAIL;

            m_ModelComs[COORDINATE_2D] = static_cast<C2DModel*>(pComponent);
        }
    }
    break;
    default:
        break;
    }


    return S_OK;
}


HRESULT CController_Model::Render(CShader* _Shader, _uint _iShaderPass)
{
    return 	m_ModelComs[m_eCurCoord]->Render(_Shader, _iShaderPass);
}

HRESULT CController_Model::Render_Default(CShader* _pShader, _uint _iShaderPass)
{
    return m_ModelComs[m_eCurCoord]->Render_Default(_pShader, _iShaderPass);
}


HRESULT CController_Model::Change_Coordinate(COORDINATE _eCoordinate)
{
    m_eCurCoord = _eCoordinate;

    return S_OK;
}

void CController_Model::Play_Animation(_float fTimeDelta, _bool _bReverse)
{
	if (m_ModelComs[m_eCurCoord])
    {
        if (m_ModelComs[m_eCurCoord]->Is_AnimModel())
        {
            if (m_ModelComs[m_eCurCoord]->Play_Animation(fTimeDelta, _bReverse))
            {
                Invoke_OnAnimEndCallBack(m_eCurCoord, m_ModelComs[m_eCurCoord]->Get_CurrentAnimIndex());
            }
        }
        else
            m_ModelComs[m_eCurCoord]->Set_DuringAnimation(false);

    }
}

void CController_Model::Invoke_OnAnimEndCallBack(COORDINATE _eCoord, _uint iAnimIdx)
{
    for (auto& callback : m_listAnimEndCallBack) 
        callback(_eCoord, iAnimIdx); 
}

HRESULT CController_Model::Binding_TextureIndex_To_3D(_uint _iIndex, _uint _eTextureType, _uint _iMaterialIndex)
{
    if (nullptr == m_ModelComs[COORDINATE_3D])
        return E_FAIL;
    static_cast<C3DModel*>(m_ModelComs[COORDINATE_3D])->Binding_TextureIndex(_iIndex, _eTextureType, _iMaterialIndex);
    return S_OK;
}

_uint CController_Model::Get_TextureIndex_To_3D(_uint _eTextureType, _uint _iMaterialIndex)
{
    if (nullptr == m_ModelComs[COORDINATE_3D])
        return E_FAIL;
    return static_cast<C3DModel*>(m_ModelComs[COORDINATE_3D])->Get_TextureIndex(_eTextureType, _iMaterialIndex);
}



void CController_Model::Set_AnimationLoop(COORDINATE _eCoord, _uint iIdx, _bool bIsLoop)
{
	m_ModelComs[_eCoord]->Set_AnimationLoop(iIdx, bIsLoop);
}

void CController_Model::Set_Animation(_uint iIdx, _bool _bReverse)
{
	m_ModelComs[m_eCurCoord]->Set_Animation(iIdx, _bReverse);
}

void CController_Model::Switch_Animation(_uint iIdx, _bool _bReverse)
{
	m_ModelComs[m_eCurCoord]->Switch_Animation(iIdx, _bReverse);
}

void CController_Model::To_NextAnimation()
{
    m_ModelComs[m_eCurCoord]->To_NextAnimation();
}


CController_Model* CController_Model::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, CON_MODEL_DESC* _pDesc)
{
    CController_Model* pInstance = new CController_Model(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize(_pDesc)))
    {
        //MSG_BOX("Failed to Created : CController_Model");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CController_Model::Free()
{
    for (auto& pModel : m_ModelComs)
    {
		Safe_Release(pModel);
    }
    Safe_Release(m_pGameInstance);
    Safe_Release(m_pContext);
    Safe_Release(m_pDevice);


    __super::Free();
}
