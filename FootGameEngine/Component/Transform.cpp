#include "GamePch.h"
#include "Transform.h"

#include "Object/GameObject.h"
// #include "Component/Camera.h"

namespace GameEngineSpace
{
	Transform::Transform(std::weak_ptr<GameObject> _gameObject) :
		ComponentBase(_gameObject, ComponentType::TRANSFORM)
		, localTM(Matrix::Identity)
		, worldTM(Matrix::Identity)
		, localPosition(Vector3::Zero)
		, localQuatRotation(Quaternion::Identity)
		, localEulerRotation(Vector3::Zero)
		, localScale(Vector3(1.0f, 1.0f, 1.0f))
		, worldPosition(Vector3::Zero)
		, worldQuatRotation(Quaternion::Identity)
		, worldEulerRotation(Vector3::Zero)
		, worldScale(Vector3(1.0f, 1.0f, 1.0f))
		, look(Vector3::UnitZ)
		, up(Vector3::UnitY)
		, right(Vector3::UnitX)
		, needTMUpdate(true)
	{
	}

	Transform::~Transform()
	{
	}

	void Transform::Release()
	{
		__super::Release();
	}

	void Transform::Update(float tick)
	{
		
	}

	void Transform::UpdateLocalTM()
	{
		localTM = Matrix::CreateScale(localScale) * Matrix::CreateFromQuaternion(localQuatRotation) * Matrix::CreateTranslation(localPosition);
	}

	void Transform::UpdateWorldTM()
	{
		// 부모를 얻어와서.. 부모의 월드에 자신의 로컬을 곱해준다.
		std::shared_ptr<GameObject> _parent = GetGameObject()->GetParent();

		if (_parent != nullptr)
		{
			worldTM = localTM * (_parent->GetComponent<Transform>()->GetWorldTM());
		}
		else
		{
			worldTM = localTM;
		}
	}

	void Transform::CheckingNeedTMUpdate()
	{
		needTMUpdate = true;

		std::vector<std::shared_ptr<GameObject>>& children = this->GetGameObject()->GetChildren();

		// 모든 자식의 트랜스폼에 TM Update가 필요하도록!
		for (auto& child : children)
		{
			child->GetComponent<Transform>()->CheckingNeedTMUpdate();
		}
	}

	void Transform::LookAt(std::shared_ptr<Transform> target, const Vector3& worldUp)
	{
		LookAt(target->GetWorldPosition(), worldUp);
	}

	void Transform::LookAt(const Vector3& targetPos, const Vector3& worldUp)
	{
		if (targetPos == worldPosition)
			return;

		Matrix lookAtMatrix = Matrix::CreateLookAt(worldPosition, targetPos, Vector3::Up);

		// 회전만 시켜주기 위해서 회전 성분 빼낸다.
		Quaternion lookAtQuat = Quaternion::CreateFromRotationMatrix(lookAtMatrix);

		lookAtQuat.Inverse(lookAtQuat);

		worldQuatRotation = Quaternion::Identity;

		// 회전 성분 업데이트
		SetRotation(lookAtQuat, Space::WORLD);
	}

	void Transform::SetLook(const Vector3& _look)
	{
		// Look이 바뀌었으니 나머지도 바꿔주자.
		look = _look;
		// 왼손잡이로 변경
		look.Normalize(look);

		// look과 월드 업을 이용해서 right를 뽑아준다.
		Vector3::UnitY.Cross(look, right); // right = UnitY X look
		//look.Cross(Vector3::UnitY, right);
		//right = -right;
		right.Normalize(right);


		look.Cross(right, up);
		//right.Cross(look, up);
		//up = -up;
		up.Normalize(up);

		Matrix matrix = Matrix{ right, up, look };
		matrix.Transpose(matrix);

		right = Vector3{ matrix.m[0][0], matrix.m[1][0], matrix.m[2][0] };
		up = -Vector3{ matrix.m[0][1], matrix.m[1][1], matrix.m[2][1] };
		look = Vector3{ matrix.m[0][2], matrix.m[1][2], matrix.m[2][2] };

		// 각 벡터 정규화
		right.Normalize(right);
		up.Normalize(up);
		look.Normalize(look);

		matrix.Invert(matrix);
		Vector3 rotation = matrix.ToEuler();
		auto quat = EulerToQuat(rotation);

		rotation.x = XMConvertToDegrees(rotation.x);
		rotation.y = XMConvertToDegrees(rotation.y);
		rotation.z = XMConvertToDegrees(rotation.z);

		SetLocalEulerRotation(rotation);
	}

