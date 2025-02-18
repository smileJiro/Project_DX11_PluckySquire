#pragma once
#include "GameObject.h"

BEGIN(Engine)
class CGameInstance;
END
BEGIN(Client)


class CPooling_Manager final : public CBase
{
	DECLARE_SINGLETON(CPooling_Manager)
private:
	CPooling_Manager();
	virtual ~CPooling_Manager() = default;
	
public:
	HRESULT					Initialize(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	void					Update();

	HRESULT					Level_Enter(_int _iChangeLevelID);
	HRESULT					Level_Exit(_int _iChangeLevelID, _int _iNextChangeLevelID);

public:
	HRESULT					Register_PoolingObject(const _wstring& _strPoolingTag, const Pooling_DESC& _tPoolingDesc, CGameObject::GAMEOBJECT_DESC* _pDesc);
	//HRESULT					Create_Objects(const _wstring& _strPoolingTag, _uint iNumPoolingObjects, _float3* _pPosition = nullptr, _float4* _pRotation = nullptr, _float3* _pScaling = nullptr);
	//HRESULT					Create_Object(const _wstring& _strPoolingTag, _float3* _pPosition = nullptr, _float4* _pRotation = nullptr, _float3* _pScaling = nullptr);
	HRESULT					Create_Object(const _wstring& _strPoolingTag, COORDINATE eCoordinate, _float3* _pPosition = nullptr, _float4* _pRotation = nullptr, _float3* _pScaling = nullptr, const _wstring* _pSectionKey = nullptr);
	HRESULT					Create_Objects(const _wstring& _strPoolingTag, _uint iNumPoolingObjects, COORDINATE eCoordinate, _float3* _pPosition = nullptr, _float4* _pRotation = nullptr, _float3* _pScaling = nullptr, const _wstring* _pSectionKey = nullptr);

private:
	HRESULT					Pooling_Objects(const _wstring& _strPoolingTag, _uint _iNumPoolingObjects);

private:
	CGameInstance*			m_pGameInstance = nullptr;
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pContext = nullptr;
	_int					m_iCurLevelID = -1;

private:
	map<_wstring, vector<CGameObject*>> m_PoolingObjects;
	map<_wstring, pair<Pooling_DESC, CGameObject::GAMEOBJECT_DESC*>> m_ObjectsDescs;

private:
	vector<CGameObject*>* Find_PoolingObjects(const _wstring& _strPoolingTag);
	pair<Pooling_DESC, CGameObject::GAMEOBJECT_DESC*>* Find_PoolingDesc(const _wstring& _strPoolingTag);

public:
	virtual void Free() override;
};

END