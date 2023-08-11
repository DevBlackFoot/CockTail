#pragma once


namespace ClientSpace
{
	// 바의 핵심 루프와 로직을 담당하는 BarCoreController입니다.
	class BarCoreController : public ComponentBase
	{
		// 외부에서 사용하지 않는 순서 enum
		enum class BarState
		{
			// 최초 진입, 튜토리얼을 보여줍니다.
			TUTORIAL,
			// Start, awake에서 지정한 손님을 순서로 smalltalk ~ order 직전까지의 상태입니다.
			INTRO,
			// 주문을 한 상태입니다.
			ORDER,
			// 미니게임 상태입니다.
			MINI_GAME,
			// 미니 게임을 마치고 제공하는 단계입니다.
			OFFER,
			// 제공 이후 조건에 따른 인사가 나옵니다. + bye인사 까지 담당합니다.
			BYE,
			// 모든 손님이 끝나고 문을 닫는 상황입니다.
			CLOSE,
			// 아무것도 아닌 디폴트 상태입니다.
			NONE
		};
		// 모든 상황은 순차적으로 진행됩니다.
			// tuto -> set_customer, intro, order, mini_game, offer, bye (3번 순회) -> close, none 순으로 진행됩니다.
			// 각각의 상황을 적절히 조절하여야합니다.

		// 레시피 상태입니다..
		enum class RecipeState
		{
			RUM,
			VODKA,
			GIN,
			NONE
		};

		// 결과! 스테이트
		enum class ResultState
		{
			GOOD,
			BAD,
			BEER,
			BYE,		// 작별인사를 위함.
			NONE
		};

		// 손님에 대한 순서 배열.
			// 내부에 손님 맵에 대한 정보를 가집니다. 0, 1, 2를 적절히 섞고
			// * 2 + (0 or 1)을 해서 결정합니다.
		int guest[3];
		// 현재 게스트 인덱스.
			// 0, 1, 2로 오릅니다.
		int nowGuestIdx;
		int nowCustomerIdx;
		// 현재 Customer 정보를 가져와줍니다.
		std::shared_ptr<Customer> nowCustomer;
		// 현재 order 정보를 가져와줍니다.
		std::shared_ptr<CockTailList> nowOrder;
		// 현재 칵테일 정보를 가져와줍니다.
		std::shared_ptr<CockTailStruct> nowCocktail;
		// 현재 상태를 지정합니다.
		BarState nowState;
		RecipeState nowRecipeState;
		ResultState resultState;

		// 초기화 등을 담당할 캔버스들을 세팅해줍니다.
		std::shared_ptr<Canvas> barCanvas;
		// 미니 게임 캔버스
		std::shared_ptr<Canvas> shakeCanvas;
		// 결과 캔버스
		std::shared_ptr<Canvas> resultCanvas;

		// 기본적으로 Item Data를 가져와서 사용해줍니다.
		std::shared_ptr<GameManager> gameManager;
		std::shared_ptr<InputManager> input;
		std::shared_ptr<ResourceManager> resourceManager;
		std::shared_ptr<SoundManager> sound;
		// 우리의 인벤토리는 주조 아이템과 재료 아이템을 모두 가지고 있습니다..
		std::unordered_map<int, std::shared_ptr<InventoryItem>> inventoryMap;
		// 칵테일 정보는 가져옵시다..
		std::unordered_map<int, std::shared_ptr<CockTailStruct>> cockTailMap;

		// 클릭을 막는 용도. => 대화를 넘어가지 않게 하기 위함.
		bool blockClick;

		// 사실 미니게임은 미니게임 대로 컴포넌트를 나눠서 진행하는 게 나을 것 같지만..
		float angle;
		const float startAngle;
		const float angleTheta;
		const int maxIterations;
		const float radius;
		Vector3 uiPos[3];
		Vector3 uiCirclePos[3];

		// 미니게임 HP
		int maxHP;
			// 체력 깎였을 때 실제로 깎일 체력
		int currentHP;
			// 깎이기 전, 목표를 할 체력 (오프셋이 깎이고 -> currentHP에 도달하면 같게 해줍니다.)
		int prevHP;
		int miniGameCount;
		int correctCount;
		bool isCorrectCockTail;
		bool isTurning;
			// 깎일 때 사용할 offset
		float hpOffset;

		// 레시피 애니메이션 관련 변수
		// lerp를 먹여줄 offset. 1이 되면 애니메이션을 종료해줍니다.
		float recipeOffset;
		// 애니메이션 속도
		float animSpeed;
		// 레시피가 애니메이팅 시작 했는가.
			// 스타트 지점에서 끝지점으로 러프만 해주는 식으로.
		bool isAnimating;
		// 레시피가 세팅이 되어 있는가.
			// 버튼을 누르면 상태에 따라 (켜져 있으면, 꺼주고 / 꺼져 있으면 켜준다.)
		bool isRecipeSet;
		Vector3 startSpritePos;
		Vector3 startButtonPos;
		Vector3 endSpritePos;
		Vector3 endButtonPos;
		// 기본적으로 레시피에 사용해줄 가격정보
		std::wstring priceText;

		// 조합 관련.
		int select[3];
		// 버튼 클릭할 때 마다 하나씩 올립니다.
		int selectIdx;

		int dialogIdx;

		// 튜토리얼을 띄우는 화면.
		int tutorialIdx;
		int prevTutorialIdx;
		// 처음 상태가 바뀔 때
		bool changeFirst;
		bool setMix;
		// 게스트 들어오는 속도를 위한 tick
		float guestTick;
		bool guestIn;

