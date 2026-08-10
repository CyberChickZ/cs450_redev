# CLOs

* CLO2 - Describe the names and functions of the elements of the graphics pipeline, as well as the input and output of each stage
* CLO9 - Explain the ways Vertex Buffer Objects can improve your display performance
* CLO10 - Create and use an OpenGL vertex and fragment shader

# Introduction

We have been refactoring our code more and more as the weeks go by. Some of you may find this annoying, but there is a purpose. With the *programmable pipeline* there was a great deal of details to cover before you could get *anything* on the screen. This complexity is great because it gives the developer a great deal of control, but the learning curve is steep.

In an attempt to flatten that curve, I made some design decisions to make conceptualizing the OpenGL process more approachable. Now that we are more familiar with how everything fits together, it is time to start moving towards "best practices." One area our code could use improvement is in how we pass data to the pipeline. We are going to look at *Uniform Buffer Objects* to help make our programs more efficient.

## Uniform Buffer Objects

As the name may suggest, *Uniform Buffer Objects* (*UBO*s) contain *uniforms*. The thing that makes them different, is that *UBO*s can be shared across multiple shader programs. Up until this point, every *uniform* we have set is only accessible in the shader program for which we set it.

So, how does having an *UBO* make this more efficient? You know, that is a great question for you all to ponder!

**HIDE ANSWER: Some data remains constant across many different shaders. It makes little sense to upload it multiple times. Additionally, each time we set a uniform in our application adds overhead to the process. So if we can set it once per program, or once per frame, then we cut down on a lot of wasted CPU processes and GPU memory allocation.**

To drive the point home, go count how many times we query for *Uniform IDs* and set them for our Solar System example last module. That is just a small scene. Now imagine a scene with *thousands* of objects and multiple lights. So we know *how* *UBO*s can improve performance by reducing redundancies of uniforms, but not *which* uniforms are well suited for an *UBO*. Any guesses?

**HIDE ANSWER: We are going to want to convert any uniforms that are updated once per program or once per frame. This includes the *Projection Matrix*, *View Matrix*, and lighting data.**

Reducing redundancy is only one of the reasons we want to move these things into *UBO*s.  From a design perspective, it makes little sense for our objects to have any knowledge (even as a pass-through) of these details. It is the shader programs that need these, so it is better to have it handled outside the object class.

## Shader Refactor

Since we are changing how we assign our uniforms, we are going to need to update our shader code. When converting from a standard uniform to a *UBO*, we need to use a different syntax in the shaders themselves. Just like we did with our *vertex attributes*, we are going to use the `layout` keyword to declare our *UBO*s. The difference is that instead of specifying a `location`, we are going to use `std140`.

### std140 Explained

What is `std140`? It is a standardized memory layout. The layout assigns *base alignments* to each data type:

|Type|Size (bytes)|Base Alignment (bytes)|
|--|--|--|
|`float`, `int`, `bool`| 4 | 4 |
|`vec2` | 8 | 8 |
|`vec3` | 12 | 16 |
| `vec4` | 16 | 16 |
| `mat3` | 48[^1] | 16 |
|`mat4` | 64 | 16 |

The *base alignment* alignments are used to calculate (once) the memory offsets for each member of our *Block* (aka the variables inside the layout). It uses these offsets to determine where to look for the next piece of data. Each member must start at an offset that is a multiple of its *base alignment*, or things can go wrong. In these cases we have to "pad" our data. This will be best explained using an example.

On our application side of things, we have a `struct` for our lighting values:

```C++
struct Light {
    glm::vec3 position {0.0f, 0.0f, 0.0f};
    glm::vec3 ambient  {0.15f, 0.15f, 0.15f};
    glm::vec3 diffuse  {0.8f, 0.8f, 0.8f};
    glm::vec3 specular {1.0f, 1.0f, 1.0f};
    glm::vec3 attenuation{ 0.5f, 0.03f, 0.003f };
};
```

Note, every thing declared in this `struct` is a `vec3`. Each will take up 12-bytes of memory in our application. The CPU *tightly* packs data. The GPU *pads* data. So, given the same variables, each will calculate offsets differently.

|Variable|C++ offset (tight)| std140 offset| Difference|
|--|--|--|--|
| position | 0 | 0 | none |
| ambient | 12 | 16 | +4 |
| diffuse | 24 | 32 | +8 |
| specular| 36 | 48 | +12|
| attenuation| 48|64|+16|

