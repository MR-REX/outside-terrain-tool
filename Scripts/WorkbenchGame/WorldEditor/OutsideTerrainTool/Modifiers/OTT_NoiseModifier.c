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
	protected float m_fForce;
	
	void OTT_NoiseModifier(OTT_ENoiseAlgorithm algorithm = OTT_ENoiseAlgorithm.Perlin, int seed = 0, float force = 0)
	{
		m_eNoiseAlgorithm = algorithm;
		m_iSeed = seed;
		m_fForce = force;
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
	
	void SetForce(float force)
	{
		m_fForce = force;
	}
	
	protected float GetMultiplier(int index, int x, int y)
	{
		float offset = m_iSeed + 0.1;
		
		if (m_eNoiseAlgorithm == OTT_ENoiseAlgorithm.Simplex)
			return Math.SimplexNoise1D(offset + index) + 1;
		
		return Math.PerlinNoise(offset + x, offset + y) + 1;
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
				if (i == 0 || j == 0 || i == rows - 1 || j == columns - 1)
					continue;
				
				heightmap[i][j] = heightmap[i][j] * GetMultiplier(index, j, i) * m_fForce;
				index++;
			}
		}
	}
}

#endif