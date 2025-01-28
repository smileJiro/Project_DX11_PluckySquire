#include "Particle_Module.h"


CParticle_Module::CParticle_Module()
{
}

HRESULT CParticle_Module::Initialize(const json& _jsonModuleInfo)
{
    if (false == _jsonModuleInfo.contains("Type"))
        return E_FAIL;
    m_eModuleType = _jsonModuleInfo["Type"];
#ifdef _DEBUG
    m_strTypeName = _jsonModuleInfo["Type"];

#endif

    if (false == _jsonModuleInfo.contains("Order"))
        return E_FAIL;
    m_iOrder = _jsonModuleInfo["Order"];

    if (false == _jsonModuleInfo.contains("Init"))
        return E_FAIL;
    m_isInit = _jsonModuleInfo["Init"];


    switch (m_eModuleType)
    {
    case MODULENONE:
        break;
    case INIT_VELOCITY_POINT:
    {
        m_eApplyData = TRANSLATION;

        if (false == _jsonModuleInfo.contains("Origin"))
            return E_FAIL;
        if (FAILED(Add_Float3("Origin", _jsonModuleInfo)))
            return E_FAIL;

        if (false == _jsonModuleInfo.contains("Amount"))
            return E_FAIL;
        m_FloatDatas.emplace("Amount", _jsonModuleInfo["Amount"]);

        break;
    }
    case INIT_VELOCITY_LINEAR:
    {
        m_eApplyData = TRANSLATION;

        if (false == _jsonModuleInfo.contains("Amount"))
            return E_FAIL;
        if (FAILED(Add_Float3("Amount", _jsonModuleInfo)))
            return E_FAIL;

        break;
    }
    case INIT_ACCELERATION:
    {
        m_eApplyData = TRANSLATION;
        break;
    }
    case GRAVITY:
    {
        m_eApplyData = TRANSLATION;

        if (false == _jsonModuleInfo.contains("Amount"))
            return E_FAIL;
        if (FAILED(Add_Float3("Amount", _jsonModuleInfo)))
            return E_FAIL;

        break;
    }
    case DRAG:
    {
        m_eApplyData = TRANSLATION;

        if (false == _jsonModuleInfo.contains("Amount"))
            return E_FAIL;
        m_FloatDatas.emplace("Amount", _jsonModuleInfo["Amount"]);

        break;
    }
    case VORTEX_ACCELERATION:
    {
        m_eApplyData = TRANSLATION;

        if (false == _jsonModuleInfo.contains("Amount"))
            return E_FAIL;
        m_FloatDatas.emplace("Amount", _jsonModuleInfo["Amount"]);

        if (false == _jsonModuleInfo.contains("Axis"))
            return E_FAIL;
        if (FAILED(Add_Float3("Axis", _jsonModuleInfo)))
            return E_FAIL;

        if (false == _jsonModuleInfo.contains("Pull Amount"))
            return E_FAIL;
        m_FloatDatas.emplace("Pull Amount", _jsonModuleInfo["Pull Amount"]);

        if (false == _jsonModuleInfo.contains("Origin Point"))
            return E_FAIL;
        if (FAILED(Add_Float3("Origin Point", _jsonModuleInfo)))
            return E_FAIL;

        break;
    }
    default:
        break;
    }


    return S_OK;
}




void CParticle_Module::Update_Translations(_float _fTimeDelta, _float4* _pPosition, _float3* _pVelocity, _float3* _pAcceleration)
{
    if (false == m_isActive)
        return;

    switch (m_eModuleType)
    {
    case INIT_VELOCITY_POINT:
    {
        XMStoreFloat3(_pVelocity, XMLoadFloat4(_pPosition) - XMLoadFloat3(&m_Float3Datas["Origin"]) * m_FloatDatas["Amount"]);
        break;
    }   
    case INIT_VELOCITY_LINEAR:
    {

        *_pVelocity = m_Float3Datas["Amount"];
        break;
    }
    case INIT_ACCELERATION:
    {
        *_pAcceleration = *_pVelocity;
        break;
    }
    case GRAVITY:
    {
        XMStoreFloat3(_pAcceleration, XMLoadFloat3(_pAcceleration) + XMLoadFloat3(&m_Float3Datas["Amount"]) * _fTimeDelta);
        break;
    }
    case DRAG:
    {
        _vector vVelocity = XMLoadFloat3(_pVelocity) * (1.f - m_FloatDatas["Amount"] * _fTimeDelta);
        //_float fLength = XMVectorGetX(XMVector3Length(vVelocity));
       

        //XMStoreFloat3(_pVelocity, XMLoadFloat3(_pVelocity) - XMLoadFloat3(_pVelocity) * m_FloatDatas["Amount"] * _fTimeDelta);
        XMStoreFloat3(_pVelocity, vVelocity);
        break;
    }
    case VORTEX_ACCELERATION:
    {
      

        _vector vDiff = XMVectorSetW(XMLoadFloat4(_pPosition) - XMLoadFloat3(&m_Float3Datas["Origin Point"]), 0.f);
        _vector vR = vDiff - XMVector3Dot(XMVector3Normalize(XMLoadFloat3(&m_Float3Datas["Axis"])), vDiff) * XMLoadFloat3(&m_Float3Datas["Axis"]);
        //// ¹Ð¾î³»´Â Èû + ´ç±â´Â Èû
        _vector vVortex = XMVector3Normalize(XMVector3Cross(XMVector3Normalize(XMLoadFloat3(&m_Float3Datas["Axis"])), vR)) * m_FloatDatas["Amount"];
        _vector vPull = - XMVector3Normalize(vR) * m_FloatDatas["Pull Amount"];
        //
        XMStoreFloat3(_pAcceleration, XMLoadFloat3(_pAcceleration) + (vVortex + vPull) * _fTimeDelta);


        break;
    }

    default:
        break;
    }
}





