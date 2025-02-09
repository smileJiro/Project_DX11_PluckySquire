#pragma once

#include "Base.h"

BEGIN(Engine)
class CGameInstance;
class CGlobalFunction_Manager final : public CBase
{
private:
	CGlobalFunction_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CGlobalFunction_Manager() = default;

public:
	_float2				Get_CursorPos(HWND _hWnd);
	_vector				Get_OnClickPos(HWND _hWnd);

	string				WStringToString(const _wstring& _wstr);
	_wstring			StringToWString(const string& _str);
	
	_float3				Get_RotationFromMatrix_Quaternion(const _fmatrix& _WorldMatrix);
	_float				Nomalize_Angle(_float _fAngle);	// 0-360 사이로 각도 변환

	_float				Lerp(_float _fLeft, _float _fRight, _float _fRatio);
	_fvector			Get_BezierCurve(_fvector _vStartPoint, _fvector _vGuidePoint, _fvector _vEndPoint, _float _fRatio);

	_bool				MatrixDecompose(_float3* _vScale, _float4* _vQuaternion, _float3* _vPosition, FXMMATRIX _Matrix);		//원하는 성분만 넣으면 행렬에서 추출

	_float3				Get_ScaleFromMatrix(const _float4x4& _Matrix);
	_float3				Get_ScaleFromMatrix(const _fmatrix _Matrix);

	_float				Get_Angle_Between_Vectors(_fvector _vNormal, _fvector _vVector1, _fvector _vVector2);	//노말벡터 기준으로 방향 벡터 간 각도 구함 (0-360도 간)

	_float				Clamp_Degrees(_float _fDegrees);		//0~360도 사이로 만듦

	_uint					Compare_VectorLength(_fvector _vVector1, _fvector _vVector2);		//같으면 0 / 1번 벡터가 크면 1 / 2번 벡터가 크면 2
	_fvector			Rotate_Vector(_fvector _vAxis, _fvector _vVector, _float _fDegrees);		//각도 넣어서 벡터 회전

private:
	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pContext = nullptr;
	CGameInstance* m_pGameInstance = nullptr;

public:
	static CGlobalFunction_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END
