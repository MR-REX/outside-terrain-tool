#ifdef WORKBENCH

enum OTT_EOutsideTerrainGeneratorTypes
{
	Simple,
	Subdivided
}

class OTT_OutsideTerrainGeneratorFactory
{
	static OTT_OutsideTerrainGenerator Create(OTT_EOutsideTerrainGeneratorTypes type, OTT_OutsideTerrainGeneratorOptions options, OTT_OutsideTerrainManager manager)
	{
		switch(type)
		{
			case OTT_EOutsideTerrainGeneratorTypes.Simple:
				return new OTT_SimpleOutsideTerrainGenerator(options, manager);
			case OTT_EOutsideTerrainGeneratorTypes.Subdivided:
				return new OTT_SubdividedOutsideTerrainGenerator(options, manager);
		}
		
		return null;
	}
}

#endif