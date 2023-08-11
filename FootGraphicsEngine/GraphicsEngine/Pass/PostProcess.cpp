#include "GraphicsPch.h"
#include "PostProcess.h"

#include "PostProcessTexture.h"
#include "GraphicsCore/DX11GraphicsCore.h"
#include "GraphicsCore/RenderTargetTexture.h"
#include "GraphicsCore/DepthStencilResource.h"

#include "Object/Shader/VertexShader.h"
#include "Object/Shader/PixelShader.h"

#include "Manager/BufferManager.h"
#include "Manager/ImGUIManager.h"
#include "Manager/ShaderManager.h"
#include "Manager/ResourceManager.h"
#include "Manager/SamplerManager.h"
#include "Manager/UIUtilsManager.h"

#include "Object/Buffer/BufferBase.h"

#include "PostProcessTexture.h"
#include "Util/VertexStruct.h"

namespace GraphicsEngineSpace
{
	void PostProcess::Start(const std::shared_ptr<RenderTargetTexture>& mainRTV,
		const std::shared_ptr<DepthStencilResource>& mainDSV)
	{
		quadVS = std::dynamic_pointer_cast<VertexShader>(ShaderManager::GetInstance()->GetShader("QuadVS"));

		postProcessPS = std::dynamic_pointer_cast<PixelShader>(ShaderManager::GetInstance()->GetShader("PostProcessAllPS"));
		notPostProcessPS = std::dynamic_pointer_cast<PixelShader>(ShaderManager::GetInstance()->GetShader("NotPostProcessPS"));
		postProcessNotBloomPS = std::dynamic_pointer_cast<PixelShader>(ShaderManager::GetInstance()->GetShader("PostProcessNotBloomPS"));

		downPS = std::dynamic_pointer_cast<PixelShader>(ShaderManager::GetInstance()->GetShader("DownPS"));
		exceptDownPS = std::dynamic_pointer_cast<PixelShader>(ShaderManager::GetInstance()->GetShader("ExceptDownPS"));
		blurHPS = std::dynamic_pointer_cast<PixelShader>(ShaderManager::GetInstance()->GetShader("BlurHPS"));
		blurVPS = std::dynamic_pointer_cast<PixelShader>(ShaderManager::GetInstance()->GetShader("BlurVPS"));
		upSamplePS = std::dynamic_pointer_cast<PixelShader>(ShaderManager::GetInstance()->GetShader("UpSamplePS"));

		this->mainRTV = mainRTV;
		this->mainDSV = mainDSV;

		graphicsCore = DX11GraphicsCore::GetInstance();

		//reduceRTV.resize(downSampleSize);

		reduceTexture = std::make_shared<PostProcessTexture>();

		// Intensity
		vignetteBuffer.x = 0.25f;
		// Smoothness
		vignetteBuffer.y = 0.4f;

		// Blur Intensity
		vignetteBuffer.z = 0.45f;
		// Blur Smoothness
		vignetteBuffer.w = 0.13f;

		vignetteColorBuffer = DirectX::Colors::Black;

		// x : clamp, y : scatter, z : threshold, w : thresholdKnee
		// 아래의 값들은 unity의 값을 가져오고 있습니다.
		float scatter = std::lerp(0.05f, 0.95f, 0.7f);
		// 일단은 2.2 감마로 계산하겠습니다..
		float linearThreshold = powf(1.0f, 2.2f);

		bloomThresholdBuffer = SimpleMath::Vector4{ 65472.f, scatter, 1.0f, linearThreshold * 0.5f };

		fadeOffset = SimpleMath::Vector4{ 1.0f, 0.f, 0.f, 0.f };
		fadeSpeed = 0.f;
		fadeOffset.y = 1.0f;

		// Blend State
		D3D11_BLEND_DESC blendDesc = {};
		ZeroMemory(&blendDesc, sizeof(D3D11_BLEND_DESC));

		blendDesc.RenderTarget[0].BlendEnable = true;
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
		blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
		blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;

		bloom = true;
		allPostProcessing = true;

		HRESULT hr = graphicsCore->GetDevice()->CreateBlendState(&blendDesc, postBlendState.GetAddressOf());
		if (FAILED(hr))
			printf("Blend State Fail");
	}

