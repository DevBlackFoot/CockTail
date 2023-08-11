#include "GamePch.h"
#include "UICreator.h"
#include "SceneManager.h"
#include "UIManager.h"
#include "GraphicsManager.h"
#include "WindowManager.h"

namespace GameEngineSpace
{
	UICreator::UICreator()
		: sizeIdx(2)
	{
		
	}

	void UICreator::CreateCanvasAll()
	{
		// 사용할 메인 캔버스를 가져옵니다.
		mainCanvas = UIManager::GetInstance()->GetMainCanvas();

		graphicsManager = GraphicsManager::GetInstance();

		// 풀스크린을 포함한..
		windowSize.resize(4);
		windowSize[0] = std::make_pair(1280, 720);
		windowSize[1] = std::make_pair(1600, 900);
		windowSize[2] = std::make_pair(1920, 1080);

		int width = GetSystemMetrics(SM_CXSCREEN);
		int height = GetSystemMetrics(SM_CYSCREEN);

		windowSize[3] = std::make_pair(width, height);

		// 아래의 모든 UI를 생성합니다.
		CreateTitleCanvas();
		CreateSettingCanvas();
		CreateCreditCanvas();
		CreateDialogCanvas();
		CreateGameHUD();
		//CreateCombinationCanvas();
		CreateSceneCanvas();
		CreateInteractionCanvas();
		//CreateMiniGameCanvas();
		CreateItemResultCanvas();
		CreateBarDialogCanvas();
		CreateStoreCanvas();
		CreatePopupCanvas();
		CreateBarCanvas();
		CreateShakeCanvas();
		CreateCursorCanvas();
		CreateResultCanvas();
		CreateInMenuCanvas();
	}

	void UICreator::CreateTitleCanvas()
	{
		auto canvas = mainCanvas->GetCanvasUI("TitleCanvas");

		auto titlebackGroundTexture = graphicsManager->LoadTexture(L"Resources/UI/TitleBackGround.png");

		auto gameTitleTexture = graphicsManager->LoadTexture(L"Resources/UI/Text/logo.png");

		auto strartButtonTexture = graphicsManager->LoadTexture(L"Resources/UI/StartButton.png");

		auto continueButtonTexture = graphicsManager->LoadTexture(L"Resources/UI/ContinueButton.png");

		auto creditButtonTexture = graphicsManager->LoadTexture(L"Resources/UI/CreditButton.png");

		auto settingButtonTexture = graphicsManager->LoadTexture(L"Resources/UI/SettingButton.png");

		auto exitButtonTexture = graphicsManager->LoadTexture(L"Resources/UI/ExitButton.png");

		// 테스트용 text
		/*
		auto testText = canvas->CreateTextUI("StyleTextTest");
		testText->SetStyleText(L"이건 /cFFFF00노란색 /cFFFFFF이/n나와야 함/n/i이건 /cFF0000이탤릭/cFFFFFF/n/b이건 /cF0E0FF볼드/cFFFFFF/n");
		testText->SetFontSize(36.f);
		testText->SetFont("NanumSquareRoundRegular");
		testText->SetPosition({0.f, 0.f, 0.3f});
		*/

		//==================(SettingBackground)===============================
		auto titleBackGroundSpriteUI = canvas->CreateSpriteUI("TitleBackGroundSpriteUI");
		titleBackGroundSpriteUI->SetTexture(titlebackGroundTexture);
		titleBackGroundSpriteUI->SetPosition(Vector3{ 0.0f, 0.0f, 0.5f });
		titleBackGroundSpriteUI->SetWidth(1920.f);
		titleBackGroundSpriteUI->SetHeight(1080.f);

		auto gameTitleSpriteUI = canvas->CreateSpriteUI("GameTitleSpriteUI");
		gameTitleSpriteUI->SetTexture(gameTitleTexture);
		gameTitleSpriteUI->SetPosition(Vector3{ -638.0f, -294.0f, 0.4f });
		gameTitleSpriteUI->SetWidth(623.f);
		gameTitleSpriteUI->SetHeight(250.f);


		//==================(Setting Buttons)===============================
		auto startButton = canvas->CreateButtonUI("StartButton");
		startButton->SetPosition(Vector3{ -820.0f, 73.0f, 0.4f });
		startButton->SetDefaultTexture(strartButtonTexture);
		startButton->SetDefaultMaskColor(0.5f, 0.5f, 0.5f, 1.0f);
		startButton->SetHoverMaskColor(1.0f, 1.0f, 1.0f, 1.0f);
		startButton->SetWidth(108.f);
		startButton->SetHeight(36.f);

		auto creditButton = canvas->CreateButtonUI("CreditButton");
		creditButton->SetPosition(Vector3{ -809.0f, 164.0f , 0.4f });
		creditButton->SetDefaultTexture(creditButtonTexture);
		creditButton->SetDefaultMaskColor(0.5f, 0.5f, 0.5f, 1.0f);
		creditButton->SetHoverMaskColor(1.0f, 1.0f, 1.0f, 1.0f);
		creditButton->SetWidth(132.f);
		creditButton->SetHeight(39.f);

		auto settingButton = canvas->CreateButtonUI("SettingButton");
		settingButton->SetPosition(Vector3{ -795.0f, 258.0f, 0.4f });
		settingButton->SetDefaultTexture(settingButtonTexture);
		settingButton->SetDefaultMaskColor(0.5f, 0.5f, 0.5f, 1.0f);
		settingButton->SetHoverMaskColor(1.0f, 1.0f, 1.0f, 1.0f);
		settingButton->SetWidth(156.f);
		settingButton->SetHeight(47.f);

		auto exitButton = canvas->CreateButtonUI("ExitButton");
		exitButton->SetPosition(Vector3{ -830.0f, 342.0f, 0.4f });
		exitButton->SetDefaultTexture(exitButtonTexture);
		exitButton->SetDefaultMaskColor(0.5f, 0.5f, 0.5f, 1.0f);
		exitButton->SetHoverMaskColor(1.0f, 1.0f, 1.0f, 1.0f);
		exitButton->SetWidth(89.f);
		exitButton->SetHeight(39.f);

		canvas->SetEnable(false);
	}

	void UICreator::CreateDialogCanvas()
	{
		auto canvas = mainCanvas->GetCanvasUI("DialogCanvas");

		auto dialogTexture = graphicsManager->LoadTexture(L"Resources/UI/Dialog.png");

		auto cockTexture1 = graphicsManager->LoadTexture(L"Resources/UI/Character/cock1.png");
		auto cockTexture2 = graphicsManager->LoadTexture(L"Resources/UI/Character/cock2.png");
		auto cockTexture3 = graphicsManager->LoadTexture(L"Resources/UI/Character/cock3.png");
		auto cockTexture4 = graphicsManager->LoadTexture(L"Resources/UI/Character/cock4.png");
		auto cockTexture5 = graphicsManager->LoadTexture(L"Resources/UI/Character/cock5.png");

		auto tailTexture1 = graphicsManager->LoadTexture(L"Resources/UI/Character/tail1.png");
		auto tailTexture2 = graphicsManager->LoadTexture(L"Resources/UI/Character/tail2.png");
		auto tailTexture3 = graphicsManager->LoadTexture(L"Resources/UI/Character/tail3.png");
		auto tailTexture4 = graphicsManager->LoadTexture(L"Resources/UI/Character/tail4.png");
		auto tailTexture5 = graphicsManager->LoadTexture(L"Resources/UI/Character/tail5.png");

		auto redTexture = graphicsManager->LoadTexture(L"Resources/UI/Character/red.png");
		auto redTexture2 = graphicsManager->LoadTexture(L"Resources/UI/Character/red2.png");
		auto redTexture3 = graphicsManager->LoadTexture(L"Resources/UI/Character/red3.png");
		auto redTexture4 = graphicsManager->LoadTexture(L"Resources/UI/Character/red4.png");


		auto tutorialClear = graphicsManager->LoadTexture(L"Resources/UI/Clear.png");

		// 튜토리얼 클리어 UI
		auto tutorialClearUI = canvas->CreateSpriteUI("TutorialClearUI");
		tutorialClearUI->SetTexture(tutorialClear);
		tutorialClearUI->SetPosition(Vector3{ 444.0f, -360.0f, 0.3f });
		tutorialClearUI->SetWidth(100.f);
		tutorialClearUI->SetHeight(100.f);
		tutorialClearUI->SetEnable(false);

		//==================(Characters)===================================
		auto cock1DialogSpriteUI = canvas->CreateSpriteUI("Cock1DialogSpriteUI");
		cock1DialogSpriteUI->SetTexture(cockTexture1);
		cock1DialogSpriteUI->SetPosition(Vector3{ -544.0f, -36.0f, 0.4f });
		cock1DialogSpriteUI->SetWidth(413.f);
		cock1DialogSpriteUI->SetHeight(600.f);
		cock1DialogSpriteUI->SetEnable(false);

		auto cock2DialogSpriteUI = canvas->CreateSpriteUI("Cock2DialogSpriteUI");
		cock2DialogSpriteUI->SetTexture(cockTexture2);
		cock2DialogSpriteUI->SetPosition(Vector3{ -544.0f, -36.0f, 0.4f });
		cock2DialogSpriteUI->SetWidth(413.f);
		cock2DialogSpriteUI->SetHeight(600.f);
		cock2DialogSpriteUI->SetEnable(false);

		auto cock3DialogSpriteUI = canvas->CreateSpriteUI("Cock3DialogSpriteUI");
		cock3DialogSpriteUI->SetTexture(cockTexture3);
		cock3DialogSpriteUI->SetPosition(Vector3{ -511.0f, -36.0f, 0.4f });
		cock3DialogSpriteUI->SetWidth(413.f);
		cock3DialogSpriteUI->SetHeight(600.f);
		cock3DialogSpriteUI->SetEnable(false);

		auto cock4DialogSpriteUI = canvas->CreateSpriteUI("Cock4DialogSpriteUI");
		cock4DialogSpriteUI->SetTexture(cockTexture4);
		cock4DialogSpriteUI->SetPosition(Vector3{ -510.0f, -36.0f, 0.4f });
		cock4DialogSpriteUI->SetWidth(413.f);
		cock4DialogSpriteUI->SetHeight(600.f);
		cock4DialogSpriteUI->SetEnable(false);

		auto cock5DialogSpriteUI = canvas->CreateSpriteUI("Cock5DialogSpriteUI");
		cock5DialogSpriteUI->SetTexture(cockTexture5);
		cock5DialogSpriteUI->SetPosition(Vector3{ -544.0f, -36.0f, 0.4f });
		cock5DialogSpriteUI->SetWidth(413.f);
		cock5DialogSpriteUI->SetHeight(600.f);
		cock5DialogSpriteUI->SetEnable(false);

		//tail
		auto tail1DialogSpriteUI = canvas->CreateSpriteUI("Tail1DialogSpriteUI");
		tail1DialogSpriteUI->SetTexture(tailTexture1);
		tail1DialogSpriteUI->SetPosition(Vector3{ -544.0f, 30.0f, 0.4f });
		tail1DialogSpriteUI->SetWidth(340.f);
		tail1DialogSpriteUI->SetHeight(466.f);
		tail1DialogSpriteUI->SetEnable(false);

		auto tail2DialogSpriteUI = canvas->CreateSpriteUI("Tail2DialogSpriteUI");
		tail2DialogSpriteUI->SetTexture(tailTexture2);
		tail2DialogSpriteUI->SetPosition(Vector3{ -544.0f, 30.0f, 0.4f });
		tail2DialogSpriteUI->SetWidth(340.f);
		tail2DialogSpriteUI->SetHeight(466.f);
		tail2DialogSpriteUI->SetEnable(false);

		auto tail3DialogSpriteUI = canvas->CreateSpriteUI("Tail3DialogSpriteUI");
		tail3DialogSpriteUI->SetTexture(tailTexture3);
		tail3DialogSpriteUI->SetPosition(Vector3{ -544.0f, 30.0f, 0.4f });
		tail3DialogSpriteUI->SetWidth(340.f);
		tail3DialogSpriteUI->SetHeight(466.f);
		tail3DialogSpriteUI->SetEnable(false);

		auto tail4DialogSpriteUI = canvas->CreateSpriteUI("Tail4DialogSpriteUI");
		tail4DialogSpriteUI->SetTexture(tailTexture4);
		tail4DialogSpriteUI->SetPosition(Vector3{ -544.0f, 30.0f, 0.4f });
		tail4DialogSpriteUI->SetWidth(340.f);
		tail4DialogSpriteUI->SetHeight(466.f);
		tail4DialogSpriteUI->SetEnable(false);

		auto tail5DialogSpriteUI = canvas->CreateSpriteUI("Tail5DialogSpriteUI");
		tail5DialogSpriteUI->SetTexture(tailTexture5);
		tail5DialogSpriteUI->SetPosition(Vector3{ -544.0f, 30.0f, 0.4f });
		tail5DialogSpriteUI->SetWidth(340.f);
		tail5DialogSpriteUI->SetHeight(466.f);
		tail5DialogSpriteUI->SetEnable(false);

		auto redDialogSpriteUI = canvas->CreateSpriteUI("RedDialogSpriteUI");
		redDialogSpriteUI->SetTexture(redTexture);
		redDialogSpriteUI->SetPosition(Vector3{ -544.0f, 16.0f, 0.4f });
		redDialogSpriteUI->SetWidth(500.f);
		redDialogSpriteUI->SetHeight(500.f);
		redDialogSpriteUI->SetEnable(false);

		auto redDialogSpriteUI2 = canvas->CreateSpriteUI("RedDialogSpriteUI2");
		redDialogSpriteUI2->SetTexture(redTexture2);
		redDialogSpriteUI2->SetPosition(Vector3{ -544.0f, 16.0f, 0.4f });
		redDialogSpriteUI2->SetWidth(500.f);
		redDialogSpriteUI2->SetHeight(500.f);
		redDialogSpriteUI2->SetEnable(false);

		auto redDialogSpriteUI3 = canvas->CreateSpriteUI("RedDialogSpriteUI3");
		redDialogSpriteUI3->SetTexture(redTexture3);
		redDialogSpriteUI3->SetPosition(Vector3{ -544.0f, 16.0f, 0.4f });
		redDialogSpriteUI3->SetWidth(500.f);
		redDialogSpriteUI3->SetHeight(500.f);
		redDialogSpriteUI3->SetEnable(false);

		auto redDialogSpriteUI4 = canvas->CreateSpriteUI("RedDialogSpriteUI4");
		redDialogSpriteUI4->SetTexture(redTexture4);
		redDialogSpriteUI4->SetPosition(Vector3{ -544.0f, 16.0f, 0.4f });
		redDialogSpriteUI4->SetWidth(500.f);
		redDialogSpriteUI4->SetHeight(500.f);
		redDialogSpriteUI4->SetEnable(false);



		//==================(Dialog)===================================
		auto dialogSpriteUI = canvas->CreateSpriteUI("DialogSpriteUI");
		dialogSpriteUI->SetTexture(dialogTexture);
		dialogSpriteUI->SetPosition(Vector3{ -29.0f, 328.0f, 0.4f });
		dialogSpriteUI->SetWidth(1411.f);
		dialogSpriteUI->SetHeight(356.f);
		dialogSpriteUI->SetEnable(false);

		auto characterName1Text = canvas->CreateTextUI("CharacterName1");
		std::wstring nameTextString = L"테일";
		characterName1Text->SetPosition(Vector3{ -671.0f, 219.0f, 0.4f });
		characterName1Text->SetText(nameTextString);
		characterName1Text->SetFont("NanumSquareNeoBold");
		characterName1Text->SetFontSize(30.f);
		characterName1Text->SetEnable(false);

		auto characterName2Text = canvas->CreateTextUI("CharacterName2");
		std::wstring nameTextString2 = L"칵";
		characterName2Text->SetPosition(Vector3{ -671.0f, 219.0f, 0.4f });
		characterName2Text->SetText(nameTextString2);
		characterName2Text->SetFont("NanumSquareNeoBold");
		characterName2Text->SetFontSize(30.f);
		characterName2Text->SetEnable(false);

		auto characterName3Text = canvas->CreateTextUI("CharacterName3");
		std::wstring nameTextString3 = L"레드";
		characterName3Text->SetPosition(Vector3{ -671.0f, 219.0f, 0.4f });
		characterName3Text->SetText(nameTextString3);
		characterName3Text->SetFont("NanumSquareNeoBold");
		characterName3Text->SetFontSize(30.f);
		characterName3Text->SetEnable(false);

		auto cutScenedialogText = canvas->CreateTextUI("CutSceneDialog");
		std::wstring dialogTextString0 = L"";
		cutScenedialogText->SetPosition(Vector3{ -50.0f, 325.0f, 0.4f });
		cutScenedialogText->SetText(dialogTextString0);
		cutScenedialogText->SetFixedWidth(1189.f);
		cutScenedialogText->SetColor(Vector4{ powf((255.f / 255.f), 2.2f), powf((255.f / 255.f), 2.2f), powf((255.f / 255.f), 2.2f), 1.f });
		cutScenedialogText->SetFont("NanumSquareRoundRegular");
		cutScenedialogText->SetFontSize(30.f);
		cutScenedialogText->SetEnable(false);

		auto tutorialExplanationText = canvas->CreateTextUI("TutorialExplanation");
		std::wstring tutoText = L"";
		tutorialExplanationText->SetPosition(Vector3{ 50.0f, -360.0f, 0.4f });
		tutorialExplanationText->SetText(tutoText);
		tutorialExplanationText->SetFont("NanumSquareRoundRegular");
		tutorialExplanationText->SetFontSize(30.f);
		tutorialExplanationText->SetEnable(false);

		canvas->SetEnable(false);
	}

	void UICreator::CreateBarDialogCanvas()
	{
		auto canvas = mainCanvas->GetCanvasUI("BarDialogCanvas");

		auto buttonTexture = graphicsManager->LoadTexture(L"Resources/UI/Button.png");

		auto reQuestTexture = graphicsManager->LoadTexture(L"Resources/UI/ReQuest.png");


		auto reQuestSpriteUI = canvas->CreateSpriteUI("ReQuestSpriteUI");
		reQuestSpriteUI->SetTexture(reQuestTexture);
		reQuestSpriteUI->SetPosition(Vector3{ 604.0f, -326.0f, 0.4f });
		reQuestSpriteUI->SetWidth(619.f);
		reQuestSpriteUI->SetHeight(256.f);
		reQuestSpriteUI->SetEnable(false);

		auto loppNameText = canvas->CreateTextUI("LoppNameText");
		std::wstring loppNameTextString = L"롭";
		loppNameText->SetPosition(Vector3{ 367.0f, -429.0f ,0.4f });
		loppNameText->SetText(loppNameTextString);
		//loppNameText->SetColor(Vector4{ powf((255.f / 255.f), 2.2f), powf((242.f / 255.f), 2.2f), powf((222.f / 255.f), 2.2f), 1.f });
		loppNameText->SetFont("NanumSquareNeoBold");
		loppNameText->SetFontSize(30.f);
		loppNameText->SetEnable(false);


		auto lexNameText = canvas->CreateTextUI("lexNameText");
		std::wstring lexNameTextString = L"렉스";
		lexNameText->SetPosition(Vector3{ 384.0f, -429.0f ,0.4f });
		lexNameText->SetText(lexNameTextString);
		//lexNameText->SetColor(Vector4{ powf((255.f / 255.f), 2.2f), powf((242.f / 255.f), 2.2f), powf((222.f / 255.f), 2.2f), 1.f });
		lexNameText->SetFont("NanumSquareNeoBold");
		lexNameText->SetFontSize(30.f);
		lexNameText->SetEnable(false);

		auto bobbNameText = canvas->CreateTextUI("BobbNameText");
		std::wstring bobbNameTextString = L"밥";
		bobbNameText->SetPosition(Vector3{ 370.0f, -426.0f ,0.4f });
		bobbNameText->SetText(bobbNameTextString);
		//bobbNameText->SetColor(Vector4{ powf((255.f / 255.f), 2.2f), powf((242.f / 255.f), 2.2f), powf((222.f / 255.f), 2.2f), 1.f });
		bobbNameText->SetFont("NanumSquareNeoBold");
		bobbNameText->SetFontSize(30.f);
		bobbNameText->SetEnable(false);



		auto customerdialogText = canvas->CreateTextUI("CustomerdialogText");
		std::wstring customerdialogString = L"";
		customerdialogText->SetPosition(Vector3{ 490.0f, -354.0f, 0.4f });
		customerdialogText->SetText(customerdialogString);
		//customerdialogText->SetColor(Vector4{ powf((255.f / 255.f), 2.2f), powf((242.f / 255.f), 2.2f), powf((222.f / 255.f), 2.2f), 1.f });
		customerdialogText->SetFont("NanumSquareRoundRegular");
		customerdialogText->SetFontSize(25.f);
		customerdialogText->SetFixedWidth(250);
		customerdialogText->SetEnable(false);


		auto selectDialogButtons1 = canvas->CreateButtonUI("SelectDialogButtons1");
		selectDialogButtons1->SetPosition(Vector3{ 415.f, -79.f, 0.4f });
		selectDialogButtons1->SetDefaultTexture(buttonTexture);
		selectDialogButtons1->SetWidth(580.f);
		selectDialogButtons1->SetHeight(60.f);
		selectDialogButtons1->SetEnable(false);

		auto selectDialogButtons2 = canvas->CreateButtonUI("SelectDialogButtons2");
		selectDialogButtons2->SetPosition(Vector3{ 415.f, 38.f, 0.4f });
		selectDialogButtons2->SetDefaultTexture(buttonTexture);
		selectDialogButtons2->SetWidth(580.f);
		selectDialogButtons2->SetHeight(60.f);
		selectDialogButtons2->SetEnable(false);

		canvas->SetEnable(false);

	}

