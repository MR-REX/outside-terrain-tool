#ifdef WORKBENCH

[WorkbenchToolAttribute(name: "Outside Terrain Tool", awesomeFontCode: 0xF1FE)]
class OTT_OutsideTerrainTool : WorldEditorTool
{
	// Category: General
	
	[
		Attribute
		(
			category: "General",
			desc: "Outside terrain generation type",
			uiwidget: UIWidgets.ComboBox,
			enums: ParamEnumArray.FromEnum(OTT_EOutsideTerrainGeneratorTypes),
			defvalue: OTT_EOutsideTerrainGeneratorTypes.Default.ToString()
		)
	]
	protected OTT_EOutsideTerrainGeneratorTypes m_eOutsideTerrainGeneratorType;
	
	[
		Attribute
		(
			category: "General",
			desc: "Size of outside terrain",
			uiwidget: UIWidgets.ComboBox,
			enums: ParamEnumArray.FromEnum(OTT_EOutsideTerrainSize),
			defvalue: OTT_EOutsideTerrainSize.Medium.ToString()
		)
	]
	protected OTT_EOutsideTerrainSize m_eOutsideTerrainSize;
	
	[
		Attribute
		(
			category: "General",
			desc: "Quality of outside terrain",
			uiwidget: UIWidgets.ComboBox,
			enums: ParamEnumArray.FromEnum(OTT_EOutsideTerrainQuality),
			defvalue: OTT_EOutsideTerrainQuality.Highest.ToString()
		)
	]
	protected OTT_EOutsideTerrainQuality m_eOutsideTerrainQuality;
	
	// Category: Terrain
	
	[
		Attribute
		(
			category: "Terrain",
			desc: "Number of blocks in a tile by width",
			uiwidget: UIWidgets.Range,
			params: "1 128 1",
			defvalue: "4"
		)
	]
	protected int m_iBlocksPerTileWidth;
	
	[
		Attribute
		(
			category: "Terrain",
			desc: "Number of blocks in a tile by height",
			uiwidget: UIWidgets.Range,
			params: "1 128 1",
			defvalue: "4"
		)
	]
	protected int m_iBlocksPerTileHeight;
	
	// Category: Physics
	
	[
		Attribute
		(
			category: "Physics",
			desc: "A method for generating physics for outside terrain chunks",
			uiwidget: UIWidgets.ComboBox,
			enums: ParamEnumArray.FromEnum(OTT_EOutsideTerrainPhysicsType),
			defvalue: OTT_EOutsideTerrainPhysicsType.Full.ToString()
		)
	]
	protected OTT_EOutsideTerrainPhysicsType m_ePhysicsType;
	
	[
		Attribute
		(
			category: "Physics",
			desc: "Physics layer preset for outside terrain chunks",
			uiwidget: UIWidgets.ComboBox,
			enums: ParamEnumArray.FromEnum(EPhysicsLayerPresets),
			defvalue: EPhysicsLayerPresets.Terrain.ToString()
		)
	]
	protected EPhysicsLayerPresets m_ePhysicsLayerPreset;
	
	// Category: Noise Modifier
	
	[
		Attribute
		(
			category: "Noise Modifier",
			desc: "Enable noise modifier for all outside terrain heightmaps",
			uiwidget: UIWidgets.CheckBox,
			defvalue: "0"
		)
	]
	protected bool m_bEnableNoiseModifier;
	
	[
		Attribute
		(
			category: "Noise Modifier",
			uiwidget: UIWidgets.ComboBox,
			enums: ParamEnumArray.FromEnum(OTT_ENoiseAlgorithm),
			defvalue: "0"
		)
	]
	protected OTT_ENoiseAlgorithm m_eNoiseAlgorithm;
	
	[
		Attribute
		(
			category: "Noise Modifier",
			desc: "Seed for noise modifier",
			defvalue: "Hello, outside terrain!"
		)
	]
	protected string m_sNoiseSeed;
	
	[
		Attribute
		(
			category: "Noise Modifier",
			desc: "Force multiplier for noise modifier",
			uiwidget: UIWidgets.Slider,
			params: "0.1 16 0.1",
			defvalue: "1"
		)
	]
	protected float m_fNoiseForce;
	
	// Category: Smoothing Modifier
	
	[
		Attribute
		(
			category: "Smoothing Modifier",
			desc: "Enable smoothing modifier for all outside terrain heightmaps",
			uiwidget: UIWidgets.CheckBox,
			defvalue: "0"
		)
	]
	protected bool m_bEnableSmoothingModifier;
	