As you can see, after the first `vec3`, the offsets of the CPU and GPU diverge. This means, that when the GPU goes to look for `ambient`, it will use the offset of `16`, which will miss the first 4-bytes of the desire data. This problem just gets worse the more variables we have with mismatched size and base alignments. What can we do?

The answer is, "We pad!" We need to construct our `struct` so that our variables all land on the appropriate offset based upon *base alignment*. Since a `vec3` takes up 12-bytes, we need to "pad" with an additional 4-bytes. Looking at the earlier table, we see that a `float` is 4-bytes. Therefore, as long as we declare a `float` after each `vec3` we will pad the offset correctly for the GPU. Note, we are renaming these variables so we can use them in a UBO (reason explained below).

```C++
struct LightUBO {
    glm::vec3 lightPosition {0.0f, 0.0f, 0.0f};
    float pad0;
    glm::vec3 lightAmbient  {0.15f, 0.15f, 0.15f};
    float pad1;
    glm::vec3 lightDiffuse  {0.8f, 0.8f, 0.8f};
    float pad2;
    glm::vec3 lightSpecular {1.0f, 1.0f, 1.0f};
    float pad3;
    glm::vec3 lightAttenuation{ 0.5f, 0.03f, 0.003f };
    float pad4;
};
```

|Variable|C++ offset |std140 offset| Difference|
|--|--|--|--|
| lightPosition | 0 | 0 | none |
| pad0    | 12 | 12 | none |
| lightAmbient | 16 | 16 | none |
| pad1 |28 | 28 |none|
| lightDiffuse | 32 | 32 | none |
|pad2 | 44 | 44 | none |
| lightSpecular| 48 | 48 | none |
| pad3 | 60 | 60 | none |
| lightAttenuation| 64|64|none|
| pad4 | 76 | 76 | none |

Notice how each `float` pushes the start of the next `vec3` to *align* with where the GPU expects to find it. We add in `pad4` at the end because `std140` requires all blocks to have a size that is a multiple of 16-bytes (80-bytes for this block).

Make sure you got this locked down before moving on, otherwise things are going to get *really* confusing.

### Shader Changes

The syntax we will use to declare our `std140` block is:

```GLSL
layout(std140) uniform VariableName {
    // block member1
    // block member2    
};
```

Let's take a look at our Vertex Shader code and try to figure out which uniforms to convert into *UBO*s.

```GLSL
// Vertex Shader
#version 410 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 uMV;
uniform mat4 uP;
uniform mat3 uN;

out vec3 fragPos;
out vec3 normal;
out vec2 texCoord;

void main()
{
    fragPos  = vec3(uMV * vec4(aPos, 1.0));
    normal   = uN * aNormal;
    texCoord = aTexCoord;

    gl_Position = uP * uMV * vec4(aPos, 1.0);
}
```

As we mentioned above, we want to store our *Projection Matrix* and *View Matrix* in a *UBO*. We *could* put each in its own *UBO*, but since they are both related to the *camera*, why not combine them into a single uniform? We can do that easily with:

```GLSL
layout(std140) uniform Camera {
    mat4 uV;
    mat4 uP;
};
```

**NOTA BENE**: we don't have to do any padding here. A `mat4`'s size matches its *base alignment*. Additionally, the `Camera` block is 128-bytes, which is a multiple of 16-bytes.

Reminder: Previously, we sent in our *Model View Matrix* as `uMV`. Now, we are sending the *View Matrix* separate from the *Model Matrix*, so we will have to rename our `uniform mat4 uMV` to `uniform mat4 uM`. This also means we will need to update what we fill it with in our application.

Additionally, since the goal is to stop sending the *View* and *Projection* matrices to `draw()`, we can no longer calculate the *Inverse Normal Matrix* inside our object.[^2] This means, we need to remove the `uniform mat3 uN` and calculate it inside `main()`. **NOTA BENE**: we want to keep our `aNormal` unchanged; these are *vertex attributes* and need to be updated every `draw()` call.

After making all the changes, we will have the following Vertex Shader code:

