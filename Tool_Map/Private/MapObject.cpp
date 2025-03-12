#include "stdafx.h"
#include "MapObject.h"
#include "GameInstance.h"
#include "Controller_Model.h"
#include "Material.h"

#include "Bone.h"
#include "Engine_Struct.h"
#include <gizmo/ImGuizmo.h>

CMapObject::CMapObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CModelObject(_pDevice, _pContext)
{
}

CMapObject::CMapObject(const CMapObject& _Prototype)
    :CModelObject(_Prototype)
{
}

HRESULT CMapObject::Initialize_Prototype()
{

    return S_OK;
}

HRESULT CMapObject::Initialize(void* _pArg)
{
    MAPOBJ_DESC* pDesc = static_cast<MAPOBJ_DESC*>(_pArg);
    m_strModelName = pDesc->szModelName;
    if (pDesc->eCreateType == CMapObject::OBJ_CREATE)
        XMStoreFloat4x4(&m_matWorld,XMMatrixIdentity());
    else
        m_matWorld = pDesc->tTransform3DDesc.matWorld;

    CBase* pBase = m_pGameInstance->Find_Prototype(pDesc->iModelPrototypeLevelID_3D, m_strModelName);

    if (nullptr == pBase)
        return E_FAIL;

    if (static_cast<CModel*>(pBase)->Is_AnimModel())
    {
        pDesc->iShaderPass_3D = (_uint)PASS_VTXANIMMESH::DEFAULT;
        pDesc->strShaderPrototypeTag_3D = TEXT("Prototype_Component_Shader_VtxAnimMesh");
    }
    else
    {

        pDesc->iShaderPass_3D = (_uint)PASS_VTXMESH::DEFAULT;
        pDesc->strShaderPrototypeTag_3D = TEXT("Prototype_Component_Shader_VtxMesh");
    }

    pDesc->eStartCoord = COORDINATE_3D;
    pDesc->isCoordChangeEnable = false;
    
    pDesc->strModelPrototypeTag_3D = m_strModelName;

    pDesc->tTransform3DDesc.vInitialPosition = _float3(0.0f, 0.0f, 0.0f);
    pDesc->tTransform3DDesc.vInitialScaling = _float3(1.0f, 1.0f, 1.0f);
    pDesc->tTransform3DDesc.fRotationPerSec = XMConvertToRadians(180.f);
    pDesc->tTransform3DDesc.fSpeedPerSec = 0.f;

    CActor::ACTOR_DESC ActorDesc;
    vector <SHAPE_COOKING_DESC> vecShapeCookingDesc = {};
    vector<SHAPE_DATA>  vecShapeData;
    C3DModel* p3DModel = static_cast<C3DModel*>(pBase);

   /* if (p3DModel->Has_CookingCollider())
    {
        _uint iColliderType = p3DModel->Get_CookingColliderType();
        MAPOBJ_DESC* pDesc = static_cast<MAPOBJ_DESC*>(_pArg);

        pDesc->eActorType = ACTOR_TYPE::STATIC;

        ActorDesc.pOwner = this;

        ActorDesc.FreezeRotation_XYZ[0] = true;
        ActorDesc.FreezeRotation_XYZ[1] = true;
        ActorDesc.FreezeRotation_XYZ[2] = true;

        ActorDesc.FreezePosition_XYZ[0] = false;
        ActorDesc.FreezePosition_XYZ[1] = false;
        ActorDesc.FreezePosition_XYZ[2] = false;
        ActorDesc.isAddActorToScene = true;



        _float3 fScale =
            _float3(XMVectorGetX(XMVector3Length(XMLoadFloat3((_float3*)&m_matWorld.m[0]))),
                XMVectorGetX(XMVector3Length(XMLoadFloat3((_float3*)&m_matWorld.m[1]))),
                XMVectorGetX(XMVector3Length(XMLoadFloat3((_float3*)&m_matWorld.m[2]))));
        _matrix matScale = XMMatrixScaling(fScale.x, fScale.y, fScale.z);

        switch (iColliderType)
        {
        case 0:
        case 1:
        {
            vecShapeData.resize(1);
            vecShapeCookingDesc.resize(1);
            vecShapeData[0].eShapeType = SHAPE_TYPE::COOKING;
            vecShapeData[0].eMaterial = ACTOR_MATERIAL::DEFAULT;
            vecShapeData[0].pShapeDesc = &vecShapeCookingDesc[0];
            vecShapeData[0].isTrigger = false;
            vecShapeData[0].isVisual = true;
            XMStoreFloat4x4(&vecShapeData[0].LocalOffsetMatrix, matScale);
            break;
        }
        case 2:
        case 3:
        {
            _uint iMeshSize = (_uint)p3DModel->Get_Meshes().size();
            vecShapeData.resize(iMeshSize);
            vecShapeCookingDesc.resize(iMeshSize);
            SHAPE_DATA tempShapeData = {};
            tempShapeData.eShapeType = SHAPE_TYPE::COOKING;
            tempShapeData.eMaterial = ACTOR_MATERIAL::DEFAULT;
            tempShapeData.isTrigger = false;
            tempShapeData.isVisual = true;
            XMStoreFloat4x4(&tempShapeData.LocalOffsetMatrix, matScale);

            fill(vecShapeData.begin(), vecShapeData.end(), tempShapeData);
            _uint iMeshIndex = 0;
            for (_uint i = 0; i < iMeshSize; ++i)
            {
                vecShapeCookingDesc[i].iShapeIndex = i;
                vecShapeData[i].pShapeDesc = &vecShapeCookingDesc[i];
            }
            break;
        }
        default:
            break;
        }

        ActorDesc.ShapeDatas.insert(ActorDesc.ShapeDatas.end(), vecShapeData.begin(), vecShapeData.end());

        pDesc->pActorDesc = &ActorDesc;

    }*/



    m_iShaderPasses[COORDINATE_2D] = pDesc->iShaderPass_2D;
    m_iShaderPasses[COORDINATE_3D] = pDesc->iShaderPass_3D;
    m_strModelPrototypeTag[COORDINATE_2D] = pDesc->strModelPrototypeTag_2D;
    m_strModelPrototypeTag[COORDINATE_3D] = pDesc->strModelPrototypeTag_3D;
    m_fFrustumCullingRange = pDesc->fFrustumCullingRange;

    m_iRenderGroupID_3D = pDesc->iRenderGroupID_3D;
    m_iPriorityID_3D = pDesc->iPriorityID_3D;

    if (FAILED(CModelObject::Ready_Components(pDesc)))
        return E_FAIL;

    if (FAILED(CPartObject::Initialize(pDesc)))
        return E_FAIL;


    CRay::RAY_DESC RayDesc = {};
    RayDesc.fViewportWidth =   (_float)g_iWinSizeX;
    RayDesc.fViewportHeight =  (_float)g_iWinSizeY;
    if (FAILED(Add_Component(LEVEL_STATIC, L"Prototype_Component_Ray",
            TEXT("Com_Ray"), reinterpret_cast<CComponent**>(&m_pRayCom),&RayDesc)))
        return E_FAIL;

    //if (pDesc->eCreateType == OBJ_LOAD)
    //{
    //    Set_WorldMatrix(m_matWorld);

    //}
    Set_PlayingAnim(false);

    CModel* pModel = m_pControllerModel->Get_Model(COORDINATE_3D);
    _uint iMaterialCnt = 0;
    if (pModel)
    {
        C3DModel* p3DModel = static_cast<C3DModel*>(pModel);
        _uint iSize = (_uint)p3DModel->Get_Materials().size();
		m_eColorShaderModes.resize(iSize);
		fill(m_eColorShaderModes.begin(), m_eColorShaderModes.end(), C3DModel::COLOR_NONE);
    }

    return S_OK;
}

