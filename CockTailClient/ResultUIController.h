#pragma once

namespace ClientSpace
{

	class ResultUIController : public ComponentBase
	{
		// 현재 상태.
		enum class CountState
		{
			// 숫자를 세는 상태
				// 연출을 재생합니다. (숫자 감소, 변경과 같은 식)
			NUM_COUNT,
			// 숫자가 끝난 상태. 따로 연출이 없습니다.
				// 특정 키를 통해 이곳으로 스킵 할 수 있게 합니다.
			COUNT_END,
			// 씬 나가기 전에 아이템 사라지는 연출.
				// 일단 넣어줍니다.
			CLEAR_ITEM,
			// 여기도 스킵 가능하게 해줍니다.
			CLEAR_END,
			NONE
		};

		// 아이템의 갯수가 명확하게 정해져 있기 때문에 배열을 사용해 줍니다.
		// 미리 계산해서 넣어둘 배열
		int craftItems[7];
		// 나머지 배열 -> 해당 부분까지 애니메이션을 넣어주기 위함.
			// 크래프트를 하고 남은 것.
		int modItems[14];
		// 현재 정보(시작점을 겸합니다.)
		int nowCraftItems[7];
		int nowIngredients[14];

		CountState nowState;

		// 게임 매니저 캐싱
		std::shared_ptr<GameManager> gameManager;
		std::shared_ptr<InputManager> input;

		std::shared_ptr<Canvas> resultCanvas;
		
		// 실제로 값들을 바꿔주기 위한 UI 캐싱
		std::shared_ptr<ButtonUI> exitButton;
		std::shared_ptr<TextUI> countText[21];

		// 인벤토리를 가져옵니다.
			// 씬을 벗어날 때 인벤토리를 갱신해줍니다.
			// 여기서 사용하는 데이터는 인벤토리 데이터가 아닙니다.
		std::unordered_map<int, std::shared_ptr<InventoryItem>> inventory;

		// 틱에 곱해줘서 카운팅 해줍니다.
		float countSpeed;
		// 카운트가 일정 이상일 때 SetText를 갱신해주는 방식으로 진행합니다.
		float tickCount;
		// 목표 카운트(애들마다 다르게 세팅해줍니다.)
		float goalCount;

		// 상태 변경 체크
		bool firstChange;

		bool ingredEnd;
		

	public:
		ResultUIController(std::weak_ptr<GameObject> obj);
		virtual ~ResultUIController();

		virtual void Awake() override;
		virtual void Start() override;

		virtual void Update(float tick) override;
		virtual void LateUpdate(float tick) override;

	private:
		// 처음 진입하면 공식에 맞게 크래프트를 다 해둡니다.
		void CraftAllItem();
		// 재료 아이템 두 개를 넣어서 얼마나 크래프트 되는지를 리턴합니다.
		int CraftItem(int ingred1, int ingred2);

		// 각 상태마다 업데이트 공식을 만듭니다.
		void CountUpdate(float tick);
		void CountEndUpdate(float tick);
		void ClearItemUpdate(float tick);
		void ClearEndUpdate(float tick);
	};

}