	[
		Attribute
		(
			category: "Smoothing Modifier",
			desc: "Number of smoothing iterations",
			uiwidget: UIWidgets.Slider,
			params: "0 32 1",
			defvalue: "1"
		)
	]
	protected int m_iSmoothingIterations;
	
	// Category: Border
	
	[
		Attribute
		(
			category: "Border",
			desc: "Height multiplier on border of outside terrain",
			defvalue: "1"
		)
	]
	protected float m_fBorderHeightMultiplier;
	
	// Category: Transform
	
	[
		Attribute
		(
			category: "Transform",
			desc: "Position offset for outside terrain chunks",
			uiwidget: UIWidgets.Coords,
			defvalue: "0 0 0"
		)
	]
	protected vector m_vChunksPositionOffset;
	
	[
		Attribute
		(
			category: "Transform",
			desc: "Depth offset for outside terrain chunks",
			defvalue: "0"
		)
	]
	protected float m_fChunksDepthOffset;
	
	// Category: Materials
	
	[
		Attribute
		(
			category: "Materials",
			desc: "Material of outside terrain chunks",
			uiwidget: UIWidgets.ResourcePickerThumbnail,
			params: "emat",
			defvalue: "{FCDCBAF69074F24C}Common/Textures/OutsideTerrainCore/DefaultMaterial.emat"
		)
	]
	protected ResourceName m_ChunksMaterial;
	
	// Category: Layer
	
	[
		Attribute
		(
			category: "Layer",
			desc: "Template for name of outside terrain chunks layer",
			defvalue: "OutsideTerrain"
		)
	]
	protected string m_sLayerNameTemplate;
	
	[
		Attribute
		(
			category: "Layer",
			desc: "Template for name of outside terrain chunk entity",
			defvalue: "OutsideTerrainChunk_%1_%2"
		)
	]
	protected string m_sEntityNameTemplate;
	
	// Category: Context
	
	[
		Attribute
		(
			category: "Context",
			desc: "Take into account shores facing the ocean",
			uiwidget: UIWidgets.CheckBox,
			defvalue: "1"
		)
	]
	protected bool m_bUseOceanLevel;
	
	[
		Attribute
		(
			category: "Context",
			desc: "Allows to ignore the North side of the map when generating outside terrain",
			uiwidget: UIWidgets.CheckBox,
			defvalue: "0"
		)
	]
	protected bool m_bIgnoreNorth;
	
	[
		Attribute
		(
			category: "Context",
			desc: "Allows to ignore the East side of the map when generating outside terrain",
			uiwidget: UIWidgets.CheckBox,
			defvalue: "0"
		)
	]
	protected bool m_bIgnoreEast;
	
	[
		Attribute
		(
			category: "Context",
			desc: "Allows to ignore the South side of the map when generating outside terrain",
			uiwidget: UIWidgets.CheckBox,
			defvalue: "0"
		)
	]
	protected bool m_bIgnoreSouth;
	
	[
		Attribute
		(
			category: "Context",
			desc: "Allows to ignore the West side of the map when generating outside terrain",
			uiwidget: UIWidgets.CheckBox,
			defvalue: "0"
		)
	]
	protected bool m_bIgnoreWest;
	
	[
		Attribute
		(
			category: "Context",
			desc: "Allows to ignore the North-West side of the map when generating outside terrain",
			uiwidget: UIWidgets.CheckBox,
			defvalue: "0"
		)
	]
	protected bool m_bIgnoreNorthWest;
	
	[
		Attribute
		(
			category: "Context",
			desc: "Allows to ignore the North-East side of the map when generating outside terrain",
			uiwidget: UIWidgets.CheckBox,
			defvalue: "0"
		)
	]
	protected bool m_bIgnoreNorthEast;
	
	[
		Attribute
		(
			category: "Context",
			desc: "Allows to ignore the South-West side of the map when generating outside terrain",
			uiwidget: UIWidgets.CheckBox,
			defvalue: "0"
		)
	]
	protected bool m_bIgnoreSouthWest;
	
	[
		Attribute
		(
			category: "Context",
			desc: "Allows to ignore the South-East side of the map when generating outside terrain",
			uiwidget: UIWidgets.CheckBox,
			defvalue: "0"
		)
	]
	protected bool m_bIgnoreSouthEast;
	
