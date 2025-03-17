#pragma once
#include "Character.h"



BEGIN(Engine)
class CShader;
class CModel;
class CVIBuffer_Collider;
END




BEGIN(Client)
class CWorldMapNPC final  : public CCharacter
{
public:
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

	enum MOVEPOS
	{
		POS_HONEYBEE = 0,
		POS_TOWER,
		POS_SWAMPSTART,
		POS_SWAMPEND,
		POS_ATRIA,
		POS_MOUNTAIN,
		POS_LAST
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
	HRESULT					Progress(_float _fTimeDelta);
	HRESULT					Pos_Ready();
	void					Change_BookOrder();
	_float2					Change_PlayerPos();

public:
	static CWorldMapNPC*		Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject*	Clone(void* _pArg);
	virtual void			Free() override;
	HRESULT					Cleanup_DeadReferences() override;


private:
	_float2					m_PosMoves[POS_LAST] = { _float2(0.f, 0.f) };

	MOVEPOS					m_iStartIndex = { POS_LAST };

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