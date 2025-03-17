#include "ModelObject.h"
#include "GameInstance.h"
#include "3DModel.h"
#include "2DModel.h"
#include "Controller_Model.h"
#include "Trail_Manager.h"



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
    m_fFrustumCullingRange = pDesc->fFrustumCullingRange;

    m_iRenderGroupID_3D = pDesc->iRenderGroupID_3D;
    m_iPriorityID_3D = pDesc->iPriorityID_3D;

    // Add

    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components(pDesc)))
        return E_FAIL;


    return S_OK;
}

void CModelObject::Priority_Update(_float _fTimeDelta)
{
    __super::Priority_Update(_fTimeDelta);
}

void CModelObject::Late_Update(_float _fTimeDelta)
{
    if (COORDINATE_3D == Get_CurCoord())
    {
        if (false == m_isFrustumCulling)
        {
            m_pGameInstance->Add_RenderObject_New(m_iRenderGroupID_3D, m_iPriorityID_3D, this);
            m_pGameInstance->Add_RenderObject_New(REDNERGROUP_SHADOWID, 0, this);
        }
    }

    /* Update 2D Object FadeAlpha Effect :: 태웅*/
    Action_Fade(_fTimeDelta);
    Action_StoppableRender(_fTimeDelta);
    /* Update Parent Matrix */
    __super::Late_Update(_fTimeDelta);
}

HRESULT CModelObject::Render_WorldPosMap(const _wstring& _strCopyRTTag, const _wstring& _strSectionTag)
{
    //m_pGameInstance->Render_Begin();
    //// 1. 자기 자신에 해당하는 RT 및 MRT를 생성한다. 이때 사이즈는 자기 자신이 추후 바인딩 할 Section RTV와 동일해야한다. 

    //// 2. 자기 자신이 생성한 MRT를 바인딩하고, 자기 자신의 World 매트릭스만 바인딩 하고 렌더를 수행한다.

    //// 3. 쉐이더에서는 자기 자신의 버텍스에 저장된 texcoord 좌표를 기준으로 렌더타겟에 worldpos를 저장한다. 

    //// 4. 저장된 RT는 일단 TargetManager가 들고있긴 할텐데.... 이게 의미가 있나 키값도 또 만들려면 번거롭고 Section 이름과 일치도 되어야하는데. 
    //

    //m_pGameInstance->Render_End();
    return S_OK;
}

HRESULT CModelObject::Render()
{

#ifdef _DEBUG
    if (m_iInstanceID == 90)
    {
        int a = 0;
        a = 2;
        a = 1;
    }
#endif // _DEBUG

    if (FAILED(Bind_ShaderResources_WVP()))
        return E_FAIL;
    COORDINATE eCoord = m_pControllerTransform->Get_CurCoord();
	CShader* pShader = m_pShaderComs[eCoord];
	_uint iShaderPass = m_iShaderPasses[eCoord];
    if (COORDINATE_3D == eCoord)
        pShader->Bind_RawValue("g_fFarZ", m_pGameInstance->Get_FarZ(), sizeof(_float));
    else if(COORDINATE_2D == eCoord)
    {
        _float fFadeAlphaRatio = m_vFadeAlpha.y / m_vFadeAlpha.x;
        switch (m_eFadeAlphaState)
        {
        case Engine::CModelObject::FADEALPHA_DEFAULT:
            fFadeAlphaRatio = 1.0f;
            break;
        case Engine::CModelObject::FADEALPHA_IN:
            fFadeAlphaRatio = m_vFadeAlpha.y / m_vFadeAlpha.x;
            break;
        case Engine::CModelObject::FADEALPHA_OUT:
            fFadeAlphaRatio = 1.0f - (m_vFadeAlpha.y / m_vFadeAlpha.x);
            break;
        default:
            break;
        }
        
        pShader->Bind_RawValue("g_fSprite2DFadeAlphaRatio", &fFadeAlphaRatio, sizeof(_float));


        /* Stoppable */
        pShader->Bind_RawValue("g_isStoppable", &m_isStoppable, sizeof(_int));
        pShader->Bind_RawValue("g_vStoppableColor", &m_vStoppableColor, sizeof(_float4));
        _float fStoppableRatio = m_vStoppableTime.y / m_vStoppableTime.x;
        pShader->Bind_RawValue("g_fStoppableRatio", &fStoppableRatio, sizeof(_float));
    }

    m_pControllerModel->Render(pShader, iShaderPass);

    return S_OK;
}

