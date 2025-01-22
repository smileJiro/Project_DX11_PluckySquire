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
    m_strModelPrototypeTag[COORDINATE_2D] = pDesc->strModelPrototypeTag_2D;
    m_strModelPrototypeTag[COORDINATE_3D] = pDesc->strModelPrototypeTag_3D;
    // Add


    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components(pDesc)))
        return E_FAIL;



    // View Matrix는 IdentityMatrix
    XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());

    // Projection Matrix는 Viewport Desc 를 기반으로 생성.
    // 2025-01-16 박예슬 수정 : Viewport Desc -> Rendertarget Size

    _float2 fRTSize = m_pGameInstance->Get_RT_Size(L"Target_Book_2D");
    
    XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((_float)fRTSize.x, (_float)fRTSize.y, 0.0f, 1.0f));


    return S_OK;
}

void CModelObject::Priority_Update(_float _fTimeDelta)
{
    __super::Priority_Update(_fTimeDelta);
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
    int a = 0;
#ifdef _DEBUG
    if (m_iInstanceID == 666)
    {
        a = 1;
        cout << a << endl;
    }
#endif // _DEBUG

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

        for (_uint j = 0; j < iNumTriangles; ++j)
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

_bool CModelObject::Is_PickingCursor_Model_Test(_float2 _fCursorPos, _float& _fDst)
{
    // 예외처리
    C3DModel* m_p3DModelCom = static_cast<C3DModel*>(m_pControllerModel->Get_Model(COORDINATE_3D));
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

    _float fTempDst = { 999999999.f };
    _bool bCliced = false;

    for (size_t i = 0; i < iNumMeshes; ++i)
    {
        const vector<_float3>& vecVerticesPos = vecMeshes[i]->Get_VerticesPos();
        const vector<_uint>& vecIndexBuffer = vecMeshes[i]->Get_IndexBuffer();
        _uint iNumTriangles = vecMeshes[i]->Get_NumTriangles();

        for (_uint j = 0; j < iNumTriangles; ++j)
        {
            _uint iIdx = j * 3;
            _matrix WorldMatrix = m_pControllerTransform->Get_WorldMatrix();

            _bool bResult = m_pRayCom->Compute_Intersect_Triangle(XMVector3TransformCoord(XMLoadFloat3(&vecVerticesPos[vecIndexBuffer[iIdx]]), WorldMatrix),
                XMVector3TransformCoord(XMLoadFloat3(&vecVerticesPos[vecIndexBuffer[iIdx + 1]]), WorldMatrix),
                XMVector3TransformCoord(XMLoadFloat3(&vecVerticesPos[vecIndexBuffer[iIdx + 2]]), WorldMatrix), _fDst);

            if (true == bResult) {
                
                if (fTempDst > _fDst) {
                    fTempDst = _fDst;
                }

                bCliced = true;
            }
        }
    }

    if (true == bCliced) {
        _fDst = fTempDst;
        return true;
    }

    return false;
}

void CModelObject::Register_OnAnimEndCallBack( const function<void(COORDINATE,_uint)>& fCallback)
{
	m_pControllerModel->Register_OnAnimEndCallBack(fCallback);
}

void CModelObject::Update(_float _fTimeDelta)
{
    m_pControllerModel->Play_Animation(_fTimeDelta);

	__super::Update(_fTimeDelta);
}

HRESULT CModelObject::Change_Coordinate(COORDINATE _eCoordinate, const _float3& _vPosition)
{
	if (FAILED(__super::Change_Coordinate(_eCoordinate, _vPosition)))
		return E_FAIL;

    return	m_pControllerModel->Change_Coordinate(_eCoordinate);
}

CModel* CModelObject::Get_Model(COORDINATE _eCoord)
{
    return m_pControllerModel->Get_Model(_eCoord);
}

void CModelObject::Set_AnimationLoop(COORDINATE _eCoord, _uint iIdx, _bool bIsLoop)
{
    m_pControllerModel->Set_AnimationLoop(_eCoord,iIdx, bIsLoop);
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
        _matrix matLocal = *static_cast<C2DModel*>(m_pControllerModel->Get_Model(COORDINATE_2D))->Get_CurrentSpriteTransform();
		_matrix matWorld = matLocal*XMLoadFloat4x4( &m_WorldMatrices[COORDINATE_2D]) ;
        _float4x4 matWorld4x4;
        XMStoreFloat4x4(&matWorld4x4 ,matWorld);
        if (FAILED(m_pShaderComs[COORDINATE_2D]->Bind_Matrix("g_WorldMatrix", &matWorld4x4)))
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



_uint CModelObject::Get_TextureIdx(_uint _eTextureType, _uint _iMaterialIndex)
{
    if (m_pControllerModel)
        return m_pControllerModel->Get_TextureIndex_To_3D(_eTextureType, _iMaterialIndex);
    return 0;
}
void CModelObject::Change_TextureIdx(_uint _iIndex, _uint _eTextureType, _uint _iMaterialIndex)
{
    if (m_pControllerModel)
        m_pControllerModel->Binding_TextureIndex_To_3D(_iIndex, _eTextureType, _iMaterialIndex);
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

#ifdef _DEBUG

HRESULT CModelObject::Imgui_Render_ObjectInfos()
{
    COORDINATE eCurCoord = m_pControllerTransform->Get_CurCoord();

    /* Model Prototype Tag */
    if (false == m_strModelPrototypeTag[eCurCoord].empty())
    {
        _string strModelPrototypeTag = "ModelTag : ";
        strModelPrototypeTag += WSTRINGTOSTRING(m_strModelPrototypeTag[eCurCoord]);
        ImGui::Text(strModelPrototypeTag.c_str());
    }


    /* Current Coord */
    eCurCoord = m_pControllerTransform->Get_CurCoord();
    _string strCurCoord = "Current Coord : ";
    switch (eCurCoord)
    {
    case Engine::COORDINATE_2D:
        strCurCoord += "2D";
        break;
    case Engine::COORDINATE_3D:
        strCurCoord += "3D";
        break;
    case Engine::COORDINATE_LAST:
        strCurCoord += "LAST";
        break;
    }
    ImGui::Text(strCurCoord.c_str());


    /* Coordinate Change Enable */
    _bool isCoordChangeEnable = m_pControllerTransform->Is_CoordChangeEnable();
    _string strCoordChangeEnable = "CoordChangeEnable : ";
    if (true == isCoordChangeEnable)
        strCoordChangeEnable += "true";
    else
        strCoordChangeEnable += "false";
    ImGui::Text(strCoordChangeEnable.c_str());


    /* Active */
    _bool isActive = Is_Active();
    _string strActive = "Active : ";
    if (true == isActive)
        strActive += "true";
    else
        strActive += "false";
    ImGui::Text(strActive.c_str());
    ImGui::SameLine();
    if (ImGui::Button("ActiveOnOff")) { isActive ^= 1; Set_Active(isActive); }

    /* isRender */
    _bool isRender = Is_Render();
    _string strRender = "Render : ";
    if (true == isRender)
        strRender += "true";
    else
        strRender += "false";
    ImGui::Text(strRender.c_str());
    ImGui::SameLine();
    if (ImGui::Button("RenderOnOff")) { isRender ^= 1; Set_Render(isRender); }


    /* Transform Data */
    ImGui::Separator();
    ImGui::Text("<Transform Data>");
    ImGui::Text("World Matrix");
    ImGui::PushItemWidth(200.f);
    _float4 vRight = {};
    XMStoreFloat4(&vRight, m_pControllerTransform->Get_State(CTransform::STATE_RIGHT));
    ImGui::BeginDisabled();/* 수정 불가 영역 시작. */
    ImGui::InputFloat4("Model_vRight", (float*)&vRight, " %.2f", ImGuiInputTextFlags_EnterReturnsTrue);

    _float4 vUp = {};
    XMStoreFloat4(&vUp, m_pControllerTransform->Get_State(CTransform::STATE_UP));
    ImGui::InputFloat4("Model_vUp", (float*)&vUp, " %.2f", ImGuiInputTextFlags_EnterReturnsTrue);

    _float4 vLook = {};
    XMStoreFloat4(&vLook, m_pControllerTransform->Get_State(CTransform::STATE_LOOK));
    ImGui::InputFloat4("Model_vLook", (float*)&vLook, " %.2f", ImGuiInputTextFlags_EnterReturnsTrue);
    ImGui::EndDisabled();/* 수정 불가 영역 끝. */

    _float4 vPosition = {};
    ImGui::PushItemWidth(150.f);
    XMStoreFloat4(&vPosition, m_pControllerTransform->Get_State(CTransform::STATE_POSITION));
    if (ImGui::InputFloat3("...", (float*)&vPosition, " %.2f", ImGuiInputTextFlags_EnterReturnsTrue))
        Set_Position(XMLoadFloat4(&vPosition));
    ImGui::SameLine();
    ImGui::Text("%.2f", vPosition.w);
    ImGui::SameLine();
    ImGui::Text("Model_vPosition");


    ImGui::PushItemWidth(150.f);
    _float3 vScale = Get_Scale();
    if (ImGui::InputFloat3("       Model_vScale", (float*)&vScale, " %.2f", ImGuiInputTextFlags_EnterReturnsTrue))
        Set_Scale(vScale);
    ImGui::PopItemWidth();


    return S_OK;
}


#endif // _DEBUG