	void UICreator::CreateSceneCanvas()
	{
		auto canvas = mainCanvas->GetCanvasUI("SceneCanvas");

		auto loadingBackGroundTexture = graphicsManager->LoadTexture(L"Resources/UI/LoadingBG.png");

		auto sceneBackGroundTexture = graphicsManager->LoadTexture(L"Resources/UI/SceneBackGround.png");

		auto loadingBackGroundSpriteUI = canvas->CreateSpriteUI("LoadingSpriteUI");
		loadingBackGroundSpriteUI->SetPosition(Vector3{ 0.f, 0.f, 0.5f });
		loadingBackGroundSpriteUI->SetWidth(1923.f);
		loadingBackGroundSpriteUI->SetHeight(1080.f);
		loadingBackGroundSpriteUI->SetTexture(loadingBackGroundTexture);
		loadingBackGroundSpriteUI->SetEnable(false);

		auto sceneBackGroundSpriteUI = canvas->CreateSpriteUI("SceneBackGroundSpriteUI");
		sceneBackGroundSpriteUI->SetPosition(Vector3{ 0.f, 0.f, 0.9f });
		sceneBackGroundSpriteUI->SetWidth(1923.f);
		sceneBackGroundSpriteUI->SetHeight(1080.f);
		sceneBackGroundSpriteUI->SetTexture(sceneBackGroundTexture);
		sceneBackGroundSpriteUI->SetEnable(false);

		// width slice 94, height 119
		auto lodingSpriteSheetTex = graphicsManager->LoadTexture(L"Resources/UI/LoadingSheet.png");

		auto loadingAnim = canvas->CreateSpriteUI("LoadingAnim");
		loadingAnim->SetPosition(Vector3{ 870.f, 450.f, 0.3f });
		loadingAnim->SetWidth(94.f);
		loadingAnim->SetHeight(119.f);
		loadingAnim->SetTexture(lodingSpriteSheetTex);

		loadingAnim->SetIsAnim(true);

		loadingAnim->SetEnable(false);

		canvas->SetEnable(false);
	}

	void UICreator::CreateCombinationCanvas()
	{
		auto canvas = mainCanvas->GetCanvasUI("CombinationCanvas");

		auto combinationBackGroundTexture = graphicsManager->LoadTexture(L"Resources/UI/Meking_BG.png");

		auto tutorialBackGroundTexture = graphicsManager->LoadTexture(L"Resources/UI/slide1.png");

		auto tutorialBackGroundTexture1 = graphicsManager->LoadTexture(L"Resources/UI/slide2.png");

		auto tutorialBackGroundTexture2 = graphicsManager->LoadTexture(L"Resources/UI/slide3.png");

		auto tutorialBackGroundTexture3 = graphicsManager->LoadTexture(L"Resources/UI/slide4.png");

		auto tutorialBackGroundTexture4 = graphicsManager->LoadTexture(L"Resources/UI/slide5.png");

		auto tutorialBackGroundTexture5 = graphicsManager->LoadTexture(L"Resources/UI/slide6.png");

		auto tutorialBackGroundTexture6 = graphicsManager->LoadTexture(L"Resources/UI/slide7.png");

		auto tutorialBackGroundTexture7 = graphicsManager->LoadTexture(L"Resources/UI/slide8.png");

		//auto tutorialButtonTexture = graphicsManager->LoadTexture(L"Resources/UI/NotUI.png");

		auto buttonTexture = graphicsManager->LoadTexture(L"Resources/UI/Button.png");

		auto mixbuttonTexture = graphicsManager->LoadTexture(L"Resources/UI/CackTailMix.png");

		auto reSetbuttonTexture = graphicsManager->LoadTexture(L"Resources/UI/ReSet.png");

		auto beerMaChineTexture = graphicsManager->LoadTexture(L"Resources/UI/BeerMachine.png");

		auto cocktailrecipeTexture = graphicsManager->LoadTexture(L"Resources/UI/Recipe_button2.png");

		auto customerTexture1 = graphicsManager->LoadTexture(L"Resources/UI/Character/npc1_1.png");

		auto customerTexture2 = graphicsManager->LoadTexture(L"Resources/UI/Character/npc2_1.png");

		auto customerTexture3 = graphicsManager->LoadTexture(L"Resources/UI/Character/npc3_1.png");

		auto recipeBackGroundTexture = graphicsManager->LoadTexture(L"Resources/UI/RecipeBack.png");

		auto rumRecipeTexture = graphicsManager->LoadTexture(L"Resources/UI/Recipe_rum.png");

		auto vodkaRecipeTexture = graphicsManager->LoadTexture(L"Resources/UI/Recipe_vodka.png");

		auto ginRecipeTexture = graphicsManager->LoadTexture(L"Resources/UI/Recipe_gin.png");

		auto informationButtonTexture = graphicsManager->LoadTexture(L"Resources/UI/NotUI.png");

		// 레시피 버튼 텍스쳐
		auto rumRecipeButtonTexture = graphicsManager->LoadTexture(L"Resources/UI/Recipe_rumButton.png");

		auto vodkaRecipeButtonTexture = graphicsManager->LoadTexture(L"Resources/UI/Recipe_vodkaButton.png");

		auto ginRecipeButtonTexture = graphicsManager->LoadTexture(L"Resources/UI/Recipe_ginButton.png");

		auto informationBackGroundTexture = graphicsManager->LoadTexture(L"Resources/UI/informationBackGround.png");
		//
		auto rumButtonTexture = graphicsManager->LoadTexture(L"Resources/UI/M_RUM.png");

		auto vodkaButtonTexture = graphicsManager->LoadTexture(L"Resources/UI/M_VODKA.png");

		auto ginButtonTexture = graphicsManager->LoadTexture(L"Resources/UI/M_GIN.png");

		auto liquorButtonTexture = graphicsManager->LoadTexture(L"Resources/UI/M_LIQUEUR.png");

		auto juiceButtonTexture = graphicsManager->LoadTexture(L"Resources/UI/M_JUICE.png");

		auto sodaButtonTexture = graphicsManager->LoadTexture(L"Resources/UI/M_SODA.png");

		auto fruitButtonTexture = graphicsManager->LoadTexture(L"Resources/UI/M_FRUIT.png");

		//Gold
		auto goldTexture = graphicsManager->LoadTexture(L"Resources/UI/casher.png");

		//==================(CombinationBackGround)===============================
		auto combinationBackGroundSpriteUI = canvas->CreateSpriteUI("CombinationBackGroundSpriteUI");
		combinationBackGroundSpriteUI->SetTexture(combinationBackGroundTexture);
		combinationBackGroundSpriteUI->SetPosition(Vector3{ 0.f, 0.f, 0.5f });
		combinationBackGroundSpriteUI->SetWidth(1920.f);
		combinationBackGroundSpriteUI->SetHeight(1080.f);
		combinationBackGroundSpriteUI->SetEnable(false);

		//==================(TutorialBackGround)===============================

		tutorialTextures.push_back(tutorialBackGroundTexture);
		tutorialTextures.push_back(tutorialBackGroundTexture1);
		tutorialTextures.push_back(tutorialBackGroundTexture2);
		tutorialTextures.push_back(tutorialBackGroundTexture3);
		tutorialTextures.push_back(tutorialBackGroundTexture4);
		tutorialTextures.push_back(tutorialBackGroundTexture5);
		tutorialTextures.push_back(tutorialBackGroundTexture6);
		tutorialTextures.push_back(tutorialBackGroundTexture7);

		for (int i = 0; i < tutorialTextures.size(); i++)
		{

			std::string Idx = std::to_string(i);
			std::string uiName = "TutorialBackGroundSpriteUI" + Idx;

			auto tutorialBackGroundSpriteUI = canvas->CreateSpriteUI(uiName);
			tutorialBackGroundSpriteUI->SetTexture(tutorialTextures[i]);
			tutorialBackGroundSpriteUI->SetPosition(Vector3{ 0.f, 0.f, 0.3f });
			tutorialBackGroundSpriteUI->SetWidth(1920.f);
			tutorialBackGroundSpriteUI->SetHeight(1080.f);
			tutorialBackGroundSpriteUI->SetEnable(false);
		}

		//==================(CharactersNPC Textrue)===================================
		auto customerSpriteUI1 = canvas->CreateSpriteUI("CustomerSpriteUI1");
		customerSpriteUI1->SetTexture(customerTexture1);
		customerSpriteUI1->SetPosition(Vector3{ 0.f, -58.f, 0.4f });
		customerSpriteUI1->SetWidth(723.f);
		customerSpriteUI1->SetHeight(713.f);
		customerSpriteUI1->SetEnable(false);

		auto customerSpriteUI2 = canvas->CreateSpriteUI("CustomerSpriteUI2");
		customerSpriteUI2->SetTexture(customerTexture2);
		customerSpriteUI2->SetPosition(Vector3{ 0.f, -91.f, 0.4f });
		customerSpriteUI2->SetWidth(700.f);
		customerSpriteUI2->SetHeight(820.f);
		customerSpriteUI2->SetEnable(false);

		auto customerSpriteUI3 = canvas->CreateSpriteUI("CustomerSpriteUI3");
		customerSpriteUI3->SetTexture(customerTexture3);
		customerSpriteUI3->SetPosition(Vector3{ 0.f, -60.f, 0.4f });
		customerSpriteUI3->SetWidth(820.f);
		customerSpriteUI3->SetHeight(741.f);
		customerSpriteUI3->SetEnable(false);

		//==================(stuffButtons)===================================
		auto rumButton = canvas->CreateButtonUI("RumButton");
		rumButton->SetPosition(Vector3{ -857.f, 366.f, 0.4f });
		rumButton->SetDefaultTexture(rumButtonTexture);
		rumButton->SetWidth(109.f);
		rumButton->SetHeight(257.f);
		rumButton->SetEnable(false);

		auto vodkaButton = canvas->CreateButtonUI("VodkaButton");
		vodkaButton->SetPosition(Vector3{ -661.f, 389.f, 0.4f });
		vodkaButton->SetDefaultTexture(vodkaButtonTexture);
		vodkaButton->SetWidth(113.f);
		vodkaButton->SetHeight(212.f);
		vodkaButton->SetEnable(false);

		auto ginButton = canvas->CreateButtonUI("GinButton");
		ginButton->SetPosition(Vector3{ -481.f, 373.f, 0.4f });
		ginButton->SetDefaultTexture(ginButtonTexture);
		ginButton->SetWidth(111.f);
		ginButton->SetHeight(243.f);
		ginButton->SetEnable(false);

		auto liquorButton = canvas->CreateButtonUI("LiquorButton");
		liquorButton->SetPosition(Vector3{ -298.f, 375.f, 0.4f });
		liquorButton->SetDefaultTexture(liquorButtonTexture);
		liquorButton->SetWidth(85.f);
		liquorButton->SetHeight(239.f);
		liquorButton->SetEnable(false);

		auto juiceButton = canvas->CreateButtonUI("JuiceButton");
		juiceButton->SetPosition(Vector3{ -112.f, 422.f, 0.4f });
		juiceButton->SetDefaultTexture(juiceButtonTexture);
		juiceButton->SetWidth(106.f);
		juiceButton->SetHeight(145.f);
		juiceButton->SetEnable(false);

		auto sodaButton = canvas->CreateButtonUI("SodaButton");
		sodaButton->SetPosition(Vector3{ 75.f, 399.f, 0.4f });
		sodaButton->SetDefaultTexture(sodaButtonTexture);
		sodaButton->SetWidth(86.f);
		sodaButton->SetHeight(191.f);
		sodaButton->SetEnable(false);

		auto fruitButton = canvas->CreateButtonUI("FruitButton");
		fruitButton->SetPosition(Vector3{ 231.f, 424.f, 0.4f });
		fruitButton->SetDefaultTexture(fruitButtonTexture);
		fruitButton->SetWidth(123.f);
		fruitButton->SetHeight(141.f);
		fruitButton->SetEnable(false);


		//==================(ReSet & MixButton)===================================
		auto mixButton = canvas->CreateButtonUI("MixButton");
		mixButton->SetPosition(Vector3{ 840.f, 483.f, 0.4f });
		mixButton->SetDefaultTexture(mixbuttonTexture);
		mixButton->SetWidth(163.f);
		mixButton->SetHeight(116.f);
		mixButton->SetEnable(false);

		auto reSetButton = canvas->CreateButtonUI("ReSetButton");
		reSetButton->SetPosition(Vector3{ 470.f, 483.f, 0.4f });
		reSetButton->SetDefaultTexture(reSetbuttonTexture);
		reSetButton->SetWidth(163.f);
		reSetButton->SetHeight(116.f);
		reSetButton->SetEnable(false);


		auto goldSpriteUI = canvas->CreateSpriteUI("GoldSpriteUI");
		goldSpriteUI->SetTexture(goldTexture);
		goldSpriteUI->SetPosition(Vector3{ 788.f, 363.f, 0.4f });
		goldSpriteUI->SetWidth(250.f);
		goldSpriteUI->SetHeight(61.f);
		goldSpriteUI->SetEnable(false);


		auto goldText = canvas->CreateTextUI("GoldText");
		std::wstring goldTextString = L"";
		goldText->SetPosition(Vector3{ 844.f, 360.f, 0.4f });
		goldText->SetText(goldTextString);
		goldText->SetFont("NanumSquareRoundBold");
		goldText->SetAlignment(TextAlignment::LEFT);
		goldText->SetColor(Vector4{ powf((224.f / 255.f), 2.2f), powf((220.f / 255.f), 2.2f), powf((215.f / 255.f), 2.2f), 1.f });
		goldText->SetFontSize(24.f);
		goldText->SetEnable(false);

		//==================(BeerMaChine)===================================
		auto beerMaChineButton = canvas->CreateButtonUI("BeerMaChineButton");
		beerMaChineButton->SetPosition(Vector3{ 656.f, 483.f, 0.4f });
		beerMaChineButton->SetDefaultTexture(beerMaChineTexture);
		beerMaChineButton->SetWidth(163.f);
		beerMaChineButton->SetHeight(116.f);
		beerMaChineButton->SetEnable(false);


		//==================(RecipeBackGround & Recipes)===================================
		auto recipeBackGroundSpriteUI = canvas->CreateSpriteUI("RecipeBackGroundSpriteUI");
		recipeBackGroundSpriteUI->SetTexture(recipeBackGroundTexture);
		recipeBackGroundSpriteUI->SetPosition(Vector3{ -733.f, -168.f, 0.4f });
		recipeBackGroundSpriteUI->SetWidth(491.f);
		recipeBackGroundSpriteUI->SetHeight(849.f);
		recipeBackGroundSpriteUI->SetEnable(true);

		//==================(CocktailrecipeButton)===================================
		auto cocktailRecipeButton = canvas->CreateButtonUI("CocktailRecipeButton");
		cocktailRecipeButton->SetPosition(Vector3{ -885.f, 193.f, 0.4f });
		cocktailRecipeButton->SetDefaultTexture(cocktailrecipeTexture);
		cocktailRecipeButton->SetWidth(90.f);
		cocktailRecipeButton->SetHeight(90.f);
		cocktailRecipeButton->SetEnable(true);

		// 레시피 버튼들
		auto rumRecipeButton = canvas->CreateButtonUI("RumRecipeButton");
		rumRecipeButton->SetPosition(Vector3{ -824.f, -450.f, 0.4f });
		rumRecipeButton->SetDefaultTexture(rumRecipeButtonTexture);
		rumRecipeButton->SetWidth(57.f);
		rumRecipeButton->SetHeight(24.f);
		rumRecipeButton->SetEnable(true);

		auto vodkaRecipeButton = canvas->CreateButtonUI("VodkaRecipeButton");
		vodkaRecipeButton->SetPosition(Vector3{ -727.f, -450.f, 0.4f });
		vodkaRecipeButton->SetDefaultTexture(vodkaRecipeButtonTexture);
		vodkaRecipeButton->SetWidth(82.f);
		vodkaRecipeButton->SetHeight(24.f);
		vodkaRecipeButton->SetEnable(true);

		auto ginRecipeButton = canvas->CreateButtonUI("GinRecipeButton");
		ginRecipeButton->SetPosition(Vector3{ -636.f, -450.f, 0.4f });
		ginRecipeButton->SetDefaultTexture(ginRecipeButtonTexture);
		ginRecipeButton->SetWidth(43.f);
		ginRecipeButton->SetHeight(24.f);
		ginRecipeButton->SetEnable(true);

		// 레시피들 UI
		auto rumRecipeSpriteUI = canvas->CreateSpriteUI("RumRecipeSpriteUI");
		rumRecipeSpriteUI->SetTexture(rumRecipeTexture);
		rumRecipeSpriteUI->SetPosition(Vector3{ -770.f, -170.f, 0.4f });
		rumRecipeSpriteUI->SetWidth(290.f);
		rumRecipeSpriteUI->SetHeight(487.f);
		rumRecipeSpriteUI->SetEnable(true);

		auto vodkaRecipeSpriteUI = canvas->CreateSpriteUI("VodkaRecipeSpriteUI");
		vodkaRecipeSpriteUI->SetTexture(vodkaRecipeTexture);
		vodkaRecipeSpriteUI->SetPosition(Vector3{ -770.f, -168.f, 0.4f });
		vodkaRecipeSpriteUI->SetWidth(290.f);
		vodkaRecipeSpriteUI->SetHeight(487.f);
		vodkaRecipeSpriteUI->SetEnable(false);

		auto ginRecipeSpriteUI = canvas->CreateSpriteUI("GinRecipeSpriteUI");
		ginRecipeSpriteUI->SetTexture(ginRecipeTexture);
		ginRecipeSpriteUI->SetPosition(Vector3{ -770.f, -168.f, 0.4f });
		ginRecipeSpriteUI->SetWidth(290.f);
		ginRecipeSpriteUI->SetHeight(487.f);
		ginRecipeSpriteUI->SetEnable(false);


		auto informationBackGround = canvas->CreateSpriteUI("InformationBackGround");
		informationBackGround->SetTexture(informationBackGroundTexture);
		informationBackGround->SetPosition(Vector3{ 0.0f, 0.0f, 0.4f });
		informationBackGround->SetWidth(100.f);
		informationBackGround->SetHeight(100.f);
		informationBackGround->SetEnable(false);



		// 칵테일의 정보 띄우는 UI
		for (int i = 0; i < 8; i++)
		{
			int idx = i;
			std::string buttonIdx = to_string(idx);
			std::string textIdx = to_string(idx);

			auto mixRumRecipeButton = canvas->CreateButtonUI("RumInformationButton" + buttonIdx);
			//mixRumRecipeButton->SetPosition(Vector3{ -719.f, -393.f, 0.4f });
			mixRumRecipeButton->SetDefaultTexture(informationButtonTexture);
			mixRumRecipeButton->SetWidth(410.f);
			mixRumRecipeButton->SetHeight(67.f);
			mixRumRecipeButton->SetEnable(true);

			switch (idx)
			{
			case 0:
				mixRumRecipeButton->SetPosition(Vector3{ -719.f, -390.f, 0.4f });
				break;
			case 1:
				mixRumRecipeButton->SetPosition({ -719.f, -328.f, 0.4f });
				break;
			case 2:
				mixRumRecipeButton->SetPosition({ -719.f, -267.f, 0.4f });
				break;
			case 3:
				mixRumRecipeButton->SetPosition({ -719.f, -206.f, 0.4f });
				break;
			case 4:
				mixRumRecipeButton->SetPosition({ -719.f, -144.f, 0.4f });
				break;
			case 5:
				mixRumRecipeButton->SetPosition({ -719.f, -81.f, 0.4f });
				break;
			case 6:
				mixRumRecipeButton->SetPosition({ -719.f, -19.f, 0.4f });
				break;
			case 7:
				mixRumRecipeButton->SetPosition({ -719.f, 43.f, 0.4f });
				break;
			default:
				break;
			}

		}


		// 칵테일의 정보 띄우는 UI
		for (int i = 0; i < 8; i++)
		{
			int idx = i;
			std::string buttonIdx = to_string(idx);

			auto VodcaInformationButton = canvas->CreateButtonUI("VodcaInformationButton" + buttonIdx);
			VodcaInformationButton->SetPosition(Vector3{ -719.f, -393.f, 0.4f });
			VodcaInformationButton->SetDefaultTexture(informationButtonTexture);
			VodcaInformationButton->SetWidth(410.f);
			VodcaInformationButton->SetHeight(67.f);
			VodcaInformationButton->SetEnable(false);

			switch (idx)
			{
			case 0:
				VodcaInformationButton->SetPosition(Vector3{ -719.f, -390.f, 0.4f });
				break;
			case 1:
				VodcaInformationButton->SetPosition({ -719.f, -328.f, 0.4f });
				break;
			case 2:
				VodcaInformationButton->SetPosition({ -719.f, -267.f, 0.4f });
				break;
			case 3:
				VodcaInformationButton->SetPosition({ -719.f, -206.f, 0.4f });
				break;
			case 4:
				VodcaInformationButton->SetPosition({ -719.f, -144.f, 0.4f });
				break;
			case 5:
				VodcaInformationButton->SetPosition({ -719.f, -81.f, 0.4f });
				break;
			case 6:
				VodcaInformationButton->SetPosition({ -719.f, -19.f, 0.4f });
				break;
			case 7:
				VodcaInformationButton->SetPosition({ -719.f, 43.f, 0.4f });
				break;
			default:
				break;
			}

		}

		// 칵테일의 정보 띄우는 UI
		for (int i = 0; i < 8; i++)
		{
			int idx = i;
			std::string buttonIdx = to_string(idx);


			auto ginInformationButton = canvas->CreateButtonUI("GinInformationButton" + buttonIdx);
			ginInformationButton->SetPosition(Vector3{ -719.f, -393.f, 0.4f });
			ginInformationButton->SetDefaultTexture(informationButtonTexture);
			ginInformationButton->SetWidth(410.f);
			ginInformationButton->SetHeight(67.f);
			ginInformationButton->SetEnable(false);

			switch (idx)
			{
			case 0:
				ginInformationButton->SetPosition(Vector3{ -719.f, -390.f, 0.4f });
				break;
			case 1:
				ginInformationButton->SetPosition({ -719.f, -328.f, 0.4f });
				break;
			case 2:
				ginInformationButton->SetPosition({ -719.f, -267.f, 0.4f });
				break;
			case 3:
				ginInformationButton->SetPosition({ -719.f, -206.f, 0.4f });
				break;
			case 4:
				ginInformationButton->SetPosition({ -719.f, -144.f, 0.4f });
				break;
			case 5:
				ginInformationButton->SetPosition({ -719.f, -81.f, 0.4f });
				break;
			case 6:
				ginInformationButton->SetPosition({ -719.f, -19.f, 0.4f });
				break;
			case 7:
				ginInformationButton->SetPosition({ -719.f, 43.f, 0.4f });
				break;
			default:
				break;
			}

		}




		auto informationText = canvas->CreateTextUI("InformationText");
		informationText->SetPosition(Vector3{ -321.f, -393.f, 0.4f });
		informationText->SetText(L"");
		informationText->SetFont("NanumSquareRoundRegular");
		informationText->SetFontSize(18.f);
		informationText->SetFixedWidth(250);
		informationText->SetEnable(true);

		auto informationText2 = canvas->CreateTextUI("InformationText2");
		informationText2->SetPosition(Vector3{ -321.f, -393.f, 0.4f });
		informationText2->SetText(L"");
		informationText2->SetFont("NanumSquareRoundRegular");
		informationText2->SetFontSize(18.f);
		informationText2->SetFixedWidth(250);
		informationText2->SetEnable(true);

		auto informationText3 = canvas->CreateTextUI("InformationText3");
		informationText3->SetPosition(Vector3{ -321.f, -393.f, 0.4f });
		informationText3->SetText(L"");
		informationText3->SetFont("NanumSquareRoundRegular");
		informationText3->SetFontSize(18.f);
		informationText3->SetFixedWidth(250);
		informationText3->SetEnable(true);


		// stuff amount
		for (int i = 0; i < 7; i++)
		{
			std::string amountIdx = to_string(i);

			auto amountText = canvas->CreateTextUI("AmountText" + amountIdx);

			amountText->SetPosition(Vector3{ -775.0f, 334.0f, 0.4f });
			amountText->SetText("0");
			amountText->SetFont("NanumSquareRoundBold");
			amountText->SetColor(Vector4{ powf((101.f / 255.f), 2.2f), powf((67.f / 255.f), 2.2f), powf((33.f / 255.f), 2.2f), 1.f });
			amountText->SetFontSize(30.f);
			amountText->SetEnable(false);
		}


		//==================(SoldOutButton)===================================
		auto soldOutButton = canvas->CreateButtonUI("SoldOutButton");
		soldOutButton->SetPosition(Vector3{ 556.f, 366.f, 0.4f });
		soldOutButton->SetDefaultTexture(buttonTexture);
		soldOutButton->SetWidth(130.f);
		soldOutButton->SetHeight(60.f);
		soldOutButton->SetEnable(false);

		canvas->SetEnable(false);
	}

