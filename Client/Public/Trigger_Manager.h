#pragma once
#include "Base.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CGameInstance;
class CGameObject;
class CTriggerObject;
END

BEGIN(Client)

class CTrigger_Manager final : public CBase
{
	DECLARE_SINGLETON(CTrigger_Manager)

public:
	enum EXIT_RETURN_MASK
	{
		NONE = 0x00,
		RIGHT = 0x01,
		LEFT = 0x02,
		UP = 0x04,
		DOWN = 0x08,
		RETURN_MASK_END
	};

private:
	CTrigger_Manager();
	virtual ~CTrigger_Manager() = default;

public:
	HRESULT						Initialize(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);

public:
	HRESULT						Load_Trigger(LEVEL_ID _eProtoLevelId, LEVEL_ID _eObjectLevelId, _wstring _szFilePath);

private:
	CGameInstance*				m_pGameInstance = nullptr;
	ID3D11Device*				m_pDevice = nullptr;
	ID3D11DeviceContext*		m_pContext = nullptr;

private:
	void						Resister_Event_Handler(_uint _iTriggerType, CTriggerObject* _pTrigger);

	_uint						Calculate_ExitDir(_fvector _vPos, _fvector _vOtherPos, PxBoxGeometry& _Box);

public:
	virtual void				Free() override;
};

END