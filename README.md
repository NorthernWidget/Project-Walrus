[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.2525437.svg)](https://doi.org/10.5281/zenodo.2525437)

# The Walrus: A Pressure and Temperature Sensor for Water-Level and Barometric Measurements

![Walrus pup; photo by Max Smith](Documentation/images/WalrusPup_MaxSmith.jpg)

***Our Walrus sensor, like this Walrus pup, is equally at home on land or in the water.*** *[Photo by Max Smith](https://commons.wikimedia.org/wiki/File:Walrus_-_Kamogawa_Seaworld_-_pup_-1.jpg)*

## Overview

The **Walrus** is an open-source sensor that measures temperature and pressure. It may be used in the air as a barometer or underwater as part of a system -- combined with barometric compensation -- to obtain changes in water level.

![Walrus HardMount v0.2.0](Documentation/images/Walrus_HardMount_v020_top_annotated_20200427.png)
***Hard-mount variant of the Walrus.*** Major components are labeled. The pads to attach the watertight plug for the cable (i.e. the "HardMount"), on left, are currently unpopulated. These may be used for this kind of connector, or may have wires soldered directly to them before being embedded in an epoxy or other potting (i.e., waterproofing and encapsulating) compound.

### Technical specifications

#### Size

Dimensions: 42.0 x 12.7 cm

>> ADD MASS once I get access to a lab balance again <<

#### Operating ranges

**Power**
* Input Voltage: 3.3 to 5.5 V DC
* Logic Voltage: 0 to Vcc (Will operate at same logic level as input power supply)

**Sensors**
* Temperature: -40 to +85 C
* Pressure, typical: 0 to 2 atm (i.e. barometer to 10 m water depth)
* Pressure, maximum, untested: 0 to 30 atm (i.e. barometer to 290 m water depth)
* Cable length: 3 m (I2C) or 1000 m (RS-485).

The pressure transducer contains an internal temperature sensor, and a secondary (and more accurate) external temperature sensor is also mounted on the *Walrus*. This external sensor is the [MCP9808](http://www.microchip.com/mymicrochip/filehandler.aspx?ddocname=en556196) in an 8-MSOP package. This sensor has a range of -40 to 125 C.

RS-485 communications allow for a signal to be transmitted over long distances, but require a device that can decode this signal, such as our [Project-Longbow](https://github.com/NorthernWidget-Skunkworks/Project-Longbow).

#### MS5803 pressure sensor variants

The pressure operating range and resolution depend on the MS5803 variant installed. All variants use the same footprint and firmware (with appropriate coefficient selection).

| Variant | Application | Range | Resolution @ OSR=4096 | Water depth equiv. |
|---------|-------------|-------|-----------------------|--------------------|
| [MS5803-01BA](https://www.te.com/commerce/DocumentDelivery/DDEController?Action=srchrtrv&DocNm=MS5803-01BA&DocType=Data+Sheet&DocLang=English) | Barometer | 10–1300 mbar | ~0.012 mbar | ~0.12 mm |
| [MS5803-02BA](https://www.te.com/commerce/DocumentDelivery/DDEController?Action=srchrtrv&DocNm=MS5803-02BA&DocType=Data+Sheet&DocLang=English) | Shallow water | 0–2 bar | ~0.020 mbar | ~0.2 mm |
| [MS5803-05BA](https://www.te.com/commerce/DocumentDelivery/DDEController?Action=srchrtrv&DocNm=MS5803-05BA&DocType=Data+Sheet&DocLang=English) | Medium depth | 0–6 bar | ~0.050 mbar | ~0.5 mm |
| [MS5803-14BA](https://www.te.com/commerce/DocumentDelivery/DDEController?Action=srchrtrv&DocNm=MS5803-14BA&DocType=Data+Sheet&DocLang=English) | Deep water | 0–14 bar | ~0.200 mbar | ~2 mm |

The **MS5803-02BA** is the standard variant (pressure measurements to ~10 m water depth). The **MS5803-01BA** converts the Walrus into a high-resolution barometer. 1 mbar ≈ 1 cm of freshwater at standard conditions.

Pressure values are stored in the I2C register map as µBar (int32). All variants fit within int32 range (~2.1 billion µBar max).

#### Resolution

Pressure: see MS5803 variant table above
Temperature sensor within pressure transducer (typical): +/- 2.0 C
External temperature sensor (typical): +/- 0.5 C


## Circuit board and assembly

The Walrus comes in two forms:
* [Large form factor, through-hole mount](https://github.com/NorthernWidget-Skunkworks/Project-TPDH-Walrus/tree/master/Hardware_LargePTH): This is the legacy board with support for I2C and full-duplex RS-485 communication
* [Smaller form factor, solder pads for wires or a waterproof plug](https://github.com/NorthernWidget-Skunkworks/Project-TPDH-Walrus/tree/master/Hardware_HardMount): This current version makes better use of board space and utilizes a half-duplex RS-485 bus in addition to an I2C bus.

### Files

The files for production are located in folders starting with the word "Hardware". These folders contain:

* "\*.brd" and "\*.sch" files. These are the circuit board and its schematic for use with the EAGLE CAD software, which you can download for free: https://www.autodesk.com/products/eagle/free-download
* A "\*.csv" file that contains the "BoM" -- the bill of materials. This lists all required parts to construct the board
* A ZIP archive containing a set of "Gerber" files. These are the files that machines use to create a circuit board and a stencil to help you to apply solder paste (see below)

### Circuit board specifications

* 2-layer copper
* Minimum trace width: 0.2 mm
* Single-sided
* Surface mount components

All Walrus boards are **single-sided** and use only **surface-mount components** for rapid fabrication using standard equipment.

### Producing circuit boards

We strongly recommend that circuit boards be printed by a reputible manufacturer. Many such industries exist in China, India, and developing or developed countries.

### Populating circuit boards

Before you can start to populate the circuit board, meaning adding components to it, you will need solder paste and a stencil. If you have access to a laser cutter, you can create your own stencil (https://learn.adafruit.com/smt-manufacturing/laser-cut-stencils). If you do not, the company that prints the circuit boards should be able to provide one. If all else fails, you can try to cut out a stencil with a small knife, or use a solder-paste syringe, though both of these methods can lead to difficulties (such as bridged connections). Additional information on how to stencil can be found here: https://www.sparkfun.com/tutorials/58.

To help you in using the stencil, we provide STL files for 3D-printable plates in each of the hardware directories. These plates hold the board in place and provide a level surface on which to use the stencil.

>> ADD PLATE FOR HARDMOUNT <<

After this, you can use a tweezers and steady hands, or a pick-and-place machine, to gently place each component over the right pads, and in the right orientation. Do not worry if they are slightly misaligned; the surface tension of the solderi, once it is melted, will help to rotate the components into place.

### Reflowing circuit boards

"Reflowing" is the process of heating solder until it flows and attaches to components. A basic introduction is here: https://learn.sparkfun.com/tutorials/electronics-assembly/reflow

There are four major ways to reflow:
1. Using a professional oven (see link above)
2. With a customized toaster oven: See https://www.sparkfun.com/tutorials/60, and a basic internet search will show you many, many other ideas
3. You can also use a simple electrical skillet -- the kind that you might use to make some food. https://www.sparkfun.com/tutorials/59
4. Using a hot-air rework station; this is a bit time-intensive, but lets you control the heat, and I often use it for small boards. https://learn.sparkfun.com/tutorials/how-to-use-a-hot-air-rework-station/all

After reflowing, you may need to clean up jumped connections using **solder wick**.

### Debugging circuit boards

After reflow is complete, check if everything worked. Do you have any jumped connections? Any remaining solder paste that hasn't been reflowed? This can be the trickiest part, and one for which access to a multimeter is essential. An oscilloscope and a digital logical analyzer can help too. Debugging electronic circuit boards is a process that requires significant skill and practice, so be sure that you can work with someone with some expertise. I would suggest an electrical engineering student or professor at a local university.


## Wiring and pinout

"Pin" is the name of the pin as labeled on the board. "Wire" is the color of the [Alpha Wire 5004C cable](https://www.digikey.com/product-detail/en/alpha-wire/5004C-SL001/5004CSL001-ND/484976) that we use; you may use something different based on what manufacturers exist where you are; that's okay, just record the color convention! (We sometimes use another cable in which we use a white wire for SDA and a green wire for SCL, but have found that Alpha Wire brand works best.)

>> Add image of cable ends (optional) and find how to integrate with housing section; cabling is a distinct topic, but the step of attaching the cable lies within the housing-construction step <<

### HardMount with plug

Here we assume that you are using standard Alpha Wire cables with waterproof attachments, available in [3-meter](https://www.digikey.com/en/products/detail/alpha-wire/AR0400101-SL357/6555492) and [1-meter](https://www.digikey.com/en/products/detail/alpha-wire/AR0400101-SL356/6555491) lengths; the 3-meter length is our standard because this is the longest that it can be and still communicate reliably via I2C. Because of the wire insulation colors available for these cables and the locations of their attachment points to the [4-pin "HardMount" plug](https://www.digikey.com/product-detail/en/te-connectivity-amp-connectors/1838893-2/A97650-ND/1764165) ([3D model](https://www.traceparts.com/en/product/allied-electronics-automation-m12-panel-mnt-male-4-way?CatalogPath=TRACEPARTS%3ATP09002002001004&Product=10-12012018-106023&PartNumber=1838893-2)), **our HardMount devices do NOT match standard wire-color definitions (or any kind of standard)**. Wire-color definitions are:

| **Color** | **Connection**        |
|-----------|-----------------------|
| Brown     | V+ (3.3-5.5 V)        |
| White     | GND                   |
| Black     | SIG1: SDA or RS-485 A |
| Blue      | SIG2: SCL or RS-485 B |

### Large-form-factor/PTH design: standard wiring suggestion

*These are our more standard wire-color suggestions*

| **Color** | **Connection**                   |
|-----------|----------------------------------|
| Red       | V+ (3.3-5.5 V)                   |
| Black     | GND                              |
| Brown     | SIG1: SDA or RS-485 A (RX+, TX+) |
| Orange    | SIG2: SCL or RS-485 B (RX-, TX-) |

If you attach your own wire, pins on the Walrus are as follows, looking at the top side with the screw hole away from you:
* Top left: SIG1: SDA for I2C; A (+) for RS-485
* Top right: SIG 2: SCL for I2C; B (-) for RS-485
* Bottom left: VCC (3.3V to 5.5V)
* Bottom right: GND

![HardMount attachment points](Documentation/images/Walrus_HardMount_v020_top_HardMount_annotated_20200601.png)


## Upload the firmware

### Downloading and installing the Arduino IDE

Go to https://www.arduino.cc/en/main/software. Choose the proper IDE version for your computer. For Windows, we suggest the non-app version to have more control over Arduino; this might change in the future. You will have to add custom libraries, so the web version will not work (at least, as of the time of writing). Download and install the Arduino IDE. Open it to begin the next steps.

### AVR ISP

To install firmware on the Walrus, you use the [2x3-pin ICSP (also called ISP) header](https://www.digikey.com/product-detail/en/3m/929665-09-03-I/3M156313-06-ND/681796) with a special device called an "in-circuit system programmer" (or just "in-system programmer; yup, that's what the acronym stands for).

Many devices exist to upload firmware, including:
* The official [AVR ISP mkII](http://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-42093-AVR-ISP-mkII_UserGuide.pdf) (no longer produced but available used)
* Using an [Arduino as an ISP](https://www.arduino.cc/en/tutorial/arduinoISP)
* The versatile [Olimex AVR-ISP-MK2](https://www.olimex.com/Products/AVR/Programmers/AVR-ISP-MK2/open-source-hardware)
* The [Adafruit USBtinyISP](https://www.adafruit.com/product/46)

***Note: Be sure to download and/or update drivers for your ISP.***

### Uploading the firmware

Using an AVR ISP, upload the proper firmware programs from the ["Firmware" directory](Firmware) in this repository using the Arduino software:
* Large form factor PTH board
  * I2C mode: No firmware required, but fuses must be set (see instructions, below)
  * RS-485 mode: [Longbow_2BA_0x6B](Firmware/Longbow_2BA_0x6B) (This firmware is for the 02BA; you will need to switch these with other coefficients, available in the commented and uncommented lines of /Firmware/Walrus_I2C_5BA, to modify this for a different transducer pressure range.)
* Small form factor hard-mount board
  * I2C mode: Set the fuses (see instructions below) and then upload [Walrus_I2C_5BA](/Firmware/Walrus_I2C_5BA/) (This firmware is for the 05BA; there are lines in this code to comment and uncomment to modify it for other versions of the Walrus.)
  * RS-485 mode: [Longbow_2BA_0x6B_HalfDuplex](Firmware/Longbow_2BA_0x6B_HalfDuplex) (This firmware is for the 02BA; you will need to switch these with other coefficients, available in the commented and uncommented lines of /Firmware/Walrus_I2C_5BA, to modify this for a different transducer pressure range.)

>> @bschulz1701: could you provide a link to the repo (is it TPDH lib) for "  * I2C mode: No firmware required. Library communicates directly with sensors via their built-in I2C addresses."

**Instructions:**

1. Open the Arduino IDE.
2. Follow [these instructions](https://github.com/SpenceKonde/ATTinyCore/blob/master/Installation.md) to install the [ATTinyCore board definitions](https://github.com/SpenceKonde/ATTinyCore)
3. Select [ATTiny1634](https://github.com/SpenceKonde/ATTinyCore/blob/master/avr/extras/ATtiny_1634.md) **(No bootloader)**
4. Plug your ISP of choice into your computer (via a USB cable) and onto the 6-pin header. There are two ways to place it on; the header is aligned such that the ribbon cable should be facing away from the board while programming. If this fails without being able to upload, try flipping the header around. This should both power the board and provide communications.
5. Go to Tools --> Programmer and select the appropriate programmer based on what you are using.
6. Go to Tools --> Burn Bootloader. Yes, we know that you just selected "ATTiny1634 (No bootloader)", but this step sets the fuses, which configure their internal oscillator and brown-out detection.
7. Unless you are configuring a large-form-factor Walrus in I2C mode (in which case you are now done), open the appropriate firmware (linked above). Then go to Sketch --> Upload Using Programmer. After several seconds, you learn whether you succeeded or failed. Hopefully it worked!

![Upload using programmer](https://media.githubusercontent.com/media/NorthernWidget-Skunkworks/Project-Symbiont-LiDAR/master/Documentation/images/UploadUsingProgrammer.png)

***Uploading using the in-system programmer.***

***Important note for Linux users:*** You must supply permissions to the Arduino IDE for it to be able to use the ICSP, or you will have to run it using `sudo`. The former option is better; the latter is easier in the moment.


## Register map and firmware internals

The Walrus firmware runs on an ATtiny1634 microcontroller and exposes an I2C register map to the host logger. The default I2C address is `0x57` (Schema 1 `'W'`; the address stored in Page 0 byte `0x1F` overrides it).

The firmware on `master` implements [NW-Device-Specification](https://github.com/NorthernWidget/NW-Device-Specification) Schema 1 (firmware patch 1, 2026-09-23, unreleased and not yet validated on hardware: #18). The layout that the last released firmware exposed is kept below for anyone reading a deployed unit.

### Legacy register map (firmware before Schema 1)

26-byte array. Identity fields (MODEL, GROUPID, INDID, FIRMWAREID) mixed into sensor data. Status ready flag is bit 7.

```
0x00        CTRL/Status   bit 7=ready; bits 1:0=update rate selector (0–3)
0x01        —             unused
0x02–0x05   Pressure      int32, µBar (library: val/1000.0 → mBar)
0x06–0x09   Temp ext      int32, 1/10000 °C (MCP9808 external sensor)
0x0A–0x0D   Temp MS5803   int32, 1/10000 °C (MS5803 internal sensor)
0x0E–0x0F   MODEL         uint16 (0x5702)
0x10–0x11   —             gap (nothing loaded)
0x12–0x13   GROUPID       uint16
0x14–0x15   INDID         uint16
0x16–0x17   FIRMWAREID    uint16 (0x0001)
```

### Register map (NW-Device-Specification Schema 1)

Two 32-byte pages. Identity lives entirely in Page 0 (EEPROM, written by [NW-Provision](https://github.com/NorthernWidget/NW-Provision); the firmware copies it to the register array at boot, checks the CRC, and substitutes its own patch version at `0x0A`). Sensor data in Page 1 (SRAM). No calibration page (MS5803 coefficients are read from its internal PROM at startup). A controller sets a start register with a one-byte write and then reads up to 32 bytes with auto-increment.

**Page 0 (0x00–0x1F) — Identity (EEPROM)**

```
Block 0 (0x00–0x07)   Core identity
  0x00        0x01              Schema (NW-Device-Specification v1)
  0x01–0x06   'W','a','l','r','u','s'   Device name (ASCII)
  0x07        0x00              Null padding

Block 1 (0x08–0x0F)   Version
  0x08        HW major
  0x09        HW minor
  0x0A        FW patch          (NW combined-repo convention)
  0x0B–0x0D   0x00,0x00,0x00    Unused (combined repo)
  0x0E–0x0F   0x00,0x00         Reserved

Block 2 (0x10–0x17)   Serial number
  0x10–0x11   0x5702            Board type ('W'=0x57, revision index 2; matches MODEL)
  0x12–0x13   [manufacture]     Group ID
  0x14–0x15   [manufacture]     Unique ID
  0x16–0x17   0x00,0x00         FirmwareID (legacy, reserved)

Block 3 (0x18–0x1F)   Integrity + administration
  0x18–0x1C   0x00 ×5           Reserved
  0x1D        0x4E              Magic byte
  0x1E        [computed]        CRC-8/SMBUS of bytes 0x00–0x1D
  0x1F        0x57              I2C address (writable over I2C; persisted to EEPROM; 0xFF = use default)
```

**Page 1 (0x20–0x3F) — Sensor data (SRAM)**

Chip table:

| Index | Chip | Measurements |
|-------|------|--------------|
| 0 | MS5803 | pressure, temperature |
| 1 | MCP9808 | external (water) temperature |

Block 0 (0x20–0x27) is the universal block defined by [NW-Device-Specification](https://github.com/NorthernWidget/NW-Device-Specification#page-1--sensor-data). On Walrus: a reading starts on a trigger (Control `0x21` bit 0) or on the free-running timer that Config `0x26` bits 1:0 select (0 = 5 s, 1 = 10 s, 2 = 60 s, 3 = 300 s); Control bit 1 selects the MS5803 and bit 2 the MCP9808; ready (Status `0x20` bit 0) clears while the chips are read and returns with the reading counter (`0x22–0x23`) incremented; a chip that does not acknowledge sets its status bit (bit 1 MS5803, bit 2 MCP9808, bit 7 summary) and latches kind 1 in the fault byte `0x27`, which the next Control write clears; boot latches unit kind 6 (reset), or kind 3 if Page 0 failed its CRC. The readings-requested word (`0x24–0x25`) is accepted but changes nothing: no chip on Walrus is powered per batch. Sleep (Control bit 7) is accepted and ignored. The main loop polls every 100 ms, so a trigger is answered within about 100 ms plus the MS5803 conversion time: status (ready, per-chip fault bits, pan-fault), control (trigger, chip select, sleep), reading counter, device config byte at 0x26, latched fault code at 0x27. Device data begins at 0x28. Config (0x26): bits 1:0 = free-running update period, 0 = 5 s, 1 = 10 s, 2 = 60 s, 3 = 300 s; bits 7:2 reserved.

```
Block 1 (0x28–0x2F)   MS5803 — pressure + temperature
  0x28–0x2B   Pressure     int32, µBar, little-endian
  0x2C–0x2D   Temp MS5803  int16, 0.01 °C, little-endian
  0x2E–0x2F   Reserved

Block 2 (0x30–0x37)   External temperature sensor (MCP9808)
  0x30–0x31   Temp ext     int16, 0.01 °C, little-endian
  0x32–0x37   Reserved

Block 3 (0x38–0x3F)   Reserved
```

## Housing

### Large-form-factor PTH design

![Walrus PTH v0.0.0 with enclosure](Documentation/images/Walrus_PTH_v000_with_enclosure_and_scale_cleaned_background_20200428.png)

***Walrus v0.0.0 PTH and its housing.*** *Left: 3D-printed top of the housing; the slits are to prevent large material from hitting the sensor. Center: the bottom of the housing. Self-tapping holes are to screw the board onto this housing. The cable entering the housing through a tightly-fitting hole on the "south" side of the image has not yet been stripped to expose the ends of the individual wires. A different variant of the housing is longer to allow the user to strip more of the cable if desired while maintaining the cable jacketing in contact with the hole. Top right: Walrus v0.0.0. This fully-functional initial version is significantly larger than the current unit. Note the pressure transducer (white circle) at top left, and the fact that this has pins and hardware for a full-duplex RS-485 connection. Bottom right: U.S. penny for scale; 19.05 mm (0.75 inch) diameter.*

### HardMount design

![Render of Walrus enclosure](Documentation/images/HardMount_withEnclosure_3Dperspective_CAD.png)
***Render of the enclosure design.*** *The epoxy encapsulation tray sits within a wider cylinder containing a screen designed to keep sediment out. Design still in progress (19 June 2020).*

### How to build

#### Materials and components required

##### All models

* 3D printer filament: ABS suggested
* Two-part epoxy: [3M DP270 Clear](https://multimedia.3m.com/mws/media/66773O/3mtm-scotch-weldtm-epoxy-potting-compound-adhesive-dp270.pdf); can be purchased from [McMaster-Carr](https://www.mcmaster.com/7467A274)
* Conformal coating to help protect components during epoxy encapsulation. We suggest [MG 419D](https://www.mgchemicals.com/products/conformal-coatings/acrylic-conformal-coating-419d) ([Digi-Key purchase link](https://www.digikey.com/product-detail/en/mg-chemicals/419D-55ML/473-1388-ND)). If you prefer a silicone conformal coat, we suggest [MG 422C](https://www.mgchemicals.com/products/conformal-coatings/422c-silicone-conformal-coating), though we recommend this instead for an external (rather than internal, as this will be), flexible coat.

##### HardMount with Plug

* [4-pin "HardMount" plug](https://www.digikey.com/product-detail/en/te-connectivity-amp-connectors/1838893-2/A97650-ND/1764165) ([3D model](https://www.traceparts.com/en/product/allied-electronics-automation-m12-panel-mnt-male-4-way?CatalogPath=TRACEPARTS%3ATP09002002001004&Product=10-12012018-106023&PartNumber=1838893-2))
* HardMount cable: **Alpha Wire AR0400105-SL357** (-40 PVC jacket)
  * [Alpha Wire page](http://www.Alpha Wire.com/en/Products/Connectivity/AlphaConnect/Cordsets/AR0400105)
  * [Example from DigiKey](https://www.digikey.com/product-detail/en/alpha-wire/AR0400105-SL357/AR0400105SL357-ND/6555497)
  * Comes in a [variety of lengths from 0.6 to 20 meters](http://www.Alpha Wire.com/en/Products/Connectivity/AlphaConnect/Cordsets/AR0400105) ([Digi-Key search results](https://www.digikey.com/products/en/cable-assemblies/circular-cable-assemblies/448?FV=2331%7C312745%2C2344%7C346572%2C2350%7C349292%2C2380%7C203167%2C-8%7C448%2C2345%7C1%2C2352%7C387800&quantity=0&ColumnSort=77&page=1&stock=1&rohs=1&nstock=1&k=M12+Alpha+Wire&pageSize=25&pkeyword=M12+Alpha+Wire))
  * User may choose a cable that terminates in a plug, alongside a matching recepticle in the wall of the data-logger box, for a specialized solution that does not require cable glands.
* One [#2-56 x 3/16" thread-cutting screw](https://www.fastenal.com/products/details/0143391) per board to fasten it to the housing for the epoxy encapsulation


##### PTH or "HardMount" design without plug

* 7x [\#4-24 self-tapping screws](https://www.polycase.com/screws-mbr-100) ([Additional link: untested but probably identical screw from Fastenal](https://www.fastenal.com/products/details/32244))
  * 3x to hold board in place
  * 4x to to attach the lid of the 3D-printed enclosure

*Assuming that we are using a 4-wire connection instead of the full-bridge RS-485 (supported by the large-form-factor/PTH board) because this is being phased out in favor of a 4-wire half-bridge.*

* [4-conductor Alpha Wire](https://www.digikey.com/product-detail/en/alpha-wire/5004C-SL001/5004CSL001-ND/484976), soldered to pads or rings


#### 3D printing the housing

Subfolders of the "3Dprint" directory contain "STL" files that you can use with a program called "[Slic3r](https://slic3r.org/)" to create 3-D printable parts for your housing. Assuming that you have a Prusa 3D printer, you will want to download the [Prusa variant of Slic3r](https://www.prusa3d.com/prusaslicer/). This comes with the appropriate setup files and information to help you configure the printer and print your parts.

>> Link to all housings once designed <<

>> TO DO: Options for HardMount with (and perhaps without) the plug. <<


#### Cabling

##### HardMount with Plug

Solder the HardMount plug onto the four SMD pads with the notch (for plug orientation) at the bottom side of the board. The best way to do this is to:
* Add small pillows of solder to each of the four pads, enough to create contact with the HardMount solder cups but not so much that it is not possible to slide the HardMount plug over them.
* Slide the HardMount plug on; it will take a bit of finesse to keep it from being pushed off of the solder pillows.
* Using some Blu Tack (or equivalent), fix the board and HardMount plug in place
* Double-check that the notch in the HardMount plug is on the back side of the board
* Solder on the first connection
* Check whether the board is straight; reflow the solder as needed until the connection is secure
* Solder the second connection
* Flip the board, ensuring that it is straight (if not, make changes)
* Solder the other two solder-cup connections

##### PTH or "HardMount" design without plug

Optionally, pass the cable through its hole in the 3D-printed enclosure bottom for the pour-over epoxy encapsulation. This means that you will not later need to pass the full length of the cable through the hole!

Next, solder the cable through the rings or onto the SMD pads. Ensure that all connections are solid prior to encapsulation. The encapsulation process will strain-relieve the cable.


#### Potting

>> @bschulz1701: with this epoxy, do we need to conformally coat the boards first?

After creating the housing, affix the logger to it using the \#4-24 screws listed above. If the design includes an integrated cable (instead of a HardMount plug), MAKE SURE that a cable of the desired length is attached beforehand! Otherwise, MAKE SURE that a HardMount plug is attached beforehand!

>> @superchap123: Your clip design would come in here too.

Before applying the epoxy, make sure that all zones where epoxy could leak out of the 3D-printed lower portion of the enclosure are sealed. The epoxy is somewhat viscous, so gaps need not be made completely airtight.

Next, pot the boards in 2-part epoxy (per the supplies list above, we suggest [3M DP270 Clear](https://multimedia.3m.com/mws/media/66773O/3mtm-scotch-weldtm-epoxy-potting-compound-adhesive-dp270.pdf); can be purchased from [McMaster-Carr](https://www.mcmaster.com/7467A274)). Be careful to fully cover everything except for the white inside membrane on the pressure transducer. If epoxy touches this, it is rendered unreliable or useless.

>> Walk through assembly with images <<

![Potting the large form factor Walrus](Documentation/images/Walrus_PTH_potting_2019-11-13_17.16.01.jpg)
***Potting the large-form-factor Walrus.*** *3D print designs for this lower/tray portion and the to-be-attached lid are included [within this repository in the "3Dprint" folder](https://github.com/NorthernWidget-Skunkworks/Project-Walrus/tree/master/3Dprint/HousingLargePTH).*


## Wiring to logger

These instructions assume that you are using a [Margay](https://github.com/NorthernWidget-Skunkworks/Project-Margay) or [Okapi](https://github.com/NorthernWidget-Skunkworks/Project-Okapi) data logger. If this is not the case, similar pins are available on a generic Arduino board.

### HardMount Plug

Pass the cable through the housing to the logger, ideally using a cable gland that you can tighten on the box containing the logger: this can provide waterproofing as well as some strain relief. Follow the table in the [Wiring and pinout](#Wiring-and-pinout) section to attach individual wires to the appropriate pins on the data logger and (if needed) RS-485 converter. In bulleted-list form, with colors given for [Alpha Wire 5004C](https://www.digikey.com/product-detail/en/alpha-wire/5004C-SL001/5004CSL001-ND/484976):

### Power

* V- (Walrus) to GND or V- (Logger)
* V+ (Walrus) to a positive voltage source between 3.3 and 5.5 V (Logger)

#### I2C

If using the sensor in I2C mode, attach the signal wires as follows:
* SIG1 (Walrus) to SDA (Logger)
* SIG2 (Walrus) to SCL (Logger)

#### RS-485

If using the sensor in RS-485 mode, attach the signal wires to the Longbow Backpack as follows:
* SIG1 (Walrus) to A+ (Longbow)
* SIG2 (Walrus) to B- (Longbow)
(You may use another RS-485 converter, in which case the wiring is up to you!)

**Then use another set of four wires to connect the Longbow to the logger:**
* GND to GND
* Vin to a 3.3 to 5.5 V power source
* SDA to SDA
* SCL to SCL
We recommend [Pololu's pre-crimped wires](https://www.pololu.com/category/71/wires-with-pre-crimped-terminals) for this with a male terminal, as it ensures that you can have these four wires in the proper order for both the Walrus and the data logger. For this reason, Northern Widget data loggers ([Margay](https://github.com/NorthernWidget-Skunkworks/Project-Margay) and [Okapi](https://github.com/NorthernWidget-Skunkworks/Project-Okapi)) always have these four pins in the order:
* GND
* V+
* SDA
* SCL

## Writing a program to connect to the Walrus sensor

After uploading the Walrus firmware, you should be able to use any standard Arduino device to connect to it and read its data.

The firmware uploaded to the Arduino device should include the [Walrus library](https://github.com/NorthernWidget-Skunkworks/Walrus_Library) as demonstrated below. Documentation for the Walrus library, auto-generated using doxygen, is available on its linked GitHub page.

### Very simple Arduino code

#### I2C

This code is intended for any generic Arduino system.

>> Note: Not all of this code is built yet. THIS IS A TEMPLATE!

```c++
// Include the Walrus library
#include "Walrus_I2C.h"

// Declare variables -- just as strings
String header;
String data;

// Instantiate class
Walrus myWalrus;

void setup(){
    // Begin Serial connection to computer at 38400 baud
    Serial.begin(38400);
    // Obtain the header just once
    header = myWalrus.getHeader();
    // Print the header to the serial monitor
    Serial.println(header);
}

void loop(){
    // Take one reading every (10 + time to take reading) seconds
    data = myWalrus.getString();
    // and print it to the screen
    Serial.println(data);
    delay(10000); // Wait 10 seconds before the next reading, inefficiently
}
```

#### RS-485

***Not yet developed.***


### Northern Widget Margay code

The [Margay data logger](github.com/NorthernWidget-Skunkworks/Project-Margay) is the lightweight and low-power open-source data-logging option from Northern Widget. It saves data to a local SD card and includes on-board status measurements and a low-drift real-time clock. We have written [a library to interface with the Margay](github.com/NorthernWidget-Skunkworks/Margay_Library), which can in turn be used to link the Margay with sensors.

#### I2C

```c++
// Include the Walrus library
#include "Margay.h"
#include "Walrus_I2C.h"

// Declare variables -- just as strings
String header;
String data;

// Instantiate classes
Walrus myWalrus;
Margay Logger; // Margay v2.2; UPDATE CODE TO INDICATE THIS

// Empty header to start; will include sensor labels and information
String Header = "";

// I2CVals for Walrus
uint8_t I2CVals[] = {0x4D}; // DEFAULT

//Number of seconds between readings
uint32_t updateRate = 60;

void setup(){
    Header = Header + myWalrus.getHeader();
    Logger.begin(I2CVals, sizeof(I2CVals), Header);
    initialize();
}

void loop(){
    initialize();
    Logger.run(update, updateRate);
}

String update() {
    initialize();
    return myWalrus.getString();
}

void initialize(){
    myWalrus.begin();
}
```

#### RS-485

***Not yet developed.***



## Deployment

Pre-deployment check:
* Ensure that the cable is appropriately strain relieved on the logger end (the screw plug or epoxy, depending on model, handles this on the sensor end).
* Check cable for nicks or tears; replace if needed.
* Ensure that the unit functions properly and gives good readings in the lab
* Check that you have programmed the desired reading duration
* Ensure that the transducer for your Walrus functions over the desired pressure range

Once you are ready, you can deploy your sensor in the river, lake, or atmosphere that you hope to measure. Remember that you must correct all pressure measuremennts for barometric pressure in order to recover a water level. If the water is saline or heavily sediment laden, you must know the salinity or sediment-adjusted density as well.

## NW-Device-Specification — Schema 1, Page 0

Implements [NW-Device-Specification](https://github.com/NorthernWidget/NW-Device-Specification) Schema 1. The 32-byte identity block (Page 0) is stored at the top of EEPROM:

```
Block 0:  Schema=0x01, Name='W','a','l','r','u','s',0x00
Block 1:  HW major=[mfr], HW minor=[mfr], FW patch=[mfr], 0x00,0x00,0x00, Reserved
Block 2:  Board type=0x5702 ('W'=0x57, rev 2), Group ID=[mfr], Unique ID=[mfr], FirmwareID=0x0000
Block 3:  Reserved, Magic=0x00, CRC=[computed], I2C address=0x57
```

Legacy deployed units use I²C address `0x4D` (pre-Schema-1).

## Acknowledgments

Support for this project provided by:

<img src="https://pbs.twimg.com/profile_images/1139626463932637186/qCak0yvY_400x400.png" alt="UMN ESCI" width="240px">

<img src="https://ane4bf-datap1.s3-eu-west-1.amazonaws.com/wmocms/s3fs-public/styles/featured_media_detail/public/advanced_page/featured_media/wmologo2016_fulltext_horizontal_rgb_en-2.jpg?C4guHHfFZ0Uv029Eo5AvJLFg6nMR47hI&itok=NVNNIT7H" alt="WMO" width="240px">

<br/>
<br/>
<a rel="license" href="http://creativecommons.org/licenses/by-sa/4.0/"><img alt="Creative Commons License" style="border-width:0" src="https://i.creativecommons.org/l/by-sa/4.0/88x31.png" /></a><br />This work is licensed under a <a rel="license" href="http://creativecommons.org/licenses/by-sa/4.0/">Creative Commons Attribution-ShareAlike 4.0 International License</a>.