void CMapObject::Priority_Update(_float _fTimeDelta)
{
    CPartObject::Priority_Update(_fTimeDelta);
}
void CMapObject::Update(_float _fTimeDelta)
{
    if (m_eMode == MODE::PICKING)
    {
        XMStoreFloat4x4(&m_matWorld, Get_WorldMatrix());
        _float4x4 fMat = {};
        _float4x4 fMatView = m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW);
        _float4x4 fMatProj = m_pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ);
        RECT rect{};
        GetWindowRect(g_hWnd, &rect);
        ImGuizmo::SetRect((_float)rect.left, (_float)rect.top, (_float)g_iWinSizeX, (_float)g_iWinSizeY);
        ImGuizmo::Manipulate(fMatView.m[0],
            fMatProj.m[0], (ImGuizmo::OPERATION)m_CurrentGizmoOperation,
            ImGuizmo::WORLD,
            m_matWorld.m[0]
        );

        for (_uint i = 0; i < CTransform::STATE_END; i++)
        {
            Set_WorldMatrix(m_matWorld);
        }
    }

    /* Update Parent Matrix */
    CModelObject::Update(_fTimeDelta);
}

void CMapObject::Late_Update(_float _fTimeDelta)
{

    /* Add Render Group */
    if (m_eMode != PREVIEW)
    {
        if (COORDINATE_3D == m_pControllerTransform->Get_CurCoord())
            Register_RenderGroup(RG_3D, PR3D_GEOMETRY);
    }

    /* Update Parent Matrix */
    CPartObject::Late_Update(_fTimeDelta);
}

