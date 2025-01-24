#include "PartObject.h"
#include "GameInstance.h"

CPartObject::CPartObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CActorObject(_pDevice, _pContext)
{
}

CPartObject::CPartObject(const CPartObject& _Prototype)
    : CActorObject(_Prototype)
{
}

HRESULT CPartObject::Initialize_Prototype()
{

    return S_OK;
}

HRESULT CPartObject::Initialize(void* _pArg)
{
    PARTOBJECT_DESC* pDesc = static_cast<PARTOBJECT_DESC*>(_pArg);

    m_pParentMatrices[COORDINATE_2D] = pDesc->pParentMatrices[COORDINATE_2D];
    m_pParentMatrices[COORDINATE_3D] = pDesc->pParentMatrices[COORDINATE_3D];

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    return S_OK;
}

void CPartObject::Priority_Update(_float _fTimeDelta)
{
	__super::Priority_Update(_fTimeDelta);
}

void CPartObject::Update(_float _fTimeDelta)
{
    switch (m_pControllerTransform->Get_CurCoord())
    {
    case Engine::COORDINATE_2D:
        if (nullptr != m_pParentMatrices[COORDINATE_2D])
            XMStoreFloat4x4(&m_WorldMatrices[COORDINATE_2D], m_pControllerTransform->Get_WorldMatrix(COORDINATE_2D) * XMLoadFloat4x4(m_pParentMatrices[COORDINATE_2D]));
        else
            XMStoreFloat4x4(&m_WorldMatrices[COORDINATE_2D], m_pControllerTransform->Get_WorldMatrix(COORDINATE_2D));
        break;
    case Engine::COORDINATE_3D:
        if(nullptr != m_pParentMatrices[COORDINATE_3D])
        {
            if (m_pSocketMatrix)
            {
                _matrix		SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

           /*     for (size_t i = 0; i < 3; i++)
                    SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);*/
                XMStoreFloat4x4(&m_WorldMatrices[COORDINATE_3D], 
                    m_pControllerTransform->Get_WorldMatrix(COORDINATE_3D) 
                    * SocketMatrix 
                    * XMLoadFloat4x4(m_pParentMatrices[COORDINATE_3D]));

            }
            else
            {
				XMStoreFloat4x4(&m_WorldMatrices[COORDINATE_3D], m_pControllerTransform->Get_WorldMatrix(COORDINATE_3D) * XMLoadFloat4x4(m_pParentMatrices[COORDINATE_3D]));
            }

        }
        else
            XMStoreFloat4x4(&m_WorldMatrices[COORDINATE_3D], m_pControllerTransform->Get_WorldMatrix(COORDINATE_3D));
        break;
    default:
        break;
    }
}

void CPartObject::Late_Update(_float _fTimeDelta)
{
    switch (m_pControllerTransform->Get_CurCoord())
    {
    case Engine::COORDINATE_2D:
        if (nullptr != m_pParentMatrices[COORDINATE_2D])
            XMStoreFloat4x4(&m_WorldMatrices[COORDINATE_2D], m_pControllerTransform->Get_WorldMatrix(COORDINATE_2D) * XMLoadFloat4x4(m_pParentMatrices[COORDINATE_2D]));
        else
            XMStoreFloat4x4(&m_WorldMatrices[COORDINATE_2D], m_pControllerTransform->Get_WorldMatrix(COORDINATE_2D));
        break;
    case Engine::COORDINATE_3D:
        if (nullptr != m_pParentMatrices[COORDINATE_3D])
        {
            if (m_pSocketMatrix)
            {
                _matrix		SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);
                for (size_t i = 0; i < 3; i++)
                    SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);
                XMStoreFloat4x4(&m_WorldMatrices[COORDINATE_3D],
                    m_pControllerTransform->Get_WorldMatrix(COORDINATE_3D)
                    * SocketMatrix
                    * XMLoadFloat4x4(m_pParentMatrices[COORDINATE_3D]));
            }
            else
            {
                XMStoreFloat4x4(&m_WorldMatrices[COORDINATE_3D], m_pControllerTransform->Get_WorldMatrix(COORDINATE_3D) * XMLoadFloat4x4(m_pParentMatrices[COORDINATE_3D]));
            }
        }
        else
            XMStoreFloat4x4(&m_WorldMatrices[COORDINATE_3D], m_pControllerTransform->Get_WorldMatrix(COORDINATE_3D));
        break;
    default:
        break;
    }
}

HRESULT CPartObject::Render()
{
    return S_OK;
}

_vector CPartObject::Get_Position() const
{
    COORDINATE eCurCoord = m_pControllerTransform->Get_CurCoord();
    return XMLoadFloat4((_float4*)&m_WorldMatrices[eCurCoord].m[3]);
}

const _float3& CPartObject::Get_Scale() const
{
    COORDINATE eCurCoord = m_pControllerTransform->Get_CurCoord();
    return _float3( XMVectorGetX(XMVector3Length(XMLoadFloat3((_float3*)&m_WorldMatrices[eCurCoord].m[0]))),
                    XMVectorGetX(XMVector3Length(XMLoadFloat3((_float3*)&m_WorldMatrices[eCurCoord].m[1]))),
                    XMVectorGetX(XMVector3Length(XMLoadFloat3((_float3*)&m_WorldMatrices[eCurCoord].m[2]))));
}

void CPartObject::Free()
{
    for (_int i = 0; i < COORDINATE_LAST; ++i)
        m_pParentMatrices[i] = nullptr;
        
    __super::Free();
}

HRESULT CPartObject::Cleanup_DeadReferences()
{
    if (FAILED(__super::Cleanup_DeadReferences()))
        return E_FAIL;

    return S_OK;
}

#ifdef _DEBUG

HRESULT CPartObject::Imgui_Render_ObjectInfos()
{
    /* Current Coord */
    COORDINATE eCurCoord = m_pControllerTransform->Get_CurCoord();
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
    ImGui::InputFloat4("Part_vRight", (float*)&vRight, " %.2f", ImGuiInputTextFlags_EnterReturnsTrue);

    _float4 vUp = {};
    XMStoreFloat4(&vUp, m_pControllerTransform->Get_State(CTransform::STATE_UP));
    ImGui::InputFloat4("Part_vUp", (float*)&vUp, " %.2f", ImGuiInputTextFlags_EnterReturnsTrue);

    _float4 vLook = {};
    XMStoreFloat4(&vLook, m_pControllerTransform->Get_State(CTransform::STATE_LOOK));
    ImGui::InputFloat4("Part_vLook", (float*)&vLook, " %.2f", ImGuiInputTextFlags_EnterReturnsTrue);
    ImGui::EndDisabled();/* 수정 불가 영역 끝. */

    _float4 vPosition = {};
    ImGui::PushItemWidth(150.f);
    XMStoreFloat4(&vPosition, m_pControllerTransform->Get_State(CTransform::STATE_POSITION));
    if (ImGui::InputFloat3("..", (float*)&vPosition, " %.2f", ImGuiInputTextFlags_EnterReturnsTrue))
        Set_Position(XMLoadFloat4(&vPosition));
    ImGui::SameLine();
    ImGui::Text("%.2f", vPosition.w);
    ImGui::SameLine();
    ImGui::Text("Part_vPosition");


    ImGui::PushItemWidth(150.f);
    _float3 vScale = Get_Scale();
    if (ImGui::InputFloat3("       Part_vScale", (float*)&vScale, " %.2f", ImGuiInputTextFlags_EnterReturnsTrue))
        Set_Scale(vScale);
    ImGui::PopItemWidth();


    return S_OK;
}

#endif // _DEBUG
