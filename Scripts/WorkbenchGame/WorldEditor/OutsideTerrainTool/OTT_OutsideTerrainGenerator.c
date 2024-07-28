#ifdef WORKBENCH

enum OTT_EOutsideTerrainDistance
{
	Minimal,
	Normal,
	Maximum
}

enum OTT_EOutsideTerrainQuality
{
	Low,
	Medium,
	High
}

class OTT_OutsideTerrainGenerator
{
	protected OTT_EOutsideTerrainDistance m_eDistance;
	protected OTT_EOutsideTerrainQuality m_eQuality;
	
	void OTT_OutsideTerrainGenerator(OTT_EOutsideTerrainDistance distance, OTT_EOutsideTerrainQuality quality)
	{
		m_eDistance = distance;
		m_eQuality = quality;
	}
	
	bool Process(OTT_NoiseModifier noiseModifier)
	{
		return false;
	}
}

#endif