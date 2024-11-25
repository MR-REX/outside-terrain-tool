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
		
		// Reusable variables
		
		vector chunkPosition;
		bool shouldCreate;
		
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
				
				CreateChunk(
					position: chunkPosition,
					angles: vector.Zero,
					size: chunkSizes,
					heightmap: {{0, 0}, {0, 0}},
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
				
				CreateChunk(
					position: chunkPosition,
					angles: vector.Zero,
					size: chunkSizes,
					heightmap: {{0, 0}, {0, 0}},
					enablePhysics: shouldEnablePhysics
				);
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
				
				CreateChunk(
					position: chunkPosition,
					angles: vector.Zero,
					size: chunkSizes,
					heightmap: {{0, 0}, {0, 0}},
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
				
				CreateChunk(
					position: chunkPosition,
					angles: vector.Zero,
					size: chunkSizes,
					heightmap: {{0, 0}, {0, 0}},
					enablePhysics: shouldEnablePhysics
				);
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
			
			CreateChunk(
				position: chunkPosition,
				angles: vector.Zero,
				size: chunkSizes,
				heightmap: {{0, 0}, {0, 0}},
				enablePhysics: shouldEnablePhysics
			);
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
			
			CreateChunk(
				position: chunkPosition,
				angles: vector.Zero,
				size: chunkSizes,
				heightmap: {{0, 0}, {0, 0}},
				enablePhysics: shouldEnablePhysics
			);
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
			
			CreateChunk(
				position: chunkPosition,
				angles: vector.Zero,
				size: chunkSizes,
				heightmap: {{0, 0}, {0, 0}},
				enablePhysics: shouldEnablePhysics
			);
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
			
			CreateChunk(
				position: chunkPosition,
				angles: vector.Zero,
				size: chunkSizes,
				heightmap: {{0, 0}, {0, 0}},
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