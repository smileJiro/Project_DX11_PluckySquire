#include "stdafx.h"
#include "PlayerState_Evict.h"
#include "Section_Manager.h"
#include "ModelObject.h"
#include "Actor_Dynamic.h"
#include "3DModel.h"
#include "Animation3D.h"
#include "Bone.h"
#include "GameInstance.h"
#include "Magic_Hand.h"

#include "Camera_Manager.h"
#include "Camera_Target.h"
#include "PlayerBody.h"

CPlayerState_Evict::CPlayerState_Evict(CPlayer* _pOwner)
	: CPlayerState(_pOwner,CPlayer::EVICT)
{
}


void CPlayerState_Evict::Update(_float _fTimeDelta)
{
	if (SMUSHLOOP == m_eCurState)
	{
		m_fSmushTimeAcc += _fTimeDelta;
		if (m_fSmushTimeAcc > m_fSmushTime)
		{
			_vector vPos = m_pOwner->Get_FinalPosition();
			_float4 v3DWorldPos = {};
			XMStoreFloat4(&v3DWorldPos, CSection_Manager::GetInstance()->Get_WorldPosition_FromWorldPosMap(m_pOwner->Get_Include_Section_Name(), _float2(XMVectorGetX(vPos), XMVectorGetY(vPos))));
			v3DWorldPos.y = 0.352f;

			m_pOwner->Change_Coordinate(COORDINATE_3D, (_float3*)&v3DWorldPos);
			m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::LATCH_ANIM_BOOKOUT_01_GT);
			m_pOwner->LookDirectionXZ_Kinematic(_vector{0,0,-1});
			m_eCurState = BOOKOUT;	
			
			// 손 다시 3D로 실행
			CMagic_Hand* pMagicHand = static_cast<CMagic_Hand*>(m_pGameInstance->Get_GameObject_Ptr(LEVEL_CHAPTER_2, TEXT("Layer_MagicHand"), 0));
			if (nullptr != pMagicHand)
				pMagicHand->Show_3DHand();

			CSection_Manager::GetInstance()->Remove_GameObject_ToCurSectionLayer(m_pOwner);

		}
	}
	else if (BOOKOUT == m_eCurState)
	{

	}
}

void CPlayerState_Evict::Enter()
{
	m_pOwner->Set_Kinematic(true);
	assert(COORDINATE_2D == m_pOwner->Get_CurCoord());
	m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::JOT_HANDEVICT);
	m_eCurState = GRABBED;
}

void CPlayerState_Evict::Exit()
{
	//m_pOwner->Set_Kinematic(false);
}

void CPlayerState_Evict::On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx)
{
	if (COORDINATE_2D == _eCoord)
	{
		if ((_uint)CPlayer::ANIM_STATE_2D::JOT_HANDEVICT == iAnimIdx)
		{
			m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::JOT_HANDEVICT_SMUSHLOOP);
			m_eCurState = SMUSHLOOP;
		}
	}
	else
	{
		if ((_uint)CPlayer::ANIM_STATE_3D::LATCH_ANIM_BOOKOUT_01_GT == iAnimIdx)
		{
			_float3 vPos;
			XMStoreFloat3(&vPos, m_pOwner->Get_RootBonePosition());
			vPos.y = 0.352f;
			m_pOwner->Set_Position(XMLoadFloat3(&vPos));
			CModelObject* pBody = m_pOwner->Get_Body();
			pBody->Set_Position({ 0,0,0 });

			//BodyTransformWOrld * ContainerWorld
			//m_pOwner->
			//static_cast<CActor_Dynamic*>(m_pOwner->Get_ActorCom())->Set_Dynamic();
			m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::LATCH_ANIM_IDLE_NERVOUS_01_GT);
			C3DModel* p3DModel = static_cast<C3DModel*>(pBody->Get_Model(COORDINATE_3D));
			vector<CBone*>& Bones = p3DModel->Get_Bones();
			static_cast<CAnimation3D*>(p3DModel->Get_Animation((_uint)CPlayer::ANIM_STATE_3D::LATCH_ANIM_IDLE_NERVOUS_01_GT))
				->Update_TransformationMatrices(Bones,0);
			for (auto& pBone : Bones)
				pBone->Update_CombinedTransformationMatrix(Bones, XMLoadFloat4x4(&p3DModel->Get_PreTransformMatrix()));


			m_pOwner->Set_Kinematic(false);
		}
		else if ((_uint)CPlayer::ANIM_STATE_3D::LATCH_ANIM_IDLE_NERVOUS_01_GT == iAnimIdx)
		{
			m_pOwner->Set_State(CPlayer::IDLE);
		}
	}
}
