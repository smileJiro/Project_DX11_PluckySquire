#pragma once
#include "Character.h"
BEGIN(Client)
class CGear final : public CCharacter
{
public:
	enum GEAR_PART { GEAR_PART_GEAR, GEAR_PART_TEETH, GEAR_PART_LAST };
	typedef struct tagGearDesc : CCharacter::CHARACTER_DESC
	{

	}GEAR_DESC;
private:
	CGear(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CGear(const CGear& _Prototype);
	virtual ~CGear() = default;

public:
	HRESULT				Initialize_Prototype() override;
	HRESULT				Initialize(void* _pArg) override;
	void				Update(_float _fTimeDelta) override;
	HRESULT				Render() override;

private:
	_bool				m_isOnSound = { true };
	_bool				m_isStop = { false };

private:
	void				On_Stop() override;	   // 자기 partobject
	void				On_UnStop() override;  // 자기 partobject

private:
	HRESULT				Ready_Components(GEAR_DESC* _pDesc);
	HRESULT				Ready_PartObjects(GEAR_DESC* _pDesc);

public:
	static CGear*		Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CGameObject*		Clone(void* _pArg) override;
	void				Free() override;
};
END
