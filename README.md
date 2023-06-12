![oggloader logo](img/OggLoader.svg) \
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

# Setup
- Download the library from the [Releases](https://github.com/GrowtopiaFli/oggloader-gdextension/releases).
- Extract it to your Godot project. (I recommend extracting the `oggloader` folder into the `addons` folder of your project)
- Make sure there aren't any errors when you open your project.

# API Usage
The library will create a singleton named `OggLoader` which you can directly call in your `.gd` files.
## OggLoader.proc
<img src="img/code2.svg" width="50%" height="50%"> \
- This function processes the `PackedByteArray` and caches it with an **ID<-(IMPORTANT)**
- It caches the `granule_positions` and `packet_data` for an `OGGPacketSequence`.
- It returns a boolean. **True** if it was successful and **False** if it failed.
- Example: \
  <img src="img/code3.svg" width="50%" height="50%">
- Note: I recommend you to check if it is cached first before calling this function so that it won't perform unnecessary things by doing `OggLoader.cached`. (Also, after you called proc and are done getting the `granule_positions` and `packet_data` variables, please uncache it if you don't want it to waste memory via `OggLoader.remove`)

## OggLoader.cached
<img src="img/code4.svg" width="50%" height="50%"> \
- This function checks if something was cached through an **ID**.
- It returns a boolean. **True** if the specified ID is in the cache and **False** if it is not.
- Example: \
  <img src="img/code5.svg" width="50%" height="50%">

## OggLoader.get_granule_positions
<img src="img/code6.svg" width="50%" height="50%"> \
- This function searches the cache via the given **ID** then returns the `granule_positions`.
- Example: \
  <img src="img/code7.svg" width="50%" height="50%">
  
## OggLoader.get_packet_data
<img src="img/code8.svg" width="50%" height="50%">\
- This function searches the cache via the given **ID** then returns the `packet_data`.
- Example: \
  <img src="img/code9.svg" width="50%" height="50%">

## OggLoader.clear
<img src="img/code10.svg" width="50%" height="50%">\
- This function clears the cache.
- Example: \
  <img src="img/code11.svg" width="50%" height="50%">

## OggLoader.remove
<img src="img/code12.svg" width="50%" height="50%">__
- This function searches the cache via the given **ID** then removes it.
- It returns a boolean. **True** if the operation was successful and **False** if it failed.
- Example: \
  <img src="img/code13.svg" width="50%" height="50%">

## OggLoader.get_ogg_metadata
