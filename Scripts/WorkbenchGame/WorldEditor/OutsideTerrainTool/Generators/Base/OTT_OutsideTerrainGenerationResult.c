#ifdef WORKBENCH

class OTT_OutsideTerrainGenerationResult
{
	protected int m_iChunksCount;
	protected int m_iVerticlesCount;
	
	void OTT_OutsideTerrainGenerationResult(int chunksCount, int verticlesCount)
	{
		m_iChunksCount = chunksCount;
	}
	
	int GetChunksCount()
	{
		return m_iChunksCount;
	}
	
	int GetVerticlesCount()
	{
		return m_iVerticlesCount;
	}
}

#endif