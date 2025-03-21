 #include "stdafx.h"
#include "2DMapWordObject.h"
#include "GameInstance.h"
#include "Section_Manager.h"
#include "Camera_Manager.h"
#include "PlayerData_Manager.h"
#include "PlayerBomb.h"


C2DMapWordObject::C2DMapWordObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :C2DMapObject(_pDevice, _pContext)
{
}

C2DMapWordObject::C2DMapWordObject(const C2DMapWordObject& _Prototype)
    :C2DMapObject(_Prototype)
{
}

HRESULT C2DMapWordObject::Initialize_Prototype()
{
    return S_OK;
}

HRESULT C2DMapWordObject::Initialize(void* _pArg)
{
    if (nullptr == _pArg)
        return E_FAIL;
    WORD_OBJECT_DESC* pDesc = static_cast<WORD_OBJECT_DESC*>(_pArg);

    json& WordObjectJson = pDesc->WordObjectJson;

    // 풀 텍스트
    _string strText = WordObjectJson["Word_Object_Tag"];
    m_strWordObjectTag = StringToWstring(strText);

    _float2 fPos = { WordObjectJson["Position"][0].get<_float>(),
        WordObjectJson["Position"][1].get<_float>() };

    _float2 fScale = { WordObjectJson["Scale"][0].get<_float>(),
        WordObjectJson["Scale"][1].get<_float>() };

    pDesc->Build_2D_Transform(fPos, fScale);

    if (WordObjectJson.contains("TargetDiff"))
        m_fTargetDiff = WordObjectJson["TargetDiff"];
    else
        m_fTargetDiff = 13.f;

    if(WordObjectJson.contains("Image_Tag_List") 
        && WordObjectJson["Image_Tag_List"].is_array())
    {
        _uint iModelCount = (_uint)WordObjectJson["Image_Tag_List"].size();
        m_ModelNames.reserve(iModelCount);
        for (auto& ImageTagJson : WordObjectJson["Image_Tag_List"])
        {
            if (ImageTagJson.contains("ImagePath"))
            {
                _string strText = ImageTagJson["ImagePath"];
                m_ModelNames.push_back(StringToWstring(strText));
            }
        }

        if (!m_ModelNames.empty())
        {
            pDesc->Build_2D_Model(SECTION_MGR->Get_SectionLeveID(),
                m_ModelNames[0],
                TEXT("Prototype_Component_Shader_VtxPosTex")
                );
        }
    }
    
    if(WordObjectJson.contains("SFX_Tag_List") 
        && WordObjectJson["SFX_Tag_List"].is_array())
    {
        _uint iModelCount = (_uint)WordObjectJson["SFX_Tag_List"].size();
        m_SFXNames.reserve(iModelCount);
        for (auto& ImageTagJson : WordObjectJson["SFX_Tag_List"])
        {
            if (ImageTagJson.contains("SFXPath"))
            {
                _string strText = ImageTagJson["SFXPath"];
                m_SFXNames.push_back(StringToWstring(strText));
            }
        }
    }


    if (WordObjectJson.contains("Word_Action_List")
        && WordObjectJson["Word_Action_List"].is_array())
    {
        for (auto& WordActionJson : WordObjectJson["Word_Action_List"])
        {
            WORD_ACTION tAction = {};
            if (WordActionJson.contains("Controller_Index"))
                tAction.iControllerIndex = WordActionJson["Controller_Index"];
            if (WordActionJson.contains("Container_Index"))
                tAction.iContainerIndex = WordActionJson["Container_Index"];

            if (WordActionJson.contains("Word_Type"))
                tAction.iWordType = WordActionJson["Word_Type"];

            if (WordActionJson.contains("Action_Type"))
                tAction.eAction = WordActionJson["Action_Type"];
            
            if (WordActionJson.contains("Param"))
            {
                if(WordActionJson["Param"].is_boolean())
                    tAction.anyParam = (_bool)WordActionJson["Param"];
                if(WordActionJson["Param"].is_number())
                    tAction.anyParam = (_uint)WordActionJson["Param"];
                if(WordActionJson["Param"].is_number_float())
                    tAction.anyParam = (_float)WordActionJson["Param"];
            }
            m_Actions.push_back(tAction);
        }
    }





    HRESULT hr =__super::Initialize(pDesc);



	if (WordObjectJson.contains("Collider_Info"))
	{
		_float2 fExtent = {};
		_float2 fOffset = {0.f,0.f};
		if (WordObjectJson["Collider_Info"].contains("Collider_Extent"))
		{
			fExtent = { WordObjectJson["Collider_Info"]["Collider_Extent"][0].get<_float>(),
				WordObjectJson["Collider_Info"]["Collider_Extent"][1].get<_float>() };
		}
        if (WordObjectJson["Collider_Info"].contains("Collider_Offset"))
        {
            fOffset = { WordObjectJson["Collider_Info"]["Collider_Offset"][0].get<_float>(),
                WordObjectJson["Collider_Info"]["Collider_Offset"][1].get<_float>() };
        }




		/* Test 2D Collider */
		m_p2DColliderComs.resize(1);
		CCollider_AABB::COLLIDER_AABB_DESC AABBDesc = {};
		AABBDesc.pOwner = this;
		AABBDesc.vExtents = fExtent;
		AABBDesc.vScale = { 1.f,1.f };
		AABBDesc.vOffsetPosition = fOffset;
		AABBDesc.isBlock = true;
		AABBDesc.iCollisionGroupID = OBJECT_GROUP::MAPOBJECT;
		CCollider_AABB* pCollider = nullptr;
		if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
			TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_p2DColliderComs[0]), &AABBDesc)))
			return E_FAIL;
	}

    Register_OnAnimEndCallBack(bind(&C2DMapWordObject::MapWordObject_AnimEnd, this, placeholders::_1, placeholders::_2));


    return S_OK; /* hr */
}

