#include "GraphicsPch.h"
#include "LightPass.h"

#include "GraphicsCore/DX11GraphicsCore.h"
#include "GraphicsCore/RenderTargetTexture.h"
#include "GraphicsCore/DepthStencilResource.h"
#include "Manager/BufferManager.h"
#include "Manager/ImGUIManager.h"

#include "Object/Shader/VertexShader.h"
#include "Object/Shader/PixelShader.h"

#include "Manager/ShaderManager.h"
#include "Manager/ResourceManager.h"
#include "Manager/SamplerManager.h"
#include "Object/Buffer/BufferBase.h"

#include "Util/VertexStruct.h"
#include "Resources/Texture.h"

namespace GraphicsEngineSpace
{
	void LightPass::Start(const std::shared_ptr<RenderTargetTexture>& mainRTV, const std::shared_ptr<DepthStencilResource>& mainDSV)
	{
		quadVS = std::dynamic_pointer_cast<VertexShader>(ShaderManager::GetInstance()->GetShader("QuadVS"));

		PBRLightPS = std::dynamic_pointer_cast<PixelShader>(ShaderManager::GetInstance()->GetShader("PBRLightPS"));
		PBRLightShadowPS = std::dynamic_pointer_cast<PixelShader>(ShaderManager::GetInstance()->GetShader("PBRLightFullShadowPS"));
		PBRLightIndoorPS = std::dynamic_pointer_cast<PixelShader>(ShaderManager::GetInstance()->GetShader("PBRLightFullShadowIndoorPS"));

		// 해당 부분이 최종 렌더 결과가 아니게 되므로.. 이 부분을 새로 만들어줍니다.
			// => light Pass의 결과가 mainRTV에 기록되는 것이 아니게 되므로..
		//lightingRTV = mainRTV;
		lightingRTV = std::make_shared<RenderTargetTexture>();
		lightingDSV = mainDSV;

		// IBL Texture를 로드해봅니다.
		IBLDiffuse = std::make_shared<Texture>();
		IBLSpecular = std::make_shared<Texture>();
		IBLBRDFLookUp = std::make_shared<Texture>();

		IBLDiffuse->Load(L"Resources/Texture/OutsideDiffuseMDR.dds");
		IBLSpecular->Load(L"Resources/Texture/OutsideSpecularMDR.dds");
		IBLBRDFLookUp->Load(L"Resources/Texture/OutsideBrdf.dds");

		graphicsCore = DX11GraphicsCore::GetInstance();

		cbLightPass = cbLight();
	}

	void LightPass::Release()
	{
		lightingRTV->Finalize();

		SafeReset(PBRLightPS);
		SafeReset(lightingRTV);
	}

	void LightPass::OnResize(int width, int height)
	{
		// 기존의 것을 지워주고 다시 생성한다.
			// DSV의 경우 Main을 사용하는 것이기에 새로 생성할 이유가 없다.
		lightingRTV->Finalize();
		lightingRTV->Init(D3DDevice, width, height, graphicsCore->GetMSAAQuality());
	}

	void LightPass::RenderStart()
	{
		graphicsCore->ResetRenderTargetView(lightingRTV->GetRenderTargetView(), Colors::Gray);
		//graphicsCore->ResetDepthStencilView(lightingDSV->GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		ID3D11DepthStencilView* depthStencil = { 0 };

		D3DDeviceContext->OMSetRenderTargets(1, lightingRTV->GetRenderTargetView().GetAddressOf(), depthStencil);
		//D3DDeviceContext->OMSetRenderTargets(1, lightingRTV->GetRenderTargetView().GetAddressOf(), lightingDSV->GetDepthStencilView().Get());

	}

