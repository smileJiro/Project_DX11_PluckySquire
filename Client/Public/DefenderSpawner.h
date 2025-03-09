#pragma once
#include "Base.h"
#include "DefenderMonster.h"
#include "Pooling_Manager.h"
BEGIN(Client)
class CSection_Manager;
class CDefenderSpawner :
    public CGameObject
{
public:
	enum SPAWN_PATTERN
	{
		PATTERN_DOT,
		PATTERN_DOT_SEQUENCE,
		PATTERN_ARROW,
		PATTERN_VERTICAL,
		PATTERN_RANDOM,
		PATTERN_LAST,
	};
	typedef struct tagDefenderSpawnerDesc : CGameObject::GAMEOBJECT_DESC
	{
		_wstring strPoolTag = TEXT("");
		_wstring strSectionName = TEXT("");
	}DEFENDER_SPAWNER_DESC;
private:
	CDefenderSpawner(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CDefenderSpawner(const CDefenderSpawner& _Prototype);
	virtual ~CDefenderSpawner() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* _pArg);
	virtual void	Update(_float _fTimeDelta);

public:
	void Spawn(SPAWN_PATTERN _ePattern, T_DIRECTION _eDirection, _float _fTimeStep = 0.f);
	void Spawn_Dot(T_DIRECTION _eDirection, _vector _vPosOffset = { 0.f,0.f,0.f });
	void Spawn_Dot_Sequence(T_DIRECTION _eDirection, _float _fTimeStep ,_vector _vPosOffset = { 0.f,0.f,0.f });
private: 

	CSection_Manager* m_pSection_Manager = nullptr;
	class CPooling_Manager* m_pPoolMgr = nullptr;
	_wstring m_strPoolTag = TEXT("");
	//_float4 m_vRightShootQuaternion = { 0.f,0.f,0.f,1.f };
	//_float4 m_vLeftShootQuaternion = { 0.f,0.f,0.f,1.f };
public:
	static CDefenderSpawner* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CGameObject* Clone(void* _pArg) override;
	HRESULT Cleanup_DeadReferences() override;
	virtual void Free() override;
};


END