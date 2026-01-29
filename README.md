# 3D Rendering in Software

This is my work to build a software 3D renderer as guided by the
["Learn Computer Graphics Programming" course by Gustavo Pezzi](https://pikuma.com/courses/learn-3d-computer-graphics-programming).

Here is what final project looks like

https://github.com/user-attachments/assets/b7e88592-1ed5-4023-8a99-a789fc463fa1

# Features

- Perspective Projection
- DDA Line Drawing Algorithm
- Loading OBJ File Content
- Back-face Culling
- Barycentric Weights (Texture Mapping)
- Painter's Algorithm
- Gouraud Shading & Phong Shading
- LookAt Camera Model
- Simple FPS Camera Model
- 3D Clipping
- Rendering Multiple Meshes

# Topics To Review

Concepts that I still lack some total understanding of:

 - Coordinate system handedness and how it affects operations such as cross
   product
 - Perspective correct interpolation

# Additional Learning Resources

 - [Subpixel Rendering](https://marioslab.io/posts/jitterbugs/)

# Notes

## Perspective Projection

The two triangles that make up the viewer's angle to the screen-space projected
point and the 3D point are similar triangles, which share a constant ratio
$\frac{BC}{DE} = \frac{AB}{AD}$ as illustrated in the top-down view of the point's X axis
below.

<img width="1102" height="1038" alt="perspective-projection-triangle" src="https://github.com/user-attachments/assets/d66de9ff-ac53-47b3-9021-a92b61c01991" />

This can be simplified:

$$
\frac{P'_x}{P_x}=\frac{1}{P_z}
$$

$$
P'_x=\frac{P_x}{P_z}
$$

Note $P_z$ in the denominator, indicating that the projected X is scaled by the
Z distance to the point. This is called the **perspective divide**.

Similarly, the Y perspective projection:

$$
P'_y=\frac{P_y}{P_z}
$$

## Left vs Right-handed Coordinate Systems

The "handedness" of the coordinate system defines how different dimensional axis
are interpreted.

 - "Left-handed" coordinate systems define Z values as growing "into" the
   screen, away from the viewer.
 - "Right-handed" coordinate systems define Z values as growing "out of" the
   screen, toward the viewer.

DirectX uses a left-handed coordinate system, while OpenGL uses a right-handed
coordinate system.

<img width="469" height="354" alt="coordinate-system-hands" src="https://github.com/user-attachments/assets/b52b7b83-8bed-417e-915a-34e93e3d3087" />


_Illustration from https://www.oreilly.com/library/view/learn-arcore/9781788830409_

A separate but related convention is triangle "winding order," or the order in
which the vertices around the edge of a triangle are traversed. The winding
order can be clockwise, or counter-clockwise.

The coordinate system and winding order convention can determine how normal
values should be calculated.

There's additional context in
[this lesson video](https://courses.pikuma.com/courses/take/learn-computer-graphics-programming/lessons/12297168-handedness-orientation).

### Left Hand Rule

Given a triangle with points $`A`$, $`B`$, $`C`$, point your index finger
in the direction of the first vector $`(B - A)`$ and your middle finger in the
direction of the second vector $`(C - A)`$. Your thumb will be pointing in the
direction of the normal vector.

### Right Hand Rule

Use the same method as above, but with the right hand. Notice that the normal
direction is inverted given the same triangle.

Physicists normally use the right hand rule.

## Transformation

### Rotation

#### Applying a rotation to a 2D vector using trigonometry functions

Goal: Rotate a 2D vector $(x, y)$ around the origin by angle $\beta$ to get
$(x', y')$.

Let $\alpha$ represent the angle from the origin to $(x, y)$.

Let $r$ represent the hypotenuse of the triangle, or radius of the rotation.

<img width="1231" height="473" alt="2d-vector-angle" src="https://github.com/user-attachments/assets/6ca791ef-8321-44c6-b364-7a4c615be460" />


Since $\cos(\alpha) = \frac{x}{r}$, $x = r \times \cos(\alpha)$

Since $\sin(\alpha) = \frac{y}{r}$, $y = r \times \sin(\alpha)$

After applying the angle $\beta$, the new coordinates can be calculated:

<img width="1347" height="570" alt="2d-vector-rotated" src="https://github.com/user-attachments/assets/a9590de1-2687-4620-9354-fe75b419202b" />


$\cos(\alpha + \beta) = \frac{x'}{r}$, so $x' = r \times \cos(\alpha + \beta)$

$\sin(\alpha + \beta) = \frac{y'}{r}$, so $y' = r \times \sin(\alpha + \beta)$

Trig functions that add two values can be expanded using the
_angle addition formula_:

$$
x' = r\cos(\alpha + \beta)
$$

$$
x' = r(\cos{\alpha} \cos{\beta} - \sin{\alpha} \sin{\beta})
$$

$$
x' = r \cos{\alpha} \cos{\beta} - r \sin{\alpha} \sin{\beta}
$$

You can substitute $r \cos{\alpha}$ with $x$, and $r \sin{\alpha}$ with $y$:

$$
x' = x \cos{\beta} - y \sin{\beta}
$$

Similarly,

$$
y' = r \sin(\alpha + \beta)
$$

$$
y' = r(\sin{\alpha} \cos{\beta} + \cos{\alpha} \sin{\beta})
$$

$$
y' = r \sin{\alpha} \cos{\beta} + r \cos{\alpha} \sin{\beta}
$$

$$
y' = y \cos{\beta} + x \sin{\beta}
$$

These are the formulas that are used by a rotation transformation matrix.

The same principle applies to 3 dimensions, but with one dimension at a time:

```c
vec3_t Vec3RotateX(vec3_t v, float angle)
{
    vec3_t rotated_vector = {
        .x = v.x,
        .y = v.y * cosf(angle) - v.z * sinf(angle),
        .z = v.y * sinf(angle) + v.z * cosf(angle),
    };
    return rotated_vector;
}

vec3_t Vec3RotateY(vec3_t v, float angle)
{
    vec3_t rotated_vector = {
        .x = v.x * cosf(angle) - v.z * sinf(angle),
        .y = v.y,
        .z = v.x * sinf(angle) + v.z * cosf(angle),
    };
    return rotated_vector;
}

vec3_t Vec3RotateZ(vec3_t v, float angle)
{
    vec3_t rotated_vector = {
        .x = v.x * cosf(angle) - v.y * sinf(angle),
        .y = v.x * sinf(angle) + v.y * cosf(angle),
        .z = v.z,
    };
    return rotated_vector;
}
```

## Vector Math Review

### Vector Magnitude

**Magnitude** refers to the length of the vector:

$$
|\vec{v}|
$$

$$
|\vec{v}| = \sqrt{{v_x}^2 + {v_y}^2}
$$

### Vector Addition

Adding vectors is basically starting one vector from the end of the other:

$$
\vec{a} + \vec{b} = (a_x + b_x, a_y + b_y)
$$

<img width="699" height="510" alt="vector-adding" src="https://github.com/user-attachments/assets/5e170344-2a21-4a04-97ce-8165b7ccffd1" />

### Vector Subtraction

Subtraction is the same as addition, but invert/negate the second vector:

$$
\vec{a} - \vec{b} = (a_x - b_x, a_y - b_y)
$$

<img width="718" height="524" alt="vector-subtraction-01" src="https://github.com/user-attachments/assets/18e59549-f3cd-45d5-8b38-d4c883bd32de" />
<img width="751" height="545" alt="vector-subtraction-02" src="https://github.com/user-attachments/assets/be4a0545-588d-495d-9deb-46a1dd1bf92c" />
<img width="720" height="520" alt="vector-subtraction-03" src="https://github.com/user-attachments/assets/c0c6b663-5b4c-420e-aab2-34f709cf2602" />

### Cross Product

The cross product helps to calculate the normal vector of a plane.

The cross product of two vectors $\vec{x} \times \vec{y}$ yields a vector that is
perpendicular to both of those vectors.

<img width="564" height="466" alt="vector-cross-product" src="https://github.com/user-attachments/assets/c9356618-2f8e-47cf-9dff-ef4a556c1e23" />

To calculate the cross product:

$$
\vec{N} = \vec{a} \times \vec{b}
$$

$$
N_x = a_y b_z - a_z b_y
$$

$$
N_y = a_z b_x - a_x b_z
$$

$$
N_z = a_x b_y - a_y b_x
$$

There are two possible perpendicular vectors for any given pair of vectors.
The order of operands will determine which direction is calculated.

<img width="522" height="705" alt="vector-cross-product-order" src="https://github.com/user-attachments/assets/3d3805fa-e9f9-4877-84d9-7bfcf99851e9" />

The **magnitude** of the cross product is related to the angle between the two
input vectors:

$$
|\vec{a} \times \vec{b}| = |a| |b| \sin{\theta}
$$

[Resource for additional information on how to derive the cross product](https://youtu.be/-1nle1mGZSQ)

### Dot Product

The dot product of two vectors produces a scalar value of the sum of the
components of each given vector multiplied together.

$$
\vec{a} \cdot \vec{b} = {a_x}{b_x} + {a_y}{b_y}
$$

When used with unit vectors, the dot product can be used to produce a
"projection" of one vector onto the other.

<img width="543" height="363" alt="vector-dot-product-projection" src="https://github.com/user-attachments/assets/8185cc64-23d5-4968-8073-1605897ea2ce" />

The more "aligned" the vectors are, the larger the dot product is. If they are
exactly the same, the dot product is $1$.

At a 90 degree offset, the dot product is $0$.

If the two vectors are complete opposites, the dot product is $-1$.

### Normalizing Vectors

A normalized vector is a vector with a magnitude of 1.

$$
\hat{a} = \frac{\vec{a}}{|\vec{a}|}
$$

If you don't care about the length of a vector, it's often better to express it
as a normalized vector.

## Back Face Culling

If we'd like to avoid rendering faces that are facing away from the camera, we
can simply compare their normal vector to the vector of the camera.

<img width="806" height="578" alt="back-face-culling-diagram" src="https://github.com/user-attachments/assets/a08af21d-47c8-4645-94c6-4ca9d61f5fb6" />


Here's how we can get the normal vector of a triangle face:

<img width="1501" height="908" alt="getting-normal-of-triangle-face" src="https://github.com/user-attachments/assets/ccfacbcb-bdaf-49da-9ea8-f68236360178" />


Note that we take our vertices in clockwise order, consistent with our chosen
coordinate system.

Once we have the normal vector, we can compare it to the camera ray vector using
the dot product to determine if the face is facing toward the camera or away
from it.

To find the camera ray vector, we simply subtract the camera position from the
point we are observing.

## Matrices

Just a way of expressing and manipulating a set of values in rows and columns.

$$
M = 
\begin{bmatrix}
-2 & 5 & 6 \\
 5 & 2 & 7
\end{bmatrix}
$$

Matrix $M$ has 2 rows and 3 columns; the *dimensions* are $2 \times 3$.

A matrix has a set of elements that can be referenced as followed:

$$
M_{3 \times 2} =
\begin{bmatrix}
m_{11} & m_{12} \\
m_{21} & m_{22} \\
m_{31} & m_{32}
\end{bmatrix}
$$

Matrices are useful for solving systems of equations:

$$
\left \lbrace
\begin{alignedat}{3}
    x + 2y -4z = 5 \\
    2x + y - 6z = 8 \\
    4x - y - 12z = 13
\end{alignedat}
\right.
$$

$$
\begin{bmatrix}
1 &  2 &  -4 & 5 \\
2 &  1 &  -6 & 8 \\
4 & -1 & -12 & 13
\end{bmatrix}
$$

In computer graphics, matrices are useful in converting sets of geometric data
into different coordinate systems. They can be used to apply translation,
rotation, projection, and many other transformations.

### Matrix Operations

#### Matrix Addition

Simply add each element together.

$$
\begin{bmatrix}
2 &  3 \\
1 & -5
\end{bmatrix} +
\begin{bmatrix}
3 & 1 \\
1 & 2
\end{bmatrix} =
\begin{bmatrix}
5 &  4 \\
2 & -3
\end{bmatrix}
$$

#### Matrix Subtraction

Simply subtract each element from each other.

$$
\begin{bmatrix}
2 &  3 \\
1 & -5
\end{bmatrix} -
\begin{bmatrix}
3 & 1 \\
1 & 2
\end{bmatrix} =
\begin{bmatrix}
-1 &  2 \\
 0 & -7
\end{bmatrix}
$$

#### Matrix Multiplication

Matrix multiplication is more complex. For each combination of row and column
you must multiply the row elements with the column elements and sum the results:

$$
\begin{bmatrix}
1 & 2 \\
3 & 4
\end{bmatrix} *
\begin{bmatrix}
5 & 6 \\
7 & 8
\end{bmatrix} =
\begin{bmatrix}
(1 \ast 5) + (2 \ast 7) & (1 \ast 6) + (2 \ast 8) \\
(3 \ast 5) + (4 \ast 7) & (3 \ast 6) + (4 \ast 8)
\end{bmatrix} =
\begin{bmatrix}
19 & 22 \\
43 & 50
\end{bmatrix}
$$

Multiplication is only possible when the number of columns on the left matrix
is equal to the number of rows on the right matrix.

The dimension of the resulting matrix will have the number of rows of the left
matrix and the number of columns of the right matrix.

$$
M_{N \times M} * M_{M \times P} = M_{N \times P}
$$

Matrix multiplication is not commutative:

$$
A * B \neq B * A
$$

### Identity Matrix

A square matrix with 1's in the diagonal and 0's everywhere else.

Any matrix multiplied by the identity matrix will return an unchanged result.

$$
\begin{bmatrix}
1 & 0 & 0 & 0 \\
0 & 1 & 0 & 0 \\
0 & 0 & 1 & 0 \\
0 & 0 & 0 & 1
\end{bmatrix}
$$

### Simple Rotation Matrix

Earlier, we determined that you can calculate the new $x$ and $y$ positions for
a given rotation $\alpha$ using simple trigonometry functions:

$$
x' = x \cos{\alpha} - y \sin{\alpha}
$$

$$
y' = y \cos{\alpha} + x \sin{\alpha}
$$

This can be represented in matrix form:

$$
\begin{bmatrix}
x' \\
y'
\end{bmatrix} =
\begin{bmatrix}
\cos{\alpha} & -\sin{\alpha} \\
\sin{\alpha} & \cos{\alpha}
\end{bmatrix} *
\begin{bmatrix}
x \\
y
\end{bmatrix} =
\begin{bmatrix}
x\cos{\alpha} - y\sin{\alpha} \\
x\sin{\alpha} + y\cos{\alpha}
\end{bmatrix}
$$

This matrix is called a 2D
[rotation matrix](https://en.wikipedia.org/wiki/Rotation_matrix):

$$
R =
\begin{bmatrix}
\cos{\theta} & -\sin{\theta} \\
\sin{\theta} & \cos{\theta}
\end{bmatrix}
$$

When it is multiplied against a set of coordinates, it produces a set of
transformed coordinates rotated by $\theta$.

### Transformation Matrices

In linear algebra, linear transformations can be represented by matrices.

$$
\begin{bmatrix}
m & m & m & m \\
m & m & m & m \\
m & m & m & m \\
m & m & m & m
\end{bmatrix} *
\begin{bmatrix}
x \\
y \\
z \\
w
\end{bmatrix}
$$

4x4 matrices are usually used to represent 3D transformations (scale,
translation, rotation, etc.)

We use 4x4 instead of 3x3 because some transformations (ex. translation) require
an extra row/column.

To enable multiplication, an extra component $w$ is added to our original
vector.

#### Scale Matrix

$$
\begin{bmatrix}
sx & 0  & 0  & 0 \\
0  & sy & 0  & 0 \\
0  & 0  & sz & 0 \\
0  & 0  & 0  & 1 \\
\end{bmatrix} *
\begin{bmatrix}
x \\
y \\
z \\
1
\end{bmatrix}
$$

Performing this multiplication yields the following:

$$
\begin{bmatrix}
(sx * x) + 0        + 0        + 0 \\
0        + (sy * y) + 0        + 0 \\
0        + 0        + (sz * z) + 0 \\
0        + 0        + 0        + (1 * 1) \\
\end{bmatrix} = 
\begin{bmatrix}
sx * x \\
sy * y \\
sz * z \\
1
\end{bmatrix}
$$

#### Translation Matrix

$$
\begin{bmatrix}
1 & 0 & 0 & tx \\
0 & 1 & 0 & ty \\
0 & 0 & 1 & tz \\
0 & 0 & 0 & 1
\end{bmatrix} *
\begin{bmatrix}
x \\
y \\
z \\
1
\end{bmatrix}
$$

Performing the multiplication yields the following:

$$
\begin{bmatrix}
x + 0 + 0 + tx \\
0 + y + 0 + ty \\
0 + 0 + z + tz \\
0 + 0 + 0 + 1
\end{bmatrix} =
\begin{bmatrix}
x + tx \\
y + ty \\
z + tz \\
1
\end{bmatrix}
$$

#### Rotation Matrix

*These are defined in a left-handed coordinate system, such that each axis is
rotated counter-clockwise around its axis. See
[direction](https://en.wikipedia.org/wiki/Rotation_matrix#Direction).*

The rotation matrix for the **X axis** looks like this:

$$
\begin{bmatrix}
1 & 0 & 0 & 0 \\
0 &  \cos{\alpha} & \sin{\alpha} & 0 \\
0 & -\sin{\alpha} & \cos{\alpha} & 0 \\
0 & 0 & 0 & 1
\end{bmatrix} *
\begin{bmatrix}
x \\
y \\
z \\
1
\end{bmatrix}
$$

The rotation matrix for the **Y axis** looks like this:

$$
\begin{bmatrix}
\cos{\alpha} & 0 & -\sin{\alpha} & 0 \\
0 & 1 & 0 & 0 \\
\sin{\alpha} & 0 &  \cos{\alpha} & 0 \\
0 & 0 & 0 & 1
\end{bmatrix} *
\begin{bmatrix}
x \\
y \\
z \\
1
\end{bmatrix}
$$

The rotation matrix for the **Z axis** looks like this:

$$
\begin{bmatrix}
 \cos{\alpha} & \sin{\alpha} & 0 & 0 \\
-\sin{\alpha} & \cos{\alpha} & 0 & 0 \\
0 & 0 & 1 & 0 \\
0 & 0 & 0 & 1
\end{bmatrix} *
\begin{bmatrix}
x \\
y \\
z \\
1
\end{bmatrix}
$$

### World Matrix

By combining translation, rotation, and scaling matrices via matrix
multiplication, we can express the location of an object in the world with a
single matrix.

$$
M_{world} = M_{translation} * M_{rotation} * M_{scale}
$$

```math
\vec{v}_{vertex} = 
M_{world} *
\begin{bmatrix}
x \\
y \\
z \\
1
\end{bmatrix}
```

#### Order of Transformations

The order of transformations matters. The usual order is:

 1. Scale
 2. Rotate
 3. Translate

If these are performed out-of-order, it may result in unexpected values. For
example, if translation is applied before rotation such that the object has
moved away from the origin (0, 0), then rotation will be still be applied around
the origin of (0, 0), exaggerating the result of the rotation transformation.

### Perspective Projection Matrix

You can also use matrices to achieve projection of points onto a plane.

Projection Matrices handle:

 - **Aspect ratio**: adjust x and y values based on screen width and height
 - **Field of view**: adjust x and y values based on FOV angle
 - **Normalization**: adjust x, y, and z values to sit between -1 and 1

<img width="832" height="1071" alt="projection-matrix-to-image-space-ndc" src="https://github.com/user-attachments/assets/02d48aa2-e968-4a80-a85f-fcafa13233de" />


The aspect ratio of the height vs width of the screen.

$$
a = \frac{h}{w}
$$

The field of view is defined as the 'scale factor' for how points should be
adjusted to fit within the given FOV angle.

<img width="1194" height="942" alt="field-of-view-trig" src="https://github.com/user-attachments/assets/566c33c3-91b7-4238-9049-7c28dbdd19d1" />


$$
f = \frac{1}{\tan(\theta / 2)}
$$

#### Normalizing Z

We must also normalize z to a 'normalized device coordinate' between 0 and 1.

We do this by defining two planes; $zfar$ and $znear$. We can then define a
scaling factor to adjust values with respect to these two planes.

$$
\lambda = \frac{zfar}{zfar - znear} - \frac{zfar * znear}{zfar - znear}
$$

We can use values $a$, $f$, $\lambda$ from above to convert world points
$`\begin{bmatrix}x \\ y \\ z \end{bmatrix}`$ to screen space
$`\begin{bmatrix} a f x \\ f y \\ \lambda z - \lambda znear \end{bmatrix}`$.

#### Deriving the Projection Matrix

We can substitute in the values we defined above:

$$
\begin{bmatrix}
a f x \\
f y \\
\lambda z - \lambda znear
\end{bmatrix} =
\begin{bmatrix}
(\frac{h}{w})(\frac{1}{\tan(\theta/2)}) * x \\
(\frac{1}{\tan(\theta/2)}) * y \\
(\frac{zfar}{zfar - znear}) * z - (\frac{zfar}{zfar - znear}) * znear
\end{bmatrix}
$$

To apply this using matrix multiplication, we can use a matrix like the
following:

$$
\begin{bmatrix}
(\frac{h}{w})(\frac{1}{\tan(\theta/2)}) & 0 & 0 & 0 \\
0 & (\frac{1}{\tan(\theta/2)}) & 0 & 0 \\
0 & 0 & (\frac{zfar}{zfar - znear}) & -(\frac{zfar}{zfar - znear}) * znear \\
0 & 0 & 1 & 0
\end{bmatrix}
$$

Note the $1$ in position $M_{4, 3}$. This is used to maintain the original
un-normalized $z$ value in the 4th dimension $w$ in order to later perform
operations such as perspective divide.

[This note](https://courses.pikuma.com/courses/take/learn-computer-graphics-programming/texts/55621841-really-grokking-the-projection-matrix)
has additional explanation for the values contained within the projection
matrix.

### Column-Major vs Row-Major

In the examples above, vertices are represented in "column-major" order:

$$
\begin{bmatrix}
x \\
y \\
z \\
w
\end{bmatrix}
$$

An alternative representation is "row-major":

$$
\begin{bmatrix}
x & y & z & w
\end{bmatrix}
$$

Different graphics APIs may choose to use different representations for a
variety of reasons.

One implication to row-major vs column-major is the order of operands for matrix
multiplication. Vertices defined in row-major format are "post-multiplied"
against a projection matrix:

```math
\vec{p}_{projected} = \vec{p} * M_{scale} * M_{rotate} * M_{translate}
```

Where as column-major vertices are "pre-multiplied":

```math
\vec{p}_{projected} = M_{translate} * M_{rotate} * M_{scale} * \vec{p}
```

[This note](https://courses.pikuma.com/courses/take/learn-computer-graphics-programming/texts/14521611-order-of-transformations-for-row-major-and-column-major)
has more details.

## Flat Shading with Global Illumination

Simple lighting can be achieved by implementing one global light with a
direction vector. The direction vector can be compared against each face normal
via dot product to determine a lighting intensity.

<img width="676" height="596" alt="simple-lighting-dot-product" src="https://github.com/user-attachments/assets/5970e822-6e61-4b2d-8859-55752e43c706" />


## Texture Mapping

Texture coordinates are represented as $`(u, v)`$ simply to differentiate from
the normal $`x, y`$ values.

UV mapping is the process of mapping the vertices of a face to positions on a
texture.

<img width="925" height="1003" alt="uv-coordinates" src="https://github.com/user-attachments/assets/f33cac4e-70d5-4a98-b227-9680258b2394" />

<img width="517" height="1085" alt="uv-coordinates-mapped" src="https://github.com/user-attachments/assets/48e2d5fc-68fc-4b5b-93e3-6915e70a5ef5" />



### Interpolating with Barycentric Coordinates

Barycentric coordinates are like applying a set of weight values on vertices
to decide where a point is located in the middle of a triangle face.

```math
P = (\alpha, \beta, \gamma)
```

These 'weight values' also represent the areas of the three sub-triangles made
by the point $`P`$

<img width="559" height="470" alt="barycentric-weights-with-area" src="https://github.com/user-attachments/assets/63c8668b-99aa-4207-b35b-1a4b8d0d8d9d" />


The sum of the barycentric weights is always $`1`$.

```math
\alpha + \beta + \gamma = 1
```

The weights 'pull' the vertices to result in coordinate $`P`$.

```math
\vec{P} = \alpha \vec{A} + \beta \vec{B} + \gamma \vec{C}
```

#### Finding Barycentric Coordinates of a Point

Given the triangle and point $`P`$ defined in the illustration above...

```math
\alpha = \frac{area\_triangle(PBC)}{area\_triangle(ABC)}
```

To calculate the area of the triangle $`PBC`$, treat it like a parallelogram.
This allows us to easily calculate the area using the cross product.
Use the same approach to determine the area of $`ABC`$.

<img width="827" height="545" alt="barycentric-parallelograms" src="https://github.com/user-attachments/assets/28b43b4e-8086-44c9-b20a-6fb4a4550455" />


```math
\alpha = \frac{area\_parallelogram(PBEC)}{area\_parallelogram(ABDC)}
```

```math
\alpha = \frac{PC \times PB}{AC \times AB}
```

```math
\beta = \frac{AC \times AP}{AC \times AB}
```

```math
\gamma = 1.0 - \alpha - \beta
```

_Ensure that the order of the cross product matches the coordinate system in
use, left-handed (clockwise) in this case._

### Perspective Correct Texture Mapping

The straight mapping we achieved so far is called "affine mapping." It does not
take into account perspective.

To find depth values, you cannot simply interpolate $`z`$ values using barycentric
weighting because the depth value is not linear across the triangle.

However, the reciprocal of the Z components _is linear_. So we can use
$`\frac{1}{z}`$ to find nthe interpolated $z$ at point $P$.

<img width="781" height="787" alt="reciprocal-triangle-z-values" src="https://github.com/user-attachments/assets/7611c825-18eb-450f-8ff7-eb08c9c74a3b" />


The original $`z`$ value of each vertex is stored in the $`w`$ value of the
vector, so that should be used in place of $`z`$.

To achieve perspective correct mapping, we:

 1. Use the reciprocal of all attributes ($`\frac{1}{w}`$) (now linear in
    screen space)
 2. Inerpolate over the triangle face (using barycentric weights,
    $`\frac{1}{w}`$ factor)
 3. Divide all attributes by $`\frac{1}{w}`$ (undoes the perspective transform).

There is a good academic paper describing the derivation of perspective correct
interpolation [here](https://s3.amazonaws.com/thinkific/file_uploads/167815/attachments/c06/b72/f58/KokLimLow-PerspectiveCorrectInterpolation.pdf).

Additional resources:

 - https://www.scratchapixel.com/lessons/3d-basic-rendering/rasterization-practical-implementation/perspective-correct-interpolation-vertex-attributes.html
 - https://www.youtube.com/watch?v=zPLfyj-Szow&t=2218s

## Z-Buffer

Can also be called the depth buffer.

This stores the depth of each screen pixel in an array, and helps determine
which pixel is "in front."

Alternative to painter's algorithm.

As explained in perspective-correct texture interpolation, the Z depth is not
linear in screen space across the surface of the triangle. Instead, like
texture mapping, the reciprocal $`\frac{1}{w}`$ is used instead.

## Camera

Or a "view matrix" is used to transform the 3D scene into a perspective from
a camera or view.

One approach to doing this is implementing a "look at" function that returns
a matrix that can transform world vertices into camera space from a certain
point looking at another point.

The matrix will:

 1. Translate the whole scene _inversely_ from the camera eye position to the
    origin (matrix $`M_T`$)
 2. Rotating the scene with _reverse_ orientation (matrix $`M_R`$) so the camera
    is positioned at the origin and facing the positive Z axis (since our
    renderer is left-handed).

```math
M_{view} = M_R * M_T =
\begin{bmatrix}
r_{11} & r_{12} & r_{13} & 0 \\
r_{21} & r_{22} & r_{23} & 0 \\
r_{31} & r_{32} & r_{33} & 0 \\
0      & 0      & 0      & 1
\end{bmatrix} *
\begin{bmatrix}
1 & 0 & 0 & t_x \\
0 & 1 & 0 & t_y \\
0 & 0 & 1 & t_z \\
0 & 0 & 0 & 1
\end{bmatrix}
```

```math
= \begin{bmatrix}
r_{11} & r_{12} & r_{13} & (r_{11}t_x + r_{12}t_y + r_{13}t_z) \\
r_{21} & r_{22} & r_{23} & (r_{21}t_x + r_{22}t_y + r_{23}t_z) \\
r_{31} & r_{32} & r_{33} & (r_{31}t_x + r_{32}t_y + r_{33}t_z) \\
0    & 0    & 0    & 1
\end{bmatrix}
```

The translation matrix will simply be the negated coordinates of the eye
position:

```math
M_T = \begin{bmatrix}
1 & 0 & 0 & -eye_x \\
0 & 1 & 0 & -eye_y \\
0 & 0 & 1 & -eye_z \\
0 & 0 & 0 & 1 \\
\end{bmatrix}
```

For the rotation matrix, we need to compute the forward ($`z`$), right ($`x`$),
and up ($`y`$) vectors.

```math
\begin{bmatrix}
x_x & y_x & z_x & 0 \\
x_y & y_y & z_y & 0 \\
x_z & y_z & z_z & 0 \\
0 & 0 & 0 & 1
\end{bmatrix}^{-1}
```

This matrix is used to convert between coordinate systems. Note, it must
be _inverted_ (since the scene must move 'around' the camera). An inverted
matrix can be thought of like an "undo" of the original matrix.

For _orthogonal_ matrices, inversion is a simple matter of transposing (flipping
so that rows become columns and columns become rows).

```math
\begin{bmatrix}
x_x & y_x & z_x & 0 \\
x_y & y_y & z_y & 0 \\
x_z & y_z & z_z & 0 \\
0 & 0 & 0 & 1
\end{bmatrix}^{T} =
\begin{bmatrix}
x_x & x_y & x_z & 0 \\
y_x & y_y & y_z & 0 \\
z_x & z_y & z_z & 0 \\
0 & 0 & 0 & 1
\end{bmatrix}
```

Multiplying the rotation and transformation matrices with the values above:

```math
M_{view} = M_R * M_T =
\begin{bmatrix}
x_x & x_y & x_z & 0 \\
y_x & y_y & y_z & 0 \\
z_x & z_y & z_z & 0 \\
0 & 0 & 0 & 1
\end{bmatrix} *
\begin{bmatrix}
1 & 0 & 0 & -eye_x \\
0 & 1 & 0 & -eye_y \\
0 & 0 & 1 & -eye_z \\
0 & 0 & 0 & 1 \\
\end{bmatrix}
```

```math
= \begin{bmatrix}
x_x & x_y & x_z & (-x_x eye_x - x_y eye_y -x_z eye_z) \\
y_x & y_y & y_z & (-y_x eye_x - y_y eye_y -y_z eye_z) \\
z_x & z_y & z_z & (-z_x eye_x - z_y eye_y -z_z eye_z) \\
0 & 0 & 0 & 1
\end{bmatrix}
```

The last column can be simplified using dot product:

```math
= \begin{bmatrix}
x_x & x_y & x_z & -dot(x, eye) \\
y_x & y_y & y_z & -dot(y, eye) \\
z_x & z_y & z_z & -dot(z, eye) \\
0 & 0 & 0 & 1
\end{bmatrix}
```

## Clipping

Clipping is the process of removing objects or line segments that are outside
the viewing volume.

For frustum clipping, six planes are used:

 - Top
 - Bottom
 - Left
 - Right
 - Near
 - Far

<img width="546" height="448" alt="frustum-clipping" src="https://github.com/user-attachments/assets/f1c9c2ce-9001-4ef8-a8a5-b2b06600c1e0" />


A plane is defined by a point $`P`$ and a normal vector $`\vec{n}`$.

### Calculating the Frustum Planes

<img width="614" height="784" alt="right-frustum-plane" src="https://github.com/user-attachments/assets/04ca6c20-f86e-48a6-976b-d8fb9921c2ea" />


Notably, the camera origin point is present on _every_ frustum plane. So it
makes for a convenient starting point $`P`$.

To calculate the right frustum plane, simply draw the normal vector at 90
degrees from the right camera boundary vector ($`\frac{fov}{2}`$ from origin)
and calculate the X and Y components of the normal vector using trig functions.

```math
P = (0, 0, 0)
```

```math
\vec{n}_x = -\cos{\frac{fov}{2}}
```

```math
\vec{n}_y = 0
```

```math
\vec{n}_z = \sin{\frac{fov}{2}}
```

A similar process can be used for the left, top, and bottom planes.

#### Right Frustum Plane

```math
\vec{n} = ( -\cos{\frac{fov}{2}}, 0, \sin{\frac{fov}{2}} )
```

#### Left Frustum Plane

```math
\vec{n} = ( \cos{\frac{fov}{2}}, 0, \sin{\frac{fov}{2}} )
```

#### Top Frustum Plane

```math
\vec{n} = ( 0, -\cos{\frac{fov}{2}}, \sin{\frac{fov}{2}} )
```

#### Bottom Frustum Plane

```math
\vec{n} = ( 0, \cos{\frac{fov}{2}}, \sin{\frac{fov}{2}} )
```

#### Near Frustum Plane

Unlike the other planes, the point $`P`$ is offset from camera origin. The
normal vector is straight out along the $`Z`$ axis.

```math
P = ( 0, 0, z_{near} )
```

```math
\vec{n} = (0, 0, 1)
```

#### Far Frustum Plane

Similarly, for the far plane:

```math
P = ( 0, 0, z_{far} )
```

```math
\vec{n} = (0, 0, -1)
```

The negative $`\vec{n}_z`$ value is notable because it denotes which direction
is considered "inside" the plane versus outside.

### Determining Whether Points are Inside, Outside, or on a Plane

A point $`Q`$ will be "on" the plane if $`(Q - P) \cdot \vec{n} = 0`$

<img width="946" height="233" alt="point-on-plane" src="https://github.com/user-attachments/assets/6bd226dd-f464-43ae-a3dc-90fa212a7537" />


A point $`Q`$ will be "inside" the plane if $`(Q - P) \cdot \vec{n} > 0`$

A point $`Q`$ will be "outside" the plane if $`(Q - P) \cdot \vec{n} < 0`$

### Determining Intersection of Lines with Planes

The linear interpolation equation allows us to calculate any point along a line:

```math
I = Q_1 + t(Q_2 - Q_1)
```

The interpolation factor $`t`$ describes the amount $`0 - 1`$ along the line
between points $`Q_1`$ and $`Q_2`$.

Given a plane that intersects the line between $`Q_1`$ and $`Q_2`$, we need to
determine the value of $`t`$ that provides us with the intersection point.

<img width="403" height="260" alt="plane-line-intersection" src="https://github.com/user-attachments/assets/d2453281-2b0b-495c-b566-82daba5edb91" />


You can use the dot product to determine each point's relationship with the
plane:

```math
dot_{Q_1} = \vec{n} \cdot (Q_1 - P)
```

```math
dot_{Q_2} = \vec{n} \cdot (Q_2 - P)
```

We can use these along with the linear interpolation equation:

```math
I = Q_1 + t(Q_2 - Q_1)
```

```math
(I - P) = (Q_1 - P) + t((Q_2 - P) - (Q_1 - P))
```

An dot product each component with $`\vec{n}`$...

```math
\vec{n} \cdot (I - P) = \vec{n} \cdot (Q_1 - P) + t(\vec{n} \cdot (Q_2 - P) - \vec{n} \cdot (Q_1 - P))
```

We know the value of $`\vec{n} \cdot (I - P)`$ is $`0`$ because it is on the
plane. Simplifying:

```math
0 = dot_{Q_1} + t(dot_{Q_2} - dot_{Q_1})
```

We need to isolate $`t`$ to determine the intersection point.

```math
t = \frac{-dot{Q_1}}{dot_{Q_2} - dot_{Q_1}}
```

or

```math
t = \frac{dot{Q_1}}{dot_{Q_1} - dot_{Q_2}}
```

### How to Clip a Polygon

First, list each of the vertices along the boundary of the polygon and
determine if each is inside or outside of the plane. The lines with vertices
that straddle the boundary must be clipped, and the intersection point should
be added to both lists.

| Inside  | Outside |
|---------|---------|
| $`I_1`$ | $`Q_1`$ |
| $`Q_2`$ | $`I_1`$ |
| $`Q_3`$ | $`I_2`$ |
| $`Q_4`$ | $`Q_5`$ |
| $`I_2`$ |         |

<img width="683" height="385" alt="vertices-to-clip" src="https://github.com/user-attachments/assets/889fee5b-2f85-4d2a-abfd-6fc73f8e63e2" />


The resulting polygon is the set of points from the "inside" vertices list.

This operation needs to be repeated for each plane in the view frustum in order
to achieve frustum space clipping.

### Turning a Clipped Polygon Back Into Triangles

To turn a polygon back into a set of triangles, we can simply iterate through
sets of 3 vertices like so:

```
for (i = 0; i < (num_vertices - 2); ++i) {
    index0 = 0;
    index1 = i + 1;
    index2 = i + 2;

    create_triangle(index0, index1, index2);
}
```

### Clipping UV Coordinates

UV coordinates for triangles can be clipped via linear interpolation using same
interpolation factor that produces the new vertices along the edges of the
clipping plane.

### Homogeneous Clipping

Usually, graphics pipelines will perform clipping _after_ projection but before
perspective divide. There are several advantages to doing this:

 - Perspective divide is where x, y, and z are divided by w. Thus, before
   perspective divide, every vertex that is inside the frustum is between
   $`-1 * w`$ and $`1 * w`$, making frustum culling as trivial as comparing
   each component against $`w`$.
 - Texture coordinates can still be interpolated linearly in this space, since
   the perspective divide has not happened yet.
 - Division by zero is avoided, since clipping and culling are against
   $`z_{near}`$.

An additional resource on homogeneous clipping:
https://fabiensanglard.net/polygon_codec/index.php

## Additional Topics

### Rasterization Rules

Conventions that handle how geometry primitives should be rasterized.

Defined conventions can make sure cases like shared edges can be handled
properly without gaps or overdraw.

A "fill convention" handles these cases with neighboring triangles. One such
convention is called the "top left rule," where pixels are defined as "inside"
a triangle if they are along the top edge of left edge.

[Course video here](https://courses.pikuma.com/courses/take/learn-computer-graphics-programming/lessons/43873235-top-left-rasterization-rule)

### Optimizing Rasterization

 - https://kristoffer-dyrkorn.github.io/triangle-rasterizer/

[Course video here](https://courses.pikuma.com/courses/take/learn-computer-graphics-programming/lessons/43873406-edge-function-barycentric-weights)

### Subpixel Rasterization

Use floating points to represent sub-pixels. Bias towards the center of each
pixel when calculating geometry (0.5, 0.5).

Fixed-point math becomes important here (vs floating points).

[Course video here](https://courses.pikuma.com/courses/take/learn-computer-graphics-programming/lessons/43873890-subpixel-rasterization)

### Recommended Resources

 - [Foundations of Game Engine Development (Eric Lengyel)](https://foundationsofgameenginedev.com/)
 - [Graphics Programming Black Book (Michael Abrash)](https://www.drdobbs.com/parallel/graphics-programming-black-book/184404919)
 - [Texture Mapping Technical Articles (Chris Hecker)](https://www.chrishecker.com/Miscellaneous_Technical_Articles)
 - [Rasterization Rules & the Edge Function (ScratchAPixel.com)](https://www.scratchapixel.com/lessons/3d-basic-rendering/rasterization-practical-implementation/rasterization-stage)
 - [Rasterization Rules (Microsoft Direct3D)](https://docs.microsoft.com/en-us/windows/win32/direct3d9/rasterization-rules#triangle-rasterization-rules)
