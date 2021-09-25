#pragma once

using namespace DirectX;

class Terrain
{
private:
	struct VertexType
	{
		DirectX::SimpleMath::Vector3 position;
		DirectX::SimpleMath::Vector2 texture;
		DirectX::SimpleMath::Vector3 normal;
	};
	struct HeightMapType
	{
		float x, y, z;
		float nx, ny, nz;
		float u, v;
	};
public:
	Terrain();
	~Terrain();

	bool Initialize(ID3D11Device*, int terrainWidth, int terrainHeight);
	void ResetTerrain(ID3D11Device*, int terrainWidth, int terrainHeight);
	void Render(ID3D11DeviceContext*);
	bool GenerateHeightMap(ID3D11Device*);
	float GenerateRandomHeight();
	void Smoothing(ID3D11Device*);
	bool Update();
	float* GetWavelength();
	float* GetAmplitude();
	int* GetRange();
	void GeneratePerlinNoise();
	void GenerateMidPoint(int size, int range, bool initialize, ID3D11Device* device);
	void SquareStep(int x, int y, int size, int range);
	void DiamondStep(int x, int y, int size, int range);
	int CheckLimitsDiamond(int x, int y);

	bool activateMidPoint = false;
	int m_range;

private:
	bool CalculateNormals();
	void Shutdown();
	void ShutdownBuffers();
	bool InitializeBuffers(ID3D11Device*);
	void RenderBuffers(ID3D11DeviceContext*);
	

private:
	bool m_terrainGeneratedToggle;
	int m_terrainWidth, m_terrainHeight;
	ID3D11Buffer * m_vertexBuffer, *m_indexBuffer;
	int m_vertexCount, m_indexCount;
	float m_frequency, m_amplitude, m_wavelength;
	HeightMapType* m_heightMap;
	std::vector<int> index_peaks;
	//arrays for our generated objects Made by directX
	std::vector<VertexPositionNormalTexture> preFabVertices;
	std::vector<uint16_t> preFabIndices;
};

