#pragma once

#include "../../FootGraphicsEngine/GraphicsEngineFramework.h"

namespace GameEngineSpace
{
	class GraphicsManager;

	using GraphicsEngineSpace::Canvas;
	using GraphicsEngineSpace::UIBase;
	using GraphicsEngineSpace::Texture;
	/**
	 * \brief ���ӿ��� ������ ��� UI�� ������ִ� ������ �ϴ� Ŭ�����Դϴ�.
	 * ���⼭ ������� ĵ�������� ��� UIManager�� mainCanvas�ȿ� �����ϰ� �� ���̹Ƿ�
	 * Ŭ������ ���� ������ �����ص� �����մϴ�.
	 *
	 * ����� ĵ���� ��ü�� UI Manager�� Init���� �����˴ϴ�.
	 * �� Ŭ���������� ������ ĵ���� �ȿ� ������� ��� ��������Ʈ �� �ؽ�Ʈ�� �����մϴ�.
	 *
	 * ������ ���� �ǰ�, UI Manager�� Init�� �� ȣ��˴ϴ�.
	 */
	class UICreator
	{
	private:
		std::shared_ptr<Canvas> mainCanvas;
		std::vector<std::shared_ptr<Texture>> tutorialTextures;

		std::shared_ptr<GraphicsManager> graphicsManager;

		// ���⿡ â ��ȯ�� ���� �������� �����غ��ô�.
		std::vector<std::pair<int, int>> windowSize;
		int sizeIdx;

	public:
		UICreator();
		~UICreator() {}

		void CreateCanvasAll();

	private:
		// ������ ������ ĵ������
		void CreateTitleCanvas();
		void CreateSettingCanvas();
		void CreateCreditCanvas();
		void CreateGameHUD();
		void CreateInMenuCanvas();
		void CreateCombinationCanvas();
		void CreateDialogCanvas();
		void CreateBarDialogCanvas();
		void CreateSceneCanvas();
		void CreateInteractionCanvas();
		void CreateMiniGameCanvas();
		void CreateItemResultCanvas();
		void CreateStoreCanvas();
		void CreatePopupCanvas();
		void CreateCursorCanvas();
		void CreateBarCanvas();
		void CreateShakeCanvas();
		void CreateResultCanvas();
	};

}
