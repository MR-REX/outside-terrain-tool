#ifdef WORKBENCH

class OTT_OutsideTerrainGenerator
{	
	protected OTT_OutsideTerrainGeneratorOptions m_Options;
	protected OTT_OutsideTerrainManager m_Manager;
	
	protected int m_iChunksCount;
	protected int m_iVerticlesCount;
	protected int m_iTriangles;
	
	void OTT_OutsideTerrainGenerator(notnull OTT_OutsideTerrainGeneratorOptions options, notnull OTT_OutsideTerrainManager manager)
	{
		m_Options = options;
		m_Manager = manager;
	}
	
	protected array<ref array<float>> GetTerrainHeightmap(int width, int height)
	{
		OTT_Terrain terrain = new OTT_Terrain();
		
		if (!terrain.IsValid())
			return null;
		
		array<ref array<float>> heightmap = terrain.GetHeightmap(width, height);
		
		foreach (OTT_HeightmapModifier heightmapModifier : m_Options.GetHeightmapModifiers())
		{
			heightmapModifier.Modify(heightmap);
		}
		
		return heightmap;
	}
	
	protected void CreateChunk(vector position, vector angles, vector size, notnull array<ref array<float>> heightmap, bool enablePhysics)
	{
		int heightmapSize = heightmap.Count();
		
		m_iChunksCount += 1;
		m_iVerticlesCount += heightmapSize * heightmapSize;
		m_iTriangles += (heightmapSize - 1) * (heightmapSize - 1) * 2;
		
		m_Manager.CreateChunk(m_iChunksCount, position, angles, size, heightmap, enablePhysics);
	}
	
	protected bool Process()
	{
		return false;
	}
	
	OTT_OutsideTerrainGenerationResult Execute()
	{
		bool success = Process();
		
		if (!success)
			return null;
		
		OTT_OutsideTerrainGenerationResult result = new OTT_OutsideTerrainGenerationResult(
			chunksCount: m_iChunksCount,
			verticlesCount: m_iVerticlesCount,
			trianglesCount: m_iTriangles
		);
		
		return result;
	}
}

#endif