```GLSL
// Vertex Shader
#version 410 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

layout(std140) uniform Camera { // New UBO code
    mat4 uV;                    // Separate View Matrix
    mat4 uP;                    // Separate Projection Matrix
};
uniform mat4 uM;                // Separate Model Matrix

out vec3 fragPos;
out vec3 normal;
out vec2 texCoord;

void main()
{
    mat4 mv = uV * uM;                      // Create a new Model-View Matrix
    fragPos  = vec3(mv * vec4(aPos, 1.0));  // updated to use the new mv variable
    mat3 n = mat3(transpose(inverse(mv)));  // Calc inv transpose in shader now
    normal = n * aNormal;                   // use the new n variable
    texCoord = aTexCoord;

    gl_Position = uP * mv * vec4(aPos, 1.0); // Updated to use the new mv variable
}
```

Now we need to update our Fragment Shader. Now, this is going to get *weird*, and I am sorry. Therefore, take a close look at the following code before moving on to how we are going to update it.

```GLSL
// Fragment Shader
#version 410 core

in vec3 fragPos;
in vec3 normal;
in vec2 texCoord;

struct Light {
    vec3 position;        // already in view space
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 attenuation;
};
uniform Light uLight;

float constant  = uLight.attenuation.x;
float linear    = uLight.attenuation.y;
float quadratic = uLight.attenuation.z;

uniform float uTime;

uniform vec3  uObjectColor;
uniform float uShininess;
uniform sampler2D uTex;
uniform int useTexture;

out vec4 FragColor;

void main()
{
    // shader code
}
```

The first thing we will do is get rid of our `struct` declaration (we will still use it in our application). We are going to replace it, and the uniform `uLight` with the following:

```GLSL
layout(std140) uniform LightBlock {
    vec3  lightPosition;
    float pad0;
    vec3  lightAmbient;
    float pad1;
    vec3  lightDiffuse;
    float pad2;
    vec3  lightSpecular;
    float pad3;
    vec3  lightAttenuation;
    float pad4;
};
```

As you can see, we are declaring a `float` as padding between each `vec3`, just like we did in our application's `struct`.[^3] Previously we had to access each of these through the `uLight` variable (e.g. `uLight.position`). Now we can just access them directly, which is why we needed to rename them to make it clear what data they hold; `position` could mean multiple things, but `lightPosition` leaves nothing to the imagination. So, let's go and update the shader code to reflect our new *UBO* data. Pay attention to all the places we had to update the names of variables.

```GLSL
// Fragment Shader
#version 410 core

in vec3 fragPos;
in vec3 normal;
in vec2 texCoord;

layout(std140) uniform LightBlock {  // replace the struct and uLight uniform
    vec3  lightPosition;
    float pad0;
    vec3  lightAmbient;
    float pad1;
    vec3  lightDiffuse;
    float pad2;
    vec3  lightSpecular;
    float pad3;
    vec3  lightAttenuation;
    float pad4;
};

float constant  = lightAttenuation.x;   // Update variable name
float linear    = lightAttenuation.y;   // Update variable name
float quadratic = lightAttenuation.z;   // Update variable name

uniform float uTime;

uniform vec3  uObjectColor;
uniform float uShininess;
uniform sampler2D uTex;
uniform int useTexture;

out vec4 FragColor;

void main()
{
    vec3 N = normalize(normal);
    vec3 L = normalize(lightPosition - fragPos);   // Update variable name
    vec3 V = normalize(-fragPos);
    vec3 H = normalize(L + V);

    // Distance attenuation (point light)
    float distance = length(lightPosition - fragPos);  // Update variable name
    float attenuation = 1.0f / (constant + linear * distance + quadratic * distance * distance);

    vec3 ambient = lightAmbient;  // Update variable name
    vec3 diffuse = max(dot(N, L), 0.0) * lightDiffuse * attenuation; // Update variable name
    vec3 specular = pow(max(dot(N, H), 0.0), uShininess) * lightSpecular * attenuation;  // Update variable name

    vec3 lightingColor = (ambient + diffuse + specular) * uObjectColor;
    if (useTexture == 1)
        lightingColor *= texture(uTex, texCoord).rgb;

    FragColor = vec4(lightingColor, 1.0);
}
```

As you can see, other than the weird padding and renaming variables, everything is pretty much the same as before.

## Application Refactor

We already covered the trickiest concept that we have to handle in our C++ code: *padding*. The rest is just a matter of moving this around to fit our new approach (e.g. removing the `view`, `projection`, and `light` from our `draw()` calls).

### Object Class

We are going to start in our Object class. Hopefully, you have been keeping up with the file changes (and making *new* files, not overwriting old ones), but in case you fell off, here are copies you can use: [object_light.hpp](../downloadable_files/week_9/object_light.hpp) and [object_light.cpp](../downloadable_files/week_9/object_light.cpp). I renamed these to `object_ubo.hpp` and `object_ubo.cpp` to help me keep track of what changes we are making.

