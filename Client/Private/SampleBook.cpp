#include "stdafx.h"
#include "SampleBook.h"
#include "GameInstance.h"
#include "Controller_Model.h"
#include "3DModel.h"
#include "Section_Manager.h"


CSampleBook::CSampleBook(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CModelObject(_pDevice, _pContext)
{
}

CSampleBook::CSampleBook(const CSampleBook& _Prototype)
    : CModelObject(_Prototype)
{
}


HRESULT CSampleBook::Initialize_Prototype()
{
    __super::Initialize_Prototype();
    return S_OK;
}

HRESULT CSampleBook::Initialize(void* _pArg)
{
    
    MODELOBJECT_DESC* pDesc = static_cast<MODELOBJECT_DESC*>(_pArg);

    pDesc->Build_3D_Model(pDesc->iCurLevelID,
        L"book",
        L"Prototype_Component_Shader_VtxAnimMesh",
        (_uint)PASS_VTXANIMMESH::RENDERTARGET_MAPP
        );
    pDesc->tTransform3DDesc.vInitialPosition = _float3(2.f, 0.f, -17.3f);
    pDesc->tTransform3DDesc.vInitialScaling = _float3(1.0f, 1.0f, 1.0f);
    pDesc->tTransform3DDesc.fRotationPerSec = XMConvertToRadians(180.f);
    pDesc->tTransform3DDesc.fSpeedPerSec = 0.f;
    
    pDesc->iRenderGroupID_3D = RG_3D;
    pDesc->iPriorityID_3D = PR3D_NONBLEND;

    __super::Initialize(_pArg);
    Set_AnimationLoop(COORDINATE_3D, 0, true);
    Set_AnimationLoop(COORDINATE_3D, 8, false);
    Set_AnimationLoop(COORDINATE_3D, 9, true);
    Set_Animation(0);

    return S_OK;
}

void CSampleBook::Priority_Update(_float _fTimeDelta)
{
    __super::Priority_Update(_fTimeDelta);
}

void CSampleBook::Update(_float _fTimeDelta)
{
    if (KEY_DOWN(KEY::M))
    {
        if (Book_Action(NEXT))
        {
            Set_ReverseAnimation(false);
            Set_Animation(8);
        }
    }            
    if (KEY_DOWN(KEY::N))
    {
        if (Book_Action(PREVIOUS))
        {
            Set_ReverseAnimation(true);
            Set_Animation(8);
        }
    }

    __super::Update(_fTimeDelta);
}

void CSampleBook::Late_Update(_float _fTimeDelta)
{
    Register_RenderGroup(m_iRenderGroupID_3D, m_iPriorityID_3D);
    __super::Update(_fTimeDelta);
}

HRESULT CSampleBook::Render()
{
    // 10. 11

    if (FAILED(Bind_ShaderResources_WVP()))
        return E_FAIL;
    COORDINATE eCoord = m_pControllerTransform->Get_CurCoord();
    CShader* pShader = m_pShaderComs[eCoord];
    _uint iShaderPass = m_iShaderPasses[eCoord];
    C3DModel* pModel = static_cast<C3DModel*>(m_pControllerModel->Get_Model(Get_CurCoord()));
    
    _float2 fLeftStartUV = { 0.f, 0.f };
    _float2 fLeftEndUV = { 0.5f, 1.f };
    _float2 fRightStartUV = { 0.5f, 0.f };
    _float2 fRightEndUV = { 1.f, 1.f };

    // 
    _uint iMaxActiveSectionCount = CSection_Manager::GetInstance()->Get_MaxCurActiveSectionCount();
    _uint iMainPageIndex = iMaxActiveSectionCount / 2;

    //XMINT2 iPreRenderPageMeshIndex = { 9, 8};
    XMINT2 iMainRenderPageMeshIndex = { 11, 10};
    XMINT2 iNextRenderPageMeshIndex = { 9, 8};
    // 양면인지, 단면인지도 파악해야해. 이건나중에하자

    for (_uint i = 0; i < (_uint)pModel->Get_Meshes().size(); ++i)
    {
        _uint iShaderPass = 0;
        auto pMesh = pModel->Get_Mesh(i);
        _uint iMaterialIndex = pMesh->Get_MaterialIndex();
        if (
            iMainRenderPageMeshIndex.x == i || iMainRenderPageMeshIndex.y == i ||
            iNextRenderPageMeshIndex.x == i || iNextRenderPageMeshIndex.y == i
            )
        {
            if (iMainRenderPageMeshIndex.x == i || iNextRenderPageMeshIndex.x == i)
            {
                if (FAILED(pShader->Bind_RawValue("g_fStartUV", &fLeftStartUV, sizeof(_float2))))
                    return E_FAIL;
                if (FAILED(pShader->Bind_RawValue("g_fEndUV", &fLeftEndUV, sizeof(_float2))))
                    return E_FAIL;
            }
            else
            {
                if (FAILED(pShader->Bind_RawValue("g_fStartUV", &fRightStartUV, sizeof(_float2))))
                    return E_FAIL;
                if (FAILED(pShader->Bind_RawValue("g_fEndUV", &fRightEndUV, sizeof(_float2))))
                    return E_FAIL;
            }
            _uint iIndex = 0;
            if (iMainRenderPageMeshIndex.x == i || iMainRenderPageMeshIndex.y == i)
                iIndex = iMainPageIndex;
            else
                iIndex = iMainPageIndex + 1;
            ID3D11ShaderResourceView* pResourceView = CSection_Manager::GetInstance()->Get_SRV_FromRenderTarget(iIndex);
            if(nullptr != pResourceView)
                pShader->Bind_SRV("g_DiffuseTexture", pResourceView);


            //if (FAILED(m_pGameInstance->Bind_RT_ShaderResource(pShader, "g_DiffuseTexture", TEXT("Target_Book_2D"))))
            //    return E_FAIL;
            iShaderPass = 4;
        }
        else
        {
            if (FAILED(pModel->Bind_Material(pShader, "g_DiffuseTexture", i, aiTextureType_DIFFUSE, 0)))
            {
                continue;
            }
        }

        if (FAILED(pModel->Bind_Matrices(pShader, "g_BoneMatrices", i)))
            return E_FAIL;

        pShader->Begin(iShaderPass);

        pMesh->Bind_BufferDesc();
        pMesh->Render();
    }

    return S_OK;
}

HRESULT CSampleBook::Render_Shadow()
{
    return S_OK;
}

_bool CSampleBook::Book_Action(BOOK_PAGE_ACTION _eAction)
{
    switch (_eAction)
    {
        case Client::CSampleBook::PREVIOUS:
            if(!SECTION_MGR->Has_Prev_Section())
                return false;
            m_eCurAction = PREVIOUS;

            break;
        case Client::CSampleBook::NEXT:
            if (!SECTION_MGR->Has_Next_Section())
                return false;
            m_eCurAction = NEXT;


            //Event_ChangeBossState

            break;
        case Client::CSampleBook::LAST:
            break;
        default:
            break;
    }

    

    return true;
}

CSampleBook* CSampleBook::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CSampleBook* pInstance = new CSampleBook(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CSampleBook");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CSampleBook::Clone(void* _pArg)
{
    CSampleBook* pInstance = new CSampleBook(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CSampleBook");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CSampleBook::Free()
{

    __super::Free();
}
