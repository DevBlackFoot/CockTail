#include "GraphicsPch.h"
#include "OITPass.h"

#include "GraphicsCore/DX11GraphicsCore.h"
#include "GraphicsCore/RenderTargetTexture.h"
#include "GraphicsCore/DepthStencilResource.h"
#include "GraphicsCore/UnorderedAccessResource.h"

#include "Manager/ShaderManager.h"
#include "Manager/ResourceManager.h"
#include "Manager/SamplerManager.h"
#include "Manager/BufferManager.h"

#include "Object/Shader/VertexShader.h"
#include "Object/Shader/PixelShader.h"
#include "Object/Buffer/BufferBase.h"

#define LAYER_COUNT 16

namespace GraphicsEngineSpace
{
	void OITPass::Start(const std::shared_ptr<RenderTargetTexture>& mainRTV,
		const std::shared_ptr<DepthStencilResource>& mainDSV)
	{
		// 각종 요소들을 만들어줘야 합니다.

		/// 사용할 RTV, DSV
		// 해당 부분이 최종 렌더 결과가 아니게 되므로.. 이 부분을 새로 만들어줍니다.
		OITRTV = std::make_shared<RenderTargetTexture>();
		// mainDSV는 가져와 줍니다.
		OITDSV = mainDSV;

		/// 셰이더
		quadVS = std::dynamic_pointer_cast<VertexShader>(ShaderManager::GetInstance()->GetShader("QuadVS"));
		OITBlendPS = std::dynamic_pointer_cast<PixelShader>(ShaderManager::GetInstance()->GetShader("OITBlendPS"));

		graphicsCore = DX11GraphicsCore::GetInstance();

		/// UAV를 가진 클래스를 생성해줍니다 => OnResize에서 사이즈에 맞게 생성해줍니다.
		pixelLinkUAResource = std::make_shared<UnorderedAccessResource>();
		firstOffsetUAResource = std::make_shared<UnorderedAccessResource>();
	}

	void OITPass::Release()
	{
		// 각종 리소스 해제
		OITRTV->Finalize();

		pixelLinkUAResource->Finalize();
		firstOffsetUAResource->Finalize();

		for (int i = 0; i < 2; i++)
			uavList[i] = nullptr;

		SafeReset(OITBlendPS);

		SafeReset(OITRTV);
		SafeReset(pixelLinkUAResource);
		SafeReset(firstOffsetUAResource);

	}

	void OITPass::OnResize(int width, int height)
	{
		// 기존의 것들을 날려주고..
		OITRTV->Finalize();

		pixelLinkUAResource->Finalize();
		firstOffsetUAResource->Finalize();

		// RTV를 생성해줍니다
			// BackGround로 라이팅이 끝난 texture를 받을 예정이기 때문에 새로운 RTV가 필요합니다.
		OITRTV->Init(D3DDevice, width, height, graphicsCore->GetMSAAQuality());

		// pixel Link에 들어갈 데이터를 만들어줍니다.
		UINT elementsCount = graphicsCore->GetScreenWidth() * graphicsCore->GetScreenHeight() * LAYER_COUNT;
		std::vector<Structure::PixelNode> initVertex(elementsCount);

		// Pixel Link를 만들어줍시다..
		pixelLinkUAResource->CreateStructuredBuffer(D3DDevice, sizeof(Structure::PixelNode), elementsCount, initVertex.data(), D3D11_BUFFER_UAV_FLAG_COUNTER);

		// first offset을 만들어줍니다.
		firstOffsetUAResource->CreateFirstOffsetBuffer(D3DDevice, graphicsCore->GetScreenWidth(), graphicsCore->GetScreenHeight(), LAYER_COUNT);
	}

	void OITPass::BeginRender()
	{
		// UAV object Clear
		Clear();

		// 이후 게임 엔진 부분에서 모든 파티클 오브젝트들을 그려줍니다.
			// TODO Particle Effect의 Draw를 호출 해야함.

	}

	// 단 한번만 불려지는 함수이기 때문에 여기서 Unbinding 및 실제로 그려주는 RTT를 세팅합시다.
	void OITPass::Render(const std::shared_ptr<RenderTargetTexture>& backGroundRTT)
	{
		// 그리고 Unbinding을 해줍니다.
		UnBindingUAV();

		// 위에서 만들어진 RTV를 사용해줍시다.
		Draw(backGroundRTT);
	}

	void OITPass::EndRender()
	{
		// SRV를 돌려줍니다..
		ID3D11ShaderResourceView* nullSRV[3] = { nullptr, nullptr, nullptr };

		D3DDeviceContext->PSSetShaderResources(0, 3, nullSRV);
	}


	// Begin Renderer 와 유사하게 한번씩만 호출해줘야하는 함수 입니다.
	void OITPass::BindingUAV()
	{
		// depth는 main DSV를 사용합니다.

		// 렌더 타겟은 없는 상태로 해줍니다.
		D3DDeviceContext->OMSetRenderTargets(0, nullptr, OITDSV->GetDepthStencilView().Get());

		// 숨겨진 카운터를 0으로 만들기 때문에 한 번만 불러줘야합니다.
		uavList[0] = pixelLinkUAResource->GetUAV();
		uavList[1] = firstOffsetUAResource->GetUAV();

		initCount[0] = -1;
		initCount[1] = -1;

		D3DDeviceContext->OMSetRenderTargetsAndUnorderedAccessViews(0,
			nullptr, OITDSV->GetDepthStencilView().Get(),
			0, 2, uavList->GetAddressOf(), initCount);
	}

