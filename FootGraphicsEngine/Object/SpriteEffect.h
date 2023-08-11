#pragma once
#include "Interface/IDXObject.h"
#include "Resources/ObjectResources.h"

namespace GraphicsEngineSpace
{
	class ResourceManager;

	/// <summary>
	/// 3D 공간에 sprite Texture를 받아서 표현해줄 Effect Object 입니다.
	/// </summary>
	class SpriteEffect : public IDXObject
	{
		ComPtr<ID3D11Device> D3DDevice;
		ComPtr<ID3D11DeviceContext> D3DDeviceContext;

		std::vector<std::shared_ptr<MeshResources>> meshResources;

		// GetInstance를 줄이기 위한 포인터 변수
		std::shared_ptr<ResourceManager> resourceManager;

		// 샘플러 두 개를 캐싱해두고 원할 때 변경합니다.
		ComPtr<ID3D11SamplerState> clampState;
		ComPtr<ID3D11SamplerState> wrapState;

		// TM 행렬
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX proj;

		// offSet으로 들어가기 위한 float2 변수
		SimpleMath::Vector4 texOffset;
		// Src 세팅
		SimpleMath::Vector4 texSrc;

		// Color, 특히 Alpha를 세팅해주기 위한 변수
		SimpleMath::Vector4 maskColor;
		SimpleMath::Vector4 emissiveColor;

		// 쉐이더에 전달할 매트릭스
		__declspec(align(16)) struct cbMatrix
		{
			SimpleMath::Matrix worldViewProj;
			SimpleMath::Matrix worldTransform;
			SimpleMath::Matrix worldInvTranspose;
		};

		// 기본적으로 Wrap
		bool isWrap;
		// 기본적으로 false
		bool isSRC;

	public:
		// 생성자는 Export를 해주어야하 한다.
		GRAPHICSENGINE_DECLSPEC SpriteEffect();
		~SpriteEffect() override;

		virtual bool Init(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pDeviceContext) override;
		virtual void Update(const XMMATRIX& _world, const XMMATRIX& _view, const XMMATRIX& _proj) override;
		virtual void PreRender(float tick) override {}
		virtual void Render() override;
		virtual void Render(std::shared_ptr<IRenderer> renderer) override;
		virtual void YieldRender(std::shared_ptr<IRenderer> renderer) override {}
		virtual void SetMeshResources(std::shared_ptr<MeshResources> objResources) override;
		virtual const std::string& GetObjName() override;
		virtual std::vector<std::shared_ptr<MeshResources>> GetMeshResources() override;

		virtual void SetTexOffset(float u, float v);
		virtual void SetTexOffset(const SimpleMath::Vector2& offset);
		virtual SimpleMath::Vector2 GetTexOffset();

		// 오프셋 세팅
		virtual void SetTexSrc(float minX, float minY, float maxX, float maxY);
		virtual void SetTexSrc(const SimpleMath::Vector4& offset);
		virtual SimpleMath::Vector4 GetTexSrc();

		virtual const SimpleMath::Matrix& GetWorld() override { return world; }
		virtual const SimpleMath::Matrix& GetView() override { return view; }
		virtual const SimpleMath::Matrix& GetProj() override { return proj; }

		// 이름은 RimColor지만.. 내부적으로는 mask Color를 바꿔줍니다.
		virtual void SetRimColor(const SimpleMath::Vector4& color) override;
		virtual void SetMaterialEmissiveColor(const std::string& matName, const SimpleMath::Vector4& color) override;
		// sprite Effect에서는 wrap을 설정하는 용도로 사용합니다.
		virtual void SetBool(const std::string& name, bool val) override;
		virtual bool GetWrap() { return isWrap; }
		virtual bool GetSRC() { return isSRC; }

	private:
		std::shared_ptr<Mesh> CreateEffectMesh(std::string name);
	};
}

