#ifdef WORKBENCH

[WorkbenchToolAttribute(name: "Outside Terrain Tool", awesomeFontCode: 0xF1FE)]
class OTT_OutsideTerrainTool : WorldEditorTool
{
	[Attribute(category: "Noise Modifier", defvalue: "1", uiwidget: UIWidgets.CheckBox, desc: "Enable noise modifier for all outside terrain heightmaps")]
	protected bool m_bEnableNoiseModifier;
	
	[Attribute(category: "Noise Modifier", defvalue: "0", uiwidget: UIWidgets.ComboBox, enums: ParamEnumArray.FromEnum(OTT_ENoiseAlgorithm))]
	protected OTT_ENoiseAlgorithm m_eNoiseAlgorithm;
	
	[Attribute(category: "Noise Modifier", defvalue: "Hello, outside terrain!", desc: "String will be converted to numeric seed for noise modifier")]
	protected string m_sNoiseModifierSeed;
	
	[ButtonAttribute("Generate")]
	void Generate()
	{
		
	}
}

#endif