	void UICreator::CreateSettingCanvas()
	{
		//==================(Canvas & LoadTexture)============================
		auto canvas = mainCanvas->GetCanvasUI("SettingCanvase");

		auto pauseBackGroundtexture = graphicsManager->LoadTexture(L"Resources/UI/PauseBG.png");

		auto phoneBackTexture = graphicsManager->LoadTexture(L"Resources/UI/MenuClosebuttonHover.png");

		auto settingBackgroundtexture = graphicsManager->LoadTexture(L"Resources/UI/SettingBackGroundUI.png");

		auto menuBackTexture = graphicsManager->LoadTexture(L"Resources/UI/phone.png");

		auto closebuttonTexture = graphicsManager->LoadTexture(L"Resources/UI/ClosebuttonHover.png");

		auto soundbarBGTexture = graphicsManager->LoadTexture(L"Resources/UI/soundbar.png");

		auto barTexture = graphicsManager->LoadTexture(L"Resources/UI/BarBg.png");

		auto leftTexture = graphicsManager->LoadTexture(L"Resources/UI/left.png");
		auto leftHover = graphicsManager->LoadTexture(L"Resources/UI/leftHover.png");

		auto rightTexture = graphicsManager->LoadTexture(L"Resources/UI/right.png");
		auto rightHover = graphicsManager->LoadTexture(L"Resources/UI/rightHover.png");

		auto volumeSettingButtonTexture = graphicsManager->LoadTexture(L"Resources/UI/NotUI.png");

		auto  pauseBackGroundSpriteUI = canvas->CreateButtonUI("PauseBackGroundSpriteUI");
		pauseBackGroundSpriteUI->SetDefaultTexture(pauseBackGroundtexture);
		pauseBackGroundSpriteUI->SetPosition(Vector3{ 0.0f, 0.0f, 0.05f });
		pauseBackGroundSpriteUI->SetWidth(1920.0f);
		pauseBackGroundSpriteUI->SetHeight(1080.0f);
		pauseBackGroundSpriteUI->SetHoverMaskColor(1.0f, 1.0f, 1.0f, 1.0f);
		pauseBackGroundSpriteUI->SetEnable(false);

		//==================(SettingBackground)===============================
		auto  settingBackgroundSpriteUI = canvas->CreateSpriteUI("SettingBackgroundSpriteUI");
		settingBackgroundSpriteUI->SetTexture(settingBackgroundtexture);
		settingBackgroundSpriteUI->SetPosition(Vector3{ 46.f, 38.f, 0.05f });
		settingBackgroundSpriteUI->SetWidth(820.0f);
		settingBackgroundSpriteUI->SetHeight(1023.0f);

		//==================(GraphicsButton)======================================
		auto windowSizeText = canvas->CreateTextUI("WindowSizeText");
		windowSizeText->SetPosition(Vector3{ 115.f, 83.f, 0.05f });
		windowSizeText->SetText(std::to_wstring(windowSize[sizeIdx].first) + L" X " + std::to_wstring(windowSize[sizeIdx].second));
		windowSizeText->SetFontSize(30.f);

		auto graphicsLeftButton = canvas->CreateButtonUI("GraphicsLeftButton");
		graphicsLeftButton->SetPosition(Vector3{ -78.f, 90.f, 0.049f });
		graphicsLeftButton->SetDefaultTexture(leftTexture);
		graphicsLeftButton->SetHoverTexture(leftHover);
		graphicsLeftButton->SetWidth(27.f);
		graphicsLeftButton->SetHeight(45.f);
		// 왼쪽을 누르면 해상도가 낮아집니다.
		graphicsLeftButton->SetClickEvent([&]
		{
			// 기존에 최대였으면 right를 가능하게 해줍니다.
			auto canvas = UIManager::GetInstance()->GetCanvas("SettingCanvase");

			if(sizeIdx == 3)
				canvas->GetButtonUI("GraphicsRightButton")->SetButtonEnable(true);

			// 한 단계 낮추고
			sizeIdx--;
			if(sizeIdx == 0)
				canvas->GetButtonUI("GraphicsLeftButton")->SetButtonEnable(false);

			if(sizeIdx == 2)
			{
				SetWindowLongPtr(WindowManager::GetInstance()->GetHWND(), GWL_STYLE, WS_VISIBLE | WS_OVERLAPPEDWINDOW);
				SetWindowPos(WindowManager::GetInstance()->GetHWND(), nullptr, 100, 100, windowSize[sizeIdx].first, windowSize[sizeIdx].second, SWP_FRAMECHANGED | SWP_NOZORDER);
			}
			else
				SetWindowPos(WindowManager::GetInstance()->GetHWND(), nullptr, 0, 0, windowSize[sizeIdx].first, windowSize[sizeIdx].second, SWP_NOMOVE | SWP_NOZORDER);

			canvas->GetTextUI("WindowSizeText")->SetText(std::to_wstring(windowSize[sizeIdx].first) + L" X " + std::to_wstring(windowSize[sizeIdx].second));
		});

		auto graphicsRightButton = canvas->CreateButtonUI("GraphicsRightButton");
		graphicsRightButton->SetPosition(Vector3{ 250.f, 90.f, 0.049f });
		graphicsRightButton->SetDefaultTexture(rightTexture);
		graphicsRightButton->SetHoverTexture(rightHover);
		graphicsRightButton->SetWidth(27.f);
		graphicsRightButton->SetHeight(45.f);
		// 오른 쪽을 누르면 해상도가 낮아집니다.
		graphicsRightButton->SetClickEvent([&]
			{
				// 기존에 최소였으면 left를 가능하게 해줍니다.
				auto canvas = UIManager::GetInstance()->GetCanvas("SettingCanvase");

				if (sizeIdx == 0)
					canvas->GetButtonUI("GraphicsLeftButton")->SetButtonEnable(true);

				// 한 단계 높이고
				sizeIdx++;
				if (sizeIdx == 3)
					canvas->GetButtonUI("GraphicsRightButton")->SetButtonEnable(false);

				if (sizeIdx == 3)
				{
					SetWindowLongPtr(WindowManager::GetInstance()->GetHWND(), GWL_STYLE, WS_VISIBLE | WS_POPUP);
					SetWindowPos(WindowManager::GetInstance()->GetHWND(), nullptr, 0, 0, windowSize[sizeIdx].first, windowSize[sizeIdx].second, SWP_FRAMECHANGED | SWP_NOZORDER);
					canvas->GetTextUI("WindowSizeText")->SetText(L"Borderless");
				}
				else
				{
					SetWindowPos(WindowManager::GetInstance()->GetHWND(), nullptr, 0, 0, windowSize[sizeIdx].first, windowSize[sizeIdx].second, SWP_NOMOVE | SWP_NOZORDER);
					canvas->GetTextUI("WindowSizeText")->SetText(std::to_wstring(windowSize[sizeIdx].first) + L" X " + std::to_wstring(windowSize[sizeIdx].second));
				}

			});


		//==================(MusicButton)======================================
		// 이거 프로그레스 바로 변경 해야함
		auto bgmVolumeText = canvas->CreateTextUI("BgmVolumeText");
		std::wstring bgmVolumeTextString = L"BgmVolume";
		bgmVolumeText->SetPosition(Vector3{ -164.f, -122.f, 0.05f });
		bgmVolumeText->SetText(bgmVolumeTextString);
		bgmVolumeText->SetEnable(false);
		bgmVolumeText->SetFontSize(19.f);

		auto volumeSettingProgressbar = canvas->CreateProgressBarUI("VolumeSettingProgressbar");
		//volumeSettingProgressbar->SetBackgroundTexture(soundbarBGTexture);
		volumeSettingProgressbar->SetBarTexture(soundbarBGTexture);
		volumeSettingProgressbar->SetFillDirection(FillDir::RIGHT);
		volumeSettingProgressbar->SetWidth(420.f);
		volumeSettingProgressbar->SetHeight(16.f);
		volumeSettingProgressbar->SetPosition(Vector3{ 85.f, -116.f, 0.05f });
		volumeSettingProgressbar->SetBarTexRect(0.f, 0.f, 430.f, 40.f);
		volumeSettingProgressbar->SetFillPercent(1.0f);
		
		
		auto volumeSettingButton = canvas->CreateButtonUI("VolumeSettingButton");
		volumeSettingButton->SetPosition(Vector3{ 85.f, -116.f, 0.04f });
		volumeSettingButton->SetDefaultTexture(volumeSettingButtonTexture);
		volumeSettingButton->SetWidth(430.f);
		volumeSettingButton->SetHeight(40.f);
		



		/*auto bgmVolumeTexture = canvas->CreateSpriteUI("BgmVolumeTexture");
		bgmVolumeTexture->SetTexture(soundbarBGTexture);
		bgmVolumeTexture->SetPosition(Vector3{ 85.f, -116.f, 0.05f });
		bgmVolumeTexture->SetWidth(420.f);
		bgmVolumeTexture->SetHeight(16.f);
		bgmVolumeTexture->SetEnable(true);
		bgmVolumeTexture->SetSrcOffset();*/

		//==================(EtcButton)======================================
		auto languageLeftButton = canvas->CreateButtonUI("LanguageLeftButton");
		languageLeftButton->SetPosition(Vector3{ -78.f, 335.f, 0.049f });
		languageLeftButton->SetDefaultTexture(leftTexture);
		languageLeftButton->SetHoverTexture(leftHover);
		languageLeftButton->SetWidth(27.f);
		languageLeftButton->SetHeight(45.f);
		languageLeftButton->SetEnable(true);

		auto languageRightButton = canvas->CreateButtonUI("LanguageRightButton");
		languageRightButton->SetPosition(Vector3{ 250.f, 335.f, 0.049f });
		languageRightButton->SetDefaultTexture(rightTexture);
		languageRightButton->SetHoverTexture(rightHover);
		languageRightButton->SetWidth(27.f);
		languageRightButton->SetHeight(45.f);
		languageRightButton->SetEnable(true);

		auto koreanText = canvas->CreateTextUI("KoreanText");
		std::wstring koreanTextString = L"Korean";
		koreanText->SetPosition(Vector3{ 111.f, 332.f, 0.05f });
		koreanText->SetText(koreanTextString);
		koreanText->SetFontSize(30.f);
		koreanText->SetEnable(true);

		auto englishText = canvas->CreateTextUI("EnglishText");
		std::wstring englishTextString = L"English";
		englishText->SetPosition(Vector3{ 127.f, 332.f, 0.05f });
		englishText->SetText(englishTextString);
		englishText->SetFontSize(30.f);
		englishText->SetEnable(false);

		//==================(CloseButton)======================================
		auto  settingPhoneBackButton = canvas->CreateButtonUI("SettingPhoneBackButton");
		settingPhoneBackButton->SetPosition(Vector3{ 100.0f, 460.0f, 0.049f });
		settingPhoneBackButton->SetDefaultTexture(phoneBackTexture);
		settingPhoneBackButton->SetDefaultMaskColor(0.5f, 0.5f, 0.5f, 1.0f);
		settingPhoneBackButton->SetHoverMaskColor(1.0f, 1.0f, 1.0f, 1.0f);
		settingPhoneBackButton->SetWidth(37.f);
		settingPhoneBackButton->SetHeight(36.f);


		auto closeButton = canvas->CreateButtonUI("SettingCloseButtonButton");
		closeButton->SetPosition(Vector3{ 270.f, 458.f, 0.049f });
		closeButton->SetDefaultTexture(closebuttonTexture);
		closeButton->SetDefaultMaskColor(0.5f, 0.5f, 0.5f, 1.0f);
		closeButton->SetHoverMaskColor(1.0f, 1.0f, 1.0f, 1.0f);
		closeButton->SetWidth(13.f);
		closeButton->SetHeight(32.f);

		canvas->SetEnable(false);
	}

	void UICreator::CreateCreditCanvas()
	{
		//==================(Canvas & LoadTexture)============================
		auto canvas = mainCanvas->GetCanvasUI("CreditCanvas");

		auto credittexture = graphicsManager->LoadTexture(L"Resources/UI/Credit.png");

		auto closebuttonTexture = graphicsManager->LoadTexture(L"Resources/UI/NotUI.png");

		//==================(SettingBackground)===============================
		auto  creditSpriteUI = canvas->CreateSpriteUI("CreditSpriteUI");
		creditSpriteUI->SetTexture(credittexture);
		creditSpriteUI->SetPosition(Vector3{ 0.f, 0.f, 0.3f });
		creditSpriteUI->SetWidth(1920.f);
		creditSpriteUI->SetHeight(5000.f);

		auto creditCloseButton = canvas->CreateButtonUI("CreditCloseButton");
		creditCloseButton->SetPosition(Vector3{ 0.0f, 0.0f, 0.3f });
		creditCloseButton->SetDefaultTexture(closebuttonTexture);
		/*creditCloseButton->SetDefaultMaskColor(0.5f, 0.5f, 0.5f, 1.0f);
		creditCloseButton->SetHoverMaskColor(1.0f, 1.0f, 1.0f, 1.0f);*/
		creditCloseButton->SetWidth(1920.f);
		creditCloseButton->SetHeight(5000.f);

		canvas->SetEnable(false);

		///////////////////////////////////////////////////////////////////////////////////////
		auto jwcanvas = mainCanvas->GetCanvasUI("JWCreditCanvas");

		auto jwcredittexture = graphicsManager->LoadTexture(L"Resources/UI/Credit.png");

		auto jwclosebuttonTexture = graphicsManager->LoadTexture(L"Resources/UI/NotUI.png");

		auto  jwcreditSpriteUI = jwcanvas->CreateSpriteUI("CreditSpriteUI");
		jwcreditSpriteUI->SetTexture(jwcredittexture);
		jwcreditSpriteUI->SetPosition(Vector3{ 0.f, 0.f, 0.3f });
		jwcreditSpriteUI->SetWidth(1920.f);
		jwcreditSpriteUI->SetHeight(5000.f);
		jwcanvas->SetEnable(false);
	}

	void UICreator::CreateGameHUD()
	{
		auto canvas = mainCanvas->GetCanvasUI("InGameCanvas");

		//auto HPBlankTex = graphicsManager->LoadTexture(L"Resources/UI/Hud/HP_BG.png");
		//auto HPTex = graphicsManager->LoadTexture(L"Resources/UI/Hud/HP.png");

		//auto character = graphicsManager->LoadTexture(L"Resources/UI/Character1.png");

		//auto boss = graphicsManager->LoadTexture(L"Resources/UI/Boss.png");

		//auto hpTexture = graphicsManager->LoadTexture(L"Resources/UI/HP_Bar2.png");

		//auto hpBackGroundTexture = graphicsManager->LoadTexture(L"Resources/UI/HP_Bar_BG2.png");

		//auto goldTexture = graphicsManager->LoadTexture(L"Resources/UI/Gold.png");

		//==================(InGameHud: Character)===============================
		/*auto  characterSpriteUI = canvas->CreateSpriteUI("Character");
		characterSpriteUI->SetTexture(character);
		characterSpriteUI->SetPosition(Vector3{ -887.f, 455.f, 0.4f });
		characterSpriteUI->SetWidth(146.f);
		characterSpriteUI->SetHeight(164.f);
		characterSpriteUI->SetEnable(true);*/

		/*auto hpBarProgressBarUI = canvas->CreateProgressBarUI("HpBar");
		hpBarProgressBarUI->SetBackgroundTexture(hpBackGroundTexture);
		hpBarProgressBarUI->SetBarTexture(hpTexture);
		hpBarProgressBarUI->SetFillDirection(FillDir::RIGHT);
		hpBarProgressBarUI->SetWidth(300.f);
		hpBarProgressBarUI->SetHeight(34.f);
		hpBarProgressBarUI->SetPosition(Vector3{ -716.f, 517.f, 0.4f });
		hpBarProgressBarUI->SetBarTexRect(0.f, 0.f, 470.f, 71.f);
		hpBarProgressBarUI->SetFillPercent(1.0f);
		hpBarProgressBarUI->SetEnable(true);*/

		// HP를 최대 체력에 맞게 만듭니다.
			// 현 시점에서는 8개 입니다.

		// 텍스쳐는 컨트롤러에서 바꿔줍니다.
		for (int i = 0; i < 10; i++)
		{
			std::string idx = to_string(i);
			auto HPSprite = canvas->CreateSpriteUI("HP" + idx);
			HPSprite->SetPosition(Vector3{ -902.f + (i * 100), -480.f, 0.4f });
			HPSprite->SetWidth(80.f);
			HPSprite->SetHeight(80.f);
			HPSprite->SetEnable(false);
		}


		// 7개의 message Text를 만듭니다.
		std::wstring systemMessageTextString = L"";
		for (int i = 0; i < 7; i++)
		{
			string tmpName = "SystemMessageText" + std::to_string(i);
			auto systemMessageText = canvas->CreateTextUI(tmpName);
			systemMessageText->SetPosition(Vector3{ 741.f, 433.f - (i * 35.f), 0.4f });
			systemMessageText->SetText(systemMessageTextString);
			systemMessageText->SetFontSize(20.f);
			systemMessageText->SetEnable(true);
			systemMessageText->SetFont("NanumSquareRoundRegular");

			if (i == 5)
				systemMessageText->SetColor({ 1.0f, 1.0f, 1.0f, 0.5f });

			if (i == 6)
				systemMessageText->SetColor({ 1.0f, 1.0f, 1.0f, 0.25f });

		}

		auto explanationText = canvas->CreateTextUI("Explanation");
		explanationText->SetPosition(Vector3{ 0.0f, -360.0f, 0.4f });
		explanationText->SetText(L"");
		explanationText->SetFont("NanumSquareRoundRegular");
		explanationText->SetFontSize(40.f);
		explanationText->SetEnable(false);


		//==================(InGameHud:Money)===============================
		/*auto goldSpriteUI = canvas->CreateSpriteUI("Gold");
		goldSpriteUI->SetTexture(goldTexture);
		goldSpriteUI->SetPosition(Vector3{ 750.f, 502.f, 0.4f });
		goldSpriteUI->SetWidth(123.f);
		goldSpriteUI->SetHeight(72.f);
		goldSpriteUI->SetEnable(true);

		auto goldText = canvas->CreateTextUI("GoldText");
		std::wstring goldTextString = L"Gold";
		goldText->SetPosition(Vector3{ 904.f, 499.f, 0.4f });
		goldText->SetText(goldTextString);
		goldText->SetFontSize(30.f);
		goldText->SetEnable(true);*/

		canvas->SetEnable(false);
	}

