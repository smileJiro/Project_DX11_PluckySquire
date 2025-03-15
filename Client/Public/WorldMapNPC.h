#pragma once
#include "Character.h"

// 이걸 상속 받는 놈들은 함수를 사용하게한다.
// abstract로 사용하게 한다.
// 이건 객체 생성하지 아니한다.
// 캐릭터 이동을 계산해준다.
// json을.. 여기로 받는다?
// 근데 객체 생성은 안하는데..



/// <생각 중>
/// json
/// 시작 위치 및 종료 위치 필요
/// 애니메이션 어떤걸로 변경할지 필요
/// 각 라인 마다 읽어서 해당 위치로 이동한다.
/// 종료 위치로 왔을 때 캐릭터의 애니메이션을 아이들로 변경 시킨다.
/// 
/// 
/// 
/// </생각 중>

BEGIN(Engine)
class CShader;
class CModel;
class CVIBuffer_Collider;
END

BEGIN(Client)
class CWorldMapNPC final  : public CCharacter
{
	enum STATE
	{
		STATE_READY,
		STATE_WAIT,
		STATE_WALK,
		STATE_ARRIVE,
		STATE_LAST
	};


	enum WORLDMAP_NPC // 파트 오브젝트로 넣을 놈
	{
		NPC_JOT,
		NPC_THRASH,
		NPC_VIOLET,
		NPC_LAST
	};

protected:
	explicit CWorldMapNPC(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	explicit CWorldMapNPC(const CWorldMapNPC& _Prototype);
	virtual ~CWorldMapNPC() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* _pArg) override;
	virtual void			Priority_Update(_float _fTimeDelta) override;
	virtual void			Update(_float _fTimeDelta) override;
	virtual void			Late_Update(_float _fTimeDelta) override;
	virtual HRESULT			Render() override;


protected:
	//virtual HRESULT			Ready_Components() override;
	HRESULT					Ready_PartObjects();
	
private:
	void					Progress(_float _fTimeDelta);
	void					Pos_Ready();
	void					Change_BookOrder();
	_float2					Change_PlayerPos();

public:
	static CWorldMapNPC*		Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject*	Clone(void* _pArg);
	virtual void			Free() override;
	HRESULT					Cleanup_DeadReferences() override;


private:
	_float2					m_PosMoves[6];

	_uint					m_iStartIndex = { 0 };

	wstring					m_strSection = { TEXT("") };

	STATE					m_eState = { STATE_READY };
	_float					m_fReadyTime = { 0.f };
	_float					m_fWaitTime = { 0.f };
	_float					m_fArriveWaitTime = { 0.f };

	_float2					m_vArrivePos = { 0.f, 0.f };
	_float2					m_vStartPos = { 0.f, 0.f };
	_bool					m_isUpdatePos = { false };

	_bool					m_isWalkAnim = { false };
	_bool					m_isArriveAnim = { false };

	_bool					m_isChangeCameraTarget = { false };


};

END