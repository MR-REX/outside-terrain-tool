#ifdef WORKBENCH

class OTT_OutsideTerrainManager
{
	protected const string CHUNK_ENTITY_CLASSNAME = "OTC_OutsideTerrainChunkEntity";
	
	protected WorldEditorAPI m_WorldEditorAPI;
	protected OTT_OutsideTerrainChunkOptions m_ChunkOptions;
	
	protected int m_iLayerID;
	
	void OTT_OutsideTerrainManager(WorldEditorAPI worldEditorAPI, OTT_OutsideTerrainChunkOptions chunkOptions)
	{
		m_WorldEditorAPI = worldEditorAPI;
		m_ChunkOptions = chunkOptions;
		m_iLayerID = -1;
	}
	
	void Initialize(string layerName)
	{
		int subsceneId = m_WorldEditorAPI.GetCurrentSubScene();
		int layerId = m_WorldEditorAPI.GetEntityLayerId(subsceneId, layerName);
		
		if (layerId == -1)
		{
			m_iLayerID = m_WorldEditorAPI.CreateSubsceneLayer(subsceneId, layerName);
			return;
		}
		
		// TODO
	}
	
	bool IsValid()
	{
		return m_iLayerID != -1;
	}
	
	IEntitySource CreateChunk(int id, vector position, vector angles, vector size, notnull array<ref array<float>> heightmap, bool enablePhysics)
	{
		if (!OTT_HeightmapHelper.IsHeightmapSquare(heightmap))
		{
			Print(string.Format("Attempt to create chunk (ID: %1) with non-squad heightmap", id), LogLevel.ERROR);
			return null;
		}
		
		string name = string.Format(m_ChunkOptions.GetEntityNameTemplate(), id);
		IEntitySource entitySource = m_WorldEditorAPI.CreateEntity(CHUNK_ENTITY_CLASSNAME, name, m_iLayerID, null, position, angles);
		
		m_WorldEditorAPI.SetVariableValue(entitySource, null, "m_Material", m_ChunkOptions.GetMaterial());
		
		m_WorldEditorAPI.SetVariableValue(entitySource, null, "m_fWidth", size[0].ToString());
		m_WorldEditorAPI.SetVariableValue(entitySource, null, "m_fHeight", size[2].ToString());
		
		m_WorldEditorAPI.SetVariableValue(entitySource, null, "m_bEnableCollision", enablePhysics.ToString(true));
		m_WorldEditorAPI.SetVariableValue(entitySource, null, "m_ePhysicsLayerPreset", m_ChunkOptions.GetPhyicsLayerPreset().ToString());
		
		return entitySource;
	}
}

#endif