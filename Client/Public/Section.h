#pragma once
#include "Base.h"
#include "Layer.h"
BEGIN(Engine)
class CGameObject;
class CLayer;
class CGameInstance;
END

BEGIN(Client)
class CSection abstract : public CBase
{
public:
	typedef struct tagSectionDesc
	{
		_int		iLayerGroupCount = 1;
		_wstring	strSectionName;
		_uint		iGroupID = RG_2D;
		_uint		iPriorityID = 0;
	}SECTION_DESC;

protected:
	CSection(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CSection() = default;

public:
	// 1. Section 자체는 마땅히 할게 없다. Section_2D는 별도의 추가작업이 있겠지. 
	virtual HRESULT Initialize(SECTION_DESC* pDesc);

public: /* Object Layer와의 상호 작용 */
	// 1. Section Layer에 Object를 추가하는 기능. (o)
	virtual HRESULT Add_GameObject_ToSectionLayer(CGameObject* _pGameObject, _uint _iLayerIndex = 0);
	virtual HRESULT Change_GameObject_LayerIndex(CGameObject* _pGameObject, _uint _iLayerIndex = 0);
	virtual HRESULT Remove_GameObject_ToSectionLayer(CGameObject* _pGameObject);

public: // 왜 private?
	// 2. Section Layer에 Object의 Active를 변경하는 기능. (o)
	HRESULT			SetActive_GameObjects(_bool _isActive, _bool _isAllActive = true, _uint _iLayerIndex = 0);
	// 3. Section Layer에 Object를 Renderer의 자신의 그룹에 Add 하는 기능.(x)
	HRESULT			Add_RenderGroup_GameObjects();
	// 4. Section Layer에 Object의 DeadCheck를 하는 기능. (o)
	HRESULT			Cleanup_DeadReferences(_bool _isAllCheck = true, _uint _iLayerIndex = 0);
	// 5. Section Layer를 Clear 하는 기능. (o)
	void	Clear_GameObjects(_bool _isAllClear = true, _uint _iLayerIndex = 0);
	// 6. Section Layer 내부의 Object를 전부 Event_Delete 호출 후, Clear 하는 기능
	virtual HRESULT	CleanUp_Section();



	_bool	Get_RenderGroupKey(_uint& _iOutputGroupID, _uint& _iOutputPriorityID) {
		_iOutputGroupID = m_iGroupID;
		_iOutputPriorityID = m_iPriorityID;
		return true;
	};
	virtual HRESULT Section_AddRenderGroup_Process() abstract;
	_bool			Is_CurSection(CGameObject* _pGameObject);


	const _wstring& Get_SectionName() {
		return m_strName;
	}

	template <typename Comparator>
	void	Sort_Layer(Comparator _funcCompair, _uint _iLayerIndex = 0)
	{
		if (!Has_Exist_Layer(_iLayerIndex))
			return;

		m_Layers[_iLayerIndex]->Sort_Objects(_funcCompair);
	}

	CLayer* Get_Include_Layer(CGameObject* _pGameObject);

	CLayer* Get_Layer(_uint _iLayerIndex) { return m_Layers[_iLayerIndex]; }


	_bool		Is_PlayerInto() { return m_isPlayerInto; };
	void		Set_PlayerInto(_bool _isPlayerInto) { m_isPlayerInto = _isPlayerInto; };




protected:
	virtual HRESULT Layer_Sort() { return S_OK; };

protected:
	ID3D11Device*				m_pDevice = nullptr;
	ID3D11DeviceContext*		m_pContext = nullptr;
	CGameInstance*				m_pGameInstance = nullptr;

protected:
	_int						m_iGroupID = {};
	_int						m_iPriorityID = {};
	_uint						m_iLayerGroupCount = {};
	_wstring					m_strName;
	CLayer**					m_Layers = nullptr;
	_bool						m_isPlayerInto = false;
private:
	
	_bool Has_Exist_Layer(_uint _iLayerIndex) { return _iLayerIndex < m_iLayerGroupCount && nullptr != m_Layers[_iLayerIndex];  }

	/* Section은 update 기반이 아니다. 즉 본인이 Active False여도 기능수행 가능. */
	virtual void Active_OnEnable();
	virtual void Active_OnDisable();

public:
	void Free() override;
};

END