	// 현재 코드에 문제가 있습니다..
		// Up 벡터와 Right벡터로 나머지 기저 벡터를 구하는 것은 무리가 있는가..
	void Transform::SetUp(const Vector3& _up)
	{
		/*up = _up;
		up.Normalize(up);

		up.Cross(Vector3::UnitZ, right);
		right.Normalize(right);

		right.Cross(up, look);
		look.Normalize(look);*/
	}

	void Transform::SetRight(const Vector3& _right)
	{
		/*right = _right;
		right.Normalize(right);

		Vector3::UnitZ.Cross(right, up);
		up.Normalize(up);

		right.Cross(up, look);
		look.Normalize(look);*/
	}

	void Transform::DecomposeMatrix(Vector3& scale, Vector3& rotation, Vector3& position, Matrix matrix)
	{
		// Position, Scale
		position.x = matrix.m[3][0];
		position.y = matrix.m[3][1];
		position.z = matrix.m[3][2];

		scale.x = sqrtf(powf(matrix.m[0][0], 2.0f) + powf(matrix.m[0][1], 2.0f) + powf(matrix.m[0][2], 2.0f));
		scale.y = sqrtf(powf(matrix.m[1][0], 2.0f) + powf(matrix.m[1][1], 2.0f) + powf(matrix.m[1][2], 2.0f));
		scale.z = sqrtf(powf(matrix.m[2][0], 2.0f) + powf(matrix.m[2][1], 2.0f) + powf(matrix.m[2][2], 2.0f));

		Vector3 matrix0 = Vector3{ matrix.m[0] };
		Vector3 matrix1 = Vector3{ matrix.m[1] };
		Vector3 matrix2 = Vector3{ matrix.m[2] };

		Vector3 def;
		matrix0.Cross(matrix1, def);

		// Negative Scale
		if (def.Dot(matrix2) < 0.0f)
		{
			scale *= -1.0f;
		}

		// Rotation
		matrix0 /= scale.x;
		matrix1 /= scale.y;
		matrix2 /= scale.z;

		Matrix rotMatrix = Matrix{matrix0, matrix1, matrix2};

		rotation = rotMatrix.ToEuler();

		rotation.x = XMConvertToDegrees(rotation.x);
		rotation.y = XMConvertToDegrees(rotation.y);
		rotation.z = XMConvertToDegrees(rotation.z);
	}

	void Transform::DecomposeLocalTM()
	{
		localTM.Decompose(localScale, localQuatRotation, localPosition);

		localEulerRotation = QuatToEuler(localQuatRotation);
	}

	void Transform::DecomposeWorldTM()
	{
		worldTM.Decompose(worldScale, worldQuatRotation, worldPosition);

		worldEulerRotation = QuatToEuler(worldQuatRotation);
	}