	void UICreator::CreateInteractionCanvas()
	{
		auto canvas = mainCanvas->GetCanvasUI("InteractionCanvas");

		auto fTexture = graphicsManager->LoadTexture(L"Resources/UI/FButton.png");

		//==================(InteractionUI)============================== 
		auto fSpriteUI = canvas->CreateSpriteUI("F");
		fSpriteUI->SetTexture(fTexture);
		fSpriteUI->SetPosition(Vector3{ 20.0f, 470.0f, 0.4f });
		fSpriteUI->SetWidth(100.f);
		fSpriteUI->SetHeight(100.f);
		fSpriteUI->SetEnable(false);

		canvas->SetEnable(false);

	}

	void UICreator::CreateMiniGameCanvas()
	{
		auto canvas = mainCanvas->GetCanvasUI("MiniGameCanvas");

		// 재료
		auto mini_Game_rumTexture = graphicsManager->LoadTexture(L"Resources/UI/Cocktail/I_rum.png");

		auto mini_Game_vodkaTexture = graphicsManager->LoadTexture(L"Resources/UI/Cocktail/I_vodka.png");

		auto mini_Game_ginTexture = graphicsManager->LoadTexture(L"Resources/UI/Cocktail/I_gin.png");

		auto mini_Game_liquorTexture = graphicsManager->LoadTexture(L"Resources/UI/Cocktail/I_liqueur.png");

		auto mini_Game_juiceTexture = graphicsManager->LoadTexture(L"Resources/UI/Cocktail/I_juice.png");

		auto mini_Game_sodaTexture = graphicsManager->LoadTexture(L"Resources/UI/Cocktail/I_soda.png");

		auto mini_Game_fruitTexture = graphicsManager->LoadTexture(L"Resources/UI/Cocktail/I_fruit.png");

		//rum mix texture
		auto mixRumTexture = graphicsManager->LoadTexture(L"Resources/UI/Cocktail/RUM.png");

		auto rumCokeTexture = graphicsManager->LoadTexture(L"Resources/UI/Cocktail/Rumcock.png");

		auto cubaLibreTexture = graphicsManager->LoadTexture(L"Resources/UI/Cocktail/CUBALIBRE.png");

		auto daiquiriTexture = graphicsManager->LoadTexture(L"Resources/UI/Cocktail/Daiqkiri.png");

		auto painKillerTexture = graphicsManager->LoadTexture(L"Resources/UI/Cocktail/PAINKILLER.png");

		auto maiTaiTexture = graphicsManager->LoadTexture(L"Resources/UI/Cocktail/MAITAI.png");

		auto plantersPunchTexture = graphicsManager->LoadTexture(L"Resources/UI/Cocktail/Planter's punch.png");

		auto bushwackerTexture = graphicsManager->LoadTexture(L"Resources/UI/Cocktail/BUSHWACKER.png");

		// vodka mix texture 
		auto mixVodkaTexture = graphicsManager->LoadTexture(L"Resources/UI/Cocktail/Vodka.png");

		auto espressoTiniTexture = graphicsManager->LoadTexture(L"Resources/UI/Cocktail/ESPRESSOTINI.png");

		auto mosCowmuleTexture = graphicsManager->LoadTexture(L"Resources/UI/Cocktail/MOSCOWMULE.png");

		auto blackRussianTexture = graphicsManager->LoadTexture(L"Resources/UI/Cocktail/BLACKRUSSIAN.png");

		auto cosmoPolitanTexture = graphicsManager->LoadTexture(L"Resources/UI/Cocktail/COSMOPOLITAN.png");

		auto bloodyMaryTexture = graphicsManager->LoadTexture(L"Resources/UI/Cocktail/BLOODYMARY.png");

		auto poetKillerTexture = graphicsManager->LoadTexture(L"Resources/UI/Cocktail/POETKILLER.png");

		auto sunRiseTexture = graphicsManager->LoadTexture(L"Resources/UI/Cocktail/SUNRISE.png");

		// gin Mix texture
		auto mixGinTexture = graphicsManager->LoadTexture(L"Resources/UI/Cocktail/Gin.png");

		auto ginFizzTexture = graphicsManager->LoadTexture(L"Resources/UI/Cocktail/GINFIZZ.png");

		auto ginTonicTexture = graphicsManager->LoadTexture(L"Resources/UI/Cocktail/GINTONIC.png");

		auto aviationTexture = graphicsManager->LoadTexture(L"Resources/UI/Cocktail/Aviatoin.png");

		auto ladykillerTexture = graphicsManager->LoadTexture(L"Resources/UI/Cocktail/LADYKILLER.png");

		auto brambleTexture = graphicsManager->LoadTexture(L"Resources/UI/Cocktail/BRAMBLE.png");

		auto lastWordTexture = graphicsManager->LoadTexture(L"Resources/UI/Cocktail/LASTWORD.png");

		auto snapperTexture = graphicsManager->LoadTexture(L"Resources/UI/Cocktail/Snapper.png");

		// 기타 버튼 등등..
		auto offerButtonTexture = graphicsManager->LoadTexture(L"Resources/UI/OfferButton.png");

		auto miniGameTexture = graphicsManager->LoadTexture(L"Resources/UI/Mini_game.png");

		auto miniGameShakerTexture = graphicsManager->LoadTexture(L"Resources/UI/Mini_game3.png");

		auto mixResultTexture = graphicsManager->LoadTexture(L"Resources/UI/Mini_game_MixResult.png");

		auto miniGameArrowTexture = graphicsManager->LoadTexture(L"Resources/UI/Mini_game_cursor.png");

		auto miniGameHpBackground = graphicsManager->LoadTexture(L"Resources/UI/Mini_game_barBG.png");

		auto miniGameHp = graphicsManager->LoadTexture(L"Resources/UI/Mini_game_bar.png");

		auto fTexture = graphicsManager->LoadTexture(L"Resources/UI/FButton.png");

		//==================(MiniGameBackGround)===================================
		auto miniGameSpriteUI = canvas->CreateSpriteUI("MiniGameSpriteUI");
		miniGameSpriteUI->SetTexture(miniGameTexture);
		miniGameSpriteUI->SetPosition(Vector3{ 0.f, 0.f, 0.3f });
		miniGameSpriteUI->SetWidth(1920.f);
		miniGameSpriteUI->SetHeight(1080.f);
		miniGameSpriteUI->SetEnable(false);

		auto miniGameShakerSpriteUI = canvas->CreateSpriteUI("MiniGameShakerSpriteUI");
		miniGameShakerSpriteUI->SetTexture(miniGameShakerTexture);
		miniGameShakerSpriteUI->SetPosition(Vector3{ 4.f, -40.f, 0.3f });
		miniGameShakerSpriteUI->SetWidth(300.f);
		miniGameShakerSpriteUI->SetHeight(290.f);
		miniGameShakerSpriteUI->SetEnable(false);

		auto miniGameArrowUI = canvas->CreateSpriteUI("MiniGameArrow");
		miniGameArrowUI->SetTexture(miniGameArrowTexture);
		miniGameArrowUI->SetPosition(Vector3{ 0.0f, 0.0f, 0.3f });
		miniGameArrowUI->SetWidth(100.f);
		miniGameArrowUI->SetHeight(100.f);
		miniGameArrowUI->SetEnable(false);

		auto miniGameHpUI = canvas->CreateProgressBarUI("MiniGameHp");
		miniGameHpUI->SetBackgroundTexture(miniGameHpBackground);
		miniGameHpUI->SetBarTexture(miniGameHp);
		miniGameHpUI->SetFillDirection(FillDir::UP);
		miniGameHpUI->SetWidth(68.f);
		miniGameHpUI->SetHeight(861.f);
		miniGameHpUI->SetPosition(Vector3{ 657.f, 4.f, 0.3f });
		miniGameHpUI->SetBarTexRect(0.f, 0.f, 68.f, 861.f);
		miniGameHpUI->SetFillPercent(1.0f);
		miniGameHpUI->SetEnable(false);

		// 현재 재료 순서 알려주는 재료이미지
		auto rumSpriteUI = canvas->CreateSpriteUI("RumSpriteUI");
		rumSpriteUI->SetTexture(mini_Game_rumTexture);
		rumSpriteUI->SetPosition(Vector3{ 0.0f, 0.0f, 0.3f });
		rumSpriteUI->SetWidth(35.f);
		rumSpriteUI->SetHeight(100.f);
		rumSpriteUI->SetEnable(false);

		auto rumSpriteUI2 = canvas->CreateSpriteUI("RumSpriteUI2");
		rumSpriteUI2->SetTexture(mini_Game_rumTexture);
		rumSpriteUI2->SetPosition(Vector3{ 0.0f, 0.0f, 0.3f });
		rumSpriteUI2->SetWidth(35.f);
		rumSpriteUI2->SetHeight(100.f);
		rumSpriteUI2->SetEnable(false);

		auto rumSpriteUI3 = canvas->CreateSpriteUI("RumSpriteUI3");
		rumSpriteUI3->SetTexture(mini_Game_rumTexture);
		rumSpriteUI3->SetPosition(Vector3{ 0.0f, 0.0f, 0.3f });
		rumSpriteUI3->SetWidth(35.f);
		rumSpriteUI3->SetHeight(100.f);
		rumSpriteUI3->SetEnable(false);

		auto vodkaSpriteUI = canvas->CreateSpriteUI("VodkaSpriteUI");
		vodkaSpriteUI->SetTexture(mini_Game_vodkaTexture);
		vodkaSpriteUI->SetPosition(Vector3{ 0.0f, 0.0f, 0.3f });
		vodkaSpriteUI->SetWidth(35.f);
		vodkaSpriteUI->SetHeight(89.f);
		vodkaSpriteUI->SetEnable(false);

		auto vodkaSpriteUI2 = canvas->CreateSpriteUI("VodkaSpriteUI2");
		vodkaSpriteUI2->SetTexture(mini_Game_vodkaTexture);
		vodkaSpriteUI2->SetPosition(Vector3{ 0.0f, 0.0f, 0.3f });
		vodkaSpriteUI2->SetWidth(35.f);
		vodkaSpriteUI2->SetHeight(89.f);
		vodkaSpriteUI2->SetEnable(false);

		auto vodkaSpriteUI3 = canvas->CreateSpriteUI("VodkaSpriteUI3");
		vodkaSpriteUI3->SetTexture(mini_Game_vodkaTexture);
		vodkaSpriteUI3->SetPosition(Vector3{ 0.0f, 0.0f, 0.3f });
		vodkaSpriteUI3->SetWidth(35.f);
		vodkaSpriteUI3->SetHeight(89.f);
		vodkaSpriteUI3->SetEnable(false);

		auto ginSpriteUI = canvas->CreateSpriteUI("GinSpriteUI");
		ginSpriteUI->SetTexture(mini_Game_ginTexture);
		ginSpriteUI->SetPosition(Vector3{ 0.0f, 0.0f, 0.3f });
		ginSpriteUI->SetWidth(35.f);
		ginSpriteUI->SetHeight(99.f);
		ginSpriteUI->SetEnable(false);

		auto ginSpriteUI2 = canvas->CreateSpriteUI("GinSpriteUI2");
		ginSpriteUI2->SetTexture(mini_Game_ginTexture);
		ginSpriteUI2->SetPosition(Vector3{ 0.0f, 0.0f, 0.3f });
		ginSpriteUI2->SetWidth(35.f);
		ginSpriteUI2->SetHeight(99.f);
		ginSpriteUI2->SetEnable(false);

		auto ginSpriteUI3 = canvas->CreateSpriteUI("GinSpriteUI3");
		ginSpriteUI3->SetTexture(mini_Game_ginTexture);
		ginSpriteUI3->SetPosition(Vector3{ 0.0f, 0.0f, 0.3f });
		ginSpriteUI3->SetWidth(35.f);
		ginSpriteUI3->SetHeight(99.f);
		ginSpriteUI3->SetEnable(false);

		auto liquorSpriteUI = canvas->CreateSpriteUI("LiquorSpriteUI");
		liquorSpriteUI->SetTexture(mini_Game_liquorTexture);
		liquorSpriteUI->SetPosition(Vector3{ 0.0f, 0.0f, 0.3f });
		liquorSpriteUI->SetWidth(28.f);
		liquorSpriteUI->SetHeight(96.f);
		liquorSpriteUI->SetEnable(false);

		auto liquorSpriteUI2 = canvas->CreateSpriteUI("LiquorSpriteUI2");
		liquorSpriteUI2->SetTexture(mini_Game_liquorTexture);
		liquorSpriteUI2->SetPosition(Vector3{ 0.0f, 0.0f, 0.3f });
		liquorSpriteUI2->SetWidth(28.f);
		liquorSpriteUI2->SetHeight(96.f);
		liquorSpriteUI2->SetEnable(false);

		auto liquorSpriteUI3 = canvas->CreateSpriteUI("LiquorSpriteUI3");
		liquorSpriteUI3->SetTexture(mini_Game_liquorTexture);
		liquorSpriteUI3->SetPosition(Vector3{ 0.0f, 0.0f, 0.3f });
		liquorSpriteUI3->SetWidth(28.f);
		liquorSpriteUI3->SetHeight(96.f);
		liquorSpriteUI3->SetEnable(false);


		auto juiceSpriteUI = canvas->CreateSpriteUI("JuiceSpriteUI");
		juiceSpriteUI->SetTexture(mini_Game_juiceTexture);
		juiceSpriteUI->SetPosition(Vector3{ 0.0f, 0.0f, 0.3f });
		juiceSpriteUI->SetWidth(34.f);
		juiceSpriteUI->SetHeight(63.f);
		juiceSpriteUI->SetEnable(false);

		auto juiceSpriteUI2 = canvas->CreateSpriteUI("JuiceSpriteUI2");
		juiceSpriteUI2->SetTexture(mini_Game_juiceTexture);
		juiceSpriteUI2->SetPosition(Vector3{ 0.0f, 0.0f, 0.3f });
		juiceSpriteUI2->SetWidth(34.f);
		juiceSpriteUI2->SetHeight(63.f);
		juiceSpriteUI2->SetEnable(false);

		auto juiceSprite3 = canvas->CreateSpriteUI("JuiceSpriteUI3");
		juiceSprite3->SetTexture(mini_Game_juiceTexture);
		juiceSprite3->SetPosition(Vector3{ 0.0f, 0.0f, 0.4f });
		juiceSprite3->SetWidth(34.f);
		juiceSprite3->SetHeight(63.f);
		juiceSprite3->SetEnable(false);

		auto sodaSpriteUI = canvas->CreateSpriteUI("SodaSpriteUI");
		sodaSpriteUI->SetTexture(mini_Game_sodaTexture);
		sodaSpriteUI->SetPosition(Vector3{ -0.0f, 0.0f, 0.3f });
		sodaSpriteUI->SetWidth(28.f);
		sodaSpriteUI->SetHeight(78.f);
		sodaSpriteUI->SetEnable(false);

		auto sodaSpriteUI2 = canvas->CreateSpriteUI("SodaSpriteUI2");
		sodaSpriteUI2->SetTexture(mini_Game_sodaTexture);
		sodaSpriteUI2->SetPosition(Vector3{ 0.f, 0.0f, 0.3f });
		sodaSpriteUI2->SetWidth(28.f);
		sodaSpriteUI2->SetHeight(78.f);
		sodaSpriteUI2->SetEnable(false);

		auto sodaSpriteUI3 = canvas->CreateSpriteUI("SodaSpriteUI3");
		sodaSpriteUI3->SetTexture(mini_Game_sodaTexture);
		sodaSpriteUI3->SetPosition(Vector3{ 0.0f, 0.0f, 0.3f });
		sodaSpriteUI3->SetWidth(28.f);
		sodaSpriteUI3->SetHeight(78.f);
		sodaSpriteUI3->SetEnable(false);

		auto fruitSpriteUI = canvas->CreateSpriteUI("FruitSpriteUI");
		fruitSpriteUI->SetTexture(mini_Game_fruitTexture);
		fruitSpriteUI->SetPosition(Vector3{ 0.0f, 0.0f, 0.3f });
		fruitSpriteUI->SetWidth(39.f);
		fruitSpriteUI->SetHeight(62.f);
		fruitSpriteUI->SetEnable(false);

		auto fruitSpriteUI2 = canvas->CreateSpriteUI("FruitSpriteUI2");
		fruitSpriteUI2->SetTexture(mini_Game_fruitTexture);
		fruitSpriteUI2->SetPosition(Vector3{ 0.0f, 0.0f, 0.3f });
		fruitSpriteUI2->SetWidth(39.f);
		fruitSpriteUI2->SetHeight(62.f);
		fruitSpriteUI2->SetEnable(false);

		auto fruitSpriteUI3 = canvas->CreateSpriteUI("FruitSpriteUI3");
		fruitSpriteUI3->SetTexture(mini_Game_fruitTexture);
		fruitSpriteUI3->SetPosition(Vector3{ 0.0f, 0.0f, 0.3f });
		fruitSpriteUI3->SetWidth(39.f);
		fruitSpriteUI3->SetHeight(62.f);
		fruitSpriteUI3->SetEnable(false);

		// 순서 알려주는 텍스트
		auto numberText = canvas->CreateTextUI("NumberText");
		std::wstring numTextString = L"1";
		numberText->SetPosition(Vector3{ 0.0f, 0.0f, 0.4f });
		numberText->SetText(numTextString);
		numberText->SetFont("NanumSquareNeoBold");
		numberText->SetFontSize(30.f);
		numberText->SetEnable(false);

		auto numberText2 = canvas->CreateTextUI("NumberText2");
		std::wstring numTextString2 = L"2";
		numberText2->SetPosition(Vector3{ 0.0f, 0.0f, 0.4f });
		numberText2->SetText(numTextString2);
		numberText2->SetFont("NanumSquareNeoBold");
		numberText2->SetFontSize(30.f);
		numberText2->SetEnable(false);

		auto numberText3 = canvas->CreateTextUI("NumberText3");
		std::wstring numTextString3 = L"3";
		numberText3->SetPosition(Vector3{ 0.0f, 0.0f, 0.4f });
		numberText3->SetText(numTextString3);
		numberText3->SetFont("NanumSquareNeoBold");
		numberText3->SetFontSize(30.f);
		numberText3->SetEnable(false);


		//==================(InteractionUI)============================== 
		auto fSpriteUI = canvas->CreateSpriteUI("F");
		fSpriteUI->SetTexture(fTexture);
		fSpriteUI->SetPosition(Vector3{ 0.0f, 400.0f, 0.3f });
		fSpriteUI->SetWidth(100.f);
		fSpriteUI->SetHeight(100.f);
		fSpriteUI->SetEnable(false);

		// 미니 게임 원 궤도에 나오는 이미지 
		auto miniGameRumSpriteUI = canvas->CreateSpriteUI("MiniGameRumSpriteUI");
		miniGameRumSpriteUI->SetTexture(mini_Game_rumTexture);
		miniGameRumSpriteUI->SetPosition(Vector3{ 0.0f, 0.0f, 0.4f });
		miniGameRumSpriteUI->SetWidth(35.f);
		miniGameRumSpriteUI->SetHeight(100.f);
		miniGameRumSpriteUI->SetEnable(false);

		auto miniGameRumSpriteUI2 = canvas->CreateSpriteUI("MiniGameRumSpriteUI2");
		miniGameRumSpriteUI2->SetTexture(mini_Game_rumTexture);
		miniGameRumSpriteUI2->SetPosition(Vector3{ 0.0f, 0.0f, 0.4f });
		miniGameRumSpriteUI2->SetWidth(35.f);
		miniGameRumSpriteUI2->SetHeight(100.f);
		miniGameRumSpriteUI2->SetEnable(false);

		auto miniGameRumSpriteUI3 = canvas->CreateSpriteUI("MiniGameRumSpriteUI3");
		miniGameRumSpriteUI3->SetTexture(mini_Game_rumTexture);
		miniGameRumSpriteUI3->SetPosition(Vector3{ 0.0f, 0.0f, 0.4f });
		miniGameRumSpriteUI3->SetWidth(35.f);
		miniGameRumSpriteUI3->SetHeight(100.f);
		miniGameRumSpriteUI3->SetEnable(false);

		auto miniGameVodkaSpriteUI = canvas->CreateSpriteUI("MiniGameVodkaSpriteUI");
		miniGameVodkaSpriteUI->SetTexture(mini_Game_vodkaTexture);
		miniGameVodkaSpriteUI->SetPosition(Vector3{ 0.0f, 0.0f, 0.4f });
		miniGameVodkaSpriteUI->SetWidth(35.f);
		miniGameVodkaSpriteUI->SetHeight(89.f);
		miniGameVodkaSpriteUI->SetEnable(false);

		auto miniGameVodkaSpriteUI2 = canvas->CreateSpriteUI("MiniGameVodkaSpriteUI2");
		miniGameVodkaSpriteUI2->SetTexture(mini_Game_vodkaTexture);
		miniGameVodkaSpriteUI2->SetPosition(Vector3{ 0.0f, 0.0f, 0.4f });
		miniGameVodkaSpriteUI2->SetWidth(35.f);
		miniGameVodkaSpriteUI2->SetHeight(89.f);
		miniGameVodkaSpriteUI2->SetEnable(false);

		auto miniGameVodkaSpriteUI3 = canvas->CreateSpriteUI("MiniGameVodkaSpriteUI3");
		miniGameVodkaSpriteUI3->SetTexture(mini_Game_vodkaTexture);
		miniGameVodkaSpriteUI3->SetPosition(Vector3{ 0.0f, 0.0f, 0.4f });
		miniGameVodkaSpriteUI3->SetWidth(35.f);
		miniGameVodkaSpriteUI3->SetHeight(89.f);
		miniGameVodkaSpriteUI3->SetEnable(false);

		auto miniGameGinSpriteUI = canvas->CreateSpriteUI("MiniGameGinSpriteUI");
		miniGameGinSpriteUI->SetTexture(mini_Game_ginTexture);
		miniGameGinSpriteUI->SetPosition(Vector3{ 0.0f, 0.0f, 0.4f });
		miniGameGinSpriteUI->SetWidth(35.f);
		miniGameGinSpriteUI->SetHeight(99.f);
		miniGameGinSpriteUI->SetEnable(false);

		auto miniGameGinSpriteUI2 = canvas->CreateSpriteUI("MiniGameGinSpriteUI2");
		miniGameGinSpriteUI2->SetTexture(mini_Game_ginTexture);
		miniGameGinSpriteUI2->SetPosition(Vector3{ 0.0f, 0.0f, 0.4f });
		miniGameGinSpriteUI2->SetWidth(35.f);
		miniGameGinSpriteUI2->SetHeight(99.f);
		miniGameGinSpriteUI2->SetEnable(false);

		auto miniGameGinSpriteUI3 = canvas->CreateSpriteUI("MiniGameGinSpriteUI3");
		miniGameGinSpriteUI3->SetTexture(mini_Game_ginTexture);
		miniGameGinSpriteUI3->SetPosition(Vector3{ 0.0f, 0.0f, 0.4f });
		miniGameGinSpriteUI3->SetWidth(35.f);
		miniGameGinSpriteUI3->SetHeight(99.f);
		miniGameGinSpriteUI3->SetEnable(false);

		auto miniGameLiquorSpriteUI = canvas->CreateSpriteUI("MiniGameLiquorSpriteUI");
		miniGameLiquorSpriteUI->SetTexture(mini_Game_liquorTexture);
		miniGameLiquorSpriteUI->SetPosition(Vector3{ 0.0f, 0.0f, 0.4f });
		miniGameLiquorSpriteUI->SetWidth(28.f);
		miniGameLiquorSpriteUI->SetHeight(96.f);
		miniGameLiquorSpriteUI->SetEnable(false);

		auto miniGameLiquorSpriteUI2 = canvas->CreateSpriteUI("MiniGameLiquorSpriteUI2");
		miniGameLiquorSpriteUI2->SetTexture(mini_Game_liquorTexture);
		miniGameLiquorSpriteUI2->SetPosition(Vector3{ 0.0f, 0.0f, 0.4f });
		miniGameLiquorSpriteUI2->SetWidth(28.f);
		miniGameLiquorSpriteUI2->SetHeight(96.f);
		miniGameLiquorSpriteUI2->SetEnable(false);

		auto miniGameLiquorSpriteUI3 = canvas->CreateSpriteUI("MiniGameLiquorSpriteUI3");
		miniGameLiquorSpriteUI3->SetTexture(mini_Game_liquorTexture);
		miniGameLiquorSpriteUI3->SetPosition(Vector3{ 0.0f, 0.0f, 0.4f });
		miniGameLiquorSpriteUI3->SetWidth(28.f);
		miniGameLiquorSpriteUI3->SetHeight(96.f);
		miniGameLiquorSpriteUI3->SetEnable(false);


		auto miniGameJuiceSpriteUI = canvas->CreateSpriteUI("MiniGameJuiceSpriteUI");
		miniGameJuiceSpriteUI->SetTexture(mini_Game_juiceTexture);
		miniGameJuiceSpriteUI->SetPosition(Vector3{ 0.0f, 0.0f, 0.4f });
		miniGameJuiceSpriteUI->SetWidth(34.f);
		miniGameJuiceSpriteUI->SetHeight(63.f);
		miniGameJuiceSpriteUI->SetEnable(false);

		auto miniGameJuiceSpriteUI2 = canvas->CreateSpriteUI("MiniGameJuiceSpriteUI2");
		miniGameJuiceSpriteUI2->SetTexture(mini_Game_juiceTexture);
		miniGameJuiceSpriteUI2->SetPosition(Vector3{ 0.0f, 0.0f, 0.4f });
		miniGameJuiceSpriteUI2->SetWidth(34.f);
		miniGameJuiceSpriteUI2->SetHeight(63.f);
		miniGameJuiceSpriteUI2->SetEnable(false);

		auto miniGameJuiceSpriteUI3 = canvas->CreateSpriteUI("MiniGameJuiceSpriteUI3");
		miniGameJuiceSpriteUI3->SetTexture(mini_Game_juiceTexture);
		miniGameJuiceSpriteUI3->SetPosition(Vector3{ 0.0f, 0.0f, 0.4f });
		miniGameJuiceSpriteUI3->SetWidth(34.f);
		miniGameJuiceSpriteUI3->SetHeight(63.f);
		miniGameJuiceSpriteUI3->SetEnable(false);

		auto miniGameSodaSpriteUI = canvas->CreateSpriteUI("MiniGameSodaSpriteUI");
		miniGameSodaSpriteUI->SetTexture(mini_Game_sodaTexture);
		miniGameSodaSpriteUI->SetPosition(Vector3{ -0.0f, 0.0f, 0.4f });
		miniGameSodaSpriteUI->SetWidth(28.f);
		miniGameSodaSpriteUI->SetHeight(78.f);
		miniGameSodaSpriteUI->SetEnable(false);

		auto miniGameSodaSpriteUI2 = canvas->CreateSpriteUI("MiniGameSodaSpriteUI2");
		miniGameSodaSpriteUI2->SetTexture(mini_Game_sodaTexture);
		miniGameSodaSpriteUI2->SetPosition(Vector3{ 0.f, 0.0f, 0.4f });
		miniGameSodaSpriteUI2->SetWidth(28.f);
		miniGameSodaSpriteUI2->SetHeight(78.f);
		miniGameSodaSpriteUI2->SetEnable(false);

		auto miniGameSodaSpriteUI3 = canvas->CreateSpriteUI("MiniGameSodaSpriteUI3");
		miniGameSodaSpriteUI3->SetTexture(mini_Game_sodaTexture);
		miniGameSodaSpriteUI3->SetPosition(Vector3{ 0.0f, 0.0f, 0.4f });
		miniGameSodaSpriteUI3->SetWidth(28.f);
		miniGameSodaSpriteUI3->SetHeight(78.f);
		miniGameSodaSpriteUI3->SetEnable(false);

		auto miniGameFruitSpriteUI = canvas->CreateSpriteUI("MiniGameFruitSpriteUI");
		miniGameFruitSpriteUI->SetTexture(mini_Game_fruitTexture);
		miniGameFruitSpriteUI->SetPosition(Vector3{ 0.0f, 0.0f, 0.4f });
		miniGameFruitSpriteUI->SetWidth(39.f);
		miniGameFruitSpriteUI->SetHeight(62.f);
		miniGameFruitSpriteUI->SetEnable(false);

		auto miniGameFruitSpriteUI2 = canvas->CreateSpriteUI("MiniGameFruitSpriteUI2");
		miniGameFruitSpriteUI2->SetTexture(mini_Game_fruitTexture);
		miniGameFruitSpriteUI2->SetPosition(Vector3{ 0.0f, 0.0f, 0.4f });
		miniGameFruitSpriteUI2->SetWidth(39.f);
		miniGameFruitSpriteUI2->SetHeight(62.f);
		miniGameFruitSpriteUI2->SetEnable(false);

		auto miniGameFruitSpriteUI3 = canvas->CreateSpriteUI("MiniGameFruitSpriteUI3");
		miniGameFruitSpriteUI3->SetTexture(mini_Game_fruitTexture);
		miniGameFruitSpriteUI3->SetPosition(Vector3{ 0.0f, 0.0f, 0.4f });
		miniGameFruitSpriteUI3->SetWidth(39.f);
		miniGameFruitSpriteUI3->SetHeight(62.f);
		miniGameFruitSpriteUI3->SetEnable(false);

		// 위의 것 다 필요 없습니다..
		for (int i = 0; i < 3; i++)
		{
			std::string icon = "SelectSprite" + to_string(i);
			std::string minigame = "MiniGameSprite" + to_string(i);
			// 표시용
			auto iconSprite = canvas->CreateSpriteUI(icon);
			iconSprite->SetPosition(Vector3{ 0.f, 0.f, 0.4f });
			iconSprite->SetEnable(false);
			// 미니 게임용
			auto miniGameSprite = canvas->CreateSpriteUI(minigame);
			miniGameSprite->SetPosition(Vector3{ 0.f, 0.f, 0.4f });
			miniGameSprite->SetEnable(false);
		}


		//==================(ResultTextrue)===================================
		auto mixResultSpriteUI = canvas->CreateSpriteUI("MixResultSpriteUI");
		mixResultSpriteUI->SetTexture(mixResultTexture);
		mixResultSpriteUI->SetPosition(Vector3{ 0.0f, 0.0f, 0.4f });
		mixResultSpriteUI->SetWidth(1920.f);
		mixResultSpriteUI->SetHeight(1080.f);
		mixResultSpriteUI->SetEnable(false);

		auto offerButtonSpriteUI = canvas->CreateButtonUI("OfferButton");
		offerButtonSpriteUI->SetPosition(Vector3{ 3.0f, 330.0f, 0.4f });
		offerButtonSpriteUI->SetDefaultTexture(offerButtonTexture);
		offerButtonSpriteUI->SetWidth(247.f);
		offerButtonSpriteUI->SetHeight(135.f);
		offerButtonSpriteUI->SetEnable(false);


		auto mixRumSpriteUI = canvas->CreateSpriteUI("MixRumSpriteUI");
		mixRumSpriteUI->SetTexture(mixRumTexture);
		mixRumSpriteUI->SetPosition(Vector3{ 0.f, 9.f, 0.4f });
		mixRumSpriteUI->SetWidth(109.f);
		mixRumSpriteUI->SetHeight(390.f);
		mixRumSpriteUI->SetEnable(false);

		auto rumCokeSpriteUI = canvas->CreateSpriteUI("RumCokeSpriteUI");
		rumCokeSpriteUI->SetTexture(rumCokeTexture);
		rumCokeSpriteUI->SetPosition(Vector3{ 0.f, 9.f, 0.4f });
		rumCokeSpriteUI->SetWidth(600.f);
		rumCokeSpriteUI->SetHeight(457.f);
		rumCokeSpriteUI->SetEnable(false);

		auto cubaLibreSpriteUI = canvas->CreateSpriteUI("CubaLibreSpriteUI");
		cubaLibreSpriteUI->SetTexture(cubaLibreTexture);
		cubaLibreSpriteUI->SetPosition(Vector3{ 0.f, 9.f, 0.4f });
		cubaLibreSpriteUI->SetWidth(600.f);
		cubaLibreSpriteUI->SetHeight(457.f);
		cubaLibreSpriteUI->SetEnable(false);

		auto daiquiriSpriteUI = canvas->CreateSpriteUI("DaiquiriSpriteUI");
		daiquiriSpriteUI->SetTexture(daiquiriTexture);
		daiquiriSpriteUI->SetPosition(Vector3{ 0.f, 9.f, 0.4f });
		daiquiriSpriteUI->SetWidth(600.f);
		daiquiriSpriteUI->SetHeight(457.f);
		daiquiriSpriteUI->SetEnable(false);

		auto painKillerSpriteUI = canvas->CreateSpriteUI("PainKillerSpriteUI");
		painKillerSpriteUI->SetTexture(painKillerTexture);
		painKillerSpriteUI->SetPosition(Vector3{ 0.f, 9.f, 0.4f });
		painKillerSpriteUI->SetWidth(600.f);
		painKillerSpriteUI->SetHeight(457.f);
		painKillerSpriteUI->SetEnable(false);

		auto maiTaiSpriteUI = canvas->CreateSpriteUI("MaiTaiSpriteUI");
		maiTaiSpriteUI->SetTexture(maiTaiTexture);
		maiTaiSpriteUI->SetPosition(Vector3{ 0.f, 9.f, 0.4f });
		maiTaiSpriteUI->SetWidth(600.f);
		maiTaiSpriteUI->SetHeight(457.f);
		maiTaiSpriteUI->SetEnable(false);

		auto plantersPunchSpriteUI = canvas->CreateSpriteUI("PlantersPunchSpriteUI");
		plantersPunchSpriteUI->SetTexture(plantersPunchTexture);
		plantersPunchSpriteUI->SetPosition(Vector3{ 0.f, 9.f, 0.4f });
		plantersPunchSpriteUI->SetWidth(600.f);
		plantersPunchSpriteUI->SetHeight(457.f);
		plantersPunchSpriteUI->SetEnable(false);

		auto bushwackerSpriteUI = canvas->CreateSpriteUI("BushwackerSpriteUI");
		bushwackerSpriteUI->SetTexture(bushwackerTexture);
		bushwackerSpriteUI->SetPosition(Vector3{ 0.f, 9.f, 0.4f });
		bushwackerSpriteUI->SetWidth(600.f);
		bushwackerSpriteUI->SetHeight(457.f);
		bushwackerSpriteUI->SetEnable(false);

		//VODKA
		auto mixVodkaSpriteUI = canvas->CreateSpriteUI("MixVodkaSpriteUI");
		mixVodkaSpriteUI->SetTexture(mixVodkaTexture);
		mixVodkaSpriteUI->SetPosition(Vector3{ 0.f, 9.f, 0.4f });
		mixVodkaSpriteUI->SetWidth(113.f);
		mixVodkaSpriteUI->SetHeight(290.f);
		mixVodkaSpriteUI->SetEnable(false);

		auto espressoTiniSpriteUI = canvas->CreateSpriteUI("EspressoTiniSpriteUI");
		espressoTiniSpriteUI->SetTexture(espressoTiniTexture);
		espressoTiniSpriteUI->SetPosition(Vector3{ 0.f, 9.f, 0.4f });
		espressoTiniSpriteUI->SetWidth(600.f);
		espressoTiniSpriteUI->SetHeight(457.f);
		espressoTiniSpriteUI->SetEnable(false);

		auto mosCowmuleSpriteUI = canvas->CreateSpriteUI("MosCowmuleSpriteUI");
		mosCowmuleSpriteUI->SetTexture(mosCowmuleTexture);
		mosCowmuleSpriteUI->SetPosition(Vector3{ 0.f, 9.f, 0.4f });
		mosCowmuleSpriteUI->SetWidth(600.f);
		mosCowmuleSpriteUI->SetHeight(457.f);
		mosCowmuleSpriteUI->SetEnable(false);

		auto blackRussianSpriteUI = canvas->CreateSpriteUI("BlackRussianSpriteUI");
		blackRussianSpriteUI->SetTexture(blackRussianTexture);
		blackRussianSpriteUI->SetPosition(Vector3{ 0.f, 9.f, 0.4f });
		blackRussianSpriteUI->SetWidth(600.f);
		blackRussianSpriteUI->SetHeight(457.f);
		blackRussianSpriteUI->SetEnable(false);

		auto cosmoPolitanSpriteUI = canvas->CreateSpriteUI("CosmoPolitanSpriteUI");
		cosmoPolitanSpriteUI->SetTexture(cosmoPolitanTexture);
		cosmoPolitanSpriteUI->SetPosition(Vector3{ 0.f, 9.f, 0.4f });
		cosmoPolitanSpriteUI->SetWidth(600.f);
		cosmoPolitanSpriteUI->SetHeight(457.f);
		cosmoPolitanSpriteUI->SetEnable(false);

		auto bloodyMarySpriteUI = canvas->CreateSpriteUI("BloodyMarySpriteUI");
		bloodyMarySpriteUI->SetTexture(bloodyMaryTexture);
		bloodyMarySpriteUI->SetPosition(Vector3{ 0.f, 9.f, 0.4f });
		bloodyMarySpriteUI->SetWidth(600.f);
		bloodyMarySpriteUI->SetHeight(457.f);
		bloodyMarySpriteUI->SetEnable(false);

		auto poetKillerSpriteUI = canvas->CreateSpriteUI("PoetKillerSpriteUI");
		poetKillerSpriteUI->SetTexture(poetKillerTexture);
		poetKillerSpriteUI->SetPosition(Vector3{ 0.f, 9.f, 0.4f });
		poetKillerSpriteUI->SetWidth(600.f);
		poetKillerSpriteUI->SetHeight(457.f);
		poetKillerSpriteUI->SetEnable(false);

		auto sunRiseSpriteUI = canvas->CreateSpriteUI("SunRiseSpriteUI");
		sunRiseSpriteUI->SetTexture(sunRiseTexture);
		sunRiseSpriteUI->SetPosition(Vector3{ 0.f, 9.f, 0.4f });
		sunRiseSpriteUI->SetWidth(600.f);
		sunRiseSpriteUI->SetHeight(457.f);
		sunRiseSpriteUI->SetEnable(false);

		//GIN
		auto mixGinSpriteUI = canvas->CreateSpriteUI("MixGinSpriteUI");
		mixGinSpriteUI->SetTexture(mixGinTexture);
		mixGinSpriteUI->SetPosition(Vector3{ 0.f, 9.f, 0.4f });
		mixGinSpriteUI->SetWidth(111.f);
		mixGinSpriteUI->SetHeight(321.f);
		mixGinSpriteUI->SetEnable(false);

		auto ginFizzSpriteUI = canvas->CreateSpriteUI("GinFizzSpriteUI");
		ginFizzSpriteUI->SetTexture(ginFizzTexture);
		ginFizzSpriteUI->SetPosition(Vector3{ 0.f, 9.f, 0.4f });
		ginFizzSpriteUI->SetWidth(600.f);
		ginFizzSpriteUI->SetHeight(457.f);
		ginFizzSpriteUI->SetEnable(false);


		auto ginTonicSpriteUI = canvas->CreateSpriteUI("GinTonicSpriteUI");
		ginTonicSpriteUI->SetTexture(ginTonicTexture);
		ginTonicSpriteUI->SetPosition(Vector3{ 0.f, 9.f, 0.4f });
		ginTonicSpriteUI->SetWidth(600.f);
		ginTonicSpriteUI->SetHeight(457.f);
		ginTonicSpriteUI->SetEnable(false);


		auto aviationSpriteUI = canvas->CreateSpriteUI("AviationSpriteUI");
		aviationSpriteUI->SetTexture(aviationTexture);
		aviationSpriteUI->SetPosition(Vector3{ 0.f, 9.f, 0.4f });
		aviationSpriteUI->SetWidth(600.f);
		aviationSpriteUI->SetHeight(457.f);
		aviationSpriteUI->SetEnable(false);

		auto ladykillerSpriteUI = canvas->CreateSpriteUI("LadykillerSpriteUI");
		ladykillerSpriteUI->SetTexture(ladykillerTexture);
		ladykillerSpriteUI->SetPosition(Vector3{ 0.f, 9.f, 0.4f });
		ladykillerSpriteUI->SetWidth(600.f);
		ladykillerSpriteUI->SetHeight(457.f);
		ladykillerSpriteUI->SetEnable(false);

		auto brambleSpriteUI = canvas->CreateSpriteUI("BrambleSpriteUI");
		brambleSpriteUI->SetTexture(brambleTexture);
		brambleSpriteUI->SetPosition(Vector3{ 0.f, 9.f, 0.4f });
		brambleSpriteUI->SetWidth(600.f);
		brambleSpriteUI->SetHeight(457.f);
		brambleSpriteUI->SetEnable(false);

		auto lastWordSpriteUI = canvas->CreateSpriteUI("LastWordSpriteUI");
		lastWordSpriteUI->SetTexture(lastWordTexture);
		lastWordSpriteUI->SetPosition(Vector3{ 0.f, 9.f, 0.4f });
		lastWordSpriteUI->SetWidth(600.f);
		lastWordSpriteUI->SetHeight(457.f);
		lastWordSpriteUI->SetEnable(false);

		auto snapperSpriteUI = canvas->CreateSpriteUI("SnapperSpriteUI");
		snapperSpriteUI->SetTexture(snapperTexture);
		snapperSpriteUI->SetPosition(Vector3{ 0.f, 9.f, 0.4f });
		snapperSpriteUI->SetWidth(600.f);
		snapperSpriteUI->SetHeight(457.f);
		snapperSpriteUI->SetEnable(false);


		canvas->SetEnable(false);
	}

