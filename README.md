# MyDS

MyDS is my very own custom Nintendo DS emulator for Windows.

I wanted to challenge myself in C++ and in something very challenging. WELL I AM SERVED.<br/>
I wanted to do something like this without looking at the source code of famous opensource DS emulator.

*I've actually seen two lines of MelonDS, pretty sure the challenge is ruined...*

## Compilation

- Download Visual Studio 2022 Community
- Clone this repo
- Open the folder with VS2022
- Cry / Laugh

## Usage

<!-- Please, don't use this...<br/> -->
This emulator is not in a working state.<br/>
If you want a proper DS emulator, use [MelonDS](https://github.com/melonDS-emu/melonDS), [DeSmuMe](https://github.com/TASEmulators/desmume) or [no$gba](https://www.nogba.com/).

## Roadmap

- CPU emulation : WIP (10% - very basic instruction set implemented)
  - *Instruction cycles are running at 22MHz max for ARM9... performance improvement will be necessary at some point*
- Display - basic color framebuffer : TBD
- Input - buttons & touchscreen : TBD

## Contributing

Contribution are not expected on this project for the moment.<br/>
At some point, issues will be open for people to give advice on implementation, but that's probably all.

## Ressources

- Rodrigo Copetti's articles on the [GBA](https://www.copetti.org/writings/consoles/game-boy-advance/) and [NDS](https://www.copetti.org/writings/consoles/nintendo-ds/) architectures
- Martin Korth's [Technical references for GBA/NDS/DSI/3DS](https://problemkaputt.de/gbatek.htm) from  (he is the no$gba developer)
- Tobias Langhoff's [CHIP-8 emulator guide](https://tobiasvl.github.io/blog/write-a-chip-8-emulator/)
- [ARMv5 architecture reference](https://developer.arm.com/documentation/ddi0100/i/?lang=en) for the CPU instructions set

For testing, I am (or will be) using :
- no$gba debug version for developpers : https://www.nogba.com/no$gba-download.htm
- [TinyNDS](https://imrannazar.com/The-Smallest-NDS-File) : the smallest NDS file for testing
- Atem2069's [fixed version of Armwrestler](https://github.com/Atem2069/armwrestler-fixed/) (original version [here](https://github.com/mic-/armwrestler)) : test rom for ARM instructions set, with a version for NDS
- RockPolish's [Rockwrestler](https://github.com/RockPolish/rockwrestler) : another NDS test rom for ARM instructions set

## License

[MIT](https://choosealicense.com/licenses/mit/)