void C2DMapWordObject::Priority_Update(_float _fTimeDelta)
{
    __super::Priority_Update(_fTimeDelta);
}
void C2DMapWordObject::Update(_float _fTimeDelta)
{

    Action_Process(_fTimeDelta);

    __super::Update(_fTimeDelta);

}

void C2DMapWordObject::Late_Update(_float _fTimeDelta)
{
    __super::Late_Update(_fTimeDelta);
}

HRESULT C2DMapWordObject::Render()
{
#ifdef _DEBUG
    if (!m_p2DColliderComs.empty())
        m_p2DColliderComs[0]->Render(SECTION_MGR->Get_Section_RenderTarget_Size(m_strSectionName));
#endif // _DEBUG
    return __super::Render();
}

_bool C2DMapWordObject::Check_Action(_uint _iControllerIndex, _uint _iContainerIndex, _uint _iWordIndex)
{
    _bool isExecute = false;

    if (m_iControllerIndex == _iControllerIndex
        && m_iContainerIndex == _iContainerIndex
        && m_iWordIndex == _iWordIndex)
        return false;

    auto iter = find_if(m_Actions.begin(), m_Actions.end(), [this,&isExecute, &_iControllerIndex, &_iContainerIndex, &_iWordIndex]
    (WORD_ACTION& tAction){
            return (tAction.iControllerIndex == _iControllerIndex
                && tAction.iContainerIndex == _iContainerIndex
                && tAction.iWordType == _iWordIndex);
        });

    return iter != m_Actions.end();
}

_bool C2DMapWordObject::Register_Action(_uint _iControllerIndex, _uint _iContainerIndex, _uint _iWordIndex)
{
	m_isRegistered = true;

	m_iContainerIndex = _iContainerIndex;
	m_iControllerIndex = _iControllerIndex;
    m_iWordIndex = _iWordIndex;
    
    m_isStartChase = false;
    m_isStartAction = false;
    m_fActionIntervalSecond = 2.f;
    m_fAccTime = 0.f;
    return true;
}

