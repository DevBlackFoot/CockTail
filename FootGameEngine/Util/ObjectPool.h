#pragma once
#include "Object/GameObject.h"

namespace GameEngineSpace
{
	// ΩÃ±€≈Ê
	class ObjectPool
	{
	private:
		static std::shared_ptr<ObjectPool> instance;

	private:
		std::unordered_map<std::string, std::vector<std::shared_ptr<GameObject>>> pool;
	
	public:
		static std::shared_ptr<ObjectPool> GetInstance();

	public:
		ObjectPool() = default;

		~ObjectPool() = default;

		void RegisterGameObject(std::string name, int size, std::function<std::shared_ptr<GameObject>()> gameObjectFunction);

		std::shared_ptr<GameObject> AcquireGameObject(std::string name);

		void ReturnGameObject(std::shared_ptr<GameObject> gameObject);

		// æ¿¿Ã πŸ≤∂ß∏∂¥Ÿ √ ±‚»≠ «ÿ¡÷¿⁄
		void Release();
	};
}
