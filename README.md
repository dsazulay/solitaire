# Solitaire

## TODO

- Support window resizing
- Use indices instead of pointers to cards
- Adjust deck and card to be more cache friendly
- Add highlight/outline for cards on hover
- Add background card for empty stacks
- Create a begin game animation
- Use string view instead of char* strings
- Save player data file before extiting the app
- Update text font
- Support seeding the RNG
- Add random class

## Vulkan

- [x] Render background quad
- [x] Rework model load functions (remove all opengl related stuff)
- [x] Rework shader load functions (remove all opengl related suff)
- [x] Render cards (add new pipeline)
- [x] Convert and load card texture
- [x] Rework texture load functions (remove all opengl related suff)
- [ ] Isolate swapchain creation into its onw function
- [ ] Fix rendering order and handness of vulkan and glfw
- [ ] Fix ImGui
