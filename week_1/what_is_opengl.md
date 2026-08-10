# Introduction

It has occurred to me that you may not know what OpenGL actually is. Likely, you know that it is something used in *Computer Graphics*. You may even know that it is *Open Source*. Yet, you may not realize what it is *specifically*. Let's make sure we all are on the same page and do a quick run down of OpenGL.

# It's a Specification

At its heart, OpenGL is a *specification*. As we learned in our [Graphics History](/graphics_history.md) module, OpenGL was developed by Silicon Graphics, Inc. (SGI) in 1991 and is now maintained by the Kronos Group. OpenGL stands for *Open Graphics Library*, and it describes a cross-platform API (application programming interface).

That sounds very technical, but it really is just a fancy way of laying out the rules for how different platforms, which wish to support OpenGL, must design the "hooks" programmers will use to generate scenes on a computer screen. In other words, it defines a common language the programmers will use to talk about the things OpenGL can do.

This means, that no matter if you are writing your application for Windows or Mac, using C++ or Java, the call used to draw a vertex array will always be `glDrawArrays(...)`. It also specifies the data types and how they are formatted. In this way, anyone can create an OpenGL implementation for any platform or programming language, as long as they follow the specification.

# OpenGL In This Course

In this course, we are using OpenGL with C++. This is the most common "flavor" of OpenGL. Additionally, the API itself is heavily influenced by C and uses many of the same conventions. Finally, GLSL (OpenGL Shading Language) uses a syntax that mirrors C very closely.

By using C++ we can enjoy the API calls matching the rest of our code. It also means that writing our shader code in GLSL will feel similar. If we were to use another language like Python for the application layer, that would create a weird disconnect to the shader code.

# What About All That Other Stuff We Installed?

Good question! Remember, OpenGL is a specification for a cross-platform API. This means it doesn't actually handle putting pixels directly to the scene. OpenGL prefers, and rightly so, to the actual individual target platforms. So, OpenGL will render our entire scene to a frame buffer, and then it is up to the specific platform to convert that to an actual image on the screen. Similarly, OpenGL doesn't concern itself with how an application gets input; it only cares about calls to the API.

Therefore, we need to have libraries to do things like create windows, get keyboard and mouse input, and load our shader code into our applications. Over the course of this term, we will go over all these things, but for now, just know that they are separate from OpenGL.

# Conclusion

We aren't going harp on the distinction between OpenGL as a specification vs OpenGL as an API, we are going to be more result focused. That said, it is something you should keep in the back of your mind in case you ever want to develop using something other than C++. I will likely refer to the syntax of the function calls as the OpenGL API, as that is the term I was exposed when learning this material, so I will forgive you for being imprecise with your language as well!
