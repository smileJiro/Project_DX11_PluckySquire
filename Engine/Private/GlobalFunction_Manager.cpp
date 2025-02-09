#include "GlobalFunction_Manager.h"
#include "GameInstance.h"

CGlobalFunction_Manager::CGlobalFunction_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pGameInstance{ CGameInstance::GetInstance() }
	, m_pDevice{ pDevice }
	, m_pContext{ pContext }
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

_vector CGlobalFunction_Manager::Get_OnClickPos(HWND hWnd)
{
	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(hWnd, &pt);

	_vector vMousePos = XMVectorSet((_float)pt.x, (_float)pt.y, 0.f, 1.f);

	_uint		iNumViewports = { 1 };
	D3D11_VIEWPORT		ViewportDesc{};

	m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

	// 마우스 -> 투영
	vMousePos = XMVectorSet(pt.x / (ViewportDesc.Width * 0.5f) - 1.f,
		pt.y / -(ViewportDesc.Height * 0.5f) + 1.f,
		0.f,
		1.f);

	// 투영 -> 뷰 스페이스
	_matrix matProj = m_pGameInstance->Get_TransformInverseMatrix(CPipeLine::D3DTS_PROJ);
	vMousePos = XMVector3TransformCoord(vMousePos, matProj);

	// 뷰 -> 월드
	_matrix matView = m_pGameInstance->Get_TransformInverseMatrix(CPipeLine::D3DTS_VIEW);
	vMousePos = XMVector3TransformCoord(vMousePos, matView);

	return vMousePos;
}

_float3 CGlobalFunction_Manager::Get_RotationFromMatrix_Quaternion(const _fmatrix& worldMatrix)
{
	_vector vRight = XMVectorSet(worldMatrix.r[0].m128_f32[0], worldMatrix.r[0].m128_f32[1], worldMatrix.r[0].m128_f32[2], 0.f);
	_vector vUp = XMVectorSet(worldMatrix.r[1].m128_f32[0], worldMatrix.r[1].m128_f32[1], worldMatrix.r[1].m128_f32[2], 0.f);
	_vector vLook = XMVectorSet(worldMatrix.r[2].m128_f32[0], worldMatrix.r[2].m128_f32[1], worldMatrix.r[2].m128_f32[2], 0.f);

	_float fScaleX = XMVectorGetX(XMVector3Length(vRight));
	_float fScaleY = XMVectorGetX(XMVector3Length(vUp));
	_float fScaleZ = XMVectorGetX(XMVector3Length(vLook));

	// Scale 제거 후 행렬
	_matrix rotationMatrix = worldMatrix;
	rotationMatrix.r[0] = XMVectorScale(vRight, 1.f / fScaleX);
	rotationMatrix.r[1] = XMVectorScale(vUp, 1.f / fScaleY);
	rotationMatrix.r[2] = XMVectorScale(vLook, 1.f / fScaleZ);

	_vector vQuaternion = XMQuaternionRotationMatrix(rotationMatrix);
	_float4 fRot4;
	XMStoreFloat4(&fRot4, vQuaternion);

	_float3 fRotation;

	// Y축 회전(Yaw) 먼저
	fRotation.y = atan2f(2.f * (fRot4.w * fRot4.y - fRot4.z * fRot4.x),
		1.f - 2.f * (fRot4.y * fRot4.y + fRot4.x * fRot4.x));

	// X축 회전(Pitch) 다음
	float sinp = 2.f * (fRot4.w * fRot4.x + fRot4.y * fRot4.z);
	if (fabs(sinp) >= 1)
		fRotation.x = copysignf(XM_PIDIV2, sinp);
	else
		fRotation.x = asinf(sinp);

	// Z축 회전(Roll) 마지막
	fRotation.z = atan2f(2.f * (fRot4.w * fRot4.z + fRot4.x * fRot4.y),
		1.f - 2.f * (fRot4.z * fRot4.z + fRot4.x * fRot4.x));


	// 라디안을 degree로 변환
	fRotation.x = XMConvertToDegrees(fRotation.x);
	fRotation.y = XMConvertToDegrees(fRotation.y);
	fRotation.z = XMConvertToDegrees(fRotation.z);

	fRotation.x = Nomalize_Angle(fRotation.x);
	fRotation.y = Nomalize_Angle(fRotation.y);
	fRotation.z = Nomalize_Angle(fRotation.z);


	return fRotation;
}

_float CGlobalFunction_Manager::Nomalize_Angle(_float fAngle)
{
	if (fAngle < 0.f)
		return fAngle + 360.f;
	return fAngle;
}

_float CGlobalFunction_Manager::Lerp(_float fLeft, _float fRight, _float fRatio)
{
	if (fRatio > 1.f)
		fRatio = 1.f;

	return fLeft * (1 - fRatio) + fRight * fRatio;
}

_fvector CGlobalFunction_Manager::Get_BezierCurve(_fvector vStartPoint, _fvector vGuidePoint, _fvector vEndPoint, _float fRatio)
{
	_vector v1 = XMVectorLerp(vStartPoint, vGuidePoint, fRatio);
	_vector v2 = XMVectorLerp(vGuidePoint, vEndPoint, fRatio);
	_vector vResult = XMVectorLerp(v1, v2, fRatio);

	return vResult;
}

