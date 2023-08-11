#include "pch.h"
#include "EnemySpawn.h"

#include "../../FootGraphicsEngine/GraphicsEngineFramework.h"

#include "Component/SpriteEffect.h"
#include "GameManager.h"
#include "EnemyBehavior.h"

namespace ClientSpace
{
	EnemySpawn::EnemySpawn(std::weak_ptr<GameObject> _object)
		: ComponentBase(_object)
		, alpha(0.5f)
		, maskColor(0.8f, 0.4f, 0.9f, 1.0f)
		, goalScale(1.0f)
		, nowHeight(0.f)
		, goalHeight(2.9f)
		, eulerZ(0.f)
		, isScaleSet(false)
		, isEnd(false)
		, isPlay(false)
		, isLoading(false)
		, tickCount(0.f)
		, loadSpeed(2.5f)
		, isLoadCompleted(false)
		, hasScale(true)
	{
	}

	EnemySpawn::~EnemySpawn()
	{
	}

	void EnemySpawn::Init(SimpleMath::Vector3 pos, float goalScale)
	{
		// 각종 정보 캐싱 및 transform등 세팅.
		auto gameObj = GetGameObject();

		transform = gameObj->GetComponent<Transform>();
		gameManager = GameManager::GetInstance();

		// 추가
		spriteEffect = gameObj->AddComponent<GameEngineSpace::SpriteEffect>();
		spriteEffect->Init("SpawnEffect");
		spriteEffect->SetIsRender(false);

		maskColor.w = alpha;
		spriteEffect->SetMaskColor(maskColor);
		// Emissive 세팅
		spriteEffect->SetEmissiveColor({ 1.5f, 0.5f, 2.5f, 1.0f });

		// 자식을 가정하고 로컬로 잡는다.
		transform->SetPosition(pos, Space::LOCAL);
		transform->SetScale(Vector3{ 0.f, 0.f, 0.f });
		//transform->SetLocalEulerRotation(Vector3{ 90.f, 45.f, 0.f });

		this->goalScale = goalScale;

	}

	void EnemySpawn::Update(float tick)
	{
		// 테스트용.
		if (tick <= 0.f || isPlay == false)
			return;

		// 골 스케일이 될때 까지 회전하면서 스케일을 늘려줍니다.
		if (isScaleSet != true && hasScale == true)
		{
			float nowScale = transform->GetLocalScale().x;
			//auto nowRot = transform->GetLocalEulerRotation();

			alpha += 2.0f * tick;

			if (alpha > 1.0f)
				alpha = 1.f;

			nowScale += 0.5f * tick;
			if (nowScale >= goalScale)
			{
				nowScale = goalScale;
				alpha = 1.f;
				isScaleSet = true;
			}
			transform->SetScale(Vector3{ nowScale, nowScale, nowScale });

			eulerZ += 200.f * tick;

			maskColor.w = alpha;
			spriteEffect->SetMaskColor(maskColor);
			transform->SetLocalEulerRotation(Vector3{ 90.f, 45.f, eulerZ });
		}
		else if (isScaleSet != true && hasScale != true)
		{
			// 바로 넘겨줍니다.
			isScaleSet = true;
		}
		// 세팅이 되었으면.. 올려줍니다.
		if (isEnd != true && isScaleSet == true)
		{
			tickCount += tick;
			if (tickCount < 0.7f)
				return;
			else
			{
				if (isLoading != true)
					isLoading = true;

				auto nowPos = transform->GetLocalPosition();

				// PBRObj의 Offset과 동일하게 가줍니다.
				nowHeight += loadSpeed * tick;
				if (nowHeight > goalHeight)
				{
					nowHeight = goalHeight;
					isEnd = true;
					tickCount = 0.f;
				}
				transform->SetPosition(Vector3{ nowPos.x, nowPos.y + loadSpeed * tick, nowPos.z }, Space::LOCAL);
			}
		}

		if (isEnd == true && isScaleSet == true)
		{
			tickCount += tick;
			if (tickCount > 1.0f)
			{
				spriteEffect->SetIsRender(false);
				isPlay = false;
				isLoadCompleted = true;
				isLoaded = true;
			}
		}
	}

	void EnemySpawn::SetPlay(bool isPlay)
	{
		// play가 실행되면 렌더 가능하도록 해줍니다.
		this->isPlay = isPlay;
		spriteEffect->SetIsRender(isPlay);
	}

	void EnemySpawn::SetEmissiveColor(const SimpleMath::Vector4& color)
	{
		spriteEffect->SetEmissiveColor(color);
	}
}
