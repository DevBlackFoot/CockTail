#pragma once
#include "Object.h"

namespace GameEngineSpace
{
	class GameObject;

	// ������Ʈ Ÿ���� �����ص� EnumClass
		// ���ο� Ÿ���� ����� �� �� �� �� �߰��غ���..
	enum class ComponentType
	{
		TRANSFORM = 0,
		RIGIDBODY = 1,
		RENDERER = 2,
		NAVAGENT = 3,
		LIGHT = 4,
		CAMERA = 5,
		ANIMATOR = 6,
		BONE = 7,
		CANVAS = 8,
		TEXT = 9,
		COLLIDER = 10,
		MONOBEHAVIOR = 11,
		NONE = 12
	};

	/**
	 * \brief ��� ������Ʈ���� ����� �Ǵ� �θ� Ŭ����.
	 *
	 * 2022.07.22(��) Dev.BlackFoot
	 */
	class ComponentBase : public Object
	{
		// �ش� ������Ʈ�� ����Ű�� �ִ� ���� ������Ʈ.
			// ��ȯ������ ���ɼ��� �ֱ� ������ Weak point�� ����Ѵ�.
		std::weak_ptr<GameObject> gameObject;

		// ��ŸƮ�� ����Ǿ��°�..! => �⺻������ false.. ��ŸƮ�� ����Ǹ� true��.
		bool isStarted;

	public:
		ComponentType componentType;

		// YamlParser���� �޾������ ID �ο�
		std::string gameObjectID;
		std::string componentID;

	public:
		ComponentBase(std::weak_ptr<GameObject> _gameObject, ComponentType _componentType = ComponentType::NONE);

		virtual ~ComponentBase();

		virtual void Release() override;

		std::shared_ptr<GameObject> GetGameObject() { return gameObject.lock(); }

		bool GetIsStarted() { return isStarted; }
		void SetIsStarted(bool _isStarted) { isStarted = _isStarted; }
	};

	// ��ȣ�� ���� ���۷����� �����ε�
		// Inner class�� ��� ������ ������ �ֱ� ������ �Ʒ��� �ۼ�
	struct ComponentOperator
	{
		// ������Ʈ Ÿ�Կ� ���� ���� �񱳿� ���۷�����..!
			// second ���ں��� first ���ڰ� �켱������ ������(���ڰ� ������ == enumClass ������ ���� ��ġ�� ������)
			// true�� ��ȯ�մϴ�.
		bool operator() (std::shared_ptr<ComponentBase> first, std::shared_ptr<ComponentBase> second)
		{
			return static_cast<uint32>(first->componentType) < static_cast<uint32>(second->componentType);
		}

	};

}