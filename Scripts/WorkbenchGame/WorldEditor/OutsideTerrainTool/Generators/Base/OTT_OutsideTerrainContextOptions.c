#ifdef WORKBENCH

enum OTT_CardinalDirections
{
	North,
	NorthEast,
	East,
	SouthEast,
	South,
	SouthWest,
	West,
	NorthWest
}

class OTT_OutsideTerrainContextOptions
{
	protected bool m_bTrackOceanLevel;
	protected ref array<OTT_CardinalDirections> m_aIgnoreDirections;
	
	void OTT_OutsideTerrainContextOptions(bool trackOceanLevel, array<OTT_CardinalDirections> ignoreDirections)
	{
		m_bTrackOceanLevel = trackOceanLevel;
		m_aIgnoreDirections = ignoreDirections;
	}
	
	bool ShouldTrackOceanLevel()
	{
		return m_bTrackOceanLevel;
	}
	
	bool ShouldIgnoreDirection(OTT_CardinalDirections direction)
	{
		return m_aIgnoreDirections.Contains(direction);
	}
}

#endif