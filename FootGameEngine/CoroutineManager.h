#pragma once

namespace GameEngineSpace
{
	class IEnumerator;

	class CoroutinManager final
	{
		// 단 하나 존재하는 SceneManager 객체
		static std::shared_ptr<CoroutinManager> instance;

	private:
		std::list<IEnumerator> coroutines;

	public:
		CoroutinManager() = default;

		~CoroutinManager() = default;

		// 이동, 복사 생성자 제거
		CoroutinManager(const CoroutinManager& _other) = delete;
		CoroutinManager& operator= (const CoroutinManager& _other) = delete;
		CoroutinManager(CoroutinManager&& _other) = delete;
		CoroutinManager& operator= (CoroutinManager&& _other) = delete;

		static std::shared_ptr<CoroutinManager> GetInstance();

	public:
		void StartCoroutine(IEnumerator coroutine);

		void StopCoroutine(IEnumerator coroutine);

		void Update(float tick);

		void Release();
	};
}

