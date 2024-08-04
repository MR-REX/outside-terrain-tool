#ifdef WORKBENCH

enum OTT_EOutsideTerrainGeneratorTypes
{
	Simple
}

class OTT_OutsideTerrainGeneratorFactory
{
	static OTT_OutsideTerrainGenerator Create(OTT_EOutsideTerrainGeneratorTypes type, OTT_OutsideTerrainGeneratorOptions options, OTT_OutsideTerrainManager manager)
	{
		switch(type)
		{
			case OTT_EOutsideTerrainGeneratorTypes.Simple:
				return new OTT_SimpleOutsideTerrainGenerator(options, manager);
		}
		
		return null;
	}
}

#endif