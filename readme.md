# About

Fourth Dimension is a 4 dimensional playground that I wrote to try and gain some understanding of what a four dimensional physical space would look like and operate.

## Physics Engine

FourthDim includes a 1 to 4 dimensional physics engine that operates only on planes and spheres and does not respect rotation.

## Rotating Hyperspheres

Each spawned hypersphere undergoes rotation at a fixed rate upon a randomly choosen plane within hyperspace. This is a purely visual effect that helps to differentiate different hyperspheres.

# 4d Camera

The abreviated version of how the 4d rendering pipeline works is thus:

```
for each model_vertex in hypershape:
    // Calculate the position in 3d space
    vec4 3d_position = (4x4_4d_camera_rotation_matrix * model_vertex + object_hyperposition)
    // Clear the strange value from the xyz position for further matrix operations
    3d_position.w = 1;
    // Perform the perspetive * view * model transformation of a typical 3d camera
    vertex_screen_space_position =
            4x4_3d_Camera_Perspective_Matrix
            * 4x4_3d_Camera_View_Matrx
            * 4x4_Model_Postion_Matrix
            * 3d_position;
```

# Controls

```
<LeftCtrl> - release mouse pointer

Mouse + WASD + <LeftShift> + <Space> - Spatial navigation of 3d camera.

1 - reverse gravity
2 - spawn ball
3 - toggle "slice" mode
4 - Display 4th dimension as shading
5 - Project 4th dimension spatially
6 - Toggle bounding hypercube vs "top" and "bottom" bounding cubes
7 - Change hypersphere render quality
8 - Reduce physics dimensions
9 - Increase physics dimensions
0 - Change spawning size mode (modes are "big", "small", and "random")

X - Clear all balls
Z - Toggle "gravity follows visual down"
C - Reset gravity
P - pause physics simulation
'/' - Reset 4d camera rotation

The remaining controls all adjust a slider value. The first listed key is "increase", the second "decrease", and the third "reset".

R, F, V - adjust slice offset (slice mode only)
T, G, B - Adjust 4d camera rotation 1
Y, H, N - Adjust 4d camera rotation 2
U, J, M - Adjust 4d camera rotation 3
I, K, ',' - Adjust 4d camera rotation 4
O, L, '.' - Adjust 4d camera rotation 5
```

# Prebuilt Windows Binary

A prebuilt windows binary is available in the zip file "windows_prebuilt". It should contain all necessary files and libraries to run fourth dimension

# Libraries needed to build from scratch

### To Download
glew
glfw
glm
freeglut

### System Libraries
opengl
glu
gdi
