FFHacksterEx
Esselle Jaye
===========================

This is an experimental extension of FFHackster.

This project is the original FFH code base updated to
Visual Studio 2019 with some extra features.
This version was used primarily to support internal/private hacks and patches.

- Updated visuals (larger editors, background art, icons, etc.)
- app-wide settings, MRU (most-recently used) list
- supports adjustable offets (address, counts, pointer adds, etc.)
- project oriented approach for ROM editing
- additional text editor categories
- additional editors
- EXPERIMENTAL support for editing disassembly-based projects (requires annotated disassembly)
- EXPERIMENTAL support for extensions

Startup Project: FFHacksterEx (desktop application)

Versioning:
  See below.


===========================

/// WARNINGS ///

WARNING:
  While Visual Studio verions later than 2015 can be used,
  beware of their attempts to upgrade the projects to a compiler toolset
  later than 2015 140_xp.

  DO **NOT** UPGRADE ANY PROJECT IN THE SOLUTION BEYOND THE 2015 140_xp TOOLSET!

  Since this release is a single codebase that targets both XP and later systems, 
  upgrading past 2015 140_xp introduces a large amount of breaking changes that also 
  make the project incompatible with XP. 
  The 2017 141_xp toolset doesn't work because parts of that toolset's standard library 
  (i.e. Microsoft's STL implementation) are implemented using functions that 
  were introduced in Vista and later, causing runtime errors when executed on XP.

  XP went out of support in 2014, but for the sake of those still using it for
  various reasons that support is retained for now.
  However, it will disappear at some point in 2022.


WARNING:
  Opening the solution sometimes loses track of the active start up project.
  This tends to happen when the .suo file is corrupted.
  To address it:
  - close the solution
  - go to the solution folder (which contains the .sln file).
  - navigate to the .vs/FFHacksterEx subfolder
  - delete the .suo file, then reopen the solution
  - set the FFHacksterEx project as the startup project
  - it should remain set once you close and reopen the solution henceforth.


WARNING:
  If you need to update the files that are copied as part of the
  FFHacksterEx project (e.g. updated .template files), you can
  open and save the PostBuild.targets file (without making any changes
  to it).
  The act of saving updates the file's timestamp, which will trigger the
  target on the next build.


===========================

/// DIRECTORY STRUCTURE ///

The project directory structure is built in such a way that the code
itself is all in the src/ folder and artifacts built from it are placed
into siblings of that folder (dist/ and output/).

Other solutions that work with this one depend on the directory structure,
so it's advisible not to change it unless you either don't use those
other solutions or are willing to adapt them to match your changes.

Initially, the dist. and output/ folders do not exist.
They are created upon Build, and their contents are purged during a Clean.
Once built, the directories will appear as below:
Assuming a parent folder anmed FFHacksterEx:

FFHacksterEx/                                        The main folder containig everything else.
    dist/                                            Contains zip batch files that create deployable zips.
        ffhacksterex-win32-x86-v140_xp.zip
        ffhacksterex-win32-x86-v140_xp-debug.zip
        ffhacksterex-win64-x64-v140_xp.zip
        ffhacksterex-win64-x64-v140_xp-debug.zip
    output/                                          Contains build outputs.
        Debug/
            Win32/
            x64/
        Release/
            Win32/
            x64/
    src/                                             Contains all source code.
        .vs/
        apps/                                        Contains all projects producing EXEs.
            FFHacksterEx/
        dlls/                                        Contains all projects producing DLLs.
            ff1asm-disch/
        libs/                                        Contains all projects producing static LIBs.
            ff1-coredefs/
            ff1-editors/
            ff1-settings/
            ff1-subeditors/
            ff1-utils/
        txt/                                         Contains text and related informational files.
            License.txt
            License-7zip.txt
            License-iconmonstr.txt
            Readme-for-devs.txt (this file)
        compat.props                                 Common property sheet used by all projects.
        FFHacksterEx.sln                             The solution file.


The batch files produced in the dist/ directory - when run - will produce
zip archives containing the files needed to run the application.
Unzipping these into a new directory will effectively create a new
installation of the FFHacksterEx.



===========================

/// Versioning ///

Various executable projects contain version-number.h files.
The version-number.h file contains macros defining the version
as both tokens and a string.
If you want to implement auto-incrementing, then you can target
this header file to accomplish that.


/// SOLUTION ITEMS ///

Files that pertain to the entire solution instead of a particular project.

  Readme.txt
    This file.

  License.txt, License-7zip.txt, and License-iconmonstr.com
     License terms for the source code and dependencies.

  compat.props
     Defines a custom property to supress the known warning that XP support is deprecated.
     Visual Studio 2017's v141 compiler is the last version that supports building for XP.
     The property sheet adds a property that supresses the warnings that spam the build log.
     If XP support is removed in the future, this will will also be removed.


/// FFHacksterEx PROJECT ///

This is the main startup project, providing the welcome page, main window,
and access to top level functionality.

  Logging
    This project writes messages to std::cout, std::cerr, and std::clog.
    When debugging, these messsages are also reflected to the debugger output window.
    - The output window logging is set up in ff1-utiles/FFBaseApp.cpp.

    To also have that ouput written to a file, the debugger (by default in all configurations)
    redirects both stdout and stderr to a file.
    - The file logging is done by redirection on the commnand line.
      Right-click the project, select Properties, then click Debugging item.
      On the Debugging page, the Command Parameters field contains the following:

      > $(OutputPath)\FFHacksterEx.exe-debug.log 2>&1

      That will redirect output to a file in the output folder (based on config and platform).
      To prevent this from happening, simply remove that text from the Command Parameters field.

    When running a release build, the batch file FFHacksterEx.exe-trace.bat runs the
    FFHacksterEx.exe and redirects its stdout and stderr to a file.
    - The batch file is deployed as build output, it runs this command:

    > FFHacksterEx.exe > FFHacksterEx.exe-trace.log 2>&1


  Packaging filter (MSNuild targets)
    The MSVC 2015 compiler that supports XP does NOT support the following:
    - copying content files to the output folders on Build
    - removing copied content from output folders on Clean

    The 2017 compiler does, but does not support XP. TO work around the issue for 2015,
    MSBuild targets are used. The Packinging filter contains the following files:
    - PostBuild.targets: this is baked into the project file. Whenever this file is
      modified, the project will be rebuilt. It imports the other targets.

    - ff-content.targets: moves files in the Content filter to the output folder on Build
      and removes them from the output folder on Clean.

    - ff-packaging.targets: creates a batch file in the dist/ folder that (when executed)
      creates a zip file with all of the deployable executables and content in the
      output folder. Each created batch file is named for its platform, configuration,
      and architecture, and creates a zip file with the same name.


  Content filter
    This filter contains files that will be copied to the output directory during a build.
    - *.template files
      The *.tenplate files are used as defaults when creating a new project.
      By using these files, new projects and hacks can add new settings without having to
      rebuild and redistribute the application.
    - 7za.exe
      Used to handle zipping and unzipping (archive projects, assembly)
    - FFhacksterEx.exe-trace.bat
      A batch file that starts FFHacksterEx.exe and redirects stdout and stderr to a file.
    - FFHacksterEx.helpconfig
      Help config file used for LocalApp and Web help configurations.


/// ff1-coredefs PROJECT ///
 
Provides shared symbols and function across the entire application.

  Assembly interface for extensions
    interop_assembly_interface.h defines the interface.
    It's kind of hard to use and will likely change in the near feature.


/// ff1-utils PROJECT ///

Provides primarily UI-level utilities and helpers.


/// ff1-editors PROJECT ///

Provides the edit dialogs used for weaponas, armor, maps, etc..


/// ff1-subeditors PROJECT ///

Provides the smaller scope edit dialogs used by editors and the main window.
Things like NES Palette, Coords editor, Weapon/Magic Graphic editor, etc..


/// ff1-settings PROJECT ///

Provides settings used with various editors and their associated edit dialogs.



===========================

/// FFHACKSTER EX ROM AND ASSEMBLY PROJECTS ///

Editing is now managed by projects.
Both ROM and disassembly files can be used for projects.
Each project has its own set of data for offsets, counts, etc.
and allows customization of those as well as the labels used
in the editor.


/// RUNTIME BEHAVIOR ///

A project can only be edited by a single instance of FFHacksteEx at a time.
To enforce this, the app creates a mutex using the project's filepath.
Any other instance attempting to open that project file will be blocked
from creating the mutex - since it already exists - until the instance that
owns it closes the project.

If you don't like this behavior, then it can be removed by removing
the m_pmutex member and its supporting code from CFFHacksterDlg,
but beware; there is no other protection against two instances of FFHEx
writing conflicting changes to the same project.

If the two FFHEx instances are of different versions, then the results
could very likely corrupt any ROM, Assembly, or settings files in the project.


/// EXISTING EDITORS ///

The Classes editor has a couple of additional features:
- Ability to assign BBMA stas and hit rate to any classes
- Ability to assign or ignore FI/TH hold MP status
- Ability to set max spell charge limits
- Ability to set max HP (requires hex/assembly hack to actually use it)
- EXPERIMENTAL copy/paste of stats in class list box via right-click

The Enemies editor allows Byte 15 to be Attack Elements or an integer value.

The Text editor adds some additional categories:
- Intro, Shop, and Status Menu
- Story (includes bridge and ending)


/// NEW EDITORS ///

The Starting Items editors allows:
- selection of key items to start the game
- selection of consumables
- show the bridge normally, at startup, or never

The Party Setup editor allows:
- Number of classes available in a new game
- Setting the default new game class selections
- locking class selections (to force a fixed party)

The EXPERIMENTAL Sprite Dialogue editor allows editing
of events tied to NPCs. With some limits, it allows
handlers, events, and items to be reassigned to various NPCs,
as well as selecting text messages and fanfares.



===========================

/// ***EXPERIMENTAL*** DISASSEMBLY SUPPORT ///

There is support for disassembly projects, but it's hacky.
Generally, projects based on a disassembly support the same
editing experience as ROM based projects.

This ***requires*** an annotated form of the Disch Disassembly.
That disassembly is not included with this project, but to create it:
- Download Disch's disassembly zip from RHDN.
- Apply the patch DischAnnotated.ips file to the zip.
- That disassembly can now be used as a clean source for new projects.

The disassembly source files use specially formatted comments in
the source as markers - hints to help the program tell what
source code changes to make. Moving or removing those comments
will break assembly editing.



===========================

/// ***EXPERIMENTAL*** EXTENSION SUPPORT ///

Extensions are DLLs that define functions which can replace
existing builtin editors.
It's hacky.

The DLL exports a single function with a C interface:
FF1GOLDITEMS_API char* Invoke(const EditorInterop * ei);
see ff1-coredefs/EditorInterop.h for more details.

Invoke returns a collection of editors in the DLL that will
be added to and where needed override the builtin editor list.

The Editors dialog manages the extensions and allows changing
the display order of editors on the main screen.



===========================

/// ***EXPERIMENTAL*** HELP ///

Help is available either locally if the help package is unzipped into the app folder. \
App settings contains a folder path to specify local help. \
Editors and screens that support help access it using F1 or Shift+F1.



===========================

/// ITEMS THAT NEED ATTENTION ///

Throughout the source, there are comments with specific labels.
Search for them using two slashes followed by one of the terms below
e.g. //TODO - add an exciting new feature without any bugs...

//TODO - an item that needs to be revisited in the short term
//REFACTOR - a working feature that could be rewritten for better performance, readability, etc.
//HACK - a hack to get things working, but without a know refactor/rewrite plan
//FUTURE - a lower priority change that could be useful later
//DEVNOTE - a note about the development/implementation that might not be obvious from reading the code
//N.B. - nota bene ("note well") points out an observation that's important to understand before modifying behavior
//BUG - a known defect that will be addressed at a later time
