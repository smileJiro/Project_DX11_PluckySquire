#pragma once
#include "Animation3D.h"
#include "Channel.h"

BEGIN(AnimTool)
class CToolChannel
	:public CChannel
{
private:
	CToolChannel();
	CToolChannel(const CToolChannel& Prototype);
	virtual ~CToolChannel() = default;

public:
	HRESULT Initialize(ifstream& inFile, const C3DModel* pModel);
	void	Export(ofstream& _outfile);

private:
	_uint m_iNameLength = 0;
public:
	static CToolChannel* Create(ifstream& inFile, const C3DModel* pModel);
	virtual void Free() override;
};
class CToolAnimation3D :
    public CAnimation3D
{
private:
	CToolAnimation3D();
	CToolAnimation3D(const CToolAnimation3D& Prototype);
	virtual ~CToolAnimation3D() = default;

public:
	virtual HRESULT Initialize(ifstream& inFile, const class C3DModel* pModel);
	void	Export(ofstream& _outfile);

	void Set_TrackPosition(_float _fTrackPos);

	_float Get_TrackPosition();
	_float Get_SpeedMagnifier() { return m_fSpeedMagnifier; }
	string Get_Name() { return m_szName; }
	_bool Is_LoopAnim() { return m_bLoop; }
private:
	_uint m_iNameLength = 0;
public:
	static CToolAnimation3D* Create(ifstream& inFile, const class C3DModel* pModel);
	virtual CToolAnimation3D* Clone();
	virtual void Free() override;
};

END