	void Transform::SetPosition(const Vector3& _newPos, Space relativeTo)
	{
		std::shared_ptr<GameObject> _parent = GetGameObject()->GetParent();

		if (relativeTo == Space::WORLD)
		{
			// 월드 자체의 포지션을 변경한 경우
			worldPosition = _newPos;

			Vector3 _parentPos = Vector3::Zero;

			Quaternion _parentRot = Quaternion::Identity;

			// 부모가 있으면
			if (_parent != nullptr)
			{
				_parentPos = _parent->GetComponent<Transform>()->GetWorldPosition();

				_parentRot = _parent->GetComponent<Transform>()->GetWorldRotation();
				
				_parentRot.Inverse(_parentRot);
			}

			// 부모가 있을 때, 로컬 포지션을 어떤 기준으로 정할 것인가 ?
			// 부모가 있을 때의 로컬 포지션은, 부모의 회전이 0일 때의 값을 로컬 포지션으로 정의한다.
			// 현재 월드의 위치에서 부모의 월드 위치를 빼준게, 자신의 로컬 위치가 된다.
			Vector3 _newLocalPos = Vector3::Transform(_newPos - _parentPos, _parentRot);

			localPosition = _newLocalPos;
		}
		else
		{
			localPosition = _newPos;

			Vector3 parentPosition = Vector3::Zero;
			
			if (_parent != nullptr)
			{
				parentPosition = _parent->GetComponent<Transform>()->GetWorldPosition();
			}

			worldPosition = parentPosition + _newPos;
		}

		CheckingNeedTMUpdate();
	}

	void Transform::SetRotation(const Quaternion& rotation, Space relativeTo /*= Space::WORLD*/)
	{
		std::shared_ptr<GameObject> _parent = GetGameObject()->GetParent();

		if (relativeTo == Space::WORLD)
		{
			worldQuatRotation = rotation;

			Quaternion parentRotationInv = Quaternion::Identity;

			if (_parent != nullptr)
				_parent->GetComponent<Transform>()->GetWorldRotation().Inverse(parentRotationInv);

			const Quaternion newLocalRotation = worldQuatRotation * parentRotationInv;

			localQuatRotation = newLocalRotation;
		}
		else
		{
			localQuatRotation = rotation;

			Quaternion parentRotation = Quaternion::Identity;

			if (_parent != nullptr)
				parentRotation = _parent->GetComponent<Transform>()->GetWorldRotation();

			const Quaternion newWorldRotation = localQuatRotation * parentRotation;

			worldQuatRotation = newWorldRotation;
		}

		localEulerRotation = QuatToEuler(localQuatRotation);
				  
		worldEulerRotation = QuatToEuler(worldQuatRotation);

		CheckingNeedTMUpdate();
	}

	void Transform::SetScale(const Vector3& _newLocalScale)
	{
		localScale = _newLocalScale;

		CheckingNeedTMUpdate();

		this->UpdateLocalTM();

		this->UpdateWorldTM();

		look = Vector3::TransformNormal(Vector3::UnitZ, worldTM);

		right = Vector3::TransformNormal(Vector3::UnitX, worldTM);

		up = Vector3::TransformNormal(Vector3::UnitY, worldTM);

		DecomposeLocalTM();

		DecomposeWorldTM();

		CheckingNeedTMUpdate();
	}

	void Transform::SetLocalEulerRotation(const Vector3& eulers)
	{
		const Vector3 radianEulers = Vector3(XMConvertToRadians(eulers.x), XMConvertToRadians(eulers.y),
			XMConvertToRadians(eulers.z));

		Quaternion rot = Quaternion::Identity;

		rot = Quaternion::CreateFromYawPitchRoll(radianEulers.y, radianEulers.x, radianEulers.z);

		SetRotation(rot, Space::LOCAL);
	}

	const Matrix& Transform::GetLocalTM()
	{
		CheckAllUpdate();

		return localTM;
	}

	const Matrix& Transform::GetWorldTM()
	{
		CheckAllUpdate();

		return worldTM;
	}

	void Transform::SetLocalTM(const Matrix& _localTM)
	{
		// Local Property
		localTM = _localTM;

		DecomposeLocalTM();

		std::shared_ptr<GameObject> _parent = GetGameObject()->GetParent();

		// World Property
		if (_parent != nullptr)
			worldTM = localTM * _parent->GetComponent<Transform>()->GetWorldTM();
		else
			worldTM = localTM;

		DecomposeWorldTM();

		CheckingNeedTMUpdate();
	}

