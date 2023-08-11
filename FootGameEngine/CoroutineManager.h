#pragma once

namespace GameEngineSpace
{
	class IEnumerator;

	class CoroutinManager final
	{
		// �� �ϳ� �����ϴ� SceneManager ��ü
		static std::shared_ptr<CoroutinManager> instance;

	private:
		std::list<IEnumerator> coroutines;

	public:
		CoroutinManager() = default;

		~CoroutinManager() = default;

		// �̵�, ���� ������ ����
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

