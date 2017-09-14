#!/bin/env python3

import argparse
import os
import sys

def slurp(filename):
    contents = ""
    with open(filename, "rb") as f:
        contents = f.read()
    return contents

def hexify(data):
    return list(map(hex, data))

def cify(filename, hexes, pretty_print):
    var = os.path.basename(filename).replace(".", "_")
    guard = "__" + var.upper() + "_H__"
    contents = ""
    contents += "#ifndef " + guard + "\n"
    contents += "#define " + guard + "\n"
    contents += "#include <Arduino.h>\n"
    contents += "#include <WString.h>\n"
    contents += "const uint32_t " + var + "_len = " + str(len(hexes)) + ";\n"
    contents += "const char " + var + "[] PROGMEM = {\n"

    for i in range(0, len(hexes)-1):
        if pretty_print and (( i % 16) == 0):
            contents = contents[0:-1]
            contents += "\n    "
        contents += hexes[i] + ", "

    contents = contents[0:-2]
    contents += "\n};\n"
    contents += "#endif\n"
    return contents

def spit(filename, contents):
    print("Wrote " + filename)
    with open(filename, "w") as f:
        f.write(contents)

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Convert any file to PROGMEM bytes.')
    parser.add_argument('files', metavar='FILES', type=str, nargs='+', help="Input files")
    parser.add_argument('--output-dir', type=str, nargs=1, help="Output directory")
    parser.add_argument('--pretty-print', dest='pretty', action='store_true', help="Should dumps be pretty printed?")
    parser.set_defaults(pretty=False)
    args = parser.parse_args()

    for f in args.files:
        output_dir = ""
        if "output_dir" in args:
            output_dir = args.output_dir[0]
        else:
            output_dir = os.path.dirname(f)
        output_file = output_dir + "/" + os.path.basename(f) + ".h"
        spit(output_file, cify(f, hexify(slurp(f)), args.pretty))