	void UICreator::CreateItemResultCanvas()
	{
		auto canvas = mainCanvas->GetCanvasUI("ItemResultCanvas");

		auto itemResultTexture = graphicsManager->LoadTexture(L"Resources/UI/ItemResult.png");

		auto lobbyButton2Texture = graphicsManager->LoadTexture(L"Resources/UI/ItemResultLobby.png");


		//==================(InGameItemResult)===============================
		auto itemResultSpriteUI = canvas->CreateSpriteUI("ItemResultSpriteUI");
		itemResultSpriteUI->SetTexture(itemResultTexture);
		itemResultSpriteUI->SetPosition(Vector3{ 0.f, 0.f, 0.5f });
		itemResultSpriteUI->SetWidth(1920.0f);
		itemResultSpriteUI->SetHeight(1080.0f);

		// 위치를 생성할 때 배치하지 말고 처음 부터 만들어버립시다.
			// 텍스트만 바꿀 수 있도록 세팅해줍시다.
		for (int i = 0; i < 21; i++)
		{
			std::string itemResultIdx = to_string(i);

			auto itemResultTextUI = canvas->CreateTextUI("ItemResultTextUI" + itemResultIdx);
			itemResultTextUI->SetText("0");
			itemResultTextUI->SetFontSize(25.f);

			switch (i)
			{
			case 0:
				itemResultTextUI->SetPosition({ -110.0f , -170.0f, 0.4f });
				break;
			case 1:
				itemResultTextUI->SetPosition({ 2.0f , -170.0f, 0.4f });
				break;
			case 2:
				itemResultTextUI->SetPosition({ 110.0f , -170.0f, 0.4f });
				break;
			case 3:
				itemResultTextUI->SetPosition({ -165.0f , -10.0f, 0.4f });
				break;
			case 4:
				itemResultTextUI->SetPosition({ -57.0f , -10.0f, 0.4f });
				break;
			case 5:
				itemResultTextUI->SetPosition({ 57.0f , -10.0f, 0.4f });
				break;
			case 6:
				itemResultTextUI->SetPosition({ 165.0f , -10.0f, 0.4f });
				break;
			case 7:
				itemResultTextUI->SetPosition({ -331.0f , 208.0f, 0.4f });
				break;
			case 8:
				itemResultTextUI->SetPosition({ -222.0f , 208.0f, 0.4f });
				break;
			case 9:
				itemResultTextUI->SetPosition({ -112.0f , 208.0f, 0.4f });
				break;
			case 10:
				itemResultTextUI->SetPosition({ -2.0f , 208.0f, 0.4f });
				break;
			case 11:
				itemResultTextUI->SetPosition({ 108.0f , 208.0f, 0.4f });
				break;
			case 12:
				itemResultTextUI->SetPosition({ 219.0f , 208.0f, 0.4f });
				break;
			case 13:
				itemResultTextUI->SetPosition({ 331.0f , 208.0f, 0.4f });
				break;
			case 14:
				itemResultTextUI->SetPosition({ -331.0f , 376.0f, 0.4f });
				break;
			case 15:
				itemResultTextUI->SetPosition({ -222.0f  , 376.0f, 0.4f });
				break;
			case 16:
				itemResultTextUI->SetPosition({ -112.0f  , 376.0f, 0.4f });
				break;
			case 17:
				itemResultTextUI->SetPosition({ -2.0f , 376.0f, 0.4f });
				break;
			case 18:
				itemResultTextUI->SetPosition({ 108.0f, 376.0f, 0.4f });
				break;
			case 19:
				itemResultTextUI->SetPosition({ 219.0f , 376.0f, 0.4f });
				break;
			case 20:
				itemResultTextUI->SetPosition({ 331.0f , 376.0f, 0.4f });
				break;
			default:
				break;
			}
		}

		auto resultLobbyButton = canvas->CreateButtonUI("ResultLobbyButton");
		resultLobbyButton->SetPosition(Vector3{ 825.f, 477.f, 0.3f });
		resultLobbyButton->SetDefaultTexture(lobbyButton2Texture);
		resultLobbyButton->SetWidth(223.f);
		resultLobbyButton->SetHeight(93.f);


		canvas->SetEnable(false);

	}

