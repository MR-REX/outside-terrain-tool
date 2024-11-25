#ifdef WORKBENCH

class OTT_SuperFlatOutsideTerrainGenerator : OTT_OutsideTerrainGenerator
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
		
		// Create plane at North
		
		if (!contextOptions.ShouldIgnoreDirection(OTT_CardinalDirections.North))
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
		
		// Create plane at South
		
		if (!contextOptions.ShouldIgnoreDirection(OTT_CardinalDirections.South))
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
		
		// Create plane at West
		
		if (!contextOptions.ShouldIgnoreDirection(OTT_CardinalDirections.West))
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
		
		// Create plane at East
		
		if (!contextOptions.ShouldIgnoreDirection(OTT_CardinalDirections.West))
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
		
		// Create plane at North-West
		
		if (!contextOptions.ShouldIgnoreDirection(OTT_CardinalDirections.NorthWest))
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
		
		// Create plane at North-East
		
		if (!contextOptions.ShouldIgnoreDirection(OTT_CardinalDirections.NorthEast))
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
		
		// Create plane at South-West
		
		if (!contextOptions.ShouldIgnoreDirection(OTT_CardinalDirections.SouthWest))
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
		
		// Create plane at South-East
		
		if (!contextOptions.ShouldIgnoreDirection(OTT_CardinalDirections.SouthEast))
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
		
		return true;
	}
}