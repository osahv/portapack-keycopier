#!/usr/bin/env python3
"""Register the key_copier external app in a Mayhem source tree (new app: sources, EXTAPPLIST and a RAM slot in external.ld).
Usage: integrate.py <mayhem-firmware dir>
Note: a new app changes the firmware image, so the firmware and ALL .ppma files must be built together
(see scripts/build.sh); a lone .ppma only works with the exact firmware it was linked against."""
import re, sys, pathlib
root = pathlib.Path(sys.argv[1])
ext = root / "firmware/application/external"

cm = ext / "external.cmake"
t = cm.read_text()
if "key_copier" not in t:
    t += """
# key_copier (added by portapack-keycopier)
list(APPEND EXTCPPSRC
	external/key_copier/main.cpp
	external/key_copier/ui_key_copier.cpp
	external/key_copier/key_formats.cpp
)
list(APPEND EXTAPPLIST key_copier)
"""
    cm.write_text(t)

ld = ext / "external.ld"
t = ld.read_text()
if "key_copier" not in t:
    addrs = [int(a, 16) for a in re.findall(r"ram_external_app_\w+\s+\(rwx\)\s*:\s*org\s*=\s*(0x[0-9A-Fa-f]+)", t)]
    nxt = max(addrs) + 0x10000
    last = list(re.finditer(r"^.*ram_external_app_\w+\s+\(rwx\).*$", t, re.M))[-1]
    t = t[:last.end() + 1] + "    ram_external_app_key_copier            (rwx) : org = 0x%X, len = 32k\n" % nxt + t[last.end() + 1:]
    section = """    .external_app_key_copier : ALIGN(4) SUBALIGN(4)
    {
        KEEP(*(.external_app.app_key_copier.application_information));
        *(*ui*external_app*key_copier*);
    } > ram_external_app_key_copier
"""
    i = t.rstrip().rfind("}")
    t = t[:i] + "\n" + section + t[i:]
    ld.write_text(t)
print("registered key_copier in", root)
