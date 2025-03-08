#pragma once
#include "ModelObject.h"
BEGIN(Client)
class CZetPack_Child final : public CModelObject
{
public:
	enum STATE 
	{
		STATE_IDLE = 0,
		STATE_TALK = 3,
		STATE_LAST
	};
	enum DIRECTION
	{
		DIR_DOWN,
		DIR_RIGHT,
		DIR_UP,
		DIR_LEFT,

		DIR_LAST
	};
public:
	typedef struct tagZetPackChildDesc : CModelObject::MODELOBJECT_DESC
	{
		class CPlayer* pPlayer = nullptr;
	}ZETPACK_CHILD_DESC;

private:
	CZetPack_Child(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CZetPack_Child(const CZetPack_Child& _Prototype);
	virtual ~CZetPack_Child() = default;

public:
	virtual HRESULT							Initialize_Prototype(); // 프로토 타입 전용 Initialize
	virtual HRESULT							Initialize(void* _pArg); // 초기화 시 필요한 매개변수를 void* 타입으로 넘겨준다.
	virtual void							Priority_Update(_float _fTimeDelta) override;
	virtual void							Update(_float fTimeDelta) override;
	virtual void							Late_Update(_float _fTimeDelta) override;
	virtual HRESULT							Render() override;

private:
	CPlayer*								m_pPlayer = nullptr;

private:
	STATE									m_ePreState = STATE::STATE_LAST;
	STATE									m_eCurState = STATE::STATE_LAST;
	DIRECTION								m_eDirection = DIRECTION::DIR_DOWN;
	
private:
	void									State_Change();
	void									State_Change_Idle();
	void									State_Change_Talk();

private:
	void									Action_State(_float _fTimeDelta);
	void									Action_State_Idle(_float _fTimeDelta);
	void									Action_State_Talk(_float _fTimeDelta);

private:
	void									On_AnimEnd(COORDINATE _eCoord, _uint _iAnimIdx);

public:
	static CZetPack_Child*					Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CZetPack_Child*							Clone(void* _pArg);
	void									Free() override;

};
END
