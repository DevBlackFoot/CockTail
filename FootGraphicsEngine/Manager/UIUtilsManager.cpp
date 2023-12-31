#include "GraphicsPch.h"
#include "UIUtilsManager.h"
#include "ResourceManager.h"

using namespace ATL;

namespace GraphicsEngineSpace
{
	std::shared_ptr<UIUtilsManager> UIUtilsManager::instance = nullptr;


	UIUtilsManager::~UIUtilsManager()
	{

	}

	std::shared_ptr<UIUtilsManager> UIUtilsManager::GetInstance()
	{
		if (instance == nullptr)
			instance = std::make_shared<UIUtilsManager>();

		return instance;
	}

	void UIUtilsManager::Init(ComPtr<ID3D11Device> _device, std::shared_ptr<SpriteBatch> spriteBatch, ComPtr<ID3D11DepthStencilState> depthState, ComPtr
		<ID3D11BlendState> blendState)
	{
		device = _device;
		this->spriteBatch = spriteBatch;
		this->depthState = depthState;
		this->blendState = blendState;

		// 여기서 모든 폰트를 만들어줍니다.
		CreateSpriteFont("gulim", "Font/gulim9k.spritefont");
		CreateSpriteFont("EBS-Light", "Font/ebsLight.spritefont");

		// 세 폰트는 각각 Bold와 Italic을 추가해줍니다.
		CreateSpriteFont("NanumSquareNeoBold", "Font/NanumSquareNeoBold.spritefont");
		CreateSpriteFont("NanumSquareNeoBoldB", "Font/NanumSquareNeoBoldB.spritefont");
		CreateSpriteFont("NanumSquareNeoBoldI", "Font/NanumSquareNeoBoldI.spritefont");

		CreateSpriteFont("NanumSquareRoundBold", "Font/NanumSquareRoundBold.spritefont");
		CreateSpriteFont("NanumSquareRoundBoldB", "Font/NanumSquareRoundBoldB.spritefont");
		CreateSpriteFont("NanumSquareRoundBoldI", "Font/NanumSquareRoundBoldI.spritefont");

		CreateSpriteFont("NanumSquareRoundRegular", "Font/NanumSquareRoundRegular.spritefont");
		CreateSpriteFont("NanumSquareRoundRegularB", "Font/NanumSquareRoundRegularB.spritefont");
		CreateSpriteFont("NanumSquareRoundRegularI", "Font/NanumSquareRoundRegularI.spritefont");

		CreateSpriteFont("RecipeKorea", "Font/RecipeKorea.spritefont");
		CreateSpriteFont("RecipeKoreaB", "Font/RecipeKoreaB.spritefont");
		CreateSpriteFont("RecipeKoreaI", "Font/RecipeKoreaI.spritefont");



		// Lim Texture를 생성해서 ID를 가지고 있게 만듭니다.
		limTex = ResourceManager::GetInstance()->LoadTexture(L"Resources/UI/UILim.png");
	}

	std::shared_ptr<SpriteFont> UIUtilsManager::GetFont(const std::string& fontName)
	{
		if (fontMap.find(fontName) == fontMap.end())
			return nullptr;

		return fontMap.at(fontName);
	}

	void UIUtilsManager::Finalize()
	{
		for (auto font : fontMap)
		{
			font.second.reset();
		}

		fontMap.clear();

		SafeReset(spriteBatch)

			SafeReset(instance)
	}

	void UIUtilsManager::CreateSpriteFont(const std::string& name, const std::string& path)
	{
		TCHAR buffer[256] = {};

		_tcscpy_s(buffer, CA2T(path.c_str()));

		std::shared_ptr<SpriteFont> dxFont = std::make_shared<SpriteFont>(device.Get(), buffer);

		if (name == "RecipeKorea" || name == "RecipeKoreaB" || name == "RecipeKoreaI")
		{
			dxFont->SetLineSpacing(32.0f);
		}
		else
		{
			dxFont->SetLineSpacing(50.0f);
		}

		// 만들어진 폰트 맵에 넣어주기
		fontMap.insert(std::pair{ name, dxFont });
	}
}
