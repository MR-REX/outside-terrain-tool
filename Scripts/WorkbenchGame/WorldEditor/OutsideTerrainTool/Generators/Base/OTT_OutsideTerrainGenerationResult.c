#ifdef WORKBENCH

class OTT_OutsideTerrainGenerationResult
{
	protected int m_iChunksCount;
	protected int m_iVerticlesCount;
	protected int m_iTrianglesCount;
	
	void OTT_OutsideTerrainGenerationResult(int chunksCount, int verticlesCount, int trianglesCount)
	{
		m_iChunksCount = chunksCount;
		m_iVerticlesCount = verticlesCount;
		m_iTrianglesCount = trianglesCount;
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
}

#endif