Each *UBO* requires a *binding point*. We are going to specify these ourselves at the top of the header file. We declare them as `const` because we don't want to accidentally change them. Make sure you *do not* put these *inside* the class declaration.

```C++
const GLuint CAMERA_BINDING_POINT = 0;
const GLuint LIGHT_BINDING_POINT  = 1;
```

Now we need to add our new *padded* `LightUBO` struct next to our old `struct Light`; we don't want to use the padded version unless needed.

```C++
struct LightUBO {
    glm::vec3 lightPosition;
    float     pad0;
    glm::vec3 lightAmbient;
    float     pad1;
    glm::vec3 lightDiffuse;
    float     pad2;
    glm::vec3 lightSpecular;
    float     pad3;
    glm::vec3 lightAttenuation;
    float     pad4;
};
```

Since we are passing in our camera and lighting data outside our objects, we need to update our `draw()` signatures. The only thing we need to keep are `time` and `modelFromStack` (for our overloaded function).

```C++
void draw(float time) const;
void draw(const glm::mat4& modelFromStack, float time) const;
```

It only makes sense to jump over to our `object_ubo.cpp` and make the changes to `draw()`, before we forget! We are going to use this as an opportunity to streamline these two functions. The two `draw()` share the same code except in how it calculates `mv`:

```C++
// original draw()
glm::mat4 mv = view * modelMatrix;

// overridden draw()
glm::mat4 finalModelMatrix = modelFromStack * modelMatrix;
glm::mat4 mv = view * finalModelMatrix;
```

I am going to propose something *crazy*, but hear me out. *What if* we passed in the *Identity Matrix* as `modelFromStack`? Do you remember what we called the *Identity Matrix*?

**HIDE ANSWER: That's right! *The Do Nothing Matrix*. If `modelFromStack` is the *Identity Matrix* then `mv` is the same thing in either version of the function.**

Therefore, we can completely *remove* our original `draw()` function and replace it with:

```C++
void Object::draw(float time) const
{
    draw(glm::mat4(1.0f), time);  // Passes the Identity Matrix as modelFromStack
}
```

Doing this will save us *a lot* of time later if we ever want to update `draw()` again. Oh, wait! We *do* want to update `draw()` right now. How convenient.

Remember that our shader no longer uses a combined *Model-View Matrix* and instead just takes the *Model Matrix* as `uM`. Therefore, we need to update the code in our overridden `draw()` function. We need to remove our `mv` calculation and instead set the `uM` variable to be our `finalModelMatrix`.

```C++
GLint mLoc = glGetUniformLocation(program, "uM");
if (mLoc >= 0) {
    glUniformMatrix4fv(mLoc, 1, GL_FALSE, glm::value_ptr(finalModelMatrix));
} else {
    std::cerr << "Failed to get uM location" << std::endl;
}
```

Next, we need to remove everything associated with `uP`, `uN`, and `uLight`; This includes any calculations and any functions that set the uniforms. After all this, our function will be much shorter!

Next, we will need a new function to bind our new *UBO*s to our shader program. Add the following signature to the header file under `private`:

```C++
void bindUniformBlocks();
```

Then, in `object_ubo.cpp` add the following function.

```C++
void Object::bindUniformBlocks() {
    GLuint camIndex = glGetUniformBlockIndex(program, "Camera");
    if (camIndex != GL_INVALID_INDEX) {
        glUniformBlockBinding(program, camIndex, CAMERA_BINDING_POINT);
    }

    GLuint lightIndex = glGetUniformBlockIndex(program, "LightBlock");
    if (lightIndex != GL_INVALID_INDEX) {
        glUniformBlockBinding(program, lightIndex, LIGHT_BINDING_POINT);
    }
}
```

While this looks similar to how we set *uniforms* in the past, here we are *binding* the *UBO* to the shader program. Whereas vanilla *uniforms* can only be accessed by one shader program, any shader program can access a *UBO* as long as it has been *bound* (aka connected). We need to add a call to `bindUniformBlocks()` right after `setupBuffers()` in `Object::init()`.



### Application

Now need to get our application set up for using *UBO*s. We are going to start by declaring two global variables to hold our *UBO* IDs.

```C++
GLuint cameraUBO = 0;
GLuint lightUBO = 0;
```