	// Button: Generate
	
	[ButtonAttribute("Generate")]
	void Generate()
	{
		// Create instance of outside terrain manager
		
		OTT_OutsideTerrainChunkOptions outsideTerrainChunkOptions = new OTT_OutsideTerrainChunkOptions(
			entityNameTemplate: m_sEntityNameTemplate,
			material: m_ChunksMaterial,
			physicsLayerPreset: m_ePhysicsLayerPreset,
			positionOffset: m_vChunksPositionOffset
		);
		
		OTT_OutsideTerrainManager outsideTerrainManager = new OTT_OutsideTerrainManager(
			worldEditorAPI: m_API,
			chunkOptions: outsideTerrainChunkOptions
		);
		
		outsideTerrainManager.Initialize(m_sLayerNameTemplate);
		
		if (!outsideTerrainManager.IsValid())
		{
			Print("Failed to create instance of outside terrain manager", LogLevel.ERROR);
			return;
		}
		
		// Create instance of outside terrain generator with factory
		
		array<ref OTT_HeightmapModifier> heightmapModifiers = {};
		
		if (m_bEnableNoiseModifier)
		{
			OTT_NoiseModifier noiseModifier = new OTT_NoiseModifier(m_eNoiseAlgorithm);
			
			noiseModifier.SetSeed(m_sNoiseSeed);
			noiseModifier.SetForce(m_fNoiseForce);
			
			heightmapModifiers.Insert(noiseModifier);
		}
		
		if (m_bEnableSmoothingModifier)
		{
			heightmapModifiers.Insert(new OTT_SmoothingModifier(m_iSmoothingIterations));
		}
		
		array<OTT_CardinalDirections> ignoreDirections = {};
		
		if (m_bIgnoreNorth) ignoreDirections.Insert(OTT_CardinalDirections.North);
		if (m_bIgnoreEast ) ignoreDirections.Insert(OTT_CardinalDirections.East);
		if (m_bIgnoreSouth) ignoreDirections.Insert(OTT_CardinalDirections.South);
		if (m_bIgnoreWest ) ignoreDirections.Insert(OTT_CardinalDirections.West);
		
		if (m_bIgnoreNorthWest) ignoreDirections.Insert(OTT_CardinalDirections.NorthWest);
		if (m_bIgnoreNorthEast) ignoreDirections.Insert(OTT_CardinalDirections.NorthEast);
		if (m_bIgnoreSouthEast) ignoreDirections.Insert(OTT_CardinalDirections.SouthEast);
		if (m_bIgnoreSouthWest) ignoreDirections.Insert(OTT_CardinalDirections.SouthWest);
		
		OTT_OutsideTerrainContextOptions contextOptions = new OTT_OutsideTerrainContextOptions(
			trackOceanLevel: m_bUseOceanLevel,
			ignoreDirections: ignoreDirections
		);
		
		OTT_OutsideTerrainGeneratorOptions generatorOptions = new OTT_OutsideTerrainGeneratorOptions(
			size: m_eOutsideTerrainSize,
			quality: m_eOutsideTerrainQuality,
			blocksPerTileWidth: m_iBlocksPerTileWidth,
			blocksPerTileHeight: m_iBlocksPerTileHeight,
			physicsType: m_ePhysicsType,
			modifiers: heightmapModifiers,
			contextOptions: contextOptions,
			depthOffset: m_fChunksDepthOffset,
			borderHeightMultiplier: m_fBorderHeightMultiplier
		);
		
		OTT_OutsideTerrainGenerator generator = OTT_OutsideTerrainGeneratorFactory.Create(
			type: m_eOutsideTerrainGeneratorType,
			options: generatorOptions,
			manager: outsideTerrainManager
		);
		
		if (!generator)
		{
			Print("Failed to create instance of outside terrain generator", LogLevel.ERROR);
			return;
		}
		
		// Generating outside terrain with generator
		
		m_API.BeginEntityAction();
		OTT_OutsideTerrainGenerationResult result = generator.Execute();
		m_API.EndEntityAction();
		
		// Processing outside terrain generation results
		
		if (!result)
		{
			Print("Failed to complete outside terrain generation", LogLevel.ERROR);
			return;
		}
		
		// Creating and displaying generation report
		
		OTT_OutsideTerrainGenerationReport report = new OTT_OutsideTerrainGenerationReport(result);
		report.Display();
	}
}

#endif