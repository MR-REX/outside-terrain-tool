#ifdef WORKBENCH

class OTT_SmoothingModifier : OTT_HeightmapModifier
{
	protected int m_iIterations;
	
	void OTT_SmoothingModifier(int iterations = 1)
	{
		m_iIterations = iterations;
	}
	
	override void Modify(notnull array<ref array<float>> heightmap)
	{
		OTT_HeightmapHelper.Smooth(heightmap, m_iIterations);
	}
}

#endif