	void OITPass::BindingSRV()
	{
		// rtv는 위에서 만들어준 RTV로 해줍니다.
			// light Pass 가 끝나고 합쳐진 버퍼를 그대로 도화지로 사용해서 파티클을 그려준다고 생각합니다.
			// (해당 부분은 SRV로 셰이더에 들어갑니다.)
		// rtv로 들어가 있는 버퍼는 srv로 뽑을 수 없습니다..
		D3DDeviceContext->OMSetRenderTargetsAndUnorderedAccessViews(1, OITRTV->GetRenderTargetView().GetAddressOf(),
		nullptr, 0, 0, nullptr, nullptr);
		
		// 여기서는 일단 UAV만 세팅해줍니다.
		D3DDeviceContext->PSSetShaderResources(1, 1, pixelLinkUAResource->GetSRV().GetAddressOf());
		D3DDeviceContext->PSSetShaderResources(2, 1, firstOffsetUAResource->GetSRV().GetAddressOf());
	}

	void OITPass::UnBindingUAV()
	{
		// Unbinding 하면 모든 것이 초기화 됩니다..
		D3DDeviceContext->ClearState();
		D3DDeviceContext->RSSetViewports(1, OITDSV->GetViewport().get());
	}

	// 해당 함수도 렌더시 한 번만 불려져야 합니다.
	void OITPass::Clear()
	{
		unsigned int clearNum = 0xffffffff;

		D3DDeviceContext->ClearUnorderedAccessViewUint(pixelLinkUAResource->GetUAV().Get(), &clearNum);
		D3DDeviceContext->ClearUnorderedAccessViewUint(firstOffsetUAResource->GetUAV().Get(), &clearNum);

		// depth는 main DSV를 사용합니다.

		// 렌더 타겟은 없는 상태로 해줍니다.
			// 뎁스버퍼를 사용할 것이기 때문에 여기서 빼줍니다..
		D3DDeviceContext->OMSetRenderTargets(0, OITRTV->GetRenderTargetView().GetAddressOf(), nullptr);

		// 숨겨진 카운터를 0으로 만들기 때문에 한 번만 불러줘야합니다.
		uavList[0] = pixelLinkUAResource->GetUAV();
		uavList[1] = firstOffsetUAResource->GetUAV();

		initCount[0] = 0;
		initCount[1] = 0;

		D3DDeviceContext->OMSetRenderTargetsAndUnorderedAccessViews(D3D11_KEEP_RENDER_TARGETS_AND_DEPTH_STENCIL,
		nullptr, nullptr,
		0, 2, uavList->GetAddressOf(), initCount);
	}

	// 완성된 레이어를 합쳐서 그려주는 함수.
	void OITPass::Draw(const std::shared_ptr<RenderTargetTexture>& backGroundRTT)
	{
		BindingSRV();

		D3DDeviceContext->OMSetBlendState(nullptr, nullptr, 0xfffffff);

		// 실제로 그려주는 함수이기 때문에 각종 셰이더들을 세팅해줍니다.
		quadVS->SetUpShader();
		OITBlendPS->SetUpShader();

		// 각 상수 버퍼를 세팅해줍니다.
			// 여기서 BackGround Texture => Light Pass에서 얻어온 RTT 가 들어갑니다.
		D3DDeviceContext->PSSetShaderResources(0, 1, backGroundRTT->GetShaderResourceView().GetAddressOf());

		// 나머지 세팅은 다른 Pass에서 한 것 처럼 해주면 될 것 같습니다.
		// 샘플러 세팅
		D3DDeviceContext->PSSetSamplers(0, 1, SamplerManager::GetInstance()->GetAnisotropicWrap().GetAddressOf());

		// Screen Info 세팅
		// 스크린 Info를 세팅해줍니다.
		SimpleMath::Vector4 screenInfo;
		screenInfo.x = DX11GraphicsCore::GetInstance()->GetScreenWidth();
		screenInfo.y = DX11GraphicsCore::GetInstance()->GetScreenHeight();
		BufferManager::GetInstance()->GetBuffer("TextureInfoCB")->SetUpBuffer(0, &screenInfo, ShaderType::PIXEL);

		// ScreenMesh를 불러와서 화면에 그려준다.
		auto screenMesh = ResourceManager::GetInstance()->basicMeshes["ScreenMesh"];
		D3DDeviceContext->RSSetState(screenMesh->GetRasterState().Get());

		UINT stride = sizeof(VertexStruct::TexVertex);
		UINT offset = 0;

		D3DDeviceContext->IASetPrimitiveTopology(screenMesh->GetPrimitiveTopology());
		D3DDeviceContext->IASetVertexBuffers(0, 1, screenMesh->vertexBuffer.GetAddressOf(), &stride, &offset);
		D3DDeviceContext->IASetIndexBuffer(screenMesh->indexBuffers[0].Get(), DXGI_FORMAT_R32_UINT, 0);

		D3DDeviceContext->DrawIndexed(screenMesh->indexBuffersSize[0], 0, 0);

		// 뎁스 스텐실을 꺼줘야합니다.
			// TODO. Renderer에서 꺼줍시다..
		// 마지막으로 End Render => 원상복귀..

		EndRender();
	}
}