HRESULT CParticle_Module::Add_Float3(const _char* _szTag, const json& _jsonInfo)
{
    if (3 > _jsonInfo[_szTag].size())
        return E_FAIL;
    
    _float3 vAdd;
    for (_int i = 0; i < 3; ++i)
        *((_float*)(&vAdd) + i) = _jsonInfo[_szTag][i];

    m_Float3Datas.emplace(_szTag, vAdd);



    return S_OK;
}

CParticle_Module* CParticle_Module::Create(const json& _jsonModuleInfo)
{
    CParticle_Module* pInstance = new CParticle_Module();

    if (FAILED(pInstance->Initialize(_jsonModuleInfo)))
    {
        MSG_BOX("Failed to Created : CParticle_Module");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CParticle_Module::Free()
{
    m_Float3Datas.clear();
    m_FloatDatas.clear();

    __super::Free();

}
#ifdef _DEBUG

HRESULT CParticle_Module::Initialize(MODULE_TYPE eType, const _string& _strTypeName)
{
    m_eModuleType = eType;
    m_strTypeName = _strTypeName;

    switch (m_eModuleType)
    {
    case MODULENONE:
        break;
    case INIT_VELOCITY_LINEAR:
        m_isInit = true;
        m_Float3Datas.emplace("Amount", _float3(0.f, 0.f, 0.f));
        break;
    case INIT_VELOCITY_POINT:
        m_isInit = true;
        m_Float3Datas.emplace("Origin", _float3(0.f, 0.f, 0.f));
        m_FloatDatas.emplace("Amount", 0.f);
        break;
    case INIT_ACCELERATION:
        m_isInit = true;
        break;
    case GRAVITY:
        m_isInit = false;
        m_Float3Datas.emplace("Amount", _float3(0.f, -5.f, 0.f));
        break;
    case DRAG:
        m_isInit = false;
        m_FloatDatas.emplace("Amount", 0.f);
        break;
    case VORTEX_ACCELERATION:
        m_isInit = false;
        m_FloatDatas.emplace("Amount", 0.f);
        m_FloatDatas.emplace("Pull Amount", 0.f);
        m_Float3Datas.emplace("Axis", _float3(0.f, 1.f, 0.f));
        m_Float3Datas.emplace("Origin Point", _float3(0.f, 0.f, 0.f));
        break;
    }

    return S_OK;
}

void CParticle_Module::Tool_Module_Update()
{
    ImGui::Text("Type :");
    ImGui::SameLine();
    ImGui::Text(m_strTypeName.c_str());

    if (ImGui::RadioButton("Active", m_isActive))
    {
        m_isActive = !m_isActive;
    }

    static _char szName[MAX_PATH] = "";
    if (ImGui::InputText("Set_Module_Name", szName, MAX_PATH))
    {
        if (0 != strcmp(szName, ""))
            m_strTypeName = szName;
    }

    if (ImGui::RadioButton("Is_Init ?", m_isInit))
    {
        m_isInit = !m_isInit;
        m_isChanged = true;
    }

    for (auto& Pair : m_FloatDatas)
    {
        if (ImGui::InputFloat(Pair.first.c_str(), &(Pair.second)))
            m_isChanged = true;
    }

    for (auto& Pair : m_Float3Datas)
    {
        if (ImGui::InputFloat3(Pair.first.c_str(), (_float*)&(Pair.second)))
            m_isChanged = true;

    }

}
HRESULT CParticle_Module::Save_Module(json& _jsonModuleInfo)
{
    _jsonModuleInfo["Init"] = m_isInit;
    _jsonModuleInfo["Type"] = m_eModuleType;
    _jsonModuleInfo["Order"] = m_iOrder;
    
    for (auto& Pair : m_FloatDatas)
    {
        _jsonModuleInfo[Pair.first.c_str()] = Pair.second;
    }

    for (auto& Pair : m_Float3Datas)
    {
        _jsonModuleInfo[Pair.first.c_str()].push_back(Pair.second.x);
        _jsonModuleInfo[Pair.first.c_str()].push_back(Pair.second.y);
        _jsonModuleInfo[Pair.first.c_str()].push_back(Pair.second.z);
    }


    return S_OK;
}
CParticle_Module* CParticle_Module::Create(MODULE_TYPE eType , const _string& _strTypeName)
{
    CParticle_Module* pInstance = new CParticle_Module();

    if (FAILED(pInstance->Initialize(eType, _strTypeName)))
    {
        MSG_BOX("Failed to Created : CParticle_Module");
        Safe_Release(pInstance);
    }

    return pInstance;
}
#endif