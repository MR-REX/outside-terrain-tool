#ifdef WORKBENCH

class OTT_SubdividedOutsideTerrainGenerator : OTT_SimpleOutsideTerrainGenerator
{
	protected int GetDivisionMultiplier()
	{
		OTT_EOutsideTerrainQuality quality = m_Options.GetQuality();
		
		if (quality == OTT_EOutsideTerrainQuality.Highest)
			return 2;
		
		return 2;
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
		
		// Getting subdivide multiplier
		
		int divisionMultiplier = GetDivisionMultiplier();
		
		// Getting very detailed terrain heightmap for subdivided chunks
		
		int subdividedChunkResolution = chunkResolution * divisionMultiplier;
		int detailedTerrainHeightmapResolution = subdividedChunkResolution * chunksCount;
		
		array<ref array<float>> detailedTerrainHeightmap = GetTerrainHeightmap(
			width: detailedTerrainHeightmapResolution,
			height: detailedTerrainHeightmapResolution
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
		
		// Joining chunks border heights on detailed terrain heightmap
		
		for (int i = 0; i < detailedTerrainHeightmapResolution; i++)
		{
			for (int j = chunkResolution - 1; j <= detailedTerrainHeightmapResolution - chunkResolution - 1; j += chunkResolution)
			{
				height = Math.Min(detailedTerrainHeightmap[i][j], detailedTerrainHeightmap[i][j + 1]);
				
				detailedTerrainHeightmap[i][j] = height;
				detailedTerrainHeightmap[i][j + 1] = height;
				
				height = Math.Min(detailedTerrainHeightmap[j][i], detailedTerrainHeightmap[j + 1][i]);
				
				detailedTerrainHeightmap[j][i] = height;
				detailedTerrainHeightmap[j + 1][i] = height;
			}
		}
		
		// Joining normal and detailed terrain heightmaps
		
		int pointIndex;
		
		float interpolationConstaint = 0.25;
		float nextHeight;
		
		for (int i = 0; i < detailedTerrainHeightmapResolution; i += divisionMultiplier)
		{
			for (int k = 0; k < divisionMultiplier; k++)
			{	
				detailedTerrainHeightmap[chunkResolution - 1][i + k] = terrainHeightmap[chunkResolution - 1][pointIndex];
				
				for (int m = 1; m < chunkResolution - 1; m++)
				{
					nextHeight = interpolationConstaint * detailedTerrainHeightmap[chunkResolution - 1 - m][i + k] +
								 (1 - interpolationConstaint) * detailedTerrainHeightmap[chunkResolution - 1 - (m - 1)][i + k];
					
					detailedTerrainHeightmap[chunkResolution - 1 - m][i + k] = nextHeight;
				}
			}
			
			pointIndex++;
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
		
		// Calculating subdivided chunk sizes
		
		float subdividedChunkWidth = chunkWidth / divisionMultiplier;
		float subdividedChunkHeight = chunkHeight / divisionMultiplier;
		
		// Horizontal flipping terrain heightmap for z-axis planes
		
		OTT_HeightmapHelper.FlipHorizontal(terrainHeightmap);
		OTT_HeightmapHelper.FlipHorizontal(detailedTerrainHeightmap);
		
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
			// Normal chunks
			
			for (int z = 1; z < chunksDepth; z++)
			{
				for (int x = 0; x < chunksCount; x++)
				{
					i = terrainHeightmapResolution - chunkResolution * (z + 1);
					j = chunkResolution * x;
					
					chunkHeightmap = OTT_HeightmapHelper.Select(terrainHeightmap, j, i, chunkResolution, chunkResolution);
					enablePhysics = z < chunkPhysicsDepth;
					
					position = {
						outsideTerrainPosition[0] + (chunkWidth / 2) + (chunkWidth * x),
						outsideTerrainPosition[1],
						outsideTerrainPosition[2] + terrainSize[2] + (chunkHeight / 2) + (chunkHeight * z) + chunksDepthOffset
					};
					
					CreateChunk(position, vector.Zero, chunkSize, chunkHeightmap, enablePhysics);
				}
			}
			
			// Subdivided chunks
			
			vector subdividedChunkSize = {subdividedChunkWidth, 0, chunkHeight};
			
			for (int x = 0; x < chunksCount * divisionMultiplier; x++)
			{
				i = detailedTerrainHeightmapResolution - chunkResolution;
				j = chunkResolution * x;
				
				chunkHeightmap = OTT_HeightmapHelper.Select(detailedTerrainHeightmap, j, i, chunkResolution, chunkResolution);
				enablePhysics = 0 < chunkPhysicsDepth;
				
				position = {
					outsideTerrainPosition[0] + (subdividedChunkWidth / 2) + (subdividedChunkWidth * x),
					outsideTerrainPosition[1],
					outsideTerrainPosition[2] + terrainSize[2] + (chunkHeight / 2) + chunksDepthOffset
				};
				
				CreateChunk(position, vector.Zero, subdividedChunkSize, chunkHeightmap, enablePhysics);
			}
		}		
		
		// Creating plane of chunks at south side
		
		m_Manager.SetChunksPrefix("South");
		
		if (ShouldProcessSide(terrainSize[0] / 2, 0) && !contextOptions.ShouldIgnoreDirection(OTT_CardinalDirections.South))
		{
			for (int z = 1; z < chunksDepth; z++)
			{
				for (int x = 0; x < chunksCount; x++)
				{
					i = chunkResolution * z;
					j = chunkResolution * x;
					
					chunkHeightmap = OTT_HeightmapHelper.Select(terrainHeightmap, j, i, chunkResolution, chunkResolution);
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
				for (int x = 1; x < chunksDepth; x++)
				{
					i = terrainHeightmapResolution - chunkResolution * (z + 1);
					j = terrainHeightmapResolution - chunkResolution * (x + 1);
					
					chunkHeightmap = OTT_HeightmapHelper.Select(terrainHeightmap, j, i, chunkResolution, chunkResolution);
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
				for (int x = 1; x < chunksDepth; x++)
				{
					i = terrainHeightmapResolution - chunkResolution * (z + 1);
					j = chunkResolution * x;
					
					chunkHeightmap = OTT_HeightmapHelper.Select(terrainHeightmap, j, i, chunkResolution, chunkResolution);
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