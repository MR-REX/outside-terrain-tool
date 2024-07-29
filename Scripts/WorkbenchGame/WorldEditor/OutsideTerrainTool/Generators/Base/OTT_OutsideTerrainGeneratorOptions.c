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

class OTT_OutsideTerrainGeneratorOptions
{
	protected OTT_EOutsideTerrainSize m_eSize;
	protected OTT_EOutsideTerrainQuality m_eQuality;
	
	protected bool m_bEnablePhysics;
	protected EPhysicsLayerPresets m_ePhysicsLayerPreset;
	
	protected ResourceName m_ChunksMaterial;
	
	protected string m_sLayerNameTemplate;
	protected string m_sEntityNameTemplate;
	
	void OTT_OutsideTerrainGeneratorOptions(
		OTT_EOutsideTerrainSize size,
		OTT_EOutsideTerrainQuality quality,
		bool enablePhysics,
		EPhysicsLayerPresets physicsLayerPreset,
		ResourceName chunksMaterial,
		string layerNameTemplate,
		string entityNameTemplate
	)
	{
		m_eSize = size;
		m_eQuality = quality;
		
		m_bEnablePhysics = enablePhysics;
		m_ePhysicsLayerPreset = physicsLayerPreset;
		
		m_ChunksMaterial = chunksMaterial;
		
		m_sLayerNameTemplate = layerNameTemplate;
		m_sEntityNameTemplate = entityNameTemplate;
	}
}

#endif