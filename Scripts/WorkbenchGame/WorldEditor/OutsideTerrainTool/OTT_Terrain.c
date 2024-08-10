#ifdef WORKBENCH

class OTT_Terrain
{
	protected BaseWorld m_World;
	
	protected vector m_vMins;
	protected vector m_vMaxs;
	protected vector m_vSize;
	
	void OTT_Terrain()
	{
		WorldEditor worldEditor = Workbench.GetModule(WorldEditor);
		
		if (!worldEditor)
			return;
		
		WorldEditorAPI worldEditorApi = worldEditor.GetApi();
		
		if (!worldEditorApi)
			return;
		
		worldEditor.GetTerrainBounds(m_vMins, m_vMaxs);
		
		m_World = worldEditorApi.GetWorld();	
		m_vSize = m_vMaxs - m_vMins;
	}
	
	bool IsValid()
	{
		return (m_World != null) && (m_vSize.Length() > 0);
	}
	
	vector GetMins()
	{
		return m_vMins;
	}
	
	vector GetMaxs()
	{
		return m_vMaxs;
	}
	
	void GetBounds(out vector mins, out vector maxs)
	{
		mins = m_vMins;
		maxs = m_vMaxs;
	}
	
	vector GetSize()
	{
		return m_vSize;
	}
	
	bool HasOcean()
	{
		return m_World.IsOcean();
	}
	
	float GetOceanBaseHeight()
	{
		return m_World.GetOceanBaseHeight();
	}
	
	float GetOceanHeight(float x, float z)
	{
		return m_World.GetOceanHeight(x, z);
	}
	
	float GetSurfaceHeight(float x, float z)
	{
		return m_World.GetSurfaceY(x, z);
	}
	
	array<ref array<float>> GetHeightmap(int m, int n)
	{
		array<ref array<float>> heightmap = new array<ref array<float>>();
		
		float widthStep = m_vSize[0] / n;
		float heightStep = m_vSize[2] / m;
		
		float x, z, height;
		
		for (int i = m - 1; i >= 0; i--)
		{
			array<float> row = new array<float>();
			
			for (int j = 0; j < n; j++)
			{
				x = widthStep * j;
				z = heightStep * i;
				
				height = m_World.GetSurfaceY(x, z);
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