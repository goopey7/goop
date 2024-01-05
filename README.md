# goop - Gaming On Open-source Playground
## Key Features
1. Editor GUI
2. C++ Scripting Through Custom Components
3. Vulkan Renderer
4. Physics
5. Scene serialization
6. Scene State - Hit play in the editor and then hit stop to go back to editing
7. SFX
8. Efficient Resource Management (instanced rendering)
## Getting Started
### Build
#### Linux - First Class Experience
You need to install `cmake, shaderc, glfw, glm, stb, sdl2`, and `assimp` through your distribution's package manager. 

For example on Arch: 
`pacman -S cmake shaderc glfw-x11 glm stb sdl2 assimp`

You'll also obviously need a C++ compiler like g++ or clang++

The rest is simple:
`mkdir build && cd build`
`cmake ..`
`make`

#### Windows - It works
First step is to get [git for windows](https://git-scm.com/download/win). If you get the standalone installer and run it all the way through that will be sufficient. The reason we need this is actually to run the bash scripts which handle custom components. We don't need git, but if you're reading this you probably should have it.

You'll also need [CMake](https://cmake.org/download/). I recommend the installer, and allow it to add the binaries to your PATH. 

Oh and you also better have [Visual Studio](https://visualstudio.microsoft.com/vs/community/). Make sure to install Desktop Development with C++ and Game Development with C++ in the Installer.

Open up the repo in file explorer and navigate to the `windows` folder. Yeah in typical windows fashion there's a bit of bs. Execute the script named `GET_LIBS.bat` which will download the required libraries.

You can then choose to either execute `BUILD_MSVC.bat`, which will make the Visual Studio Solution and build a debug executable, or `GEN_VS_PROJ.bat`, which will make the Solution and open up Visual Studio.

Set the project named "goop" to run as the startup project.

### Running
goop uses Vulkan for 3D rendering. So you'll need GPU/iGPU drivers that support Vulkan. Most modern hardware should be sufficient.

If you want to run in Debug mode you must install the [Vulkan SDK](https://vulkan.lunarg.com/sdk/home). The program will otherwise crash looking for validation layers which don't exist on your machine.

### Editing Scenes
Run the executable and you'll be greeted with the editor:
* On the left-hand-side is the entity view. This is where you select and create entities.
* In the middle is the viewport of the scene. Hold Left or Right Mouse Button to navigate around the environment.
* On the right is the inspector. All of the components for the selected entity are exposed. And you can add components to the selected entity.
* On the bottom is the scene browser. You can load a scene there, or create a new one.![Editor](https://lh3.googleusercontent.com/pw/ABLVV87LDiXllQYY0zBZdMfwMJ_I8MRpq5EqdY5jGk07cuIye-bnjn6fkvjX7-S-mYm3k0sR0FmQYGNVhoAoQ_N-dow4kTm7BLhweUccr95eya-AaJHMlWBxvisoUnbwqL0ka7tHKlDlXWkP0pey0dLXZwy--Q=w2025-h1350-s-no-gm?authuser=0)

### Editor Controls
#### While Holding LMB/RMB
WASD - strafe 
E/Q - Up/Down
Mouse X/Y - Pitch/Yaw

### Your First Component
Ideally all of your gamedev coding activity will be spent within the `components/` directory. To create a component, go to the inspector view in the editor and click Create Custom Component. Give it a name such as `HelloWorld` and click Create.

Following this example, a `HelloWorld.h` and `HelloWorld.cpp` file will appear in the `components/` directory.

Open up `HelloWorld.cpp` in your favorite editor. The template given should be self-explanatory. 

You will need to close the editor and run CMake again to pickup the new files.

`init()` occurs when the game starts
`update(float dt)` occurs every frame
`onCollisionEnter(goop::Entity other)` happens the moment a collision happens
`onCollisionExit(goop::Entity other)` happens the moment a collision ends
`gui()` allows you to add your own `ImGui` calls to the editor and it'll show up in the inspector view

### Built-in Components
* `TransformComponent` - the position of the entity in the world
* `MeshComponent` - can either be from a 3d file or a generated primitive (cube). Must have a texture loaded from a file. Without this component you will not see anything.
* `RigidBodyComponent` - Allows your entity to react to physics. If mass is set to zero it will act as a static object.
* `CameraComponent` - When active is set to true you will see through this camera's position and rotation.
* `TagComponent` - contains a string to identify the entity. Used for serialization and editor gui. But could be handy for the gamedev.

### Build the Game (no editor) - Way smoother frames
Go to line 67 of CMakeLists.txt and comment out this line:
`target_compile_definitions(${PROJECT_NAME} PRIVATE GOOP_APPTYPE_EDITOR)`
so that it looks like this:
`#target_compile_definitions(${PROJECT_NAME} PRIVATE GOOP_APPTYPE_EDITOR)`

Just rerun CMake, build, and you're good to go!

To build the editor again just go back and uncomment that line and rerun CMake.
## Report and Critical Reflection
This is the most complicated software project I've ever worked on. To get all of the current features working before the deadline has required an unfathomable amount of time and sanity. 

So on that note, I'll start with the successes. 

The first component of the engine I worked on was the Vulkan renderer. I followed [vulkan-tutorial.com](https://vulkan-tutorial.com) up to the Model Loading chapter. Since the API itself is C, and the tutorial was purely a rendering tutorial not giving tips on software design, it was up to me to organize the code into classes. Once I had a seemingly well behaved Vulkan app (a triangle which doesn't crash when the window resizes), I integrated it with the Grengine architecture we were learning in week 2. And that's how the engine codebase started.

I already had experience with CMake since I use it for all my C++ projects on Linux. But it was only to glob source files and maybe link the odd library. This time around shaders get compiled as well as a meta-meta-programming script. And many libraries get compiled.

I guess I can't just say meta-meta-programming without explaining myself. So let's talk about custom components! I didn't have time to look into scripting with a different language but the engine definitely could go in that direction. So to compromise we have C++ scripting. When building the program with CMake, a bash script will run to check .cpp files in the `components/` directory. If there are any, it will then also update a header file which contains an `std::variant` type. So it'll add a variant and make a factory function. All based off of the file name in the directory! So yeah I think we can call that meta-meta-programming. That was a totally original and probably naive idea.

Loading and unloading meshes and textures is also pretty efficient and robust. If you try and load a mesh more than once, the engine will actually just increment the amount of instances instead of duplicating the vertices and indices. This means we can take advantage of Vulkan's instanced rendering feature.

What plagued this submission the most was that despite being told to make a game alongside the engine, I was lazer focused on the technology. And you can tell. The inline helper functions in places like `<goop/Input.h>` get the job done, but were added last minute in a scramble to get something that could maybe turn into a game by the submission. And so it's definitely missing some functions and lacking in thought for the application programmer. If I wanted to make an FPS, I could get the camera working fine. But how on earth would I ray-cast or shoot something that triggers but doesn't do collision physics? Having just those two basic additional features would go a really long way in the potential for games.

I'm also very sad to say that this submission does not make use of DLLs. One of my original visions was being able to just slot an editor DLL in and then boom you're in the editor. Due to time constraints I did not have much time to investigate.

Also the editor doesn't have any system for multiple games/projects. Which to me feels like an essential game engine feature. But maybe for in-house engines not so much? It's definitely something I'd want for my engine.

Another unfortunate failing at submission time is that all of the math is glm. There's no wrapper or subsystem to allow for any other math libraries. Although the solution isn't technically challenging, it's laborious. So from an educational standpoint it's not a priority until I need to implement a Direct3D renderer. 

# References
assimp team (2006) assimp. Available at: [https://github.com/assimp/assimp](https://github.com/assimp/assimp) 

(Accessed: 5 January 2024).
Barrett, S. (2017) stb image. Available at: [https://github.com/nothings/stb](https://github.com/nothings/stb) (Accessed: 5 January 2024).

Caini, M. (2017) EnTT. Available at: [https://github.com/skypjack/entt](https://github.com/skypjack/entt) (Accessed: 5 January 2024).

Coumans, E. (2022) bullet3. Available at: [https://github.com/bulletphysics/bullet3](https://github.com/bulletphysics/bullet3) (Accessed: 5 January 2024).

Geelnard, M. and Löwy, C. (2002) GLFW. Available at: [https://github.com/glfw/glfw](https://github.com/glfw/glfw) (Accessed: 5 January 2024).

G-Truc Creation (2005) glm. Available at: [https://github.com/g-truc/glm](https://github.com/g-truc/glm) (Accessed: 5 January 2024).

Kapoulkine, A. (2018) volk. Available at: [https://github.com/zeux/volk](https://github.com/zeux/volk) (Accessed: 5 January 2024).

Khronos Group (2016) Vulkan 1.0 specification. Available at: [https://www.khronos.org/registry/vulkan/specs/1.0/pdf/vkspec.pdf](https://www.khronos.org/registry/vulkan/specs/1.0/pdf/vkspec.pdf) (Accessed: 5 January 2024).

Komppa, J. (2013) SoLoud audio engine. Available at: [https://github.com/jarikomppa/soloud](https://github.com/jarikomppa/soloud) (Accessed: 5 January 2024).

Lantinga, S. (1997) SDL2. Available at: [https://www.libsdl.org/](https://www.libsdl.org/) (Accessed: 5 January 2024).

Niels, L. (2013) JSON for Modern C++. Available at: [https://github.com/nlohmann/json](https://github.com/nlohmann/json) (Accessed: 5 January 2024).

Ocornut, M. (2014) Dear ImGui. Available at: [https://github.com/ocornut/imgui](https://github.com/ocornut/imgui) (Accessed: 5 January 2024).

Okada, S. (2017) PicoSHA2 - a C++ SHA256 hash generator. Available at: [https://github.com/okdshin/PicoSHA2](https://github.com/okdshin/PicoSHA2) (Accessed: 5 January 2024).

Overvoorde, A. (2021) Vulkan tutorial. Available at: [https://vulkan-tutorial.com/](https://vulkan-tutorial.com/) (Accessed: 5 January 2024).
