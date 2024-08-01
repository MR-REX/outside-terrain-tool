#ifdef WORKBENCH

class OTT_OutsideTerrainChunkOptions
{
	protected string m_sEntityNameTemplate;
	protected ResourceName m_Material;
	protected EPhysicsLayerPresets m_ePhysicsLayerPreset;
	
	void OTT_OutsideTerrainChunkOptions(string entityNameTemplate, ResourceName material, EPhysicsLayerPresets physicsLayerPreset)
	{
		m_sEntityNameTemplate = entityNameTemplate;
		m_Material = material;
		m_ePhysicsLayerPreset = physicsLayerPreset;
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
}

#endif