	void UICreator::CreateStoreCanvas()
	{
		auto canvas = mainCanvas->GetCanvasUI("StoreCanvas");

		auto storeBG = graphicsManager->LoadTexture(L"Resources/UI/Store/Store_BG.png");
		auto fillTex = graphicsManager->LoadTexture(L"Resources/UI/Store/Skill_Level.png");
		auto blankTex = graphicsManager->LoadTexture(L"Resources/UI/Store/Skill_level_b.png");
		auto hoverTex = graphicsManager->LoadTexture(L"Resources/UI/Store/Select_BG.png");
		auto defaultTex = graphicsManager->LoadTexture(L"Resources/UI/NotUI.png");
		auto goldTex = graphicsManager->LoadTexture(L"Resources/UI/Gold.png");
		auto closeTex = graphicsManager->LoadTexture(L"Resources/UI/ObjectBox.png");

		auto backGround = canvas->CreateSpriteUI("StoreBackGround");
		backGround->SetTexture(storeBG);
		backGround->SetPosition({ 0.f, 0.f, 0.5f });
		// 16 : 9 비율
		backGround->SetWidth(1920.f);
		backGround->SetHeight(1080.f);

		for (int i = 0; i < 3; i++)
		{
			std::string idx = to_string(i);

			auto selectButton = canvas->CreateButtonUI("SkillSelectButton" + idx);
			selectButton->SetDefaultTexture(defaultTex);
			selectButton->SetHoverMaskColor(1.f, 1.f, 1.f, 0.7f);
			selectButton->SetHoverTexture(hoverTex);
			selectButton->SetWidth(1060.f);
			selectButton->SetHeight(133.f);
			selectButton->SetPosition({ -216.f, -162.f + (i * 190.f), 0.5f });
		}

		for (int i = 0; i < 5; i++)
		{
			std::string idx = to_string(i);

			auto powerLevelFill = canvas->CreateSpriteUI("PowerLevelFill" + idx);
			powerLevelFill->SetTexture(fillTex);
			powerLevelFill->SetWidth(88.f);
			powerLevelFill->SetHeight(88.f);
			powerLevelFill->SetPosition({ -351.f + (i * 120.f), -162.f, 0.4f });
			powerLevelFill->SetEnable(false);

			auto powerLevelBlank = canvas->CreateSpriteUI("PowerLevelBlank" + idx);
			powerLevelBlank->SetTexture(blankTex);
			powerLevelBlank->SetWidth(117.f);
			powerLevelBlank->SetHeight(117.f);
			powerLevelBlank->SetPosition({ -350.f + (i * 120.f), -162.f, 0.35f });

			auto healthLevelFill = canvas->CreateSpriteUI("HealthLevelFill" + idx);
			healthLevelFill->SetTexture(fillTex);
			healthLevelFill->SetWidth(88.f);
			healthLevelFill->SetHeight(88.f);
			healthLevelFill->SetPosition({ -351.f + (i * 120.f), 28.f, 0.4f });
			healthLevelFill->SetEnable(false);

			auto healthLevelBlank = canvas->CreateSpriteUI("HealthLevelBlank" + idx);
			healthLevelBlank->SetTexture(blankTex);
			healthLevelBlank->SetWidth(117.f);
			healthLevelBlank->SetHeight(117.f);
			healthLevelBlank->SetPosition({ -350.f + (i * 120.f), 28.f, 0.35f });

			auto speedLevelFill = canvas->CreateSpriteUI("SpeedLevelFill" + idx);
			speedLevelFill->SetTexture(fillTex);
			speedLevelFill->SetWidth(88.f);
			speedLevelFill->SetHeight(88.f);
			speedLevelFill->SetPosition({ -351.f + (i * 120.f), 218.f, 0.4f });
			speedLevelFill->SetEnable(false);

			auto speedLevelBlank = canvas->CreateSpriteUI("SpeedLevelBlank" + idx);
			speedLevelBlank->SetTexture(blankTex);
			speedLevelBlank->SetWidth(117.f);
			speedLevelBlank->SetHeight(117.f);
			speedLevelBlank->SetPosition({ -350.f + (i * 120.f), 218.f, 0.35f });
		}

		auto powerName = canvas->CreateTextUI("PowerTitle");
		powerName->SetFontSize(40.f);
		powerName->SetPosition({ -617.f, -166.f, 0.4f });
		powerName->SetText(L"손목 단련");
		powerName->SetFont("NanumSquareNeoBold");

		auto healthName = canvas->CreateTextUI("HealthTitle");
		healthName->SetFontSize(40.f);
		healthName->SetPosition({ -617.f, 24.f, 0.4f });
		healthName->SetText(L"체력 단련");
		healthName->SetFont("NanumSquareNeoBold");

		auto speedName = canvas->CreateTextUI("SpeedTitle");
		speedName->SetFontSize(40.f);
		speedName->SetPosition({ -617.f, 213.f, 0.4f });
		speedName->SetText(L"서빙 연습");
		speedName->SetFont("NanumSquareNeoBold");

		auto cost = canvas->CreateTextUI("Cost");
		cost->SetFontSize(28.f);
		cost->SetPosition({ 707.f, 392.f, 0.4f });
		cost->SetColor({ 1.0f, 0.0f, 0.0f, 1.0f });
		cost->SetText(L"");
		cost->SetFont("NanumSquareNeoBold");

		// 부연 설명
		auto skillDesc = canvas->CreateTextUI("SkillDesc");
		skillDesc->SetFontSize(30.f);
		skillDesc->SetPosition({ 555.f, -166.f, 0.4f });
		skillDesc->SetFixedWidth(435.f);
		skillDesc->SetNText(L"");
		skillDesc->SetFont("NanumSquareRoundRegular");

		// 핵심 내용
		auto skillPoint = canvas->CreateTextUI("SkillPoint");
		skillPoint->SetFontSize(35.f);
		skillPoint->SetNText(L"");
		skillPoint->SetFixedWidth(385.f);
		skillPoint->SetColor({ 0.8f, 0.8f, 1.0f, 1.0f });
		skillPoint->SetPosition({ 546.f, 200.f, 0.4f });
		skillPoint->SetFont("NanumSquareRoundBold");


		// 871 490 // -870
		/*auto closeText = canvas->CreateTextUI("CloseText");
		closeText->SetFontSize(32.f);
		closeText->SetText(L"F is Close");
		closeText->SetColor({ 0.36f, 0.28f, 0.21f, 1.0f });
		closeText->SetPosition({ -782.f, 490.f, 0.4f });*/

		auto closeButton = canvas->CreateButtonUI("CloseButton");
		closeButton->SetDefaultTexture(defaultTex);
		closeButton->SetHoverTexture(closeTex);
		closeButton->SetPosition({ 0.f, 445.f, 0.4f });
		closeButton->SetWidth(330.f);
		closeButton->SetHeight(110.f);

		auto nowAmount = canvas->CreateTextUI("CurrentMoney");
		nowAmount->SetFontSize(32.f);
		nowAmount->SetFont("NanumSquareNeoBold");
		nowAmount->SetText(L"");
		nowAmount->SetColor({ 0.9f, 0.9f, 1.0f, 1.0f });
		nowAmount->SetPosition({ 835.f, 490.f, 0.4f });

		auto amountSprite = canvas->CreateSpriteUI("AmountSprite");
		amountSprite->SetTexture(goldTex);
		amountSprite->SetPosition({ 694.f, 500.f, 0.4f });
		amountSprite->SetWidth(56.f);
		amountSprite->SetHeight(30.f);
		amountSprite->SetMaskColor({ 0.85f, 0.85f, 1.0f, 1.0f });

		canvas->SetEnable(false);

	}

	void UICreator::CreatePopupCanvas()
	{
		auto canvas = mainCanvas->GetCanvasUI("PopUpCanvas");

		auto popupTex = graphicsManager->LoadTexture(L"Resources/UI/CockPopupFirst.png");
		auto hoverTex = graphicsManager->LoadTexture(L"Resources/UI/Store/Select_BG.png");
		auto defaultTex = graphicsManager->LoadTexture(L"Resources/UI/NotUI.png");
		//auto maskTex = graphicsManager->LoadTexture(L"Resources/UI/ButtonMask.png");

		auto popupUI = canvas->CreateSpriteUI("PopUp");
		popupUI->SetTexture(popupTex);
		popupUI->SetPosition({ 200.f, -60.f, 0.4f });
		popupUI->SetWidth(246.f);
		popupUI->SetHeight(178.f);

		for (int i = 0; i < 3; i++)
		{
			std::string idx = to_string(i);

			auto selectButton = canvas->CreateButtonUI("SelectButton" + idx);
			selectButton->SetDefaultTexture(defaultTex);
			selectButton->SetHoverMaskColor(1.f, 1.f, 1.f, 0.7f);
			selectButton->SetHoverTexture(hoverTex);
			selectButton->SetWidth(225.f);
			selectButton->SetHeight(50.f);
			selectButton->SetPosition({ 210.f, -110.f + (i * 50.f), 0.35f });

			if (i == 1)
				selectButton->SetEnable(false);
		}


		canvas->SetEnable(false);
	}

	void UICreator::CreateCursorCanvas()
	{
		auto canvas = mainCanvas->GetCanvasUI("CursorCanvas");

		auto cursorTex = graphicsManager->LoadTexture(L"Resources/UI/Icon/cursor.png");
		auto infoTex = graphicsManager->LoadTexture(L"Resources/UI/informationBackGround.png");

		auto info = canvas->CreateSpriteUI("Info");
		info->SetWidth(155.f);
		info->SetHeight(45.f);
		info->SetMaskAlpha(0.8f);
		info->SetCursor(true);
		info->SetTexture(infoTex);
		info->SetEnable(false);

		auto infoText = canvas->CreateTextUI("InfoText");
		infoText->SetFont("NanumSquareRoundBold");
		// RUM, VODKA, GIN, LIQUOR, JUICE, SODA, FRUIT
			// 6글자 53.f / 5글자 63.f / 4글자 66.f / 3글자 72.f
			// 10씩 안으로 넣어보자
		infoText->SetText(L"");
		infoText->SetColor(Vector4{ 1.f, 1.f, 1.f, 1.f });
		infoText->SetFontSize(20.f);
		infoText->SetCursor(true);
		infoText->SetEnable(false);

		auto cursor = canvas->CreateSpriteUI("Cursor");
		cursor->SetPosition({ 0.f, 0.f, 0.0001f });
		cursor->SetWidth(45.f);
		cursor->SetHeight(45.f);
		cursor->SetCursor(true);
		cursor->SetTexture(cursorTex);

	}

