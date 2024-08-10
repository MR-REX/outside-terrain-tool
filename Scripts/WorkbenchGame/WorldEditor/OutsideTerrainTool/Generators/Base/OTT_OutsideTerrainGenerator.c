#ifdef WORKBENCH

class OTT_OutsideTerrainGenerator
{	
	protected OTT_OutsideTerrainGeneratorOptions m_Options;
	protected OTT_OutsideTerrainManager m_Manager;
	
	protected ref OTT_Terrain m_Terrain;
	
	protected int m_iChunksCount;
	protected int m_iVerticesCount;
	protected int m_iTriangles;
	
	void OTT_OutsideTerrainGenerator(notnull OTT_OutsideTerrainGeneratorOptions options, notnull OTT_OutsideTerrainManager manager)
	{
		m_Options = options;
		m_Manager = manager;
		
		m_Terrain = new OTT_Terrain();
	}
	
	protected array<ref array<float>> GetTerrainHeightmap(int width, int height)
	{
		array<ref array<float>> heightmap = m_Terrain.GetHeightmap(width, height);
		
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
		m_iVerticesCount += heightmapSize * heightmapSize;
		m_iTriangles += (heightmapSize - 1) * (heightmapSize - 1) * 2;
		
		m_Manager.CreateChunk(m_iChunksCount, position, angles, size, heightmap, enablePhysics);
	}
	
	protected bool Process()
	{
		return false;
	}
	
	OTT_OutsideTerrainGenerationResult Execute()
	{
		int startedAt = System.GetUnixTime();
		bool success = Process();
		
		if (!success)
			return null;
		
		OTT_OutsideTerrainGenerationResult result = new OTT_OutsideTerrainGenerationResult(
			chunksCount: m_iChunksCount,
			verticesCount: m_iVerticesCount,
			trianglesCount: m_iTriangles,
			generationDuration: System.GetUnixTime() - startedAt
		);
		
		return result;
	}
}

#endif