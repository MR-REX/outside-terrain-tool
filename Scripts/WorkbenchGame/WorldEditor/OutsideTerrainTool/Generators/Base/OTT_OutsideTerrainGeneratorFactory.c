#ifdef WORKBENCH

enum OTT_EOutsideTerrainGeneratorTypes
{
	Default,
	Simple,
	Subdivided,
	Waterfall,
	Flat
}

class OTT_OutsideTerrainGeneratorFactory
{
	static OTT_OutsideTerrainGenerator Create(OTT_EOutsideTerrainGeneratorTypes type, OTT_OutsideTerrainGeneratorOptions options, OTT_OutsideTerrainManager manager)
	{
		switch(type)
		{
			case OTT_EOutsideTerrainGeneratorTypes.Default:
				return new OTT_DefaultOutsideTerrainGenerator(options, manager);
			case OTT_EOutsideTerrainGeneratorTypes.Simple:
				return new OTT_SimpleOutsideTerrainGenerator(options, manager);
			case OTT_EOutsideTerrainGeneratorTypes.Subdivided:
				return new OTT_SubdividedOutsideTerrainGenerator(options, manager);
			case OTT_EOutsideTerrainGeneratorTypes.Waterfall:
				return new OTT_WaterfallOutsideTerrainGenerator(options, manager);
			case OTT_EOutsideTerrainGeneratorTypes.Flat:
				return new OTT_FlatOutsideTerrainGenerator(options, manager);
		}
		
		return null;
	}
}

#endif