#pragma once


namespace ClientSpace
{
	// ���� �ٽ� ������ ������ ����ϴ� BarCoreController�Դϴ�.
	class BarCoreController : public ComponentBase
	{
		// �ܺο��� ������� �ʴ� ���� enum
		enum class BarState
		{
			// ���� ����, Ʃ�丮���� �����ݴϴ�.
			TUTORIAL,
			// Start, awake���� ������ �մ��� ������ smalltalk ~ order ���������� �����Դϴ�.
			INTRO,
			// �ֹ��� �� �����Դϴ�.
			ORDER,
			// �̴ϰ��� �����Դϴ�.
			MINI_GAME,
			// �̴� ������ ��ġ�� �����ϴ� �ܰ��Դϴ�.
			OFFER,
			// ���� ���� ���ǿ� ���� �λ簡 ���ɴϴ�. + bye�λ� ���� ����մϴ�.
			BYE,
			// ��� �մ��� ������ ���� �ݴ� ��Ȳ�Դϴ�.
			CLOSE,
			// �ƹ��͵� �ƴ� ����Ʈ �����Դϴ�.
			NONE
		};
		// ��� ��Ȳ�� ���������� ����˴ϴ�.
			// tuto -> set_customer, intro, order, mini_game, offer, bye (3�� ��ȸ) -> close, none ������ ����˴ϴ�.
			// ������ ��Ȳ�� ������ �����Ͽ����մϴ�.

		// ������ �����Դϴ�..
		enum class RecipeState
		{
			RUM,
			VODKA,
			GIN,
			NONE
		};

		// ���! ������Ʈ
		enum class ResultState
		{
			GOOD,
			BAD,
			BEER,
			BYE,		// �ۺ��λ縦 ����.
			NONE
		};

		// �մԿ� ���� ���� �迭.
			// ���ο� �մ� �ʿ� ���� ������ �����ϴ�. 0, 1, 2�� ������ ����
			// * 2 + (0 or 1)�� �ؼ� �����մϴ�.
		int guest[3];
		// ���� �Խ�Ʈ �ε���.
			// 0, 1, 2�� �����ϴ�.
		int nowGuestIdx;
		int nowCustomerIdx;
		// ���� Customer ������ �������ݴϴ�.
		std::shared_ptr<Customer> nowCustomer;
		// ���� order ������ �������ݴϴ�.
		std::shared_ptr<CockTailList> nowOrder;
		// ���� Ĭ���� ������ �������ݴϴ�.
		std::shared_ptr<CockTailStruct> nowCocktail;
		// ���� ���¸� �����մϴ�.
		BarState nowState;
		RecipeState nowRecipeState;
		ResultState resultState;

		// �ʱ�ȭ ���� ����� ĵ�������� �������ݴϴ�.
		std::shared_ptr<Canvas> barCanvas;
		// �̴� ���� ĵ����
		std::shared_ptr<Canvas> shakeCanvas;
		// ��� ĵ����
		std::shared_ptr<Canvas> resultCanvas;

		// �⺻������ Item Data�� �����ͼ� ������ݴϴ�.
		std::shared_ptr<GameManager> gameManager;
		std::shared_ptr<InputManager> input;
		std::shared_ptr<ResourceManager> resourceManager;
		std::shared_ptr<SoundManager> sound;
		// �츮�� �κ��丮�� ���� �����۰� ��� �������� ��� ������ �ֽ��ϴ�..
		std::unordered_map<int, std::shared_ptr<InventoryItem>> inventoryMap;
		// Ĭ���� ������ �����ɽô�..
		std::unordered_map<int, std::shared_ptr<CockTailStruct>> cockTailMap;

		// Ŭ���� ���� �뵵. => ��ȭ�� �Ѿ�� �ʰ� �ϱ� ����.
		bool blockClick;

		// ��� �̴ϰ����� �̴ϰ��� ��� ������Ʈ�� ������ �����ϴ� �� ���� �� ������..
		float angle;
		const float startAngle;
		const float angleTheta;
		const int maxIterations;
		const float radius;
		Vector3 uiPos[3];
		Vector3 uiCirclePos[3];

		// �̴ϰ��� HP
		int maxHP;
			// ü�� ���� �� ������ ���� ü��
		int currentHP;
			// ���̱� ��, ��ǥ�� �� ü�� (�������� ���̰� -> currentHP�� �����ϸ� ���� ���ݴϴ�.)
		int prevHP;
		int miniGameCount;
		int correctCount;
		bool isCorrectCockTail;
		bool isTurning;
			// ���� �� ����� offset
		float hpOffset;

		// ������ �ִϸ��̼� ���� ����
		// lerp�� �Կ��� offset. 1�� �Ǹ� �ִϸ��̼��� �������ݴϴ�.
		float recipeOffset;
		// �ִϸ��̼� �ӵ�
		float animSpeed;
		// �����ǰ� �ִϸ����� ���� �ߴ°�.
			// ��ŸƮ �������� ���������� ������ ���ִ� ������.
		bool isAnimating;
		// �����ǰ� ������ �Ǿ� �ִ°�.
			// ��ư�� ������ ���¿� ���� (���� ������, ���ְ� / ���� ������ ���ش�.)
		bool isRecipeSet;
		Vector3 startSpritePos;
		Vector3 startButtonPos;
		Vector3 endSpritePos;
		Vector3 endButtonPos;
		// �⺻������ �����ǿ� ������� ��������
		std::wstring priceText;

		// ���� ����.
		int select[3];
		// ��ư Ŭ���� �� ���� �ϳ��� �ø��ϴ�.
		int selectIdx;

		int dialogIdx;