	void LightPass::Render(const std::vector<std::shared_ptr<RenderTargetTexture>>& renderTargets)
	{
		RenderStart();

		quadVS->SetUpShader();
		PBRLightPS->SetUpShader();

		// cb 생성..
		cbLightPass.DirLights[0] = directionalLight;
		cbLightPass.EyePosW = eyePos;
		cbLightPass.DirLightCnt = DIRECTION_LIGHT_COUNT;
		cbLightPass.PointLightCnt = POINT_LIGHT_COUNT;
		cbLightPass.SpotLightCnt = SPOT_LIGHT_COUNT;

		for (int i = 0; i < pointLights.size(); i++)
		{
			cbLightPass.PointLights[i] = pointLights[i];
		}

		for (int i = 0; i < spotLights.size(); i++)
		{
			cbLightPass.SpotLights[i] = spotLights[i];
		}

		BufferManager::GetInstance()->GetBuffer("LightCB")->SetUpBuffer(0, &cbLightPass, ShaderType::PIXEL);

		// Albedo
		D3DDeviceContext->PSSetShaderResources(0, 1, renderTargets[0]->GetShaderResourceView().GetAddressOf());
		//DMRAO
		D3DDeviceContext->PSSetShaderResources(1, 1, renderTargets[1]->GetShaderResourceView().GetAddressOf());
		//Normal
		D3DDeviceContext->PSSetShaderResources(2, 1, renderTargets[2]->GetShaderResourceView().GetAddressOf());
		//Position
		D3DDeviceContext->PSSetShaderResources(3, 1, renderTargets[3]->GetShaderResourceView().GetAddressOf());
		//Emissive
		D3DDeviceContext->PSSetShaderResources(4, 1, renderTargets[4]->GetShaderResourceView().GetAddressOf());
		
		D3DDeviceContext->PSSetSamplers(0, 1, SamplerManager::GetInstance()->GetLinearClamp().GetAddressOf());
		D3DDeviceContext->PSSetSamplers(1, 1, SamplerManager::GetInstance()->GetComparisonLinear().GetAddressOf());
		D3DDeviceContext->PSSetSamplers(2, 1, SamplerManager::GetInstance()->GetLinearWrap().GetAddressOf());

		// ScreenMesh를 불러와서 화면에 그려준다.
		auto screenMesh = ResourceManager::GetInstance()->basicMeshes["ScreenMesh"];
		D3DDeviceContext->RSSetState(screenMesh->GetRasterState().Get());

		UINT stride = sizeof(VertexStruct::TexVertex);
		UINT offset = 0;

		D3DDeviceContext->IASetPrimitiveTopology(screenMesh->GetPrimitiveTopology());
		D3DDeviceContext->IASetVertexBuffers(0, 1, screenMesh->vertexBuffer.GetAddressOf(), &stride, &offset);
		D3DDeviceContext->IASetIndexBuffer(screenMesh->indexBuffers[0].Get(), DXGI_FORMAT_R32_UINT, 0);

		D3DDeviceContext->DrawIndexed(screenMesh->indexBuffersSize[0], 0, 0);

		EndRender();
	}

