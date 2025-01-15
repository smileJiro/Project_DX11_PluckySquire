#include "Engine_Function.h"

XMVECTOR Engine::ExtractQuaternionFromMatrix(const XMMATRIX& _Matrix)
{
	// 상위 3x3 부분을 회전 행렬로 사용
	XMMATRIX rotationMatrix = XMMatrixRotationMatrix(M);

	// 3x3 회전 행렬에서 쿼터니언을 추출
	XMVECTOR quaternion = XMQuaternionRotationMatrix(rotationMatrix);

	return quaternion;
}
string Engine::MatrixToString(_float4x4 vMatrix)
{
	string strMatrix;
	strMatrix += std::to_string(vMatrix._11) + " " + std::to_string(vMatrix._12) + " " + std::to_string(vMatrix._13) + " " + std::to_string(vMatrix._14) + "\n";
	strMatrix += std::to_string(vMatrix._21) + " " + std::to_string(vMatrix._22) + " " + std::to_string(vMatrix._23) + " " + std::to_string(vMatrix._24) + "\n";
	strMatrix += std::to_string(vMatrix._31) + " " + std::to_string(vMatrix._32) + " " + std::to_string(vMatrix._33) + " " + std::to_string(vMatrix._34) + "\n";
	strMatrix += std::to_string(vMatrix._41) + " " + std::to_string(vMatrix._42) + " " + std::to_string(vMatrix._43) + " " + std::to_string(vMatrix._44) + "\n";
	return strMatrix;
}