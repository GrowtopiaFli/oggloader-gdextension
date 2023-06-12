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

# Info
This project was aimed to be used as a **GDExtension Library**.

## The Problem
- Godot 4 has removed the ability to directly load a `PackedByteArray` onto an `AudioStreamOGGVorbis` instance.

## How The Loading Works
- Based on Godot's source code, the `load` function is a dynamic function that calls `ResourceImporter` -> `ResourceFormatImporter`.
- It handles some extra stuff before it is detected as an OGG file, then it calls `resource_importer_ogg_vorbis`.
- Afterwards, it decodes it with the OGG library from Xiph via a C++ Byte Array from reading a file.
- Then it gives the data to an `OGGPacketSequence` before being returned as an `AudioStreamOGGVorbis` instance.
<img src="img/code1.svg" width="50%" height="50%">

## Solution
- To process the `PackedByteArray` and get the `granule_positions` and `packet_data` for the `OGGPacketSequence`.

## Extras
- I also added a feature to read the metadata of the OGG file. `[sample_rate, bitrate, artist, title...]`

# Usage
