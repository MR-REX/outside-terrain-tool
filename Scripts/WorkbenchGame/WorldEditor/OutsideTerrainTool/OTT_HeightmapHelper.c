#ifdef WORKBENCH

class OTT_HeightmapHelper
{	
	static void GetHeightmapSize(notnull array<ref array<float>> heightmap, out int width = 0, out int height = 0)
	{
		int rows = heightmap.Count();
		
		if (rows < 2)
			return;
		
		int columns = heightmap[0].Count();
		
		if (columns < 2)
			return;
		
		width = columns;
		height = rows;
	}
	
	static array<float> ToFlat(notnull array<ref array<float>> heightmap)
	{
		int width, height;
		GetHeightmapSize(heightmap, width, height);
		
		array<float> heights = {};
		
		for (int i = height - 1; i >= 0; i--)
		{
			for (int j = 0; j < width; j++)
			{
				heights.Insert(heightmap[i][j]);
			}
		}
		
		return heights;
	}
	
	static bool IsHeightmapSquare(notnull array<ref array<float>> heightmap)
	{
		int width, height;
		GetHeightmapSize(heightmap, width, height);
		
		return width == height;
	}
	
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
			for (int j = 0; j < width / 2; j++)
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
		
		for (int i = 0; i < height / 2; i++)
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
	
	static array<ref array<float>> Select(array<ref array<float>> heightmap, int x, int y, int width, int height)
	{		
		array<ref array<float>> part = {};
		
		for (int i = y; i < y + height; i++)
		{
			array<float> row = {};
			
			for (int j = x; j < x + width; j++)
			{
				row.Insert(heightmap[i][j]);
			}
			
			part.Insert(row);
		}
		
		return part;
	}
	
	static array<ref array<float>> Resize(array<ref array<float>> heightmap, int width, int height)
	{
		int sourceWidth, sourceHeight;
		GetHeightmapSize(heightmap, sourceWidth, sourceHeight);
		
		array<ref array<float>> resized = {};
		int x, y;
		
		for (int i = 0; i < height; i++)
		{
			array<float> row = {};
			
			for (int j = 0; j < width; j++)
			{
				x = j % sourceWidth;
				y = i % sourceHeight;
				
				row.Insert(heightmap[y][x]);
			}
			
			resized.Insert(row);
		}
		
		return resized;
	}
	
	static void Rotate(array<ref array<float>> heightmap, int iterations = 1)
	{
		int size = heightmap.Count();
		float value;
		
		for (int iteration = 0; iteration < iterations; iteration++)
		{
			for (int i = 0; i < size / 2; i++)
			{
				for (int j = i; j < size - i - 1; j++)
				{
					value = heightmap[i][j];
					
					heightmap[i][j] = heightmap[size - 1 - j][i];
					heightmap[size - 1 - j][i] = heightmap[size - 1 - i][size - 1 - j];
					heightmap[size - 1 - i][size - 1 - j] = heightmap[j][size - 1 - i];
					heightmap[j][size - 1 - i] = value;
				}
			}
		}
	}
	
	static array<ref array<float>> Blend(array<ref array<float>> sourceHeightmap, array<ref array<float>> additionalHeightmap)
	{
		int sourceWidth, sourceHeight;
		GetHeightmapSize(sourceHeightmap, sourceWidth, sourceHeight);
		
		int additionalWidth, additionalHeight;
		GetHeightmapSize(additionalHeightmap, additionalWidth, additionalHeight);
		
		int width = Math.Max(sourceWidth, additionalWidth);
		int height = Math.Max(sourceHeight, additionalHeight);
		
		array<ref array<float>> heightmap = Resize(sourceHeightmap, width, height);
		
		for (int i = 0; i < additionalHeight; i++)
		{
			for (int j = 0; j < additionalWidth; j++)
			{
				heightmap[i][j] = (heightmap[i][j] + additionalHeightmap[i][j]) / 2;
			}
		}
		
		return heightmap;
	}
}

#endif