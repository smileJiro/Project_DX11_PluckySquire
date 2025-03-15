#pragma once
#include "Base.h"
#include "TrailInstance.h"

BEGIN(Engine)
class C3DModel;
class CShader;
class ENGINE_DLL CTrail_Manager final : public CBase
{
	DECLARE_SINGLETON(CTrail_Manager)
private:
	CTrail_Manager();
	virtual ~CTrail_Manager() = default;

public:
	void Update(_float _fTimeDelta);

public:
	vector<CTrailInstance*>* Find_Trails(_int _strTrailID);
	HRESULT Add_Trail(_int _strTrailID, CTrailInstance::TRAIL_DESC* _pDesc);
	HRESULT Render_Trails(_int _strTrailID, C3DModel* _pModelCom, CShader* _pShaderCom);
	
public:
	_int Create_TrailID() 
	{
		return m_iTrailID++;
	}
private:
	map<_int, vector<CTrailInstance*>> m_TrailInstances;
	static _int m_iTrailID;
public:
	void Free() override;
};

END
