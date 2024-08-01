#ifdef WORKBENCH

enum OTT_EOutsideTerrainSize
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

enum OTT_EOutsideTerrainPhysicsType
{
	Disabled,
	Partial,
	Full
}

class OTT_OutsideTerrainGeneratorOptions
{	
	protected OTT_EOutsideTerrainSize m_eSize;
	protected OTT_EOutsideTerrainQuality m_eQuality;
	protected OTT_EOutsideTerrainPhysicsType m_ePhysicsType;
	
	protected ref array<ref OTT_HeightmapModifier> m_aHeightmapModifiers;
	
	void OTT_OutsideTerrainGeneratorOptions(OTT_EOutsideTerrainSize size, OTT_EOutsideTerrainQuality quality, OTT_EOutsideTerrainPhysicsType physicsType, notnull array<ref OTT_HeightmapModifier> modifiers)
	{
		m_eSize = size;
		m_eQuality = quality;
		m_ePhysicsType = physicsType;
		
		m_aHeightmapModifiers = modifiers;
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
}

#endif