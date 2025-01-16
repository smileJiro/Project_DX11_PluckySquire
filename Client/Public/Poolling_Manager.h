#pragma once
#include "GameObject.h"

BEGIN(Engine)
class CGameInstance;
END
BEGIN(Client)

typedef struct tagPoollingData
{
	_int iPrototypeLevelID;
	_wstring strPrototypeTag;
	_wstring strLayerTag;
}POOLLING_DESC;

class CPoolling_Manager final : public CBase
{
	DECLARE_SINGLETON(CPoolling_Manager)
private:
	CPoolling_Manager();
	virtual ~CPoolling_Manager() = default;
	
public:
	HRESULT					Initialize(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	void					Update();

	HRESULT					Level_Enter(_int _iChangeLevelID);
	HRESULT					Level_Exit(_int _iChangeLevelID, _int _iNextChangeLevelID);

public:
	HRESULT					Register_PoollingObject(const _wstring& _strPoollingTag, const POOLLING_DESC& _tPoollingDesc, CGameObject::GAMEOBJECT_DESC* _pDesc);
	HRESULT					Create_Objects(const _wstring& _strPoollingTag, _uint iNumPoolingObjects, _float3* _pPosition = nullptr, _float3* _pScaling = nullptr);
	HRESULT					Create_Object(const _wstring& _strPoollingTag, _float3* _pPosition = nullptr, _float3* _pScaling = nullptr);
	HRESULT					Pooling_Objects(const _wstring& _strPoollingTag, _uint _iNumPoolingObjects);

private:
	CGameInstance*			m_pGameInstance = nullptr;
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pContext = nullptr;
	_int					m_iCurLevelID = -1;

private:
	map<_wstring, vector<CGameObject*>> m_PoollingObjects;
	map<_wstring, pair<POOLLING_DESC, CGameObject::GAMEOBJECT_DESC*>> m_ObjectsDescs;

private:
	vector<CGameObject*>* Find_PoollingObjects(const _wstring& _strPoollingTag);
	pair<POOLLING_DESC, CGameObject::GAMEOBJECT_DESC*>* Find_PoollingDesc(const _wstring& _strPoollingTag);

public:
	virtual void Free() override;
};

END