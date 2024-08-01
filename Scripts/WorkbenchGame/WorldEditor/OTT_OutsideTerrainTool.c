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
			//defvalue: OTT_EOutsideTerrainGeneratorTypes..ToString()
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
			defvalue: OTT_EOutsideTerrainSize.Normal.ToString()
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
			defvalue: OTT_EOutsideTerrainQuality.High.ToString()
		)
	]
	protected OTT_EOutsideTerrainQuality m_eOutsideTerrainQuality;
	
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
			defvalue: "1"
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
	
	// Category: Smoothing Modifier
	
	[
		Attribute
		(
			category: "Smoothing Modifier",
			desc: "Enable smoothing modifier for all outside terrain heightmaps",
			uiwidget: UIWidgets.CheckBox,
			defvalue: "1"
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
			defvalue: "OutsideTerrainChunk_%1"
		)
	]
	protected string m_sEntityNameTemplate;
	
	// Internal class fields
	
	protected string m_sReportTemplate = "Total number of chunks: %1\nTotal number of verticles: %2\nTotal number of triangles: %3";
	
	// Button: Generate
	
	[ButtonAttribute("Generate")]
	void Generate()
	{
		// Create instance of outside terrain manager
		
		OTT_OutsideTerrainChunkOptions outsideTerrainChunkOptions = new OTT_OutsideTerrainChunkOptions(
			entityNameTemplate: m_sEntityNameTemplate,
			material: m_ChunksMaterial,
			physicsLayerPreset: m_ePhysicsLayerPreset
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
			
			heightmapModifiers.Insert(noiseModifier);
		}
		
		if (m_bEnableSmoothingModifier)
		{
			heightmapModifiers.Insert(new OTT_SmoothingModifier(m_iSmoothingIterations));
		}
		
		OTT_OutsideTerrainGeneratorOptions generatorOptions = new OTT_OutsideTerrainGeneratorOptions(
			size: m_eOutsideTerrainSize,
			quality: m_eOutsideTerrainQuality,
			physicsType: m_ePhysicsType,
			modifiers: heightmapModifiers
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
		
		string report = string.Format(m_sReportTemplate, result.GetChunksCount(), result.GetVerticlesCount(), result.GetTrianglesCount());
		Workbench.Dialog("Outside terrain generated", report);
	}
}

#endif