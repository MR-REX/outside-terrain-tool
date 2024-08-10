#ifdef WORKBENCH

class OTT_OutsideTerrainGenerationResult
{
	protected int m_iChunksCount;
	protected int m_iVerticesCount;
	protected int m_iTrianglesCount;
	
	protected int m_iGenerationDuration
	
	void OTT_OutsideTerrainGenerationResult(int chunksCount, int verticesCount, int trianglesCount, int generationDuration)
	{
		m_iChunksCount = chunksCount;
		m_iVerticesCount = verticesCount;
		m_iTrianglesCount = trianglesCount;
		m_iGenerationDuration = generationDuration;
	}
	
	int GetChunksCount()
	{
		return m_iChunksCount;
	}
	
	int GetVerticesCount()
	{
		return m_iVerticesCount;
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