#ifdef WORKBENCH

class OTT_DefaultOutsideTerrainGenerator : OTT_OutsideTerrainGenerator
{	
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
				return 0.15;
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
		
		// Getting terrain position and size
		
		vector terrainMins = m_Terrain.GetMins();
		vector terrainSize = m_Terrain.GetSize();
		
		// Calculate terrain heightmap
		
		int terrainHeightmapWidth = m_Terrain.GetHeightmapWidth();
		int terrainHeightmapHeight = m_Terrain.GetHeightmapHeight();
		
		// Getting terrain tiles count
		
		int tilesPerWidth = m_Terrain.GetTilesPerWidth();
		int tilesPerHeight = m_Terrain.GetTilesPerHeight();
		
		// Getting terrain tile properties
		
		int blocksPerTileWidth = m_Options.GetBlocksPerTileWidth();
		int blocksPerTileHeight = m_Options.GetBlocksPerTileHeight();
		
		// Calculate chunks count
		
		int chunksPerWidth = blocksPerTileWidth * tilesPerWidth;
		int chunksPerHeight = blocksPerTileHeight * tilesPerHeight;
		
		if (chunksPerWidth < 1 || chunksPerHeight < 1)
			return false;
		
		// Calculate chunks heightmap resolution
		
		int chunkHeightmapWidth = terrainHeightmapWidth / chunksPerWidth + 1;
		int chunkHeightmapHeight = terrainHeightmapHeight / chunksPerHeight + 1;
		
		// Calculate terrain heightmap
		
		array<ref array<float>> terrainHeightmap = GetTerrainHeightmap(
			width: terrainHeightmapWidth,
			height: terrainHeightmapHeight,
			useUnitScale: true
		);
		
		// Calculate chunks depth
		
		float chunksDepthMultiplier = GetChunksDepthMultiplier();
		
		int chunksDepthX = Math.Ceil(chunksPerWidth * chunksDepthMultiplier);
		int chunksDepthZ = Math.Ceil(chunksPerHeight * chunksDepthMultiplier);
		
		// Calculate depth of chunks with physics
		
		float chunkPhysicsDepthMultiplier = GetChunkPhysicsDepthMultiplier();
		
		int chunkPhysicsDepthX = Math.Ceil(chunksPerWidth * chunkPhysicsDepthMultiplier);
		int chunkPhysicsDepthZ = Math.Ceil(chunksPerHeight * chunkPhysicsDepthMultiplier);
		
		// Getting outside terrain border properties
		
		float chunksDepthOffset = m_Options.GetDepthOffset();
		float borderHeightMultiplier = m_Options.GetBorderHeightMultiplier();
		
		// Calculating outside terrain position
		
		vector outsideTerrainPosition = {terrainMins[0], 0, terrainMins[2]};
		
		// Calculating chunk sizes
		
		float chunkWidth = terrainSize[0] / chunksPerWidth;
		float chunkHeight = terrainSize[2] / chunksPerHeight;
		
		vector chunkSize = {chunkWidth, 0, chunkHeight};
		
		// Horizontal flipping terrain heightmap for z-axis planes
		
		OTT_HeightmapHelper.FlipHorizontal(terrainHeightmap);
		
		// Creating copy of terrain heightmap for angular planes
		
		array<ref array<float>> cachedHeightmap = OTT_HeightmapHelper.Resize(terrainHeightmap, terrainHeightmapWidth, terrainHeightmapHeight);
		
		// Initializing temporary variables
		
		array<ref array<float>> chunkHeightmap;
		
		int i, j;
		
		vector position;
		bool enablePhysics;
		
		// Creating plane of chunks at north side
		
		m_Manager.SetChunksPrefix("North");
		