		// Ʃ�丮���� ���� ȭ��.
		int tutorialIdx;
		int prevTutorialIdx;
		// ó�� ���°� �ٲ� ��
		bool changeFirst;
		bool setMix;
		// �Խ�Ʈ ������ �ӵ��� ���� tick
		float guestTick;
		bool guestIn;

		bool miniGameSuccess = false;

		// ���� ������ ��ư�� �����ǿ� �´� ��ư����.
		int checkIngredient[24];

		// �ٽ������� ���� �״� ��������..
		// �� ��ư��
		std::vector<std::shared_ptr<ButtonUI>> stuffButtons;
		// �ͽ� ���� ���
		std::shared_ptr<ButtonUI> mixButton;
		std::shared_ptr<ButtonUI> resetButton;
		std::shared_ptr<ButtonUI> beerButton;
		std::shared_ptr<ButtonUI> closeButton;
		// ���� ���� �ؽ�Ʈ��
		std::vector<std::shared_ptr<TextUI>> selectText;
		std::vector<std::shared_ptr<SpriteUI>> selectSprite;
		// ������ ���� ��ư (ȣ�� üũ ��ư)
		std::vector<std::shared_ptr<ButtonUI>> recipeHoverButtons;
		std::vector<std::shared_ptr<ButtonUI>> recipeTitleButton;
		std::shared_ptr<ButtonUI> recipeButton;
		//������ ��׶���
		std::shared_ptr<SpriteUI> recipeBackGround;

		std::vector<std::shared_ptr<SpriteUI>> recipeSprite;
		std::shared_ptr<SpriteUI> infoBack;
		std::shared_ptr<TextUI> infoText;

		// �̴� ���� �̹��� ĳ��
			// ���ʿ� �ߴ� ����
		std::vector<std::shared_ptr<SpriteUI>> miniGameSelect;
			// ���� ���� ������.
		std::vector<std::shared_ptr<SpriteUI>> miniGameIcon;
		std::shared_ptr<SpriteUI> miniGameArrow;
		std::shared_ptr<SpriteUI> shaker;
		std::shared_ptr<SpriteUI> FSprite;
			// ������ �� ����.
		std::vector<std::shared_ptr<TextUI>> miniGameNumber;
		std::shared_ptr<SpriteUI> countDownBack;
		std::shared_ptr<TextUI> countDown;
		// ü�¹�
		std::shared_ptr<ProgressBarUI> miniGameHP;

		// amount
		std::vector<std::shared_ptr<TextUI>> amountTexts;

		std::shared_ptr<SpriteUI> tutorial;
		std::shared_ptr<TextUI> nowGold;

		// �մ� ����
		std::shared_ptr<TextUI> customerName;
		std::shared_ptr<SpriteUI> customerSprite;
		std::shared_ptr<SpriteUI> requestSprite;
		std::shared_ptr<TextUI> dialogText;

		// ��� ����
		std::shared_ptr<SpriteUI> mixCockTail;
		std::shared_ptr<SpriteUI> mixCockTailName;
		std::shared_ptr<SpriteUI> mixBG;
		std::shared_ptr<ButtonUI> offerButton;

		float spriteAlpha;
		float alphaSpeed;

		// ������ ���� ���� ����
		bool startBye;

		// ��� �׽�Ʈ�� idx
		int dialogTestIdx;
		// 0 ����Ŀ, 1, 2, 3�� ������Ʈ.
		bool shakePlay[4];
		float shakeTick[4];
		float shakeSpeed[4];
		float shakeOffset[4];

		// select
		bool selectPlay[3];
		float selectOffset[3];
		// scale�� ���� ƽ(�ð�)
		float selectTick[3];

		// ī��Ʈ �ٿ�� ƽ
		float countDownTick;

		// mix ����� ƽ 0.5�� ����
		float mixTick;
		bool displayResult;

		// ��ư Ŭ��
		float fTick;
		bool buttonClick;

	public:
		BarCoreController(std::weak_ptr<GameObject> obj);
		virtual ~BarCoreController();

		virtual void Awake() override;
		virtual void Start() override;

		// ������Ʈ���� 
		virtual void Update(float tick) override;
		virtual void LateUpdate(float tick) override;

	private:
		// ���� ��Ȳ�� ���� �Լ��� �и��غ��ϴ�.
		void SetAllButtonEvent();

		// ������ ��Ȳ�� ��� ������Ʈ�� �������� �Լ��� �����մϴ�.
		void TutorialUpdate(float tick);
		void IntroUpdate(float tick);
		void OrderUpdate(float tick);
		void MiniGameUpdate(float tick);
		void OfferUpdate(float tick);
		void ByeUpdate(float tick);
		void CloseUpdate(float tick);

		// ��� ��ư�� ������ �ʰ� �ϴ� �Լ�
		void AllButtonEnable(bool isEnable);
		void AllStuffButtonEnable(bool isEnable);
		void AllRecipeTitleButtonEnable(bool isEnable);
		// ��� ���� ������ ���� ���� �Լ��� ����
		void AllRecipeSpriteDisable();
		void AllCustomerUIEnable(bool isEnable);
		void MiniGameSetUp();
		const std::shared_ptr<Texture>& GetIconTexture(int idx);

		// ���� �Խ�Ʈ�� �����Ŀ� ���� ��������Ʈ ��ġ�� �������ֱ� ����.
		void SetCustomerPosition();
		void GenerateCircleRandomPos();
		void SetMixedCockTailTexture();

		// s, e ���� n�� ���� ����ũ..
		void PlayRotation(float tick, int idx);
		void PlayScaleAnim(float tick, int idx);
		void StartRotation(int idx);

		// ������ �� ����
		void SetSelect(int idx);
		void ResetSelect();
		void SetNotFoundRecipe();
	};

}
