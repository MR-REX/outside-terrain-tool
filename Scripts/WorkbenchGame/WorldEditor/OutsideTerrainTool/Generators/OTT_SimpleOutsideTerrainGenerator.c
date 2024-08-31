#ifdef WORKBENCH

class OTT_SimpleOutsideTerrainGenerator : OTT_OutsideTerrainGenerator
{
	protected int GetChunkResolution()
	{
		OTT_EOutsideTerrainQuality quality = m_Options.GetQuality();
		
		switch (quality)
		{
			case OTT_EOutsideTerrainQuality.Highest:
				return 32;
			case OTT_EOutsideTerrainQuality.High:
				return 24;
			case OTT_EOutsideTerrainQuality.Medium:
				return 16;
			case OTT_EOutsideTerrainQuality.Low:
				return 8;
			case OTT_EOutsideTerrainQuality.Lowest:
				return 4;
		}
		
		return 0;
	}
	
	protected int GetChunksCount()
	{
		OTT_EOutsideTerrainQuality quality = m_Options.GetQuality();
		
		switch (quality)
		{
			case OTT_EOutsideTerrainQuality.Highest:
				return 32;
			case OTT_EOutsideTerrainQuality.High:
				return 24;
			case OTT_EOutsideTerrainQuality.Medium:
				return 16;
			case OTT_EOutsideTerrainQuality.Low:
				return 8;
			case OTT_EOutsideTerrainQuality.Lowest:
				return 4;
		}
		
		return 0;
	}
	
	protected float GetChunksDepthMultiplier()
	{
		OTT_EOutsideTerrainSize size = m_Options.GetSize();
		
		switch (size)
		{
			case OTT_EOutsideTerrainSize.Maximum:
				return 1;
			case OTT_EOutsideTerrainSize.Large:
				return 0.75;
			case OTT_EOutsideTerrainSize.Medium:
				return 0.5;
			case OTT_EOutsideTerrainSize.Small:
				return 0.25;
		}
		
		return 0;
	}
	
	protected float GetChunkPhysicsDepthMultiplier()
	{
		OTT_EOutsideTerrainPhysicsType physicsType = m_Options.GetPhysicsType();
		
		switch (physicsType)
		{
			case OTT_EOutsideTerrainPhysicsType.Full:
				return 1;
			case OTT_EOutsideTerrainPhysicsType.Huge:
				return 0.75;
			case OTT_EOutsideTerrainPhysicsType.Partial:
				return 0.5;
			case OTT_EOutsideTerrainPhysicsType.Small:
				return 0.25;
		}
		
		return 0;
	}
	
	protected bool ShouldProcessSide(float x, float z)
	{
		OTT_OutsideTerrainContextOptions contextOptions = m_Options.GetContextOptions();
		
		if (!contextOptions.ShouldTrackOceanLevel())
			return true;
		
		return m_Terrain.GetSurfaceHeight(x, z) > m_Terrain.GetOceanHeight(x, z);
	}
	