	void PostProcess::Release()
	{
		SafeReset(postProcessPS);
	}

	void PostProcess::OnResize(int width, int height)
	{
		// 둘 모두의 사이즈가 0보다 클 때만.
		if (BloomMipUpRTV.size() > 0 && BloomMipDownRTV.size() > 0)
		{
			// 기존에 있던 것을 지워줍니다.
			for (int i = 0; i < downSampleSize; i++)
			{
				//reduceRTV[i]->Finalize();
				BloomMipUpRTV[i]->Finalize();
				BloomMipDownRTV[i]->Finalize();
			}
		}

		BloomMipUpRTV.clear();
		BloomMipDownRTV.clear();

		// 사이즈가 0보다 클 때만
		if (BlurMipUpRTV.size() > 0 && BlurMipDownRTV.size() > 0)
		{
			for (int i = 0; i < blurDownSize; i++)
			{
				BlurMipDownRTV[i]->Finalize();
				BlurMipUpRTV[i]->Finalize();
			}
		}

		BlurMipUpRTV.clear();
		BlurMipDownRTV.clear();

		// 들어오는 값에 따라 downSampleSize를 변경해줍니다. (ref. Unity)
			// 변경된 넓이, 높이를 1/2 해준 값..
		int tw = width >> 1;
		int th = height >> 1;

		int maxSize = max(tw, th);
		int iterations = static_cast<int>(floorf(log2f(maxSize) - 1.0f));
		iterations -= 1;
		// 16 => unity bloom Max mip 기본값.
		downSampleSize = std::clamp(iterations, 1, 16);
		blurDownSize = std::clamp(downSampleSize - 3, 1, 8);

		/*
		*/
		// reduceTexture를 만들어줍니다.
		reduceTexture->Finalize();
		reduceTexture->Init(D3DDevice, width, height, downSampleSize);

		// 변경된 사이즈에 맞게 다시 생성해줍니다.
		BloomMipUpRTV.resize(downSampleSize);
		BloomMipDownRTV.resize(downSampleSize);

		for (int i = 0; i < downSampleSize; i++)
		{
			BloomMipUpRTV[i] = std::make_shared<RenderTargetTexture>();
			BloomMipDownRTV[i] = std::make_shared<RenderTargetTexture>();
		}

		// 유니티와 다르게 우리는 미리 RenderTarget Texture를 만들어 둬야합니다.
		BloomMipDownRTV[0]->Init(D3DDevice, tw, th, graphicsCore->GetMSAAQuality());
		BloomMipUpRTV[0]->Init(D3DDevice, tw, th, graphicsCore->GetMSAAQuality());

		for (int i = 1; i < downSampleSize; i++)
		{
			// 새로운 사이즈 생성.
			tw = max(1, tw >> 1);
			th = max(1, th >> 1);

			BloomMipDownRTV[i]->Init(D3DDevice, tw, th, graphicsCore->GetMSAAQuality());
			BloomMipUpRTV[i]->Init(D3DDevice, tw, th, graphicsCore->GetMSAAQuality());
		}

		// 블러를 위해서 다시 해줍니다.
		tw = width >> 1;
		th = height >> 1;

		BlurMipUpRTV.resize(blurDownSize);
		BlurMipDownRTV.resize(blurDownSize);

		for (int i = 0; i < blurDownSize; i++)
		{
			BlurMipUpRTV[i] = std::make_shared<RenderTargetTexture>();
			BlurMipDownRTV[i] = std::make_shared<RenderTargetTexture>();
		}

		// 유니티와 다르게 우리는 미리 RenderTarget Texture를 만들어 둬야합니다.
		BlurMipDownRTV[0]->Init(D3DDevice, tw, th, graphicsCore->GetMSAAQuality());
		BlurMipUpRTV[0]->Init(D3DDevice, tw, th, graphicsCore->GetMSAAQuality());

		for (int i = 1; i < blurDownSize; i++)
		{
			// 새로운 사이즈 생성.
			tw = max(1, tw >> 1);
			th = max(1, th >> 1);

			BlurMipDownRTV[i]->Init(D3DDevice, tw, th, graphicsCore->GetMSAAQuality());
			BlurMipUpRTV[i]->Init(D3DDevice, tw, th, graphicsCore->GetMSAAQuality());
		}
	}