	void UICreator::CreateBarCanvas()
	{
		auto canvas = mainCanvas->GetCanvasUI("BarCanvas");

		// 배경
		auto combinationBackGroundTexture = graphicsManager->LoadTexture(L"Resources/UI/Making_BG.png");

		// 튜토리얼 텍스쳐는... 리소스 매니저에 저장해두고 가져옵시다.

		// 각종 버튼 텍스쳐 => 교체할 일이 없다.
		auto buttonTexture = graphicsManager->LoadTexture(L"Resources/UI/Button.png");

		auto mixbuttonTexture = graphicsManager->LoadTexture(L"Resources/UI/CockTailMix.png");

		auto reSetbuttonTexture = graphicsManager->LoadTexture(L"Resources/UI/ReSet.png");

		auto beerMachineTexture = graphicsManager->LoadTexture(L"Resources/UI/BeerMachine.png");

		auto cocktailrecipeTexture = graphicsManager->LoadTexture(L"Resources/UI/Recipe_button2.png");

		// 고객도 가져옵니다. 리소스 매니저에서.
		// 레시피 BG
		auto recipeBackGroundTexture = graphicsManager->LoadTexture(L"Resources/UI/RecipeBack.png");

		auto rumRecipeTexture = graphicsManager->LoadTexture(L"Resources/UI/Recipe_rum.png");

		auto vodkaRecipeTexture = graphicsManager->LoadTexture(L"Resources/UI/Recipe_vodka.png");

		auto ginRecipeTexture = graphicsManager->LoadTexture(L"Resources/UI/Recipe_gin.png");

		// 없는 버튼..
		auto informationButtonTexture = graphicsManager->LoadTexture(L"Resources/UI/NotUI.png");

		// 레시피 버튼 텍스쳐
		auto rumRecipeButtonTexture = graphicsManager->LoadTexture(L"Resources/UI/Recipe_rumButton.png");

		auto vodkaRecipeButtonTexture = graphicsManager->LoadTexture(L"Resources/UI/Recipe_vodkaButton.png");

		auto ginRecipeButtonTexture = graphicsManager->LoadTexture(L"Resources/UI/Recipe_ginButton.png");

		auto informationBackGroundTexture = graphicsManager->LoadTexture(L"Resources/UI/informationBackGround.png");
		// 제조시 사용할 버튼 텍스쳐
		auto rumButtonTexture = graphicsManager->LoadTexture(L"Resources/UI/M_RUM.png");

		auto vodkaButtonTexture = graphicsManager->LoadTexture(L"Resources/UI/M_VODKA.png");

		auto ginButtonTexture = graphicsManager->LoadTexture(L"Resources/UI/M_GIN.png");

		auto liquorButtonTexture = graphicsManager->LoadTexture(L"Resources/UI/M_LIQUEUR.png");

		auto juiceButtonTexture = graphicsManager->LoadTexture(L"Resources/UI/M_JUICE.png");

		auto sodaButtonTexture = graphicsManager->LoadTexture(L"Resources/UI/M_SODA.png");

		auto fruitButtonTexture = graphicsManager->LoadTexture(L"Resources/UI/M_FRUIT.png");

		//Gold
		auto goldTexture = graphicsManager->LoadTexture(L"Resources/UI/casher.png");

		// 말풍선
		auto reQuestTexture = graphicsManager->LoadTexture(L"Resources/UI/ReQuest.png");

		//==================(CombinationBackGround)===============================
		auto combinationBackGroundSpriteUI = canvas->CreateSpriteUI("CombinationBackGround");
		combinationBackGroundSpriteUI->SetTexture(combinationBackGroundTexture);
		combinationBackGroundSpriteUI->SetPosition(Vector3{ 0.f, 0.f, 0.7f });
		combinationBackGroundSpriteUI->SetWidth(1920.f);
		combinationBackGroundSpriteUI->SetHeight(1080.f);

		// 튜토리얼 텍스쳐 자체는 있기 때문에.. 하나만 생성해서 텍스쳐를 변경해줍니다.
		auto tutorialBackGroundSpriteUI = canvas->CreateSpriteUI("BarTutorialSprite");
		tutorialBackGroundSpriteUI->SetPosition(Vector3{ 0.f, 0.f, 0.3f });
		tutorialBackGroundSpriteUI->SetWidth(1920.f);
		tutorialBackGroundSpriteUI->SetHeight(1080.f);
		tutorialBackGroundSpriteUI->SetEnable(false);

		// npc의 경우도 하나만..! 위치는 캐릭터 따라 적절히 변경해줍니다.
			// 롭 -58, 렉스 -91, 밥 -60
			// 없다가 인트로 때 나타난다고 생각합시다..
		auto customerSpriteUI = canvas->CreateSpriteUI("CustomerSprite");
		customerSpriteUI->SetPosition(Vector3{ 0.f, -58.f, 0.6f });
		customerSpriteUI->SetWidth(723.f);
		customerSpriteUI->SetHeight(713.f);
		// 알파 값을 조정할 예정.
		customerSpriteUI->SetMaskAlpha(0.f);

		//==================(stuffButtons)===================================
		//달라지는 게 없기 때문에 그대로 가져와 줍니다.
		// 각 버튼의 hover event를 추가해줍니다.
		auto rumButton = canvas->CreateButtonUI("RumButton");
		rumButton->SetPosition(Vector3{ -857.f, 366.f, 0.5f });
		rumButton->SetDefaultTexture(rumButtonTexture);
		rumButton->SetWidth(109.f);
		rumButton->SetHeight(257.f);
		rumButton->SetEnterEvent([&]
			{
				auto cursorCanvas = UIManager::GetInstance()->GetCanvas("CursorCanvas");
				auto info = cursorCanvas->GetSpriteUI("Info");
				auto infoText = cursorCanvas->GetTextUI("InfoText");

				info->SetEnable(true);
				infoText->SetEnable(true);
			});
		rumButton->SetHoverEvent([&]
			{
				auto cursorCanvas = UIManager::GetInstance()->GetCanvas("CursorCanvas");
				auto pos = cursorCanvas->GetSpriteUI("Cursor")->GetPosition();
				auto infoText = cursorCanvas->GetTextUI("InfoText");
				auto scale = cursorCanvas->GetScreenScale();
				
				infoText->SetText(L"RUM");
				infoText->SetPosition({ pos.x + (72.f * scale.x), pos.y + (37.5f * scale.x), 0.0001f });
			});
		rumButton->SetOutEvent([&]
			{
				auto cursorCanvas = UIManager::GetInstance()->GetCanvas("CursorCanvas");
				auto info = cursorCanvas->GetSpriteUI("Info");
				auto infoText = cursorCanvas->GetTextUI("InfoText");

				info->SetEnable(false);
				infoText->SetText(L"");
				infoText->SetEnable(false);
			});

		auto vodkaButton = canvas->CreateButtonUI("VodkaButton");
		vodkaButton->SetPosition(Vector3{ -661.f, 389.f, 0.5f });
		vodkaButton->SetDefaultTexture(vodkaButtonTexture);
		vodkaButton->SetWidth(113.f);
		vodkaButton->SetHeight(212.f);
		vodkaButton->SetEnterEvent([&]
			{
				auto cursorCanvas = UIManager::GetInstance()->GetCanvas("CursorCanvas");
				auto info = cursorCanvas->GetSpriteUI("Info");
				auto infoText = cursorCanvas->GetTextUI("InfoText");

				info->SetEnable(true);
				infoText->SetEnable(true);
			});
		vodkaButton->SetHoverEvent([&]
			{
				auto cursorCanvas = UIManager::GetInstance()->GetCanvas("CursorCanvas");
				auto pos = cursorCanvas->GetSpriteUI("Cursor")->GetPosition();
				auto infoText = cursorCanvas->GetTextUI("InfoText");
				auto scale = cursorCanvas->GetScreenScale();
				
				infoText->SetText(L"VODKA");
				infoText->SetPosition({ pos.x + (57.f * scale.x), pos.y + (37.5f * scale.x), 0.0001f });
			});
		vodkaButton->SetOutEvent([&]
			{
				auto cursorCanvas = UIManager::GetInstance()->GetCanvas("CursorCanvas");
				auto info = cursorCanvas->GetSpriteUI("Info");
				auto infoText = cursorCanvas->GetTextUI("InfoText");

				info->SetEnable(false);
				infoText->SetText(L"");
				infoText->SetEnable(false);
			});

		auto ginButton = canvas->CreateButtonUI("GinButton");
		ginButton->SetPosition(Vector3{ -481.f, 373.f, 0.5f });
		ginButton->SetDefaultTexture(ginButtonTexture);
		ginButton->SetWidth(111.f);
		ginButton->SetHeight(243.f);
		ginButton->SetEnterEvent([&]
			{
				auto cursorCanvas = UIManager::GetInstance()->GetCanvas("CursorCanvas");
				auto info = cursorCanvas->GetSpriteUI("Info");
				auto infoText = cursorCanvas->GetTextUI("InfoText");

				info->SetEnable(true);
				infoText->SetEnable(true);
			});
		ginButton->SetHoverEvent([&]
			{
				auto cursorCanvas = UIManager::GetInstance()->GetCanvas("CursorCanvas");
				auto pos = cursorCanvas->GetSpriteUI("Cursor")->GetPosition();
				auto infoText = cursorCanvas->GetTextUI("InfoText");
				auto scale = cursorCanvas->GetScreenScale();
				
				infoText->SetText(L"GIN");
				infoText->SetPosition({ pos.x + (76.f * scale.x), pos.y + (37.5f * scale.x), 0.0001f });
			});
		ginButton->SetOutEvent([&]
			{
				auto cursorCanvas = UIManager::GetInstance()->GetCanvas("CursorCanvas");
				auto info = cursorCanvas->GetSpriteUI("Info");
				auto infoText = cursorCanvas->GetTextUI("InfoText");

				info->SetEnable(false);
				infoText->SetText(L"");
				infoText->SetEnable(false);
			});

		auto liquorButton = canvas->CreateButtonUI("LiquorButton");
		liquorButton->SetPosition(Vector3{ -298.f, 375.f, 0.5f });
		liquorButton->SetDefaultTexture(liquorButtonTexture);
		liquorButton->SetWidth(85.f);
		liquorButton->SetHeight(239.f);
		liquorButton->SetEnterEvent([&]
			{
				auto cursorCanvas = UIManager::GetInstance()->GetCanvas("CursorCanvas");
				auto info = cursorCanvas->GetSpriteUI("Info");
				auto infoText = cursorCanvas->GetTextUI("InfoText");

				info->SetEnable(true);
				infoText->SetEnable(true);
			});
		liquorButton->SetHoverEvent([&]
			{
				auto cursorCanvas = UIManager::GetInstance()->GetCanvas("CursorCanvas");
				auto pos = cursorCanvas->GetSpriteUI("Cursor")->GetPosition();
				auto infoText = cursorCanvas->GetTextUI("InfoText");
				auto scale = cursorCanvas->GetScreenScale();
				
				infoText->SetText(L"LIQUOR");
				infoText->SetPosition({ pos.x + (53.f * scale.x), pos.y + (37.5f * scale.x), 0.0001f });
			});
		liquorButton->SetOutEvent([&]
			{
				auto cursorCanvas = UIManager::GetInstance()->GetCanvas("CursorCanvas");
				auto info = cursorCanvas->GetSpriteUI("Info");
				auto infoText = cursorCanvas->GetTextUI("InfoText");

				info->SetEnable(false);
				infoText->SetText(L"");
				infoText->SetEnable(false);
			});

		auto juiceButton = canvas->CreateButtonUI("JuiceButton");
		juiceButton->SetPosition(Vector3{ -112.f, 422.f, 0.5f });
		juiceButton->SetDefaultTexture(juiceButtonTexture);
		juiceButton->SetWidth(106.f);
		juiceButton->SetHeight(145.f);
		juiceButton->SetEnterEvent([&]
			{
				auto cursorCanvas = UIManager::GetInstance()->GetCanvas("CursorCanvas");
				auto info = cursorCanvas->GetSpriteUI("Info");
				auto infoText = cursorCanvas->GetTextUI("InfoText");

				info->SetEnable(true);
				infoText->SetEnable(true);
			});
		juiceButton->SetHoverEvent([&]
			{
				auto cursorCanvas = UIManager::GetInstance()->GetCanvas("CursorCanvas");
				auto pos = cursorCanvas->GetSpriteUI("Cursor")->GetPosition();
				auto infoText = cursorCanvas->GetTextUI("InfoText");
				auto scale = cursorCanvas->GetScreenScale();
				
				infoText->SetText(L"JUICE");
				infoText->SetPosition({ pos.x + (63.f * scale.x), pos.y + (37.5f * scale.x), 0.0001f });
			});
		juiceButton->SetOutEvent([&]
			{
				auto cursorCanvas = UIManager::GetInstance()->GetCanvas("CursorCanvas");
				auto info = cursorCanvas->GetSpriteUI("Info");
				auto infoText = cursorCanvas->GetTextUI("InfoText");

				info->SetEnable(false);
				infoText->SetText(L"");
				infoText->SetEnable(false);
			});


		auto sodaButton = canvas->CreateButtonUI("SodaButton");
		sodaButton->SetPosition(Vector3{ 75.f, 399.f, 0.5f });
		sodaButton->SetDefaultTexture(sodaButtonTexture);
		sodaButton->SetWidth(86.f);
		sodaButton->SetHeight(191.f);
		sodaButton->SetEnterEvent([&]
			{
				auto cursorCanvas = UIManager::GetInstance()->GetCanvas("CursorCanvas");
				auto info = cursorCanvas->GetSpriteUI("Info");
				auto infoText = cursorCanvas->GetTextUI("InfoText");

				info->SetEnable(true);
				infoText->SetEnable(true);
			});
		sodaButton->SetHoverEvent([&]
			{
				auto cursorCanvas = UIManager::GetInstance()->GetCanvas("CursorCanvas");
				auto pos = cursorCanvas->GetSpriteUI("Cursor")->GetPosition();
				auto infoText = cursorCanvas->GetTextUI("InfoText");
				auto scale = cursorCanvas->GetScreenScale();

				infoText->SetText(L"SODA");
				infoText->SetPosition({ pos.x + (66.f * scale.x), pos.y + (37.5f * scale.x), 0.0001f });
			});
		sodaButton->SetOutEvent([&]
			{
				auto cursorCanvas = UIManager::GetInstance()->GetCanvas("CursorCanvas");
				auto info = cursorCanvas->GetSpriteUI("Info");
				auto infoText = cursorCanvas->GetTextUI("InfoText");

				info->SetEnable(false);
				infoText->SetText(L"");
				infoText->SetEnable(false);
			});

		auto fruitButton = canvas->CreateButtonUI("FruitButton");
		fruitButton->SetPosition(Vector3{ 231.f, 424.f, 0.5f });
		fruitButton->SetDefaultTexture(fruitButtonTexture);
		fruitButton->SetWidth(123.f);
		fruitButton->SetHeight(141.f);
		fruitButton->SetEnterEvent([&]
			{
				auto cursorCanvas = UIManager::GetInstance()->GetCanvas("CursorCanvas");
				auto info = cursorCanvas->GetSpriteUI("Info");
				auto infoText = cursorCanvas->GetTextUI("InfoText");

				info->SetEnable(true);
				infoText->SetEnable(true);
			});
		fruitButton->SetHoverEvent([&]
			{
				auto cursorCanvas = UIManager::GetInstance()->GetCanvas("CursorCanvas");
				auto pos = cursorCanvas->GetSpriteUI("Cursor")->GetPosition();
				auto infoText = cursorCanvas->GetTextUI("InfoText");
				auto scale = cursorCanvas->GetScreenScale();

				infoText->SetText(L"FRUIT");
				infoText->SetPosition({ pos.x + (63.f * scale.x), pos.y + (37.5f * scale.x), 0.0001f });
			});
		fruitButton->SetOutEvent([&]
			{
				auto cursorCanvas = UIManager::GetInstance()->GetCanvas("CursorCanvas");
				auto info = cursorCanvas->GetSpriteUI("Info");
				auto infoText = cursorCanvas->GetTextUI("InfoText");

				info->SetEnable(false);
				infoText->SetText(L"");
				infoText->SetEnable(false);
			});

		//==================(ReSet & MixButton)===================================
		auto mixButton = canvas->CreateButtonUI("MixButton");
		mixButton->SetPosition(Vector3{ 840.f, 483.f, 0.5f });
		mixButton->SetDefaultTexture(mixbuttonTexture);
		mixButton->SetWidth(163.f);
		mixButton->SetHeight(116.f);

		auto reSetButton = canvas->CreateButtonUI("ResetButton");
		reSetButton->SetPosition(Vector3{ 470.f, 483.f, 0.5f });
		reSetButton->SetDefaultTexture(reSetbuttonTexture);
		reSetButton->SetWidth(163.f);
		reSetButton->SetHeight(116.f);

		auto goldSpriteUI = canvas->CreateSpriteUI("GoldSpriteUI");
		goldSpriteUI->SetTexture(goldTexture);
		goldSpriteUI->SetPosition(Vector3{ 788.f, 363.f, 0.5f });
		goldSpriteUI->SetWidth(250.f);
		goldSpriteUI->SetHeight(61.f);

		auto goldText = canvas->CreateTextUI("GoldText");
		std::wstring goldTextString = L"";
		goldText->SetPosition(Vector3{ 844.f, 360.f, 0.5f });
		goldText->SetText(goldTextString);
		goldText->SetFont("NanumSquareRoundBold");
		goldText->SetAlignment(TextAlignment::LEFT);
		goldText->SetColor(Vector4{ powf((224.f / 255.f), 2.2f), powf((220.f / 255.f), 2.2f), powf((215.f / 255.f), 2.2f), 1.f });
		goldText->SetFontSize(24.f);

		//==================(BeerMaChine)===================================
		auto beerMachineButton = canvas->CreateButtonUI("BeerMachineButton");
		beerMachineButton->SetPosition(Vector3{ 656.f, 483.f, 0.5f });
		beerMachineButton->SetDefaultTexture(beerMachineTexture);
		beerMachineButton->SetWidth(163.f);
		beerMachineButton->SetHeight(116.f);

		//==================(RecipeBackGround & Recipes)===================================
		auto recipeBackGroundSpriteUI = canvas->CreateSpriteUI("RecipeBackGroundSpriteUI");
		recipeBackGroundSpriteUI->SetTexture(recipeBackGroundTexture);
		recipeBackGroundSpriteUI->SetPosition(Vector3{ -733.f, -168.f, 0.49f });
		recipeBackGroundSpriteUI->SetWidth(491.f);
		recipeBackGroundSpriteUI->SetHeight(849.f);

		//==================(CocktailrecipeButton)===================================
		auto cocktailRecipeButton = canvas->CreateButtonUI("CocktailRecipeButton");
		cocktailRecipeButton->SetPosition(Vector3{ -885.f, 193.f, 0.48f });
		cocktailRecipeButton->SetDefaultTexture(cocktailrecipeTexture);
		cocktailRecipeButton->SetWidth(90.f);
		cocktailRecipeButton->SetHeight(90.f);

		// 레시피 버튼들
		auto rumRecipeButton = canvas->CreateButtonUI("RumRecipeButton");
		rumRecipeButton->SetPosition(Vector3{ -824.f, -450.f, 0.47f });
		rumRecipeButton->SetDefaultTexture(rumRecipeButtonTexture);
		rumRecipeButton->SetWidth(57.f);
		rumRecipeButton->SetHeight(24.f);

		auto vodkaRecipeButton = canvas->CreateButtonUI("VodkaRecipeButton");
		vodkaRecipeButton->SetPosition(Vector3{ -727.f, -450.f, 0.47f });
		vodkaRecipeButton->SetDefaultTexture(vodkaRecipeButtonTexture);
		vodkaRecipeButton->SetWidth(82.f);
		vodkaRecipeButton->SetHeight(24.f);

		auto ginRecipeButton = canvas->CreateButtonUI("GinRecipeButton");
		ginRecipeButton->SetPosition(Vector3{ -636.f, -450.f, 0.47f });
		ginRecipeButton->SetDefaultTexture(ginRecipeButtonTexture);
		ginRecipeButton->SetWidth(43.f);
		ginRecipeButton->SetHeight(24.f);

		auto rumRecipeSpriteUI = canvas->CreateSpriteUI("RumRecipeSpriteUI");
		rumRecipeSpriteUI->SetTexture(rumRecipeTexture);
		rumRecipeSpriteUI->SetPosition(Vector3{ -770.f, -170.f, 0.47f });
		rumRecipeSpriteUI->SetWidth(290.f);
		rumRecipeSpriteUI->SetHeight(487.f);
		rumRecipeSpriteUI->SetHeight(487.f);
		rumRecipeSpriteUI->SetEnable(false);

		auto vodkaRecipeSpriteUI = canvas->CreateSpriteUI("VodkaRecipeSpriteUI");
		vodkaRecipeSpriteUI->SetTexture(vodkaRecipeTexture);
		vodkaRecipeSpriteUI->SetPosition(Vector3{ -770.f, -168.f, 0.47f });
		vodkaRecipeSpriteUI->SetWidth(290.f);
		vodkaRecipeSpriteUI->SetHeight(487.f);
		vodkaRecipeSpriteUI->SetEnable(false);

		auto ginRecipeSpriteUI = canvas->CreateSpriteUI("GinRecipeSpriteUI");
		ginRecipeSpriteUI->SetTexture(ginRecipeTexture);
		ginRecipeSpriteUI->SetPosition(Vector3{ -770.f, -168.f, 0.47f });
		ginRecipeSpriteUI->SetWidth(290.f);
		ginRecipeSpriteUI->SetHeight(487.f);
		ginRecipeSpriteUI->SetEnable(false);


		// 여기서 그냥 버튼 이벤트를 바인딩 해도 되지 않을까
			// 어차피 클릭하면 저 스프라이트가 나와야하니까.
		rumRecipeButton->SetClickEvent([&]
			{
				auto barcanvas = UIManager::GetInstance()->GetCanvas("BarCanvas");
				// 럼을 눌렀으니 럼만 활성화 시켜줍니다.
				barcanvas->GetSpriteUI("RumRecipeSpriteUI")->SetEnable(true);
				barcanvas->GetSpriteUI("VodkaRecipeSpriteUI")->SetEnable(false);
				barcanvas->GetSpriteUI("GinRecipeSpriteUI")->SetEnable(false);
			});

		vodkaRecipeButton->SetClickEvent([&]
			{
				auto barcanvas = UIManager::GetInstance()->GetCanvas("BarCanvas");
				// 럼을 눌렀으니 럼만 활성화 시켜줍니다.
				barcanvas->GetSpriteUI("RumRecipeSpriteUI")->SetEnable(false);
				barcanvas->GetSpriteUI("VodkaRecipeSpriteUI")->SetEnable(true);
				barcanvas->GetSpriteUI("GinRecipeSpriteUI")->SetEnable(false);
			});

		ginRecipeButton->SetClickEvent([&]
			{
				auto barcanvas = UIManager::GetInstance()->GetCanvas("BarCanvas");
				// 럼을 눌렀으니 럼만 활성화 시켜줍니다.
				barcanvas->GetSpriteUI("RumRecipeSpriteUI")->SetEnable(false);
				barcanvas->GetSpriteUI("VodkaRecipeSpriteUI")->SetEnable(false);
				barcanvas->GetSpriteUI("GinRecipeSpriteUI")->SetEnable(true);
			});

		// 텍스트 배경
		auto informationBackGround = canvas->CreateSpriteUI("InformationBackGround");
		informationBackGround->SetTexture(informationBackGroundTexture);
		informationBackGround->SetPosition(Vector3{ 0.0f, 0.0f, 0.45f });
		informationBackGround->SetMaskAlpha(0.8f);
		informationBackGround->SetWidth(100.f);
		informationBackGround->SetHeight(100.f);
		informationBackGround->SetEnable(false);

		// 레시피 버튼의 경우 위치가 같다고 가정합니다..
		for (int i = 0; i < 8; i++)
		{
			std::string uiIdx = to_string(i);
			auto recipeButton = canvas->CreateButtonUI("InformationButton" + uiIdx);
			recipeButton->SetDefaultTexture(informationButtonTexture);
			recipeButton->SetWidth(410.f);
			recipeButton->SetHeight(67.f);
			recipeButton->SetEnable(true);

			switch (i)
			{
			case 0:
				recipeButton->SetPosition(Vector3{ -719.f, -390.f, 0.45f });
				break;
			case 1:
				recipeButton->SetPosition({ -719.f, -328.f, 0.45f });
				break;
			case 2:
				recipeButton->SetPosition({ -719.f, -267.f, 0.45f });
				break;
			case 3:
				recipeButton->SetPosition({ -719.f, -206.f, 0.45f });
				break;
			case 4:
				recipeButton->SetPosition({ -719.f, -144.f, 0.45f });
				break;
			case 5:
				recipeButton->SetPosition({ -719.f, -81.f, 0.45f });
				break;
			case 6:
				recipeButton->SetPosition({ -719.f, -19.f, 0.45f });
				break;
			case 7:
				recipeButton->SetPosition({ -719.f, 43.f, 0.45f });
				break;
			default:
				break;
			}
		}

		auto informationText = canvas->CreateTextUI("InformationText");
		informationText->SetPosition(Vector3{ -321.f, -393.f, 0.4f });
		informationText->SetText(L"");
		informationText->SetFont("NanumSquareRoundRegular");
		informationText->SetFontSize(18.f);
		informationText->SetFixedWidth(250);

		// stuff amount
		for (int i = 0; i < 7; i++)
		{
			std::string amountIdx = to_string(i);

			auto amountText = canvas->CreateTextUI("AmountText" + amountIdx);

			amountText->SetPosition(Vector3{ -775.0f, 334.0f, 0.4f });
			amountText->SetText(L"0");
			amountText->SetFont("NanumSquareRoundBold");
			amountText->SetColor(Vector4{ powf((101.f / 255.f), 2.2f), powf((67.f / 255.f), 2.2f), powf((33.f / 255.f), 2.2f), 1.f });
			amountText->SetFontSize(30.f);
		}

		//==================(SoldOutButton)===================================
		auto soldOutButton = canvas->CreateButtonUI("SoldOutButton");
		soldOutButton->SetPosition(Vector3{ 556.f, 366.f, 0.5f });
		soldOutButton->SetDefaultTexture(buttonTexture);
		soldOutButton->SetWidth(130.f);
		soldOutButton->SetHeight(60.f);
		soldOutButton->SetEnable(false);

		// 대화창까지 넣을거다.
		auto reQuestSpriteUI = canvas->CreateSpriteUI("ReQuestSpriteUI");
		reQuestSpriteUI->SetTexture(reQuestTexture);
		reQuestSpriteUI->SetPosition(Vector3{ 604.0f, -326.0f, 0.5f });
		reQuestSpriteUI->SetWidth(619.f);
		reQuestSpriteUI->SetHeight(256.f);
		reQuestSpriteUI->SetEnable(false);

		// 이름도 사실상 하나면 됩니다.
		auto nameText = canvas->CreateTextUI("nameText");

		nameText->SetPosition(Vector3{ 388.0f, -426.0f ,0.4f });
		nameText->SetText(L"");
		nameText->SetFont("NanumSquareNeoBold");
		nameText->SetFixedWidth(58.f);
		nameText->SetFontSize(30.f);
		nameText->SetEnable(false);

		auto customerdialogText = canvas->CreateTextUI("CustomerdialogText");
		customerdialogText->SetPosition(Vector3{ 490.0f, -354.0f, 0.4f });
		customerdialogText->SetText(L"");
		customerdialogText->SetFont("NanumSquareRoundRegular");
		customerdialogText->SetFontSize(25.f);
		customerdialogText->SetFixedWidth(250);
		customerdialogText->SetEnable(false);

		// 선택 텍스트 + 스프라이트
		for (int i = 0; i < 3; i++)
		{
			std::string idx = to_string(i);

			auto selectText = canvas->CreateTextUI("SelectText" + idx);
			auto selectSprite = canvas->CreateSpriteUI("SelectMixSprite" + idx);

			/*if (i == 0)
			{
				selectText->SetText(L"RECIPE");
				selectText->SetPosition(Vector3{ 492.0f, 283.0f, 0.45f });
			}
			else if (i == 1)
			{
				selectText->SetText(L"NOT");
				selectText->SetPosition(Vector3{ 662.0f, 283.0f, 0.45f });
			}
			else
			{
				selectText->SetText(L"FOUND");
				selectText->SetPosition(Vector3{ 844.0f, 283.0f, 0.45f });
			}*/

			if (i == 0)
			{
				selectText->SetText(L"");

				selectSprite->SetWidth(sodaButtonTexture->GetWidth() * 0.2f);
				selectSprite->SetHeight(sodaButtonTexture->GetHeight() * 0.2f);
			}
			else if (i == 1)
			{
				selectText->SetText(L"");

				selectSprite->SetWidth(fruitButtonTexture->GetWidth() * 0.2f);
				selectSprite->SetHeight(fruitButtonTexture->GetHeight() * 0.2f);
			}
			else
			{
				selectText->SetText(L"");

				selectSprite->SetWidth(liquorButtonTexture->GetWidth() * 0.2f);
				selectSprite->SetHeight(liquorButtonTexture->GetHeight() * 0.2f);
			}

			selectText->SetPosition(Vector3{ 485.0f + 180.f * i, 283.0f, 0.45f });
			selectText->SetFixedWidth(70.f);


			selectText->SetFont("NanumSquareRoundBold");
			selectText->SetColor(Vector4{ 1.f, 1.f, 1.f, 1.f });
			selectText->SetFontSize(20.f);

			selectSprite->SetPosition(Vector3{ 425.0f + 180.f * i, 285.0f, 0.4f });
		}

		canvas->SetEnable(false);
	}

