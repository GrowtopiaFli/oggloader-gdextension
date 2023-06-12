![oggloader logo](img/OggLoader.png) \
![oggloader badge1](https://img.shields.io/github/release-date/GrowtopiaFli/oggloader-gdextension?style=for-the-badge)
![oggloader badge2](https://img.shields.io/github/last-commit/GrowtopiaFli/oggloader-gdextension/master?style=for-the-badge) \
`OggLoader` is meant to be used with the `OggPacketSequence` class in Godot.

# Goals
- To use it for one of my projects
- To integrate external OGGs with Godot
- To integrate memory stored OGGs with Godot
- To help the Godot community

# Credits
[NathanFranke's GDExtension Template](https://github.com/nathanfranke/gdextension) \
[Xiph's Documentation](https://xiph.org/doc) \
[Godot](https://github.com/godotengine/godot/tree/37d1dfef9d81aade27ab0c56fc6b6f12f6a08045) \
[godot-cpp](https://github.com/godotengine/godot-cpp/tree/3a9118cb0dafef74773216c47a030c3a4a490747)

# Documentation
This project was aimed to be used as a **GDExtension Library**.
## How Loading Works
- Based on Godot's source code, the `load` function is a dynamic function that calls `ResourceImporter`. \
- Then it handles some extra stuff before it is detected as an OGG file which it then calls `resource_import_ogg_vorbis`. \
