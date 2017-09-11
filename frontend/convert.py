#!/bin/env python3

import sys
import os

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
    contents += "PGM_P " + var + " PROGMEM = {"

    for i in range(0, len(hexes)-1):
        if (i % 16) == 0:
            contents += "\n    "
        contents += hexes[i] + ", "

    contents = contents[0:-2]
    contents += "\n};\n"
    contents += "#endif\n"
    return contents

def spit(filename, contents):
    print("Wrote " + filename + ":\n")
    print(contents)
    with open(filename, "w") as f:
        f.write(contents)

if __name__ == "__main__":
    for arg in sys.argv[1:]:
        spit(arg + ".h", cify(arg, hexify(slurp(arg))))
