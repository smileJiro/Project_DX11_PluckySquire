#pragma once
#include "Base.h"
BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)
class CEffect2D;
class CEffect2D_Manager : public CBase
{
	DECLARE_SINGLETON(CEffect2D_Manager)

private:
	CEffect2D_Manager();
	virtual ~CEffect2D_Manager() = default;

public:
	HRESULT								Initialize(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);

public:
	HRESULT								Level_Exit(_int _iChangeLevelID, _int _iNextChangeLevelID);
	HRESULT								Level_Enter(_int _iChangeLevelID);

public: /* Effect를 등록, 해제 */
	HRESULT								Register_EffectPool(const _wstring& _strEffectModelTag, LEVEL_ID _ePrototypeModelLevelID, _uint _iNumInitialPoolingEffects);
	HRESULT								Remove_EffectPool(const _wstring& _strEffectModelTag);
	vector<CEffect2D*>*					Find_EffectPool(const _wstring& _strEffectModelTag);

public: /* Effect를 생성 */
	HRESULT								Pooling_Effects(const _wstring& _strEffectModelTag, _uint _iNumInitialPoolingEffects);

public: /* Effect를 재생 */
	HRESULT								Play_Effect(const _wstring& _strEffectModelTag, 
		const _wstring& _strSectionKey, 
		_fmatrix _2DWorldMatrix, 
		_float _fDelayTime = 0.0f, 
		_uint _iAnimIndex = 0, 
		_bool _isLoop = false, 
		_float _fLifeTime = 0.0f,
		_uint _iSectionLayerGroup = SECTION_2D_PLAYMAP_EFFECT);

private:
	CGameInstance*						m_pGameInstance = nullptr;
	ID3D11Device*						m_pDevice = nullptr;
	ID3D11DeviceContext*				m_pContext = nullptr;
	_uint								m_iCurLevelID = 0;

private:
	map<_wstring, vector<CEffect2D*>>	m_EffectPools;
	
private:
	void Clear();
public:
	virtual void Free() override;
};
END
