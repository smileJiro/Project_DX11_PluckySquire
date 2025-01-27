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

    return S_OK;
}

HRESULT CSection_Manager::Level_Exit(_int _iChangeLevelID, _int _iNextChangeLevelID)
{
    m_iCurLevelID = _iChangeLevelID;

    Clear_Sections();

    /* 전환되는 레벨에 따른 Section 설정 필요. */
    _string strJsonPath;
    switch (m_iCurLevelID)
    {
    case Client::LEVEL_LOADING:
        strJsonPath = "";
        break;
    case Client::LEVEL_STATIC:
        strJsonPath = "";
        break;
    case Client::LEVEL_LOGO:
        strJsonPath = "";
        break;
    case Client::LEVEL_GAMEPLAY:
        strJsonPath = "";
        break;
    default:
        return E_FAIL;
    }

    if (FAILED(Ready_CurLevelSections(strJsonPath)))
        return E_FAIL;

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

HRESULT CSection_Manager::SetActive_Section(const _wstring& _strSectionTag, _bool _isActive)
{
    CSection* pSection = Find_Section(_strSectionTag);
    if (nullptr == pSection)
        return E_FAIL;

    /* Section Active를 False 처리하면, 내부적으로 Objects Set_Active()*/
    pSection->Set_Active(_isActive);

    return S_OK;
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

ID3D11ShaderResourceView* CSection_Manager::Get_SRV_FromTexture(const _wstring& _strSectionTag, _uint _iTextureIndex)
{
    CSection_2D* pSection_2D = dynamic_cast<CSection_2D*>(Find_Section(_strSectionTag));
    if (nullptr == pSection_2D)
        return nullptr;

    return pSection_2D->Get_SRV_FromTexture(_iTextureIndex);
}

void CSection_Manager::Clear_Sections()
{
    for (auto& Pair : m_CurLevelSections)
        Safe_Release(Pair.second);

    m_CurLevelSections.clear();
}

HRESULT CSection_Manager::Ready_CurLevelSections(const _string& _strJsonPath)
{
    /* Test Code */
    CSection_2D* pSection_2D = CSection_2D::Create(m_pDevice, m_pContext);
    if (nullptr == pSection_2D)
    {
        int a = 0;
    }
    m_CurLevelSections.emplace(TEXT("Section_Test"), pSection_2D);

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
