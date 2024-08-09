#ifdef WORKBENCH

class OTT_SimpleOutsideTerrainGenerator : OTT_OutsideTerrainGenerator
{
	protected int GetChunkResolution()
	{
		OTT_EOutsideTerrainQuality quality = m_Options.GetQuality();
		
		switch (quality)
		{
			case OTT_EOutsideTerrainQuality.High:
				return 32;
			case OTT_EOutsideTerrainQuality.Medium:
				return 24;
			case OTT_EOutsideTerrainQuality.Low:
				return 16;
		}
		
		return 0;
	}
	
	protected int GetChunksCount()
	{
		OTT_EOutsideTerrainQuality quality = m_Options.GetQuality();
		
		switch (quality)
		{
			case OTT_EOutsideTerrainQuality.High:
				return 32;
			case OTT_EOutsideTerrainQuality.Medium:
				return 24;
			case OTT_EOutsideTerrainQuality.Low:
				return 16;
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
			case OTT_EOutsideTerrainSize.Normal:
				return 0.5;
			case OTT_EOutsideTerrainSize.Minimal:
				return 0.25;
		}
		
		return 0;
	}
	
	override protected bool Process()
	{
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
		
		// Gettings terrain size
		
		vector terrainSize = m_vTerrainMaxs - m_vTerrainMins;
		
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
		
		// Creating plane of chunks at north side
		
		for (int z = 0; z < chunksDepth; z++)
		{
			for (int x = 0; x < chunksCount; x++)
			{
				i = terrainHeightmapResolution - chunkResolution * (z + 1);
				j = chunkResolution * x;
				
				chunkHeightmap = OTT_HeightmapHelper.Select(terrainHeightmap, j, i, chunkResolution, chunkResolution);
				
				position = {
					m_vTerrainMins[0] + (chunkWidth / 2) + (chunkWidth * x),
					m_vTerrainMins[1],
					m_vTerrainMins[2] + terrainSize[2] + (chunkHeight / 2) + (chunkHeight * z)
				};
				
				CreateChunk(position, vector.Zero, chunkSize, chunkHeightmap, true);
			}
		}
		
		// Creating plane of chunks at south side
		
		for (int z = 0; z < chunksDepth; z++)
		{
			for (int x = 0; x < chunksCount; x++)
			{
				i = chunkResolution * z;
				j = chunkResolution * x;
				
				chunkHeightmap = OTT_HeightmapHelper.Select(terrainHeightmap, j, i, chunkResolution, chunkResolution);
				
				position = {
					m_vTerrainMins[0] + (chunkWidth / 2) + (chunkWidth * x),
					m_vTerrainMins[1],
					m_vTerrainMins[2] - (chunkHeight / 2) - (chunkHeight * z)
				};
				
				CreateChunk(position, vector.Zero, chunkSize, chunkHeightmap, true);
			}
		}
		
		// Creating plane of chunks at west side
		
		OTT_HeightmapHelper.Rotate(terrainHeightmap, 2);
		
		for (int z = 0; z < chunksCount; z++)
		{
			for (int x = 0; x < chunksDepth; x++)
			{
				i = terrainHeightmapResolution - chunkResolution * (z + 1);
				j = terrainHeightmapResolution - chunkResolution * (x + 1);
				
				chunkHeightmap = OTT_HeightmapHelper.Select(terrainHeightmap, j, i, chunkResolution, chunkResolution);
				
				position = {
					m_vTerrainMins[0] - (chunkWidth / 2) - (chunkWidth * x),
					m_vTerrainMins[1],
					m_vTerrainMins[2] + (chunkHeight / 2) + (chunkHeight * z)
				};
				
				CreateChunk(position, vector.Zero, chunkSize, chunkHeightmap, true);
			}
		}
		
		// Creating plane of chunks at east side
		
		for (int z = 0; z < chunksCount; z++)
		{
			for (int x = 0; x < chunksDepth; x++)
			{
				i = terrainHeightmapResolution - chunkResolution * (z + 1);
				j = chunkResolution * x;
				
				chunkHeightmap = OTT_HeightmapHelper.Select(terrainHeightmap, j, i, chunkResolution, chunkResolution);
				
				position = {
					m_vTerrainMins[0] + terrainSize[0] + (chunkWidth / 2) + (chunkWidth * x),
					m_vTerrainMins[1],
					m_vTerrainMins[2] + (chunkHeight / 2) + (chunkHeight * z)
				};
				
				CreateChunk(position, vector.Zero, chunkSize, chunkHeightmap, true);
			}
		}
		
		// Creating plane of chunks at north-west side
		
		OTT_HeightmapHelper.FlipVertical(cachedHeightmap);
		
		for (int z = 0; z < chunksDepth; z++)
		{
			for (int x = 0; x < chunksDepth; x++)
			{
				i = terrainHeightmapResolution - chunkResolution * (z + 1);
				j = (chunkResolution * (chunksCount - chunksDepth)) + (chunkResolution * x);
				
				chunkHeightmap = OTT_HeightmapHelper.Select(cachedHeightmap, j, i, chunkResolution, chunkResolution);
				
				position = {
					m_vTerrainMins[0] - ((chunksDepth - 1) * chunkWidth) - (chunkWidth / 2) + (chunkWidth * x),
					m_vTerrainMins[1],
					m_vTerrainMins[2] + terrainSize[2] + (chunkHeight / 2) + (chunkHeight * z)
				};
				
				CreateChunk(position, vector.Zero, chunkSize, chunkHeightmap, true);
			}
		}
		
		// Creating plane of chunks at north-east side
		
		for (int z = 0; z < chunksDepth; z++)
		{
			for (int x = 0; x < chunksDepth; x++)
			{
				i = terrainHeightmapResolution - chunkResolution * (z + 1);
				j = chunkResolution * x;
				
				chunkHeightmap = OTT_HeightmapHelper.Select(cachedHeightmap, j, i, chunkResolution, chunkResolution);
				
				position = {
					m_vTerrainMins[0] + terrainSize[0] + (chunkWidth / 2) + (chunkWidth * x),
					m_vTerrainMins[1],
					m_vTerrainMins[2] + terrainSize[2] + (chunkHeight / 2) + (chunkHeight * z)
				};
				
				CreateChunk(position, vector.Zero, chunkSize, chunkHeightmap, true);
			}
		}
		
		// Creating plane of chunks at south-west side
		
		for (int z = 0; z < chunksDepth; z++)
		{
			for (int x = 0; x < chunksDepth; x++)
			{
				i = chunkResolution * z;
				j = terrainHeightmapResolution - chunkResolution * (x + 1);
				
				chunkHeightmap = OTT_HeightmapHelper.Select(cachedHeightmap, j, i, chunkResolution, chunkResolution);
				
				position = {
					m_vTerrainMins[0] - (chunkWidth / 2) - (chunkWidth * x),
					m_vTerrainMins[1],
					m_vTerrainMins[2] - (chunkHeight / 2) - (chunkHeight * z)
				};
				
				CreateChunk(position, vector.Zero, chunkSize, chunkHeightmap, true);
			}
		}
		
		// Creating plane of chunks at south-east side
		
		for (int z = 0; z < chunksDepth; z++)
		{
			for (int x = 0; x < chunksDepth; x++)
			{
				i = chunkResolution * z;
				j = chunkResolution * x;
				
				chunkHeightmap = OTT_HeightmapHelper.Select(cachedHeightmap, j, i, chunkResolution, chunkResolution);
				
				position = {
					m_vTerrainMins[0] + terrainSize[0] + (chunkWidth / 2) + (chunkWidth * x),
					m_vTerrainMins[1],
					m_vTerrainMins[2] - (chunkHeight / 2) - (chunkHeight * z)
				};
				
				CreateChunk(position, vector.Zero, chunkSize, chunkHeightmap, true);
			}
		}
		
		return true;
	}
}

#endif