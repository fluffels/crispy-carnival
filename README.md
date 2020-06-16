# spicy-carnival

Goals for this project:
* implement skybox
* manage multiple shaders in a good way
* refactor Vulkan code a bit

You'll need a cube map to put under `textures/` in the form of 6 `.png` images:
* `nx.png`: negative x
* `px.png`: positive x --- to the right
* `ny.png`: negative y
* `py.png`: positive y --- down the screen because of Vulkan
* `nz.png`: negative z
* `pz.png`: positive z --- into the screen
