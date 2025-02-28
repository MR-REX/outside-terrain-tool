#ifdef WORKBENCH

class OTT_FlatOutsideTerrainGenerator : OTT_OutsideTerrainGenerator
{
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
	
	protected int GetFlatHeightmapResolution()
	{
		return 2;
	}
	
	protected array<ref array<float>> GenerateFlatHeightmap(int resolution)
	{
		array<ref array<float>> flatHeightmap = {};
		
		for (int i = 0; i < resolution; i++)
		{
			array<float> heights = {};
			
			for (int j = 0; j < resolution; j++)
			{
				heights.Insert(0);
			}
			
			flatHeightmap.Insert(heights);
		}
		
		return flatHeightmap;
	}
	
	protected bool ShouldEnablePhysics()
	{
		OTT_EOutsideTerrainPhysicsType physicsType = m_Options.GetPhysicsType();
		return physicsType == OTT_EOutsideTerrainPhysicsType.Full;
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
		
		// Getting terrain data
		
		vector terrainMins = m_Terrain.GetMins();
		vector terrainSize = m_Terrain.GetSize();
		
		// Calculating plane sizes
		
		float planeSizeMultiplier = GetPlaneSizeMultiplier();
		
		float planeLength = terrainSize[0];
		float planeDepth = planeLength * planeSizeMultiplier;
		
		// Calculating outside terrain position
		
		vector outsideTerrainPosition = {terrainMins[0], 0, terrainMins[2]};
		
		// Generating flat heightmap
		
		int flatHeightmapResolution = GetFlatHeightmapResolution();
		array<ref array<float>> flatHeightmap = GenerateFlatHeightmap(flatHeightmapResolution);
		
		// Should enable physics for planes?
		
		bool shouldEnablePhysics = ShouldEnablePhysics();
		
		// Getting generation options
		
		float planeDepthOffset = m_Options.GetDepthOffset();
		
		// Reusable variables
		
		vector planePosition;
		bool shouldCreate;
		
		// Create plane at North
		
		m_Manager.SetChunksPrefix("North");
		
		shouldCreate = ShouldProcessSide(terrainSize[0] / 2, terrainSize[2] - 1) &&
					   !contextOptions.ShouldIgnoreDirection(OTT_CardinalDirections.North);
		
		if (shouldCreate)
		{
			planePosition = outsideTerrainPosition + {
				terrainSize[0] / 2,
				0,
				terrainSize[2] + planeDepth / 2 + planeDepthOffset
			};
			
			CreateChunk(
				position: planePosition,
				angles: vector.Zero,
				size: {planeLength, 0, planeDepth},
				heightmap: flatHeightmap,
				enablePhysics: shouldEnablePhysics
			);
		}
		
		m_Manager.SetProcessProgress(1 / 8);
		
		// Create plane at South
		
		m_Manager.SetChunksPrefix("South");
		
		shouldCreate = ShouldProcessSide(terrainSize[0] / 2, 0) &&
					   !contextOptions.ShouldIgnoreDirection(OTT_CardinalDirections.South);
		
		if (shouldCreate)
		{
			planePosition = outsideTerrainPosition + {
				terrainSize[0] / 2,
				0,
				-(planeDepth / 2) - planeDepthOffset
			};
			
			CreateChunk(
				position: planePosition,
				angles: vector.Zero,
				size: {planeLength, 0, planeDepth},
				heightmap: flatHeightmap,
				enablePhysics: shouldEnablePhysics
			);
		}
		
		m_Manager.SetProcessProgress(2 / 8);
		
		// Create plane at West
		
		m_Manager.SetChunksPrefix("West");
		
		shouldCreate = ShouldProcessSide(0, terrainSize[2] / 2) &&
					   !contextOptions.ShouldIgnoreDirection(OTT_CardinalDirections.West);
		
		if (shouldCreate)
		{
			planePosition = outsideTerrainPosition + {
				-(planeDepth / 2) - planeDepthOffset,
				0,
				terrainSize[2] / 2
			};
			
			CreateChunk(
				position: planePosition,
				angles: vector.Zero,
				size: {planeDepth, 0, planeLength},
				heightmap: flatHeightmap,
				enablePhysics: shouldEnablePhysics
			);
		}
		
		m_Manager.SetProcessProgress(3 / 8);
		
		// Create plane at East
		
		m_Manager.SetChunksPrefix("East");
		
		shouldCreate = ShouldProcessSide(terrainSize[0] - 1, terrainSize[2] / 2) &&
					   !contextOptions.ShouldIgnoreDirection(OTT_CardinalDirections.East);
		
		if (shouldCreate)
		{
			planePosition = outsideTerrainPosition + {
				terrainSize[0] + planeDepth / 2 + planeDepthOffset,
				0,
				terrainSize[2] / 2
			};
			
			CreateChunk(
				position: planePosition,
				angles: vector.Zero,
				size: {planeDepth, 0, planeLength},
				heightmap: flatHeightmap,
				enablePhysics: shouldEnablePhysics
			);
		}
		
		m_Manager.SetProcessProgress(4 / 8);
		
		// Create plane at North-West
		
		m_Manager.SetChunksPrefix("NorthWest");
		
		shouldCreate = ShouldProcessSide(terrainSize[0] / 2, terrainSize[2] - 1) &&
					   ShouldProcessSide(0, terrainSize[2] / 2) &&
					   !contextOptions.ShouldIgnoreDirection(OTT_CardinalDirections.NorthWest);
		
		if (shouldCreate)
		{
			planePosition = outsideTerrainPosition + {
				-(planeDepth / 2) - planeDepthOffset,
				0,
				terrainSize[2] + planeDepth / 2 + planeDepthOffset
			};
			
			CreateChunk(
				position: planePosition,
				angles: vector.Zero,
				size: {planeDepth, 0, planeDepth},
				heightmap: flatHeightmap,
				enablePhysics: shouldEnablePhysics
			);
		}
		
		m_Manager.SetProcessProgress(5 / 8);
		
		// Create plane at North-East
		
		m_Manager.SetChunksPrefix("NorthEast");
		
		shouldCreate = ShouldProcessSide(terrainSize[0] / 2, terrainSize[2] - 1) &&
					   ShouldProcessSide(terrainSize[0] - 1, terrainSize[2] / 2) &&
					   !contextOptions.ShouldIgnoreDirection(OTT_CardinalDirections.NorthEast);
		
		if (shouldCreate)
		{
			planePosition = outsideTerrainPosition + {
				terrainSize[0] + planeDepth / 2 + planeDepthOffset,
				0,
				terrainSize[2] + planeDepth / 2 + planeDepthOffset
			};
			
			CreateChunk(
				position: planePosition,
				angles: vector.Zero,
				size: {planeDepth, 0, planeDepth},
				heightmap: flatHeightmap,
				enablePhysics: shouldEnablePhysics
			);
		}
		
		m_Manager.SetProcessProgress(6 / 8);
		
		// Create plane at South-West
		
		m_Manager.SetChunksPrefix("SouthWest");
		
		shouldCreate = ShouldProcessSide(terrainSize[0] / 2, 0) &&
					   ShouldProcessSide(0, terrainSize[2] / 2) &&
					   !contextOptions.ShouldIgnoreDirection(OTT_CardinalDirections.SouthWest);
		
		if (shouldCreate)
		{
			planePosition = outsideTerrainPosition + {
				-(planeDepth / 2) - planeDepthOffset,
				0,
				-(planeDepth / 2) - planeDepthOffset
			};
			
			CreateChunk(
				position: planePosition,
				angles: vector.Zero,
				size: {planeDepth, 0, planeDepth},
				heightmap: flatHeightmap,
				enablePhysics: shouldEnablePhysics
			);
		}
		
		m_Manager.SetProcessProgress(7 / 8);
		
		// Create plane at South-East
		
		m_Manager.SetChunksPrefix("SouthEast");
		
		shouldCreate = ShouldProcessSide(terrainSize[0] / 2, 0) &&
					   ShouldProcessSide(terrainSize[0] - 1, terrainSize[2] / 2) &&
					   !contextOptions.ShouldIgnoreDirection(OTT_CardinalDirections.SouthEast);
		
		if (shouldCreate)
		{
			planePosition = outsideTerrainPosition + {
				terrainSize[0] + planeDepth / 2 + planeDepthOffset,
				0,
				-(planeDepth / 2) - planeDepthOffset
			};
			
			CreateChunk(
				position: planePosition,
				angles: vector.Zero,
				size: {planeDepth, 0, planeDepth},
				heightmap: flatHeightmap,
				enablePhysics: shouldEnablePhysics
			);
		}
		
		m_Manager.SetProcessProgress(8 / 8);
		
		return true;
	}
}