#!/usr/bin/env python3
"""Extract wikitext content from the MediaWiki API JSON dump."""
import json
import sys

with open(sys.argv[1]) as f:
    data = json.load(f)
content = data["query"]["pages"][0]["revisions"][0]["slots"]["main"]["content"]
print(content)
