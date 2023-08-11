#include "GraphicsPch.h"
#include "ShadowPass.h"

#include "GraphicsCore/DX11GraphicsCore.h"
#include "GraphicsCore/RenderTargetTexture.h"
#include "GraphicsCore/DepthStencilResource.h"
#include "Interface/IDXObject.h"
#include "Manager/BufferManager.h"

#include "Object/Shader/VertexShader.h"
#include "Object/Shader/GeometryShader.h"
#include "Interface/IDXObject.h"

#include "Resources/RenderingMaterial.h"
#include "Resources/ObjectResources.h"

#include "Manager/ShaderManager.h"
#include "Manager/ResourceManager.h"
#include "Manager/SamplerManager.h"
#include "Object/PBRObj.h"
#include "Object/Buffer/BufferBase.h"

#include "Util/VertexStruct.h"

namespace GraphicsEngineSpace
{
	void ShadowPass::Start(const std::shared_ptr<RenderTargetTexture>& mainRTV,
		const std::shared_ptr<DepthStencilResource>& mainDSV)
	{
		auto shaderManager = ShaderManager::GetInstance();
		// dirlight �����츦 ���� ��.
		shadowStaticVS = std::dynamic_pointer_cast<VertexShader>(shaderManager->GetShader("ShadowStaticVS"));
		shadowSkinnedVS = std::dynamic_pointer_cast<VertexShader>(shaderManager->GetShader("ShadowSkinnedVS"));

		// point light �����츦 ���� ��.
		geoShadowStaticVS = std::dynamic_pointer_cast<VertexShader>(shaderManager->GetShader("GeoShadowStaticVS"));
		geoShadowSkinnedVS = std::dynamic_pointer_cast<VertexShader>(shaderManager->GetShader("GeoShadowSkinnedVS"));

		shadowGS = std::dynamic_pointer_cast<GeometryShader>(shaderManager->GetShader("ShadowGS"));

		this->mainDSV = mainDSV;

		dirShadowDSV = std::make_shared<DepthStencilResource>();
		pointShadowDSV = std::make_shared<DepthStencilResource>();
		pointBakedShadowDSV = std::make_shared<DepthStencilResource>();
		spotShadowDSV = std::make_shared<DepthStencilResource>();
		spotBakedShadowDSV = std::make_shared<DepthStencilResource>();

		dirShadowDSV->DeleteImmediateDepthStencil();
		dirShadowDSV->CreateDepthStencil(D3DDevice, D3DDeviceContext, 4096, 4096, true);

		graphicsCore = DX11GraphicsCore::GetInstance();
	}

	void ShadowPass::Release()
	{
		SafeReset(shadowStaticVS)
			SafeReset(shadowSkinnedVS)
	}

	void ShadowPass::OnResize(int width, int height)
	{

	}

	// ���� �ٲ� �� ���� DSV�� �ʱ�ȭ ���� �ݴϴ�.
	void ShadowPass::ResetIsSetPointLightCnt()
	{
		// Scene�� �ٲ� �� ������ View�� �����ش�.
			// => �̰� �ݵ�� ���� �Ǿ����.
		pointShadowDSV->DeleteImmediateDepthStencil();
		pointBakedShadowDSV->DeleteImmediateDepthStencil();

		isSetPointLightCnt = false;
	}

	void ShadowPass::ResetIsSetSpotLightCnt()
	{
		spotShadowDSV->DeleteImmediateDepthStencil();
		spotBakedShadowDSV->DeleteImmediateDepthStencil();

		isSetSpotLightCnt = false;
	}

	void ShadowPass::SetPointLightArray(int pointLightCnt)
	{
		pointShadowDSV->CreateDepthStencilCube(D3DDevice, D3DDeviceContext, 1024, 1024, pointLightCnt);
		pointBakedShadowDSV->CreateDepthStencilCube(D3DDevice, D3DDeviceContext, 1024, 1024, pointLightCnt);

		/*
		pointShadowDSV->CreateDepthStencilCube(D3DDevice, D3DDeviceContext, 512, 512, pointLightCnt);
		pointBakedShadowDSV->CreateDepthStencilCube(D3DDevice, D3DDeviceContext, 512, 512, pointLightCnt);
		*/

		isSetPointLightCnt = true;
	}

