#pragma once

#include "DeviceResources.h"
#include "Light.h"


//Class from which we create all shader objects used by the framework
//This single class can be expanded to accomodate shaders of all different types with different parameters
class Shader
{
public:
	Shader();
	~Shader();

	//we could extend this to load in only a vertex shader, only a pixel shader etc.  or specialised init for Geometry or domain shader. 
	//All the methods here simply create new versions corresponding to your needs
	bool InitStandard(ID3D11Device * device, WCHAR * vsFilename, WCHAR * psFilename);		//Loads the Vert / pixel Shader pair
	bool InitBloomExtract(ID3D11Device* device, WCHAR* vsFilename, WCHAR* psFilename);
	bool SetShaderParameters(ID3D11DeviceContext * context, DirectX::SimpleMath::Matrix  *world, DirectX::SimpleMath::Matrix  *view, DirectX::SimpleMath::Matrix  *projection, Light *sceneLight1, ID3D11ShaderResourceView* texture1);
	bool SetBlomExtractParameters(ID3D11DeviceContext* context, float t, ID3D11ShaderResourceView* texture1);
	bool SetGaussianBlurParameters(ID3D11DeviceContext* context, DirectX::XMFLOAT4 offset[15], DirectX::XMFLOAT4 weights[15], int count, ID3D11ShaderResourceView* texture1);
	bool SetCombineParameters(ID3D11DeviceContext* context, float blsat, float blin, float basesat, float basein, ID3D11ShaderResourceView* texture1, ID3D11ShaderResourceView* texture2);
	bool SetPerlinParameters(ID3D11DeviceContext* context, DirectX::SimpleMath::Matrix* world, DirectX::SimpleMath::Matrix* view, DirectX::SimpleMath::Matrix* projection, float t, float a, Light* sceneLight1, ID3D11ShaderResourceView* texture1);
	bool SetShaderSkyBoxParameters(ID3D11DeviceContext* context, DirectX::SimpleMath::Vector3 cpos, DirectX::SimpleMath::Matrix* world, DirectX::SimpleMath::Matrix* view, DirectX::SimpleMath::Matrix* projection, ID3D11ShaderResourceView* texture1);
	bool InitSkyBox(ID3D11Device* device, WCHAR* vsFilename, WCHAR* psFilename);
	bool InitGaussianBlur(ID3D11Device* device, WCHAR* vsFilename, WCHAR* psFilename);
	bool InitBloomCombine(ID3D11Device* device, WCHAR* vsFilename, WCHAR* psFilename);
	bool InitPerlinNoise(ID3D11Device* device, WCHAR* vsFilename, WCHAR* psFilename);
	void EnableShader(ID3D11DeviceContext * context);



private:
	//standard matrix buffer supplied to all shaders
	struct MatrixBufferType
	{
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX projection;
	};

	//buffer for information of a single light
	struct LightBufferType
	{
		DirectX::SimpleMath::Vector4 ambient;
		DirectX::SimpleMath::Vector4 diffuse;
		DirectX::SimpleMath::Vector3 position;
		float padding;
	};

	//buffer for bloomextract
	struct BloomExtractBufferType
	{
		float BloomThreshold;
		uint8_t na[12];

	};

	//buffer for bloomextract
	struct GaussianBlurBufferType
	{
		DirectX::XMFLOAT2 sampleOffsets[15];
		//float sampleWeights[15];
		float padding[2];

	};
	struct CombineBufferType
	{
		float bloomSat;
		float baseSat;
		float bloomIn;
		float baseIn;
	

	};
	struct WaterBufferType {
		float time;
		float alpha;
		float padding[2];
	};

	//buffer to pass in camera world Position
	struct CameraBufferType
	{
		DirectX::SimpleMath::Vector3 cameraPosition;
		float padding;
	};

	

	//Shaders
	Microsoft::WRL::ComPtr<ID3D11VertexShader>								m_vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>								m_pixelShader;
	ID3D11InputLayout*														m_layout;
	ID3D11Buffer*															m_matrixBuffer;
	ID3D11SamplerState*														m_sampleState;

	ID3D11Buffer*															m_lightBuffer;
	ID3D11Buffer*															m_bloomBuffer;
	ID3D11Buffer*															m_gaussianBlurBuffer;
	ID3D11Buffer*															m_bloomCombineBuffer;
	ID3D11Buffer*															m_waterBuffer;
	ID3D11Buffer*															m_cameraBuffer;


};

