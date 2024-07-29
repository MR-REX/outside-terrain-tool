#ifdef WORKBENCH

class OTT_OutsideTerrainGenerator
{	
	protected OTT_OutsideTerrainGeneratorOptions m_Options;
	
	void OTT_OutsideTerrainGenerator(notnull OTT_OutsideTerrainGeneratorOptions options)
	{
		m_Options = options;
	}
	
	bool Process(OTT_NoiseModifier noiseModifier = null)
	{
		return false;
	}
}

#endif