HRESULT CMapObject::Render()
{
    CModelObject::Render();
    return S_OK;
}


HRESULT CMapObject::Render_Preview(_float4x4* _ViewMat, _float4x4* _ProjMat)
{
    if (m_pControllerModel->Get_Model(COORDINATE_3D)->Is_AnimModel())
    {
        m_pControllerModel->Play_Animation(0.f,m_bReverseAnimation);
    }
    else
    {
        CMapObject::Update(0.f);
        CMapObject::Late_Update(0.f);
    }

    if (FAILED(m_pShaderComs[COORDINATE_3D]->Bind_Matrix("g_ViewMatrix", _ViewMat)))
        return E_FAIL;

    if (FAILED(m_pShaderComs[COORDINATE_3D]->Bind_Matrix("g_ProjMatrix", _ProjMat)))
        return E_FAIL;

    CModelObject::Render();
    return S_OK;
}

#ifdef _DEBUG


HRESULT CMapObject::Get_Textures(vector<TEXTURE_INFO>& _Diffuses, _uint _eTextureType)
{

    CModel* pModel = m_pControllerModel->Get_Model(COORDINATE_3D);
    _uint iMaterialCnt = 0;
    if (pModel)
    {
        auto Bones = static_cast<C3DModel*>(pModel)->Get_Bones();

        _string strBoneName = Bones[0]->Get_Name();
        _uint iBoneNameSize = (_uint)strBoneName.size();


        auto pMaterials = static_cast<C3DModel*>(pModel)->Get_Materials();
        for (auto pMaterial : pMaterials)
        {
            _uint iDiffuseCnt = 0;
            TEXTURE_INFO tInfo = { iMaterialCnt, };
            ID3D11ShaderResourceView* pSRV = nullptr;

            do
            {
                pSRV = pMaterial->Find_Texture((aiTextureType)_eTextureType, iDiffuseCnt);

                tInfo.iTextureIndex = iDiffuseCnt;
                tInfo.pSRV = pSRV;
                if (pSRV != nullptr)
                {
                    lstrcpy(tInfo.szTextureName, pMaterial->Find_Name((aiTextureType)_eTextureType, iDiffuseCnt)->c_str());
                }
                _Diffuses.push_back(tInfo);
                iDiffuseCnt++;
            } while (nullptr != pSRV);
            iMaterialCnt++;
        }
    }
    else
        return E_FAIL;
    return S_OK;
}

