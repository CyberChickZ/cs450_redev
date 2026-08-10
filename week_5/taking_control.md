# CLOs

* CLO3 - Create a dynamic 3D scene using OpenGL

# Introduction

We have come a long way, but we are passive observers of our scenes. One way to make us feel more connected to our scenes is to make them responsive to *us*. Therefore, we are going to set up some new functions to process keyboard and mouse inputs and convert those into changes in our scene.

# How to be in Control

Managing input was traditionally a very difficult thing to do in the early days of gaming. Each device had its own driver that had to be managed in order to send and receive data. Luckily, things have come a long way and much of this has been standardized. Furthermore, we have chosen to use GLFW to manage much of the operating side of things, which allows us to not worry about OS specific details and instead focus on more important matters.

To get keyboard input using GLFW is simply a matter of writing a callback function and registering it with `glfwSetKeyCallback()`. This will allow us to process *single* button presses. Think of these types of actions as "toggles:" things that get turned on and off.[^1]

When specifying a specific action to a specific key-press, it is common to use the term *key mapping*. Below, is the basic structure used to set up key mapping.

```C++
void key_callback(GLFWwindow * window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_P && action == GLFW_PRESS) {
        // do a thing
    }
}
```

The code above specifies the callback we will use to gather all our key presses. The parameters must be set up exactly as shown, otherwise it won't work. Let's look at each of them.

* *window* - this is the window we are going to be looking for key presses in. In this course, we will only ever have the one window, but it is technically possible to have multiple.
* *key* - numerical value representing the *key* pressed. Each value is defined using a human-readable macro (called "Key Tokens"). `GLFW_KEY_P` has the value of `80`.[^2]
* *scancode* - platform-specific hardware code for keys (rarely used).
* *action* - specifies the *event* related to the key. `GLFW_PRESS` means the key has been pressed down whereas `GLFW_RELEASE` means the key has been released. There also is `GLFW_REPEAT` for a key being *held* down.
* *mods* - identifies if any *modification* keys were also pressed/released/held at the time. For example, `GLFW_MOD_SHIFT` would indicate the `key` was pressed at the same time as the shift button. Note, these are accessed using a `bitwise AND` (`mods & GFLW_MOD_SHIFT`).

After writing the callback function, we need to *register* it with our window. So, in `main()` we need to place it anywhere after we create the window, but before our loop. Let's put it right next to our other callback (`glfwSetFramebufferSizeCallback()`).

# Taking Control

Now that we have the basics out of the way, let's see if we can take control of our 3D cube from last week. As always, create a new project for this lesson. We are going to base it on where we left off last time ([our_first_3d_scene.cpp](../downloadable_files/our_first_3d_scene.cpp)). Copy this code into a new `.cpp` so you don't overwrite your previous work. I am not going to provide the shader code this time, as by now you should have it down pretty solidly (or you can skip back to the previous module).

Take the code provided above for the callback function, and copy-and-paste it into your application under where we wrote the framebuffer callback. So that we can see if we have set it up correctly, replace `// do a thing` with `std::cout << "P key pressed" << std::endl;`. Now, in `main()`, right after we set the framebuffer callback, add `glfwSetKeyCallback(window, key_callback);`.

Please build and run your code to ensure this all works correctly before moving forward. With the render window open, press the "P" key and look at the console window. You should see "P key pressed" each time you press the key. This means it is working.

Now, let's make the key press *do something*, but what? Any guesses as to why I picked the "P" key?

**Hide Answer: We are going to PAUSE our animation!**

In order to allow our key press to stop our animations, we need to make some changes. First, we need to create a *toggle flag* which will hold the current state of "pausedness" (not a word, but we are going with it!). Each time we press the key, we want the flag to flip to the opposite.

So, at the top of our file, where we have our globals, add `bool paused = false;`. When `paused` is `true`, our animations won't run and vice versa.

Next, we want to write a function to manage the toggling of this flag. We *could* put this in the callback function, but we may want to have many key mappings and things would get cluttered. Therefore, create a new function somewhere before our callback function declaration.

```C++
void togglePause() {
    if (!paused) {
        paused = true;
        std::cout << "PAUSED" << std::endl;
    } else {
        paused = false;
        std::cout << "UNPAUSED" << std::endl;
    }
}
```

Simple, right? Note, I added some output statement to help us keep track of the current state of the flag (helpful for debugging). Now, we need to call this in our actual callback by replacing our `cout` statement in our callback's if statement.

Now, flipping a flag back and forth does us no good, unless we use it for something. Since our animations are based on *time*, we need to go where we first access it (in `main()` when we call `display()`). We want to remove the `glfwGetTime()` call and use it in concert with our `paused` flag.

```C++
double animationTime = 0.0f;
while(!glfwWindowShouldClose(window)) {
    if (!paused) {
        animationTime = glfwGetTime();
    }
    display(window, animationTime);
    ...
}
```

We added a new variable outside the render loop: `animationTime`. This value is only updated when the `paused` flag is `false`. Go ahead and run the code and verify that you can indeed pause our cube from spinning.

Pretty neat, huh? But, there is a problem with our approach. While `pause` is `true` the clock keeps ticking. This means that when we *unpause* our animations jump to the current time! This can be very jarring, especially with complex scenes. Any guesses how we can "fix" this issue?

**Hide Answer: We only want to count time while *unpaused*. To do this, we need to keep track of how much time we spend paused and then subtract that from the current time.**

In order to do this, we need to create two more globals (back to the top!).

```C++
double pauseStartTime = 0.0f;
double totalPausedTime = 0.0f;
```

We will use these in our `togglePause()` function.

```C++
void togglePause() {
    if (!paused) {
        paused = true;
        std::cout << "PAUSED" << std::endl;
        pauseStartTime = glfwGetTime();
    }
    else {
        paused = false;
        std::cout << "UNPAUSED" << std::endl;
        totalPausedTime += glfwGetTime() - pauseStartTime;
    }
}
```

Let's unpack this. When we enter a *paused* state, we "save" the current time for use later. When we enter an *unpaused* state, we use that value to calculate how long the pause lasted. We use this difference when calculating `animationTime` in `display()`.

```C++
if (!paused) {
    animationTime = glfwGetTime() - totalPausedTime;
}
```

Now, `animationTime` will only be updated when `unpaused`, and that time will reflect only the passage of time while `unpaused`. Go ahead and run the program again and witness the *awesome*. No matter how long you wait between pressing "P", the animation will always pick up where it left off.

# Other Means of Control

Above, we covered how to handle key presses as toggle events. This worked great for pausing our animation, but what about movement of the camera? While it is possible to move the camera's position one key press at a time, it isn't how we generally expect to interact with a 3D scene.

We will cover the specifics in our next lesson. For now, feel free to experiment with other things you can do with key presses.

* Changing the direction of rotation
* Scaling the size of the cube
* Moving the cube around the scene
* Close the window when ESC is pressed

[^1]: The code that makes this possible is `glfwPollEvents()`, which is what calls our callback when a key is pressed.
[^2]: [Full list of GLFW Key Tokens](https://www.glfw.org/docs/3.3/group__keys.html)