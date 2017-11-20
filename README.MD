## libcurl Playground

This is a project for testing out the libcurl library.

Current features: Download the Internet ! (actually just the links..)


program usage:

`` ./bin/program <wikipedia-url> [depth=1] ``

Uses a BFS algorithm to recursively collect reachable links from the specified start page until the max depth is reached.
