#include "stdafx.h"
#include "Section_Manager.h"
#include "GameInstance.h"

#include "Section_2D.h"

IMPLEMENT_SINGLETON(CSection_Manager)
CSection_Manager::CSection_Manager()
    :m_pGameInstance(CGameInstance::GetInstance())
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CSection_Manager::Initialize(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
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

CSection* CSection_Manager::Find_Section(const _wstring& _strSectionTag)
{
    auto& iter = m_CurLevelSections.find(_strSectionTag);

    if (iter == m_CurLevelSections.end())
        return nullptr;

    return (*iter).second;
}

HRESULT CSection_Manager::Add_GameObject_ToSectionLayer(const _wstring& _strSectionTag, CGameObject* _pGameObject)
{
    CSection* pSection = Find_Section(_strSectionTag);
    if (nullptr == pSection)
        return E_FAIL;

    if (FAILED(pSection->Add_GameObject_ToSectionLayer(_pGameObject)))
        return E_FAIL;

    return S_OK;
}

HRESULT CSection_Manager::Add_GameObject_ToCurSectionLayer(CGameObject* _pGameObject)
{
    if (nullptr == m_pCurSection)
        return E_FAIL;
    return m_pCurSection->Add_GameObject_ToSectionLayer(_pGameObject);
}
HRESULT CSection_Manager::Remove_GameObject_ToCurSectionLayer(CGameObject* _pGameObject)
{
    if (nullptr == m_pCurSection)
        return E_FAIL;
    return m_pCurSection->Remove_GameObject_ToSectionLayer(_pGameObject);
}

HRESULT CSection_Manager::SetActive_Section(const _wstring& _strSectionTag, _bool _isActive)
{
    CSection* pSection = Find_Section(_strSectionTag);
    if (nullptr == pSection)
        return E_FAIL;

    /* Section Active를 False 처리하면, 내부적으로 Objects Set_Active()*/
    pSection->Set_Active(_isActive);

    return S_OK;
}

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

        if (nullptr != pSection)
        {
            if(FAILED(pSection->Add_RenderGroup_GameObjects()))
                return E_FAIL;
        }
    }

    return S_OK;
}

HRESULT CSection_Manager::Change_CurSection(const _wstring _strSectionKey)
{
    auto iter = m_CurLevelSections.find(_strSectionKey);
    if (iter != m_CurLevelSections.end())
    {
        m_CurActiveSections.clear();
        m_CurActiveSections.resize(m_iMaxCurActiveSectionCount);

        m_pCurSection = (*iter).second;

        Section_Active(m_pCurSection->Get_SectionName(), (m_iMaxCurActiveSectionCount / 2));

    

        return S_OK;
    }
    else
        return E_FAIL;
}

_bool CSection_Manager::Is_CurSection(CGameObject* _pGameObject)
{
    if(nullptr != m_pCurSection)
        return m_pCurSection->Is_CurSection(_pGameObject);
    return false;
}

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

ID3D11ShaderResourceView* CSection_Manager::Get_SRV_FromRenderTarget(const _wstring& _strSectionTag)
{
    CSection_2D* pSection_2D = dynamic_cast<CSection_2D*>(Find_Section(_strSectionTag));
    if (nullptr == pSection_2D)
        return nullptr;

    return pSection_2D->Get_SRV_FromRenderTarget();
}

ID3D11ShaderResourceView* CSection_Manager::Get_SRV_FromRenderTarget(_uint _iCurActiveIndex)
{
    if (0 >= _iCurActiveIndex || m_iMaxCurActiveSectionCount <= _iCurActiveIndex || nullptr == m_CurActiveSections[_iCurActiveIndex])
        return nullptr;

    CSection_2D* pSection_2D = dynamic_cast<CSection_2D*>(m_CurActiveSections[_iCurActiveIndex]);
    if (nullptr == pSection_2D)
        return nullptr;

    return pSection_2D->Get_SRV_FromRenderTarget();
}

ID3D11ShaderResourceView* CSection_Manager::Get_SRV_FromRenderTarget()
{
    CSection_2D* pSection_2D = dynamic_cast<CSection_2D*>(m_pCurSection);
    if (nullptr == pSection_2D)
        return nullptr;

    return pSection_2D->Get_SRV_FromRenderTarget();
}

ID3D11ShaderResourceView* CSection_Manager::Get_SRV_FromTexture(const _wstring& _strSectionTag, _uint _iTextureIndex)
{
    CSection_2D* pSection_2D = dynamic_cast<CSection_2D*>(Find_Section(_strSectionTag));
    if (nullptr == pSection_2D)
        return nullptr;

    return pSection_2D->Get_SRV_FromTexture(_iTextureIndex);
}

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
        _uint iModelSize = ChapterJson.size();
        m_2DModelInfos.resize(iModelSize);
        _uint iIndex = 0;
        for (auto ChildJson : ChapterJson)
        {
            m_2DModelInfos[iIndex].strModelName = ChildJson["TextureName"];
            m_2DModelInfos[iIndex].strModelType = ChildJson["ModelType"];
            m_2DModelInfos[iIndex].isActive = ChildJson["Active"];
            m_2DModelInfos[iIndex].isCollider = ChildJson["Collider"];
            m_2DModelInfos[iIndex].isSorting = ChildJson["Sorting"];

            if (m_isActive &&
                ChildJson.contains("ActivePropertis") &&
                ChildJson["ActivePropertis"].contains("ActiveType"))
            {
                m_2DModelInfos[iIndex].strActiveType = ChildJson["ActivePropertis"]["ActiveType"];
            }

            if (m_2DModelInfos[iIndex].isCollider && ChildJson.contains("ColliderPropertis"))
            {
                auto ColliderPropertis = ChildJson["ColliderPropertis"];
                if (ColliderPropertis.contains("ColliderType"))
                    m_2DModelInfos[iIndex].strColliderType = ColliderPropertis["ColliderType"];
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

    Safe_Release(m_pGameInstance);
    Safe_Release(m_pContext);
    Safe_Release(m_pDevice);

    __super::Free();
}
// TODO : Section 생성 시 Key와 Section Class 내부의 strName 일치.
// 현재 복수의 섹션을 생성하고 Layer에 삽입하는 것 까진 테스트 완 
// Renderer와 연동하는 작업 해야하고, 동적인 RTV 삽입 삭제에 대한 처리가 가능해야함
// 문제 1 : 개발자 입장에서 어떻게 Section의 RenderGroupTag를 알게하나 
// 해결 1 : Section 안에 있는 Object는 원래 Section이 RenderGroup을 지정하여 전달하기에 상관없다.
// 그러면 기존 프레임 워크에서 존재하던 3D 오브젝트들은 어떤식으로 처리할지, 기본 RenderGroup을 어떻게 편하게 접근하게 할지.
// Default 한 Object들은 최대한 기존 프레임워크 방향대로 그리게하고, Section이 생성되고 해당 Section 내부의 Object들은 Section에 의해서 
// Renderer에 전달되어야한다. 
// Player 같은 경우 Section 간의 이동 처리도 필요하고, Section 자체를 탈출할수도 있어야함. 