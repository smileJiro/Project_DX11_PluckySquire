#pragma once

#include "Playable.h"
#include "AnimEventReceiver.h"

BEGIN(Engine)
class CModelObject;
END

BEGIN(Client)

class CPip_Player : public CPlayable
{
public:
	typedef struct : public CCharacter::CHARACTER_DESC
	{
		
	} PIP_DESC;

	enum PIP_PART {PIP_BODY, PIP_LAST};

private:
	CPip_Player(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CPip_Player(const CPip_Player& _Prototype);
	virtual ~CPip_Player() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* _pArg) override;
	virtual void	Priority_Update(_float _fTimeDelta) override;
	virtual void	Update(_float _fTimeDelta) override;
	virtual void	Late_Update(_float _fTimeDelta) override;

private:
	CModelObject* m_pBody = nullptr;
	F_DIRECTION	  m_eDirection = F_DIRECTION::RIGHT;

private:
	HRESULT	Ready_Components();
	HRESULT	Ready_PartObjects();

public:
	static CPip_Player* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;
};

END