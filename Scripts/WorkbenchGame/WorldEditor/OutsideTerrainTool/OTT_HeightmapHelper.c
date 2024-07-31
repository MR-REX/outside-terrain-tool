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
	
	static void Smooth(notnull array<ref array<float>> heightmap, int iterations = 1)
	{
		int height = heightmap.Count();
		
		if (height < 1)
			return;
		
		int width = heightmap[0].Count();
		
		if (width < 1)
			return;
		
		float sum;
		
		for (int iteration = 0; iteration < iterations; iteration++)
		{
			for (int i = 1; i < height - 1; i++)
			{
				for (int j = 1; j < width - 1; j++)
				{
					sum = 0;
					
					for (int k = -1; k <= 1; k++)
					{
						for (int l = -1; l <= 1; l++)
						{
							sum += heightmap[i + k][j + l];
						}
					}
					
					heightmap[i][j] = sum / 9;
				}
			}
		}
	}
}

#endif