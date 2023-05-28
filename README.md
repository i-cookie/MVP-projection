## How to run this program?

#### Basic execution

There is an executable file **compile_run.sh**, which can be run directly to build and run the program.

```bash
bash ./compile_run.sh
```

The program will ask the user whether he/she wants to input the vertices and triangles manually. Enter **y** for yes or **n** for no.

Without any parameter, the program will run with all settings of **default** values. i.e.,

- The program will display via GUI
- The position of camera is (0,0,10)

- The vertices are (-1,0,-5), (1,0,-5) and (0,2,-5);
- The axis is (0,0,-1), which is the -z direction
- The rotation angle is 0
- The translation is (0,0,0), and scaling is (1,1,1)
- The eye_fov is $\dfrac{\pi}{2}$, and aspect_ratio is 1
- zNear = -1, zFar = -7

The default settings can be found in the **report**.

In **GUI mode**, you can press **a** and **d** on keyboard to increase or decrease the rotation angle.

#### Parameters

The user can input parameters when executing the bash file. The table below shows whether you muse input a parameter.

| Parameter       | Explanation                                                  | Necessary                                                    | Default      |
| --------------- | ------------------------------------------------------------ | ------------------------------------------------------------ | ------------ |
| -G / -I         | run with a GUI / just output an image                        | **Yes** if the parameters below will be input, otherwise **No** | -G           |
| \<filename\>    | (only used after **-I**) the file name of the result image   | **Yes** (only in **-I **mode)                                | /            |
| \<angle\>       | the rotation angle                                           | **Yes** if the parameters below will be input, otherwise **No** | 0            |
| \<coordinates\> | **a group of 6 parameters**, which stands for the xyz coordinates of P0 and P1 | **No**                                                       | 0 0 0 0 0 -1 |

For example:

```bash
bash ./compile_run.sh -I mypic.png 60 0 0 -1 0 1 -1
```

means running the program **without GUI**, rotate the triangles for **60 degree** around the axis determined by **(0,0,-1)** and **(0,1,-1)**, then output the result image as **mypic.png**.

This means you can rotate the triangles around **arbitrary** single axis (not necessarily pass the origin).

If you want to adjust **T, S, eye_pos, eye_fov, aspect_ratio, zNear and zFar**, you have to modify the code. The parameters are in main() funtcion.

#### Manual input vertices and triangles

As mentioned above, the program will ask the user whether he/she wants to input the vertices and triangles manually. If **y** is chosen, the program will show a series of prompt for vertices and triangles.

For example:

```bash
bash ./compile_run.sh -I mypic.png

...(building process omitted)

Would you like to manually set vertices and triangles? (y/n): y
You can now set a vertex in the 3D right-hand coordinate system!
set the x coordinate: -1
set the y coordinate: 0
set the z coordinate: -5
your vertex of index 0 is (-1 0 -5).
is this your last vertex? (y/n): n
You can now set a vertex in the 3D right-hand coordinate system!
set the x coordinate: 1
set the y coordinate: 0
set the z coordinate: -5
your vertex of index 1 is (1 0 -5).
is this your last vertex? (y/n): n
You can now set a vertex in the 3D right-hand coordinate system!
set the x coordinate: 0
set the y coordinate: 2
set the z coordinate: -4
your vertex of index 2 is (0 2 -4).
is this your last vertex? (y/n): y
You can now choose 3 vertices to draw a triangle! (you have indices 0 - 2)
Enter the FIRST index of vertex you choose: 0
Enter the SECOND index of vertex you choose: 1
Enter the THIRD index of vertex you choose: 2
the vertices of the triangle are: 0 - 1 - 2
is this your last triangle? (y/n): y
Photo generationg complete!
```

