#ifdef WORKBENCH

class OTT_OutsideTerrainGenerator
{	
	protected OTT_OutsideTerrainGeneratorOptions m_Options;
	
	protected int m_iChunksCount;
	protected int m_iVerticlesCount;
	
	void OTT_OutsideTerrainGenerator(notnull OTT_OutsideTerrainGeneratorOptions options)
	{
		m_Options = options;
	}
	
	bool Process(OTT_NoiseModifier noiseModifier = null)
	{
		return false;
	}
	
	OTT_OutsideTerrainGenerationResult Execute(OTT_NoiseModifier noiseModifier = null)
	{
		bool success = Process(noiseModifier);
		
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