_bool C2DMapWordObject::Action_Execute(_uint _iControllerIndex, _uint _iContainerIndex, _uint _iWordIndex)
{
    _bool isExecute = false;
    for_each(m_Actions.begin(), m_Actions.end(), [this,&isExecute, &_iControllerIndex, &_iContainerIndex, &_iWordIndex]
    (WORD_ACTION& tAction){
            if (tAction.iControllerIndex == _iControllerIndex
                && tAction.iContainerIndex == _iContainerIndex
                && tAction.iWordType == _iWordIndex)
            {
                isExecute = true;
                m_iContainerIndex = _iContainerIndex;
                m_iControllerIndex = _iControllerIndex;
                m_iWordIndex = _iWordIndex;
                switch (tAction.eAction)
                {
                case IMAGE_CHANGE:
                {
                    _uint iImageIndex = any_cast<_uint>(tAction.anyParam);

                    if (m_iModelIndex != iImageIndex)
                        m_pControllerModel->Change_Model(COORDINATE_2D, SECTION_MGR->Get_SectionLeveID(), m_ModelNames[iImageIndex]);
                    m_iModelIndex = iImageIndex;

					if (m_SFXNames.empty() == false && (_uint)m_SFXNames.size() > iImageIndex)
						START_SFX(m_SFXNames[iImageIndex], 60.f, false);
                }
                break;
                case WORD_OBJECT_ACTIVE:
                {
                    _bool isActive = any_cast<_bool>(tAction.anyParam);
                    m_IsWordActive = isActive;
                    Set_Active(m_IsWordActive);
                }
                break;
                case ANIMATION_CHANGE:
                {
                    _uint iAnimIndex = any_cast<_uint>(tAction.anyParam);
                    Set_AnimationLoop(COORDINATE_2D, iAnimIndex, true);
                    Set_Animation(iAnimIndex);
                }
                break;
                case POSITION_CHANGE_X:
                {
                    _float fPostionX = any_cast<_float>(tAction.anyParam);
                    _vector vOriginalPosition = Get_FinalPosition();
                    Set_Position(XMVectorSetX(vOriginalPosition, fPostionX));
                }
                break;
                case POSITION_CHANGE_Y:
                {
                    _float fPostionY = any_cast<_float>(tAction.anyParam);
                    _vector vOriginalPosition = Get_FinalPosition();
                    Set_Position(XMVectorSetY(vOriginalPosition, fPostionY));
                }
                break;                
                case COLLIDER_ACTIVE:
                {
                    _bool isActive = any_cast<_bool>(tAction.anyParam);
                    if (!m_p2DColliderComs.empty() && nullptr != m_p2DColliderComs[0])
                        m_p2DColliderComs[0]->Set_Active(isActive);
                }
                break;
                case WORD_OBJECT_RENDER:
                {
                    _bool isRender = any_cast<_bool>(tAction.anyParam);
                    m_IsWordRender = isRender;
                    Set_Render(m_IsWordRender);
                }
                break;
                default:
					assert(nullptr);
                    break;
                }

            }
        });
    return isExecute;
}

const C2DMapWordObject::WORD_ACTION* C2DMapWordObject::Find_Action(_uint _iControllerIndex, _uint _iContainerIndex, _uint _iWordIndex)
{
    auto iter = find_if(m_Actions.begin(), m_Actions.end(), [&_iControllerIndex ,&_iContainerIndex, &_iWordIndex]
    (WORD_ACTION& tAction)->_bool {
            return tAction.iControllerIndex == _iControllerIndex
                && tAction.iContainerIndex == _iContainerIndex
                && tAction.iWordType == _iWordIndex;
                
        });

    if (m_Actions.end() != iter)
    {
        return &(*iter);
    }

    return nullptr;
}

