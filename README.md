# C-Code-Shaper

A silly tool that turns C source code into ASCII art by reshaping it to fit inside images

This is purely for fun and absolutely useless for anything serious.

## How it works:

Give it a C file and an image

The tool tokenizes your C code (handles macros, preprocessor directives, comments)

It tries to fit those tokens into the image shape by treating light pixels as space for code and dark pixels as blank

Outputs a new C file where your original code is molded into the image pattern

## Usage:
`shapec.py [-h] -i IMAGE [-o OUTPUT] [-t THRESHOLD] [-s SCALE] [-c COMMENT_FILL] [-f COMMENT_FILE] source`

## Positional Arguments:
- `source`: C source file

## Options:

- `-i` `IMAGE`: Your input image (required)

- `-o` `OUTPUT`: Where to save the result (default out.c)

- `-t` `THRESHOLD`: Brightness threshold (0-255, lower = darker, default 100)

- `-s` `SCALE`: Manually scale the image

- `-c` `COMMENT_FILL`: Character(s) to fill comment areas with (defaults to be "_")

- `-f` `COMMENT_FILE`: Use text from a file as comment filler

## Examples of what you can do:

<img width="1597" height="1355" alt="Screenshot 2026-05-18 161134" src="https://github.com/user-attachments/assets/c50b7462-330c-4b39-9436-631ee5d45a00" />
<img width="1010" height="1190" alt="Screenshot 2026-05-18 161514" src="https://github.com/user-attachments/assets/2f84a9b0-94c3-4218-9f9c-eba08f8a1e2b" />
<img width="1088" height="1285" alt="Screenshot 2026-05-18 161558" src="https://github.com/user-attachments/assets/6547590d-2dc1-4096-beb1-114628485b15" />
<img width="1332" height="1618" alt="Screenshot 2026-05-18 161645" src="https://github.com/user-attachments/assets/30bfd884-8841-4859-a8b7-c0c5451fb466" />
<img width="1158" height="1352" alt="Screenshot 2026-05-18 162201" src="https://github.com/user-attachments/assets/ad1fcda7-6223-4d11-8302-90c0a9612649" />
<img width="2598" height="645" alt="Screenshot 2026-05-18 162312" src="https://github.com/user-attachments/assets/42a87d61-4f09-4384-8ae2-2d0a6ff71789" />
<img width="1496" height="1798" alt="Screenshot 2026-05-18 161901" src="https://github.com/user-attachments/assets/bd662938-f387-4d95-8f3a-43b81cfb4451" />