Next, we need to create a function to handle the binding of our buffer data.  This should look familiar as it is similar to how we handled our *VBO* data in earlier lessons. Back then, we:

1. Generated our buffers
2. Bound our buffers using an ID
3. Filled them with `glBufferData()`

For our *VBO*s we need to add one more step: bind a *buffer base*. Take a look at the full function below. I will see you on the other side to go over the details.

```C++
void createUBOs()
{
    // Camera UBO
    glGenBuffers(1, &cameraUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, cameraUBO);
    glBufferData(GL_UNIFORM_BUFFER, 
                 2 * sizeof(glm::mat4), 
                 nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 
                     CAMERA_BINDING_POINT, 
                     cameraUBO);

    // Light UBO
    glGenBuffers(1, &lightUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, lightUBO);
    glBufferData(GL_UNIFORM_BUFFER, 
                 sizeof(LightUBO),
                 nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER,
                     LIGHT_BINDING_POINT,
                     lightUBO);

    glBindBuffer(GL_UNIFORM_BUFFER, 0);  // Unbind buffers
}
```

Let's highlight what is the same and what is different.

* Both use `glGenBuffers()` in the same way
  * 1<sup>st</sup> argument - how many buffers to generate
  * 2<sup>nd</sup> argument - the variable used to save the ID
* Both use `glBindBuffer()` similarly
  * 1<sup>st</sup> argument - the type of buffer. *UBO*s use `GL_UNIFORM_BUFFER` (duh) vs. `GL_ARRAY_BUFFER`
  * 2<sup>nd</sup> argument - the buffer ID
* Both use `glBufferData()` in similar ways
  * 1<sup>st</sup> argument - `GL_UNIFORM_BUFFER` vs. `GL_ARRAY_BUFFER`
  * 2<sup>nd</sup> argument - The size of the data stored in the buffer
  * 3<sup>rd</sup> argument - The *VBO* directly uploads the data, the *UBO* will have to wait until it is generated (e.g. each frame), hence `nullptr`
  * 4<sup>th</sup> argument - Specifies how the buffer will be used, which the GPU uses to decide where to place it in memory
    * *VBO* - `GL_STATIC_DRAW` upload once and draw with it many times
    * *UBO* - `GL_DYNAMIC_DRAW` data will change regularly
* Only *UBO*s use `glBindBufferBase()`
  * 1<sup>st</sup> argument - the type of buffer (`GL_UNIFORM_BUFFER`)
  * 2<sup>nd</sup> argument - which binding point to use (defined in our Object header)
  * 3<sup>rd</sup> argument - the ID of the buffer to place in the slot defined in the previous argument

As you can see, most of it is similar to how we handled other buffers. One thing to note is that the `glBindBufferBase()` is what makes the buffer visible to shader programs that connect to it (details to come). Just like the *VBO*s, we only need to bind our *UBO*s once in our application.

Write this function somewhere above `main()`. Then, in `main()`, find where the context is set (after `init(window)`), and add:

```C++
createUBOs();
```

That is it for binding the buffers. Now we need to write code to update and fill them; remember, there is no data in them after we call `createUBOs()`, the memory was just allocated. We are going to write a function for each of our *UBO*s. First, let's handle the `cameraUBO`. In this *UBO* we need to pass our `view` and `projection` matrices, so they are passed in.

```C++
void updateCameraUBO(GLuint cameraUBO, const glm::mat4& view, const glm::mat4& projection)
{
    glBindBuffer(GL_UNIFORM_BUFFER, cameraUBO);
    glBufferSubData(GL_UNIFORM_BUFFER,
                    0,
                    sizeof(glm::mat4),
                    glm::value_ptr(view));
    glBufferSubData(GL_UNIFORM_BUFFER,
                    sizeof(glm::mat4),
                    sizeof(glm::mat4), 
                    glm::value_ptr(projection));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
```

We again have to *bind* our buffer so OpenGL knows which one we want to manipulate. Then we need to fill the different `Camera` block members. We do this with `glBufferSubData()`. We call this twice: once for `view` and once for `projection`.

Let's break down the `glBufferSubData()` parameters

1. The type of buffer (`GL_UNIFORM_BUFFER`)
2. The *offset* where the block member starts
3. The size of the block member to be set
4. The value with which to fill the block member

**NOTA BENE**: Our second call uses an *offset* of `size(glm::mat4)`. Why do you suppose that is?

