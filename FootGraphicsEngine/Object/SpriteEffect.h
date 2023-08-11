#pragma once
#include "Interface/IDXObject.h"
#include "Resources/ObjectResources.h"

namespace GraphicsEngineSpace
{
	class ResourceManager;

	/// <summary>
	/// 3D ������ sprite Texture�� �޾Ƽ� ǥ������ Effect Object �Դϴ�.
	/// </summary>
	class SpriteEffect : public IDXObject
	{
		ComPtr<ID3D11Device> D3DDevice;
		ComPtr<ID3D11DeviceContext> D3DDeviceContext;

		std::vector<std::shared_ptr<MeshResources>> meshResources;

		// GetInstance�� ���̱� ���� ������ ����
		std::shared_ptr<ResourceManager> resourceManager;

		// ���÷� �� ���� ĳ���صΰ� ���� �� �����մϴ�.
		ComPtr<ID3D11SamplerState> clampState;
		ComPtr<ID3D11SamplerState> wrapState;

		// TM ���
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX proj;

		// offSet���� ���� ���� float2 ����
		SimpleMath::Vector4 texOffset;
		// Src ����
		SimpleMath::Vector4 texSrc;

		// Color, Ư�� Alpha�� �������ֱ� ���� ����
		SimpleMath::Vector4 maskColor;
		SimpleMath::Vector4 emissiveColor;

		// ���̴��� ������ ��Ʈ����
		__declspec(align(16)) struct cbMatrix
		{
			SimpleMath::Matrix worldViewProj;
			SimpleMath::Matrix worldTransform;
			SimpleMath::Matrix worldInvTranspose;
		};

		// �⺻������ Wrap
		bool isWrap;
		// �⺻������ false
		bool isSRC;

	public:
		// �����ڴ� Export�� ���־���� �Ѵ�.
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

		// ������ ����
		virtual void SetTexSrc(float minX, float minY, float maxX, float maxY);
		virtual void SetTexSrc(const SimpleMath::Vector4& offset);
		virtual SimpleMath::Vector4 GetTexSrc();

		virtual const SimpleMath::Matrix& GetWorld() override { return world; }
		virtual const SimpleMath::Matrix& GetView() override { return view; }
		virtual const SimpleMath::Matrix& GetProj() override { return proj; }

		// �̸��� RimColor����.. ���������δ� mask Color�� �ٲ��ݴϴ�.
		virtual void SetRimColor(const SimpleMath::Vector4& color) override;
		virtual void SetMaterialEmissiveColor(const std::string& matName, const SimpleMath::Vector4& color) override;
		// sprite Effect������ wrap�� �����ϴ� �뵵�� ����մϴ�.
		virtual void SetBool(const std::string& name, bool val) override;
		virtual bool GetWrap() { return isWrap; }
		virtual bool GetSRC() { return isSRC; }

	private:
		std::shared_ptr<Mesh> CreateEffectMesh(std::string name);
	};
}

