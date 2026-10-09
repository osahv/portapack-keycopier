# Why Key Copier needs a matching firmware

Mayhem external apps (`.ppma`) are separate binaries loaded from the SD card, which suggests you could just copy one
onto an existing installation. In practice that only works for an app that was linked against the *exact* firmware image
on the device. This page records what we found while trying to make a stand-alone `.ppma` work on the stock v2.4.0.

## How the loader checks compatibility

`ui_external_items_menu_loader.cpp` lists an app only when the header version matches **and** the app's `VERSION_MD5`
equals the firmware's. `VERSION_MD5` is derived from the version string the firmware was built with, so building with the
same `VERSION_STRING` (here `v2.4.0`) gives a matching hash. That is only a gate, not a guarantee.

## What actually goes wrong

External apps call firmware functions at **absolute addresses** taken from the `application.elf` they were linked with.
If the main firmware image differs even slightly from the one on the device, those calls land in the wrong place. Our
first build showed this on the device as an M0 hard fault right after launching the app.

A rebuild of v2.4.0 from the tagged source with the official Docker image (`dockerfile-nogit`, ARM GCC 9.2.1,
`VERSION_STRING=v2.4.0`) reproduces the official `firmware_hackrf.bin` to within **15 bytes** out of 1 MiB, so the build
environment is sound. The problem starts when anything is added:

1. **A new app** grows tables in the main image, shifting everything after them.
2. **Reusing the slot of an existing app** (keeping the table size): the image still differed by about 400 kB. Merged
   string literals, weak/COMDAT instantiations and functions that the linker garbage-collects when the replaced app
   disappears all change the read-only data layout of the main image.
3. Calling any firmware function that the stock image does not contain (for example the
   `Painter::draw_string(Point, Font, Color, Color, string_view)` overload) forces the linker to keep extra code.

Making the main image byte-identical to the official one while adding code is therefore not realistic.

## Consequence

The firmware and **every** `.ppma` must be built together. `scripts/build.sh` does that and appends a suffix to the
version string (`v2.4.0-kc1`) so the firmware does not pick up stock `.ppma` files from the card.

The clean long-term solution is to include the app in Mayhem itself, so that it is built with every release.