	void LightPass::Render(const std::vector<std::shared_ptr<RenderTargetTexture>>& renderTargets,
		std::shared_ptr<DepthStencilResource> pointShadowResource, std::shared_ptr<DepthStencilResource> spotShadowResource)
	{
		RenderStart();

		quadVS->SetUpShader();
		PBRLightShadowPS->SetUpShader();

		// cb 생성..
		cbLightPass.DirLights[0] = directionalLight;
		cbLightPass.EyePosW = eyePos;
		cbLightPass.DirLightCnt = DIRECTION_LIGHT_COUNT;
		cbLightPass.PointLightCnt = pointLights.size();
		cbLightPass.SpotLightCnt = spotLights.size();

		for(int i = 0; i < pointLights.size(); i++)
		{
			cbLightPass.PointLights[i] = pointLights[i];
		}

		for (int i = 0; i < spotLights.size(); i++)
		{
			cbLightPass.SpotLights[i] = spotLights[i];
		}

		BufferManager::GetInstance()->GetBuffer("LightCB")->SetUpBuffer(0, &cbLightPass, ShaderType::PIXEL);

		SimpleMath::Vector4 textureInfo = { 1024.f,
			1024.f,
			1.f / 1024.f,
			1.f / 1024.f };

		BufferManager::GetInstance()->GetBuffer("TextureInfoCB")->SetUpBuffer(1, &textureInfo, ShaderType::PIXEL);

		BufferManager::GetInstance()->GetBuffer("ColorCB")->SetUpBuffer(2, &ambientSkyColor, ShaderType::PIXEL);

		// Albedo
		D3DDeviceContext->PSSetShaderResources(0, 1, renderTargets[0]->GetShaderResourceView().GetAddressOf());
		//DMRAO
		D3DDeviceContext->PSSetShaderResources(1, 1, renderTargets[1]->GetShaderResourceView().GetAddressOf());
		//Normal
		D3DDeviceContext->PSSetShaderResources(2, 1, renderTargets[2]->GetShaderResourceView().GetAddressOf());
		//Position
		D3DDeviceContext->PSSetShaderResources(3, 1, renderTargets[3]->GetShaderResourceView().GetAddressOf());
		//Emissive
		D3DDeviceContext->PSSetShaderResources(4, 1, renderTargets[4]->GetShaderResourceView().GetAddressOf());
		ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
		//Shadow
		D3DDeviceContext->PSSetShaderResources(5, 1, nullSRV);
		//Shadow
		D3DDeviceContext->PSSetShaderResources(6, 1, pointShadowResource->GetDepthShaderResource().GetAddressOf());
		D3DDeviceContext->PSSetShaderResources(7, 1, spotShadowResource->GetDepthShaderResource().GetAddressOf());


		D3DDeviceContext->PSSetSamplers(0, 1, SamplerManager::GetInstance()->GetLinearClamp().GetAddressOf());
		D3DDeviceContext->PSSetSamplers(1, 1, SamplerManager::GetInstance()->GetComparisonLinear().GetAddressOf());
		D3DDeviceContext->PSSetSamplers(2, 1, SamplerManager::GetInstance()->GetLinearWrap().GetAddressOf());

		// ScreenMesh를 불러와서 화면에 그려준다.
		auto screenMesh = ResourceManager::GetInstance()->basicMeshes["ScreenMesh"];
		D3DDeviceContext->RSSetState(screenMesh->GetRasterState().Get());

		UINT stride = sizeof(VertexStruct::TexVertex);
		UINT offset = 0;

		D3DDeviceContext->IASetPrimitiveTopology(screenMesh->GetPrimitiveTopology());
		D3DDeviceContext->IASetVertexBuffers(0, 1, screenMesh->vertexBuffer.GetAddressOf(), &stride, &offset);
		D3DDeviceContext->IASetIndexBuffer(screenMesh->indexBuffers[0].Get(), DXGI_FORMAT_R32_UINT, 0);

		D3DDeviceContext->DrawIndexed(screenMesh->indexBuffersSize[0], 0, 0);

		EndRender();
	}

