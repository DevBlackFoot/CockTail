#pragma once

namespace ClientSpace
{

	class ResultUIController : public ComponentBase
	{
		// ���� ����.
		enum class CountState
		{
			// ���ڸ� ���� ����
				// ������ ����մϴ�. (���� ����, ����� ���� ��)
			NUM_COUNT,
			// ���ڰ� ���� ����. ���� ������ �����ϴ�.
				// Ư�� Ű�� ���� �̰����� ��ŵ �� �� �ְ� �մϴ�.
			COUNT_END,
			// �� ������ ���� ������ ������� ����.
				// �ϴ� �־��ݴϴ�.
			CLEAR_ITEM,
			// ���⵵ ��ŵ �����ϰ� ���ݴϴ�.
			CLEAR_END,
			NONE
		};

		// �������� ������ ��Ȯ�ϰ� ������ �ֱ� ������ �迭�� ����� �ݴϴ�.
		// �̸� ����ؼ� �־�� �迭
		int craftItems[7];
		// ������ �迭 -> �ش� �κб��� �ִϸ��̼��� �־��ֱ� ����.
			// ũ����Ʈ�� �ϰ� ���� ��.
		int modItems[14];
		// ���� ����(�������� ���մϴ�.)
		int nowCraftItems[7];
		int nowIngredients[14];

		CountState nowState;

		// ���� �Ŵ��� ĳ��
		std::shared_ptr<GameManager> gameManager;
		std::shared_ptr<InputManager> input;

		std::shared_ptr<Canvas> resultCanvas;
		
		// ������ ������ �ٲ��ֱ� ���� UI ĳ��
		std::shared_ptr<ButtonUI> exitButton;
		std::shared_ptr<TextUI> countText[21];

		// �κ��丮�� �����ɴϴ�.
			// ���� ��� �� �κ��丮�� �������ݴϴ�.
			// ���⼭ ����ϴ� �����ʹ� �κ��丮 �����Ͱ� �ƴմϴ�.
		std::unordered_map<int, std::shared_ptr<InventoryItem>> inventory;

		// ƽ�� �����༭ ī���� ���ݴϴ�.
		float countSpeed;
		// ī��Ʈ�� ���� �̻��� �� SetText�� �������ִ� ������� �����մϴ�.
		float tickCount;
		// ��ǥ ī��Ʈ(�ֵ鸶�� �ٸ��� �������ݴϴ�.)
		float goalCount;

		// ���� ���� üũ
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
		// ó�� �����ϸ� ���Ŀ� �°� ũ����Ʈ�� �� �صӴϴ�.
		void CraftAllItem();
		// ��� ������ �� ���� �־ �󸶳� ũ����Ʈ �Ǵ����� �����մϴ�.
		int CraftItem(int ingred1, int ingred2);

		// �� ���¸��� ������Ʈ ������ ����ϴ�.
		void CountUpdate(float tick);
		void CountEndUpdate(float tick);
		void ClearItemUpdate(float tick);
		void ClearEndUpdate(float tick);
	};

}
