# Penalty Shootout

---

## Description
Penalty shootout game as the project for CS488 ( Computer Graphics ). All the libraries and the framework are provided by the course staff. I implemented everything in `project/` except `SceneNode`, `JointNode`, `GeometryNode` and `scene_lua`, however, I adjusted the classes to achieve new features. 

---

## Dependencies
* OpenGL 3.2+
* GLFW
    * http://www.glfw.org/
* Lua
    * http://www.lua.org/
* Premake4
    * https://github.com/premake/premake-4.x/wiki
    * http://premake.github.io/download.html
* GLM
    * http://glm.g-truc.net/0.9.7/index.html
* ImGui
    * https://github.com/ocornut/imgui


---

## Building Projects

    $ premake4 gmake
    $ make

This will build the following static libraries, and place them in the top level **lib**
folder of your cs488 project directory.
* libcs488-framework.a
* libglfw3.a
* libimgui.a

Next we can build a specific project. Navigate to **project** and run the following commands

    $ cd project/
    $ premake4 gmake
    $ make
    
## Run project

    $ ./project
    
## User interface
* Use the arrow keys to set the direction of the ball
* Use the slider to control the initial speed of the ball
* Press `space` to "kick" the ball
* Press `s` after taking a shot to switch players
* Press `g` for `GodMode`
   * Use arrow keys, `+` and `-` to translate view
   * Use `w`, `s`, `a`, and `d` to rotate the view
