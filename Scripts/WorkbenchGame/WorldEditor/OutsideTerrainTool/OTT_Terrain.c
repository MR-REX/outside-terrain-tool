#ifdef WORKBENCH

class OTT_Terrain
{
	private static const float MIN_UNIT_SCALE = 0.001;
	
	protected BaseWorld m_World;
	
	protected vector m_vMins;
	protected vector m_vMaxs;
	protected vector m_vSize;
	
	protected int m_iHeightmapWidth;
	protected int m_iHeightmapHeight;
	
	protected int m_iTilesPerWidth;
	protected int m_iTilesPerHeight;
	
	protected float m_fUnitScale;
	
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
		
		m_iHeightmapWidth = worldEditorApi.GetTerrainResolutionX();
		m_iHeightmapHeight = worldEditorApi.GetTerrainResolutionY();
		
		m_iTilesPerWidth = worldEditorApi.GetTerrainTilesX();
		m_iTilesPerHeight = worldEditorApi.GetTerrainTilesY();
		
		m_fUnitScale = worldEditorApi.GetTerrainUnitScale();
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
	
	int GetHeightmapWidth()
	{
		return m_iHeightmapWidth;
	}
	
	int GetHeightmapHeight()
	{
		return m_iHeightmapHeight;
	}
	
	int GetTilesPerWidth()
	{
		return m_iTilesPerWidth;
	}
	
	int GetTilesPerHeight()
	{
		return m_iTilesPerHeight;
	}
	
	float GetUnitScale()
	{
		return m_fUnitScale;
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
	
	array<ref array<float>> GetHeightmap(int m, int n, bool useUnitScale = false)
	{
		array<ref array<float>> heightmap = new array<ref array<float>>();
		
		float widthStep = m_vSize[0] / n;
		float heightStep = m_vSize[2] / m;
		
		if (useUnitScale)
		{
			widthStep = m_fUnitScale;
			heightStep = m_fUnitScale;
		}
		
		float x, z, height;
		
		for (int i = m - 1; i >= 0; i--)
		{
			array<float> row = new array<float>();
			
			for (int j = 0; j < n; j++)
			{
				x = Math.Clamp(widthStep * j, MIN_UNIT_SCALE, m_vSize[0] - MIN_UNIT_SCALE);
				z = Math.Clamp(heightStep * i, MIN_UNIT_SCALE, m_vSize[2] - MIN_UNIT_SCALE);
				
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