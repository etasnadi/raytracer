Raytracer based on Whitted, 1980 paper.

Only depends on `Qt` and `cmake` & `glm`. Tested on Ubuntu 20.04 (should work on Windows as well).

Build instructions: install the dependencies and use `cmake`.

![alt text](sample.png)

The room scene showing the refractions and reflections for thin and dense objects (shadows are disabled, only the blue ball supports it).

Two *primitives* implemented so far:
* Triangle
* Sphere

The readme uses the primitive and object terms interchangeably.

Entry point: `app.cc`.

The `display.cc` manages the drawing to the Qt canvas and also handles the keyboard input to the worker thread (world can be rotated using the arrows).

The worker thread calls the `Renderer.render()` in the `render.cc` once it receives a re-render signal from the environment.

The renderer sets up the viewport at initialization, and instantiates a `Scene` object in the constructor (the `RoomScene` is the only scene that is added in the `room_scene.cc`).

A `Scene` object has a `buildScene` method that instantiates the primitives using the `Models` helper class (only boxes and squares can be built using triangles) and the lights. The scene has the responsibility to intersect the ojbects in the scene using the `trace()` method (since it has the knowledge where the objects are for exaple, space partitioning algorithms should go here). It casts a ray and matches the closest object. The reference to the primitive is determined by a lookup based on the list of primitives in the scene and the matched object's `excite` function is called to determine its color at the intersection point.

Each object has a virtual `excite` method that receives the incoming `Ray` object, the `Intersection` struct (that contains the intersection info such as surface intersection point, surface normal at the intersection), and also, a weak pointer to a `Scene` object that can be used to recursively cast further rays to intersect other objects (this is a cyclyc dependence, but the reference to the `Scene` object will not be stored).

How a primitive determines its color is depend on the particular  `Shader` object assigned to the primitive using the `setShader()` method when building the scene. For example, a `Triangle` accepts a `TriangleShader` while a `Sphere` receives a
`SphereShader` to react to the incoming ray. The `excite()` function forwards the parameters to the shader, hence the user can prepare the object to react to the incoming radiation. The `Shader` base class contain helper methods to determine the reflective and refractive ray directions. (The code to determine the diffuse component is implemented in the `Scene` because the lights should be considered to properly handle the shadows). An example shader, the `GenericTriangleShader` is implemented to handle the reflection and the refraction. For the `Sphere`, a similar shader is added, but it is a bit more complicated because it is a dense object so the normals should be adjusted for refraction when the ray is entering or leaving the object.

The user can freely implement new custom shaders for better simulation.
