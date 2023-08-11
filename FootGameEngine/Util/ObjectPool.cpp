#include "GamePch.h"
#include "ObjectPool.h"

namespace GameEngineSpace
{
	std::shared_ptr<ObjectPool> ObjectPool::instance = nullptr;

	std::shared_ptr<ObjectPool> ObjectPool::GetInstance()
	{
		// 인스턴스가 없으면 만들어주고 있으면 인스턴스 리턴
		if (instance == nullptr)
		{
			instance = std::make_shared<ObjectPool>();
		}

		return instance;
	}

	void ObjectPool::RegisterGameObject(std::string name, int size, std::function<std::shared_ptr<GameObject>()> gameObjectFunction)
	{
		std::vector<std::shared_ptr<GameObject>> objects(size);

		for (int i = 0; i < size; ++i)
		{
			objects[i] = gameObjectFunction();
		}

		pool[name] = objects;
	}

	std::shared_ptr<GameObject> ObjectPool::AcquireGameObject(std::string name)
	{
		if (pool.find(name) == pool.end())
		{
			return nullptr;
		}
		
		for (auto& obj : pool[name])
		{
			if (!obj->GetIsEnable())
			{
				obj->SetGameObjectEnable(true);
				obj->Start();

				return obj;
			}
		}

		return nullptr;
	}

	void ObjectPool::ReturnGameObject(std::shared_ptr<GameObject> gameObject)
	{
		gameObject->GetComponent<Transform>()->SetPosition({10000, 10000, 10000});
		gameObject->SetGameObjectEnable(false);
	}
		
	void ObjectPool::Release()
	{
		for (auto& objectList : pool)
		{
			for (auto& obj : objectList.second)
			{
				obj.reset();
			}

			objectList.second.clear();
		}

		pool.clear();
	}
}