	void Transform::SetWorldTM(const Matrix& _worldTM)
	{
		// World Property
		worldTM = _worldTM;

		DecomposeWorldTM();

		std::shared_ptr<GameObject> _parent = GetGameObject()->GetParent();

		if (_parent != nullptr)
			localTM = worldTM * _parent->GetComponent<Transform>()->GetWorldTM().Invert();
		else
			localTM = worldTM;

		DecomposeLocalTM();

		CheckingNeedTMUpdate();
	}

	void Transform::Rotate(const Vector3& eulers, Space relativeTo)
	{
		CheckAllUpdate();

		const Vector3 radianEulers = Vector3(XMConvertToRadians(eulers.x), XMConvertToRadians(eulers.y),
			XMConvertToRadians(eulers.z));

		Quaternion deltaRot = Quaternion::Identity;

		if (relativeTo == Space::WORLD)
			deltaRot = Quaternion::CreateFromYawPitchRoll(radianEulers.y, radianEulers.x, radianEulers.z);
		else if (relativeTo == Space::LOCAL)
			deltaRot = Quaternion::CreateFromAxisAngle(look, radianEulers.z) *
			Quaternion::CreateFromAxisAngle(right, radianEulers.x) * Quaternion::CreateFromAxisAngle(up, radianEulers.y);

		SetRotation(deltaRot, relativeTo);
	}

	void Transform::Rotate(const Vector3& axis, float angle, Space relativeTo)
	{
		CheckAllUpdate(); 
		
		const float radian = XMConvertToRadians(angle);

		Quaternion deltaRot = Quaternion::Identity;

		if (relativeTo == Space::WORLD)
		{
			deltaRot = Quaternion::CreateFromAxisAngle(axis, radian);
		}
		else if (relativeTo == Space::LOCAL)
		{
			// Local Coordinate에 맞게 조절. 현재 회전에 대해서 axis를 돌려준다.
			const Vector3 localAxis =
				Vector3::TransformNormal(axis, CreateFromLookRightUp(look, right, up));

			deltaRot = Quaternion::CreateFromAxisAngle(localAxis, radian);
		}

		SetRotation(deltaRot, relativeTo);
	}

	void Transform::Rotate(const Quaternion& quat, Space relativeTo)
	{
		CheckAllUpdate();

		SetRotation(quat, relativeTo);
	}

	void Transform::RotateAround(const Vector3& point, const Vector3& axis, float angle)
	{
		CheckAllUpdate();

		// Point를 부모로 취하고 포지션을 변환시킨다.
		const Vector3 diff = (worldPosition - point);

		// degree to radian.
		const float radian = XMConvertToRadians(angle);

		const Matrix rotateAroundMat = Matrix::CreateTranslation(diff) * Matrix::CreateFromAxisAngle(axis, radian)
			* Matrix::CreateTranslation(Vector3::Transform(-diff, Matrix::CreateFromAxisAngle(axis, radian)));

		const Matrix newWorldTM = worldTM * rotateAroundMat;

		worldTM = newWorldTM;

		std::shared_ptr<GameObject> _parent = GetGameObject()->GetParent();

		if (_parent != nullptr)
			localTM = worldTM * (_parent->GetComponent<Transform>()->GetWorldTM().Invert());
		else
			localTM = worldTM;

		CheckingNeedTMUpdate();
	}

	void Transform::Translate(const Vector3& pos, Space relativeTo)
	{
		CheckAllUpdate();

		if (relativeTo == Space::LOCAL)
		{
			const Matrix& localRot = Matrix::CreateFromQuaternion(localQuatRotation);

			const Vector3 movement = Vector3::Transform(pos, localRot);

			worldPosition = worldPosition + movement;
		}
		else
		{
			worldPosition = worldPosition + pos;
		}

		std::shared_ptr<GameObject> _parent = GetGameObject()->GetParent();

		if (_parent != nullptr)
		{
			const Quaternion& parentRot = _parent->GetComponent<Transform>()->GetWorldRotation();

			const Vector3& parentPos = _parent->GetComponent<Transform>()->GetWorldPosition();

			const Matrix parentWorldTM = Matrix::CreateFromQuaternion(parentRot) * Matrix::CreateTranslation(parentPos);

			Matrix parentWorldTMInv;

			parentWorldTM.Invert(parentWorldTMInv);

			// 로컬 위치는 월드 위치에서 부모 변환의 역변환을 취하면 된다. (스케일 제외)
			localPosition = Vector3::Transform(worldPosition, parentWorldTMInv);
		}
		else
		{
			localPosition = worldPosition;
		}

		CheckingNeedTMUpdate();
	}

