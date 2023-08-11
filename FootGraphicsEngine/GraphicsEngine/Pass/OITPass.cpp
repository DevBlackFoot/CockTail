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
		// ���� ��ҵ��� �������� �մϴ�.

		/// ����� RTV, DSV
		// �ش� �κ��� ���� ���� ����� �ƴϰ� �ǹǷ�.. �� �κ��� ���� ������ݴϴ�.
		OITRTV = std::make_shared<RenderTargetTexture>();
		// mainDSV�� ������ �ݴϴ�.
		OITDSV = mainDSV;

		/// ���̴�
		quadVS = std::dynamic_pointer_cast<VertexShader>(ShaderManager::GetInstance()->GetShader("QuadVS"));
		OITBlendPS = std::dynamic_pointer_cast<PixelShader>(ShaderManager::GetInstance()->GetShader("OITBlendPS"));

		graphicsCore = DX11GraphicsCore::GetInstance();

		/// UAV�� ���� Ŭ������ �������ݴϴ� => OnResize���� ����� �°� �������ݴϴ�.
		pixelLinkUAResource = std::make_shared<UnorderedAccessResource>();
		firstOffsetUAResource = std::make_shared<UnorderedAccessResource>();
	}

	void OITPass::Release()
	{
		// ���� ���ҽ� ����
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
		// ������ �͵��� �����ְ�..
		OITRTV->Finalize();

		pixelLinkUAResource->Finalize();
		firstOffsetUAResource->Finalize();

		// RTV�� �������ݴϴ�
			// BackGround�� �������� ���� texture�� ���� �����̱� ������ ���ο� RTV�� �ʿ��մϴ�.
		OITRTV->Init(D3DDevice, width, height, graphicsCore->GetMSAAQuality());

		// pixel Link�� �� �����͸� ������ݴϴ�.
		UINT elementsCount = graphicsCore->GetScreenWidth() * graphicsCore->GetScreenHeight() * LAYER_COUNT;
		std::vector<Structure::PixelNode> initVertex(elementsCount);

		// Pixel Link�� ������ݽô�..
		pixelLinkUAResource->CreateStructuredBuffer(D3DDevice, sizeof(Structure::PixelNode), elementsCount, initVertex.data(), D3D11_BUFFER_UAV_FLAG_COUNTER);

		// first offset�� ������ݴϴ�.
		firstOffsetUAResource->CreateFirstOffsetBuffer(D3DDevice, graphicsCore->GetScreenWidth(), graphicsCore->GetScreenHeight(), LAYER_COUNT);
	}

	void OITPass::BeginRender()
	{
		// UAV object Clear
		Clear();

		// ���� ���� ���� �κп��� ��� ��ƼŬ ������Ʈ���� �׷��ݴϴ�.
			// TODO Particle Effect�� Draw�� ȣ�� �ؾ���.

	}

	// �� �ѹ��� �ҷ����� �Լ��̱� ������ ���⼭ Unbinding �� ������ �׷��ִ� RTT�� �����սô�.
	void OITPass::Render(const std::shared_ptr<RenderTargetTexture>& backGroundRTT)
	{
		// �׸��� Unbinding�� ���ݴϴ�.
		UnBindingUAV();

		// ������ ������� RTV�� ������ݽô�.
		Draw(backGroundRTT);
	}

	void OITPass::EndRender()
	{
		// SRV�� �����ݴϴ�..
		ID3D11ShaderResourceView* nullSRV[3] = { nullptr, nullptr, nullptr };

		D3DDeviceContext->PSSetShaderResources(0, 3, nullSRV);
	}


	// Begin Renderer �� �����ϰ� �ѹ����� ȣ��������ϴ� �Լ� �Դϴ�.
	void OITPass::BindingUAV()
	{
		// depth�� main DSV�� ����մϴ�.

		// ���� Ÿ���� ���� ���·� ���ݴϴ�.
		D3DDeviceContext->OMSetRenderTargets(0, nullptr, OITDSV->GetDepthStencilView().Get());

		// ������ ī���͸� 0���� ����� ������ �� ���� �ҷ�����մϴ�.
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
		// rtv�� ������ ������� RTV�� ���ݴϴ�.
			// light Pass �� ������ ������ ���۸� �״�� ��ȭ���� ����ؼ� ��ƼŬ�� �׷��شٰ� �����մϴ�.
			// (�ش� �κ��� SRV�� ���̴��� ���ϴ�.)
		// rtv�� �� �ִ� ���۴� srv�� ���� �� �����ϴ�..
		D3DDeviceContext->OMSetRenderTargetsAndUnorderedAccessViews(1, OITRTV->GetRenderTargetView().GetAddressOf(),
		nullptr, 0, 0, nullptr, nullptr);
		
		// ���⼭�� �ϴ� UAV�� �������ݴϴ�.
		D3DDeviceContext->PSSetShaderResources(1, 1, pixelLinkUAResource->GetSRV().GetAddressOf());
		D3DDeviceContext->PSSetShaderResources(2, 1, firstOffsetUAResource->GetSRV().GetAddressOf());
	}

	void OITPass::UnBindingUAV()
	{
		// Unbinding �ϸ� ��� ���� �ʱ�ȭ �˴ϴ�..
		D3DDeviceContext->ClearState();
		D3DDeviceContext->RSSetViewports(1, OITDSV->GetViewport().get());
	}

	// �ش� �Լ��� ������ �� ���� �ҷ����� �մϴ�.
	void OITPass::Clear()
	{
		unsigned int clearNum = 0xffffffff;

		D3DDeviceContext->ClearUnorderedAccessViewUint(pixelLinkUAResource->GetUAV().Get(), &clearNum);
		D3DDeviceContext->ClearUnorderedAccessViewUint(firstOffsetUAResource->GetUAV().Get(), &clearNum);

		// depth�� main DSV�� ����մϴ�.

		// ���� Ÿ���� ���� ���·� ���ݴϴ�.
			// �������۸� ����� ���̱� ������ ���⼭ ���ݴϴ�..
		D3DDeviceContext->OMSetRenderTargets(0, OITRTV->GetRenderTargetView().GetAddressOf(), nullptr);

		// ������ ī���͸� 0���� ����� ������ �� ���� �ҷ�����մϴ�.
		uavList[0] = pixelLinkUAResource->GetUAV();
		uavList[1] = firstOffsetUAResource->GetUAV();

		initCount[0] = 0;
		initCount[1] = 0;

		D3DDeviceContext->OMSetRenderTargetsAndUnorderedAccessViews(D3D11_KEEP_RENDER_TARGETS_AND_DEPTH_STENCIL,
		nullptr, nullptr,
		0, 2, uavList->GetAddressOf(), initCount);
	}

	// �ϼ��� ���̾ ���ļ� �׷��ִ� �Լ�.
	void OITPass::Draw(const std::shared_ptr<RenderTargetTexture>& backGroundRTT)
	{
		BindingSRV();

		D3DDeviceContext->OMSetBlendState(nullptr, nullptr, 0xfffffff);

		// ������ �׷��ִ� �Լ��̱� ������ ���� ���̴����� �������ݴϴ�.
		quadVS->SetUpShader();
		OITBlendPS->SetUpShader();

		// �� ��� ���۸� �������ݴϴ�.
			// ���⼭ BackGround Texture => Light Pass���� ���� RTT �� ���ϴ�.
		D3DDeviceContext->PSSetShaderResources(0, 1, backGroundRTT->GetShaderResourceView().GetAddressOf());

		// ������ ������ �ٸ� Pass���� �� �� ó�� ���ָ� �� �� �����ϴ�.
		// ���÷� ����
		D3DDeviceContext->PSSetSamplers(0, 1, SamplerManager::GetInstance()->GetAnisotropicWrap().GetAddressOf());

		// Screen Info ����
		// ��ũ�� Info�� �������ݴϴ�.
		SimpleMath::Vector4 screenInfo;
		screenInfo.x = DX11GraphicsCore::GetInstance()->GetScreenWidth();
		screenInfo.y = DX11GraphicsCore::GetInstance()->GetScreenHeight();
		BufferManager::GetInstance()->GetBuffer("TextureInfoCB")->SetUpBuffer(0, &screenInfo, ShaderType::PIXEL);

		// ScreenMesh�� �ҷ��ͼ� ȭ�鿡 �׷��ش�.
		auto screenMesh = ResourceManager::GetInstance()->basicMeshes["ScreenMesh"];
		D3DDeviceContext->RSSetState(screenMesh->GetRasterState().Get());

		UINT stride = sizeof(VertexStruct::TexVertex);
		UINT offset = 0;

		D3DDeviceContext->IASetPrimitiveTopology(screenMesh->GetPrimitiveTopology());
		D3DDeviceContext->IASetVertexBuffers(0, 1, screenMesh->vertexBuffer.GetAddressOf(), &stride, &offset);
		D3DDeviceContext->IASetIndexBuffer(screenMesh->indexBuffers[0].Get(), DXGI_FORMAT_R32_UINT, 0);

		D3DDeviceContext->DrawIndexed(screenMesh->indexBuffersSize[0], 0, 0);

		// ���� ���ٽ��� ������մϴ�.
			// TODO. Renderer���� ���ݽô�..
		// ���������� End Render => ���󺹱�..

		EndRender();
	}
}
