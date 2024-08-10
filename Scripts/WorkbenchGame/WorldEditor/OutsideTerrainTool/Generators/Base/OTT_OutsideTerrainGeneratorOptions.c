#ifdef WORKBENCH

enum OTT_EOutsideTerrainSize
{
	Maximum,
	Large,
	Medium,
	Small
}

enum OTT_EOutsideTerrainQuality
{
	Highest,
	High,
	Medium,
	Low,
	Lowest
}

enum OTT_EOutsideTerrainPhysicsType
{
	Full,
	Huge,
	Partial,
	Small,
	Disabled
}

class OTT_OutsideTerrainGeneratorOptions
{	
	protected OTT_EOutsideTerrainSize m_eSize;
	protected OTT_EOutsideTerrainQuality m_eQuality;
	protected OTT_EOutsideTerrainPhysicsType m_ePhysicsType;
	
	protected ref array<ref OTT_HeightmapModifier> m_aHeightmapModifiers;
	protected OTT_OutsideTerrainContextOptions m_contextOptions;
	
	void OTT_OutsideTerrainGeneratorOptions(OTT_EOutsideTerrainSize size, OTT_EOutsideTerrainQuality quality, OTT_EOutsideTerrainPhysicsType physicsType, notnull array<ref OTT_HeightmapModifier> modifiers, OTT_OutsideTerrainContextOptions contextOptions)
	{
		m_eSize = size;
		m_eQuality = quality;
		m_ePhysicsType = physicsType;
		
		m_aHeightmapModifiers = modifiers;
		m_contextOptions = contextOptions;
	}
	
	OTT_EOutsideTerrainSize GetSize()
	{
		return m_eSize;
	}
	
	OTT_EOutsideTerrainQuality GetQuality()
	{
		return m_eQuality;
	}
	
	OTT_EOutsideTerrainPhysicsType GetPhysicsType()
	{
		return m_ePhysicsType;
	}
	
	array<ref OTT_HeightmapModifier> GetHeightmapModifiers()
	{
		return m_aHeightmapModifiers;
	}
	
	OTT_OutsideTerrainContextOptions GetContextOptions()
	{
		return m_contextOptions;
	}
}

#endif