HRESULT CMapObject::Add_Textures(TEXTURE_INFO& _tDiffuseInfo, _uint _eTextureType)
{
    CModel* pModel = m_pControllerModel->Get_Model(COORDINATE_3D);
    if (pModel)
    {
        auto pMaterials = static_cast<C3DModel*>(pModel)->Get_Materials();
        if (_tDiffuseInfo.iMaterialIndex >= pMaterials.size())
            return E_FAIL;
        if (FAILED(pMaterials[_tDiffuseInfo.iMaterialIndex]->Add_Texture((aiTextureType)_eTextureType,
            _tDiffuseInfo.pSRV
            , _tDiffuseInfo.szTextureName
        )))
            return E_FAIL;
        if (FAILED(pMaterials[_tDiffuseInfo.iMaterialIndex]->Ready_PixelConstBuffer()))
            return E_FAIL;

        return pMaterials[_tDiffuseInfo.iMaterialIndex]->Update_PixelConstBuffer();
    }
    return S_OK;
}
HRESULT CMapObject::Delete_Texture(_uint _iIndex, _uint _eTextureType, _uint _iMaterialIndex)
{
    CModel* pModel = m_pControllerModel->Get_Model(COORDINATE_3D);
    if (pModel)
    {
        auto pMaterials = static_cast<C3DModel*>(pModel)->Get_Materials();
        if (_iMaterialIndex >= pMaterials.size())
            return E_FAIL;
        return pMaterials[_iMaterialIndex]->Delete_Texture((aiTextureType)_eTextureType, _iIndex);
    }
    return S_OK;

}
HRESULT	CMapObject::Push_Texture(const _string _strTextureName, _uint _eTextureType)
{
    if (L"" == m_strModelName || nullptr == m_pControllerModel)
        return E_FAIL;

    CModel* pModel = m_pControllerModel->Get_Model(COORDINATE_3D);
    _wstring strTextureFileName = StringToWstring(_strTextureName) + L".dds";

    ID3D11ShaderResourceView* pSRV = { nullptr };


    if (nullptr != pModel)
    {
        C3DModel* p3DModel = static_cast<C3DModel*>(pModel);
        auto pMaterials = static_cast<C3DModel*>(pModel)->Get_Materials();
        // 자동 텍스쳐 바인딩은, 마테리얼 정보가 지정되어있지 않아보이므로 그냥 0번 마테리얼을 기준으로 삼습니다. 있다면.
        if (!pMaterials.empty())
        {
            _uint iTextureIdx = 0;
            pSRV =  pMaterials[0]->Find_Texture((aiTextureType)_eTextureType, strTextureFileName, &iTextureIdx);

            if (nullptr == pSRV)
            {
                _wstring strTextureFath = STATIC_3D_MODEL_FILE_PATH; 
                strTextureFath += L"NonAnim/" + m_strModelName;
                _tchar szName[MAX_PATH] = {};
                lstrcpy(szName, (strTextureFath + L"/" + strTextureFileName).c_str());

                if (SUCCEEDED(CreateDDSTextureFromFile(m_pDevice, szName, nullptr, &pSRV)) && nullptr != pSRV)
                {
                    CMapObject::TEXTURE_INFO tAddInfo;
                    iTextureIdx += 1;
                    tAddInfo.iTextureIndex = iTextureIdx;
                    tAddInfo.iMaterialIndex = 0;
                    tAddInfo.pSRV = pSRV;
                    lstrcpy(tAddInfo.szTextureName, strTextureFileName.c_str());
                    
                    if (FAILED(Add_Textures(tAddInfo, _eTextureType)))
                        return E_FAIL;
                    else
                    {
                        Change_TextureIdx(iTextureIdx, _eTextureType, 0);
                        return S_OK;
                    }
                }
            }
            else 
                Change_TextureIdx(iTextureIdx,_eTextureType,0);
            

        }
    }
    return E_FAIL;
}

void CMapObject::Set_Diffuse_Color(_uint _iMaterialIndex, const _float4 _fDiffuseColor, _bool _isUpdate)
{
    CModel* pModel = m_pControllerModel->Get_Model(COORDINATE_3D);
    if (pModel)
    {
        C3DModel* p3DModel = static_cast<C3DModel*>(pModel);

        switch (m_eColorShaderModes[_iMaterialIndex])
        {
        case Engine::C3DModel::COLOR_DEFAULT:
            p3DModel->Set_MaterialConstBuffer_Albedo(_iMaterialIndex, _fDiffuseColor, _isUpdate);
            break;
        case Engine::C3DModel::MIX_DIFFUSE:
            p3DModel->Set_MaterialConstBuffer_MultipleAlbedo(_iMaterialIndex, _fDiffuseColor, _isUpdate);
            break;
        }
    }
}


