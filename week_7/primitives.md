# CLOs

* CLO 2 - Describe the names and functions of the elements of the graphics pipeline, as well as the input and output of each stage.

# Introduction

Think back to when we went over the [OpenGL Pipeline](../week_2/opengl_pipeline.md). What came right before *Rasterization*?

**HIDE ANSWER** That's right! After all the shaders are processed (excluding the Fragment Shader), the pipeline moves into *Primitive Assembly*. But what is a *primitive*?

*Primitives* are "shapes" that tell OpenGL how to group/treat vertices passed into the pipeline after a *draw* call (e.g. `glDrawArrays()`). We have already seen one such *primitive*: `GL_POINTS`. By specifying `GL_POINTS` we instructed OpenGL to treat each vertex as a single point and render it. You can imagine that this would get very tedious if we could only render single points to draw our scenes.

Fortunately, OpenGL supports *many* different types of primitives.

# Primitive Types

OpenGL has access to *seven* "core" primitive types when using the programmable pipeline.[^1] You may not use all of them in this course, but it is useful to have some knowledge of each. We can categorize these into three distinct groups:

* Point
* Line
* Triangle

Let us take a look at each in turn.

## Point Primitive

### GL_POINTS

As mentioned above, we have already seen and used this type of primitive in our [Get to the Point](../week_2/get_to_the_point.md) exploration. When using `GL_POINTS` with any draw call, each vertex will pass through the pipeline as a single point. So if we pass in four vertices, we will get four distinct points (shown below).

![GL_POINTS demonstration showing four independent points](../images/undesignated_images/gl_points.svg)

This may sound fairly mundane, but there are some very good uses for `GL_POINTS`. One common usage is in particle systems.[^2] Since points don't have any geometry, they are exceedingly efficient to generate and display. We can adjust the size of each and even have textures applied so they display as sprites.

## Line Primitives

Unlike when using `GL_POINTS`, vertices sent into the pipeline are grouped together to draw lines. What type of line depends on the exact primitive specified.

### GL_LINES

For each two vertices processed, OpenGL will draw a line between them. If four points are passed in, then the first and second form a line and then the third and fourth form a second line (shown below). If an odd number of vertices are sent, then the last one is ignored. 

![GL_LINES demonstration. v0 connects to v1 and v2 connects to v3](../images/undesignated_images/gl_lines.svg)

It may seem weird to generate disconnected lines, but they can be used effectively when creating dashed-outlines or displaying vector fields.[^3]

### GL_LINE_STRIP

Very similar to `GL_LINES`, but this time, all the vertices are connected in one continuous line (shown below).

![GL_LINE_STRIP demonstration. v0 connects to v1, v1 connects to v2, and v2 connects to v3](../images/undesignated_images/gl_line_strip.svg)

`GL_LINE_STRIP` is useful when creating solid outlines or even rendering things such as hair or grass.

### GL_LINE_LOOP

`GL_LINE_LOOP` is exactly the same thing as `GL_LINE_STRIP`, but it connects the first and last vertices automatically (shown below). Please note, that the same behavior can be reproduced using `GL_LINE_STRIP`, but would require sending in the first vertex twice (once at the start and once at the end).

![GL_LINE_LOOP demonstration. v0 connects to v1, v1 connects to v2, v2 connects to v3, and v3 connects to v0](../images/undesignated_images/gl_line_loop.svg)

This primitive is very useful for creating polygonal shapes or outlines of objects.

## Triangle Primitives

In many ways, *triangles* are the workhorse of any graphics rendering system. While you may hear of some systems using "quads", remember, each quad can be broken up into two triangles. In fact, many 3D art tools will convert the quads used in the editors to triangles when exporting for use when rendering (or the application splits them)

One reason triangles are preferred for rendering, is that, by definition, they must be *planar*. This means that no matter where in 3D space the three vertices are, it is possible to draw a flat surface that all intersect with. This is not true with quads. Additionally, with triangle meshes[^4], we don't have to worry about the issue of having concave polygons, which cause issues with oh so many things: rendering, collision detection, and pathfinding (to name a few).

