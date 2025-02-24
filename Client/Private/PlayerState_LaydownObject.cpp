#include "stdafx.h"
#include "PlayerState_LaydownObject.h"
#include "Animation3D.h"
#include "CarriableObject.h"


CPlayerState_LaydownObject::CPlayerState_LaydownObject(CPlayer* _pOwner)
	:CPlayerState(_pOwner, CPlayer::LAYDOWNOBJECT)
{
}

void CPlayerState_LaydownObject::Update(_float _fTimeDelta)
{
	COORDINATE eCOord = m_pOwner->Get_CurCoord();

	m_fTimeAcc += _fTimeDelta;
	//cout << "time" << m_fTimeAcc ;
	//내려놓은 이후
	if (m_fTimeAcc >= m_fLerpEndTime)
	{
		if (false == m_bLayed)
		{
			Lay();
			m_bLayed = true;
		}
		//cout << "After Lay" << endl;
		return;
	}
	KEYFRAME tLeftKeyFrame;
	KEYFRAME tRightKeyFrame;
	_float fRatio = 0.f;
	//내려놓는 중
	if (m_fTimeAcc >= m_fLerpStartTime)
	{
		fRatio = (m_fTimeAcc - m_fLerpStartTime) / (m_fLerpEndTime - m_fLerpStartTime);
		tLeftKeyFrame = m_tCarryingKeyFrame;
		tRightKeyFrame = m_tLayDownFrame;
		//cout << "Layuing";
	}
	//내려놓을 준비
	else
	{
		fRatio = 0;
		tLeftKeyFrame = m_tCarryingKeyFrame;
		tRightKeyFrame = m_tCarryingKeyFrame;
		//cout << "Ready";
	}
	//cout << endl;

	KEYFRAME tKeyFrame = Lerp_Frame(tLeftKeyFrame, tRightKeyFrame, fRatio);
	_float4x4 matWorld;
	XMStoreFloat4x4(&matWorld, XMMatrixAffineTransformation(XMLoadFloat3(&tKeyFrame.vScale), XMVectorSet(0.f, 0.f, 0.f, 1.f), XMLoadFloat4(&tKeyFrame.vRotation), XMVectorSetW(XMLoadFloat3(&tKeyFrame.vPosition), 1.f)));
	m_pCarriableObject->Set_WorldMatrix(matWorld);
}

void CPlayerState_LaydownObject::Enter()
{
	COORDINATE eCoord = m_pOwner->Get_CurCoord();

	m_pOwner->Get_Body()->Set_ReverseAnimation(true);
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


	m_pCarriableObject = m_pOwner->Get_CarryingObject();
	m_pOwner->Set_CarryingObject(nullptr);
	_float fLayStartProgress = COORDINATE_2D == eCoord ? m_f2DLayStartProgress : m_f3DLayStartProgress;
	_float fFloorProgress = COORDINATE_2D == eCoord ? m_f2DFloorPrgress : m_f3DFloorPrgress;
	_float fAnimTime = m_pOwner->Get_AnimationTIme();
	m_fLerpStartTime = fAnimTime * (1-fLayStartProgress);
	m_fLerpEndTime = fAnimTime * (1-fFloorProgress);

	//TransformWOrld(0,0,0) * Socket(0,1,0) * Parent(10,0,10) * ParentBody(0,0,0)
	//소켓에서 떼고 TransformWOrld에 CarryingKeyFrame(0,1,0) 적용 
	//LerpStart때까지 대기
	//TransformWOrld를 (0,1,0)에서 (0,0,1)로 보간
	//LerpEnd이후엔 Parent,ParentBody를 떼고 TransformWorld에 Parent * LaydownMatrix 적용

	//여길 지나는 순간 TransformWOrld(10,1,10) * Parent(10,0,10) * ParentBody(000)
	m_pCarriableObject->Set_SocketMatrix(COORDINATE_2D, nullptr);
	m_pCarriableObject->Set_SocketMatrix(COORDINATE_3D, nullptr);

	_float3 vScale = m_pCarriableObject->Get_FinalScale();
	//CarryingKeyFrame
	_matrix matCarryingOfset = XMLoadFloat4x4(&m_pCarriableObject->Get_HeadUpMatrix(eCoord));
	m_tCarryingKeyFrame.Set_Matrix(matCarryingOfset);
	m_tCarryingKeyFrame.vScale = vScale;
	m_pCarriableObject->Set_WorldMatrix(m_tCarryingKeyFrame.Get_Matrix());

	_vector vTmp;
	if (COORDINATE_3D == eCoord)
	{
		vTmp = _vector{ 0,0,1 }*m_pOwner->Get_PickupRange(eCoord);
		vTmp = XMVectorSetY(vTmp, XMVectorGetY(vTmp) + 0.5f);
	}
	else
	{
		vTmp = m_pOwner->Get_LookDirection(COORDINATE_2D) * m_pOwner->Get_PickupRange(eCoord);
		vTmp = XMVectorSetY(vTmp, XMVectorGetY(vTmp) + 11.f);
	}
	_matrix mat3DPickupOffset = matCarryingOfset;
	mat3DPickupOffset.r[3].m128_f32[1] = 0.f;
	mat3DPickupOffset.r[3] += vTmp;
	m_tLayDownFrame.Set_Matrix(mat3DPickupOffset);
	m_tLayDownFrame.vScale = vScale;

	if (COORDINATE_3D == eCoord)
	{
		m_pCarriableObject->Set_Kinematic(true);
		m_pOwner->Set_Kinematic(true);
	}
}

void CPlayerState_LaydownObject::Exit()
{
	if(m_pOwner->Is_SwordMode())
		m_pOwner->Equip_Part(CPlayer::PLAYER_PART_SWORD);

	if (COORDINATE_3D == m_pOwner->Get_CurCoord())
	{
		m_pCarriableObject->Set_Kinematic(false);
		m_pOwner->Set_Kinematic(false);
	}
}

void CPlayerState_LaydownObject::On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx)
{
	m_pOwner->Get_Body()->Set_ReverseAnimation(false);
	m_pOwner->Set_State(CPlayer::IDLE);
}

void CPlayerState_LaydownObject::Lay()
{
	m_pCarriableObject->Set_ParentMatrix(COORDINATE_3D, nullptr);
	m_pCarriableObject->Set_ParentMatrix(COORDINATE_2D, nullptr);
	m_pCarriableObject->Set_SocketMatrix(COORDINATE_3D, nullptr);
	m_pCarriableObject->Set_SocketMatrix(COORDINATE_2D, nullptr);
	m_pCarriableObject->Set_ParentBodyMatrix(COORDINATE_3D, nullptr);
	m_pCarriableObject->Set_ParentBodyMatrix(COORDINATE_2D, nullptr);
	m_pCarriableObject->Set_Carrier(nullptr);

	_float4x4 matCarriableWorld;
	XMStoreFloat4x4(&matCarriableWorld, XMLoadFloat4x4(&m_tLayDownFrame.Get_Matrix()) * m_pOwner->Get_WorldMatrix());
	m_pCarriableObject->Set_WorldMatrix(matCarriableWorld);
}
