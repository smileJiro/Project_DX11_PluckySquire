#pragma once
#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CParticle_System : public CGameObject
{
private:
	CParticle_System(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CParticle_System(const CParticle_System& _Prototype);
	virtual ~CParticle_System() = default;

public:

private:


public:
	static	CParticle_System* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _tchar* _szFilePath);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void		 Free() override;
	virtual HRESULT		 Cleanup_DeadReferences() override;
};

END