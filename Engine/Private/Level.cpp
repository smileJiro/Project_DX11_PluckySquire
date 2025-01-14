#include "Level.h"
#include "GameInstance.h"

CLevel::CLevel(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : m_pDevice(_pDevice)
    , m_pContext(_pContext)
    , m_pGameInstance(CGameInstance::GetInstance()) // 싱글톤 객체에 대한 포인터를 담아 놓음.
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
    Safe_AddRef(m_pGameInstance);

}

HRESULT CLevel::Initialize()
{

    return S_OK;
}

void CLevel::Update(_float _fTimeDelta)
{

}

HRESULT CLevel::Render()
{

    return S_OK;
}


void CLevel::Free()
{
    // Free()의 역할 : 소멸자는 소멸자 코드
    // 일반적인 소멸자라고 생각한다면, 자식 객체의 소멸자에 대한 실행 코드를 완수 한뒤 가장 마지막에
    // 부모 소멸자를 호출한다. 하지만, 만에하나 부모 클래스에서 자식 객체의 자원에 의존적인 경우, 
    // 자식객체에 존재하는 자원들이 미리 해제되어있으면 문제가 생긴다.
    // 부모 객체에서 자식 객체의 자원에 의존적인 코드를 작성하는 것은 일반적으로 좋은 설계는 아니지만,
    // 만약의 상황에는 Safe_Release()를 부모의 Free()함수 호출 후에 호출하여야한다.
    Safe_Release(m_pGameInstance);
    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
    __super::Free();
}