	void ShadowPass::SetSpotLightArray(int spotLightCnt)
	{
		spotShadowDSV->CreateDepthStencilArray(D3DDevice, D3DDeviceContext, 1024, 1024, spotLightCnt);
		spotBakedShadowDSV->CreateDepthStencilArray(D3DDevice, D3DDeviceContext, 1024, 1024, spotLightCnt);

		/*
		spotShadowDSV->CreateDepthStencilArray(D3DDevice, D3DDeviceContext, 512, 512, spotLightCnt);
		spotBakedShadowDSV->CreateDepthStencilArray(D3DDevice, D3DDeviceContext, 512, 512, spotLightCnt);
		*/

		isSetSpotLightCnt = true;
	}

	void ShadowPass::RenderStart()
	{
		// ���� Ÿ�� ���� ���� DSV�� �������༭ SRV�� ������ش�.
			// ��ǻ� �̺κ��� Begin Render ���� �����ҵ�..
			// ���� Ÿ�� Ŭ���� => Shadow Render => EndRender
			// ������Ʈ ���� (Begin, Render, End)
			// Light Render ������ ����Ǿ���� ��.

		D3DDeviceContext->RSSetViewports(1, dirShadowDSV->GetViewport().get());

		graphicsCore->ResetDepthStencilView(dirShadowDSV->GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		ID3D11RenderTargetView* renderTargets[1] = { 0 };
		D3DDeviceContext->OMSetRenderTargets(1, renderTargets, dirShadowDSV->GetDepthStencilView().Get());
	}

	void ShadowPass::Render(std::shared_ptr<IDXObject> object)
	{
		// object ������ �������� ������.. mesh �ϳ��� shadow�� ���ٸ�
				// ������Ʈ ��ü�� shadow�� ���� ��..
		if ((object->GetMeshResources()[0]->isShadow != true) || (object->GetIsLight() != true))
			return;

		// �ε��߿� �ε��츦 �׸��� �ʽ��ϴ�.
		if (object->GetLoadCompleted() != true)
			return;

		// ������Ʈ �� ��� ���۸� ������ش�.
		cbMatrix cbPerObj;
		SimpleMath::Matrix world = object->GetWorld();

		std::vector<SimpleMath::Matrix> nowBoneTM;
		if (std::dynamic_pointer_cast<PBRObj>(object) != nullptr)
			nowBoneTM = std::dynamic_pointer_cast<PBRObj>(object)->GetNowBoneTM();

		// ������Ʈ�� �޾Ƽ� �����ش�.
		for (auto& meshRes : object->GetMeshResources())
		{
			if ((meshRes->isSkinnedMesh != true))
				return;

			if (meshRes->isRender != true)
				continue;

			UINT stride;
			UINT offset = 0;

			// ��Ű�� �޽�
			shadowSkinnedVS->SetUpShader();
			stride = sizeof(VertexStruct::PBRSkinnedVertex);

			cbBoneMatrix cbBone;

			for (int i = 0; i < nowBoneTM.size(); i++)
			{
				cbBone.boneMatrix[i] = nowBoneTM[i];
			}

			BufferManager::GetInstance()->GetBuffer("BoneMatrixCB")->SetUpBuffer(2, &cbBone, ShaderType::VERTEX);

			cbPerObj.worldTransform = world;

			auto material = meshRes->material;

			for (int matCnt = 0; matCnt < material.size(); matCnt++)
			{
				if (meshRes->mesh->indexBuffersSize.empty())
					break;

				// ��Ų�� �ƴϸ� Continue
				if (meshRes->mesh->materials[matCnt] != "SkinMaterial")
					continue;

				// ��� ���� ����.
				BufferManager::GetInstance()->GetBuffer("MatrixCB")->SetUpBuffer(0, &cbPerObj, ShaderType::VERTEX);

				BufferManager::GetInstance()->GetBuffer("LightViewProjCB")->SetUpBuffer(1, &lightViewProj, ShaderType::VERTEX);

				D3DDeviceContext->PSSetShader(nullptr, nullptr, 0);
				D3DDeviceContext->RSSetState(RasterizerState::GetSpotDepthRS().Get());

				D3DDeviceContext->IASetPrimitiveTopology(meshRes->mesh->GetPrimitiveTopology());
				D3DDeviceContext->IASetVertexBuffers(0, 1, meshRes->mesh->vertexBuffer.GetAddressOf(), &stride, &offset);
				D3DDeviceContext->IASetIndexBuffer(meshRes->mesh->indexBuffers[matCnt].Get(), DXGI_FORMAT_R32_UINT, 0);

				D3DDeviceContext->DrawIndexed(meshRes->mesh->indexBuffersSize[matCnt], 0, 0);
			}
		}
	}

	void ShadowPass::Bake(std::shared_ptr<IDXObject> object)
	{
		if (object->GetMeshResources()[0]->isShadow != true || object->GetIsLight() != true)
			return;

		// ������Ʈ �� ��� ���۸� ������ش�.
		cbMatrix cbPerObj;
		SimpleMath::Matrix world = object->GetWorld();

		// ������Ʈ�� �޾Ƽ� �����ش�.
		for (auto& meshRes : object->GetMeshResources())
		{
			UINT stride;
			UINT offset = 0;

			if (meshRes->isSkinnedMesh == true)
				return;

			shadowStaticVS->SetUpShader();
			stride = sizeof(VertexStruct::PBRStaticVertex);

			auto newWorld = world;
			cbPerObj.worldTransform = newWorld;

			auto material = meshRes->material;

			for (int matCnt = 0; matCnt < material.size(); matCnt++)
			{
				if (meshRes->mesh->indexBuffersSize.empty())
					break;

				// ���� üũ
				if (material[matCnt]->material_Transparency > 0.f)
					continue;

				// ��� ���� ����.
				BufferManager::GetInstance()->GetBuffer("MatrixCB")->SetUpBuffer(0, &cbPerObj, ShaderType::VERTEX);

				BufferManager::GetInstance()->GetBuffer("LightViewProjCB")->SetUpBuffer(1, &lightViewProj, ShaderType::VERTEX);

				D3DDeviceContext->PSSetShader(nullptr, nullptr, 0);
				D3DDeviceContext->RSSetState(RasterizerState::GetSpotDepthRS().Get());

				D3DDeviceContext->IASetPrimitiveTopology(meshRes->mesh->GetPrimitiveTopology());
				D3DDeviceContext->IASetVertexBuffers(0, 1, meshRes->mesh->vertexBuffer.GetAddressOf(), &stride, &offset);
				D3DDeviceContext->IASetIndexBuffer(meshRes->mesh->indexBuffers[matCnt].Get(), DXGI_FORMAT_R32_UINT, 0);

				D3DDeviceContext->DrawIndexed(meshRes->mesh->indexBuffersSize[matCnt], 0, 0);
			}
		}
	}

	void ShadowPass::EndRender()
	{
		ID3D11RenderTargetView* renderTargets[1] = { 0 };

		// ����� ���� ������Ʈ�� ���̴��� nullptr�� �������ݴϴ�.
		D3DDeviceContext->GSSetShader(nullptr, nullptr, 0);

		// ������ �������..
		D3DDeviceContext->OMSetRenderTargets(1, renderTargets, mainDSV->GetDepthStencilView().Get());
		D3DDeviceContext->RSSetViewports(1, mainDSV->GetViewport().get());
	}

	void ShadowPass::PointRenderStart(int idx)
	{
		D3DDeviceContext->RSSetViewports(1, pointShadowDSV->GetViewport().get());

		auto depthStencilArray = pointShadowDSV->GetDepthStencilArray();
		auto tmpDepthStencil = depthStencilArray[idx];

		graphicsCore->ResetDepthStencilView(tmpDepthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		ID3D11RenderTargetView* renderTargets[1] = { 0 };
		D3DDeviceContext->OMSetRenderTargets(1, renderTargets, tmpDepthStencil.Get());
	}

	void ShadowPass::PointBakeStart(int idx)
	{
		D3DDeviceContext->RSSetViewports(1, pointBakedShadowDSV->GetViewport().get());

		auto depthStencilArray = pointBakedShadowDSV->GetDepthStencilArray();
		auto tmpDepthStencil = depthStencilArray[idx];

		graphicsCore->ResetDepthStencilView(tmpDepthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		ID3D11RenderTargetView* renderTargets[1] = { 0 };
		D3DDeviceContext->OMSetRenderTargets(1, renderTargets, tmpDepthStencil.Get());
	}

	void ShadowPass::PointRender(std::shared_ptr<IDXObject> object)
	{
		if (object->GetMeshResources()[0]->isShadow != true || object->GetIsLight() != true)
			return;

		// �ε��߿� �ε��츦 �׸��� �ʽ��ϴ�.
		if (object->GetLoadCompleted() != true)
			return;

		cbMatrix cbPerObj;
		SimpleMath::Matrix world = object->GetWorld();

		std::vector<SimpleMath::Matrix> nowBoneTM;
		if (std::dynamic_pointer_cast<PBRObj>(object) != nullptr)
			nowBoneTM = std::dynamic_pointer_cast<PBRObj>(object)->GetNowBoneTM();

		// ������Ʈ�� �޾Ƽ� �����ش�.
		for (auto& meshRes : object->GetMeshResources())
		{
			if (meshRes->isSkinnedMesh != true)
				return;

			if (meshRes->isRender != true)
				continue;

			UINT stride;
			UINT offset = 0;

			// ��Ű�� �޽�
			geoShadowSkinnedVS->SetUpShader();
			stride = sizeof(VertexStruct::PBRSkinnedVertex);

			cbBoneMatrix cbBone;

			for (int i = 0; i < nowBoneTM.size(); i++)
			{
				cbBone.boneMatrix[i] = nowBoneTM[i];
			}

			BufferManager::GetInstance()->GetBuffer("BoneMatrixCB")->SetUpBuffer(2, &cbBone, ShaderType::VERTEX);

			cbPerObj.worldTransform = world;

			auto material = meshRes->material;

			// Skin Material�� ���ؼ� ��ο� ���� �ٿ����ô�..
			for (int matCnt = 0; matCnt < material.size(); matCnt++)
			{
				if (meshRes->mesh->indexBuffersSize.empty())
					break;

				// ��Ų�� �ƴϸ� Continue
				if (meshRes->mesh->materials[matCnt] != "SkinMaterial")
					continue;

				// ��� ���� ����.
				BufferManager::GetInstance()->GetBuffer("MatrixCB")->SetUpBuffer(0, &cbPerObj, ShaderType::VERTEX);

				// Geometry�� light Matrix ���۸� �־��ݴϴ�.
					// Ư�� ����Ʈ Light�� lightViewProj�� ���;� �մϴ�.
				BufferManager::GetInstance()->GetBuffer("LightMatrixCB")->SetUpBuffer(1, &lightMatrix, ShaderType::GEOMETRY);

				shadowGS->SetUpShader();

				D3DDeviceContext->PSSetShader(nullptr, nullptr, 0);
				D3DDeviceContext->RSSetState(RasterizerState::GetPointDepthRS().Get());

				D3DDeviceContext->IASetPrimitiveTopology(meshRes->mesh->GetPrimitiveTopology());
				D3DDeviceContext->IASetVertexBuffers(0, 1, meshRes->mesh->vertexBuffer.GetAddressOf(), &stride, &offset);
				D3DDeviceContext->IASetIndexBuffer(meshRes->mesh->indexBuffers[matCnt].Get(), DXGI_FORMAT_R32_UINT, 0);

				D3DDeviceContext->DrawIndexed(meshRes->mesh->indexBuffersSize[matCnt], 0, 0);
			}
		}
	}

	void ShadowPass::PointBake(std::shared_ptr<IDXObject> object)
	{
		if (object->GetMeshResources()[0]->isShadow != true)
			return;

		cbMatrix cbPerObj;
		SimpleMath::Matrix world = object->GetWorld();

		// ������Ʈ�� �޾Ƽ� �����ش�.
		for (auto& meshRes : object->GetMeshResources())
		{
			UINT stride;
			UINT offset = 0;

			// Skin ���� üũ
			if (meshRes->isSkinnedMesh == true)
				return;

			// ���� �޽�
			geoShadowStaticVS->SetUpShader();
			stride = sizeof(VertexStruct::PBRStaticVertex);

			auto newWorld = world;
			cbPerObj.worldTransform = newWorld;

			auto material = meshRes->material;


			for (int matCnt = 0; matCnt < material.size(); matCnt++)
			{
				if (meshRes->mesh->indexBuffersSize.empty())
					break;

				// ���� üũ
				if (material[matCnt]->material_Transparency > 0.f)
					continue;

				// ��� ���� ����.
				BufferManager::GetInstance()->GetBuffer("MatrixCB")->SetUpBuffer(0, &cbPerObj, ShaderType::VERTEX);

				// Geometry�� light Matrix ���۸� �־��ݴϴ�.
					// Ư�� ����Ʈ Light�� lightViewProj�� ���;� �մϴ�.
				BufferManager::GetInstance()->GetBuffer("LightMatrixCB")->SetUpBuffer(1, &lightMatrix, ShaderType::GEOMETRY);

				shadowGS->SetUpShader();

				D3DDeviceContext->PSSetShader(nullptr, nullptr, 0);
				D3DDeviceContext->RSSetState(RasterizerState::GetPointDepthRS().Get());

				D3DDeviceContext->IASetPrimitiveTopology(meshRes->mesh->GetPrimitiveTopology());
				D3DDeviceContext->IASetVertexBuffers(0, 1, meshRes->mesh->vertexBuffer.GetAddressOf(), &stride, &offset);
				D3DDeviceContext->IASetIndexBuffer(meshRes->mesh->indexBuffers[matCnt].Get(), DXGI_FORMAT_R32_UINT, 0);

				D3DDeviceContext->DrawIndexed(meshRes->mesh->indexBuffersSize[matCnt], 0, 0);
			}
		}
	}

	void ShadowPass::SpotRenderStart(int idx)
	{
		D3DDeviceContext->RSSetViewports(1, spotShadowDSV->GetViewport().get());

		auto depthStencilArray = spotShadowDSV->GetDepthStencilArray();
		auto tmpDepthStencil = depthStencilArray[idx];

		graphicsCore->ResetDepthStencilView(tmpDepthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		ID3D11RenderTargetView* renderTargets[1] = { 0 };
		D3DDeviceContext->OMSetRenderTargets(1, renderTargets, tmpDepthStencil.Get());
	}

	void ShadowPass::SpotBakeStart(int idx)
	{
		D3DDeviceContext->RSSetViewports(1, spotBakedShadowDSV->GetViewport().get());

		auto depthStencilArray = spotBakedShadowDSV->GetDepthStencilArray();
		auto tmpDepthStencil = depthStencilArray[idx];

		graphicsCore->ResetDepthStencilView(tmpDepthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		ID3D11RenderTargetView* renderTargets[1] = { 0 };
		D3DDeviceContext->OMSetRenderTargets(1, renderTargets, tmpDepthStencil.Get());
	}

	void ShadowPass::SetLightViewProj(SimpleMath::Matrix lightViewProj)
	{
		this->lightViewProj = lightViewProj;
	}

	void ShadowPass::SetLightMatrix(std::vector<SimpleMath::Matrix> lightMatrix)
	{
		for (int i = 0; i < 6; i++)
		{
			this->lightMatrix.lightViewProj[i] = lightMatrix[i];
		}
	}

	void ShadowPass::SetLightMatrix(PBR::PointLight pointLight)
	{
		for (int i = 0; i < 6; i++)
		{
			this->lightMatrix.lightViewProj[i] = pointLight.lightViewProj[i];
		}
	}
}
