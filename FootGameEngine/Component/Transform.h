#pragma once
#include "Object/ComponentBase.h"

#include "Define/FootEngineTypedef.h"

namespace GameEngineSpace
{
	class GameObject;
	class Rigidbody;

	enum class Space
	{
		WORLD,
		LOCAL
	};

	struct InterpolateData
	{
		Vector3 position;

		Quaternion rotation;

		float t;
	};

	/**
	 * \brief ��� ���� ������Ʈ�� �ݵ�� ������ �ִ� Ʈ������ ������Ʈ �Դϴ�.
	 *
	 * ������Ʈ�� ��ġ, ����, ������ ������ ������ �ֽ��ϴ�.
	 *
	 * 2022.07.22(��) Dev.BlackFoot
	 */
	class Transform : public ComponentBase
	{
		// ���� ��ġ ���� ������
		// Transform Matrix
			// Local TM == �θ��� �������� �θ� ���������� �ڱ� ��ġ�� �̵���Ű�� TM / LocalTM
			// World TM == ���� �������� World ��ġ�� ���� TM
		Matrix localTM;
		Matrix worldTM;

		Vector3 localPosition;
		Vector3 localEulerRotation;
		Quaternion localQuatRotation;
		Vector3 localScale;

		Vector3 worldPosition;
		Vector3 worldEulerRotation;
		Quaternion worldQuatRotation;
		Vector3 worldScale;

		Vector3 look;
		Vector3 up;
		Vector3 right;

		// TM ������Ʈ�� ���� ���� ���� �� �Լ�.
		// ���� �ڽ��� TM�� ���� ������ϴ°�.
		bool needTMUpdate;

		float mousePosX;
		float mousePosY;


	public:
		// interpolate�� �ָ� ���� ������ �ϱ����� transform ť
		std::queue<InterpolateData> interpolateQueue;
		
		// YAMLParser�κ��� ���� ID ����Ʈ
		std::vector<std::string> childIDList;

		bool isInterpolate = false;

	public:
		Transform(std::weak_ptr<GameObject> _gameObject);

		virtual ~Transform();

		virtual void Release() override;

		virtual void Update(float tick) override;

		// ������ ��ġ ������, ����� �Ǵ� ������ �������ݴϴ�.
		void SetPosition(const Vector3& _newPos, Space relativeTo = Space::WORLD);
		void SetRotation(const Quaternion& rotation, Space relativeTo = Space::WORLD);
		// Scale�� ��� ���带 ��������� �ʽ��ϴ�..
		void SetScale(const Vector3& _newLocalScale);

		void SetLocalEulerRotation(const Vector3& eulers);

		// ������ Private ���� ����.
		const Vector3& GetLocalPosition() { CheckAllUpdate(); return localPosition; }
		const Quaternion& GetLocalRotation() { CheckAllUpdate(); return localQuatRotation; }
		const Vector3& GetLocalEulerRotation() 
		{
			CheckAllUpdate();

			return localEulerRotation;
			/*return Vector3(XMConvertToRadians(localEulerRotation.x),
				XMConvertToRadians(localEulerRotation.y), XMConvertToRadians(localEulerRotation.z));*/
		};
		const Vector3& GetLocalScale() { CheckAllUpdate(); return localScale; }

		const Vector3& GetWorldPosition() { CheckAllUpdate(); return worldPosition; }
		const Quaternion& GetWorldRotation() { CheckAllUpdate(); return worldQuatRotation; }
		const Vector3& GetWorldScale() { CheckAllUpdate(); return worldScale; }

		void LookAt(std::shared_ptr<Transform> target, const Vector3& worldUp = Vector3::Up);

		void LookAt(const Vector3& targetPos, const Vector3& worldUp = Vector3::Up);

		const Vector3& GetLook() { CheckAllUpdate(); return look; }
		void SetLook(const Vector3& _look);
		const Vector3& GetUp() { CheckAllUpdate(); return up; }
		void SetUp(const Vector3& _up);
		const Vector3& GetRight() { CheckAllUpdate(); return right; }
		void SetRight(const Vector3& _right);

		const Matrix& GetLocalTM();
		const Matrix& GetWorldTM();

		void SetLocalTM(const Matrix& _localTM);
		void SetWorldTM(const Matrix& _worldTM);

		// ���� �������� ȸ��
		void Rotate(const Vector3& eulers, Space relativeTo = Space::LOCAL);

		void Rotate(const Vector3& axis, float angle, Space relativeTo = Space::LOCAL);

		void Rotate(const Quaternion& quat, Space relativeTo);

		void RotateAround(const Vector3& point, const Vector3& axis, float angle);

		// ������ǥ�� �ڱ��ڽ�(local)���� �̵��Ѵ�.
		void Translate(const Vector3& pos, Space relativeTo = Space::LOCAL);

		void CheckAllUpdate();

		// ���Ϸ��� ���ʹϾ�����~
		Vector4 EulerToQuat(Vector3 euler);

		Vector3 QuatToEuler(Quaternion value);

		Matrix CreateFromLookRightUp(const Vector3& look, const Vector3& right, const Vector3& up);

		void PushInterpolateQueue(Vector3 position, Quaternion rotation, float t);

	private:
		// ���� TM�� ����
		void UpdateLocalTM();
		// WorldTM�� ����
		void UpdateWorldTM();
		// needTM UPdate ���� True�� ����
		void CheckingNeedTMUpdate();

		// Decompose
		void DecomposeMatrix(Vector3& scale, Vector3& rotation, Vector3& position, Matrix matrix);

		void DecomposeLocalTM();

		void DecomposeWorldTM();
	};
}