	void PostProcess::RenderStart()
	{
		// 그릴 부분을 깔끔하게 해줍니다..
		graphicsCore->ResetRenderTargetView(mainRTV->GetRenderTargetView(), Colors::AliceBlue);
		graphicsCore->ResetDepthStencilView(mainDSV->GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		//ID3D11DepthStencilView* depthStencil = { 0 };

		D3DDeviceContext->OMSetRenderTargets(1, mainRTV->GetRenderTargetView().GetAddressOf(), mainDSV->GetDepthStencilView().Get());

		// ViewPort 세팅을..해줍니다.
		D3DDeviceContext->RSSetViewports(1, mainDSV->GetViewport().get());

	}

	void PostProcess::Blur(const std::shared_ptr<RenderTargetTexture>& sourceTarget)
	{
		auto UIUtils = UIUtilsManager::GetInstance();
		auto spriteBatch = UIUtils->GetSpriteBatch();

		auto threshold = BufferManager::GetInstance()->GetBuffer("ThresholdCB");
		auto texInfo = BufferManager::GetInstance()->GetBuffer("TextureInfoCB");
		auto fadeoff = BufferManager::GetInstance()->GetBuffer("ViewPosCB");

		// 첫번째 다운 샘플.
		graphicsCore->ResetRenderTargetView(BlurMipDownRTV[0]->GetRenderTargetView(), Colors::Black);
		ID3D11DepthStencilView* nulldepth = { 0 };

		// 렌더 타겟 세팅
		D3DDeviceContext->OMSetRenderTargets(1, BlurMipDownRTV[0]->GetRenderTargetView().GetAddressOf(), nulldepth);
		int texWidth = BloomMipDownRTV[0]->GetTextureWidth();
		int texHeight = BloomMipDownRTV[0]->GetTextureHeight();

		// 뷰포트를 만들어서 세팅해봅니다.
		D3D11_VIEWPORT downViewPort;

		downViewPort.TopLeftX = 0.0f;
		downViewPort.TopLeftY = 0.0f;
		downViewPort.Width = static_cast<float>(texWidth);
		downViewPort.Height = static_cast<float>(texHeight);
		downViewPort.MinDepth = 0.0f;
		downViewPort.MaxDepth = 1.0f;

		D3DDeviceContext->RSSetViewports(1, &downViewPort);

		// 커스텀 셰이더를 넣지 않고 그대로 그려줍니다.
			// 블룸 처리 셰이더를 넣어봅시다.
		spriteBatch->Begin(SpriteSortMode_Immediate, postBlendState.Get(), SamplerManager::GetInstance()->GetLinearClamp().Get(), UIUtils->GetDepthState().Get(), nullptr, [=]()
			{
				exceptDownPS->SetUpShader();
				threshold->SetUpBuffer(3, &bloomThresholdBuffer, ShaderType::PIXEL);
			});
		spriteBatch->Draw(sourceTarget->GetShaderResourceView().Get(), RECT{ 0, 0, texWidth, texHeight });
		spriteBatch->End();

		int lastDownIdx = 0;
		for (int i = 1; i < blurDownSize; i++)
		{
			// HBlur Render
				// 유니티상 HBlur의 타겟(Dest)은 MipUp이다.
			graphicsCore->ResetRenderTargetView(BlurMipUpRTV[i]->GetRenderTargetView(), Colors::Black);
			D3DDeviceContext->OMSetRenderTargets(1, BlurMipUpRTV[i]->GetRenderTargetView().GetAddressOf(), nulldepth);

			texWidth = BlurMipUpRTV[i]->GetTextureWidth();
			texHeight = BlurMipUpRTV[i]->GetTextureHeight();

			downViewPort.TopLeftX = 0.0f;
			downViewPort.TopLeftY = 0.0f;
			downViewPort.Width = static_cast<float>(texWidth);
			downViewPort.Height = static_cast<float>(texHeight);
			downViewPort.MinDepth = 0.0f;
			downViewPort.MaxDepth = 1.0f;

			D3DDeviceContext->RSSetViewports(1, &downViewPort);

			// SourceTex(dest가 아님)의 사이즈가 들어가야한다..
			texInfoBuffer.x = BlurMipDownRTV[lastDownIdx]->GetTextureWidth();
			texInfoBuffer.y = BlurMipDownRTV[lastDownIdx]->GetTextureHeight();
			texInfoBuffer.z = 1.0f / static_cast<float>(texWidth);
			texInfoBuffer.w = 1.0f / static_cast<float>(texHeight);

			// 텍스쳐 그리기, 사용자 정의 PS사용
			spriteBatch->Begin(DirectX::SpriteSortMode_Immediate, postBlendState.Get(), SamplerManager::GetInstance()->GetLinearClamp().Get(), UIUtils->GetDepthState().Get(), nullptr,
				[=]()
				{
					blurHPS->SetUpShader();
					texInfo->SetUpBuffer(4, &texInfoBuffer, ShaderType::PIXEL);
				});
			spriteBatch->Draw(BlurMipDownRTV[lastDownIdx]->GetShaderResourceView().Get(), RECT{ 0, 0, texWidth, texHeight });
			spriteBatch->End();

			// VBlur
				// Source MipUp, Dest MipDown
			graphicsCore->ResetRenderTargetView(BlurMipDownRTV[i]->GetRenderTargetView(), Colors::Black);
			D3DDeviceContext->OMSetRenderTargets(1, BlurMipDownRTV[i]->GetRenderTargetView().GetAddressOf(), nulldepth);

			texWidth = BlurMipDownRTV[i]->GetTextureWidth();
			texHeight = BlurMipDownRTV[i]->GetTextureHeight();

			// ViewPort의 크기는 동일하다
				// i번째 Mip Up, Mip Down의 텍스트 길이와 높이는 같다.

			// SourceTex(dest가 아님)의 사이즈가 들어가야한다..
			texInfoBuffer.x = BlurMipUpRTV[i]->GetTextureWidth();
			texInfoBuffer.y = BlurMipUpRTV[i]->GetTextureHeight();
			texInfoBuffer.z = 1.0f / static_cast<float>(texWidth);
			texInfoBuffer.w = 1.0f / static_cast<float>(texHeight);

			// 텍스쳐 그리기, 사용자 정의 PS사용
			spriteBatch->Begin(DirectX::SpriteSortMode_Immediate, postBlendState.Get(), SamplerManager::GetInstance()->GetLinearClamp().Get(), UIUtils->GetDepthState().Get(), nullptr,
				[=]()
				{
					blurVPS->SetUpShader();
					texInfo->SetUpBuffer(4, &texInfoBuffer, ShaderType::PIXEL);
				});
			spriteBatch->Draw(BlurMipUpRTV[i]->GetShaderResourceView().Get(), RECT{ 0, 0, texWidth, texHeight });
			spriteBatch->End();

			lastDownIdx = i;
		}
		ID3D11ShaderResourceView* null[] = { nullptr, nullptr, nullptr };

		// UpSampling
			// 순차적으로 lerp하면서 올라옵니다.
		for (int i = blurDownSize - 2; i >= 0; i--)
		{
			// dest => MipUp / source -> HighMip = MipDown
			// 중간에 들어가는 Texture(lowMip) => 기본적으로 mipUp(i + 1)
			auto lowMip = (i == blurDownSize - 2) ? BlurMipDownRTV[i + 1] : BlurMipUpRTV[i + 1];

			// dest 세팅
				// 렌더타겟에 블렌드가 되지 않기 때문에.. Reset하지 말고 Set만 해줘봅시다.
			D3DDeviceContext->OMSetRenderTargets(1, BlurMipUpRTV[i]->GetRenderTargetView().GetAddressOf(), nulldepth);
			texWidth = BlurMipUpRTV[i]->GetTextureWidth();
			texHeight = BlurMipUpRTV[i]->GetTextureHeight();

			downViewPort.TopLeftX = 0.0f;
			downViewPort.TopLeftY = 0.0f;
			downViewPort.Width = static_cast<float>(texWidth);
			downViewPort.Height = static_cast<float>(texHeight);
			downViewPort.MinDepth = 0.0f;
			downViewPort.MaxDepth = 1.0f;

			D3DDeviceContext->RSSetViewports(1, &downViewPort);

			// 텍스쳐 그리기, 사용자 정의 PS사용
				// 블러를 변경시키려면.. 여기서 스캐터 값을 변경해보는 것도..
			spriteBatch->Begin(DirectX::SpriteSortMode_Immediate, postBlendState.Get(), SamplerManager::GetInstance()->GetLinearClamp().Get(), UIUtils->GetDepthState().Get(), nullptr,
				[=]()
				{
					upSamplePS->SetUpShader();
					D3DDeviceContext->PSSetShaderResources(2, 1, lowMip->GetShaderResourceView().GetAddressOf());
					threshold->SetUpBuffer(3, &fadeOffset, ShaderType::PIXEL);
				});
			spriteBatch->Draw(BlurMipDownRTV[i]->GetShaderResourceView().Get(), RECT{ 0, 0, texWidth, texHeight });
			spriteBatch->End();

			D3DDeviceContext->PSSetShaderResources(0, 3, null);
		}
	}

	void PostProcess::Bloom(const std::shared_ptr<RenderTargetTexture>& sourceTarget)
	{
		auto UIUtils = UIUtilsManager::GetInstance();
		auto spriteBatch = UIUtils->GetSpriteBatch();

		auto threshold = BufferManager::GetInstance()->GetBuffer("ThresholdCB");
		auto texInfo = BufferManager::GetInstance()->GetBuffer("TextureInfoCB");
		auto fadeoff = BufferManager::GetInstance()->GetBuffer("ViewPosCB");

		// 첫번째 다운샘플링을 해줍니다. MipDown Zero에 만들어줍니다.
		graphicsCore->ResetRenderTargetView(BloomMipDownRTV[0]->GetRenderTargetView(), Colors::Black);
		ID3D11DepthStencilView* nulldepth = { 0 };

		// 렌더 타겟 세팅
		D3DDeviceContext->OMSetRenderTargets(1, BloomMipDownRTV[0]->GetRenderTargetView().GetAddressOf(), nulldepth);
		int texWidth = BloomMipDownRTV[0]->GetTextureWidth();
		int texHeight = BloomMipDownRTV[0]->GetTextureHeight();

		// 뷰포트를 만들어서 세팅해봅니다.
		D3D11_VIEWPORT downViewPort;

		downViewPort.TopLeftX = 0.0f;
		downViewPort.TopLeftY = 0.0f;
		downViewPort.Width = static_cast<float>(texWidth);
		downViewPort.Height = static_cast<float>(texHeight);
		downViewPort.MinDepth = 0.0f;
		downViewPort.MaxDepth = 1.0f;

		D3DDeviceContext->RSSetViewports(1, &downViewPort);

		// 텍스쳐 그리기(DownSample)
				// 블룸 커브를 먹여줍니다.
		spriteBatch->Begin(SpriteSortMode_Immediate, postBlendState.Get(), SamplerManager::GetInstance()->GetLinearClamp().Get(), UIUtils->GetDepthState().Get(), nullptr,
			[=]()
			{
				downPS->SetUpShader();
				threshold->SetUpBuffer(3, &bloomThresholdBuffer, ShaderType::PIXEL);
			});
		spriteBatch->Draw(sourceTarget->GetShaderResourceView().Get(), RECT{ 0, 0, texWidth, texHeight });
		spriteBatch->End();

		// 위의 것을 바탕으로 다운 샘플을하면서 그려줍니다.
		int lastDownIdx = 0;
		for (int i = 1; i < downSampleSize; i++)
		{
			// HBlur Render
				// 유니티상 HBlur의 타겟(Dest)은 MipUp이다.
			graphicsCore->ResetRenderTargetView(BloomMipUpRTV[i]->GetRenderTargetView(), Colors::Black);
			D3DDeviceContext->OMSetRenderTargets(1, BloomMipUpRTV[i]->GetRenderTargetView().GetAddressOf(), nulldepth);

			texWidth = BloomMipUpRTV[i]->GetTextureWidth();
			texHeight = BloomMipUpRTV[i]->GetTextureHeight();

			downViewPort.TopLeftX = 0.0f;
			downViewPort.TopLeftY = 0.0f;
			downViewPort.Width = static_cast<float>(texWidth);
			downViewPort.Height = static_cast<float>(texHeight);
			downViewPort.MinDepth = 0.0f;
			downViewPort.MaxDepth = 1.0f;

			D3DDeviceContext->RSSetViewports(1, &downViewPort);

			// SourceTex(dest가 아님)의 사이즈가 들어가야한다..
			texInfoBuffer.x = BloomMipDownRTV[lastDownIdx]->GetTextureWidth();
			texInfoBuffer.y = BloomMipDownRTV[lastDownIdx]->GetTextureHeight();
			texInfoBuffer.z = 1.0f / static_cast<float>(texWidth);
			texInfoBuffer.w = 1.0f / static_cast<float>(texHeight);

			// 텍스쳐 그리기, 사용자 정의 PS사용
			spriteBatch->Begin(DirectX::SpriteSortMode_Immediate, postBlendState.Get(), SamplerManager::GetInstance()->GetLinearClamp().Get(), UIUtils->GetDepthState().Get(), nullptr,
				[=]()
				{
					blurHPS->SetUpShader();
					texInfo->SetUpBuffer(4, &texInfoBuffer, ShaderType::PIXEL);
				});
			spriteBatch->Draw(BloomMipDownRTV[lastDownIdx]->GetShaderResourceView().Get(), RECT{ 0, 0, texWidth, texHeight });
			spriteBatch->End();

			// VBlur
				// Source MipUp, Dest MipDown
			graphicsCore->ResetRenderTargetView(BloomMipDownRTV[i]->GetRenderTargetView(), Colors::Black);
			D3DDeviceContext->OMSetRenderTargets(1, BloomMipDownRTV[i]->GetRenderTargetView().GetAddressOf(), nulldepth);

			texWidth = BloomMipDownRTV[i]->GetTextureWidth();
			texHeight = BloomMipDownRTV[i]->GetTextureHeight();

			// ViewPort의 크기는 동일하다
				// i번째 Mip Up, Mip Down의 텍스트 길이와 높이는 같다.

			// SourceTex(dest가 아님)의 사이즈가 들어가야한다..
			texInfoBuffer.x = BloomMipUpRTV[i]->GetTextureWidth();
			texInfoBuffer.y = BloomMipUpRTV[i]->GetTextureHeight();
			texInfoBuffer.z = 1.0f / static_cast<float>(texWidth);
			texInfoBuffer.w = 1.0f / static_cast<float>(texHeight);

			// 텍스쳐 그리기, 사용자 정의 PS사용
			spriteBatch->Begin(DirectX::SpriteSortMode_Immediate, postBlendState.Get(), SamplerManager::GetInstance()->GetLinearClamp().Get(), UIUtils->GetDepthState().Get(), nullptr,
				[=]()
				{
					blurVPS->SetUpShader();
					texInfo->SetUpBuffer(4, &texInfoBuffer, ShaderType::PIXEL);
				});
			spriteBatch->Draw(BloomMipUpRTV[i]->GetShaderResourceView().Get(), RECT{ 0, 0, texWidth, texHeight });
			spriteBatch->End();

			lastDownIdx = i;
		}
		ID3D11ShaderResourceView* null[] = { nullptr, nullptr, nullptr };

		// UpSampling
			// 순차적으로 lerp하면서 올라옵니다.
		for (int i = downSampleSize - 2; i >= 0; i--)
		{
			// dest => MipUp / source -> HighMip = MipDown
			// 중간에 들어가는 Texture(lowMip) => 기본적으로 mipUp(i + 1)
			auto lowMip = (i == downSampleSize - 2) ? BloomMipDownRTV[i + 1] : BloomMipUpRTV[i + 1];

			// dest 세팅
				// 렌더타겟에 블렌드가 되지 않기 때문에.. Reset하지 말고 Set만 해줘봅시다.
			D3DDeviceContext->OMSetRenderTargets(1, BloomMipUpRTV[i]->GetRenderTargetView().GetAddressOf(), nulldepth);
			texWidth = BloomMipUpRTV[i]->GetTextureWidth();
			texHeight = BloomMipUpRTV[i]->GetTextureHeight();

			downViewPort.TopLeftX = 0.0f;
			downViewPort.TopLeftY = 0.0f;
			downViewPort.Width = static_cast<float>(texWidth);
			downViewPort.Height = static_cast<float>(texHeight);
			downViewPort.MinDepth = 0.0f;
			downViewPort.MaxDepth = 1.0f;

			D3DDeviceContext->RSSetViewports(1, &downViewPort);

			// 텍스쳐 그리기, 사용자 정의 PS사용
			spriteBatch->Begin(DirectX::SpriteSortMode_Immediate, postBlendState.Get(), SamplerManager::GetInstance()->GetLinearClamp().Get(), UIUtils->GetDepthState().Get(), nullptr,
				[=]()
				{
					upSamplePS->SetUpShader();
					D3DDeviceContext->PSSetShaderResources(2, 1, lowMip->GetShaderResourceView().GetAddressOf());
					threshold->SetUpBuffer(3, &bloomThresholdBuffer, ShaderType::PIXEL);
				});
			spriteBatch->Draw(BloomMipDownRTV[i]->GetShaderResourceView().Get(), RECT{ 0, 0, texWidth, texHeight });
			spriteBatch->End();

			D3DDeviceContext->PSSetShaderResources(0, 3, null);
		}

	}

	// 모든 렌더가 완료된(Deferred로 lighting연산까지 완료된) RenderTarget이 들어와야합니다.
	void PostProcess::Render(const std::shared_ptr<RenderTargetTexture>& renderTarget)
	{
		//DownSample(renderTarget);
		if (bloom == true || allPostProcessing == false)
		{
			Blur(renderTarget);
			Bloom(renderTarget);
		}

		RenderStart();

		quadVS->SetUpShader();
		if(allPostProcessing == true && bloom == true)
			postProcessPS->SetUpShader();
		else if(allPostProcessing == true && bloom != true)
			postProcessNotBloomPS->SetUpShader();
		else
			notPostProcessPS->SetUpShader();

		// 현재 RenderTarget의 Texture를 얻어옵니다.
		D3DDeviceContext->PSSetShaderResources(0, 1, renderTarget->GetShaderResourceView().GetAddressOf());
		D3DDeviceContext->PSSetSamplers(0, 1, SamplerManager::GetInstance()->GetLinearClamp().GetAddressOf());

		D3DDeviceContext->PSSetShaderResources(2, 1, BloomMipUpRTV[0]->GetShaderResourceView().GetAddressOf());
		D3DDeviceContext->PSSetShaderResources(3, 1, BlurMipUpRTV[0]->GetShaderResourceView().GetAddressOf());

		// ScreenMesh를 불러와서 화면에 그려준다.
		auto screenMesh = ResourceManager::GetInstance()->basicMeshes["ScreenMesh"];
		D3DDeviceContext->RSSetState(screenMesh->GetRasterState().Get());

		UINT stride = sizeof(VertexStruct::TexVertex);
		UINT offset = 0;

		D3DDeviceContext->IASetPrimitiveTopology(screenMesh->GetPrimitiveTopology());
		D3DDeviceContext->IASetVertexBuffers(0, 1, screenMesh->vertexBuffer.GetAddressOf(), &stride, &offset);
		D3DDeviceContext->IASetIndexBuffer(screenMesh->indexBuffers[0].Get(), DXGI_FORMAT_R32_UINT, 0);

		// Buffer Setup
		BufferManager::GetInstance()->GetBuffer("ColorCB")->SetUpBuffer(0, &vignetteColorBuffer, ShaderType::PIXEL);
		BufferManager::GetInstance()->GetBuffer("ThresholdCB")->SetUpBuffer(1, &vignetteBuffer, ShaderType::PIXEL);
		BufferManager::GetInstance()->GetBuffer("TexOffsetCB")->SetUpBuffer(3, &bloomThresholdBuffer, ShaderType::PIXEL);
		// 현재 렌더 타겟의 텍스쳐 크기를..
		texInfoBuffer.x = renderTarget->GetTextureWidth();
		texInfoBuffer.y = renderTarget->GetTextureHeight();
		texInfoBuffer.z = 1 / renderTarget->GetTextureWidth();
		texInfoBuffer.w = 1 / renderTarget->GetTextureHeight();

		BufferManager::GetInstance()->GetBuffer("TextureInfoCB")->SetUpBuffer(4, &texInfoBuffer, ShaderType::PIXEL);
		// 페이드 아웃 세팅합니다.
		BufferManager::GetInstance()->GetBuffer("ViewPosCB")->SetUpBuffer(5, &fadeOffset, ShaderType::PIXEL);


		fadeOffset.x += 0.016f * fadeSpeed;
		if (fadeOffset.x < 0.f)
		{
			// 페이드 인 아웃이 끝나면 속도를 0으로 해줍니다.
			fadeSpeed = 0.f;
			fadeOffset.x = 0.f;
		}
		else if (fadeOffset.x > 1.0f)
		{
			fadeSpeed = 0.f;
			fadeOffset.x = 1.0f;
		}

		D3DDeviceContext->DrawIndexed(screenMesh->indexBuffersSize[0], 0, 0);

		EndRender();
	}

	void PostProcess::EndRender()
	{
		ID3D11ShaderResourceView* nullSRV[4] = { nullptr };

		D3DDeviceContext->PSSetShaderResources(0, 4, nullSRV);

		// 원상복귀 시켜줍니다
		//D3DDeviceContext->OMSetRenderTargets(1, mainRTV->GetRenderTargetView().GetAddressOf(), mainDSV->GetDepthStencilView().Get());
	}

	void PostProcess::ImGuiRender()
	{

		auto imgui = ImGUIManager::GetInstance();
		auto shaderManager = ShaderManager::GetInstance();

		/*if (imgui->Begin("Check Vignette"))
		{

			float intensity = vignetteBuffer.x;
			float smoothness = vignetteBuffer.y;
			//float vignetteColor[4] = { vignetteColorBuffer.x, vignetteColorBuffer.y, vignetteColorBuffer.z, vignetteColorBuffer.w };
			float offset = fadeOffset.x;
			float blur = fadeOffset.y;
			float blurIntensity = vignetteBuffer.z;
			float blurSmoothness = vignetteBuffer.w;

			imgui->DragFloat("Intensity", &intensity, 0.005f, 0.f, 1.0f);
			imgui->DragFloat("Smoothness", &smoothness, 0.005f, 0.01f, 1.0f);
			imgui->DragFloat("Offset", &offset, 0.005f, 0.00f, 1.0f);
			imgui->DragFloat("Blur", &blur, 0.005f, 0.0f, 1.0f);
			imgui->DragFloat("Blur Intensity", &blurIntensity, 0.005f, 0.0f, 1.0f);
			imgui->DragFloat("Blur Smoothness", &blurSmoothness, 0.005f, 0.01f, 1.0f);

			//imgui->ColorPicker4("Vignette Color", vignetteColor);

			vignetteBuffer.x = intensity;
			vignetteBuffer.y = smoothness;
			vignetteBuffer.z = blurIntensity;
			vignetteBuffer.w = blurSmoothness;

			/*vignetteColorBuffer.x = vignetteColor[0];
			vignetteColorBuffer.y = vignetteColor[1];
			vignetteColorBuffer.z = vignetteColor[2];
			vignetteColorBuffer.w = vignetteColor[3];#1#

			fadeOffset.x = offset;
			fadeOffset.y = blur;
		}
		imgui->End();*/

		if (imgui->Begin("Check PostProcess"))
		{
			bool bloomCheck = bloom;
			bool postprocessCheck = allPostProcessing;

			imgui->CheckBox("Bloom", &bloomCheck);
			imgui->CheckBox("All Post Processing", &postprocessCheck);

			bloom = bloomCheck;
			allPostProcessing = postprocessCheck;
		}
		imgui->End();
	}

	void PostProcess::StartFadeIn(float speed)
	{
		fadeSpeed = abs(speed);
	}

	void PostProcess::StartFadeOut(float speed)
	{
		fadeSpeed = -abs(speed);
	}
}