	void LightPass::Render(const std::vector<std::shared_ptr<RenderTargetTexture>>& renderTargets,
		std::shared_ptr<DepthStencilResource> pointShadowResource,
		std::shared_ptr<DepthStencilResource> spotShadowResource,
		std::shared_ptr<DepthStencilResource> bakedPointShadowResource,
		std::shared_ptr<DepthStencilResource> bakedSpotShadowResource)
	{
		RenderStart();

		quadVS->SetUpShader();
		PBRLightShadowPS->SetUpShader();

		// cb 생성..
		cbLightPass.DirLights[0] = directionalLight;
		cbLightPass.EyePosW = eyePos;
		cbLightPass.DirLightCnt = DIRECTION_LIGHT_COUNT;
		cbLightPass.PointLightCnt = pointLights.size();
		cbLightPass.SpotLightCnt = spotLights.size();

		for (int i = 0; i < pointLights.size(); i++)
		{
			cbLightPass.PointLights[i] = pointLights[i];
		}

		for (int i = 0; i < spotLights.size(); i++)
		{
			cbLightPass.SpotLights[i] = spotLights[i];
		}

		BufferManager::GetInstance()->GetBuffer("LightCB")->SetUpBuffer(0, &cbLightPass, ShaderType::PIXEL);

		SimpleMath::Vector4 textureInfo = { 1024.f,
			1024.f,
			1.f / 1024.f,
			1.f / 1024.f };

		BufferManager::GetInstance()->GetBuffer("TextureInfoCB")->SetUpBuffer(1, &textureInfo, ShaderType::PIXEL);

		BufferManager::GetInstance()->GetBuffer("ColorCB")->SetUpBuffer(2, &ambientSkyColor, ShaderType::PIXEL);

		// Albedo
		D3DDeviceContext->PSSetShaderResources(0, 1, renderTargets[0]->GetShaderResourceView().GetAddressOf());
		//DMRAO
		D3DDeviceContext->PSSetShaderResources(1, 1, renderTargets[1]->GetShaderResourceView().GetAddressOf());
		//Normal
		D3DDeviceContext->PSSetShaderResources(2, 1, renderTargets[2]->GetShaderResourceView().GetAddressOf());
		//Position
		D3DDeviceContext->PSSetShaderResources(3, 1, renderTargets[3]->GetShaderResourceView().GetAddressOf());
		//Emissive
		D3DDeviceContext->PSSetShaderResources(4, 1, renderTargets[4]->GetShaderResourceView().GetAddressOf());
		ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
		//Shadow
		D3DDeviceContext->PSSetShaderResources(5, 1, nullSRV);
		//Shadow
		D3DDeviceContext->PSSetShaderResources(6, 1, pointShadowResource->GetDepthShaderResource().GetAddressOf());
		D3DDeviceContext->PSSetShaderResources(7, 1, spotShadowResource->GetDepthShaderResource().GetAddressOf());
		D3DDeviceContext->PSSetShaderResources(8, 1, bakedPointShadowResource->GetDepthShaderResource().GetAddressOf());
		D3DDeviceContext->PSSetShaderResources(9, 1, bakedSpotShadowResource->GetDepthShaderResource().GetAddressOf());

		D3DDeviceContext->PSSetSamplers(0, 1, SamplerManager::GetInstance()->GetLinearClamp().GetAddressOf());
		D3DDeviceContext->PSSetSamplers(1, 1, SamplerManager::GetInstance()->GetComparisonLinear().GetAddressOf());
		D3DDeviceContext->PSSetSamplers(2, 1, SamplerManager::GetInstance()->GetLinearWrap().GetAddressOf());

		// ScreenMesh를 불러와서 화면에 그려준다.
		auto screenMesh = ResourceManager::GetInstance()->basicMeshes["ScreenMesh"];
		D3DDeviceContext->RSSetState(screenMesh->GetRasterState().Get());

		UINT stride = sizeof(VertexStruct::TexVertex);
		UINT offset = 0;

		D3DDeviceContext->IASetPrimitiveTopology(screenMesh->GetPrimitiveTopology());
		D3DDeviceContext->IASetVertexBuffers(0, 1, screenMesh->vertexBuffer.GetAddressOf(), &stride, &offset);
		D3DDeviceContext->IASetIndexBuffer(screenMesh->indexBuffers[0].Get(), DXGI_FORMAT_R32_UINT, 0);

		D3DDeviceContext->DrawIndexed(screenMesh->indexBuffersSize[0], 0, 0);

		EndRender();
	}

