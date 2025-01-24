#pragma once
#include "Bone.h"

BEGIN(AnimTool)
class CToolBone :
    public CBone
{
private:
	CToolBone();
	virtual ~CToolBone() = default;

public:
	virtual HRESULT Initialize(ifstream& _inFile, _int iParentBoneIndex) override;
	void						Export(ofstream& _outfile);
	_uint Get_NumChildBones() { return m_iNumChildBones; }
private:
	_uint m_iNumChildBones = 0;
public:
	static CToolBone* Create(ifstream& _inFile, _int _iParentBoneIndex);
	virtual CBone* Clone();
	virtual void Free() override;
};

END