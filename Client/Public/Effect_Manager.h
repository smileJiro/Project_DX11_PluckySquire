#pragma once
#include "Base.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CEffect_System;
class CGameInstance;
END

BEGIN(Client)

class CEffect_Manager : public CBase
{
	DECLARE_SINGLETON(CEffect_Manager)
private:
	CEffect_Manager();
	virtual ~CEffect_Manager() = default;
	

public:
	void					Update_Manager();
public:
	HRESULT					Add_Effect(class CEffect_System* _pEffectSystem, _bool _isInitialActive = false);

	void					Active_EffectPosition(const _tchar* _szEffectTag, _bool _isReset = true, _fvector _vPosition = XMVectorSet(0.f, 0.f, 0.f, 1.f));
	void					Active_EffectPositionID(const _tchar* _szEffectTag, _bool _isReset = true, _fvector _vPosition = XMVectorSet(0.f, 0.f, 0.f, 1.f),  _uint _iEffectID = 0);
	void					Active_Effect(const _tchar* _szEffectTag, _bool _isReset = true, const _float4x4* _pWorldMatrix = nullptr);
	void					Active_EffectID(const _tchar* _szEffectTag, _bool _isReset = true, const _float4x4* _pWorldMatrix = nullptr, _uint _iEffectID = 0);
	void					InActive_Effect(const _tchar* _szEffectTag);
	void					InActive_EffectID(const _tchar* _szEffectTag, _uint _iEffectID);
	void					Stop_Spawn(const _tchar* _szEffectTag, _float _fDelayTime = 5.f);
	void					Stop_SpawnID(const _tchar* _szEffectTag, _float _fDelayTime = 5.f, _uint _iEffectID = 0);



private:
	CGameInstance*										m_pGameInstance = { nullptr };
	multimap<const _wstring, class CEffect_System*>		m_EffectSystems;
	list<class CEffect_System*>							m_UpdateSystems;

private:
	class CEffect_System*	Find_Effect(const _tchar* _szEffectTag, _Out_ _bool* _pActive = nullptr);
	
#ifdef _DEBUG
private:
	void					Update_DebugWindow();
#endif
public:
	virtual void Free() override;
};

END