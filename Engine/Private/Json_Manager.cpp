#include "Json_Manager.h"

CJson_Manager::CJson_Manager()
{
}

HRESULT CJson_Manager::Load_Json(const _tchar* _szFilePath, _Out_ json* _pOutJson)
{
    if (nullptr == _pOutJson)
    {
        MSG_BOX("Json 객체가 존재하지 않습니다.");
        return E_FAIL;
    }

    std::ifstream inputFile(_szFilePath);
    if (!inputFile.is_open())
    {
        MSG_BOX("파일을 읽을 수가 없습니다.");
        inputFile.close();
        return E_FAIL;
    }
    
    inputFile >> *_pOutJson;
    inputFile.close();

    return S_OK;
}

CJson_Manager* CJson_Manager::Create()
{
    return new CJson_Manager();
}

void CJson_Manager::Free()
{
    __super::Free();
}