const _float4 CMapObject::Get_Diffuse_Color(_uint _iMaterialIndex)
{
    _float4 fReturnColor = { 1.f,1.f,1.f,1.f };
    CModel* pModel = m_pControllerModel->Get_Model(COORDINATE_3D);
    if (pModel)
    {
        C3DModel* p3DModel = static_cast<C3DModel*>(pModel);

        switch (m_eColorShaderModes[_iMaterialIndex])
        {
        case Engine::C3DModel::COLOR_DEFAULT:
            fReturnColor = p3DModel->Get_MaterialConstBuffer_Albedo(_iMaterialIndex);
            break;
        case Engine::C3DModel::MIX_DIFFUSE:
            fReturnColor = p3DModel->Get_MaterialConstBuffer_MultipleAlbedo(_iMaterialIndex);
            break;
        default:
            break;
        }
    }

    return fReturnColor;
}

void CMapObject::Set_Color_Shader_Mode(_uint _iMaterialIndex, C3DModel::COLOR_SHADER_MODE _eMode, _bool _isUpdate)
{
    CModel* pModel = m_pControllerModel->Get_Model(COORDINATE_3D);
    _float4 fColor = {};
    if (pModel)
    {
        C3DModel* p3DModel = static_cast<C3DModel*>(pModel);

        switch (_eMode)
        {
        case Engine::C3DModel::COLOR_NONE:
            // 알베도맵 사용함.
            // mix color를 초기화시켜줌.
			p3DModel->Set_MaterialConstBuffer_UseAlbedoMap(_iMaterialIndex, true, _isUpdate);
            p3DModel->Set_MaterialConstBuffer_MultipleAlbedo(_iMaterialIndex, _float4(1.f, 1.f, 1.f, 1.f), _isUpdate);
            break;
        case Engine::C3DModel::COLOR_DEFAULT:
            // 알베도맵 사용하지 않음.
            // 전에 mix color를 사용중이었다면, 가져와서 default color에 적용한 후,
			// mix color를 초기화.
			p3DModel->Set_MaterialConstBuffer_UseAlbedoMap(_iMaterialIndex, false, _isUpdate);
            
            if (m_eColorShaderModes[_iMaterialIndex] == Engine::C3DModel::COLOR_DEFAULT)
            {
                fColor = Get_Diffuse_Color(_iMaterialIndex);
                p3DModel->Set_MaterialConstBuffer_Albedo(_iMaterialIndex, fColor, _isUpdate);
            }
            
            p3DModel->Set_MaterialConstBuffer_MultipleAlbedo(_iMaterialIndex, _float4(1.f, 1.f, 1.f, 1.f), _isUpdate);
            break;
        case Engine::C3DModel::MIX_DIFFUSE:
            // 알베도맵 사용함.
            // 전에 default color를 사용중이었다면, 가져와서 mix color에 적용함.
            p3DModel->Set_MaterialConstBuffer_UseAlbedoMap(_iMaterialIndex, true, _isUpdate);
            
            if (m_eColorShaderModes[_iMaterialIndex] == Engine::C3DModel::COLOR_DEFAULT)
            {
			    fColor = Get_Diffuse_Color(_iMaterialIndex);
				p3DModel->Set_MaterialConstBuffer_MultipleAlbedo(_iMaterialIndex, fColor, _isUpdate);
            }
            
            break;
        default:
            break;
        }
    }

	m_eColorShaderModes[_iMaterialIndex] = _eMode;
}

C3DModel::COLOR_SHADER_MODE CMapObject::Get_Color_Shader_Mode(_uint _iMaterialIndex)
{
    return m_eColorShaderModes[_iMaterialIndex];
}


HRESULT CMapObject::Add_Texture_Type(_uint _eTextureType)
{
    CModel* pModel = m_pControllerModel->Get_Model(COORDINATE_3D);
    if (pModel)
    {
        auto pMaterials = static_cast<C3DModel*>(pModel)->Get_Materials();
        
        for (auto& pMaterial : pMaterials)
            if (FAILED(pMaterial->Add_Texture((aiTextureType)_eTextureType)))
                return E_FAIL;

        return S_OK;
    }
    return S_OK;
}

