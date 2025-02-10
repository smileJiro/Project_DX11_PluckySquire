#include "stdafx.h"
#include "Section_Manager.h"
#include "GameInstance.h"

#include "Section_2D.h"
#include "Collision_Manager.h"

IMPLEMENT_SINGLETON(CSection_Manager)
CSection_Manager::CSection_Manager()
    :m_pGameInstance(CGameInstance::GetInstance())
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CSection_Manager::Initialize(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{

    m_iCurActiveSectionIndex = m_iMaxCurActiveSectionCount / 2;

    if (nullptr == _pDevice)
        return E_FAIL;
    if (nullptr == _pContext)
        return E_FAIL;

    m_pDevice = _pDevice;
    Safe_AddRef(m_pDevice);

    m_pContext = _pContext;
    Safe_AddRef(m_pContext);

    m_CurActiveSections.resize(m_iMaxCurActiveSectionCount);

    return S_OK;
}
/// <summary>
/// _iNextChangeLevelID가 -1일 때, 리소스를 전부 로드한 뒤의 _iChangeLevelID레벨로 이동할 시점이라고 판단 
/// </summary>
/// <param name="_iChangeLevelID"></param>
/// <param name="_iNextChangeLevelID"></param>
HRESULT CSection_Manager::Level_Exit(_int _iChangeLevelID, _int _iNextChangeLevelID)
{
    m_iCurLevelID = _iChangeLevelID;

    Clear_Sections();

    /* 전환되는 레벨에 따른 Section 설정 필요. */
    _wstring strJsonPath = L"";
    _wstring strMapInfoJson = L"_Data";
    _wstring strModelInfoJsonName = L"_TagMatchingData";
    
    
    // 임시 : 로딩구조 바뀌면 변경
    _bool isSectionLoading = false;
    switch (m_iCurLevelID)
    {
    case Client::LEVEL_LOADING:
        strJsonPath = L"";
        break;
    case Client::LEVEL_STATIC:
        strJsonPath = L"";
        break;
    case Client::LEVEL_LOGO:
        strJsonPath = L"";
        break;
    case Client::LEVEL_GAMEPLAY:
        strJsonPath = L"Chapter1";
        isSectionLoading = true;
        break;
    default:
        return E_FAIL;
    }
    if(isSectionLoading)
    {
        if (FAILED(Ready_CurLevelSectionModels(strJsonPath + strModelInfoJsonName)))
            return E_FAIL;
        
        if (FAILED(Ready_CurLevelSections(strJsonPath + strMapInfoJson)))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CSection_Manager::Level_Enter(_int _iChangeLevelID)
{
    return S_OK;
}

/// <summary>
/// 섹션 키로 섹션 객체를 직접 찾는다.
/// </summary>
/// <param name="_strSectionTag">섹션 키</param>
CSection* CSection_Manager::Find_Section(const _wstring& _strSectionTag)
{
    auto& iter = m_CurLevelSections.find(_strSectionTag);

    if (iter == m_CurLevelSections.end())
        return nullptr;

    return (*iter).second;
}

/// <summary>
/// 섹션 키로 해당 섹션에 오브젝트를 추가한다.
/// </summary>
/// <param name="_strSectionTag">섹션 키</param>
/// <param name="_pGameObject">오브젝트</param>
HRESULT CSection_Manager::Add_GameObject_ToSectionLayer(const _wstring& _strSectionTag, CGameObject* _pGameObject)
{
    CSection* pSection = Find_Section(_strSectionTag);
    if (nullptr == pSection)
        return E_FAIL;

    if (FAILED(pSection->Add_GameObject_ToSectionLayer(_pGameObject)))
        return E_FAIL;

    return S_OK;
}

/// <summary>
/// 섹션 키로 해당 섹션에 해당 오브젝트를 제거한다.
/// </summary>
/// <param name="_strSectionTag">섹션 키</param>
/// <param name="_pGameObject">오브젝트</param>
HRESULT CSection_Manager::Remove_GameObject_ToSectionLayer(const _wstring& _strSectionTag, CGameObject* _pGameObject)
{
    CSection* pSection = Find_Section(_strSectionTag);
    if (nullptr == pSection)
        return E_FAIL;

    if (FAILED(pSection->Remove_GameObject_ToSectionLayer(_pGameObject)))
        return E_FAIL;

    return S_OK;
}
/// <summary>
/// x책에 활성화된 섹션에 오브젝트를 집어넣는다.
/// </summary>
/// <param name="_pGameObject">오브젝트</param>
/// <param name="_iLayerIndex">레이어 지정 (ref. Section_2D::SECTION_2D_RENDERGROUP)</param>
HRESULT CSection_Manager::Add_GameObject_ToCurSectionLayer(CGameObject* _pGameObject, _uint _iLayerIndex)
{
    if (nullptr == m_pCurSection)
        return E_FAIL;
    return m_pCurSection->Add_GameObject_ToSectionLayer(_pGameObject, _iLayerIndex);
}
/// <summary>
/// 책에 활성화된 섹션에서 오브젝트를 제거한다.
/// </summary>
/// <param name="_pGameObject">오브젝트</param>
HRESULT CSection_Manager::Remove_GameObject_ToCurSectionLayer(CGameObject* _pGameObject)
{
    if (nullptr == m_pCurSection)
        return E_FAIL;
    return m_pCurSection->Remove_GameObject_ToSectionLayer(_pGameObject);
}

/// <summary>
/// 해당 섹션의 활성화 여부를 결정한다.
/// </summary>
/// <param name="_strSectionTag">섹션 키</param>
/// <param name="_isActive">활성화 여부</param>
HRESULT CSection_Manager::SetActive_Section(const _wstring& _strSectionTag, _bool _isActive)
{
    CSection* pSection = Find_Section(_strSectionTag);
    if (nullptr == pSection)
        return E_FAIL;

    /* Section Active를 False 처리하면, 내부적으로 Objects Set_Active()*/
    pSection->Set_Active(_isActive);

    return S_OK;
}

/// <summary>
/// 섹션 루프 - 현재 활성화된 섹션에게 준비 작업을 진행하고, 렌더그룹에 오브젝트를 삽입하게끔 한다.
/// </summary>
HRESULT CSection_Manager::Section_AddRenderGroup_Process()
{
    for (auto& pSection : m_CurActiveSections)
    {
        if (pSection == nullptr)
            continue;
        CSection_2D* pSection_2D = dynamic_cast<CSection_2D*>(pSection);
        if (nullptr == pSection_2D)
            return E_FAIL;

        pSection_2D->Register_RenderGroup_ToRenderer();

        pSection_2D->Sort_Layer([](const CGameObject* pLeftGameObject, const CGameObject* pRightGameObject)->_bool {
            return XMVectorGetY(pLeftGameObject->Get_FinalPosition()) > XMVectorGetY(pRightGameObject->Get_FinalPosition());
            },CSection_2D::SECTION_2D_RENDERGROUP::SECTION_2D_OBJECT);

        if (nullptr != pSection)
        {
            if(FAILED(pSection->Add_RenderGroup_GameObjects()))
                return E_FAIL;
        }
    }

    return S_OK;
}

/// <summary>
/// 해당 섹션의 렌더타겟 사이즈를 가져온다.
/// </summary>
/// <param name="_strSectionTag">섹션 키</param>
_float2 CSection_Manager::Get_Section_RenderTarget_Size(const _wstring _strSectionKey)
{
    CSection* pSection = Find_Section(_strSectionKey);
    if (nullptr == pSection)
        return _float2(-1.f, -1.f);
    CSection_2D* pSection_2D = dynamic_cast<CSection_2D*>(pSection);
    if (nullptr == pSection_2D)
        return _float2(-1.f, -1.f);
    return pSection_2D->Get_RenderTarget_Size();
}

/// <summary>
/// 해당 섹션으로 책 활성화 섹션을 변경한다..
/// </summary>
/// <param name="_strSectionTag">섹션 키</param>
HRESULT CSection_Manager::Change_CurSection(const _wstring _strSectionKey)
{
    // Clear_Section
    //CCollision_Manager::GetInstance()->Register_Section();
    auto iter = m_CurLevelSections.find(_strSectionKey);
    if (iter != m_CurLevelSections.end())
    {
        m_CurActiveSections.clear();
        m_CurActiveSections.resize(m_iMaxCurActiveSectionCount);

        m_pCurSection = (*iter).second;

        if (m_pCurSection)
            CCollision_Manager::GetInstance()->Register_Section(m_pCurSection->Get_SectionName());
        Section_Active(m_pCurSection->Get_SectionName(), m_iCurActiveSectionIndex);

    

        return S_OK;
    }
    else
        return E_FAIL;
}

/// <summary>
/// 오브젝트가 현재 활성화된 책 섹션에 포함되어 있는가 확인한다.
/// </summary>
/// <param name="_pGameObject">오브젝트</param>
_bool CSection_Manager::Is_CurSection(CGameObject* _pGameObject)
{
    if(nullptr != m_pCurSection)
        return m_pCurSection->Is_CurSection(_pGameObject);
    return false;
}

/// <summary>
/// 오브젝트가 섹션에 포함되어있는지 확인하고, 있으면 섹션 키 포인트, 없으면 nullptr을 반환
/// </summary>
/// <param name="_pGameObject">오브젝트</param>
const _wstring* CSection_Manager::Get_SectionKey(CGameObject* _pGameObject)
{
    const _wstring* pReturn = nullptr;
    for (auto& SectionPair : m_CurLevelSections)
    {
        auto pLayer = SectionPair.second->Get_Include_Layer(_pGameObject);
        if (nullptr != pLayer)
            return &SectionPair.second->Get_SectionName();
    }
    return pReturn;
}

/// <summary>
/// 책 렌더타겟을 언맵하여 책 렌더타겟 기준 좌표에서 3D 공간의 World Position을 획득한다.
/// </summary>
/// <param name="_v2DTransformPosition">책 렌더타겟 기준 좌표(Proj 좌표)</param>
_vector CSection_Manager::Get_WorldPosition_FromWorldPosMap(_float2 _v2DTransformPosition)
{
    // 맵핑하여 데이터 접근
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    m_pContext->Map(m_pBookWorldPosMap, 0, D3D11_MAP_READ, 0, &mappedResource);

    // 2D Transform 위치를 픽셀 좌표계로 변환. 해당 텍스쳐의 가로 세로 사이즈를 알아야함.
    _int iWidth = mappedResource.RowPitch / sizeof(_float) / 4;
    _int iHeight = mappedResource.DepthPitch / mappedResource.RowPitch;

    // 원하는 픽셀 좌표 (픽셀 좌표는 UV 좌표를 변환해서 계산)
    _int iPixelX = (_int)std::ceil(_v2DTransformPosition.x) + iWidth / 2;            // X 좌표
    _int iPixelY = (_int)(std::ceil(_v2DTransformPosition.y) * -1.0f) + iHeight / 2; // Y 좌표

    // RowPitch는 한 줄의 바이트 수를 나타냄
    _float* fData = static_cast<_float*>(mappedResource.pData);

    // 픽셀 위치 계산 (4 floats per pixel)
    _int rowPitchInPixels = mappedResource.RowPitch / sizeof(_float) / 4;
    _int iIndex = iPixelY * rowPitchInPixels + iPixelX;

    if (iWidth * iHeight <= iIndex || 0 > iIndex)
        return XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
    
    
    _uint iDefaultIndex = iIndex * 4;

    // float4 데이터 읽기
    _float x = fData[iDefaultIndex]; // Red 채널
    _float y = fData[iDefaultIndex + 1]; // Green 채널
    _float z = fData[iDefaultIndex + 2]; // Blue 채널
    //_float w = fData[iIndex * 4 + 3]; // Alpha 채널

    if (0.f == x &&
        0.f == y &&
        0.f == z
        )
    {
        _vector vLerpPos =  XMVectorLerp(XMVectorSet(
            fData[iDefaultIndex - 4],
            fData[iDefaultIndex - 3],
            fData[iDefaultIndex - 2],
            fData[iDefaultIndex - 1]
        ),
            XMVectorSet(
                fData[iDefaultIndex + 4],
                fData[iDefaultIndex + 5],
                fData[iDefaultIndex + 6],
                fData[iDefaultIndex + 7]
            ), 0.5f);

        x = XMVectorGetX(vLerpPos);
        y = XMVectorGetY(vLerpPos);
        z = XMVectorGetZ(vLerpPos);
    }


    // 맵핑 해제
    m_pContext->Unmap(m_pBookWorldPosMap, 0);

    return XMVectorSet(x, y, z, 1.0f);
}
/// <summary>
/// 해당 섹션을 Section_2D로 간주하고, 2DMap의 RTV을 가져온다.
/// </summary>
/// <param name="_strSectionTag">섹션 키</param>
ID3D11RenderTargetView* CSection_Manager::Get_RTV_FromRenderTarget(const _wstring& _strSectionTag)
{
    /* Section 2D 인경우만 사용가능하다 */
    /* Get RTV, SRV 모두 프레임당 호출횟수가 많지 않을 것으로 예상되어 일단 이렇게 처리 함.*/
    /* 추후 성능적 개선이 필요하다면 section에 TypeID를 부여 */
    CSection_2D* pSection_2D = dynamic_cast<CSection_2D*>(Find_Section(_strSectionTag));
    if (nullptr == pSection_2D)
        return nullptr;

    return pSection_2D->Get_RTV_FromRenderTarget();
}
/// <summary>
/// 해당 섹션을 Section_2D로 간주하고, 2DMap의 SRV을 가져온다.
/// </summary>
/// <param name="_strSectionTag">섹션 키</param>
ID3D11ShaderResourceView* CSection_Manager::Get_SRV_FromRenderTarget(const _wstring& _strSectionTag)
{
    CSection_2D* pSection_2D = dynamic_cast<CSection_2D*>(Find_Section(_strSectionTag));
    if (nullptr == pSection_2D)
        return nullptr;

    return pSection_2D->Get_SRV_FromRenderTarget();
}
/// <summary>
/// 해당 섹션을 Section_2D로 간주하고, 2DMap의 SRV을 가져온다.
/// </summary>
/// <param name="_strSectionTag">활성화된 섹션 인덱스</param>
ID3D11ShaderResourceView* CSection_Manager::Get_SRV_FromRenderTarget(_uint _iCurActiveIndex)
{
    if (0 >= _iCurActiveIndex || m_iMaxCurActiveSectionCount <= _iCurActiveIndex || nullptr == m_CurActiveSections[_iCurActiveIndex])
        return nullptr;

    CSection_2D* pSection_2D = dynamic_cast<CSection_2D*>(m_CurActiveSections[_iCurActiveIndex]);
    if (nullptr == pSection_2D)
        return nullptr;

    return pSection_2D->Get_SRV_FromRenderTarget();
}
/// <summary>
/// 현재 책에 활성화된 메인 섹션을 Section_2D로 간주하고, 2DMap의 SRV을 가져온다.
/// </summary>
ID3D11ShaderResourceView* CSection_Manager::Get_SRV_FromRenderTarget()
{
    CSection_2D* pSection_2D = dynamic_cast<CSection_2D*>(m_pCurSection);
    if (nullptr == pSection_2D)
        return nullptr;

    return pSection_2D->Get_SRV_FromRenderTarget();
}
//뭐지 이거 필요한가
ID3D11ShaderResourceView* CSection_Manager::Get_SRV_FromTexture(const _wstring& _strSectionTag, _uint _iTextureIndex)
{
    CSection_2D* pSection_2D = dynamic_cast<CSection_2D*>(Find_Section(_strSectionTag));
    if (nullptr == pSection_2D)
        return nullptr;

    return pSection_2D->Get_SRV_FromTexture(_iTextureIndex);
}
/// <summary>
/// 책의 활성화 섹션을 지정할 때, 양옆 지정된 숫자만큼의 페이지를 활성화된 섹션으로 간주한다.
/// 재귀함수
/// </summary>
/// <param name="_strSectionTag">기준 활성화 섹션 키</param>
/// <param name="iCurSectionIndex">기준 활성화 섹션 인덱스</param>
void CSection_Manager::Section_Active(const _wstring& _strSectionTag, _uint iCurSectionIndex)
{
    CSection* pCurSection = Find_Section(_strSectionTag);
    if (nullptr == pCurSection)
        return;
    m_CurActiveSections[iCurSectionIndex] = pCurSection;

    if (FAILED(pCurSection->SetActive_GameObjects(true)))
        return;

    if (0 >= iCurSectionIndex || m_iMaxCurActiveSectionCount <= iCurSectionIndex)
        return;

    _uint iPreIndex = iCurSectionIndex - 1;
    _uint iNextIndex = iCurSectionIndex + 1;

    CSection_2D* pSection_2D = dynamic_cast<CSection_2D*>(Find_Section(_strSectionTag));
    if (nullptr == pSection_2D)
        return;

    _wstring strPageTag;
    if (pSection_2D->Is_NextPage(strPageTag) && m_CurActiveSections[iNextIndex] == nullptr)
        Section_Active(strPageTag, iNextIndex);

    if (pSection_2D->Is_PrePage(strPageTag) && m_CurActiveSections[iPreIndex] == nullptr)
        Section_Active(strPageTag, iPreIndex);
}

void CSection_Manager::Clear_Sections()
{
    for (auto& Pair : m_CurLevelSections)
        Safe_Release(Pair.second);
    m_CurActiveSections.clear();
    m_CurLevelSections.clear();
    m_pCurSection = nullptr;
    CCollision_Manager::GetInstance()->Clear_GroupFilter();
}

HRESULT CSection_Manager::Ready_CurLevelSectionModels(const _wstring& _strJsonPath)
{
    m_2DModelInfos.clear();
    _wstring strChapterPath = MAP_2D_DEFAULT_PATH;
    strChapterPath += L"ComponentTagMatching/";
    strChapterPath += _strJsonPath + L".json";

    json ChapterJson;
    std::ifstream inputFile(strChapterPath);
    if (!inputFile.is_open()) {
        throw std::runtime_error("json Error :  " + WstringToString(strChapterPath));
        return E_FAIL;
    }

    inputFile >> ChapterJson;
    if (ChapterJson.is_array())
    {
        _uint iModelSize = (_uint)ChapterJson.size();
        m_2DModelInfos.resize(iModelSize);
        _uint iIndex = 0;
        for (auto ChildJson : ChapterJson)
        {
            m_2DModelInfos[iIndex].strModelName = ChildJson["TextureName"];
            m_2DModelInfos[iIndex].eModelType = ChildJson["ModelType"];
            m_2DModelInfos[iIndex].isActive = ChildJson["Active"];
            m_2DModelInfos[iIndex].isCollider = ChildJson["Collider"];
            m_2DModelInfos[iIndex].isSorting = ChildJson["Sorting"];
            m_2DModelInfos[iIndex].isBackGround = ChildJson["BackGround"];

            if (m_isActive &&
                ChildJson.contains("ActivePropertis") &&
                ChildJson["ActivePropertis"].contains("ActiveType"))
            {
                m_2DModelInfos[iIndex].eActiveType = ChildJson["ActivePropertis"]["ActiveType"];
            }

            if (m_2DModelInfos[iIndex].isCollider && ChildJson.contains("ColliderPropertis"))
            {
                auto ColliderPropertis = ChildJson["ColliderPropertis"];
                if (ColliderPropertis.contains("ColliderType"))
                    m_2DModelInfos[iIndex].eColliderType = ColliderPropertis["ColliderType"];
                if (ColliderPropertis.contains("ColliderInfo"))
                {
                    auto ColliderInfo = ColliderPropertis["ColliderInfo"];

                    if (ColliderInfo.contains("Collider_Offset_Pos"))
                        m_2DModelInfos[iIndex].fCollider_Offset_Pos = { ColliderInfo["Collider_Offset_Pos"]["X"],ColliderInfo["Collider_Offset_Pos"]["Y"] };
                    if (ColliderInfo.contains("Collider_Extent"))
                        m_2DModelInfos[iIndex].fCollider_Extent = { ColliderInfo["Collider_Extent"]["X"],ColliderInfo["Collider_Extent"]["Y"] };
                    if (ColliderInfo.contains("Collider_Radius"))
                        m_2DModelInfos[iIndex].fCollider_Radius = ColliderInfo["Collider_Radius"];
                   
                }
            }
            if (m_2DModelInfos[iIndex].isCollider && ChildJson.contains("SortingPropertis"))
            {
                auto SortingPropertis = ChildJson["SortingPropertis"];
                if (SortingPropertis.contains("Sorting_Offset_Pos"))
                    m_2DModelInfos[iIndex].fSorting_Offset_Pos = { SortingPropertis["Sorting_Offset_Pos"]["X"],SortingPropertis["Sorting_Offset_Pos"]["Y"] };
            }
            iIndex++;
        }
    }

    return S_OK;
}

HRESULT CSection_Manager::Ready_CurLevelSections(const _wstring& _strJsonPath)
{
    m_CurLevelSections.clear();
    _wstring strChapterPath = MAP_2D_DEFAULT_PATH;
    strChapterPath += _strJsonPath + L".json";

    json ChapterJson ;
    std::ifstream inputFile(strChapterPath);
    if (!inputFile.is_open()) {
        throw std::runtime_error("json Error :  " + WstringToString(strChapterPath));
        return E_FAIL;
    }

    inputFile >> ChapterJson;

    _wstring strStartSectionKey = L"";

    if (ChapterJson.is_array())
    {
        _uint iIndex = 0;
        for (auto ChildJson : ChapterJson)
        {

            CSection_2D* pSection_2D = CSection_2D::Create(m_pDevice, m_pContext, m_iPriorityGenKey, ChildJson);
            if (nullptr == pSection_2D)
            {
                MSG_BOX("Failed Create CSection_2D");
                return E_FAIL;
            } 
            m_iPriorityGenKey += 10;
            

    

            if (m_CurLevelSections.empty())
                strStartSectionKey = pSection_2D->Get_SectionName();
            m_CurLevelSections.try_emplace(pSection_2D->Get_SectionName(), pSection_2D);
            iIndex++;
        }
    }


    Change_CurSection(strStartSectionKey);

    return S_OK;
}

void CSection_Manager::Free()
{
    Clear_Sections();

    Safe_Release(m_pBookWorldPosMap);

    Safe_Release(m_pGameInstance);
    Safe_Release(m_pContext);
    Safe_Release(m_pDevice);

    __super::Free();
}