#pragma once
#include "Component.h"
BEGIN(Engine)

typedef struct tagAnimationController
{
	_float	fTransitonReadyPercent; // 전환 가능 시점.
	_float	fBlendDuration; // 전환 속도.
	_float	fPlaySpeedPercent; // 재생 속도 보정치.
	_bool	isLoop; // 루프 체크,
	_int	iNextAnimation; // 만약 루프가 아니면서 메세지 큐에 메세지가 없을 때, 호출 해야하는 애니메이션 저장.

}ANIM_CONTROLLER;



class CAnimation;
class CBone;
class CTransform;
class ENGINE_DLL CAnimator final : public CComponent
{
public:
	typedef struct tagAnimatorDesc
	{
		_uint iNumAnimations;
		_uint iNumBones;
		_float4x4 PreTransformMatrix;
		vector<CAnimation*>* pOriginAnimations;
		vector<CBone*>* pOriginBones;
		vector<ANIM_CONTROLLER>* pControllers;
	}ANIMATOR_DESC;
private:
	CAnimator(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CAnimator(const CAnimator& _Prototype);
	virtual ~CAnimator() = default;


public:
	virtual HRESULT			Initialize_Prototype();
	virtual HRESULT			Initialize(void* _pArg) override;


public:
	_bool					Play_Animation(_float _fTimeDelta, _bool* _isTransitionReady = nullptr);

public:
	void					Excute_AnimEvent(_float _fTimeDelta);
	void					SetUp_Animation(_uint _iAnimIndex, _bool _isLoop);

public:
	// Get
	_vector Get_RootLocalMovement() const { return XMLoadFloat3(&m_vRootLocalMovement); }
	_float Get_CurAnimation_TrackPosition();

	// Set
	void Set_AnimationPlaySpeedPercent(_uint _iAnimIndex, _float _fSpeedPercent);
private: /* for. Default Data */
	_uint							m_iNumAnimations = 0;
	_uint							m_iNumBones = 0;
	_float4x4						m_PreTransformMatrix = {};
	vector<CAnimation*>				m_Animations;
	vector<CBone*>					m_Bones;


private: /* for. Animation Blend */
	unordered_map<_uint, KEYFRAME>	m_CurAnimFirstKeyFrames = {};
	vector<_uint>					m_BlendBoneIndices = {}; // 애니메이션 블렌드에 필요한 본 인덱스들.
	vector<KEYFRAME>				m_BlendBoneKeyFrames = {}; // 블렌드 요청 시점의 각각의 본들의 키프레임 값.
	_float							m_fBlendCurTrackPosition = 0.0f;
	_float							m_fBlendDuration = 0.1f; // 일단 모두 0.2고정으로 테스트.
	_bool							m_isBlend = false;

private:
	_float3							m_PreRootBonePos = {};

private: /* for. Animation Controller */
	vector<ANIM_CONTROLLER>			m_Controllers = {};

	_uint							m_iCurAnimIndex = 0;
	_uint							m_iPreAnimIndex = 0;
	_bool							m_isTransitionReady = false;
	_bool							m_isLoop = false;
	_bool							m_isFinished = false;

	list<_int>						m_AnimPriorityQueue; // 최우선 
	list<_int>						m_AnimQueue; // 일반
	_uint							m_iQueueMaxSize = 3;
	_float							m_fPriorityQueueResetTime = 3.0f;
	_float							m_fQueueResetTime = 3.0f;

private: /* for. Root Motion */
	_float3							m_vPreBonePosition = { 0.0f, 0.0f, 0.0f };
	_float3							m_vRootLocalMovement = { 0.0f, 0.0f, 0.0f };
private:
	void					BlendAnimation(_float _fTimeDelta);


public:
	static CAnimator* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CComponent* Clone(void* _pArg) override;
	virtual void Free() override;


};

END
