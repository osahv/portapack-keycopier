#!/usr/bin/env python3
"""Register the key_copier external app in a Mayhem source tree.  Usage: integrate.py <mayhem-firmware dir>

Two layouts of Mayhem are handled:
  * old (e.g. v2.4.0): sources + EXTAPPLIST in external.cmake and a RAM slot + section appended to external.ld;
  * new (tier system, `next` since 2026-10): sources + EXTAPPLIST in external.cmake, a tier.txt (0 = never internal)
    and the tier linker scripts regenerated with tools/update_external_tier_ld.py.
A new app changes the firmware image, so firmware and ALL apps must be built together (scripts/build.sh)."""
import re, subprocess, sys, pathlib

root = pathlib.Path(sys.argv[1])
ext = root / "firmware/application/external"
SOURCES = ["main.cpp", "ui_key_copier.cpp", "key_formats.cpp"]


def insert_before_close(text, block_start, lines):
    """Insert `lines` just before the first line that is exactly ')' after `block_start`."""
    i = text.index(block_start)
    m = re.compile(r"^\)\s*$", re.M).search(text, i)
    assert m, "closing parenthesis of %s not found" % block_start
    return text[:m.start()] + lines + text[m.start():]


cm = ext / "external.cmake"
t = cm.read_text()
if "key_copier" not in t:
    t = insert_before_close(t, "set(EXTCPPSRC", "\n\t#key copier (portapack-keycopier)\n" + "".join("\texternal/key_copier/%s\n" % s for s in SOURCES))
    t = insert_before_close(t, "set(EXTAPPLIST", "\tkey_copier\n")
    cm.write_text(t)

new_layout = (root / "tools/update_external_tier_ld.py").exists()
if new_layout:
    (ext / "key_copier" / "tier.txt").write_text("0\n")
    subprocess.run([sys.executable, "tools/update_external_tier_ld.py"], cwd=root, check=True)
    layout = "tier system"
else:
    ld = ext / "external.ld"
    t = ld.read_text()
    if "key_copier" not in t:
        addrs = [int(a, 16) for a in re.findall(r"ram_external_app_\w+\s+\(rwx\)\s*:\s*org\s*=\s*(0x[0-9A-Fa-f]+)", t)]
        last = list(re.finditer(r"^.*ram_external_app_\w+\s+\(rwx\).*$", t, re.M))[-1]
        t = t[:last.end() + 1] + "    ram_external_app_key_copier            (rwx) : org = 0x%X, len = 32k\n" % (max(addrs) + 0x10000) + t[last.end() + 1:]
        section = """    .external_app_key_copier : ALIGN(4) SUBALIGN(4)
    {
        KEEP(*(.external_app.app_key_copier.application_information));
        *(*ui*external_app*key_copier*);
    } > ram_external_app_key_copier
"""
        i = t.rstrip().rfind("}")
        t = t[:i] + "\n" + section + t[i:]
        ld.write_text(t)
    layout = "classic external.ld"
print("registered key_copier in %s (%s)" % (root, layout))
