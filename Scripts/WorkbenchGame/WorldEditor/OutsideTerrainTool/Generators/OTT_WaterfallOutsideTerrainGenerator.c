#ifdef WORKBENCH

class OTT_WaterfallOutsideTerrainGenerator : OTT_OutsideTerrainGenerator
{
	protected const float INTERPOLATION_CONSTAINT = 0.25;
			
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
		
		return 2;
	}
	
	protected int GetChunksCount()
	{
		OTT_EOutsideTerrainQuality quality = m_Options.GetQuality();
		
		switch (quality)
		{
			case OTT_EOutsideTerrainQuality.Highest:
				return 64;
			case OTT_EOutsideTerrainQuality.High:
				return 48;
			case OTT_EOutsideTerrainQuality.Medium:
				return 32;
			case OTT_EOutsideTerrainQuality.Low:
				return 16;
			case OTT_EOutsideTerrainQuality.Lowest:
				return 8;
		}
		
		return 4;
	}
	
	protected float GetChunkSize()
	{
		return m_Terrain.GetSize()[0] / GetChunksCount();
	}
	
	protected float GetPlaneSizeMultiplier()
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
	
	protected bool ShouldProcessSide(float x, float z)
	{
		OTT_OutsideTerrainContextOptions contextOptions = m_Options.GetContextOptions();
		
		if (!contextOptions.ShouldTrackOceanLevel())
			return true;
		
		return m_Terrain.GetSurfaceHeight(x, z) > m_Terrain.GetOceanHeight(x, z);
	}
	
	protected bool processChunks()
	{	
		// Getting context options
		
		OTT_OutsideTerrainContextOptions contextOptions = m_Options.GetContextOptions();
		
		// Getting chunk options
		
		OTT_OutsideTerrainChunkOptions chunkOptions = m_Manager.GetChunkOptions();
		
		// Getting chunks offset from chunk options
		
		vector chunksPositionOffset = chunkOptions.GetPositionOffset();
		
		// Getting terrain data
		
		vector terrainMins = m_Terrain.GetMins();
		vector terrainSize = m_Terrain.GetSize();
		
		// Calculating outside terrain position
		
		vector outsideTerrainPosition = {terrainMins[0], 0, terrainMins[2]};
		
		// Getting chunks properties
		
		int chunkResolution = GetChunkResolution();
		int chunksCount = GetChunksCount();
		
		// Getting chunk size
		
		float chunkSize = GetChunkSize();
		vector chunkSizes = {chunkSize, 0, chunkSize};
		
		// Should enable physics for chunks?
		
		bool shouldEnablePhysics = m_Options.GetPhysicsType() == OTT_EOutsideTerrainPhysicsType.Full;
		
		// Getting generation options
		
		float chunksDepthOffset = m_Options.GetDepthOffset();
		
		// Getting terrain heightmap
		
		int terrainHeightmapResolution = chunkResolution * chunksCount;
		
		array<ref array<float>> terrainHeightmap = m_Terrain.GetHeightmap(
			terrainHeightmapResolution,
			terrainHeightmapResolution
		);
		
		// Flipping terrain heightmap
		
		OTT_HeightmapHelper.FlipHorizontal(terrainHeightmap);
		
		// Alignment with planes for North and South sides
		
		for (int i = 0; i < terrainHeightmapResolution; i++)
		{
			// North
			
			terrainHeightmap[terrainHeightmapResolution - chunkResolution][i] = chunksPositionOffset[1];
			
			// South
			
			terrainHeightmap[chunkResolution - 1][i] = chunksPositionOffset[1];
		}
		
		// Smoothing transition for North and South sides
		
		float nextHeight;
		
		for (int i = 0; i < terrainHeightmapResolution; i++)
		{
			for (int k = 1; k < chunkResolution - 1; k++)
			{
				// North
				
				nextHeight = INTERPOLATION_CONSTAINT * terrainHeightmap[terrainHeightmapResolution - chunkResolution + k][i] +
							 (1 - INTERPOLATION_CONSTAINT) * terrainHeightmap[terrainHeightmapResolution - chunkResolution + (k - 1)][i];
				
				terrainHeightmap[terrainHeightmapResolution - chunkResolution + k][i] = nextHeight;
				
				// South
				
				nextHeight = INTERPOLATION_CONSTAINT * terrainHeightmap[chunkResolution - 1 - k][i] +
							 (1 - INTERPOLATION_CONSTAINT) * terrainHeightmap[chunkResolution - 1 - (k - 1)][i];
				
				terrainHeightmap[chunkResolution - 1 - k][i] = nextHeight;
			}
		}
		
		// Reusable variables
		
		vector chunkPosition;
		bool shouldCreate;
		
		array<ref array<float>> chunkHeightmap;
		int x, y;
		
		// Stored chunks heightmaps
		
		array<ref array<float>> storedHeightmaps[8];
		
		// Error correction: connecting chunks for North and South sides
		
		for (int j = chunkResolution - 1; j < terrainHeightmapResolution - 1; j += chunkResolution)
		{
			for (int i = 0; i < chunkResolution; i++)
			{
				terrainHeightmap[i][j] = terrainHeightmap[i][j + 1];
				terrainHeightmap[terrainHeightmapResolution - 1 - i][j] = terrainHeightmap[terrainHeightmapResolution - 1 - i][j + 1];
			}
		}
		
		// Creating chunks for North side
		
		m_Manager.SetChunksPrefix("North");
		
		shouldCreate = ShouldProcessSide(terrainSize[0] / 2, terrainSize[2] - 1) &&
					   !contextOptions.ShouldIgnoreDirection(OTT_CardinalDirections.North);
		
		if (shouldCreate)
		{
			for (int i = 0; i < chunksCount; i++)
			{
				chunkPosition = outsideTerrainPosition + {
					(chunkSize / 2) + (chunkSize * i),
					-chunksPositionOffset[1],
					terrainSize[2] + (chunkSize / 2) + chunksDepthOffset
				};
				
				x = chunkResolution * i;
				y = terrainHeightmapResolution - chunkResolution;
				
				chunkHeightmap = OTT_HeightmapHelper.Select(terrainHeightmap, x, y, chunkResolution, chunkResolution);
				
				CreateChunk(
					position: chunkPosition,
					angles: vector.Zero,
					size: chunkSizes,
					heightmap: chunkHeightmap,
					enablePhysics: shouldEnablePhysics
				);
				
				if (i == 0) storedHeightmaps[0] = chunkHeightmap;
				if (i == chunksCount - 1) storedHeightmaps[1] = chunkHeightmap;
			}
		}
		
		// Creating chunks for South side
		
		m_Manager.SetChunksPrefix("South");
		
		shouldCreate = ShouldProcessSide(terrainSize[0] / 2, 0) &&
					   !contextOptions.ShouldIgnoreDirection(OTT_CardinalDirections.South);
		
		if (shouldCreate)
		{
			for (int i = 0; i < chunksCount; i++)
			{
				chunkPosition = outsideTerrainPosition + {
					(chunkSize / 2) + (chunkSize * i),
					-chunksPositionOffset[1],
					-(chunkSize / 2) - chunksDepthOffset
				};
				
				x = chunkResolution * i;
				y = 0;
				
				chunkHeightmap = OTT_HeightmapHelper.Select(terrainHeightmap, x, y, chunkResolution, chunkResolution);
				
				CreateChunk(
					position: chunkPosition,
					angles: vector.Zero,
					size: chunkSizes,
					heightmap: chunkHeightmap,
					enablePhysics: shouldEnablePhysics
				);
				
				if (i == 0) storedHeightmaps[2] = chunkHeightmap;
				if (i == chunksCount - 1) storedHeightmaps[3] = chunkHeightmap;
			}
		}
		
		// Restoring, flipping and rotating terrain heightmap
		
		terrainHeightmap = m_Terrain.GetHeightmap(
			terrainHeightmapResolution,
			terrainHeightmapResolution
		);
		
		OTT_HeightmapHelper.FlipHorizontal(terrainHeightmap);
		OTT_HeightmapHelper.Rotate(terrainHeightmap, 2);
		
		// Alignment with planes for West and East sides
		
		for (int i = 0; i < terrainHeightmapResolution; i++)
		{
			// West
			
			terrainHeightmap[i][terrainHeightmapResolution - chunkResolution] = chunksPositionOffset[1];
			
			// East
			
			terrainHeightmap[i][chunkResolution - 1] = chunksPositionOffset[1];
		}
		
		// Smoothing transition for West and East sides
		
		for (int i = 0; i < terrainHeightmapResolution; i++)
		{
			for (int k = 1; k < chunkResolution - 1; k++)
			{
				// West
				
				nextHeight = INTERPOLATION_CONSTAINT * terrainHeightmap[i][terrainHeightmapResolution - chunkResolution + k] +
							 (1 - INTERPOLATION_CONSTAINT) * terrainHeightmap[i][terrainHeightmapResolution - chunkResolution + (k - 1)];
				
				terrainHeightmap[i][terrainHeightmapResolution - chunkResolution + k] = nextHeight;
				
				// East
				
				nextHeight = INTERPOLATION_CONSTAINT * terrainHeightmap[i][chunkResolution - 1 - k] +
							 (1 - INTERPOLATION_CONSTAINT) * terrainHeightmap[i][chunkResolution - 1 - (k - 1)];
				
				terrainHeightmap[i][chunkResolution - 1 - k] = nextHeight;
			}
		}
		
		// Error correction: connecting chunks for West and East sides
		
		for (int j = 0; j < chunkResolution; j++)
		{
			for (int i = chunkResolution - 1; i < terrainHeightmapResolution - 1; i += chunkResolution)
			{
				terrainHeightmap[i][j] = terrainHeightmap[i + 1][j];
				terrainHeightmap[i][terrainHeightmapResolution - 1 - j] = terrainHeightmap[i + 1][terrainHeightmapResolution - 1 - j];
			}
		}
		
		// Creating chunks for West side
		
		m_Manager.SetChunksPrefix("West");
		
		shouldCreate = ShouldProcessSide(0, terrainSize[2] / 2) &&
					   !contextOptions.ShouldIgnoreDirection(OTT_CardinalDirections.West);
		
		if (shouldCreate)
		{
			for (int i = 0; i < chunksCount; i++)
			{
				chunkPosition = outsideTerrainPosition + {
					-(chunkSize / 2) - chunksDepthOffset,
					-chunksPositionOffset[1],
					(chunkSize / 2) + (chunkSize * i)
				};
				
				x = terrainHeightmapResolution - chunkResolution;
				y = terrainHeightmapResolution - chunkResolution * (i + 1);
				
				chunkHeightmap = OTT_HeightmapHelper.Select(terrainHeightmap, x, y, chunkResolution, chunkResolution);
				
				CreateChunk(
					position: chunkPosition,
					angles: vector.Zero,
					size: chunkSizes,
					heightmap: chunkHeightmap,
					enablePhysics: shouldEnablePhysics
				);
				
				if (i == 0) storedHeightmaps[4] = chunkHeightmap;
				if (i == chunksCount - 1) storedHeightmaps[5] = chunkHeightmap;
			}
		}
		
		// Creating chunks for East side
		
		m_Manager.SetChunksPrefix("East");
		
		shouldCreate = ShouldProcessSide(terrainSize[0] - 1, terrainSize[2] / 2) &&
					   !contextOptions.ShouldIgnoreDirection(OTT_CardinalDirections.East);
		
		if (shouldCreate)
		{
			for (int i = 0; i < chunksCount; i++)
			{
				chunkPosition = outsideTerrainPosition + {
					terrainSize[0] + (chunkSize / 2) + chunksDepthOffset,
					-chunksPositionOffset[1],
					(chunkSize / 2) + (chunkSize * i)
				};
				
				x = 0;
				y = terrainHeightmapResolution - chunkResolution * (i + 1);
				
				chunkHeightmap = OTT_HeightmapHelper.Select(terrainHeightmap, x, y, chunkResolution, chunkResolution);
				
				CreateChunk(
					position: chunkPosition,
					angles: vector.Zero,
					size: chunkSizes,
					heightmap: chunkHeightmap,
					enablePhysics: shouldEnablePhysics
				);
				
				if (i == 0) storedHeightmaps[6] = chunkHeightmap;
				if (i == chunksCount - 1) storedHeightmaps[7] = chunkHeightmap;
			}
		}
		
		// Restoring and flipping terrain heightmap
		
		terrainHeightmap = m_Terrain.GetHeightmap(
			terrainHeightmapResolution,
			terrainHeightmapResolution
		);
		
		OTT_HeightmapHelper.FlipHorizontal(terrainHeightmap);
		OTT_HeightmapHelper.FlipVertical(terrainHeightmap);
		
		// Alignment with planes for North-West chunk
		
		for (int i = 0; i < chunkResolution; i++)
		{
			terrainHeightmap[terrainHeightmapResolution - chunkResolution + i][terrainHeightmapResolution - chunkResolution] = chunksPositionOffset[1];
			terrainHeightmap[terrainHeightmapResolution - chunkResolution][terrainHeightmapResolution - chunkResolution + i] = chunksPositionOffset[1];
		}
		
		// Smoothing transition for North-West chunk
		
		for (int i = 0; i < terrainHeightmapResolution; i++)
		{
			for (int k = 1; k < chunkResolution - 1; k++)
			{
				// From +X
				
				nextHeight = INTERPOLATION_CONSTAINT * terrainHeightmap[i][terrainHeightmapResolution - chunkResolution + k] +
							 (1 - INTERPOLATION_CONSTAINT) * terrainHeightmap[i][terrainHeightmapResolution - chunkResolution + (k - 1)];
				
				terrainHeightmap[i][terrainHeightmapResolution - chunkResolution + k] = nextHeight;
				
				// From -Z
				
				nextHeight = INTERPOLATION_CONSTAINT * terrainHeightmap[terrainHeightmapResolution - chunkResolution + k][i] +
							 (1 - INTERPOLATION_CONSTAINT) * terrainHeightmap[terrainHeightmapResolution - chunkResolution + (k - 1)][i];
				
				terrainHeightmap[terrainHeightmapResolution - chunkResolution + k][i] = nextHeight;
			}
		}
		
		// Create chunk at North-West corner
		
		m_Manager.SetChunksPrefix("NorthWest");
		
		shouldCreate = ShouldProcessSide(terrainSize[0] / 2, terrainSize[2] - 1) &&
					   ShouldProcessSide(0, terrainSize[2] / 2) &&
					   !contextOptions.ShouldIgnoreDirection(OTT_CardinalDirections.NorthWest);
		
		if (shouldCreate)
		{
			chunkPosition = outsideTerrainPosition + {
				-(chunkSize / 2) - chunksDepthOffset,
				-chunksPositionOffset[1],
				terrainSize[2] + (chunkSize / 2) + chunksDepthOffset
			};
			
			x = terrainHeightmapResolution - chunkResolution;
			y = terrainHeightmapResolution - chunkResolution;
			
			chunkHeightmap = OTT_HeightmapHelper.Select(terrainHeightmap, x, y, chunkResolution, chunkResolution);
			
			// Join with first North side chunk
			
			if (storedHeightmaps[0] != null)
			{
				for (int i = 0; i < chunkResolution; i++)
				{
					chunkHeightmap[i][chunkResolution - 1] = storedHeightmaps[0][i][0];
				}
			}
			
			// Join with last West side chunk
			
			if (storedHeightmaps[5] != null)
			{
				for (int i = 0; i < chunkResolution; i++)
				{
					chunkHeightmap[chunkResolution - 1][i] = storedHeightmaps[5][0][i];
				}
			}
			
			CreateChunk(
				position: chunkPosition,
				angles: vector.Zero,
				size: chunkSizes,
				heightmap: chunkHeightmap,
				enablePhysics: shouldEnablePhysics
			);
		}
		
		// Alignment with planes for North-East chunk
		
		for (int i = 0; i < chunkResolution; i++)
		{
			terrainHeightmap[terrainHeightmapResolution - chunkResolution][i] = chunksPositionOffset[1];
			terrainHeightmap[terrainHeightmapResolution - 1 - i][chunkResolution - 1] = chunksPositionOffset[1];
		}
		
		// Smoothing transition for North-East chunk
		
		for (int i = 0; i < terrainHeightmapResolution; i++)
		{
			for (int k = 1; k < chunkResolution - 1; k++)
			{
				// From -X
				
				nextHeight = INTERPOLATION_CONSTAINT * terrainHeightmap[i][chunkResolution - 1 - k] +
							 (1 - INTERPOLATION_CONSTAINT) * terrainHeightmap[i][chunkResolution - 1 - (k - 1)];
				
				terrainHeightmap[i][chunkResolution - 1 - k] = nextHeight;
				
				// From -Z
				
				nextHeight = INTERPOLATION_CONSTAINT * terrainHeightmap[terrainHeightmapResolution - chunkResolution + k][i] +
							 (1 - INTERPOLATION_CONSTAINT) * terrainHeightmap[terrainHeightmapResolution - chunkResolution + (k - 1)][i];
				
				terrainHeightmap[terrainHeightmapResolution - chunkResolution + k][i] = nextHeight;
			}
		}
		
		// Create chunk at North-East corner
		
		m_Manager.SetChunksPrefix("NorthEast");
		
		shouldCreate = ShouldProcessSide(terrainSize[0] / 2, terrainSize[2] - 1) &&
					   ShouldProcessSide(terrainSize[0] - 1, terrainSize[2] / 2) &&
					   !contextOptions.ShouldIgnoreDirection(OTT_CardinalDirections.NorthEast);
		
		if (shouldCreate)
		{
			chunkPosition = outsideTerrainPosition + {
				terrainSize[0] + (chunkSize / 2) + chunksDepthOffset,
				-chunksPositionOffset[1],
				terrainSize[2] + (chunkSize / 2) + chunksDepthOffset
			};
			
			x = 0;
			y = terrainHeightmapResolution - chunkResolution;
			
			chunkHeightmap = OTT_HeightmapHelper.Select(terrainHeightmap, x, y, chunkResolution, chunkResolution);
			
			// Join with last North side chunk
			
			if (storedHeightmaps[1] != null)
			{
				for (int i = 0; i < chunkResolution; i++)
				{
					chunkHeightmap[i][0] = storedHeightmaps[1][i][chunkResolution - 1];
				}
			}
			
			// Join with last East side chunk
			
			if (storedHeightmaps[7] != null)
			{
				for (int i = 0; i < chunkResolution; i++)
				{
					chunkHeightmap[chunkResolution - 1][i] = storedHeightmaps[7][0][i];
				}
			}
			
			CreateChunk(
				position: chunkPosition,
				angles: vector.Zero,
				size: chunkSizes,
				heightmap: chunkHeightmap,
				enablePhysics: shouldEnablePhysics
			);
		}
		
		// Alignment with planes for South-West chunk
		
		for (int i = 0; i < chunkResolution; i++)
		{
			terrainHeightmap[i][terrainHeightmapResolution - chunkResolution] = chunksPositionOffset[1];
			terrainHeightmap[chunkResolution - 1][terrainHeightmapResolution - chunkResolution + i] = chunksPositionOffset[1];
		}
		
		// Smoothing transition for South-West chunk
		
		for (int i = 0; i < terrainHeightmapResolution; i++)
		{
			for (int k = 1; k < chunkResolution - 1; k++)
			{
				// From +X
				
				nextHeight = INTERPOLATION_CONSTAINT * terrainHeightmap[i][terrainHeightmapResolution - chunkResolution + k] +
							 (1 - INTERPOLATION_CONSTAINT) * terrainHeightmap[i][terrainHeightmapResolution - chunkResolution + (k - 1)];
				
				terrainHeightmap[i][terrainHeightmapResolution - chunkResolution + k] = nextHeight;
				
				// From +Z
				
				nextHeight = INTERPOLATION_CONSTAINT * terrainHeightmap[chunkResolution - 1 - k][i] +
							 (1 - INTERPOLATION_CONSTAINT) * terrainHeightmap[chunkResolution - 1 - (k - 1)][i];
				
				terrainHeightmap[chunkResolution - 1 - k][i] = nextHeight;
			}
		}
		
		// Create chunk at South-West corner
		
		m_Manager.SetChunksPrefix("SouthWest");
		
		shouldCreate = ShouldProcessSide(terrainSize[0] / 2, 0) &&
					   ShouldProcessSide(0, terrainSize[2] / 2) &&
					   !contextOptions.ShouldIgnoreDirection(OTT_CardinalDirections.SouthWest);
		
		if (shouldCreate)
		{
			chunkPosition = outsideTerrainPosition + {
				-(chunkSize / 2) - chunksDepthOffset,
				-chunksPositionOffset[1],
				-(chunkSize / 2) - chunksDepthOffset
			};
			
			x = terrainHeightmapResolution - chunkResolution;
			y = 0;
			
			chunkHeightmap = OTT_HeightmapHelper.Select(terrainHeightmap, x, y, chunkResolution, chunkResolution);
			
			// Join with first West side chunk
			
			if (storedHeightmaps[4] != null)
			{
				for (int i = 0; i < chunkResolution; i++)
				{
					chunkHeightmap[0][i] = storedHeightmaps[4][chunkResolution - 1][i];
				}
			}
			
			// Join with first South side chunk
			
			if (storedHeightmaps[2] != null)
			{
				for (int i = 0; i < chunkResolution; i++)
				{
					chunkHeightmap[i][chunkResolution - 1] = storedHeightmaps[2][i][0];
				}
			}
			
			CreateChunk(
				position: chunkPosition,
				angles: vector.Zero,
				size: chunkSizes,
				heightmap: chunkHeightmap,
				enablePhysics: shouldEnablePhysics
			);
		}
		
		// Alignment with planes for South-East chunk
		
		for (int i = 0; i < chunkResolution; i++)
		{
			terrainHeightmap[chunkResolution - 1][i] = chunksPositionOffset[1];
			terrainHeightmap[i][chunkResolution - 1] = chunksPositionOffset[1];
		}
		
		// Smoothing transition for South-East chunk
		
		for (int i = 0; i < terrainHeightmapResolution; i++)
		{
			for (int k = 1; k < chunkResolution - 1; k++)
			{
				// From -X
				
				nextHeight = INTERPOLATION_CONSTAINT * terrainHeightmap[i][chunkResolution - 1 - k] +
							 (1 - INTERPOLATION_CONSTAINT) * terrainHeightmap[i][chunkResolution - 1 - (k - 1)];
				
				terrainHeightmap[i][chunkResolution - 1 - k] = nextHeight;
				
				// From +Z
				
				nextHeight = INTERPOLATION_CONSTAINT * terrainHeightmap[chunkResolution - 1 - k][i] +
							 (1 - INTERPOLATION_CONSTAINT) * terrainHeightmap[chunkResolution - 1 - (k - 1)][i];
				
				terrainHeightmap[chunkResolution - 1 - k][i] = nextHeight;
			}
		}
		
		// Create chunk at South-East corner
		
		m_Manager.SetChunksPrefix("SouthEast");
		
		shouldCreate = ShouldProcessSide(terrainSize[0] / 2, 0) &&
					   ShouldProcessSide(terrainSize[0] - 1, terrainSize[2] / 2) &&
					   !contextOptions.ShouldIgnoreDirection(OTT_CardinalDirections.SouthEast);
		
		if (shouldCreate)
		{
			chunkPosition = outsideTerrainPosition + {
				terrainSize[0] + (chunkSize / 2) + chunksDepthOffset,
				-chunksPositionOffset[1],
				-(chunkSize / 2) + chunksDepthOffset
			};
			
			x = 0;
			y = 0;
			
			chunkHeightmap = OTT_HeightmapHelper.Select(terrainHeightmap, x, y, chunkResolution, chunkResolution);
			
			// Join with first East side chunk
			
			if (storedHeightmaps[6] != null)
			{
				for (int i = 0; i < chunkResolution; i++)
				{
					chunkHeightmap[0][i] = storedHeightmaps[6][chunkResolution - 1][i];
				}
			}
			
			// Join with last South side chunk
			
			if (storedHeightmaps[3] != null)
			{
				for (int i = 0; i < chunkResolution; i++)
				{
					chunkHeightmap[i][0] = storedHeightmaps[3][i][chunkResolution - 1];
				}
			}
			
			CreateChunk(
				position: chunkPosition,
				angles: vector.Zero,
				size: chunkSizes,
				heightmap: chunkHeightmap,
				enablePhysics: shouldEnablePhysics
			);
		}
		
		return true;
	}
	
	protected bool processPlanes()
	{
		// Getting context options
		
		OTT_OutsideTerrainContextOptions contextOptions = m_Options.GetContextOptions();
		
		// Getting terrain data
		
		vector terrainMins = m_Terrain.GetMins();
		vector terrainSize = m_Terrain.GetSize();
		
		// Calculating outside terrain position
		
		vector outsideTerrainPosition = {terrainMins[0], 0, terrainMins[2]};
		
		// Calculating plane sizes
		
		float planeSizeMultiplier = GetPlaneSizeMultiplier();
		
		float planeLength = terrainSize[0];
		float planeDepth = planeLength * planeSizeMultiplier;
		
		// Getting chunk size
		
		float chunkSize = GetChunkSize();
		
		// Should enable physics for planes?
		
		bool shouldEnablePhysics = m_Options.GetPhysicsType() == OTT_EOutsideTerrainPhysicsType.Full;
		
		// Getting generation options
		
		float planeDepthOffset = m_Options.GetDepthOffset();
		
		// Reusable variables
		
		vector planePosition;
		bool shouldCreate;
		
		// Generating flat heightmap
		
		array<ref array<float>> flatHeightmap = {{0, 0}, {0, 0}};
		
		// Create plane at North
		
		m_Manager.SetChunksPrefix("North");
		
		shouldCreate = ShouldProcessSide(terrainSize[0] / 2, terrainSize[2] - 1) &&
					   !contextOptions.ShouldIgnoreDirection(OTT_CardinalDirections.North);
		
		if (shouldCreate)
		{
			planePosition = outsideTerrainPosition + {
				terrainSize[0] / 2,
				0,
				terrainSize[2] + planeDepth / 2 + chunkSize + planeDepthOffset
			};
			
			CreateChunk(
				position: planePosition,
				angles: vector.Zero,
				size: {planeLength + chunkSize * 2, 0, planeDepth},
				heightmap: flatHeightmap,
				enablePhysics: shouldEnablePhysics
			);
		}
		
		// Create plane at South
		
		m_Manager.SetChunksPrefix("South");
		
		shouldCreate = ShouldProcessSide(terrainSize[0] / 2, 0) &&
					   !contextOptions.ShouldIgnoreDirection(OTT_CardinalDirections.South);
		
		if (shouldCreate)
		{
			planePosition = outsideTerrainPosition + {
				terrainSize[0] / 2,
				0,
				-(planeDepth / 2) - chunkSize - planeDepthOffset
			};
			
			CreateChunk(
				position: planePosition,
				angles: vector.Zero,
				size: {planeLength + chunkSize * 2, 0, planeDepth},
				heightmap: flatHeightmap,
				enablePhysics: shouldEnablePhysics
			);
		}
		
		// Create plane at West
		
		m_Manager.SetChunksPrefix("West");
		
		shouldCreate = ShouldProcessSide(0, terrainSize[2] / 2) &&
					   !contextOptions.ShouldIgnoreDirection(OTT_CardinalDirections.West);
		
		if (shouldCreate)
		{
			planePosition = outsideTerrainPosition + {
				-(planeDepth / 2) - chunkSize - planeDepthOffset,
				0,
				terrainSize[2] / 2
			};
			
			CreateChunk(
				position: planePosition,
				angles: vector.Zero,
				size: {planeDepth, 0, planeLength + chunkSize * 2},
				heightmap: flatHeightmap,
				enablePhysics: shouldEnablePhysics
			);
		}
		
		// Create plane at East
		
		m_Manager.SetChunksPrefix("East");
		
		shouldCreate = ShouldProcessSide(terrainSize[0] - 1, terrainSize[2] / 2) &&
					   !contextOptions.ShouldIgnoreDirection(OTT_CardinalDirections.East);
		
		if (shouldCreate)
		{
			planePosition = outsideTerrainPosition + {
				terrainSize[0] + planeDepth / 2 + chunkSize + planeDepthOffset,
				0,
				terrainSize[2] / 2
			};
			
			CreateChunk(
				position: planePosition,
				angles: vector.Zero,
				size: {planeDepth, 0, planeLength + chunkSize * 2},
				heightmap: flatHeightmap,
				enablePhysics: shouldEnablePhysics
			);
		}
		
		// Create plane at North-West
		
		m_Manager.SetChunksPrefix("NorthWest");
		
		shouldCreate = ShouldProcessSide(terrainSize[0] / 2, terrainSize[2] - 1) &&
					   ShouldProcessSide(0, terrainSize[2] / 2) &&
					   !contextOptions.ShouldIgnoreDirection(OTT_CardinalDirections.NorthWest);
		
		if (shouldCreate)
		{
			planePosition = outsideTerrainPosition + {
				-(planeDepth / 2) - chunkSize - planeDepthOffset,
				0,
				terrainSize[2] + planeDepth / 2 + chunkSize + planeDepthOffset
			};
			
			CreateChunk(
				position: planePosition,
				angles: vector.Zero,
				size: {planeDepth, 0, planeDepth},
				heightmap: flatHeightmap,
				enablePhysics: shouldEnablePhysics
			);
		}
		
		// Create plane at North-East
		
		m_Manager.SetChunksPrefix("NorthEast");
		
		shouldCreate = ShouldProcessSide(terrainSize[0] / 2, terrainSize[2] - 1) &&
					   ShouldProcessSide(terrainSize[0] - 1, terrainSize[2] / 2) &&
					   !contextOptions.ShouldIgnoreDirection(OTT_CardinalDirections.NorthEast);
		
		if (shouldCreate)
		{
			planePosition = outsideTerrainPosition + {
				terrainSize[0] + planeDepth / 2 + chunkSize + planeDepthOffset,
				0,
				terrainSize[2] + planeDepth / 2 + chunkSize + planeDepthOffset
			};
			
			CreateChunk(
				position: planePosition,
				angles: vector.Zero,
				size: {planeDepth, 0, planeDepth},
				heightmap: flatHeightmap,
				enablePhysics: shouldEnablePhysics
			);
		}
		
		// Create plane at South-West
		
		m_Manager.SetChunksPrefix("SouthWest");
		
		shouldCreate = ShouldProcessSide(terrainSize[0] / 2, 0) &&
					   ShouldProcessSide(0, terrainSize[2] / 2) &&
					   !contextOptions.ShouldIgnoreDirection(OTT_CardinalDirections.SouthWest);
		
		if (shouldCreate)
		{
			planePosition = outsideTerrainPosition + {
				-(planeDepth / 2) - chunkSize - planeDepthOffset,
				0,
				-(planeDepth / 2) - chunkSize - planeDepthOffset
			};
			
			CreateChunk(
				position: planePosition,
				angles: vector.Zero,
				size: {planeDepth, 0, planeDepth},
				heightmap: flatHeightmap,
				enablePhysics: shouldEnablePhysics
			);
		}
		
		// Create plane at South-East
		
		m_Manager.SetChunksPrefix("SouthEast");
		
		shouldCreate = ShouldProcessSide(terrainSize[0] / 2, 0) &&
					   ShouldProcessSide(terrainSize[0] - 1, terrainSize[2] / 2) &&
					   !contextOptions.ShouldIgnoreDirection(OTT_CardinalDirections.SouthEast);
		
		if (shouldCreate)
		{
			planePosition = outsideTerrainPosition + {
				terrainSize[0] + planeDepth / 2 + chunkSize + planeDepthOffset,
				0,
				-(planeDepth / 2) - chunkSize - planeDepthOffset
			};
			
			CreateChunk(
				position: planePosition,
				angles: vector.Zero,
				size: {planeDepth, 0, planeDepth},
				heightmap: flatHeightmap,
				enablePhysics: shouldEnablePhysics
			);
		}
		
		return true;
	}
	
	override protected bool Process()
	{
		return processChunks() && processPlanes();
	}
}