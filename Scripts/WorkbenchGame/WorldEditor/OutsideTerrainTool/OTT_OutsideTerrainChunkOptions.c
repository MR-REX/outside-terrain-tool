#ifdef WORKBENCH

class OTT_OutsideTerrainChunkOptions
{
	protected string m_sEntityNameTemplate;
	protected ResourceName m_Material;
	protected EPhysicsLayerPresets m_ePhysicsLayerPreset;
	protected vector m_vPositionOffset;
	
	void OTT_OutsideTerrainChunkOptions(string entityNameTemplate, ResourceName material, EPhysicsLayerPresets physicsLayerPreset, vector positionOffset)
	{
		m_sEntityNameTemplate = entityNameTemplate;
		m_Material = material;
		m_ePhysicsLayerPreset = physicsLayerPreset;
		m_vPositionOffset = positionOffset;
	}
	
	string GetEntityNameTemplate()
	{
		return m_sEntityNameTemplate;
	}
	
	ResourceName GetMaterial()
	{
		return m_Material;
	}
	
	EPhysicsLayerPresets GetPhyicsLayerPreset()
	{
		return m_ePhysicsLayerPreset;
	}
	
	vector GetPositionOffset()
	{
		return m_vPositionOffset;
	}
}

#endif