	void Transform::CheckAllUpdate()
	{
		if (needTMUpdate)
		{
			this->UpdateLocalTM();

			this->UpdateWorldTM();

			look = Vector3::TransformNormal(Vector3::UnitZ, worldTM);

			right = Vector3::TransformNormal(Vector3::UnitX, worldTM);

			up = Vector3::TransformNormal(Vector3::UnitY, worldTM);

			DecomposeLocalTM();

			DecomposeWorldTM();
			
			needTMUpdate = false;
		}
	}

	Vector4 Transform::EulerToQuat(Vector3 euler)
	{
		Vector4 result;

		float x = XMConvertToRadians(euler.x) * 0.5f;
		float y = XMConvertToRadians(euler.y) * 0.5f;
		float z = XMConvertToRadians(euler.z) * 0.5f;

		float sinX = sinf(x);
		float cosX = cosf(x);
		float sinY = sinf(y);
		float cosY = cosf(y);
		float sinZ = sinf(z);
		float cosZ = cosf(z);

		result.x = (cosY * sinX * cosZ) + (sinY * cosX * sinZ);
		result.y = (sinY * cosX * cosZ) - (cosY * sinX * sinZ);
		result.z = (cosY * cosX * sinZ) - (sinY * sinX * cosZ);
		result.w = (cosY * cosX * cosZ) + (sinY * sinX * sinZ);

		return result;
	}

	Vector3 Transform::QuatToEuler(Quaternion value)
	{
		Vector3 rotation;

		double x = value.x;
		double y = value.y;
		double z = value.z;
		double w = value.w;
		double sqx = x * x;
		double sqy = y * y;
		double sqz = z * z;
		double sqw = w * w;
		float exceptCheck = 2.0f * (w * x - y * z);
		float eulerX = 0.f;

		if (abs(exceptCheck) >= 1.f)
			eulerX = copysign(3.141592f / 2, exceptCheck);
		else
			eulerX = asin(2.0f * (w * x - y * z));

		float eulerY = atan2(2.0f * (x * z + w * y), (-sqx - sqy + sqz + sqw));
		float eulerZ = atan2(2.0f * (x * y + z * w), (-sqx + sqy - sqz + sqw));
		rotation = Vector3{ eulerX, eulerY, eulerZ };

		rotation = Vector3{ XMConvertToDegrees(rotation.x),
					   XMConvertToDegrees(rotation.y),
					   XMConvertToDegrees(rotation.z) };

		return rotation;
	}

	Matrix Transform::CreateFromLookRightUp(const Vector3& look, const Vector3& right, const Vector3& up)
	{
		Matrix R;

		R._11 = right.x;    R._12 = right.y;    R._13 = right.z;    R._14 = 0.f;
		R._21 = up.x;       R._22 = up.y;       R._23 = up.z;       R._24 = 0.f;
		R._31 = look.x;     R._32 = look.y;     R._33 = look.z;     R._34 = 0.f;
		R._41 = 0.f;        R._42 = 0.f;        R._43 = 0.f;        R._44 = 1.f;

		return R;
	}

	void Transform::PushInterpolateQueue(Vector3 position, Quaternion rotation, float t)
	{
		InterpolateData interpolateData;
		interpolateData.position = position;
		interpolateData.rotation = rotation;
		interpolateData.t = t;

		interpolateQueue.push(interpolateData);
	}
}