_bool CGlobalFunction_Manager::MatrixDecompose(_float3* _vScale, _float4* _vQuaternion, _float3* _vPosition, FXMMATRIX _Matrix)
{
	_vector vScale, vRotation, vPosition;
	if (false == XMMatrixDecompose(&vScale, &vRotation, &vPosition, _Matrix))
		return false;

	if (nullptr != _vScale)
		XMStoreFloat3(_vScale, vScale);
	if (nullptr != _vQuaternion)
		XMStoreFloat4(_vQuaternion, vRotation);
	if (nullptr != _vPosition)
		XMStoreFloat3(_vPosition, vPosition);

	return true;
}

_float3 CGlobalFunction_Manager::Get_ScaleFromMatrix(const _float4x4& _Matrix)
{
	return _float3(XMVectorGetX(XMVector3Length(XMLoadFloat4((_float4*)_Matrix.m[0]))),
				   XMVectorGetX(XMVector3Length(XMLoadFloat4((_float4*)_Matrix.m[1]))),
				   XMVectorGetX(XMVector3Length(XMLoadFloat4((_float4*)_Matrix.m[2]))));
}

_float3 CGlobalFunction_Manager::Get_ScaleFromMatrix(const _fmatrix _Matrix)
{
	return _float3(XMVectorGetX(XMVector3Length(_Matrix.r[0])),
				   XMVectorGetX(XMVector3Length(_Matrix.r[1])),
				   XMVectorGetX(XMVector3Length(_Matrix.r[2])));
}

_float CGlobalFunction_Manager::Get_Angle_Between_Vectors(_fvector _vNormal, _fvector _vVector1, _fvector _vVector2)
{
	_vector vVec1 = XMVector3Normalize(_vVector1);
	_vector vVec2 = XMVector3Normalize(_vVector2);
	_float fAngle = XMConvertToDegrees(acosf(XMVectorGetX(XMVector3Dot(vVec1, vVec2))));
	_float fResult = XMVectorGetZ(XMVector3Dot(_vNormal, XMVector3Cross(vVec1, vVec2)));
	if (0 > fResult)
	{
		fAngle = 360 - fAngle;
	}
	return fAngle;
}

_float CGlobalFunction_Manager::Clamp_Degrees(_float _fDegrees)
{
	_float fResult = _fDegrees;

	if (0.f > _fDegrees)
		fResult = _fDegrees + 360.f;

	else if (360.f <= _fDegrees)
		fResult = _fDegrees - 360.f;

	return fResult;
}

_uint CGlobalFunction_Manager::Compare_VectorLength(_fvector _vVector1, _fvector _vVector2)
{
	_float fLength1 = XMVectorGetX(XMVector3Length(_vVector1));
	_float fLength2 = XMVectorGetX(XMVector3Length(_vVector2));

	if (fLength1 == fLength2)
		return 0;
	else if (fLength1 > fLength2)
		return 1;
	else
		return 2;
}

_fvector CGlobalFunction_Manager::Rotate_Vector(_fvector _vAxis, _fvector _vVector, _float _fDegrees)
{
	_vector vQuaternion = XMQuaternionRotationAxis(_vAxis, XMConvertToRadians(_fDegrees));
	return XMVector3Rotate(_vVector, vQuaternion);
}

_float2 CGlobalFunction_Manager::Get_CursorPos(HWND hWnd)
{
	POINT ptCursorPos;
	GetCursorPos(&ptCursorPos);
	ScreenToClient(hWnd, &ptCursorPos);

	return _float2((_float)ptCursorPos.x, (_float)ptCursorPos.y);
}

string CGlobalFunction_Manager::WStringToString(const _wstring& _wstr)
{
	if (_wstr.empty()) return "";

	int iStrLen = WideCharToMultiByte(CP_UTF8, 0, _wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
	if (iStrLen <= 0) return "";

	std::string str(iStrLen, 0);
	WideCharToMultiByte(CP_UTF8, 0, _wstr.c_str(), -1, &str[0], iStrLen, nullptr, nullptr);

	// Null-terminated string에서 null 제거
	str.resize(iStrLen - 1);
	return str;


	/* 효쪽이 버전. */
	/*if (wstr.empty()) 
		return string();

	_int isize = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (_int)wstr.size(), nullptr, 0, nullptr, nullptr);
	string szResult(isize, 0);

	WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (_int)wstr.size(), &szResult[0], isize, nullptr, nullptr);

	return szResult;*/
}

_wstring CGlobalFunction_Manager::StringToWString(const string& _str)
{
	if (_str.empty()) return L"";

	int iWstrLen = MultiByteToWideChar(CP_UTF8, 0, _str.c_str(), -1, nullptr, 0);
	if (iWstrLen <= 0) return L"";

	std::wstring wstr(iWstrLen, 0);
	MultiByteToWideChar(CP_UTF8, 0, _str.c_str(), -1, &wstr[0], iWstrLen);

	// Null-terminated string에서 null 제거
	wstr.resize(iWstrLen - 1);
	return wstr;


	/* 효쪽이 버전 */
	/*if (str.empty())
		return _wstring();

	_int isize = MultiByteToWideChar(CP_UTF8, 0, &str[0], (_int)str.size(), nullptr, 0);
	_wstring wszResult(isize, 0);

	MultiByteToWideChar(CP_UTF8, 0, &str[0], (_int)str.size(), &wszResult[0], isize);

	return wszResult;*/
}

CGlobalFunction_Manager* CGlobalFunction_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	return new CGlobalFunction_Manager(pDevice, pContext);
}
void CGlobalFunction_Manager::Free()
{
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	__super::Free();
}