HRESULT CMapObject::Save_Override_Material(HANDLE _hFile)
{
    DWORD	dwByte(0);

    _uint iOverrideCount =  0;
    vector< OVERRIDE_MATERIAL_INFO> OverrideMaterials;
    CModel* pModel = m_pControllerModel->Get_Model(COORDINATE_3D);
    if (pModel)
    {
        auto pMaterials = static_cast<C3DModel*>(pModel)->Get_Materials();
     
        for (_uint iMaterialIndex = 0; iMaterialIndex < pMaterials.size(); ++iMaterialIndex)
        {
            for (_uint iTexTypeIndex = 1; iTexTypeIndex < aiTextureType_UNKNOWN; ++iTexTypeIndex)
            {
                _uint iTexIndex = Get_TextureIdx(iTexTypeIndex, iMaterialIndex);
                if (0 < iTexIndex)
                {
                    iOverrideCount++;
                    OverrideMaterials.push_back({ iMaterialIndex, iTexTypeIndex, iTexIndex });
                }
            }
        }
    }

    WriteFile(_hFile, &iOverrideCount, sizeof(_uint), &dwByte, nullptr);

    if (iOverrideCount > 0 && (_uint) OverrideMaterials.size() == iOverrideCount)
    {
        for (auto& tOverrideInfo : OverrideMaterials)
        {
            WriteFile(_hFile, &tOverrideInfo.iMaterialIndex, sizeof(_uint), &dwByte, nullptr);
            WriteFile(_hFile, &tOverrideInfo.iTexTypeIndex, sizeof(_uint), &dwByte, nullptr);
            WriteFile(_hFile, &tOverrideInfo.iTexIndex, sizeof(_uint), &dwByte, nullptr);
        }
    }

    return S_OK;
}


HRESULT CMapObject::Save_Override_Color(HANDLE _hFile)
{
    DWORD	dwByte(0);
    // 컬러를 섞을 갯수를 재고 기록한다.
    _uint iMaterialCount = 0;

    for_each(m_eColorShaderModes.begin(), m_eColorShaderModes.end(), [&iMaterialCount](auto eMode) {
        switch (eMode)
        {
        case Engine::C3DModel::COLOR_DEFAULT:
        case Engine::C3DModel::MIX_DIFFUSE:
        {
            iMaterialCount++;
        }
        break;
        default:
            break;
        }
        });
    WriteFile(_hFile, &iMaterialCount, sizeof(_uint), &dwByte, nullptr);

    // 컬러 정보가 있는 마테리얼이면, 마테리얼 인덱스-컬러모드-컬러 순으로 기록한다.
    for (_uint i = 0; i < (_uint)m_eColorShaderModes.size(); i++)
    {
        switch (m_eColorShaderModes[i])
        {
        case Engine::C3DModel::COLOR_DEFAULT:
        case Engine::C3DModel::MIX_DIFFUSE:
        {
            WriteFile(_hFile, &i, sizeof(_uint), &dwByte, nullptr);
            WriteFile(_hFile, &m_eColorShaderModes[i], sizeof(C3DModel::COLOR_SHADER_MODE), &dwByte, nullptr);

            _float4 fDefaultDiffuseColor = Get_Diffuse_Color(i);
            WriteFile(_hFile, &fDefaultDiffuseColor, sizeof(_float4), &dwByte, nullptr);
        }
            break;
        default:
            break;
        }
    }


    return S_OK;
}

HRESULT CMapObject::Save_Sksp(HANDLE _hFile)
{
    DWORD	dwByte(0);
    //_uint iSpsk = m_isSpsk ? 1 : 0;
    WriteFile(_hFile, &m_iSkspType, sizeof(_uint), &dwByte, nullptr);

    if (m_iSkspType != SKSP_NONE)
    {
        _char		szSaveSpskName[MAX_PATH];
        strcpy_s(szSaveSpskName, m_strSpskTag.c_str());
        WriteFile(_hFile, &szSaveSpskName, (DWORD)(sizeof(_char) * MAX_PATH), &dwByte, nullptr);
    }

    return S_OK;
}



