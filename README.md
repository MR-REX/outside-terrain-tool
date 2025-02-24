# Outside Terrain Tool

**Outside Terrain Tool** is a developer tool for **Arma Reforger Tools** that integrates directly into the World Editor,
allowing you to generate outside terrain with just a few clicks. This tool simplifies the process of creating
expansive, seamless (outside) terrain beyond the boundaries of the main map, using the heightmap of the existing world
terrain as a foundation.

> [!IMPORTANT]
> This tool is designed to work hand-in-hand with the [Outside Terrain Core](https://github.com/MR-REX/outside-terrain-core),
> which renders the generated terrain in-game. Together, they provide a complete solution for creating and displaying
> outside terrain in Arma Reforger.

## Features

- **Multiple Generation Types.** Choose from various outside terrain generation methods to suit your project needs.
- **Heightmap-Based Generation.** Generate outside terrain based on the heightmap of the main (world) terrain.
- **Customizable Size and Quality.** Adjust the size and quality of the generated terrain to balance performance and detail.
- **Physical Properties Customization.** Configure physical properties such as `Range` and `Layer Preset` for the generated terrain.
- **Heightmap Modifiers.** Apply intermediate modifiers like `Noise` and `Smoothing` to fine-tune the outside terrain's heightmap.
- **Ocean Level and Cardinal Directions.** Generate terrain based on the `Ocean Level` or manually select cardinal directions for terrain generation.

## Usage

### Generating Outside Terrain

1. Add the **Outside Terrain Core** as a dependency to your project and run it together with the **Outside Terrain Tool**.
2. Open the **World Editor** in Arma Reforger Tools.
3. Select the **Outside Terrain Tool** from the tools menu.
4. Configure the generation settings:
    - Choose the `Outside Terrain Generator Type`.
    - Adjust the size, quality, and physical properties of the outside terrain.
    - Apply modifiers like `Noise` or `Smoothing` to the outside terrain heightmap.
    - Select whether to generate terrain based on the `Ocean Level` or manually specify cardinal directions.
5. Click **Generate** to create the outside terrain.

## How It Works

The **Outside Terrain Tool** uses the heightmap of the main terrain to create a seamless extension of the landscape. By applying modifiers
like Noise and Smoothing, you can create varied and realistic terrain features. The generated outside terrain is saved as entity chunks
at selected entity layer, which can be rendered and interacted with in-game using the **Outside Terrain Core** addon.

## License

This project is licensed under the **Arma Public License Share Alike (APL-SA)**. See the
[LICENSE](https://github.com/MR-REX/outside-terrain-tool/blob/main/LICENSE) file for details.

## Acknowledgments

A special thanks to **Bohemia Interactive** for creating Arma Reforger.

## Support and Feedback

If you encounter any issues or have suggestions for improvement, please
[open an issue](https://github.com/MR-REX/outside-terrain-tool/issues) on GitHub.

## Links

- [Arma Reforger Workshop Page](https://reforger.armaplatform.com/workshop/61F2B96F0D3F27D4-OutsideTerrainTool)
- [Outside Terrain Core](https://github.com/MR-REX/outside-terrain-core)