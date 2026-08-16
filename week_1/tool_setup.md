<div align="right">
  <a href="tool_setup.md"><img src="https://img.shields.io/badge/Windows-0078D4?logo=data:image/svg+xml;base64,PHN2ZyB4bWxucz0iaHR0cDovL3d3dy53My5vcmcvMjAwMC9zdmciIHZpZXdCb3g9IjAgMCAyNCAyNCI+PHBhdGggZmlsbD0id2hpdGUiIGQ9Ik0wIDMuNDVMOS43NSAyLjF2OS40NUgwem0xMC45NS0xLjVMMjQgMHYxMS41NUgxMC45NXpNMCAxMi40NWg5Ljc1djkuNDVMMCAyMC41NXptMTAuOTUgMEgyNFYyNGwtMTMuMDUtMS45eiIvPjwvc3ZnPg==" alt="Windows setup (this page)"></a>
  <a href="mac_tool_setup.md"><img src="https://img.shields.io/badge/macOS-000000?logo=apple" alt="macOS setup"></a>
</div>

# Introduction

Now that we have gone over the history computer graphics, it is time to get our hands dirty! Before we can put pixels to screen, we need to ensure our development environment is set up correctly. There are lots of ways of using OpenGL: C++, Python, and Java. In this course, we are going to focus on writing applications in C++.

We will be using the following tech stack:

* Visual Studio
* GLFW (Graphics Library Framework)
* GLEW (OpenGL Extension Wrangler Library)
* GLM (OpenGL Mathematics)
* SOIL 2 (a fork of Simple OpenGL Image Library)
* Premake (needed to compile SOIL 2)

Over the course of this exploration, we will walk you through how to install all of these components. At the end you will be able to display a single pixel to the screen!

You are free to make changes to where things are installed, but the choices made for this exploration are designed to make things easy to get working. If you deviate from these instructions you may run into difficulties, which we may not be able to help you fix. Therefore, only strike your own path if you are experienced and capable of troubleshooting.

Warning: Many of the components discussed below are in constant flux, so sometimes version numbers won't line up exactly. If that happens, just reach out to the instructor or look to Ed Discussions for solutions. I will endeavor to keep this page up-to-date, but things *will* slip through the cracks.

# Visual Studio Installation

Most of you probably already use VS Code, where VS stands for "Visual Studio." Unfortunately, VS Code is primarily a text editor; for OpenGL we need a full-fledged Integrated Development Environment (IDE). This is a beefy piece of software, so make sure you have the hard drive space before proceeding; some installs can take 20GB or more.

At the time of writing, the most recent version of Visual Studio is 2026, which we will be using. If you have an older version of Visual Studio, you will most likely not need to upgrade, as OpenGL doesn't really care. You may just need to "retarget" the sample project to your version.

