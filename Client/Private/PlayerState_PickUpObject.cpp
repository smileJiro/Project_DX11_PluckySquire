#include "stdafx.h"
#include "PlayerState_PickUpObject.h"
#include "Animation3D.h"
#include "CarriableObject.h"
#include "Actor_Dynamic.h"

CPlayerState_PickUpObject::CPlayerState_PickUpObject(CPlayer* _pOwner)
	:CPlayerState(_pOwner, CPlayer::PICKUPOBJECT)
{
}

//시작 상태, 목표 상태, 애니메이션 진행도 를 이용해 보간.
void CPlayerState_PickUpObject::Update(_float _fTimeDelta)
{
	COORDINATE eCOord = m_pOwner->Get_CurCoord();
	KEYFRAME tLeftKeyFrame; 
	KEYFRAME tRightKeyFrame;
	_float fRatio = 0.f;
	_float fProgress = m_pOwner->Get_AnimProgress();
	_float fAlignStartProgress = COORDINATE_3D == eCOord ? m_f3DAlignStartProgress : m_f2DAlignStartProgress;
	_float fAlignEndProgress = COORDINATE_3D == eCOord ? m_f3DAlignEndProgress : m_f2DAlignEndProgress;
	_float fHeadProgress = COORDINATE_3D == eCOord ? m_f3DHeadProgress : m_f2DHeadProgress;

	if (fProgress >= fHeadProgress)
	{
		if (false == m_bAligned)
		{
			Align();
			m_bAligned = true;
			return;
		}
	}
	 if( fProgress >= fAlignEndProgress)
	{
		fRatio = (fProgress - fAlignEndProgress) / (fHeadProgress - fAlignEndProgress);
		tLeftKeyFrame = m_tPickupKeyFrame;
		tRightKeyFrame = m_tCarryingKeyFrame;
	}
	else if(fProgress >=fAlignStartProgress )
	{
		fRatio = (fProgress - fAlignStartProgress) / (fAlignEndProgress - fAlignStartProgress);
		tLeftKeyFrame = m_tOriginalKeyFrame;
		tRightKeyFrame = m_tPickupKeyFrame;
	}
	else
	{
		 fRatio = 0;
		 tLeftKeyFrame = m_tOriginalKeyFrame;
		tRightKeyFrame = m_tOriginalKeyFrame;
	}
	 if (false == m_bAligned)
	 {
		 KEYFRAME tKeyFrame = Lerp_Frame(tLeftKeyFrame, tRightKeyFrame, fRatio);
		 _float4x4 matWorld;
		 XMStoreFloat4x4(&matWorld, XMMatrixAffineTransformation(XMLoadFloat3(&tKeyFrame.vScale), XMVectorSet(0.f, 0.f, 0.f, 1.f), XMLoadFloat4(&tKeyFrame.vRotation), XMVectorSetW(XMLoadFloat3(&tKeyFrame.vPosition), 1.f)));
		 m_pCarriableObject->Set_WorldMatrix(matWorld);
	 }
}

