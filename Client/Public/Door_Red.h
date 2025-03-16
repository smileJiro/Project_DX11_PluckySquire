			#pragma once
#include "Door_2D.h"

BEGIN(Client)
class CDoor_Red : public CDoor_2D
{
public:
	typedef struct tagDoorRedDesc : public CDoor_2D::DOOR_2D_DESC
	{
		_float		     fTargetDiff = -1.f;
		_wstring		 strLayerTag;
	}DOOR_RED_DESC;
private:
	CDoor_Red(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CDoor_Red(const CDoor_Red& _Prototype);
	virtual ~CDoor_Red() = default;

public:
	virtual HRESULT Initialize(void* _pArg) override;
	virtual void	Update(_float _fTimeDelta) override;


	void	Set_ClosingDoor();


public:
	void			On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx);

private:
	_bool					m_isStartClose = { false };
	_bool					m_isStartOpen = { false };
	_float					m_fTargetDiff = { -1.f };
	_wstring				m_strLayerTag;

private:
	void			Set_AnimLoop();
	virtual void	Switch_Animation_By_State() override;

	


public:
	static	CDoor_Red* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void			Free() override;
};

END