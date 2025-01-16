#include "ModelObject.h"
#include "GameInstance.h"
#include "3DModel.h"
#include "2DModel.h"
#include "Controller_Model.h"


CModelObject::CModelObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CPartObject(_pDevice, _pContext)
{
}

CModelObject::CModelObject(const CModelObject& _Prototype)
    : CPartObject(_Prototype)
{
}

HRESULT CModelObject::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CModelObject::Initialize(void* _pArg)
{
    MODELOBJECT_DESC* pDesc = static_cast<MODELOBJECT_DESC*>(_pArg);

    // Save 
    m_iShaderPasses[COORDINATE_2D] = pDesc->iShaderPass_2D;
    m_iShaderPasses[COORDINATE_3D] = pDesc->iShaderPass_3D;

    // Add


    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components(pDesc)))
        return E_FAIL;


    D3D11_VIEWPORT ViewportDesc{};
    _uint iNumViewports = 1;
    m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

    // View Matrix는 IdentityMatrix
    XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
    // Projection Matrix는 Viewport Desc 를 기반으로 생성.
    XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)ViewportDesc.Width, (_float)ViewportDesc.Height, 0.0f, 1.0f));


    return S_OK;
}

void CModelObject::Late_Update(_float _fTimeDelta)
{    /* Add Render Group */
    if (COORDINATE_3D == m_pControllerTransform->Get_CurCoord())
        m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
    else if (COORDINATE_2D == m_pControllerTransform->Get_CurCoord())
        m_pGameInstance->Add_RenderObject(CRenderer::RG_BOOK_2D, this);

    /* Update Parent Matrix */
    __super::Late_Update(_fTimeDelta);
}

HRESULT CModelObject::Render()
{
    if (FAILED(Bind_ShaderResources_WVP()))
        return E_FAIL;
    COORDINATE eCoord = m_pControllerTransform->Get_CurCoord();
	CShader* pShader = m_pShaderComs[eCoord];
	_uint iShaderPass = m_iShaderPasses[eCoord];
    m_pControllerModel->Render(pShader, iShaderPass);

    return S_OK;
}

_bool CModelObject::Is_PickingCursor_Model(_float2 _fCursorPos, _float& _fDst)
{
    // 예외처리
	C3DModel* m_p3DModelCom = static_cast<C3DModel*>( m_pControllerModel->Get_Model(COORDINATE_3D));
    if (nullptr == m_p3DModelCom)
        return false;
    if (nullptr == m_pRayCom)
        return false;

    // 레이 매트릭스 설정.
    m_pRayCom->Update_RayInfoFromCursor(_float2(_fCursorPos.x, _fCursorPos.y),
        m_pGameInstance->Get_TransformInverseMatrix(CPipeLine::D3DTS_VIEW),
        m_pGameInstance->Get_TransformInverseMatrix(CPipeLine::D3DTS_PROJ));

    // 모델의 메쉬를 순회하며, 충돌했다면 바로 리턴하고 dst 값을 넘기자.
    size_t iNumMeshes = m_p3DModelCom->Get_NumMeshes();
    const vector<CMesh*>& vecMeshes = m_p3DModelCom->Get_Meshes();

    for (size_t i = 0; i < iNumMeshes; ++i)
    {
        const vector<_float3>& vecVerticesPos = vecMeshes[i]->Get_VerticesPos();
        const vector<_uint>& vecIndexBuffer = vecMeshes[i]->Get_IndexBuffer();
        _uint iNumTriangles = vecMeshes[i]->Get_NumTriangles();

        for (size_t j = 0; j < iNumTriangles; ++j)
        {
            _bool bResult = false;
            _uint iIdx = j * 3;
            _matrix WorldMatrix = m_pControllerTransform->Get_WorldMatrix();

            bResult = m_pRayCom->Compute_Intersect_Triangle(XMVector3TransformCoord(XMLoadFloat3(&vecVerticesPos[vecIndexBuffer[iIdx]]), WorldMatrix),
                XMVector3TransformCoord(XMLoadFloat3(&vecVerticesPos[vecIndexBuffer[iIdx + 1]]), WorldMatrix),
                XMVector3TransformCoord(XMLoadFloat3(&vecVerticesPos[vecIndexBuffer[iIdx + 2]]), WorldMatrix), _fDst);
            if (bResult)
            {
                return true;
            }
        }
    }


    return false;
}

void CModelObject::Update(_float fTimeDelta)
{
    m_pControllerModel->Play_Animation(fTimeDelta);

	__super::Update(fTimeDelta);
}

HRESULT CModelObject::Change_Coordinate(COORDINATE _eCoordinate, const _float3& _vPosition)
{
	if (FAILED(__super::Change_Coordinate(_eCoordinate, _vPosition)))
		return E_FAIL;

    return	m_pControllerModel->Change_Coordinate(_eCoordinate);
}

void CModelObject::Set_AnimationLoop(_uint iIdx, _bool bIsLoop)
{
    m_pControllerModel->Set_AnimationLoop(iIdx, bIsLoop);
}