	void UICreator::CreateShakeCanvas()
	{
		auto canvas = mainCanvas->GetCanvasUI("ShakeCanvas");

		auto countTex = graphicsManager->LoadTexture(L"Resources/UI/informationBackGround.png");

		auto miniGameTexture = graphicsManager->LoadTexture(L"Resources/UI/Mini_game.png");

		auto miniGameShakerTexture = graphicsManager->LoadTexture(L"Resources/UI/Mini_game3.png");

		auto miniGameArrowTexture = graphicsManager->LoadTexture(L"Resources/UI/Mini_game_cursor.png");

		auto miniGameHpBackground = graphicsManager->LoadTexture(L"Resources/UI/Mini_game_barBG.png");

		auto miniGameHp = graphicsManager->LoadTexture(L"Resources/UI/Mini_game_bar.png");

		auto fTexture = graphicsManager->LoadTexture(L"Resources/UI/FButton.png");

		//==================(MiniGameBackGround)===================================
		auto miniGameSpriteUI = canvas->CreateSpriteUI("MiniGameSpriteUI");
		miniGameSpriteUI->SetTexture(miniGameTexture);
		miniGameSpriteUI->SetPosition(Vector3{ 0.f, 0.f, 0.4f });
		miniGameSpriteUI->SetWidth(1920.f);
		miniGameSpriteUI->SetHeight(1080.f);

		auto miniGameShakerSpriteUI = canvas->CreateSpriteUI("MiniGameShakerSpriteUI");
		miniGameShakerSpriteUI->SetTexture(miniGameShakerTexture);
		miniGameShakerSpriteUI->SetPosition(Vector3{ 4.f, -40.f, 0.29f });
		miniGameShakerSpriteUI->SetUsingRotation(true);
		miniGameShakerSpriteUI->SetWidth(300.f);
		miniGameShakerSpriteUI->SetHeight(290.f);

		auto miniGameHpUI = canvas->CreateProgressBarUI("MiniGameHp");
		miniGameHpUI->SetBackgroundTexture(miniGameHpBackground);
		miniGameHpUI->SetBarTexture(miniGameHp);
		miniGameHpUI->SetFillDirection(FillDir::UP);
		miniGameHpUI->SetWidth(68.f);
		miniGameHpUI->SetHeight(861.f);
		miniGameHpUI->SetPosition(Vector3{ 657.f, 4.f, 0.29f });
		miniGameHpUI->SetBarTexRect(0.f, 0.f, 68.f, 861.f);
		miniGameHpUI->SetFillPercent(1.0f);

		//==================(InteractionUI)============================== 
		auto fSpriteUI = canvas->CreateSpriteUI("F");
		fSpriteUI->SetTexture(fTexture);
		fSpriteUI->SetPosition(Vector3{ 0.0f, 400.0f, 0.3f });
		fSpriteUI->SetWidth(100.f);
		fSpriteUI->SetHeight(100.f);

		// 재료 순서 세 가지. 미니 게임 세가지.
		// 위의 것 다 필요 없습니다..
		for (int i = 0; i < 3; i++)
		{
			std::string icon = "SelectSprite" + to_string(i);
			std::string minigame = "MiniGameSprite" + to_string(i);
			// 표시용
			auto iconSprite = canvas->CreateSpriteUI(icon);
			iconSprite->SetPosition(Vector3{ 0.f, 0.f, 0.25f });
			// 미니 게임용
			auto miniGameSprite = canvas->CreateSpriteUI(minigame);
			miniGameSprite->SetPosition(Vector3{ 0.f, 0.f, 0.25f });
			miniGameSprite->SetUsingRotation(true);

			// 순서 알려주는 텍스트.
			std::string number = "NumberText" + to_string(i);
			std::wstring tmpNum = to_wstring(i + 1);
			auto numberText = canvas->CreateTextUI(number);
			numberText->SetPosition(Vector3{ 0.0f, 0.0f, 0.25f });
			numberText->SetText(tmpNum);
			numberText->SetFont("NanumSquareNeoBold");
			numberText->SetFontSize(30.f);
		}

		auto miniGameArrowUI = canvas->CreateSpriteUI("MiniGameArrow");
		miniGameArrowUI->SetTexture(miniGameArrowTexture);
		miniGameArrowUI->SetPosition(Vector3{ 0.0f, 0.0f, 0.2f });
		miniGameArrowUI->SetWidth(100.f);
		miniGameArrowUI->SetHeight(100.f);

		// 가장 위에서 가립시다.
		auto miniGameCount = canvas->CreateSpriteUI("CountBackGround");
		miniGameCount->SetTexture(countTex);
		miniGameCount->SetPosition(Vector3{ 0.f, 0.f, 0.1f });
		miniGameCount->SetMaskAlpha(0.8f);
		miniGameCount->SetWidth(1920.f);
		miniGameCount->SetHeight(1080.f);

		auto miniGameCountText = canvas->CreateTextUI("CountText");
		miniGameCountText->SetFont("NanumSquareNeoBold");
		miniGameCountText->SetPosition(Vector3{ 0.f, 0.f, 0.09f });
		miniGameCountText->SetText(L"3");
		miniGameCountText->SetFontSize(128.f);

		canvas->SetEnable(false);
	}

	void UICreator::CreateResultCanvas()
	{
		auto canvas = mainCanvas->GetCanvasUI("ResultCanvas");

		auto mixFirstResultTex = graphicsManager->LoadTexture(L"Resources/UI/SceneBackGround.png");

		auto mixResultTexture = graphicsManager->LoadTexture(L"Resources/UI/Mini_game_MixResult.png");

		auto offerButtonTexture = graphicsManager->LoadTexture(L"Resources/UI/OfferButton.png");

		//==================(ResultTextrue)===================================
		auto mixResultSpriteUI = canvas->CreateSpriteUI("MixResultSpriteUI");
		mixResultSpriteUI->SetTexture(mixResultTexture);
		mixResultSpriteUI->SetPosition(Vector3{ 0.0f, 0.0f, 0.4f });
		mixResultSpriteUI->SetWidth(1920.f);
		mixResultSpriteUI->SetHeight(1080.f);

		auto firstResult = canvas->CreateSpriteUI("MixFirstBG");
		firstResult->SetTexture(mixFirstResultTex);
		firstResult->SetPosition(Vector3{ 0.0f, 0.0f, 0.2f });
		firstResult->SetWidth(1920.f);
		firstResult->SetHeight(1080.f);

		auto offerButton = canvas->CreateButtonUI("OfferButton");
		offerButton->SetPosition(Vector3{ 3.0f, 330.0f, 0.4f });
		offerButton->SetDefaultTexture(offerButtonTexture);
		offerButton->SetWidth(247.f);
		offerButton->SetHeight(135.f);
		offerButton->SetEnable(false);

		// 최종 결과 스프라이트 두 개.
		auto mixCockTailUI = canvas->CreateSpriteUI("MixCockTailSpriteUI");
		mixCockTailUI->SetPosition(Vector3{ 0.f, 9.f, 0.19f });
		// 사실 이건 텍스쳐 정보 가져와서 세팅 해줘도 된다.
		mixCockTailUI->SetWidth(109.f);
		mixCockTailUI->SetHeight(390.f);
		mixCockTailUI->SetMaskAlpha(0.f);

		// 최종결과 이름
		auto mixCockTailNameUI = canvas->CreateSpriteUI("MixCockTailNameUI");
		mixCockTailNameUI->SetPosition(Vector3{ 0.f, -337.f, 0.18f });
		mixCockTailNameUI->SetWidth(811.f);
		mixCockTailNameUI->SetHeight(82.f);
		mixCockTailNameUI->SetMaskAlpha(0.f);

		canvas->SetEnable(false);
	}

	void UICreator::CreateInMenuCanvas()
	{
		auto canvas = mainCanvas->GetCanvasUI("InGameMenuCanvas");

		auto menuBackTexture = graphicsManager->LoadTexture(L"Resources/UI/phone.png");

		auto phoneBackTexture = graphicsManager->LoadTexture(L"Resources/UI/MenuClosebuttonHover.png");

		auto pauseBackGroundtexture = graphicsManager->LoadTexture(L"Resources/UI/PauseBG.png");

		auto inventoryButtonTexture = graphicsManager->LoadTexture(L"Resources/UI/MenuInventoryButton.png");

		auto cotrolButtonTexture = graphicsManager->LoadTexture(L"Resources/UI/MenuControlButton.png");

		auto settingButtonTexture = graphicsManager->LoadTexture(L"Resources/UI/MenuSettingButton.png");

		auto lobbyButtonTexture = graphicsManager->LoadTexture(L"Resources/UI/MenuReturnButton.png");

		auto settingBackgroundtexture = graphicsManager->LoadTexture(L"Resources/UI/SettingBackGroundUI.png");

		auto controlTexture = graphicsManager->LoadTexture(L"Resources/UI/MenuControl.png");

		auto inventoryTexture = graphicsManager->LoadTexture(L"Resources/UI/Inventory.png");

		auto itemResultTexture = graphicsManager->LoadTexture(L"Resources/UI/ItemResult.png");

		auto slotTexture = graphicsManager->LoadTexture(L"Resources/UI/UILim.png");

		auto closebuttonTexture = graphicsManager->LoadTexture(L"Resources/UI/ClosebuttonHover.png");


		auto  menupauseBackGroundSpriteUI = canvas->CreateButtonUI("MenuPauseBackGroundSpriteUI");
		menupauseBackGroundSpriteUI->SetDefaultTexture(pauseBackGroundtexture);
		menupauseBackGroundSpriteUI->SetPosition(Vector3{ 0.0f, 0.0f, 0.06f });
		menupauseBackGroundSpriteUI->SetHoverMaskColor(1.0f, 1.0f, 1.0f, 1.0f);
		menupauseBackGroundSpriteUI->SetWidth(1920.0f);
		menupauseBackGroundSpriteUI->SetHeight(1080.0f);



		//==================(InGameMenuBackGround)===============================
		auto  menuBackground = canvas->CreateSpriteUI("Menu");
		menuBackground->SetTexture(menuBackTexture);
		menuBackground->SetPosition(Vector3{ 46.f, 38.f, 0.05f });
		menuBackground->SetWidth(820.0f);
		menuBackground->SetHeight(1023.0f);




		//==================(InGameMenuTimeText)===============================
		for (int i = 0; i < 2; i++)
		{
			int idx = i;
			std::string timeIdx = to_string(idx);

			auto menuTimeText = canvas->CreateTextUI("MenuTimeText" + timeIdx);
			menuTimeText->SetText("00");
			menuTimeText->SetPosition(Vector3{ 46.f, 38.f, 0.05f });
			menuTimeText->SetFontSize(65.f);
			menuTimeText->SetFont("NanumSquareRoundBold");
			menuTimeText->SetEnable(true);

			switch (idx)
			{
			case 0:
				menuTimeText->SetPosition(Vector3{ -23.f, -295.f, 0.05f });
				break;
			case 1:
				menuTimeText->SetPosition(Vector3{ 190.f, -295.f, 0.05f });
				break;
			default:
				break;
			}

		}


		auto timeSignText = canvas->CreateTextUI("TimeSignText");
		timeSignText->SetText(":");
		timeSignText->SetPosition(Vector3{ 89.f, -295.f, 0.05f });
		timeSignText->SetFontSize(60.f);
		timeSignText->SetFont("NanumSquareRoundBold");
		


		//==================(InGameInventoryButton)===============================
		auto inventoryButton = canvas->CreateButtonUI("InventoryButton");
		inventoryButton->SetPosition(Vector3{ -100.f, -120.f, 0.05f });
		inventoryButton->SetDefaultTexture(inventoryButtonTexture);
		inventoryButton->SetWidth(140.f);
		inventoryButton->SetHeight(140.f);
		

		//==================(InGameControlButton)===============================
		auto cotrolButton = canvas->CreateButtonUI("ControlButton");
		cotrolButton->SetPosition(Vector3{ 85.f, -120.f, 0.05f });
		cotrolButton->SetDefaultTexture(cotrolButtonTexture);
		cotrolButton->SetWidth(140.f);
		cotrolButton->SetHeight(140.f);
		

		//==================(InGameSettingButton)===============================
		auto settingButton = canvas->CreateButtonUI("SettingButton");
		settingButton->SetPosition(Vector3{ 265.f, -120.f, 0.05f });
		settingButton->SetDefaultTexture(settingButtonTexture);
		settingButton->SetWidth(140.f);
		settingButton->SetHeight(140.f);
		

		//==================(InGameControlSpriteUI)===============================
		auto controlSpriteUI = canvas->CreateSpriteUI("ControlSpriteUI");
		controlSpriteUI->SetTexture(controlTexture);
		controlSpriteUI->SetPosition(Vector3{ 46.f, 38.f, 0.05f });
		controlSpriteUI->SetWidth(820.0f);
		controlSpriteUI->SetHeight(1023.0f);
		controlSpriteUI->SetEnable(false);


		//==================(InGameMenuInventory)===============================
		auto inventorySpriteUI = canvas->CreateSpriteUI("InventorySpriteUI");
		inventorySpriteUI->SetTexture(inventoryTexture);
		inventorySpriteUI->SetPosition(Vector3{ 46.f, 38.f, 0.05f });
		inventorySpriteUI->SetWidth(820.0f);
		inventorySpriteUI->SetHeight(1023.0f);
		inventorySpriteUI->SetEnable(false);

		for (int i = 0; i < 14; i++)
		{
			int index = i + 8;
			std::string slotIdx = to_string(index);

			auto slotSpriteUI = canvas->CreateButtonUI("SlotSpriteUI" + slotIdx);
			slotSpriteUI->SetDefaultTexture(slotTexture);

			slotSpriteUI->SetHoverMaskColor({ 1.0f, 1.0f, 1.0f, 1.0f });
			slotSpriteUI->SetPressMaskColor({ 1.0f, 1.0f, 1.0f, 1.0f });
			slotSpriteUI->SetPosition(Vector3{ -196.0f, -184.0f, 0.05f });
			slotSpriteUI->SetWidth(90.f);
			slotSpriteUI->SetHeight(90.f);
			slotSpriteUI->SetEnable(false);
		}



		for (int i = 0; i < 14; i++)
		{
			int index = i + 8;
			std::string stuffIdx = to_string(index);

			auto stuffTextUI = canvas->CreateTextUI("StuffTextUI" + stuffIdx);
			stuffTextUI->SetText("00");
			stuffTextUI->SetPosition(Vector3{ -800.f, -480.f, 0.05f });
			stuffTextUI->SetFontSize(25.f);
			stuffTextUI->SetFixedWidth(48.f);
			stuffTextUI->SetEnable(false);
		}

		auto descText = canvas->CreateTextUI("InventoryDescText");
		descText->SetText(L"");
		descText->SetPosition(Vector3{ 89.f, 228.f, 0.05f });
		descText->SetFontSize(25.f);
		descText->SetFixedWidth(528.f);
		descText->SetFont("NanumSquareRoundRegular");

		descText->SetEnable(false);

		auto nameText = canvas->CreateTextUI("InventoryNameText");
		nameText->SetText(L"");
		nameText->SetFixedWidth(135.f);
		nameText->SetPosition(Vector3{ -129.f, 126.f, 0.05f });
		nameText->SetFontSize(28.f);
		nameText->SetFont("NanumSquareRoundBold");
		nameText->SetEnable(false);

		auto  menuPhoneBackButton = canvas->CreateButtonUI("MenuPhoneBack");
		menuPhoneBackButton->SetPosition(Vector3{ 100.0f, 460.0f, 0.05f });
		menuPhoneBackButton->SetDefaultTexture(phoneBackTexture);
		menuPhoneBackButton->SetDefaultMaskColor(0.5f, 0.5f, 0.5f, 1.0f);
		menuPhoneBackButton->SetHoverMaskColor(1.0f, 1.0f, 1.0f, 1.0f);
		menuPhoneBackButton->SetWidth(37.f);
		menuPhoneBackButton->SetHeight(36.f);


		auto menuCloseButton = canvas->CreateButtonUI("MenuCloseButton");
		menuCloseButton->SetPosition(Vector3{ 270.f, 458.f, 0.05f });
		menuCloseButton->SetDefaultTexture(closebuttonTexture);
		menuCloseButton->SetDefaultMaskColor(0.5f, 0.5f, 0.5f, 1.0f);
		menuCloseButton->SetHoverMaskColor(1.0f, 1.0f, 1.0f, 1.0f);
		menuCloseButton->SetWidth(13.f);
		menuCloseButton->SetHeight(32.f);

		canvas->SetEnable(false);
	}
}
