#pragma once


namespace ClientSpace
{
	class GameManager;

	// Ingame���� �������� �Ծ��� �� �ش� ������ ȭ�鿡 ǥ�����ִ� ����� ����ϴ� ItemGetUI�Դϴ�.
	// 2023.06.26 Dev.BlackFoot
	class ItemGetUI : public ComponentBase
	{
	private:
		// ���� �Ŵ������� �޾ƿͼ� ���ø� ���ִ� �������� �ۼ����ݴϴ�.
		std::shared_ptr<Canvas> ingameCanvas;
		std::shared_ptr<GameManager> gameManager;
		// �⺻������ �־��� �ؽ�Ʈ�Դϴ�.
		std::wstring systemMessageText;
		// �ؽ�Ʈ UI�� �̸�
		vector<std::shared_ptr<TextUI>> recentTextUI;

		// 3�ʸ� ���� ���� ī��Ʈ
		float tickCount;
		// ó�� ��ȸ �ϸ鼭 �ý��� �޽����� ���� ���� bool ����
		bool needOffText;

	public:
		ItemGetUI(std::weak_ptr<GameObject> _object);
		virtual ~ItemGetUI();

		// Ŭ���� ���ο��� ����� ��ϵ� ĳ��.
		void Init();

		void Update(float tick) override;
	};

}
