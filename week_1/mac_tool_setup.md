<div align="right">
  <a href="tool_setup.md"><img src="https://img.shields.io/badge/Windows-0078D4?logo=data:image/svg+xml;base64,PHN2ZyB4bWxucz0iaHR0cDovL3d3dy53My5vcmcvMjAwMC9zdmciIHZpZXdCb3g9IjAgMCAyNCAyNCI+PHBhdGggZmlsbD0id2hpdGUiIGQ9Ik0wIDMuNDVMOS43NSAyLjF2OS40NUgwem0xMC45NS0xLjVMMjQgMHYxMS41NUgxMC45NXpNMCAxMi40NWg5Ljc1djkuNDVMMCAyMC41NXptMTAuOTUgMEgyNFYyNGwtMTMuMDUtMS45eiIvPjwvc3ZnPg==" alt="Windows setup"></a>
  <a href="mac_tool_setup.md"><img src="https://img.shields.io/badge/macOS-000000?logo=apple" alt="macOS setup (this page)"></a>
</div>

# Tool Setup — macOS

## VS Code

No Visual Studio on macOS. Use **VS Code**.

1. Install [VS Code](https://code.visualstudio.com/)
2. Install the **C/C++** extension (Microsoft)
3. Enable the `code` terminal command: [official docs](https://code.visualstudio.com/docs/setup/mac#_launching-from-the-command-line)

## Compiler + Libraries

From your course folder (e.g. `~/CS450`):

```bash
xcode-select --install        # compiler
brew install glfw glew glm    # libraries
git clone https://github.com/SpartanJ/SOIL2   # SOIL2 is not on Homebrew
cd SOIL2
clang -c src/SOIL2/*.c        # build it from source
ar rcs libsoil2.a *.o
cd ..
```

## Compile + Run

Replace `<your_file>` and `<your_prog>` with your own names:

```bash
clang++ <your_file>.cpp -o <your_prog> \
  -I ~/CS450/SOIL2/src -I /opt/homebrew/include \
  -L /opt/homebrew/lib -L ~/CS450/SOIL2 \
  -lglfw -lGLEW -lsoil2 \
  -framework OpenGL -framework Cocoa -framework IOKit \
  -DGL_SILENCE_DEPRECATION

./<your_prog>
```

VS Code config below is optional.

## VS Code Config (optional)

One-time setup. `Cmd+Shift+B` = build, `F5` = build + run.

<details>
<summary><b>The three files</b></summary>

In your course folder:

```bash
cd ~/CS450
mkdir .vscode
code .
```

`tasks.json` — compiles the currently open file with `Cmd+Shift+B`:

```json
{
  "version": "2.0.0",
  "tasks": [
    {
      "type": "cppbuild",
      "label": "build opengl", // launch.json's preLaunchTask must match this
      "command": "/usr/bin/clang++",
      "args": [
        "-g", // debug symbols, needed for breakpoints
        "${file}", // the file currently open in the editor
        "-o",
        "${fileDirname}/${fileBasenameNoExtension}", // output executable next to the .cpp
        "-I",
        "${workspaceFolder}/SOIL2/src", // SOIL2 headers
        "-I",
        "/opt/homebrew/include", // brew headers: GLFW, GLEW, GLM
        "-L",
        "/opt/homebrew/lib", // brew libraries
        "-L",
        "${workspaceFolder}/SOIL2", // where libsoil2.a lives
        "-lglfw", // windowing/input
        "-lGLEW", // OpenGL function loader
        "-lsoil2", // image loading
        "-framework",
        "OpenGL",
        "-framework",
        "Cocoa", // macOS windowing, GLFW needs it
        "-framework",
        "IOKit", // macOS hardware access, GLFW needs it
        "-DGL_SILENCE_DEPRECATION" // silence Apple's OpenGL deprecation warnings
      ],
      "options": { "cwd": "${fileDirname}" },
      "problemMatcher": ["$gcc"],
      "group": { "kind": "build", "isDefault": true } // makes this THE Cmd+Shift+B task
    }
  ]
}
```

`launch.json` — `F5` = build + run:

```json
{
  "version": "0.2.0",
  "configurations": [
    {
      "name": "Run OpenGL",
      "type": "cppdbg", // C/C++ extension debugger
      "request": "launch",
      "program": "${fileDirname}/${fileBasenameNoExtension}", // the executable the build task produced
      "cwd": "${fileDirname}", // run from the file's folder, relative paths work
      "preLaunchTask": "build opengl", // auto-build before running
      "MIMode": "lldb", // macOS debugger
      "targetArchitecture": "arm64", // Intel Mac: x86_64
      "logging": { "moduleLoad": false } // hides the "Loaded ..." spam in the Debug Console
    }
  ]
}
```

`c_cpp_properties.json` — IntelliSense only, no effect on compiling or linking:

```json
{
  "configurations": [
    {
      "name": "Mac",
      "includePath": [
        "${workspaceFolder}/**", // your own headers
        "/opt/homebrew/include", // brew headers
        "${workspaceFolder}/SOIL2/src" // SOIL2 headers
      ],
      "compilerPath": "/usr/bin/clang++",
      "cppStandard": "c++17"
    }
  ],
  "version": 4
}
```

</details>

## Test

Open [test_install.cpp](../downloadable_files/week_1/test_install.cpp). Edit two lines:

1. `SOIL_load_image(...)` path → any jpg on your machine
2. `SOIL_free_image_data(image);` → `free(image);` (current SOIL2 removed this function but left its declaration in the header — calling it breaks the build)

Compile + run.

Beaver Orange (`#D73F09`) window + `GLM vector: 1` + `SOIL2 image loaded: ...` = working.
