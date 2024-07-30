#ifdef WORKBENCH

class OTT_HeightmapHelper
{
	static void FlipVertical(notnull array<ref array<float>> heightmap)
	{
		int height = heightmap.Count();
		
		if (height < 2)
			return;
		
		int width = heightmap[0].Count();
		
		if (width < 2)
			return;
		
		int k;
		float value;
		
		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				k = (width - 1) - j;
				value = heightmap[i][j];
				
				heightmap[i][j] = heightmap[i][k];
				heightmap[i][k] = value;
			}
		}
	}
	
	static void FlipHorizontal(notnull array<ref array<float>> heightmap)
	{
		int height = heightmap.Count();
		
		if (height < 2)
			return;
		
		int k;
		array<float> row;
		
		for (int i = 0; i < height * 0.5; i++)
		{
			k = (height - 1) - i;
			row = heightmap[i];
			
			heightmap[i] = heightmap[k];
			heightmap[k] = row;
		}
	}
}

#endif