		bool miniGameSuccess = false;

		// 현재 선택한 버튼이 레시피에 맞는 버튼인지.
		int checkIngredient[24];

		// 핵심적으로 껐다 켰다 관리해줄..
		// 술 버튼들
		std::vector<std::shared_ptr<ButtonUI>> stuffButtons;
		// 믹스 리셋 비어
		std::shared_ptr<ButtonUI> mixButton;
		std::shared_ptr<ButtonUI> resetButton;
		std::shared_ptr<ButtonUI> beerButton;
		std::shared_ptr<ButtonUI> closeButton;
		// 선택 관련 텍스트들
		std::vector<std::shared_ptr<TextUI>> selectText;
		std::vector<std::shared_ptr<SpriteUI>> selectSprite;
		// 레시피 관련 버튼 (호버 체크 버튼)
		std::vector<std::shared_ptr<ButtonUI>> recipeHoverButtons;
		std::vector<std::shared_ptr<ButtonUI>> recipeTitleButton;
		std::shared_ptr<ButtonUI> recipeButton;
		//레시피 백그라운드
		std::shared_ptr<SpriteUI> recipeBackGround;

		std::vector<std::shared_ptr<SpriteUI>> recipeSprite;
		std::shared_ptr<SpriteUI> infoBack;
		std::shared_ptr<TextUI> infoText;

		// 미니 게임 이미지 캐싱
			// 왼쪽에 뜨는 선택
		std::vector<std::shared_ptr<SpriteUI>> miniGameSelect;
			// 원에 들어가는 아이콘.
		std::vector<std::shared_ptr<SpriteUI>> miniGameIcon;
		std::shared_ptr<SpriteUI> miniGameArrow;
		std::shared_ptr<SpriteUI> shaker;
		std::shared_ptr<SpriteUI> FSprite;
			// 아이콘 옆 숫자.
		std::vector<std::shared_ptr<TextUI>> miniGameNumber;
		std::shared_ptr<SpriteUI> countDownBack;
		std::shared_ptr<TextUI> countDown;
		// 체력바
		std::shared_ptr<ProgressBarUI> miniGameHP;

		// amount
		std::vector<std::shared_ptr<TextUI>> amountTexts;

		std::shared_ptr<SpriteUI> tutorial;
		std::shared_ptr<TextUI> nowGold;

		// 손님 관련
		std::shared_ptr<TextUI> customerName;
		std::shared_ptr<SpriteUI> customerSprite;
		std::shared_ptr<SpriteUI> requestSprite;
		std::shared_ptr<TextUI> dialogText;

		// 결과 관련
		std::shared_ptr<SpriteUI> mixCockTail;
		std::shared_ptr<SpriteUI> mixCockTailName;
		std::shared_ptr<SpriteUI> mixBG;
		std::shared_ptr<ButtonUI> offerButton;

		float spriteAlpha;
		float alphaSpeed;

		// 나가는 것을 위한 변수
		bool startBye;

		// 대사 테스트용 idx
		int dialogTestIdx;
		// 0 쉐이커, 1, 2, 3각 오브젝트.
		bool shakePlay[4];
		float shakeTick[4];
		float shakeSpeed[4];
		float shakeOffset[4];

		// select
		bool selectPlay[3];
		float selectOffset[3];
		// scale을 위한 틱(시간)
		float selectTick[3];

		// 카운트 다운용 틱
		float countDownTick;

		// mix 결과용 틱 0.5로 설정
		float mixTick;
		bool displayResult;

		// 버튼 클릭
		float fTick;
		bool buttonClick;

	public:
		BarCoreController(std::weak_ptr<GameObject> obj);
		virtual ~BarCoreController();

		virtual void Awake() override;
		virtual void Start() override;

		// 업데이트에서 
		virtual void Update(float tick) override;
		virtual void LateUpdate(float tick) override;

	private:
		// 각종 상황에 대한 함수를 분리해봅니다.
		void SetAllButtonEvent();

		// 각각의 상황에 어떻게 업데이트를 돌려줄지 함수를 세팅합니다.
		void TutorialUpdate(float tick);
		void IntroUpdate(float tick);
		void OrderUpdate(float tick);
		void MiniGameUpdate(float tick);
		void OfferUpdate(float tick);
		void ByeUpdate(float tick);
		void CloseUpdate(float tick);

		// 모든 버튼을 눌리지 않게 하는 함수
		void AllButtonEnable(bool isEnable);
		void AllStuffButtonEnable(bool isEnable);
		void AllRecipeTitleButtonEnable(bool isEnable);
		// 모두 켜줄 이유가 전혀 없는 함수기 때문
		void AllRecipeSpriteDisable();
		void AllCustomerUIEnable(bool isEnable);
		void MiniGameSetUp();
		const std::shared_ptr<Texture>& GetIconTexture(int idx);

		// 지금 게스트가 누구냐에 따라서 스프라이트 위치를 조정해주기 위함.
		void SetCustomerPosition();
		void GenerateCircleRandomPos();
		void SetMixedCockTailTexture();

		// s, e 까지 n초 동안 쉐이크..
		void PlayRotation(float tick, int idx);
		void PlayScaleAnim(float tick, int idx);
		void StartRotation(int idx);

		// 선택한 것 세팅
		void SetSelect(int idx);
		void ResetSelect();
		void SetNotFoundRecipe();
	};

}