The first step is to go to Azure for Students (https://azure.microsoft.com/en-us/free/students). Sign in either by clicking the "Sign in" button in the top right or by clicking the "Start free" on the landing page. As an OSU student, you should already have a Microsoft Account associated with your OSU email. If you don't already see your email listed, just pick "Use another account" and follow the prompts with your OSU email.

Once you have logged in, you will be taken to the Education Overview page. Find the "Free Software" portion of the dashboard and click "Explore All." This will take you to a screen with a search field into which, you should type "Visual Studio."

You will see multiple versions of Visual Studio, and it is very important that you pick the correct one, so please be careful. Click the "Visual Studio Enterprise 2026" option to reveal a sidebar with an option to "View Key" and "Download."

We will need this key in order to actually run the software, so go ahead and reveal it. I recommend you copy this key into a text file that you can save somewhere safe on your computer. Don't worry, it isn't sensitive like a password, so there is no need to worry about it being saved as plain text.

Once you save your key, go ahead and click the download button. This will prompt you to download the VisualStudioSetup.exe file. This is a fairly small file, and only acts as the installer. After the brief download, go ahead and run the installer.

When the installer is up and running, you will be presented with a dizzying array of options for packages to install. For this course, we only care about "Desktop development with C++." Check this option and click "Install." Unless you have particular needs, my recommendation is to follow all the default installation recommendations.

Now, sit back and *wait*. This is a huge install, and it will take a long time, no matter how fast your internet.

# Note On Extracting Zip Files

Depending on how your application runs, you may end up extracting the files into folders that have a slightly different structure. By default, Windows will extract a zip file's contents into a folder with the same name as the file itself. This will result in a directory within a directory that share the same name. If, instead, you click the "browse" button, and navigate to the GL folder and extract directly into it, then your files should be extracted in the way these instructions expect. If for any reason your file structure isn't exactly the same, click through one more layer of directories and check to see if the mentioned files are there. You can modify the instructions on the fly if this is the case. If you still have trouble, reach out, and we would be happy to help.

# GLFW

While the VS behemoth downloads, we can use our time to get the rest of the frameworks we will need. Let's start with GLFW.

OpenGL handles a lot for us, but since it is cross-platform compatible we need to have platform specific code for things like creating windows and utilizing input devices. GLFW provides the C++ code needed for these crucial tasks.

We need a place to store all these frameworks and I highly recommend that you create a folder called "GL" on your C drive (or wherever you will be working). It is a really bad idea to install these things on a virtual drive; trust me.

Go to the GLFW homepage (https://www.glfw.org/) and click the "Download" tab at the top. I am going to assume you have a 64-bit computer, so click the button "64-bit Windows binaries" to initiate the download. This should be fairly quick, and once download extract the contents into the GL folder you created.

You should now have a folder in your GL directory called something like "glfw-3.4.bin.WIN64", which you need to rename to simply "GLFW". This is important for later steps, so please don't use any other name.

# GLEW

I am pretty sure VS is still downloading, so let's grab some pieces we need. Now, we are going to acquire GLEW. This is a very technical library that basically preloads the function pointers from the graphics drivers so we can actually use them in our applications.

Go to the GLEW website (https://glew.sourceforge.net/) and click the "Windows 32-bit and 64-bit" binaries link. Again, extract the contents into the GL folder. Rename this new folder to "GLEW".

# GLM

Before the programmable pipeline, OpenGL relied on a lot of built-in math functions to handle things like rotations. Now, I am not a monster, and I am not going to make you write your own matrix math functions; we are going to use GLM, which contains all the graphics math we will need.

Navigate to the repo containing GLM (https://github.com/g-truc/glm). To download, click the green "<> Code" button and select "Download ZIP". I bet you know what I am going to say now! Extract the file into the GL folder and rename it "GLM".

# SOIL 2

Just like we needed GLFW to manage system calls, we need SOIL 2 to make loading textures easier.

Go to the SOIL 2 repo (https://github.com/SpartanJ/SOIL2), and just like with GLM, download all the code as a zip. Again, unzip the contents into the GL folder and rename the new directory to "SOIL2".

We need to do a few more things to get SOIL 2 ready to use: download Premake.

# Premake

Premake is similar to *make* in that it is used to build source code. Go to the download page (https://premake.github.io/download/) and download the Windows Pre-Built Binary. It will download as a zip containing one file. Extract this file wherever you like and then copy it into the SOIL2 folder.

Now it is time to dig out your favorite terminal application and navigate to the SOIL2 folder. Once there, you need to type the following `premake5.exe vs2022`. I hear you shouting, "But we downloaded VS 2026, not VS 2022!" and you would be correct. Unfortunately, `vs2026` is not an option at the time of this writing.

Once you have run this command, a new folder will appear in your SOIL2 directory: "make". Click through and then into the "windows" folder contained therein. Now, hopefully, VS 2026 has finished installing, but if it hasn't, we will need to wait. We need VS 2026 to open the "SOIL2.sln" project file. 

When you are able, open the "SOIL2.sln" file, which should open VS 2026. You will likely be shown a screen stating that "Some projects are missing components or targeting outdated tool sets..." Click "Retarget all" and then "Apply." This is necessary to convert the VS2022 settings to the new VS2026 IDE we are using.

On the right, you will see the Solution Explorer. Right-click on the "soil2-static-lib" project and select "Build." Go ahead and close VS 2026, we are done with this project.

# Creating a VS Template

We now have everything we need to set up Visual Studio for OpenGL development. Unfortunately, what we are about to do is complicated and fairly involved. Therefore, we want to set it up in such a way that we only have to do it once. In order for this to happen, we need to create a Visual Studio Template. Before we can do so, we have some files to move around.

## Moving Files Around

In the GL folder, create a new directory and call it "OpenGL_Template". In this new directory, create two more: "lib" and "include". We will be filling this with files from the downloaded frameworks. 

Find the glew32.dll file located in GL>GLEW>bin>Release>x64 folder and copy it into our new template directory.

In the "lib" folder we need to copy and paste the following files:

* glew32.lib - found in GL>GLEW>lib>Release>x64
* glfw3.lib - found in GL>GLFW>lib-vc2022
* soil2-debug.lib - found in GL>SOIL2>lib>windows

In the "include" folder we need to copy the following *directories* (not just the files). When we are done, we will have four directories in the "include" folder: "GL", "GLFW", "glm", and "SOIL2".

* GL>GLEW>include>GL
* GL>GLFW>include>GLFW
* GL>GLM>glm
* GL>SOIL2>src>SOIL2

When you are done, you should have a file structure like this:

```
GL/
├── GLEW
├── GLFW
├── GLM
├── SOIL2
└── OpenGL_Template/
    ├── glew32.dll
    ├── lib/
    │   ├── glew32.lib
    │   ├── glfw3.lib
    │   └── soil2-debug.lib
    └── include/
        ├── GL/
        │   ├── eglew.h
        │   ├── glew.h
        │   ├── glxew.h
        │   └── wglew.h
        ├── GLFW/
        │   ├── glfw3.h
        │   └── glfw3native.h
        ├── glm/
        │   ├── glm.hpp
        │   ├── geometric.hpp
        │   └── etc...
        └── SOIL2/
            ├── SOIL2.c
            ├── SOIL2.h
            └── etc...
```

## Setting up the Template

Open up Visual Studio and create a new project. When prompted, select "Empty Project" and hit "Next." Name the project "OpenGL Template" and click "create."

Toward the top middle (under the toolbar), you should see a pull down menu that *should* say 'x64'. If it says 'x86' switch it to 'x64'. Next to this pull down, you should see 'Debug'.

On the menubar, select Project and then Properties. At the top of this new window, you should see "Configuration". Ensure that it says "All Configurations" so that we only have to do the following steps *once*.

Find where it says "VC++ Directories" (it may say "C/C++ Directories"). Under the "General" option, you should see "Include Directories". If you click on it, you will now see a down arrow at the end of the row. Click it and select "<Edit...>".

Click the "New Line" button (folder icon with a starburst). You will now see an ellipse ("...") at the end of the first row. Clicking this will bring up a File Explorer. Navigate to the "include" folder we created above and "select folder". Hit "OK".

Now, click "Linker" on the left and then "General." Find the "Additional Library Directories" row and click on it. Using the same steps we just went through, add the "lib" folder we created above.

Also under "Linker", click "Input" and then "Additional Dependencies". Click through to edit the row. This time we are just going to type in the dependencies (one on each line) and hit "OK":

* glfw3.lib
* glew32.lib
* soil2-debug.lib
* opengl32.lib

Now, we hit "apply".

Now we need to add glew32.dll to the project. Right-click on the "OpenGL Template" in the Solution Explorer and click "Add" and then "Existing Item..." Navigate to where we saved this file in the "OpenGL Template" folder (next to the "include" and "lib" folders) and add it.

Now, right-click on the file in the Solution Explorer and select "Properties". Again, ensure that "All Configurations" is selected. We need to change the "Item Type" to "Copy File". Hit "Apply" and this should bring up "Copy File" under "Configuration Properties". Click this and ensure that "Destination Directories" reads "$(OutDir)". Now we can click "OK".

Make sure to save all changes before moving on.

We are now ready to create the template! Go to "Project" and select "Export Template". Make sure "Project Template" is selected and click through until "Finish".

Now, when we create a new project in Visual Studio, we will see an option to select our own template, so we don't *ever* have to do all that nonsense again! That said, there is one thing we will still need to. We will have to copy the "glew32.dll" file into our new project directory; that is why we put it in our template directory for easy access.

# Crossing our fingers that everything worked!

I am just kidding, we are going to test to ensure everything is working correctly.

Go ahead and open up Visual Studio and create a new project. When asked, select our brand new *OpenGL Template*. You can choose to name this whatever you like and save it wherever you like, but I went with "Test Project" and saved it into the directory I am using for the course work ("CS450").

When the solution loads, we need to add something to build and run. We are going to use [test_install.cpp](../downloadable_files/test_install.cpp). We are providing this file simply as an easy way to verify all the files are where they should be, and you have correctly set up your template. Download this file so you can add it to the project.

Note: `test_install.cpp` loads an image from `C:\GL\SOIL2\bin\img_mars.jpg`, which may not exist on your machine. If it doesn't, change that path in `SOIL_load_image(...)` to any image on your computer.

To add it to the project, right click on "Source Files" in the right-hand *Solution Explorer*. Select "Add" and then "Existing Item". Navigate to where you downloaded the file, and click "Add". The file will now appear in the Solution Explorer. Go ahead and right click it so it opens up. You can look through the code, but honestly, we will be covering writing your own OpenGL file later.

At the top of the screen, click "Build" and then "Build Solution" or simply hit "F7". This should pop up an "Output" window at the bottom of the screen. If everything was set up correctly, you should see something like "Build: 1 succeeded, 0 failed..." If you don't, try to noodle through any build failures or reach out for help.

Once the solution is built, we want to run it. To do this we can hit "Ctrl-F5" or click "Debug" and then "Start Without Debugging". This should bring up two windows. The top window should be "OpenGL Test" and it should have a lovely shade of Beaver Orange. The other window should be a terminal and it should display:

```C++
GLM vector: 1
SOIL2 image loaded: 512x512
```

If you see anything else, that means there is a problem. Any failures should have a message to highlight which part isn't working. Again, if you need help figuring it out, reach out, and we would be happy to assist you.

# Conclusion

In this exploration, we have installed all the software and frameworks required to create an OpenGL program. We have also created a Visual Studio template that we can use for all our projects so we can streamline our process. Finally, we tested that all of this worked correctly. Now we are ready to learn actual Graphics *stuff*.
