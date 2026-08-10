# CLOs

* CLO1 - Explain the different coordinate systems used in computer graphics

# Indtroduction

We just covered Left-Handed vs. Right-Handed coordinate systems. This is very important when visualizing how we will move about our 3D scene, but ultimately, since we are only using one (Right-Handed) in this course, it is not something you need to consider that often. On the other hand, what we are about to discuss needs to be at the front of your mind every step of the way.

# Space

Who is excited to talk about *space*!?

<div align="center" markdown="1">

![Image of Galaxy UGC 2885 provided by esahubble.org](../images/week_2/space.jpg)

</div>

No, not that kind of *space*! Hopefully you are all still excited, because we need to talk about *Object, World, View, and Clip Space*.

## Local/Model Space

We have discussed that our program will convert the vertices into pixels on a screen. Those vertices will represent all sorts of things. Some of them will be part of the landscape/terrain and fairly static. Others will be part of objects or models that we will animate in order to have them move around the scene.

Typically, each of these elements will be created using 3D modeling software (e.g. Blender or Maya). These models need to have a way to describe where each vertex is in relation to the others (of the same model), and, typically, we do this by orienting all of them around the origin (0, 0, 0).

The term we are going to use most often to describe this *Object Space*. There are other terms you will see, but since OpenGL uses *Object Space*, we will follow its lead. Those other terms are: *Local Space* and *Model Space*.

Each model/object will have its own set of local coordinates. It is even possible to combine multiple objects/models into a larger unit. For example, we may wish to model a head separately from the body and then combine them by using transforms to translate, scale, rotate each element into the correct place. This combined model would also have its own *Object Space*.

## World Space

When placing objects into our 3D scene we can't use the models' vertices as-is. If we were to do so, then all the objects would be placed at the origin: all on top of one another. Clearly, this would not be what we are looking to do.

We will need to translate the model to where we want it. We will need to scale the model to ensure it is the correct size. We will need to rotate the model so it faces the direction we need. Later, we will learn that we can combine all these actions into something called the *Model Matrix*.

By applying the *Model Matrix* to our objects, we move them from *Object Space* into *World Space*.

## View Space

Ok, so we have discussed how models have their own *object* space. We then went over how we convert that into *world* space. At this point, our models are nicely placed in our 3D world. What is lacking, is any sense of where *we*, the user, is within this world.[^1]

We can put ourselves into our scenes using a *camera*. OpenGL has a built-in camera, that is centered at the origin (0, 0, 0) and pointing along the negative Z-axis. Having the camera in a fixed location pointing in a fixed direction isn't really helpful.

Therefore, we need to transform our objects from *World Space* into *View Space*. We will also call this *Eye Space* due to it simulating our eyes in the scene. During this transformation, we will move all the vertices around to reflect the desired orientation of the camera (yes, we move *the world* to point the camera!).

Later, we will generate something called the *View Matrix*. Just like how the *Model Matrix* was used to move coordinates from *Object Space* to *World Space*, the *View Matrix* will move coordinates from *World space* into *View Space*.

## Clip Space

So, now we have all the vertices moved around so they are oriented around our camera. This was an important step, but we still need to do more before we can get pixels on the screen. Why do you think we can't just stop here?

**Hide Answer: Our scene contains things that are *behind* our camera (or otherwise outside our field of view), and we can't actually display those vertices. Having our GPU waste cycles performing calculations on things we can't *see* is not a good idea.**

We need a way to tell the computer which vertices of the scene are visible at any given time. "But wait! Didn't we handle that in converting to *View Space*?", you may ask. When doing that conversion, we just moved all the objects in the world and reoriented them in relation to the camera's position. Let's do a thought experiment.

Imagine you are a combat pilot. Let's call you *Maverick*. Now, imagine you have a weapons operator with you called...I don't know...*Goose*. As you focus on flying and what is directly ahead of you, Goose can point out other enemy jets by saying, "Bogie at 6 o'clock!" This is a relative location describing a location *behind* you, not the GPS coordinates of the jet. That is what *View Space* does, converts GPS coordinates into something relative to the user.[^2]

Let's look at a rough diagram showing part of the process used to determine what is visible.

<div align="center" markdown="1">

![Eye Space diagram](../images/week_2/eye_space.svg)

</div>

At the top right you will see the *eye* (sometimes called the *camera*). It has an *up* vector, indicating which direction is...uhh...up in relation to the eye. It also has a *look* vector, indicating in which direction the eye is...uhh...looking. In our example, the eye is looking directly at the red cube, which has been placed in our *world space*, but transformed into *view space*.

Out of the eye we can see four lines labeled the *view volume*. The edges of this volume create something called the *projection plan*. You can see a mini red cube on the plane, but you cannot see the yellow triangular prism in the plane. This is because it is outside our viewing volume (aka outside our field of vision).

We will need to calculate something called the *Projection Matrix* (covered later in the course) and apply it to the combined *Model* and *View* matrices. This operation will result in our vertices being moved into *Clip Space* (another name for the *project plane* is the *clipping plane*).

At this stage, our vertices are ready to be sent off to the rasterizer, which will handle *clipping* our scenes to the vertices within the view volume (cutting out the rest). Not only does this determine the exact vertices that are visible, it also limits waste GPU cycles.

# Coming back to Earth[^3]

This information was originally presented with a *ton* of math in a later week, but decided that it was more important to present the key concepts early in the term. We will be fleshing out each of these matrices and how to use them in the coming weeks. For now, just try to visualize the differences between all these *spaces*. Having a solid mental model as we write our first programs will be a great boon to you.

[^1]: This is the "royal" we, so no grammar issue here!
[^2]: I just realized, after writing that analogy, I could have just said the last line and left out my reference to *Top Gun*, but what's the fun in that?
[^3]: Would you believe that I actually dislike puns? I have no idea why I am constantly using them in my courses.