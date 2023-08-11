#include "GraphicsPch.h"
#include "SpriteEffect.h"

#include "Manager/ShaderManager.h"
#include "Manager/BufferManager.h"
#include "Manager/ResourceManager.h"

#include "Shader/ShaderBase.h"
#include "Buffer/BufferBase.h"

#include "GraphicsEngine/Renderer.h"
#include "Manager/SamplerManager.h"
#include "Resources/RenderingMaterial.h"

namespace GraphicsEngineSpace
{
	SpriteEffect::SpriteEffect()
		: D3DDevice(nullptr)
		, D3DDeviceContext(nullptr)
		, resourceManager(nullptr)
		, world()
		, view()
		, proj()
		, texOffset(0.f, 0.f, 0.f, 0.f)
		, texSrc(0.f, 0.f, 1.f, 1.f)
		, maskColor(DirectX::Colors::White)
		, emissiveColor(0.f, 0.f, 0.f, 1.0f)
		, isWrap(true)
		, isSRC(false)
	{
	}

	SpriteEffect::~SpriteEffect()
	{
		for (auto meshRes : meshResources)
		{
			SafeReset(meshRes);
		}
	}

	bool SpriteEffect::Init(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext)
	{
		// 예외 확인
		assert(pDevice);
		assert(pDeviceContext);

		D3DDevice = pDevice;
		D3DDeviceContext = pDeviceContext;

		resourceManager = ResourceManager::GetInstance();

		clampState = SamplerManager::GetInstance()->GetLinearClamp();
		wrapState = SamplerManager::GetInstance()->GetLinearWrap();

		return true;
	}

	void SpriteEffect::Update(const XMMATRIX& _world, const XMMATRIX& _view, const XMMATRIX& _proj)
	{
		// 행렬 넣기
		world = _world;
		view = _view;
		proj = _proj;
	}

	void SpriteEffect::Render()
	{
		assert(D3DDeviceContext);

		std::shared_ptr<Mesh> mesh = meshResources[0]->mesh;

		ShaderManager::GetInstance()->GetShader("SpriteEffectVS")->SetUpShader();
		if(isSRC == true)
			ShaderManager::GetInstance()->GetShader("SpriteEffectSRCPS")->SetUpShader();
		else
			ShaderManager::GetInstance()->GetShader("SpriteEffectPS")->SetUpShader();

		UINT stride = sizeof(VertexStruct::TexVertex);
		UINT offset = 0;
		BufferManager::GetInstance()->GetBuffer("TexOffsetCB")->SetUpBuffer(1, &texOffset, ShaderType::VERTEX);

		cbMatrix cbPerObj;
		cbPerObj.worldViewProj = world * view * proj;
		cbPerObj.worldTransform = world;
		// 안쓸 예정이기 때문..
		cbPerObj.worldInvTranspose = SimpleMath::Matrix::Identity;
		BufferManager::GetInstance()->GetBuffer("MatrixCB")->SetUpBuffer(0, &cbPerObj, ShaderType::VERTEX);

		// 색깔 버퍼를 넣어줍니다.
		BufferManager::GetInstance()->GetBuffer("ColorCB")->SetUpBuffer(0, &maskColor, ShaderType::PIXEL);
		// Emissive를 넣어줍니다.
		BufferManager::GetInstance()->GetBuffer("ThresholdCB")->SetUpBuffer(1, &emissiveColor, ShaderType::PIXEL);
		// 픽셀에 SRC를 넣어줍니다.
		BufferManager::GetInstance()->GetBuffer("TextureInfoCB")->SetUpBuffer(2, &texSrc, ShaderType::PIXEL);

		D3DDeviceContext->IASetPrimitiveTopology(mesh->GetPrimitiveTopology());
		D3DDeviceContext->IASetVertexBuffers(0, 1, mesh->vertexBuffer.GetAddressOf(), &stride, &offset);
		D3DDeviceContext->IASetIndexBuffer(mesh->indexBuffers[0].Get(), DXGI_FORMAT_R32_UINT, 0);

		// Sampler와 texture 세팅
		D3DDeviceContext->PSSetShaderResources(0, 1, meshResources[0]->material[0]->albedoTex->GetMapSRV().GetAddressOf());

		if(isWrap == true)
			D3DDeviceContext->PSSetSamplers(0, 1, wrapState.GetAddressOf());
		else
			D3DDeviceContext->PSSetSamplers(0, 1, clampState.GetAddressOf());


		D3DDeviceContext->RSSetState(mesh->GetRasterState().Get());

		D3DDeviceContext->DrawIndexed(mesh->indexBuffersSize[0], 0, 0);
	}