	void LightPass::Render(const std::vector<std::shared_ptr<RenderTargetTexture>>& renderTargets,
		const std::vector<std::shared_ptr<DepthStencilResource>>& shadowMaps)
	{
		RenderStart();

		quadVS->SetUpShader();
		if(isIndoor == true)
			PBRLightIndoorPS->SetUpShader();
		else
			PBRLightShadowPS->SetUpShader();

		// cb 생성..
		cbLightPass.DirLights[0] = directionalLight;
		cbLightPass.EyePosW = eyePos;
		cbLightPass.DirLightCnt = DIRECTION_LIGHT_COUNT;
		cbLightPass.PointLightCnt = pointLights.size();
		cbLightPass.SpotLightCnt = spotLights.size();

		for (int i = 0; i < pointLights.size(); i++)
		{
			cbLightPass.PointLights[i] = pointLights[i];
		}

		for (int i = 0; i < spotLights.size(); i++)
		{
			cbLightPass.SpotLights[i] = spotLights[i];
		}

		BufferManager::GetInstance()->GetBuffer("LightCB")->SetUpBuffer(0, &cbLightPass, ShaderType::PIXEL);

		SimpleMath::Vector4 textureInfo = { 1024.f,
			1024.f,
			1.f / 1024.f,
			1.f / 1024.f };

		SimpleMath::Vector4 dirTexInfo = { 4096.f,	
			4096.f,
			1 / 4096.f,
			1 / 4096.f
		};

		BufferManager::GetInstance()->GetBuffer("TextureInfoCB")->SetUpBuffer(1, &textureInfo, ShaderType::PIXEL);

		BufferManager::GetInstance()->GetBuffer("ColorCB")->SetUpBuffer(2, &ambientSkyColor, ShaderType::PIXEL);

		BufferManager::GetInstance()->GetBuffer("TexOffsetCB")->SetUpBuffer(3, &dirTexInfo, ShaderType::PIXEL);

		BufferManager::GetInstance()->GetBuffer("ThresholdCB")->SetUpBuffer(4, &IBLOffset, ShaderType::PIXEL);

		// Albedo
		D3DDeviceContext->PSSetShaderResources(0, 1, renderTargets[0]->GetShaderResourceView().GetAddressOf());
		//DMRAO
		D3DDeviceContext->PSSetShaderResources(1, 1, renderTargets[1]->GetShaderResourceView().GetAddressOf());
		//Normal
		D3DDeviceContext->PSSetShaderResources(2, 1, renderTargets[2]->GetShaderResourceView().GetAddressOf());
		//Position
		D3DDeviceContext->PSSetShaderResources(3, 1, renderTargets[3]->GetShaderResourceView().GetAddressOf());
		//Emissive
		D3DDeviceContext->PSSetShaderResources(4, 1, renderTargets[4]->GetShaderResourceView().GetAddressOf());
		//Shadow
		D3DDeviceContext->PSSetShaderResources(5, 1, shadowMaps[0]->GetDepthShaderResource().GetAddressOf());
		//Shadow
		D3DDeviceContext->PSSetShaderResources(6, 1, shadowMaps[1]->GetDepthShaderResource().GetAddressOf());
		D3DDeviceContext->PSSetShaderResources(7, 1, shadowMaps[2]->GetDepthShaderResource().GetAddressOf());
		D3DDeviceContext->PSSetShaderResources(8, 1, shadowMaps[3]->GetDepthShaderResource().GetAddressOf());
		D3DDeviceContext->PSSetShaderResources(9, 1, shadowMaps[4]->GetDepthShaderResource().GetAddressOf());

		if (isIndoor == true)
		{
			ID3D11ShaderResourceView* nullSRV0[1] = { nullptr };
			D3DDeviceContext->PSSetShaderResources(10, 1, nullSRV0);
			ID3D11ShaderResourceView* nullSRV1[1] = { nullptr };
			D3DDeviceContext->PSSetShaderResources(11, 1, nullSRV1);
			ID3D11ShaderResourceView* nullSRV2[1] = { nullptr };
			D3DDeviceContext->PSSetShaderResources(12, 1, nullSRV2);

		}
		else
		{
			D3DDeviceContext->PSSetShaderResources(10, 1, IBLDiffuse->GetMapSRV().GetAddressOf());
			D3DDeviceContext->PSSetShaderResources(11, 1, IBLSpecular->GetMapSRV().GetAddressOf());
			D3DDeviceContext->PSSetShaderResources(12, 1, IBLBRDFLookUp->GetMapSRV().GetAddressOf());
		}

		D3DDeviceContext->PSSetSamplers(0, 1, SamplerManager::GetInstance()->GetLinearClamp().GetAddressOf());
		D3DDeviceContext->PSSetSamplers(1, 1, SamplerManager::GetInstance()->GetComparisonLinear().GetAddressOf());
		D3DDeviceContext->PSSetSamplers(2, 1, SamplerManager::GetInstance()->GetLinearWrap().GetAddressOf());

		// ScreenMesh를 불러와서 화면에 그려준다.
		auto screenMesh = ResourceManager::GetInstance()->basicMeshes["ScreenMesh"];
		D3DDeviceContext->RSSetState(screenMesh->GetRasterState().Get());

		UINT stride = sizeof(VertexStruct::TexVertex);
		UINT offset = 0;

		D3DDeviceContext->IASetPrimitiveTopology(screenMesh->GetPrimitiveTopology());
		D3DDeviceContext->IASetVertexBuffers(0, 1, screenMesh->vertexBuffer.GetAddressOf(), &stride, &offset);
		D3DDeviceContext->IASetIndexBuffer(screenMesh->indexBuffers[0].Get(), DXGI_FORMAT_R32_UINT, 0);

		D3DDeviceContext->DrawIndexed(screenMesh->indexBuffersSize[0], 0, 0);

		EndRender();
	}