**HIDE ANSWER: It is because `view` is a `mat4`, so `projection` starts immediately after. Remember, `mat4`s have a size that is a multiple of 16-bytes, so they naturally fall along the *base alignment***

Finally, we have to unbind the buffer with `glBindBuffer(GL_UNIFORM_BUFFER, 0)`.

Now, let's take a look at our function to bind our `LightBlock` *UBO*. This function takes in a the `view` matrix so we can calculate the light's position. It also takes in the `Light` struct we defined and filled in `display()`.

```C++
void updateLightUBO(GLuint lightUBO, const Light& light, const glm::mat4& view)
{
    LightUBO data;
    data.lightPosition    = glm::vec3(view * glm::vec4(light.position, 1.0f));
    data.lightAmbient     = light.ambient;
    data.lightDiffuse     = light.diffuse;
    data.lightSpecular    = light.specular;
    data.lightAttenuation = light.attenuation;

    glBindBuffer(GL_UNIFORM_BUFFER, lightUBO);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(LightUBO), &data);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
```

First we declare a `struct` called `data` using our updated (and padded) `lightUBO`. We then transfer our light values from `light` to `data`. We then follow the same steps we used for `updateCameraUBO()`, but this time we only need one `glBUfferSubData()` call because we get to upload the entire struct at once! Before, when we were updating vanilla uniforms in our struct, we had to do it one at a time. Also note that we don't have to specify any values for the padding `float`s. This is because they are only their to make sure the data follows `std140` spacing requirements.

We now have a way to fill our *UBO*s, we just need to call these functions. Any ideas where we want to call them?

**HIDE ANSWER: Of course we want to call these in `display()`. The *View Matrix* can update each frame so both of these *UBO*s need to be kept up-to-date.**

So, in `display()`, after `view` is filled, add our two new calls:

```C++
updateCameraUBO(cameraUBO, view, projection);
updateLightUBO(lightUBO, myLight, view);
```

With our *UBO*s filled, we can now draw our objects to our hearts' content. One thing to note though, if you are editing an application with existing `draw()` calls, you will need to update to reflect the changes we made to them. It may be fun to try to take your code from *Matrix Stacking* and testing it with our new *UBO*s.

# UBO Wrapup

Ok, I know what you are thinking. "We did *ALL* that work to only end up with the same thing on screen as before!" This is a fair observation, but what we did here is actually important.

First, we greatly increased the efficiency of our application. With the small scenes we are dealing with, it likely won't be felt. Yet, as you start creating more elaborate scenes, limiting how many times you have to upload data to the GPU will increase performance.

Additionally, by using *UBO*s we allow multiple shaders to use the same data, so the GPU memory isn't wasted on redundancies. Remember how we created a separate shader for our Sun to animate the texture? Under the old approach, each of our shader programs had their own version of the *Projection Matrix* and *View Matrix* (though combined with the *Model Matrix*). Now imagine an application with *dozens* (if not *hundreds*) of shader programs. Most would likely need access to these matrices, and it would eat up RAM like whoa if each shader program had their own copy.

But more importantly, using *UBO*s is pretty much required for any advanced graphics techniques. For example, you likely want to have multiple lights in a scene. Uploading each individually as a uniform would be very tedious. With *UBO*s you can have a large array of light structs and upload those all at once for your shader to iterate over and apply each to the scene.

Other techniques that require *UBO*s:

* Shadow Maps
* Deferred Rendering
* Instancing
* Physically Based Rendering

So, rest assured that our efforts were *not* wasted.

As always, I want to encourage you to explore what we just learned. The first step should be converting one of our older programs into one that uses *UBO*s. The conversion process will help highlight all the pieces that are different. Then, I have some suggestions for you to really push yourself:

* Try adding multiple lights to a scene using a *UBO*
* Create multiple shaders that all use the same *UBO*
* Get rid of the padding of our `struct LightUBO` and see what happens

Feel free to share your experiments on the discussion board!

[^1]: A `mat3` is stored as three `vec3`s, but each has a *base alignment* of 16: 3 * 16 = 48 (not the expected 36).
[^2]: From our previous version of `draw()`: `glm::mat3 n = glm::mat3(glm::transpose(glm::inverse(mv)));`
[^3]: *Technically*, we don't need to do this because the GPU automatically rounds the offset up to the next multiple of the *base alignment*. We are explicitly declaring the padding variables to keep things consistent and to make clear what is happening in memory.
