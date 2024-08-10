#ifdef WORKBENCH

class OTT_OutsideTerrainGenerationReport
{
	protected OTT_OutsideTerrainGenerationResult m_generationResult;
	
	void OTT_OutsideTerrainGenerationReport(OTT_OutsideTerrainGenerationResult result)
	{
		m_generationResult = result;
	}
	
	string GetTextContent()
	{
		array<string> lines = {
			"Outside terrain successfully generated.",
			"",
			"Number of chunks: %1",
			"Number of vertices: %2",
			"Number of triangles: %3",
			"",
			"Duration of process: %4"
		};
		
		return string.Format(
			SCR_StringHelper.Join("\n", lines),
			m_generationResult.GetChunksCount(),
			m_generationResult.GetVerticesCount(),
			m_generationResult.GetTrianglesCount(),
			SCR_FormatHelper.FormatTime(m_generationResult.GetGenerationDuration())
		);
	}
	
	void Display()
	{
		Workbench.Dialog("Outside Terrain: Report", GetTextContent());
	}
}

#endif