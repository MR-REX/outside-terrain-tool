#ifdef WORKBENCH

class OTT_OutsideTerrainGenerationResult
{
	protected int m_iChunksCount;
	protected int m_iVerticlesCount;
	protected int m_iTrianglesCount;
	
	protected int m_iGenerationDuration
	
	void OTT_OutsideTerrainGenerationResult(int chunksCount, int verticlesCount, int trianglesCount, int generationDuration)
	{
		m_iChunksCount = chunksCount;
		m_iVerticlesCount = verticlesCount;
		m_iTrianglesCount = trianglesCount;
		m_iGenerationDuration = generationDuration;
	}
	
	int GetChunksCount()
	{
		return m_iChunksCount;
	}
	
	int GetVerticlesCount()
	{
		return m_iVerticlesCount;
	}
	
	int GetTrianglesCount()
	{
		return m_iTrianglesCount;
	}
	
	int GetGenerationDuration()
	{
		return m_iGenerationDuration;
	}
}

#endif