	void SpriteEffect::Render(std::shared_ptr<IRenderer> renderer)
	{
		assert(D3DDeviceContext);

		renderer->GraphicsDebugBeginEvent(meshResources[0]->ObjName);
		
		Render();

		renderer->GraphicsDebugEndEvent();
	}

	void SpriteEffect::SetMeshResources(std::shared_ptr<MeshResources> objResources)
	{
		meshResources.emplace_back(objResources);
	}

	const std::string& SpriteEffect::GetObjName()
	{
		return meshResources[0]->ObjName;
	}

	std::vector<std::shared_ptr<MeshResources>> SpriteEffect::GetMeshResources()
	{
		return meshResources;
	}

	void SpriteEffect::SetTexOffset(float u, float v)
	{
		texOffset.x = u;
		texOffset.y = v;
	}

	void SpriteEffect::SetTexOffset(const SimpleMath::Vector2& offset)
	{
		texOffset.x = offset.x;
		texOffset.y = offset.y;
	}

	SimpleMath::Vector2 SpriteEffect::GetTexOffset()
	{
		return SimpleMath::Vector2{texOffset.x, texOffset.y};
	}

	void SpriteEffect::SetTexSrc(float minX, float minY, float maxX, float maxY)
	{
		// 이걸 지정했다는 것은 src를 쓰겠다는 뜻.
		isSRC = true;

		texSrc.x = minX;
		texSrc.y = minY;
		texSrc.z = maxX;
		texSrc.w = maxY;
	}

	void SpriteEffect::SetTexSrc(const SimpleMath::Vector4& offset)
	{
		// 이걸 지정했다는 것은 src를 쓰겠다는 뜻.
		isSRC = true;
		texSrc = offset;
	}

	SimpleMath::Vector4 SpriteEffect::GetTexSrc()
	{
		return texSrc;
	}

	void SpriteEffect::SetRimColor(const SimpleMath::Vector4& color)
	{
		maskColor = color;
	}

	void SpriteEffect::SetMaterialEmissiveColor(const std::string& matName, const SimpleMath::Vector4& color)
	{
		// 이름은 상관 없이 Emissive 컬러만 바꿔줍니다.
		emissiveColor = color;
	}

	void SpriteEffect::SetBool(const std::string& name, bool val)
	{
		if(name == "Wrap")
			isWrap = val;
		else if(name == "SRC")
			isSRC = val;
	}

	std::shared_ptr<Mesh> SpriteEffect::CreateEffectMesh(std::string name)
	{
		// 외부에서 불러올 메시(FBX에서 불러온 메시..)
		FBXBinaryData::MeshData binaryMesh;

		std::ifstream ifs("BinaryFile/Mesh/" + name + ".mesh", std::ios_base::binary);
		boost::iostreams::filtering_streambuf<boost::iostreams::input> buffer;
		buffer.push(boost::iostreams::zlib_decompressor());
		buffer.push(ifs);
		boost::archive::binary_iarchive iaMesh(buffer);
		iaMesh >> binaryMesh;

		std::vector<VertexStruct::TexVertex> vertices;

		for (auto& vertex : binaryMesh.meshVertexList)
		{
			auto tempVertex = std::make_unique<VertexStruct::TexVertex>();

			tempVertex->Pos.x = vertex.position.x;
			tempVertex->Pos.y = vertex.position.y;
			tempVertex->Pos.z = vertex.position.z;

			tempVertex->Tex.x = vertex.uv.x;
			tempVertex->Tex.y = vertex.uv.y;

			vertices.push_back(*tempVertex);
		}

		std::vector<std::vector<unsigned int>> indices = binaryMesh.indices;

		auto mesh = resourceManager->CreateMesh(
			vertices,
			indices,
			name,
			D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
			RasterizerState::GetSolidRS()
		);

		mesh->SetName(binaryMesh.meshName);

		return mesh;
	}
}
