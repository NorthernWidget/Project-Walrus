# Project-Walrus

The Walrus: an ATtiny1634 carrying a TE MS5803-05BA and a Microchip MCP9808, presenting itself to a logger as a Schema 1 device at 0x57. `Walrus_Library` is the logger-side half.

## Standards

Follow the NW standards in the root `CLAUDE.md` one level above `github/`, and the Schema 1 layouts in [NW-Device-Specification](https://github.com/NorthernWidget/NW-Device-Specification). Run `python3 ../NW-Tests/style_check.py .` before any commit touching `.ino/.cpp/.h`.

## The two buses

- **To the logger it is a peripheral**, at 0x57, through the **USI** in two-wire mode - `Wire`, which ATTinyCore routes to the USI on a 1634. Not the hardware TWI peripheral; that is the Apis's path, through `WireS`.
- **To its own chips it is the controller**, bit-banging PA2/PA3 with `SlowSoftWire`. It passes `internal_pullup = false`, so it depends entirely on the board's pullups.

## Facts worth keeping

- The firmware's MCP9808 decode is **correct** across -40 to +125 C. A commented-out alternative below it is Microchip's own Example 5-1 transcribed, and that example returns the magnitude, positive, for every sub-zero reading. Do not "fix" the live code to match it.
- MCP9808 Register 5-4's Note 2 claims a 0.25 C power-up resolution; Register 5-7's POR legend (`R/W-1, R/W-1`) says 0.0625 C. The register definition wins.
- The MCP9808 reads 0x0000 until 250 ms after power-up, which matters because a Margay cuts the sensor rail at every sleep.
- The MS5803 compensation constants `COEF0`-`COEF15` match the 05BA datasheet exactly; they are per-variant, so a different MS5803 needs a different set.

## Verifying without hardware

`NW-Sim/tests/in_the_loop.sh` runs this firmware against the real library on a simulated bus, with both chips modelled from their datasheets, and diffs the logger's transcript against a recorded baseline.

## Hard rule

**Never** create a git tag, GitHub release, push to a shared remote, or close an issue unless explicitly asked in the current message. If in doubt, ask.
