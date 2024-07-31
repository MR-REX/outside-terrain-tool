#ifdef WORKBENCH

class OTT_OutsideTerrainGenerator
{	
	protected OTT_OutsideTerrainGeneratorOptions m_Options;
	protected ref array<ref OTT_HeightmapModifier> m_aHeightmapModifiers;
	
	protected int m_iChunksCount;
	protected int m_iVerticlesCount;
	
	void OTT_OutsideTerrainGenerator(notnull OTT_OutsideTerrainGeneratorOptions options, notnull array<ref OTT_HeightmapModifier> modifiers)
	{
		m_Options = options;
		m_aHeightmapModifiers = modifiers;
	}
	
	protected array<ref array<float>> GetTerrainHeightmap(int width, int height)
	{
		OTT_Terrain terrain = new OTT_Terrain();
		
		if (!terrain.IsValid())
			return null;
		
		array<ref array<float>> heightmap = terrain.GetHeightmap(width, height);
		
		foreach (OTT_HeightmapModifier heightmapModifier : m_aHeightmapModifiers)
		{
			heightmapModifier.Modify(heightmap);
		}
		
		return heightmap;
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
			verticlesCount: m_iVerticlesCount
		);
		
		return result;
	}
}

#endif