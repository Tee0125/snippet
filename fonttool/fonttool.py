#!/usr/bin/env python

import copy
import sys

from xml.etree.ElementTree import parse, ElementTree

def update_node(p):
    # remove nameEntry for Macintosh
    for e in p.getiterator("localizedName"):
        if e.get("platformName") == "Macintosh":
            p.remove(e)

    # copy nameEntry from Microsoft to Macintosh
    for e in p.getiterator("localizedName"):
        if e.get("platformName") == "Microsoft":
            if e.get("languageID") == "1033":
                # English name
                n = copy.deepcopy(e)

                n.attrib["platformID"] = "1"
                n.attrib["platformName"] = "Macintosh"
                n.attrib["scriptID"] = "0"
                n.attrib["scriptName"] = "Roman"
                n.attrib["languageID"] = "0"

                p.append(n)
            elif e.get("languageID") == "1042":
                # Korean name
                n = copy.deepcopy(e)

                n.attrib["platformID"] = "1"
                n.attrib["platformName"] = "Macintosh"
                n.attrib["scriptID"] = "3"
                n.attrib["scriptName"] = "Korean"
                n.attrib["languageID"] = "23"

                p.append(n)

if len(sys.argv) != 2:
    print "Usage: %s xmlfilename" % sys.argv[0]
    exit

tree = parse(sys.argv[1])
root = tree.getroot()

root.find("nameTableEntry")
child=root.getiterator("nameTableEntry")

for e in child:
    if e.get("nameTypeName") == "Family":
        update_node(e)
    elif e.get("nameTypeName") == "Style":
        update_node(e)
    elif e.get("nameTypeName") == "Unique":
        update_node(e)
    elif e.get("nameTypeName") == "Full":
        update_node(e)

ElementTree(root).write(sys.argv[1])