void CModelObject::Set_Animation(_uint iIdx)
{
    m_pControllerModel->Set_Animation(iIdx);
}

void CModelObject::Switch_Animation(_uint iIdx)
{
    m_pControllerModel->Switch_Animation(iIdx);
}

void CModelObject::To_NextAnimation()
{
    m_pControllerModel->To_NextAnimation();
}

HRESULT CModelObject::Ready_Components(MODELOBJECT_DESC* _pDesc)
{
    _int iStaticLevelID = m_pGameInstance->Get_StaticLevelID();

    switch (_pDesc->eStartCoord)
    {
    case Engine::COORDINATE_2D:
    {
        /* Com_Shader_3D */
        if (FAILED(Add_Component(iStaticLevelID, _pDesc->strShaderPrototypeTag_3D,
            TEXT("Com_Shader_3D"), reinterpret_cast<CComponent**>(&m_pShaderComs[COORDINATE_3D]))))
            return E_FAIL;
        /* Com_Shader_2D */
        if (FAILED(Add_Component(iStaticLevelID, _pDesc->strShaderPrototypeTag_2D,
            TEXT("Com_Shader_2D"), reinterpret_cast<CComponent**>(&m_pShaderComs[COORDINATE_2D]))))
            return E_FAIL;
        if (true == _pDesc->isCoordChangeEnable)
        {
        }
        break;
    }
    case Engine::COORDINATE_3D:
    {
        /* Com_Shader_3D */
        if (FAILED(Add_Component(iStaticLevelID, _pDesc->strShaderPrototypeTag_3D,
            TEXT("Com_Shader_3D"), reinterpret_cast<CComponent**>(&m_pShaderComs[COORDINATE_3D]))))
            return E_FAIL;
        if (true == _pDesc->isCoordChangeEnable)
        {
            /* Com_Shader_2D */
            if (FAILED(Add_Component(iStaticLevelID, _pDesc->strShaderPrototypeTag_2D,
                TEXT("Com_Shader_2D"), reinterpret_cast<CComponent**>(&m_pShaderComs[COORDINATE_2D]))))
                return E_FAIL;
        }
        break;
    }
    default:
        break;
    }

    CController_Model::CON_MODEL_DESC tModelDesc{};
    tModelDesc.eStartCoord = _pDesc->eStartCoord;
	tModelDesc.isCoordChangeEnable = _pDesc->isCoordChangeEnable;
	tModelDesc.iCurLevelID = iStaticLevelID;
	tModelDesc.i2DModelPrototypeLevelID = _pDesc->iModelPrototypeLevelID_2D;
	tModelDesc.i3DModelPrototypeLevelID = _pDesc->iModelPrototypeLevelID_3D;
	tModelDesc.wstr2DModelPrototypeTag = _pDesc->strModelPrototypeTag_2D;
	tModelDesc.wstr3DModelPrototypeTag = _pDesc->strModelPrototypeTag_3D;

	m_pControllerModel = CController_Model::Create(m_pDevice, m_pContext, &tModelDesc);
    if (nullptr == m_pControllerModel)
        return E_FAIL;

    return S_OK;
}

HRESULT CModelObject::Bind_ShaderResources_WVP()
{
    switch (m_pControllerTransform->Get_CurCoord())
    {
    case Engine::COORDINATE_2D:
        if (FAILED(m_pShaderComs[COORDINATE_2D]->Bind_Matrix("g_WorldMatrix", &m_WorldMatrices[COORDINATE_2D])))
            return E_FAIL;

        if (FAILED(m_pShaderComs[COORDINATE_2D]->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
            return E_FAIL;

        /* 추후 우리 그리고자하는 렌더 타겟 사이즈로 projectionMatrix 를 변경해줘야함. */
        // m_pGameInstance->Get_RT_Size(TEXT("")); // 렌더타겟 사이즈 받아오는 함수는 만들어뒀음. 2D 3D 공간
        if (FAILED(m_pShaderComs[COORDINATE_2D]->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
            return E_FAIL;

        break;
    case Engine::COORDINATE_3D:
        if (FAILED(m_pShaderComs[COORDINATE_3D]->Bind_Matrix("g_WorldMatrix", &m_WorldMatrices[COORDINATE_3D])))
            return E_FAIL;

        if (FAILED(m_pShaderComs[COORDINATE_3D]->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
            return E_FAIL;

        if (FAILED(m_pShaderComs[COORDINATE_3D]->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
            return E_FAIL;

        break;
    default:
        break;
    }

    return S_OK;
}


CModelObject* CModelObject::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CModelObject* pInstance = new CModelObject(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CModelObject");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CModelObject::Clone(void* _pArg)
{
    CModelObject* pInstance = new CModelObject(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CModelObject");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CModelObject::Free()
{
    Safe_Release(m_pControllerModel);

    for (_int i = 0; i < COORDINATE_LAST; ++i)
    {
        Safe_Release(m_pShaderComs[i]);
    }

    __super::Free();
}