void CPlayerState_PickUpObject::Enter()
{
	assert(m_pOwner->Is_CarryingObject());
	m_pOwner->UnEquip_Part(CPlayer::PLAYER_PART_SWORD);
	COORDINATE eCoord = m_pOwner->Get_CurCoord();
	m_pCarriableObject = m_pOwner->Get_CarryingObject();


	//static_cast<CActor_Dynamic*>(m_pCarriableObject->Get_ActorCom())->Set_ShapeEnable((_uint)SHAPE_USE::SHAPE_BODY, false);

	//OriginalKeyFrame
	_matrix matOriginalOfset = m_pCarriableObject->Get_FinalWorldMatrix();
	_matrix vPlayerWorld = m_pOwner->Get_FinalWorldMatrix();
	matOriginalOfset *= XMMatrixInverse(nullptr, vPlayerWorld);
	m_tOriginalKeyFrame.Set_Matrix(matOriginalOfset);


	//PickUpKeyFrame
	_vector vTmp;
	if(COORDINATE_3D == eCoord)
	{
		vTmp = _vector{ 0,0,1 }*m_pOwner->Get_PickupRange(eCoord);
		vTmp = XMVectorSetY(vTmp, XMVectorGetY(vTmp) + 0.5f);
	}
	else
	{
		vTmp = m_pOwner->Get_LookDirection(COORDINATE_2D) * m_pOwner->Get_PickupRange(eCoord);
		vTmp = XMVectorSetY(vTmp, XMVectorGetY(vTmp) + 11.f);
	}
	_matrix mat3DPickupOffset = XMMatrixTranslationFromVector(vTmp);
	m_tPickupKeyFrame.Set_Matrix(mat3DPickupOffset);
	m_tPickupKeyFrame.vScale = m_tOriginalKeyFrame.vScale;

	//CarryingKeyFrame
	_matrix matCarryingOfset = XMLoadFloat4x4(&m_pCarriableObject->Get_HeadUpMatrix(eCoord));
	m_tCarryingKeyFrame.Set_Matrix(matCarryingOfset);
	m_tCarryingKeyFrame.vScale = m_tOriginalKeyFrame.vScale;

	if (COORDINATE_3D == eCoord)
	{
		m_pOwner->Set_Kinematic(true);
	}
	m_pCarriableObject->PickUpStart(m_pOwner, matOriginalOfset);


	if (COORDINATE_3D == eCoord)
	{
		m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::LATCH_PICKUP_GT);
	}
	else
	{
		F_DIRECTION eFDir = EDir_To_FDir(m_pOwner->Get_2DDirection());
		switch (eFDir)
		{
		case Client::F_DIRECTION::LEFT:
		case Client::F_DIRECTION::RIGHT:
			m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_PICKUP_OBJECT_RIGHT);
			break;
		case Client::F_DIRECTION::UP:
			m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_PICKUP_OBJECT_UP);
			break;
		case Client::F_DIRECTION::DOWN:
			m_pOwner->Switch_Animation((_uint)CPlayer::ANIM_STATE_2D::PLAYER_PICKUP_OBJECT_DOWN);
			break;
		default:
			break;
		}
	}
	//cout << "Enter: " << v.m128_f32[0] << " " << v.m128_f32[1] << " " << v.m128_f32[2] << endl;
}

void CPlayerState_PickUpObject::Exit()
{
	if (COORDINATE_3D == m_pOwner->Get_CurCoord())
	{
		//static_cast<CActor_Dynamic*>(m_pCarriableObject->Get_ActorCom())->Set_ShapeEnable((_uint)SHAPE_USE::SHAPE_BODY, true);
	}
	m_pOwner->Set_Kinematic(false);
	m_pOwner->Set_InteractObject(nullptr);
	//cout << "Align: " << v.m128_f32[0] << " " << v.m128_f32[1] << " " << v.m128_f32[2] << endl;
}

void CPlayerState_PickUpObject::On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx)
{
	if (COORDINATE_3D == _eCoord)
	{
		if (iAnimIdx == (_uint)CPlayer::ANIM_STATE_3D::LATCH_PICKUP_GT)
		{
			m_pOwner->Set_State(CPlayer::IDLE);
		}
	}
	else
	{
		if (iAnimIdx == (_uint)CPlayer::ANIM_STATE_2D::PLAYER_PICKUP_OBJECT_RIGHT
			|| iAnimIdx == (_uint)CPlayer::ANIM_STATE_2D::PLAYER_PICKUP_OBJECT_UP
			|| iAnimIdx == (_uint)CPlayer::ANIM_STATE_2D::PLAYER_PICKUP_OBJECT_DOWN)
		{
			m_pOwner->Set_State(CPlayer::IDLE);
		}
	}
}

void CPlayerState_PickUpObject::Align()
{
	m_pCarriableObject->PickUpEnd();

	//cout << "Align: " << v.m128_f32[0] << " " << v.m128_f32[1] << " " << v.m128_f32[2] << endl;
}
