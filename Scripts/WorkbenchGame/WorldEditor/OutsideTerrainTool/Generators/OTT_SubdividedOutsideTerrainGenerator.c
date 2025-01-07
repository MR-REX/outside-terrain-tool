#ifdef WORKBENCH

class OTT_SubdividedOutsideTerrainGenerator : OTT_SimpleOutsideTerrainGenerator
{
	protected const float INTERPOLATION_CONSTANT = 0.25;
	
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
		
		array<ref array<float>> copiedDetailedTerrainHeightmap = OTT_HeightmapHelper.Resize(
			detailedTerrainHeightmap,
			detailedTerrainHeightmapResolution,
			detailedTerrainHeightmapResolution
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
		
		float nextHeight;
		
		for (int i = 0; i < detailedTerrainHeightmapResolution; i += divisionMultiplier)
		{
			for (int k = 0; k < divisionMultiplier; k++)
			{
				int index;
				
				// North
				
				detailedTerrainHeightmap[chunkResolution - 1][i + k] = terrainHeightmap[chunkResolution - 1][pointIndex];
				
				for (int m = 1; m < chunkResolution - 1; m++)
				{
					nextHeight = INTERPOLATION_CONSTANT * detailedTerrainHeightmap[chunkResolution - 1 - m][i + k] +
								 (1 - INTERPOLATION_CONSTANT) * detailedTerrainHeightmap[chunkResolution - 1 - (m - 1)][i + k];
					
					detailedTerrainHeightmap[chunkResolution - 1 - m][i + k] = nextHeight;
				}
				
				// South
				
				index = detailedTerrainHeightmapResolution - chunkResolution;
				detailedTerrainHeightmap[index][i + k] = terrainHeightmap[terrainHeightmapResolution - chunkResolution][pointIndex];
				
				for (int m = 1; m < chunkResolution - 1; m++)
				{
					nextHeight = INTERPOLATION_CONSTANT * detailedTerrainHeightmap[index + m][i + k] +
								 (1 - INTERPOLATION_CONSTANT) * detailedTerrainHeightmap[index + (m - 1)][i + k];
					
					detailedTerrainHeightmap[index + m][i + k] = nextHeight;
				}
				
				// West
				
				index = chunkResolution - 1;
				copiedDetailedTerrainHeightmap[i + k][index] = terrainHeightmap[pointIndex][index];
				
				for (int m = 1; m < chunkResolution - 1; m++)
				{
					nextHeight = INTERPOLATION_CONSTANT * copiedDetailedTerrainHeightmap[i + k][index - m] +
								 (1 - INTERPOLATION_CONSTANT) * copiedDetailedTerrainHeightmap[i + k][index - (m - 1)];
					
					copiedDetailedTerrainHeightmap[i + k][index - m] = nextHeight;
				}
				
				// East
				
				index = detailedTerrainHeightmapResolution - chunkResolution;
				copiedDetailedTerrainHeightmap[i + k][index] = terrainHeightmap[pointIndex][terrainHeightmapResolution - chunkResolution];
				
				for (int m = 1; m < chunkResolution - 1; m++)
				{
					nextHeight = INTERPOLATION_CONSTANT * copiedDetailedTerrainHeightmap[i + k][index + m] +
								 (1 - INTERPOLATION_CONSTANT) * copiedDetailedTerrainHeightmap[i + k][index + (m - 1)];
					
					copiedDetailedTerrainHeightmap[i + k][index + m] = nextHeight;
				}
			}
			
			pointIndex++;
		}
		
		// 1. Errors correction after interpolation process
		
		// 1.1. Eliminating sharp elevation changes at borders
		
		float leftHeight, rightHeight;
		
		for (int i = 1; i < detailedTerrainHeightmapResolution - 1; i += 2)
		{
			// North
			
			leftHeight = detailedTerrainHeightmap[chunkResolution - 1][i - 1];
			rightHeight = detailedTerrainHeightmap[chunkResolution - 1][i + 1];
			
			detailedTerrainHeightmap[chunkResolution - 1][i] = (leftHeight + rightHeight) / 2;
			
			// South
			
			leftHeight = detailedTerrainHeightmap[detailedTerrainHeightmapResolution - chunkResolution][i - 1];
			rightHeight = detailedTerrainHeightmap[detailedTerrainHeightmapResolution - chunkResolution][i + 1];
			
			detailedTerrainHeightmap[detailedTerrainHeightmapResolution - chunkResolution][i] = (leftHeight + rightHeight) / 2;
			
			// West
			
			leftHeight = copiedDetailedTerrainHeightmap[i - 1][chunkResolution - 1];
			rightHeight = copiedDetailedTerrainHeightmap[i + 1][chunkResolution - 1];
			
			copiedDetailedTerrainHeightmap[i][chunkResolution - 1] = (leftHeight + rightHeight) / 2;
			
			// East
			
			leftHeight = copiedDetailedTerrainHeightmap[i - 1][detailedTerrainHeightmapResolution - chunkResolution];
			rightHeight = copiedDetailedTerrainHeightmap[i + 1][detailedTerrainHeightmapResolution - chunkResolution];
			
			copiedDetailedTerrainHeightmap[i][detailedTerrainHeightmapResolution - chunkResolution] = (leftHeight + rightHeight) / 2;
		}
		
		// 1.2. Merge heights at transitions of subdivided chunks (North & South)
		
		for (int i = 0; i < chunkResolution; i++)
		{
			for (int j = chunkResolution - 1; j < detailedTerrainHeightmapResolution - 1; j += chunkResolution)
			{
				// North
				
				detailedTerrainHeightmap[i][j] = detailedTerrainHeightmap[i][j + 1];
				
				// South
				
				detailedTerrainHeightmap[detailedTerrainHeightmapResolution - 1 - i][j] = detailedTerrainHeightmap[detailedTerrainHeightmapResolution - 1 - i][j + 1];
			
				// West
				
				copiedDetailedTerrainHeightmap[j][i] = copiedDetailedTerrainHeightmap[j + 1][i];
				
				// East
				
				copiedDetailedTerrainHeightmap[j][detailedTerrainHeightmapResolution - 1 - i] = copiedDetailedTerrainHeightmap[j + 1][detailedTerrainHeightmapResolution - 1 - i];
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
		
		// Calculating subdivided chunk sizes
		
		float subdividedChunkWidth = chunkWidth / divisionMultiplier;
		float subdividedChunkHeight = chunkHeight / divisionMultiplier;
		
		// Horizontal flipping terrain heightmap for z-axis planes
		
		OTT_HeightmapHelper.FlipHorizontal(terrainHeightmap);
		OTT_HeightmapHelper.FlipHorizontal(detailedTerrainHeightmap);
		
		OTT_HeightmapHelper.FlipHorizontal(copiedDetailedTerrainHeightmap);
		
		// Creating copy of terrain heightmap for angular planes
		
		array<ref array<float>> cachedHeightmap = OTT_HeightmapHelper.Resize(terrainHeightmap, terrainHeightmapResolution, terrainHeightmapResolution);
		
		// Diagonal joins corrections
		
		for (int i = 0; i < chunkResolution; i++)
		{
			// Horizontal : North (North-West & North-East)
			
			detailedTerrainHeightmap[detailedTerrainHeightmapResolution - chunkResolution + i][0] = cachedHeightmap[terrainHeightmapResolution - chunkResolution + i][0];
			detailedTerrainHeightmap[detailedTerrainHeightmapResolution - chunkResolution + i][detailedTerrainHeightmapResolution - 1] = cachedHeightmap[terrainHeightmapResolution - chunkResolution + i][terrainHeightmapResolution - 1];
		
			// Horizontal : South (South-West & South-East)
			
			detailedTerrainHeightmap[i][0] = cachedHeightmap[i][0];
			detailedTerrainHeightmap[i][detailedTerrainHeightmapResolution - 1] = cachedHeightmap[i][terrainHeightmapResolution - 1];
			
			// Vertical : West (North-West & South-West)
			
			copiedDetailedTerrainHeightmap[detailedTerrainHeightmapResolution - 1][i] = cachedHeightmap[terrainHeightmapResolution - 1][i];
			copiedDetailedTerrainHeightmap[0][i] = cachedHeightmap[0][i];
			
			// Vertical : East (North-East & South-East)
			
			copiedDetailedTerrainHeightmap[detailedTerrainHeightmapResolution - 1][detailedTerrainHeightmapResolution - chunkResolution + i] = cachedHeightmap[terrainHeightmapResolution - 1][terrainHeightmapResolution - chunkResolution + i];
			copiedDetailedTerrainHeightmap[0][detailedTerrainHeightmapResolution - chunkResolution + i] = cachedHeightmap[0][terrainHeightmapResolution - chunkResolution + i];
		}
		
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
			// Normal chunks
			
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
			
			// Subdivided chunks
			
			vector subdividedChunkSize = {subdividedChunkWidth, 0, chunkHeight};
			
			for (int x = 0; x < chunksCount * divisionMultiplier; x++)
			{
				i = 0;
				j = chunkResolution * x;
				
				chunkHeightmap = OTT_HeightmapHelper.Select(detailedTerrainHeightmap, j, i, chunkResolution, chunkResolution);
				enablePhysics = 0 < chunkPhysicsDepth;
				
				position = {
					outsideTerrainPosition[0] + (subdividedChunkWidth / 2) + (subdividedChunkWidth * x),
					outsideTerrainPosition[1],
					outsideTerrainPosition[2] - (chunkHeight / 2) - chunksDepthOffset
				};
				
				CreateChunk(position, vector.Zero, subdividedChunkSize, chunkHeightmap, enablePhysics);
			}
		}
		
		// Creating plane of chunks at west side
		
		m_Manager.SetChunksPrefix("West");
		
		OTT_HeightmapHelper.Rotate(terrainHeightmap, 2);
		OTT_HeightmapHelper.Rotate(detailedTerrainHeightmap, 2);
		OTT_HeightmapHelper.Rotate(copiedDetailedTerrainHeightmap, 2);
		
		if (ShouldProcessSide(0, terrainSize[2] / 2) && !contextOptions.ShouldIgnoreDirection(OTT_CardinalDirections.West))
		{	
			// Normal chunks
			
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
			
			// Subdivided chunks
			
			vector subdividedChunkSize = {chunkHeight, 0, subdividedChunkWidth};
			
			for (int z = 0; z < chunksCount * divisionMultiplier; z++)
			{	
				i = detailedTerrainHeightmapResolution - chunkResolution * (z + 1);
				j = detailedTerrainHeightmapResolution - chunkResolution;
				
				chunkHeightmap = OTT_HeightmapHelper.Select(copiedDetailedTerrainHeightmap, j, i, chunkResolution, chunkResolution);
				enablePhysics = 0 < chunkPhysicsDepth;
				
				position = {
					outsideTerrainPosition[0] - (chunkWidth / 2) - chunksDepthOffset,
					outsideTerrainPosition[1],
					outsideTerrainPosition[2] + (subdividedChunkWidth / 2) + (subdividedChunkWidth * z)
				};
				
				CreateChunk(position, vector.Zero, subdividedChunkSize, chunkHeightmap, enablePhysics);
			}
		}
		
		// Creating plane of chunks at east side
		
		m_Manager.SetChunksPrefix("East");
		
		if (ShouldProcessSide(terrainSize[0] - 1, terrainSize[2] / 2) && !contextOptions.ShouldIgnoreDirection(OTT_CardinalDirections.East))
		{
			// Normal chunks
			
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
			
			// Subdivided chunks
			
			vector subdividedChunkSize = {chunkHeight, 0, subdividedChunkWidth};
			
			for (int z = 0; z < chunksCount * divisionMultiplier; z++)
			{
				i = detailedTerrainHeightmapResolution - chunkResolution * (z + 1);
				j = 0;
				
				chunkHeightmap = OTT_HeightmapHelper.Select(copiedDetailedTerrainHeightmap, j, i, chunkResolution, chunkResolution);
				enablePhysics = 0 < chunkPhysicsDepth;
				
				position = {
					outsideTerrainPosition[0] + terrainSize[0] + (chunkWidth / 2) + chunksDepthOffset,
					outsideTerrainPosition[1],
					outsideTerrainPosition[2] + (subdividedChunkWidth / 2) + (subdividedChunkWidth * z)
				};
				
				CreateChunk(position, vector.Zero, subdividedChunkSize, chunkHeightmap, enablePhysics);
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