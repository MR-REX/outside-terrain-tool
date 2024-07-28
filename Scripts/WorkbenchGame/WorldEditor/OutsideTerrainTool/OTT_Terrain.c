#ifdef WORKBENCH

class OTT_Terrain
{
	protected BaseWorld m_World;
	protected vector m_vSize;
	
	void OTT_Terrain()
	{
		WorldEditor worldEditor = Workbench.GetModule(WorldEditor);
		
		if (!worldEditor)
			return;
		
		WorldEditorAPI worldEditorApi = worldEditor.GetApi();
		
		if (!worldEditorApi)
			return;
		
		m_World = worldEditorApi.GetWorld();
		
		vector mins, maxs;
		worldEditor.GetTerrainBounds(mins, maxs);
		
		m_vSize = maxs - mins;
	}
	
	bool IsValid()
	{
		return (m_World != null) && (m_vSize.Length() > 0);
	}
	
	array<ref array<float>> GetHeightmap(int m, int n)
	{
		array<ref array<float>> heightmap = new array<ref array<float>>();
		
		float widthStep = m_vSize[0] / n;
		float heightStep = m_vSize[2] / m;
		
		float height;
		
		for (int i = 0; i < m; i++)
		{
			array<float> row = new array<float>();
			
			for (int j = 0; j < n; j++)
			{
				height = m_World.GetSurfaceY(widthStep * j, heightStep * i);
				row.Insert(height);
			}
			
			heightmap.Insert(row);
		}
		
		return heightmap;
	}
	
	array<ref array<float>> GetHeightmap(int size)
	{
		return GetHeightmap(size, size);
	}
}

#endif