	override protected bool Process()
	{
		// Getting context options
		
		OTT_OutsideTerrainContextOptions contextOptions = m_Options.GetContextOptions();
		
		// Getting chunks configuration
		
		int chunkResolution = GetChunkResolution();
		int chunksCount = GetChunksCount();
		
		if (chunkResolution == 0 || chunksCount == 0)
			return false;
		
		// Getting terrain heightmap
		
		int terrainHeightmapResolution = chunkResolution * chunksCount;
		
		if (terrainHeightmapResolution % 2 != 0)
			return false;
		
		array<ref array<float>> terrainHeightmap = GetTerrainHeightmap(
			width: terrainHeightmapResolution,
			height: terrainHeightmapResolution
		);
		
		// Getting chunks depth
		
		float chunksDepthMultiplier = GetChunksDepthMultiplier();
		int chunksDepth = Math.Ceil(chunksCount * chunksDepthMultiplier);
		
		if (chunksDepth == 0 || chunksDepth > chunksCount)
			return false;
		
		// Getting depth of chunks with physics
		
		float chunkPhysicsDepthMultiplier = GetChunkPhysicsDepthMultiplier();
		int chunkPhysicsDepth = Math.Ceil(chunksDepth * chunkPhysicsDepthMultiplier);
		
		// Joining chunks border heights on terrain heightmap
		
		float height;
		
		for (int i = 0; i < terrainHeightmapResolution; i++)
		{
			for (int j = chunkResolution - 1; j <= terrainHeightmapResolution - chunkResolution - 1; j += chunkResolution)
			{
				height = Math.Min(terrainHeightmap[i][j], terrainHeightmap[i][j + 1]);
				
				terrainHeightmap[i][j] = height;
				terrainHeightmap[i][j + 1] = height;
				
				height = Math.Min(terrainHeightmap[j][i], terrainHeightmap[j + 1][i]);
				
				terrainHeightmap[j][i] = height;
				terrainHeightmap[j + 1][i] = height;
			}
		}
		
		// Getting terrain properties
		
		vector terrainMins = m_Terrain.GetMins();
		vector terrainSize = m_Terrain.GetSize();
		
		// Getting outside terrain border properties
		
		float chunksDepthOffset = m_Options.GetDepthOffset();
		float borderHeightMultiplier = m_Options.GetBorderHeightMultiplier();
		
		// Calculating outside terrain position
		
		vector outsideTerrainPosition = {terrainMins[0], 0, terrainMins[2]};
		
		// Calculating chunk sizes
		
		float chunkWidth = terrainSize[0] / chunksCount;
		float chunkHeight = terrainSize[2] / chunksCount;
		
		vector chunkSize = {chunkWidth, 0, chunkHeight};
		
		// Horizontal flipping terrain heightmap for z-axis planes
		
		OTT_HeightmapHelper.FlipHorizontal(terrainHeightmap);
		
		// Creating copy of terrain heightmap for angular planes
		
		array<ref array<float>> cachedHeightmap = OTT_HeightmapHelper.Resize(terrainHeightmap, terrainHeightmapResolution, terrainHeightmapResolution);
		
		// Initializing temporary variables
		
		array<ref array<float>> chunkHeightmap;
		
		int i, j;
		
		vector position;
		bool enablePhysics;
		
		// Creating plane of chunks at north side
		
		m_Manager.SetChunksPrefix("North");
		
		if (ShouldProcessSide(terrainSize[0] / 2, terrainSize[2] - 1) && !contextOptions.ShouldIgnoreDirection(OTT_CardinalDirections.North))
		{
			for (int z = 0; z < chunksDepth; z++)
			{
				for (int x = 0; x < chunksCount; x++)
				{
					i = terrainHeightmapResolution - chunkResolution * (z + 1);
					j = chunkResolution * x;
					
					chunkHeightmap = OTT_HeightmapHelper.Select(terrainHeightmap, j, i, chunkResolution, chunkResolution);
					
					if (z == 0)
					{
						for (int k = 0; k < chunkResolution; k++)
						{
							chunkHeightmap[chunkResolution - 1][k] = chunkHeightmap[chunkResolution - 1][k] * borderHeightMultiplier;
						}
					}
					
					enablePhysics = z < chunkPhysicsDepth;
					
					position = {
						outsideTerrainPosition[0] + (chunkWidth / 2) + (chunkWidth * x),
						outsideTerrainPosition[1],
						outsideTerrainPosition[2] + terrainSize[2] + (chunkHeight / 2) + (chunkHeight * z) + chunksDepthOffset
					};
					
					CreateChunk(position, vector.Zero, chunkSize, chunkHeightmap, enablePhysics);
				}
			}
		}
		
		// Creating plane of chunks at south side
		
		m_Manager.SetChunksPrefix("South");
		
		if (ShouldProcessSide(terrainSize[0] / 2, 0) && !contextOptions.ShouldIgnoreDirection(OTT_CardinalDirections.South))
		{
			for (int z = 0; z < chunksDepth; z++)
			{
				for (int x = 0; x < chunksCount; x++)
				{
					i = chunkResolution * z;
					j = chunkResolution * x;
					
					chunkHeightmap = OTT_HeightmapHelper.Select(terrainHeightmap, j, i, chunkResolution, chunkResolution);
					
					if (z == 0)
					{
						for (int k = 0; k < chunkResolution; k++)
						{
							chunkHeightmap[0][k] = chunkHeightmap[0][k] * borderHeightMultiplier;
						}
					}
					
					enablePhysics = z < chunkPhysicsDepth;
					
					position = {
						outsideTerrainPosition[0] + (chunkWidth / 2) + (chunkWidth * x),
						outsideTerrainPosition[1],
						outsideTerrainPosition[2] - (chunkHeight / 2) - (chunkHeight * z) - chunksDepthOffset
					};
					
					CreateChunk(position, vector.Zero, chunkSize, chunkHeightmap, enablePhysics);
				}
			}
		}
		
		// Creating plane of chunks at west side
		
		m_Manager.SetChunksPrefix("West");
		OTT_HeightmapHelper.Rotate(terrainHeightmap, 2);
		
		if (ShouldProcessSide(0, terrainSize[2] / 2) && !contextOptions.ShouldIgnoreDirection(OTT_CardinalDirections.West))
		{
			for (int z = 0; z < chunksCount; z++)
			{
				for (int x = 0; x < chunksDepth; x++)
				{
					i = terrainHeightmapResolution - chunkResolution * (z + 1);
					j = terrainHeightmapResolution - chunkResolution * (x + 1);
					
					chunkHeightmap = OTT_HeightmapHelper.Select(terrainHeightmap, j, i, chunkResolution, chunkResolution);
					
					if (x == 0)
					{
						for (int k = 0; k < chunkResolution; k++)
						{
							chunkHeightmap[k][chunkResolution - 1] = chunkHeightmap[k][chunkResolution - 1] * borderHeightMultiplier;
						}
					}
					
					enablePhysics = x < chunkPhysicsDepth;
					
					position = {
						outsideTerrainPosition[0] - (chunkWidth / 2) - (chunkWidth * x) - chunksDepthOffset,
						outsideTerrainPosition[1],
						outsideTerrainPosition[2] + (chunkHeight / 2) + (chunkHeight * z)
					};
					
					CreateChunk(position, vector.Zero, chunkSize, chunkHeightmap, enablePhysics);
				}
			}
		}
		
		// Creating plane of chunks at east side
		
		m_Manager.SetChunksPrefix("East");
		
		if (ShouldProcessSide(terrainSize[0] - 1, terrainSize[2] / 2) && !contextOptions.ShouldIgnoreDirection(OTT_CardinalDirections.East))
		{
			for (int z = 0; z < chunksCount; z++)
			{
				for (int x = 0; x < chunksDepth; x++)
				{
					i = terrainHeightmapResolution - chunkResolution * (z + 1);
					j = chunkResolution * x;
					
					chunkHeightmap = OTT_HeightmapHelper.Select(terrainHeightmap, j, i, chunkResolution, chunkResolution);
					
					if (x == 0)
					{
						for (int k = 0; k < chunkResolution; k++)
						{
							chunkHeightmap[k][0] = chunkHeightmap[k][0] * borderHeightMultiplier;
						}
					}
					
					enablePhysics = x < chunkPhysicsDepth;
					
					position = {
						outsideTerrainPosition[0] + terrainSize[0] + (chunkWidth / 2) + (chunkWidth * x) + chunksDepthOffset,
						outsideTerrainPosition[1],
						outsideTerrainPosition[2] + (chunkHeight / 2) + (chunkHeight * z)
					};
					
					CreateChunk(position, vector.Zero, chunkSize, chunkHeightmap, enablePhysics);
				}
			}
		}
		
		// Creating plane of chunks at north-west side
		
		m_Manager.SetChunksPrefix("NorthWest");
		OTT_HeightmapHelper.FlipVertical(cachedHeightmap);
		
		if (ShouldProcessSide(terrainSize[0] / 2, terrainSize[2] - 1) && ShouldProcessSide(0, terrainSize[2] / 2) && !contextOptions.ShouldIgnoreDirection(OTT_CardinalDirections.NorthWest))
		{
			for (int z = 0; z < chunksDepth; z++)
			{
				for (int x = 0; x < chunksDepth; x++)
				{
					i = terrainHeightmapResolution - chunkResolution * (z + 1);
					j = (chunkResolution * (chunksCount - chunksDepth)) + (chunkResolution * x);
					
					chunkHeightmap = OTT_HeightmapHelper.Select(cachedHeightmap, j, i, chunkResolution, chunkResolution);
					
					if (z == 0 && x == chunksDepth - 1)
					{
						chunkHeightmap[chunkResolution - 1][chunkResolution - 1] = chunkHeightmap[chunkResolution - 1][chunkResolution - 1] * borderHeightMultiplier;
					}
					
					enablePhysics = x >= (chunksDepth - chunkPhysicsDepth) && z < chunkPhysicsDepth;
					
					position = {
						outsideTerrainPosition[0] - ((chunksDepth - 1) * chunkWidth) - (chunkWidth / 2) + (chunkWidth * x) - chunksDepthOffset,
						outsideTerrainPosition[1],
						outsideTerrainPosition[2] + terrainSize[2] + (chunkHeight / 2) + (chunkHeight * z) + chunksDepthOffset
					};
					
					CreateChunk(position, vector.Zero, chunkSize, chunkHeightmap, enablePhysics);
				}
			}
		}
		
		// Creating plane of chunks at north-east side
		
		m_Manager.SetChunksPrefix("NorthEast");
		
		if (ShouldProcessSide(terrainSize[0] / 2, terrainSize[2] - 1) && ShouldProcessSide(terrainSize[0] - 1, terrainSize[2] / 2) && !contextOptions.ShouldIgnoreDirection(OTT_CardinalDirections.NorthEast))
		{
			for (int z = 0; z < chunksDepth; z++)
			{
				for (int x = 0; x < chunksDepth; x++)
				{
					i = terrainHeightmapResolution - chunkResolution * (z + 1);
					j = chunkResolution * x;
					
					chunkHeightmap = OTT_HeightmapHelper.Select(cachedHeightmap, j, i, chunkResolution, chunkResolution);
					
					if (z == 0 && x == 0)
					{
						chunkHeightmap[chunkResolution - 1][0] = chunkHeightmap[chunkResolution - 1][0] * borderHeightMultiplier;
					}
					
					enablePhysics = x < chunkPhysicsDepth && z < chunkPhysicsDepth;
					
					position = {
						outsideTerrainPosition[0] + terrainSize[0] + (chunkWidth / 2) + (chunkWidth * x) + chunksDepthOffset,
						outsideTerrainPosition[1],
						outsideTerrainPosition[2] + terrainSize[2] + (chunkHeight / 2) + (chunkHeight * z) + chunksDepthOffset
					};
					
					CreateChunk(position, vector.Zero, chunkSize, chunkHeightmap, enablePhysics);
				}
			}
		}
		
		// Creating plane of chunks at south-west side
		
		m_Manager.SetChunksPrefix("SouthWest");
		
		if (ShouldProcessSide(terrainSize[0] / 2, 0) && ShouldProcessSide(0, terrainSize[2] / 2) && !contextOptions.ShouldIgnoreDirection(OTT_CardinalDirections.SouthWest))
		{
			for (int z = 0; z < chunksDepth; z++)
			{
				for (int x = 0; x < chunksDepth; x++)
				{
					i = chunkResolution * z;
					j = terrainHeightmapResolution - chunkResolution * (x + 1);
					
					chunkHeightmap = OTT_HeightmapHelper.Select(cachedHeightmap, j, i, chunkResolution, chunkResolution);
					
					if (z == 0 && x == 0)
					{
						chunkHeightmap[0][chunkResolution - 1] = chunkHeightmap[0][chunkResolution - 1] * borderHeightMultiplier;
					}
					
					enablePhysics = x < chunkPhysicsDepth && z < chunkPhysicsDepth;
					
					position = {
						outsideTerrainPosition[0] - (chunkWidth / 2) - (chunkWidth * x) - chunksDepthOffset,
						outsideTerrainPosition[1],
						outsideTerrainPosition[2] - (chunkHeight / 2) - (chunkHeight * z) - chunksDepthOffset
					};
					
					CreateChunk(position, vector.Zero, chunkSize, chunkHeightmap, enablePhysics);
				}
			}
		}
		
		// Creating plane of chunks at south-east side
		
		m_Manager.SetChunksPrefix("SouthEast");
		
		if (ShouldProcessSide(terrainSize[0] / 2, 0) && ShouldProcessSide(terrainSize[0] - 1, terrainSize[2] / 2) && !contextOptions.ShouldIgnoreDirection(OTT_CardinalDirections.SouthEast))
		{
			for (int z = 0; z < chunksDepth; z++)
			{
				for (int x = 0; x < chunksDepth; x++)
				{
					i = chunkResolution * z;
					j = chunkResolution * x;
					
					chunkHeightmap = OTT_HeightmapHelper.Select(cachedHeightmap, j, i, chunkResolution, chunkResolution);
					
					if (z == 0 && x == 0)
					{
						chunkHeightmap[0][0] = chunkHeightmap[0][0] * borderHeightMultiplier;
					}
					
					enablePhysics = x < chunkPhysicsDepth && z < chunkPhysicsDepth;
					
					position = {
						outsideTerrainPosition[0] + terrainSize[0] + (chunkWidth / 2) + (chunkWidth * x) + chunksDepthOffset,
						outsideTerrainPosition[1],
						outsideTerrainPosition[2] - (chunkHeight / 2) - (chunkHeight * z) - chunksDepthOffset
					};
					
					CreateChunk(position, vector.Zero, chunkSize, chunkHeightmap, enablePhysics);
				}
			}
		}
		
		return true;
	}
}

#endif