#pragma once
#include "Base.h"

// 여기 엔진 오브젝트 헤더 모아둘게.

// 각각의 오브젝트를 임의의 기준으로 나누어 관리한다.


BEGIN(Engine)
class CGameObject;
class CController_Transform;
class CLayer;
class CComponent;
class CObject_Manager final : public CBase
{
private:
	CObject_Manager();
	virtual ~CObject_Manager() = default;

public:
	HRESULT Initialize(_uint _iNumLevels); // 해당 클라이언트의 Level 개수를 받는다.
	void Priority_Update(_float _fTimeDelta);
	void Update(_float _fTimeDelta);
	void Late_Update(_float _fTimeDelta);
	HRESULT Level_Exit(_uint _iLevelID);
public:
	_bool		Is_EmptyLayer(_uint _iLevelID, const _wstring& _strLayerTag);
	CLayer*		Find_Layer(_uint _iLevelID, const _wstring& _strLayerTag);
	/* Container용 과 Part용 분리. */
	CComponent* Find_Component(_uint _iLevelID, const _wstring& _strLayerTag, const _wstring& _strComponentTag, _uint _iObjectIndex = 0);
	CComponent* Find_Part_Component(_uint _iLevelID, const _wstring& _strLayerTag, _uint _iPartObjectIndex, const _wstring& _strPartComponentTag, _uint _iObjectIndex = 0);

public:
	// Object를 원하는 레이어에 추가. 하지만 이때 Prototype_Manger와 자동으로 연동되어 Proto해둔 객체들을 찾아 Layer에 넣는다.
	HRESULT Add_GameObject_ToLayer(_uint _iPrototypeLevelID, const _wstring& _strPrototypeTag, _uint _iLevelID, const _wstring& _strLayerTag, void* pArg = nullptr);
	HRESULT Add_GameObject_ToLayer(_uint _iPrototypeLevelID, const _wstring& _strPrototypeTag, _uint _iLevelID, const _wstring& _strLayerTag, CGameObject** _ppOut, void* pArg = nullptr);
	HRESULT Add_GameObject_ToLayer(_uint _iLevelID, const _wstring& _strLayerTag, CGameObject* _pGameObject); // 사본객체 생성 후 던지는.



	void	Set_Layer_Culling(_uint _iLevelID, const _wstring& _strLayerTag, _bool _isCulling);


public:
	CGameObject* Get_PickingModelObjectByCursor(_uint _iLevelID, const _wstring& _strLayerTag, _float2 _fCursorPos);
	/* Scale이 고려 된, 가장 가까운 오브젝트를 찾고 리턴하는 함수. */
	CGameObject* Find_NearestObject_Scaled(_uint _iLevelID, const _wstring& _strLayerTag, CController_Transform* const _pTransform, CGameObject* pExceptionObject = nullptr);

	CGameObject* Get_GameObject_Ptr(_int _iLevelID, const _wstring& _strLayerTag, _int _iObjectIndex);
	map<const _wstring, class CLayer*>* Get_Layers_Ptr() { return m_pLayers; }
	_uint Get_NumLevels() { return m_iNumLevels; }
private:
	_uint m_iNumLevels = 0;
	map<const _wstring, class CLayer*>* m_pLayers = nullptr;
	typedef map<const _wstring, class CLayer*> LAYERS;

	class CGameInstance* m_pGameInstance = nullptr;

private:
	HRESULT Clear(_uint _iLevelID); // Clear하고자하는 임의의 레벨에 대한 index를 매개변수로.
public:
	static CObject_Manager* Create(_uint iNumLevels);
	virtual void Free() override;
};

END