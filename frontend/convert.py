#!/bin/env python3

import argparse
import os
import sys

def slurp(filename):
    contents = ""
    with open(filename, "r") as f:
        contents = f.read()
    return contents

def hexify(string):
    return list(map(hex, map(ord, string)))

def cify(filename, hexes):
    var = os.path.basename(filename).replace(".", "_")
    guard = "__" + var.upper() + "_H__"
    contents = ""
    contents += "#ifndef " + guard + "\n"
    contents += "#define " + guard + "\n"
    contents += "#include <Arduino.h>\n"
    contents += "#include <WString.h>\n"
    contents += "const char " + var + "[] PROGMEM = {\n"

    for i in range(0, len(hexes)-1):
        if (i % 16) == 0:
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
    args = parser.parse_args()

    for f in args.files:
        output_dir = ""
        if "output_dir" in args:
            output_dir = args.output_dir[0]
        else:
            output_dir = os.path.dirname(f)
        output_file = output_dir + "/" + os.path.basename(f) + ".h"
        spit(output_file, cify(f, hexify(slurp(f))))
