#pragma once

namespace ClientSpace
{
	class PlayerController;

	// Room4에서 적들을 웨이브로 소환하는 컴포넌트입니다.
	class EnemyWaveSpawn : public ComponentBase
	{
	private:
		// 정보를 얻어올 게임 매니저
		std::shared_ptr<GameManager> gameManager;

		std::shared_ptr<PlayerController> playerController;

		// 현재 웨이브 인덱스
		int waveIdx;
		// 현재의 킬 카운트 => 게임 매니저에서 얻어옵니다.
		// 웨이브의 목표 킬 카운트 => 적의 개수
		int waveKillGoal;
		// 웨이브가 돌 때 마다 킬 카운트를 초기화 시켜줍니다.

		// Spawn Event를 보기 위한 bool 변수
			// 한 번만 실행되어야한다. (false 일 때, 몬스터를 소환해주는 느낌으로)
		bool wave1Play;
		bool wave2Play;
		bool wave3Play;

		// 내부적으로 분류를 해둡시다.
			// ID, TYPE(1 - Melee, 2 - Range, 3 - Toad), PosX, PosY, PosZ, RotY
		std::vector<std::vector<int>> wave1EnemyData;
		std::vector<std::vector<int>> wave2EnemyData;
		std::vector<std::vector<int>> wave3EnemyData;

		std::shared_ptr<SceneManager> sceneManager;

		// 재우 - waveStart 
		bool isWaveStart;

	public:
		EnemyWaveSpawn(std::weak_ptr<GameObject> _object);
		virtual ~EnemyWaveSpawn();

		// 초창기 세팅..
			// 각종 세팅 및 정보 가져오기
		void Init();

		// 빌드씬 부분에서 몬스터들 다 소환해두기
			// 첫 번째 웨이브를 제외한 몬스터들 다 SetEnable False로 해둡니다.
		void SetAllEnemy();

		// 현재의 웨이브 인덱스를 체크하면서 로딩이 안되었으면 로딩 시켜주는 것.
		void Update(float tick) override;

		// Wave시작 상태를 바꾸는 함수
		void SetIsWaveStart(bool val) { isWaveStart = val; }

		void SetPlayerController(std::shared_ptr<PlayerController> val) { playerController = val; }
	};

}
