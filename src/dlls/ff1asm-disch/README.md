# asm-ff1disch

This is an assemblky implementation DLL used with FFHacksterEx.

This experimental DLL handles translating marked lines in the annotated version
of the Disch disassembly source code into bytes in a ROM buffer, and vice versa. \
This allows the editor to present data from th eassembly source files
in the same editors used to modify ROM files (as the original FFHackster does).

#### asm-ff1disch.vcxproj
Main project file.

#### asm-ff1disch.cpp
This is the main DLL source file. \
It defines exported functions and internal implementation functions.

#### asm-ff1disch.h
Main import/export heade3r.
Defines symbols and functions used by FFHacksterEx to
opaquely call functions in the DLL. \
Not the best implementation - to say the least - but it's functional.

#### asm-ff1disch.rc and resource.h
Defines all resources and resource symbols defined by the DLL.

#### data_functions.cpp/h
Defines functions used for data handling when working with assmebly source.

#### dialog_functions.cpp/h
Implements functions dealing with the sprite dialogue editor. \
Translates assembly source for the sprite routines into a form usable
by the Sprite Dialogue editor in FFHacksterEx.

#### shared.cpp/h
Common symbols and functions used across the DLL.

#### version-number.h
Sets the version number used to build the DLL.

#### Pasckaging/*
Contains MSBuild targets to create a batch file that packages the DLL
as a zip file.