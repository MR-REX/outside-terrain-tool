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
	
	protected int m_iBlocksPerTileWidth;
	protected int m_iBlocksPerTileHeight;
	
	protected ref array<ref OTT_HeightmapModifier> m_aHeightmapModifiers;
	protected OTT_OutsideTerrainContextOptions m_contextOptions;
	
	protected float m_fDepthOffset;
	protected float m_fBorderHeightMultiplier;
	
	void OTT_OutsideTerrainGeneratorOptions(
		OTT_EOutsideTerrainSize size,
		OTT_EOutsideTerrainQuality quality,
		int blocksPerTileWidth,
		int blocksPerTileHeight,
		OTT_EOutsideTerrainPhysicsType physicsType,
		notnull array<ref OTT_HeightmapModifier> modifiers,
		OTT_OutsideTerrainContextOptions contextOptions,
		float depthOffset,
		float borderHeightMultiplier
	)
	{
		m_eSize = size;
		m_eQuality = quality;
		m_ePhysicsType = physicsType;
		
		m_iBlocksPerTileWidth = blocksPerTileWidth;
		m_iBlocksPerTileHeight = blocksPerTileHeight;
		
		m_aHeightmapModifiers = modifiers;
		m_contextOptions = contextOptions;
		
		m_fDepthOffset = depthOffset;
		m_fBorderHeightMultiplier = borderHeightMultiplier;
	}
	
	OTT_EOutsideTerrainSize GetSize()
	{
		return m_eSize;
	}
	
	OTT_EOutsideTerrainQuality GetQuality()
	{
		return m_eQuality;
	}
	
	int GetBlocksPerTileWidth()
	{
		return m_iBlocksPerTileWidth;
	}
	
	int GetBlocksPerTileHeight()
	{
		return m_iBlocksPerTileHeight;
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
	
	float GetDepthOffset()
	{
		return m_fDepthOffset;
	}
	
	float GetBorderHeightMultiplier()
	{
		return m_fBorderHeightMultiplier;
	}
}

#endif