### GL_TRIANGLES

Just like how `GL_LINES` created a line out of every two vertices processed, `GL_TRIANGLES` creates a triangle out of every *three* vertices sent into the pipeline (shown below). Please note, that no vertex is reused.

![GL_TRIANGLES demonstration. v0, v1, and v2 form one triangle. v3, v4, v5 form another triangle](../images/undesignated_images/gl_triangles.svg)

You wouldn't want to create a mesh using `GL_TRIANGLES`, but they are still very useful for things like particle systems or billboarding[^5].

### GL_TRIANGLE_STRIP

`GL_TRIANGLE_STRIP` creates a continuous series of connected triangles automatically (shown below). You could recreate this behavior using `GL_TRIANGLES`, but you would have to pass in vertices multiple times, which isn't efficient.

![GL_TRIANGLE_STRIP demonstration. v0, v1, and v2 form a triangle. v1, v2, and v3 form a triangle. v2, v3, and v4 form a triangle. v3, v4, and v5 form a triangle](../images/undesignated_images/gl_triangle_strip.svg)

As mentioned above, this primitive is very useful for rendering meshes that are continuous. Object files don't typically use this because the faces aren't often stored in a way that has triangles arranged in a strip. That said, terrain and cloth are often structured this way, so `GL_TRIANGLE_STRIP` is used.

### GL_TRIANGLE_FAN

`GL_TRIANGLE_FAN` also creates connected triangles, but this time, each triangle starts from the first vertex processed (shown below).

![GL_TRIANGLE_FAN demonstration. v0, v1, and v2 form a triangle. v0, v2, and v3 form a triangle. v0, v3, and v4 form a triangle. v0, v4, and v5 form a triangle](../images/undesignated_images/gl_triangle_fan.svg)

This primitive is very useful if you ever need to create circles or cones. You could also use it to create spirals (perhaps for a staircase).

## Primitives not listed above

There actually *more* primitives available for OpenGL, but they each require either *Geometry Shaders* or *Tessellation Shaders*. Therefore, we won't be using these at all in this course. For those that just want a complete list, here are the ones not mentioned above.

* `GL_LINES_ADJACENCY`
* `GL_LINE_STRIP_ADJACENCY`
* `GL_TRIANGLES_ADJACENCY`
* `GL_TRIANGLE_STRIP_ADJACENCY`
* `GL_PATCHES`

The first four are used with Geometry Shaders, and behave similarly to the non-adjacency versions. The big difference is that the *lines* or *triangles* also have access to nearby vertices. This is necessary for the Geometry Shader to alter the vertices and still keep everything connected.

The last one is used exclusively by Tessellation Shaders. Basically, instead of sending in a fully fleshed out mesh, we send in a *patch* that contains just enough vertices to allow the GPU to subdivide into the desired level of detail. For example, imagine sending in just four vertices to describe the outline of a body of water and having the tessellation shader "fill in" the parts in-between. This reduces greatly the number of vertices your meshes need to hold.

# Wrapping it up

Make sure you know these primitives well. You will need to use at least one of them each time you call a draw function. Which one you will want to use will depend on the specific task you are attempting to accomplish. When working on your projects in this course, I highly recommend you experiment by swapping out one primitive for another to see how the scene changes.[^6]

[^1]: There are in fact *twelve* primitives total. The four not listed are used exclusively with Geometry Shaders, which won't be used in this course.
[^2]: [Particle Systems](https://opengl-notes.readthedocs.io/en/latest/topics/particles-mapping/particles.html)
[^3]: [Vector Field](https://en.wikipedia.org/wiki/Vector_field)
[^4]: [Polygon Mesh](https://en.wikipedia.org/wiki/Polygon_mesh)
[^5]: Creating a 2D object that is always facing the camera, thus giving the impression of it being 3D.
[^6]: Most will fail horribly, but that is half the fun!
