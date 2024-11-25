#ifdef WORKBENCH

class OTT_FlatOutsideTerrainGenerator : OTT_OutsideTerrainGenerator
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
			
			terrainHeightmap[terrainHeightmapResolution - chunkResolution][i] = 0;
			
			// South
			
			terrainHeightmap[chunkResolution - 1][i] = 0;
		}
		
		// Smoothing transition for North and South sides
		
		float interpolationConstaint = 0.25;
		float nextHeight;
		
		for (int i = 0; i < terrainHeightmapResolution; i++)
		{
			for (int k = 1; k < chunkResolution - 1; k++)
			{
				// North
				
				nextHeight = interpolationConstaint * terrainHeightmap[terrainHeightmapResolution - chunkResolution + k][i] +
							 (1 - interpolationConstaint) * terrainHeightmap[terrainHeightmapResolution - chunkResolution + (k - 1)][i];
				
				terrainHeightmap[terrainHeightmapResolution - chunkResolution + k][i] = nextHeight;
				
				// South
				
				nextHeight = interpolationConstaint * terrainHeightmap[chunkResolution - 1 - k][i] +
							 (1 - interpolationConstaint) * terrainHeightmap[chunkResolution - 1 - (k - 1)][i];
				
				terrainHeightmap[chunkResolution - 1 - k][i] = nextHeight;
			}
		}
		
		// Reusable variables
		
		vector chunkPosition;
		bool shouldCreate;
		
		array<ref array<float>> chunkHeightmap;
		int x, y;
		
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
					0,
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
					0,
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
			
			terrainHeightmap[i][terrainHeightmapResolution - chunkResolution] = 0;
			
			// East
			
			terrainHeightmap[i][chunkResolution - 1] = 0;
		}
		
		// Smoothing transition for West and East sides
		
		for (int i = 0; i < terrainHeightmapResolution; i++)
		{
			for (int k = 1; k < chunkResolution - 1; k++)
			{
				// West
				
				nextHeight = interpolationConstaint * terrainHeightmap[i][terrainHeightmapResolution - chunkResolution + k] +
							 (1 - interpolationConstaint) * terrainHeightmap[i][terrainHeightmapResolution - chunkResolution + (k - 1)];
				
				terrainHeightmap[i][terrainHeightmapResolution - chunkResolution + k] = nextHeight;
				
				// East
				
				nextHeight = interpolationConstaint * terrainHeightmap[i][chunkResolution - 1 - k] +
							 (1 - interpolationConstaint) * terrainHeightmap[i][chunkResolution - 1 - (k - 1)];
				
				terrainHeightmap[i][chunkResolution - 1 - k] = nextHeight;
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
					0,
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
					0,
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
			terrainHeightmap[terrainHeightmapResolution - chunkResolution + i][terrainHeightmapResolution - chunkResolution] = 0;
			terrainHeightmap[terrainHeightmapResolution - chunkResolution][terrainHeightmapResolution - chunkResolution + i] = 0;
		}
		
		// Smoothing transition for North-West chunk
		
		for (int i = 0; i < terrainHeightmapResolution; i++)
		{
			for (int k = 1; k < chunkResolution - 1; k++)
			{
				// From +X
				
				nextHeight = interpolationConstaint * terrainHeightmap[i][terrainHeightmapResolution - chunkResolution + k] +
							 (1 - interpolationConstaint) * terrainHeightmap[i][terrainHeightmapResolution - chunkResolution + (k - 1)];
				
				terrainHeightmap[i][terrainHeightmapResolution - chunkResolution + k] = nextHeight;
				
				// From -Z
				
				nextHeight = interpolationConstaint * terrainHeightmap[terrainHeightmapResolution - chunkResolution + k][i] +
							 (1 - interpolationConstaint) * terrainHeightmap[terrainHeightmapResolution - chunkResolution + (k - 1)][i];
				
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
				0,
				terrainSize[2] + (chunkSize / 2) + chunksDepthOffset
			};
			
			x = terrainHeightmapResolution - chunkResolution;
			y = terrainHeightmapResolution - chunkResolution;
			
			chunkHeightmap = OTT_HeightmapHelper.Select(terrainHeightmap, x, y, chunkResolution, chunkResolution);
			
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
			terrainHeightmap[terrainHeightmapResolution - chunkResolution][i] = 0;
			terrainHeightmap[terrainHeightmapResolution - 1 - i][chunkResolution - 1] = 0;
		}
		
		// Smoothing transition for North-East chunk
		
		for (int i = 0; i < terrainHeightmapResolution; i++)
		{
			for (int k = 1; k < chunkResolution - 1; k++)
			{
				// From -X
				
				nextHeight = interpolationConstaint * terrainHeightmap[i][chunkResolution - 1 - k] +
							 (1 - interpolationConstaint) * terrainHeightmap[i][chunkResolution - 1 - (k - 1)];
				
				terrainHeightmap[i][chunkResolution - 1 - k] = nextHeight;
				
				// From -Z
				
				nextHeight = interpolationConstaint * terrainHeightmap[terrainHeightmapResolution - chunkResolution + k][i] +
							 (1 - interpolationConstaint) * terrainHeightmap[terrainHeightmapResolution - chunkResolution + (k - 1)][i];
				
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
				0,
				terrainSize[2] + (chunkSize / 2) + chunksDepthOffset
			};
			
			x = 0;
			y = terrainHeightmapResolution - chunkResolution;
			
			chunkHeightmap = OTT_HeightmapHelper.Select(terrainHeightmap, x, y, chunkResolution, chunkResolution);
			
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
			terrainHeightmap[i][terrainHeightmapResolution - chunkResolution] = 0;
			terrainHeightmap[chunkResolution - 1][terrainHeightmapResolution - chunkResolution + i] = 0;
		}
		
		// Smoothing transition for South-West chunk
		
		for (int i = 0; i < terrainHeightmapResolution; i++)
		{
			for (int k = 1; k < chunkResolution - 1; k++)
			{
				// From +X
				
				nextHeight = interpolationConstaint * terrainHeightmap[i][terrainHeightmapResolution - chunkResolution + k] +
							 (1 - interpolationConstaint) * terrainHeightmap[i][terrainHeightmapResolution - chunkResolution + (k - 1)];
				
				terrainHeightmap[i][terrainHeightmapResolution - chunkResolution + k] = nextHeight;
				
				// From +Z
				
				nextHeight = interpolationConstaint * terrainHeightmap[chunkResolution - 1 - k][i] +
							 (1 - interpolationConstaint) * terrainHeightmap[chunkResolution - 1 - (k - 1)][i];
				
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
				0,
				-(chunkSize / 2) - chunksDepthOffset
			};
			
			x = terrainHeightmapResolution - chunkResolution;
			y = 0;
			
			chunkHeightmap = OTT_HeightmapHelper.Select(terrainHeightmap, x, y, chunkResolution, chunkResolution);
			
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
			terrainHeightmap[chunkResolution - 1][i] = 0;
			terrainHeightmap[i][chunkResolution - 1] = 0;
		}
		
		// Smoothing transition for South-East chunk
		
		for (int i = 0; i < terrainHeightmapResolution; i++)
		{
			for (int k = 1; k < chunkResolution - 1; k++)
			{
				// From -X
				
				nextHeight = interpolationConstaint * terrainHeightmap[i][chunkResolution - 1 - k] +
							 (1 - interpolationConstaint) * terrainHeightmap[i][chunkResolution - 1 - (k - 1)];
				
				terrainHeightmap[i][chunkResolution - 1 - k] = nextHeight;
				
				// From +Z
				
				nextHeight = interpolationConstaint * terrainHeightmap[chunkResolution - 1 - k][i] +
							 (1 - interpolationConstaint) * terrainHeightmap[chunkResolution - 1 - (k - 1)][i];
				
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
				0,
				-(chunkSize / 2) + chunksDepthOffset
			};
			
			x = 0;
			y = 0;
			
			chunkHeightmap = OTT_HeightmapHelper.Select(terrainHeightmap, x, y, chunkResolution, chunkResolution);
			
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