	void LightPass::EndRender()
	{
		ID3D11ShaderResourceView* nullSRV[13] = { nullptr };

		D3DDeviceContext->PSSetShaderResources(0, 13, nullSRV);

		directionalLight = PBR::DirectionalLight();
		pointLights.clear();
		spotLights.clear();

		// 다음 Pass의 Start 지점에서 세팅을 다시 해주기 때문에.. 따로 OMSetRenderTargets를 호출하지 않는다.
	}

	void LightPass::IMGuiRender()
	{
		auto imgui = ImGUIManager::GetInstance();

		if(imgui->Begin("Ambient Light"))
		{
			/*
			float AmbientLight[4] = { ambientSkyColor.x, ambientSkyColor.y, ambientSkyColor.z, ambientSkyColor.w };

			imgui->SliderFloat4("Ambient", AmbientLight, 0.0f, 1.0f);

			ambientSkyColor.x = AmbientLight[0];
			ambientSkyColor.y = AmbientLight[1];
			ambientSkyColor.z = AmbientLight[2];
			ambientSkyColor.w = AmbientLight[3];
			*/

			float iblIntensity = IBLOffset.x;

			imgui->DragFloat("Intensity", &iblIntensity, 0.0001f, 0.0f, 1.0f, "%.4f");

			IBLOffset.x = iblIntensity;
		}
		imgui->End();
	}

	void LightPass::SetDirectionalLight(SimpleMath::Vector3 color, SimpleMath::Vector3 direction, float power, bool isShadow, SimpleMath::Matrix lightViewProj)
	{
		directionalLight.color = color;
		directionalLight.direction = direction;
		directionalLight.power = power;
		directionalLight.isShadow = static_cast<float>(isShadow);
		directionalLight.lightViewProj = lightViewProj;
	}

	void LightPass::SetPointLight(SimpleMath::Vector3 color, SimpleMath::Vector3 position, float power, float range, bool isShadow, std::vector<SimpleMath::Matrix> lightViewProj)
	{
		PBR::PointLight pointLight;

		pointLight.color = color;
		pointLight.position = position;
		pointLight.power = power;
		pointLight.range = range;
		pointLight.isShadow = static_cast<float>(isShadow);

		for (int i = 0; i < 6; i++)
		{
			pointLight.lightViewProj[i] = lightViewProj[i];
		}

		pointLights.emplace_back(pointLight);
	}

	void LightPass::SetSpotLight(SimpleMath::Vector3 color, SimpleMath::Vector3 position, SimpleMath::Vector3 direction, float power, float innerSpotAngle, float outerSpotAngle, float range, bool
	                             isShadow, SimpleMath::Matrix lightViewProj)
	{
		PBR::SpotLight spotLight;

		spotLight.color = color;
		spotLight.position = position;
		spotLight.direction = direction;
		spotLight.power = power;
		spotLight.innerAngle = innerSpotAngle;
		spotLight.outerAngle = outerSpotAngle;
		spotLight.isShadow = static_cast<float>(isShadow);
		spotLight.range = range;
		spotLight.lightViewProj = lightViewProj;

		spotLights.emplace_back(spotLight);
	}

	void LightPass::SetAmbientSkyColor(SimpleMath::Vector4 color)
	{
		ambientSkyColor = color;
	}

	void LightPass::SetEyePos(const SimpleMath::Vector3& eyePos)
	{
		this->eyePos = eyePos;
	}
}
