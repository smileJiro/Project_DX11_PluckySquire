#pragma once
#include "Component.h"
BEGIN(Engine)
class IAnimEventReceiver;
typedef struct tagAnimEvent
{
	void WriteFile(std::ofstream& outFile)
	{
		outFile.write(reinterpret_cast<char*>(&iAnimIndex), sizeof(_uint));
		_uint iLength = strFuncName.length();
		outFile.write(reinterpret_cast<char*>(&iLength), sizeof(_uint));
		outFile.write(strFuncName.c_str(), iLength);
		outFile.write(reinterpret_cast<char*>(&fProgress), sizeof(_float));
	}
	void ReadFile(std::ifstream& inFile)
	{
		inFile.read(reinterpret_cast<char*>(&iAnimIndex), sizeof(_uint));
		_uint iLength = 0;
		inFile.read(reinterpret_cast<char*>(&iLength), sizeof(_uint));
		strFuncName.resize(iLength);
		inFile.read(const_cast<char*>(strFuncName.c_str()), iLength);
		inFile.read(reinterpret_cast<char*>(&fProgress), sizeof(_float));
	}
	_uint iAnimIndex = 0;
	string strFuncName;
	_float fProgress;
	_bool bIsTriggered = false;
}ANIM_EVENT;
class CAnimation;
class CModel;
class ENGINE_DLL CAnimEventGenerator :
    public CComponent
{
public:
	typedef struct tagAnimEventGeneratorDesc
	{
		CModel* pSenderModel = nullptr;
		IAnimEventReceiver* pReceiver = nullptr;
	}ANIMEVTGENERATOR_DESC;
private:
	CAnimEventGenerator(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CAnimEventGenerator(const CAnimEventGenerator& _Prototype);
	virtual ~CAnimEventGenerator() = default;
public:
	virtual HRESULT	 Initialize_Prototype(const _char* _pAnimEventFilePath);
	virtual HRESULT Initialize(void* _pArg) override;
    virtual void Update(_float fTimeDelta) override;

	void Register_Event(_uint iAnimIndex, const ANIM_EVENT& tEvent)
	{
		m_AnimEvents[iAnimIndex].push_back(tEvent);
	}
	void Reset()
	{
		for (auto& tAnimEvent : m_AnimEvents)
		{
			for (auto& tEvent : tAnimEvent.second)
			{
				tEvent.bIsTriggered = false;
			}
		}
	}

	map<_uint, vector< ANIM_EVENT>>& Get_AnimEvents() { return m_AnimEvents; }
#ifdef _TOOL
public:
	HRESULT Export_AnimEvents(ofstream& _OutFile);
#endif // _TOOL

private:
	CModel* m_pSenderModel = nullptr;
	IAnimEventReceiver* m_pReceiver = nullptr;
	//Key : AnimIndex, Value : AnimEventVector
    map<_uint,vector< ANIM_EVENT>>m_AnimEvents;

public:
	static CAnimEventGenerator* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _char* _pAnimEventFilePath);
	virtual CComponent* Clone(void* _pArg) override;
	virtual void Free() override;
};

END