		if (ShouldProcessSide(terrainSize[0] / 2, terrainSize[2] - 1) && !contextOptions.ShouldIgnoreDirection(OTT_CardinalDirections.North))
		{
			for (int z = 0; z < chunksDepthZ; z++)
			{
				for (int x = 0; x < chunksPerWidth; x++)
				{
					i = terrainHeightmapHeight - chunkHeightmapHeight * (z + 1) + z;
					j = chunkHeightmapWidth * x - x;
					
					chunkHeightmap = OTT_HeightmapHelper.Select(terrainHeightmap, j, i, chunkHeightmapWidth, chunkHeightmapHeight);
					
					if (z == 0)
					{
						for (int k = 0; k < chunkHeightmapWidth; k++)
						{
							chunkHeightmap[chunkHeightmapHeight - 1][k] = chunkHeightmap[chunkHeightmapHeight - 1][k] * borderHeightMultiplier;
						}
					}
					
					enablePhysics = z < chunkPhysicsDepthZ;
					
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
			for (int z = 0; z < chunksDepthZ; z++)
			{
				for (int x = 0; x < chunksPerWidth; x++)
				{
					i = chunkHeightmapHeight * z - z;
					j = chunkHeightmapWidth * x - x;
					
					chunkHeightmap = OTT_HeightmapHelper.Select(terrainHeightmap, j, i, chunkHeightmapWidth, chunkHeightmapHeight);
					
					if (z == 0)
					{
						for (int k = 0; k < chunkHeightmapWidth; k++)
						{
							chunkHeightmap[0][k] = chunkHeightmap[0][k] * borderHeightMultiplier;
						}
					}
					
					enablePhysics = z < chunkPhysicsDepthZ;
					
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
			for (int z = 0; z < chunksPerHeight; z++)
			{
				for (int x = 0; x < chunksDepthX; x++)
				{
					i = terrainHeightmapHeight - chunkHeightmapHeight * (z + 1) + z;
					j = terrainHeightmapWidth - chunkHeightmapWidth * (x + 1) + x;
					
					chunkHeightmap = OTT_HeightmapHelper.Select(terrainHeightmap, j, i, chunkHeightmapWidth, chunkHeightmapHeight);
					
					if (x == 0)
					{
						for (int k = 0; k < chunkHeightmapHeight; k++)
						{
							chunkHeightmap[k][chunkHeightmapWidth - 1] = chunkHeightmap[k][chunkHeightmapWidth - 1] * borderHeightMultiplier;
						}
					}
					
					enablePhysics = x < chunkPhysicsDepthX;
					
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
			for (int z = 0; z < chunksPerHeight; z++)
			{
				for (int x = 0; x < chunksDepthX; x++)
				{
					i = terrainHeightmapHeight - chunkHeightmapHeight * (z + 1) + z;
					j = chunkHeightmapWidth * x - x;
					
					chunkHeightmap = OTT_HeightmapHelper.Select(terrainHeightmap, j, i, chunkHeightmapWidth, chunkHeightmapHeight);
					
					if (x == 0)
					{
						for (int k = 0; k < chunkHeightmapHeight; k++)
						{
							chunkHeightmap[k][0] = chunkHeightmap[k][0] * borderHeightMultiplier;
						}
					}
					
					enablePhysics = x < chunkPhysicsDepthX;
					
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
			for (int z = 0; z < chunksDepthZ; z++)
			{
				for (int x = 0; x < chunksDepthX; x++)
				{
					i = terrainHeightmapHeight - chunkHeightmapHeight * (z + 1) + z;
					j = terrainHeightmapWidth - chunkHeightmapWidth * (x + 1) + x;
					
					chunkHeightmap = OTT_HeightmapHelper.Select(cachedHeightmap, j, i, chunkHeightmapWidth, chunkHeightmapHeight);
					
					if (z == 0 && x == chunksDepthX - 1)
					{
						chunkHeightmap[chunkHeightmapHeight - 1][chunkHeightmapWidth - 1] = chunkHeightmap[chunkHeightmapHeight - 1][chunkHeightmapWidth - 1] * borderHeightMultiplier;
					}
					
					enablePhysics = x >= (chunksDepthX - chunkPhysicsDepthX) && z < chunkPhysicsDepthZ;
					
					position = {
						outsideTerrainPosition[0] - (chunkWidth / 2) - (chunkWidth * x) - chunksDepthOffset,
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
			for (int z = 0; z < chunksDepthZ; z++)
			{
				for (int x = 0; x < chunksDepthX; x++)
				{
					i = terrainHeightmapHeight - chunkHeightmapHeight * (z + 1) + z;
					j = chunkHeightmapWidth * x - x;
					
					chunkHeightmap = OTT_HeightmapHelper.Select(cachedHeightmap, j, i, chunkHeightmapWidth, chunkHeightmapHeight);
					
					if (z == 0 && x == 0)
					{
						chunkHeightmap[chunkHeightmapHeight - 1][0] = chunkHeightmap[chunkHeightmapHeight - 1][0] * borderHeightMultiplier;
					}
					
					enablePhysics = x < chunkPhysicsDepthX && z < chunkPhysicsDepthZ;
					
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
			for (int z = 0; z < chunksDepthZ; z++)
			{
				for (int x = 0; x < chunksDepthX; x++)
				{
					i = chunkHeightmapHeight * z - z;
					j = terrainHeightmapWidth - chunkHeightmapWidth * (x + 1) + x;
					
					chunkHeightmap = OTT_HeightmapHelper.Select(cachedHeightmap, j, i, chunkHeightmapWidth, chunkHeightmapHeight);
					
					if (z == 0 && x == 0)
					{
						chunkHeightmap[0][chunkHeightmapWidth - 1] = chunkHeightmap[0][chunkHeightmapWidth - 1] * borderHeightMultiplier;
					}
					
					enablePhysics = x < chunkPhysicsDepthX && z < chunkPhysicsDepthZ;
					
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
			for (int z = 0; z < chunksDepthZ; z++)
			{
				for (int x = 0; x < chunksDepthX; x++)
				{
					i = chunkHeightmapHeight * z - z;
					j = chunkHeightmapWidth * x - x;
					
					chunkHeightmap = OTT_HeightmapHelper.Select(cachedHeightmap, j, i, chunkHeightmapWidth, chunkHeightmapHeight);
					
					if (z == 0 && x == 0)
					{
						chunkHeightmap[0][0] = chunkHeightmap[0][0] * borderHeightMultiplier;
					}
					
					enablePhysics = x < chunkPhysicsDepthX && z < chunkPhysicsDepthZ;
					
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