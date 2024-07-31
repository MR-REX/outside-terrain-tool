#ifdef WORKBENCH

enum OTT_ENoiseAlgorithm
{
	Perlin,
	Simplex
}

class OTT_NoiseModifier : OTT_HeightmapModifier
{
	protected OTT_ENoiseAlgorithm m_eNoiseAlgorithm;
	protected int m_iSeed;
	
	void OTT_NoiseModifier(OTT_ENoiseAlgorithm algorithm = OTT_ENoiseAlgorithm.Perlin, int seed = 0)
	{
		m_eNoiseAlgorithm = algorithm;
		m_iSeed = seed;
	}
	
	void SetAlgorithm(OTT_ENoiseAlgorithm algorithm)
	{
		m_eNoiseAlgorithm = algorithm;
	}
	
	void SetSeed(int seed)
	{
		m_iSeed = seed;
	}
	
	void SetSeed(string input)
	{
		int length = input.Trim();
		int sum = 0;
		
		for (int i = 0; i < length; i++)
		{
			sum += input.ToAscii(i);
		}
		
		m_iSeed = Math.AbsInt(sum);
	}
	
	protected float GetMultiplier(int index, int x, int y)
	{
		float offset = m_iSeed + 0.1;
		
		if (m_eNoiseAlgorithm == OTT_ENoiseAlgorithm.Simplex)
			return (Math.SimplexNoise1D(offset + index) + 1) / 2;
		
		return Math.PerlinNoise01(offset + x, offset + y);
	}
	
	override void Modify(notnull array<ref array<float>> heightmap)
	{
		int rows = heightmap.Count();
		
		if (rows < 1)
			return;
		
		int columns = heightmap[0].Count();
		int index = 0;
		
		for (int i = 0; i < rows; i++)
		{
			for (int j = 0; j < columns; j++)
			{
				heightmap[i][j] = heightmap[i][j] * GetMultiplier(index, j, i);
				index++;
			}
		}
	}
}

#endif