void C2DMapWordObject::Action_Process(_float _fTimeDelta)
{
	if (false == m_isRegistered)
        return;

    if (false == m_isStartChase)
    {
        CPlayerData_Manager::GetInstance()->Get_CurrentPlayer_Ptr()->Set_BlockPlayerInput(true);

        CCamera_Manager::GetInstance()->Change_CameraTarget(this);
        CCamera_Manager::GetInstance()->Start_Changing_ArmLength_Increase(
            (CCamera_Manager::CAMERA_TYPE)CCamera_Manager::GetInstance()->Get_CameraType(),
            1.f,
            5.f,
            EASE_IN_OUT
        );
        m_isStartChase = true;
    }
    else if(false == m_isStartAction)
    {
        _vector v3DPosition = SECTION_MGR->Get_WorldPosition_FromWorldPosMap(m_strSectionName,
            _float2(XMVectorGetX(m_pControllerTransform->Get_State(CTransform::STATE_POSITION)), XMVectorGetY(m_pControllerTransform->Get_State(CTransform::STATE_POSITION))));
        v3DPosition = XMVectorSetY(v3DPosition, XMVectorGetZ(v3DPosition));
        _vector vCamPosition = CCamera_Manager::GetInstance()->Get_CameraVector(CTransform::STATE_POSITION);
        vCamPosition = XMVectorSetY(vCamPosition, XMVectorGetZ(vCamPosition));

        if (XMVectorGetX(XMVector2Length(v3DPosition - vCamPosition)) < m_fTargetDiff)
            m_isStartAction = true;
    }
    else 
    {
        if (0.f < m_fActionIntervalSecond)
        {
            m_fAccTime += _fTimeDelta;
        
			if (m_fAccTime >= m_fActionIntervalSecond)
			{
                m_fActionIntervalSecond *= -1.f;
                m_fActionIntervalSecond += 1.5f;
                m_fAccTime = 0.f;
				SECTION_MGR->Word_Action_To_Section(m_strSectionName, m_iControllerIndex, m_iContainerIndex, m_iWordIndex, false);
			}
        }
        else 
        {
            m_fAccTime -= _fTimeDelta;

            if (m_fAccTime <= m_fActionIntervalSecond)
            {
                // Time Interval
                auto pPlayer = CPlayerData_Manager::GetInstance()->Get_CurrentPlayer_Ptr();
                CCamera_Manager::GetInstance()->Change_CameraTarget(pPlayer);
                CPlayerData_Manager::GetInstance()->Get_CurrentPlayer_Ptr()->Set_BlockPlayerInput(false);
                CCamera_Manager::GetInstance()->Start_Changing_ArmLength_Decrease(
                    (CCamera_Manager::CAMERA_TYPE)CCamera_Manager::GetInstance()->Get_CameraType(),
                    1.f,
                    5.f,
                    EASE_IN_OUT
                );
                m_isRegistered = false;
            }
        }
    }
}

void C2DMapWordObject::On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
	__super::On_Collision2D_Enter(_pMyCollider, _pOtherCollider, _pOtherObject);
    
    if (m_strModelName == L"BoomBox")
    {
        if (0 == Get_CurrentAnimIndex())
        {
		    CPlayerBomb* pBomb = dynamic_cast<CPlayerBomb*>(_pOtherObject);
		    if (nullptr == pBomb)
		    	return;
            else
            {
                Switch_Animation(3);
            }
        }

    }

}

void C2DMapWordObject::Active_OnEnable()
{
    if (m_IsWordActive != true)
        Set_Active(m_IsWordActive);
}

void C2DMapWordObject::Active_OnDisable()
{
    if (m_IsWordActive != false)
        Set_Active(m_IsWordActive);
}

void C2DMapWordObject::MapWordObject_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx)
{
    if (m_strModelName == L"BoomBox" && 3 == Get_CurrentAnimIndex())
        Event_DeleteObject(this);
}


C2DMapWordObject* C2DMapWordObject::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    C2DMapWordObject* pInstance = new C2DMapWordObject(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : C2DMapWordObject");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* C2DMapWordObject::Clone(void* _pArg)
{
    C2DMapWordObject* pInstance = new C2DMapWordObject(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : C2DMapWordObject");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void C2DMapWordObject::Free()
{
    __super::Free();
}