HRESULT CModelObject::Render_Shadow(_float4x4* _pViewMatrix, _float4x4* _pProjMatrix)
{
    COORDINATE eCoord = m_pControllerTransform->Get_CurCoord();
    if (COORDINATE_2D == eCoord)
        return S_OK;

    if (FAILED(m_pShaderComs[COORDINATE_3D]->Bind_Matrix("g_WorldMatrix", &m_WorldMatrices[COORDINATE_3D])))
        return E_FAIL;

    if (FAILED(m_pShaderComs[COORDINATE_3D]->Bind_Matrix("g_ViewMatrix", _pViewMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderComs[COORDINATE_3D]->Bind_Matrix("g_ProjMatrix", _pProjMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderComs[COORDINATE_3D]->Bind_RawValue("g_vCamPosition", m_pGameInstance->Get_CamPosition(), sizeof(_float4))))
        return E_FAIL;
    
    CShader* pShader = m_pShaderComs[eCoord];

    _uint iShaderPass = {};
    if (m_pControllerModel->Get_Model(COORDINATE_3D)->Is_AnimModel())
    {
        iShaderPass = (_uint)PASS_VTXANIMMESH::SHADOWMAP;
    }
    else
    {
        iShaderPass = (_uint)PASS_VTXMESH::SHADOWMAP;
    }
    m_pControllerModel->Render_Shadow(m_pShaderComs[eCoord], iShaderPass);

    return S_OK;
}

HRESULT CModelObject::Render_PlayerDepth()
{
    COORDINATE eCoord = m_pControllerTransform->Get_CurCoord();
    if (COORDINATE_3D == eCoord)
    {
        if (FAILED(Bind_ShaderResources_WVP()))
            return E_FAIL;

        CShader* pShader = m_pShaderComs[COORDINATE_3D];
        if(m_pControllerModel->Get_Model(COORDINATE_3D)->Is_AnimModel())
            m_pControllerModel->Render(pShader, (_uint)PASS_VTXANIMMESH::PLAYERDEPTH);
        else
            m_pControllerModel->Render(pShader, (_uint)PASS_VTXMESH::PLAYERDEPTH);
    }


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
    //// 예외처리
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

void CModelObject::Check_FrustumCulling()
{
    if (COORDINATE_3D == Get_CurCoord())
        m_isFrustumCulling = !m_pGameInstance->isIn_Frustum_InWorldSpace(Get_FinalPosition(), 5.f);
    else
        m_isFrustumCulling = false;
}

void CModelObject::Start_FadeAlphaIn(_float _fFadeAlphaTime)
{
    if (COORDINATE_2D != Get_CurCoord())
        return;

    m_eFadeAlphaState = FADEALPHA_IN;
    m_vFadeAlpha.x = _fFadeAlphaTime;
    m_vFadeAlpha.y = 0.0f;
}

void CModelObject::Start_FadeAlphaOut(_float _fFadeAlphaTime)
{
    if (COORDINATE_2D != Get_CurCoord())
        return;

    m_eFadeAlphaState = FADEALPHA_OUT;
    m_vFadeAlpha.x = _fFadeAlphaTime;
    m_vFadeAlpha.y = 0.0f;
}

void CModelObject::Action_Fade(_float _fTimeDelta)
{
    if (COORDINATE_2D != Get_CurCoord())
        return;

    switch (m_eFadeAlphaState)
    {
    case Engine::CModelObject::FADEALPHA_DEFAULT:
        break;
    case Engine::CModelObject::FADEALPHA_IN:
    {
        m_vFadeAlpha.y += _fTimeDelta;
        if (m_vFadeAlpha.x <= m_vFadeAlpha.y)
        {
            m_vFadeAlpha.y = m_vFadeAlpha.x;
            m_eFadeAlphaState = FADEALPHA_DEFAULT;
        }
    }
        break;
    case Engine::CModelObject::FADEALPHA_OUT:
    {
        m_vFadeAlpha.y += _fTimeDelta;
        if (m_vFadeAlpha.x <= m_vFadeAlpha.y)
        {
            m_vFadeAlpha.y = m_vFadeAlpha.x;
            //m_eFadeAlphaState = FADEALPHA_DEFAULT;
        }
    }
        break;
    default:
        break;
    }
}

void CModelObject::Update(_float _fTimeDelta)
{
    if (m_iInstanceID == 889) {
        int i = 0;
    }
    if (m_pControllerModel)
    {
        if(m_bPlayingAnim)
        {
            if (false == m_isFrustumCulling)
                m_pControllerModel->Play_Animation(_fTimeDelta, m_bReverseAnimation);
        }
        else
        {
            if (false == m_isFrustumCulling)
                m_pControllerModel->Play_Animation(0, m_bReverseAnimation);
        }
    }

	__super::Update(_fTimeDelta);
}

HRESULT CModelObject::Change_Coordinate(COORDINATE _eCoordinate, _float3* _pNewPosition)
{
	if (FAILED(__super::Change_Coordinate(_eCoordinate, _pNewPosition)))
		return E_FAIL;

    return	m_pControllerModel->Change_Coordinate(_eCoordinate);
}

CModel* CModelObject::Get_Model(COORDINATE _eCoord)
{
    return m_pControllerModel->Get_Model(_eCoord);
}

_float CModelObject::Get_AnimationTime(_uint iAnimIndex)
{
    return m_pControllerModel->Get_Model(Get_CurCoord())->Get_AnimationTime(iAnimIndex);
}

_float CModelObject::Get_AnimationTime()
{
    return m_pControllerModel->Get_Model(Get_CurCoord())->Get_AnimationTime();    
}

_float CModelObject::Get_CurrentAnimationProgress()
{
    return m_pControllerModel->Get_Model(Get_CurCoord())->Get_CurrentAnimProgeress();
}

void CModelObject::Set_AnimationLoop(COORDINATE _eCoord, _uint iIdx, _bool bIsLoop)
{
    m_pControllerModel->Set_AnimationLoop(_eCoord,iIdx, bIsLoop);
}

void CModelObject::Set_Animation(_uint iIdx)
{
    m_pControllerModel->Set_Animation(iIdx, m_bReverseAnimation);
}

void CModelObject::Set_3DAnimationTransitionTime(_uint iIdx, _float _fTime)
{
 	static_cast<C3DModel*>( m_pControllerModel->Get_Model(COORDINATE_3D))->Set_AnimationTransitionTime(iIdx, _fTime);
}

void CModelObject::Switch_Animation(_uint iIdx)
{
    m_pControllerModel->Switch_Animation(iIdx, m_bReverseAnimation);
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
        /* Com_Shader_2D */
        if (FAILED(Add_Component(iStaticLevelID, _pDesc->strShaderPrototypeTag_2D,
            TEXT("Com_Shader_2D"), reinterpret_cast<CComponent**>(&m_pShaderComs[COORDINATE_2D]))))
            return E_FAIL;
        if (true == _pDesc->isCoordChangeEnable)
        {
            /* Com_Shader_3D */
            if (FAILED(Add_Component(iStaticLevelID, _pDesc->strShaderPrototypeTag_3D,
                TEXT("Com_Shader_3D"), reinterpret_cast<CComponent**>(&m_pShaderComs[COORDINATE_3D]))))
                return E_FAIL;
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
	tModelDesc.tModel3DDesc.isDeepCopyConstBuffer = _pDesc->isDeepCopyConstBuffer;

	m_pControllerModel = CController_Model::Create(m_pDevice, m_pContext, &tModelDesc);

    //pDesc->isDeepCopyConstBuffer;
    //pDesc->vMaterialDefaultColor;
    if (nullptr == m_pControllerModel)
        return E_FAIL;


    return S_OK;
}

void CModelObject::Update_Trail(_int _iTrailID, _float _fTimeDelta)
{
    /* Test */
    if (false == m_isTrail)
        return;
    C3DModel* p3DModel = static_cast<C3DModel*>(m_pControllerModel->Get_Model(COORDINATE_3D));
    if (nullptr == p3DModel)
        return;

    m_vTrailCreateTime.y += _fTimeDelta;
    if (m_vTrailCreateTime.x <= m_vTrailCreateTime.y)
    {
        m_vTrailCreateTime.y = 0.0f;
        CTrailInstance::TRAIL_DESC TrailDesc = {};
        TrailDesc.WorldMatrix = m_WorldMatrices[COORDINATE_3D];
        TrailDesc.fTrailTime = m_fTrailDuration;
        TrailDesc.iNumMeshes = p3DModel->Get_NumMeshes();
        TrailDesc.vTrailColor = m_vTrailColor;
        TrailDesc.MeshesBoneMatrices;

        for (_uint i = 0; i < TrailDesc.iNumMeshes; ++i)
        {
            static array<_float4x4, 256> BoneArray = {};
            ZeroMemory(&BoneArray, sizeof BoneArray);
            p3DModel->Copy_BoneMatrices(i, &BoneArray);
            TrailDesc.MeshesBoneMatrices.push_back(BoneArray);
        }

        CTrail_Manager::GetInstance()->Add_Trail(_iTrailID, &TrailDesc);
    }
}

HRESULT CModelObject::Render_Trail()
{
    COORDINATE eCoord = Get_CurCoord();
    if (COORDINATE_3D != eCoord)
        return E_FAIL;

    /* World Matrix는 각각의 Trail이 바인딩할거야. */
    if (FAILED(m_pShaderComs[eCoord]->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderComs[eCoord]->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

    if (FAILED(CTrail_Manager::GetInstance()->Render_Trails(m_iTrailID, static_cast<C3DModel*>(m_pControllerModel->Get_Model(eCoord)), m_pShaderComs[eCoord])))
        return E_FAIL;

    return S_OK;
}

void CModelObject::Action_StoppableRender(_float _fTimeDelta)
{
    if (false == m_isStoppable)
        return;

    m_vStoppableTime.y += _fTimeDelta * m_fUpDownFactor;
    if (m_vStoppableTime.x <= m_vStoppableTime.y)
    {
        m_vStoppableTime.y = m_vStoppableTime.x;
        m_fUpDownFactor = -1.0f;
    }
    else if (0.0f >= m_vStoppableTime.y)
    {
        m_vStoppableTime.y = 0.0f;
        m_fUpDownFactor = 1.0f;
    }
}

HRESULT CModelObject::Bind_ShaderResources_WVP()
{
    switch (m_pControllerTransform->Get_CurCoord())
    {
    case Engine::COORDINATE_2D:
        _matrix matLocal = *static_cast<C2DModel*>(m_pControllerModel->Get_Model(COORDINATE_2D))->Get_CurrentSpriteTransform();
        _matrix matRatioScalling = XMMatrixScaling((_float)RATIO_BOOK2D_X, (_float)RATIO_BOOK2D_Y, 1.f);
        matLocal *= matRatioScalling;

        _matrix matWorld = matLocal*XMLoadFloat4x4( &m_WorldMatrices[COORDINATE_2D]) ;

        _float4x4 matWorld4x4;
        XMStoreFloat4x4(&matWorld4x4 ,matWorld);
        if (FAILED(m_pShaderComs[COORDINATE_2D]->Bind_Matrix("g_WorldMatrix", &matWorld4x4)))
            return E_FAIL;
        // 뷰, 투영 액세스 금지 !
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
_uint CModelObject::Get_CurrentAnimIndex()
{
    return m_pControllerModel->Get_Model(Get_CurCoord())->Get_CurrentAnimIndex();
}
void CModelObject::Set_PlayingAnim(_bool _bPlaying)
{
    m_bPlayingAnim = _bPlaying;
}

void CModelObject::Set_ReverseAnimation(_bool _bReverse)
{ 
    m_bReverseAnimation = _bReverse; 
	m_pControllerModel->Get_Model(Get_CurCoord())->Switch_Reverse(_bReverse);
}

void CModelObject::Set_Progress(COORDINATE _eCoord, _uint _iIdx, _float _fProgress, _bool _bReverse)
{
    m_pControllerModel->Get_Model(Get_CurCoord())->Set_Progress(_iIdx, _fProgress, _bReverse);
}

_bool CModelObject::Is_DuringAnimation()
{
    return m_pControllerModel->Get_Model(Get_CurCoord())->Is_DuringAnimation();
}

_bool CModelObject::Is_AnimTransition()
{
    if (COORDINATE_2D == Get_CurCoord())
        return false;
    else
        return static_cast<C3DModel*> (m_pControllerModel->Get_Model(COORDINATE_3D))->Is_AnimTransition();
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
    ImGui::Text("Section Tag");
    ImGui::SameLine();
    ImGui::Text(L"" == m_strSectionName ? "empty" : WSTRINGTOSTRING(m_strSectionName).c_str());


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
    _float3 vScale = m_pControllerTransform->Get_Scale();
    if (ImGui::InputFloat3("       Model_vScale", (float*)&vScale, " %.2f", ImGuiInputTextFlags_EnterReturnsTrue))
        Set_Scale(vScale);
    ImGui::PopItemWidth();


    return S_OK;
}


#endif // _DEBUG
