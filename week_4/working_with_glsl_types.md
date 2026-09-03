# Introduction

We are about to really get into using GLSL data types. Instead of trying to describe all of them as they are introduced, I wanted to do a quick overview of the types and how to access their values, so you aren't surprised later.

# Vectors

We will be using vectors *a lot*. Already, we have seen that they can be lengths of 2, 3, or 4. They are declared using `vec2`, `vec3`, and `vec4`.

Thus far, we have used them to store *position* and *color* data. You may recall that we can access the values in vectors using `.x` and `.y` when dealing with position. If you paid close attention, you have also seen how you can access it with `.r` when dealing with colors. How does GLSL know what *flavor* of data is stored in any given vector?

Well, it *can't*. GLSL has multiple accessor *flavors* to help us (the programmer) create readable code.

* Position (`x`, `y`, `z`, `w`)
  * `.x` - Component 1 (Horizontal / Width)
  * `.y` - Component 2 (Vertical / Height)
  * `.z` - Component 3 (Depth)
  * `.w` - Component 4 (Homogeneous Coordinate / Scaling Factor)
* Color (`r`, `g`, `b`, `a`)
  * `.r` - Component 1 (Red)
  * `.g` - Component 2 (Green)
  * `.b` - Component 3 (Blue)
  * `.a` - Component 4 (Alpha / Transparency)
* Texture Coordinate (`s`, `t`, `p`, `q`)
  * `.s` - Component 1 (Horizontal Texture Axis / U)
  * `.t` - Component 2 (Vertical Texture Axis / V)
  * `.p` - Component 3 (Depth Texture Axis)
  * `.q` - Component 4 (Scaling Factor)

You should be familiar with most of these (excluding the texture terms), but if you aren't, don't worry; we will be covering them frequently. For now, just know that you can use `.x`, `.r`, and `.s` to get at the value of the first component of any vector.

Using one character returns a scalar: `.x` &rarr; `float`/`int`. Using more characters returns a vector: `.rgb` &rarr; `vec3`. We can even play with the order!

This ability will be very helpful when we need to do cross products, which only work with `vec3`s. We will often use `vec4`s to store our positions (using Homogeneous Coordinates) to make matrix math easier, but we have to strip out the fourth element for lots of math: `vec3 pos = position.xyz;`. We will need to do the same thing when calculating lighting, which is directional, which also requires a `vec3`.

Another neat thing that GLSL allows when it comes to vectors is that it lets us *Swizzle*. No, I didn't make up that term, it is *real*. It means we can access the components in interesting ways.

There is no set *order* or *number* of components you have to access for any given vector. We can even duplicate the components returned!

* `position.yx` is just as valid as `position.xy`
* `color.rb` allows us to completely skip `g`
* `texture.ssss` allows us to return a `vec4` of just the first component
* `position.xyzy` is completely valid

The possibilities are many, as long as you follow one rule. You *cannot* mix the *flavors*! `position.xb` is *not* valid.

# Matrices

Sadly, GLSL doesn't have any unique accessors for matrices. So why am I even bothering to mention them? Well, a `mat4` is just an array of four `vec4`s (each a separate column).

This means that if we want to access a specific column, we can then use our vector accessors on it. For example, the `Model Matrix`'s last column holds the *World Space* coordinates of the model itself: `modelMatrix[3].xyz`. This is just one situation where this feature will be helpful.

# Review

Make sure you are familiar with the concepts of *Flavors* and *Swizzle*. These two features of GLSL will be used frequently, and we will not have time to explain why we are using `.rgb` vs `.xyz`. You need to know that they are equivalent.

Finally, the single-component accessors (`.x`, `.r`, `.s`) also work on GLM vectors, but *swizzling* (`.xyz`, `.rgb`) does not work on GLM vectors by default[^1]. We are going to avoid GLM swizzling in this course.

[^1]: [GLM Advanced Usage](https://glm.g-truc.net/0.9.1/api/a00002.html)