#pragma once
BEGIN(Engine)
class IAnimEventReceiver 
{
public:
	void Bind_AnimEventFunc(string _strFuncName, function<void()> _Func)
	{
		m_mapFunc[_strFuncName] = _Func;
	}
	void Invoke(string _strFuncName)
	{
		m_mapFunc[_strFuncName]();
	}
protected:
	map<string, function<void()>> m_mapFunc;
};

END