HRESULT CMapObject::Load_Override_Material(HANDLE _hFile)
{

    DWORD	dwByte(0);
    _uint iOverrideCount = 0;

    ReadFile(_hFile, &iOverrideCount, sizeof(_uint), &dwByte, nullptr);
    if (0 < iOverrideCount)
    {
        for (_uint i = 0; i < iOverrideCount; i++)
        {
            OVERRIDE_MATERIAL_INFO tInfo = {};
            ReadFile(_hFile, &tInfo.iMaterialIndex, sizeof(_uint), &dwByte, nullptr);
            ReadFile(_hFile, &tInfo.iTexTypeIndex, sizeof(_uint), &dwByte, nullptr);
            ReadFile(_hFile, &tInfo.iTexIndex, sizeof(_uint), &dwByte, nullptr);

            Change_TextureIdx(tInfo.iTexIndex, tInfo.iTexTypeIndex, tInfo.iMaterialIndex);
        }
    }
    return S_OK;
}


HRESULT CMapObject::Load_Override_Color(HANDLE _hFile)
{
    DWORD	dwByte(0);
    _uint iMaterialCount = 0;
    ReadFile(_hFile, &iMaterialCount, sizeof(_uint), &dwByte, nullptr);

    for (_uint i = 0; i < iMaterialCount; i++)
    {
        _uint iMaterialIndex = 0;
        ReadFile(_hFile, &iMaterialIndex, sizeof(_uint), &dwByte, nullptr);
        ReadFile(_hFile, &m_eColorShaderModes[iMaterialIndex], sizeof(C3DModel::COLOR_SHADER_MODE), &dwByte, nullptr);
        switch (m_eColorShaderModes[iMaterialIndex])
        {
        case Engine::C3DModel::COLOR_DEFAULT:
        case Engine::C3DModel::MIX_DIFFUSE:
        {
            Set_Color_Shader_Mode(iMaterialIndex, m_eColorShaderModes[iMaterialIndex]);
            _float4 fDefaultDiffuseColor = {};
            ReadFile(_hFile, &fDefaultDiffuseColor, sizeof(_float4), &dwByte, nullptr);
            Set_Diffuse_Color(iMaterialIndex, fDefaultDiffuseColor);
        }
        break;
        default:
            break;
        }
    }
 //   DWORD	dwByte(0);
 //   ReadFile(_hFile, &m_eColorShaderModes[0], sizeof(C3DModel::COLOR_SHADER_MODE), &dwByte, nullptr);

	//_float4 fDefaultDiffuseColor = { 1.f,1.f,1.f,1.f };

 //   switch (m_eColorShaderModes[0])
 //   {
 //   case Engine::C3DModel::COLOR_DEFAULT:
 //   case Engine::C3DModel::MIX_DIFFUSE:
 //       ReadFile(_hFile, &fDefaultDiffuseColor, sizeof(_float4), &dwByte, nullptr);
 //       break;
 //   default:
 //       break;
 //   }

 //   Set_Color_Shader_Mode(0, m_eColorShaderModes[0]);
 //   Set_Diffuse_Color(0, fDefaultDiffuseColor);

    return S_OK;
}

HRESULT CMapObject::Load_Sksp(HANDLE _hFile)
{
    DWORD	dwByte(0);
    _uint iSpsk =  0;
    ReadFile(_hFile, &m_iSkspType, sizeof(_uint), &dwByte, nullptr);

    _bool isSksp = 0 <  m_iSkspType ? true : false;
    if (isSksp)
    {
        _char		szSaveSpskName[MAX_PATH];
        ReadFile(_hFile, &szSaveSpskName, (DWORD)(sizeof(_char) * MAX_PATH), &dwByte, nullptr);
        m_strSpskTag = szSaveSpskName;
    }

    return S_OK;
}

_uint CMapObject::Get_MaterialCount()
{
    CModel* pModel = m_pControllerModel->Get_Model(COORDINATE_3D);
    if (pModel)
    {
        auto pMaterials = static_cast<C3DModel*>(pModel)->Get_Materials();

        return (_uint)pMaterials.size();
    }
    return 0;
}



#endif // _DEBUG


void CMapObject::Create_Complete()
{
    m_eMode = NORMAL; 
    XMStoreFloat4x4(&m_matWorld, Get_WorldMatrix());
}


CMapObject* CMapObject::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CMapObject* pInstance = new CMapObject(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        //MSG_BOX("Failed to Created : CMapObject");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CMapObject::Clone(void* _pArg)
{
    CMapObject* pInstance = new CMapObject(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        //MSG_BOX("Failed to Cloned : CMapObject");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMapObject::Free()
{


    __super::Free();
}
