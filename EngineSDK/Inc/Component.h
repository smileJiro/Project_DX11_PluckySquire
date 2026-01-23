#pragma once
#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CComponent abstract : public CBase
{
protected:
	CComponent(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CComponent(const CComponent& Prototype);
	virtual ~CComponent() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* _pArg);
	virtual void	Priority_Update(_float _fTimeDelta) { return; }
	virtual void	Update(_float _fTimeDelta) { return; }
	virtual void	Late_Update(_float _fTimeDelta) { return; }
#ifdef NDEBUG
public:
	virtual HRESULT Render() { return S_OK; }
#endif
protected:
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };
	class CGameInstance*		m_pGameInstance = { nullptr };

protected:
	_bool m_isCloned = false;
public:
	virtual CComponent* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END