#pragma once

namespace GameEngineSpace
{
	class SpriteEffect;
}

namespace ClientSpace
{
	//class EnemyBehavior;

	/**
	 * \brief 적이 나타날 때 렌더되는 Spawn Effect를 담당하는 Component입니다.
	 */
	class EnemySpawn : public ComponentBase
	{
	private:
		std::shared_ptr<GameEngineSpace::SpriteEffect> spriteEffect;
		std::shared_ptr<GameManager> gameManager;
		std::shared_ptr<Transform> transform;

		//std::shared_ptr<EnemyBehavior> enemyBehavior;

		float alpha;
		// 마스크 컬러.
		SimpleMath::Vector4 maskColor;

		// 몬스터 크기에 따른 목표 스케일 값 지정.
			// 비율을 동등하게 늘린다고 가정합니다.
		float goalScale;
		float nowHeight;
		float eulerZ;
		// 목표 height
		float goalHeight;

		// 스케일이 일정하게 되었는가
		bool isScaleSet;
		// 위에 도달 하였는가.
		bool isEnd;

		// 이펙트를 시작하였는가.
		bool isPlay;
		// 로딩을 시작하는 가.
		bool isLoading;
		// 로딩 완료 여부를 알려주는 bool 변수
		bool isLoadCompleted;

		// 마법진이 있는가
		bool hasScale;

		float tickCount;
		float loadSpeed;

		bool isLoaded = false;

	public:
		EnemySpawn(std::weak_ptr<GameObject> _object);
		virtual ~EnemySpawn();

		// 포지션 값, 목표 스케일을 세팅하도록 합시다.
		void Init(SimpleMath::Vector3 pos, float goalScale);

		void Update(float tick) override;
		void SetPlay(bool isPlay);

		// 다른 컴포넌트에서 현재 로딩 중임을 알아볼 수 있도록 하는 isLoading 정보를 얻을 수 있도록 한다.
		bool GetIsLoading() { return isLoading; }
		void SetGoalScale(float goalScale) { this->goalScale = goalScale; }
		void SetGoalHeight(float goalHeight) { this->goalHeight = goalHeight; }
		float GetGoalHeight() { return goalHeight; }
		void SetHasScale(bool hasScale) { this->hasScale = hasScale; }
		bool GetIsLoadCompleted() { return isLoadCompleted; }
		void SetLoadCompleted() { isLoadCompleted = true; }
		bool GetIsLoaded() { return isLoaded; }

		void SetEmissiveColor(const SimpleMath::Vector4& color);
		void SetLoadSpeed(float speed) { this->loadSpeed = speed; }
		float GetLoadSpeed() { return this->loadSpeed; }
	};

}
