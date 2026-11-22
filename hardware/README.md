# Hardware

| Folder | Content |
|---|---|
| [`nanobot-rev1`](nanobot-rev1) | REV1 (November 2022): schematic (PDF), BOM, ESP32 pin assignment |
| [`nanobot-rev2`](nanobot-rev2) | REV2 (April 2023): schematic (PDF), BOM with LCSC part numbers, ESP32 pin assignment, EAGLE 9.6 schematic and board in `source/` |
| [`mechanical`](mechanical) | 3D-printed parts (3MF / STL) |

The Gerber, drill and pick-and-place files of both revisions are attached to the [releases](https://github.com/marvinramm/NanoBot/releases).

**Build REV1.** REV2 works only partly (e.g. the reset button does not work) and is published as a reference for its power section: USB-C charger, cell balancing, undervoltage protection and battery switch.

The EAGLE source of REV2 uses parts from my own libraries; they are embedded in the `.sch` / `.brd` files, so the design opens without them.

## Pin assignment

The `*_ESP32_pinout.csv` files list every ESP32 GPIO with its function on the board. Sensors behind the analog multiplexer and the shift registers are mapped in `Pins.h` of the matching firmware ([REV2](../firmware/NanoOS/Pins.h), [REV1](../firmware/NanoOS-REV1/Pins.h)).

## License

The hardware design files, 3D models and documentation are licensed under [CC BY 4.0](LICENSE). See [LICENSING.md](../LICENSING.md) for the attribution text.
