#include "Engine_Function.h"

XMVECTOR Engine::ExtractQuaternionFromMatrix(const XMMATRIX& _Matrix)
{
	// 상위 3x3 부분을 회전 행렬로 사용
	XMMATRIX rotationMatrix = XMMatrixRotationMatrix(M);

	// 3x3 회전 행렬에서 쿼터니언을 추출
	XMVECTOR quaternion = XMQuaternionRotationMatrix(rotationMatrix);

	return quaternion;
}
