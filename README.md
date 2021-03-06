# spicy-carnival

Goals for this project:
- :white_check_mark: implement skybox
- :white_check_mark: refactor Vulkan code a bit
- :white_check_mark: manage multiple effects in a non-OOP way
- :white_check_mark: find a good way to load .obj files
- :white_check_mark: implement text rendering
- :white_check_mark: implement good 6 degrees of freedom camera controls with momentum
- :white_check_mark: implement `#include` for shaders
- :black_square_button: automate link between uniform block in `Camera.h` and shaders

You'll need a cube map to put under `textures/` in the form of 6 `.png` images:
* `nx.png`: negative x
* `px.png`: positive x --- to the right
* `ny.png`: negative y
* `py.png`: positive y --- down the screen because of Vulkan
* `nz.png`: negative z
* `pz.png`: positive z --- into the screen
