#pragma once


namespace ClientSpace
{
	class GameManager;

	// Ingame에서 아이템을 먹었을 때 해당 내역을 화면에 표시해주는 기능을 담당하는 ItemGetUI입니다.
	// 2023.06.26 Dev.BlackFoot
	class ItemGetUI : public ComponentBase
	{
	private:
		// 게임 매니저에서 받아와서 세팅만 해주는 느낌으로 작성해줍니다.
		std::shared_ptr<Canvas> ingameCanvas;
		std::shared_ptr<GameManager> gameManager;
		// 기본적으로 넣어줄 텍스트입니다.
		std::wstring systemMessageText;
		// 텍스트 UI의 이름
		vector<std::shared_ptr<TextUI>> recentTextUI;

		// 3초를 세기 위한 카운트
		float tickCount;
		// 처음 순회 하면서 시스템 메시지를 끄기 위한 bool 변수
		bool needOffText;

	public:
		ItemGetUI(std::weak_ptr<GameObject> _object);
		virtual ~ItemGetUI();

		// 클래스 내부에서 사용할 목록들 캐싱.
		void Init();

		void Update(float tick) override;
	};

}
