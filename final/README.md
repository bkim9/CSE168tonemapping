# torrey
UCSD CSE 168 renderer
https://cseweb.ucsd.edu/~tzli/cse168/

# Build
All the dependencies are included. Use CMake to build.
```
git clone https://github.com/BachiLi/torrey_public
mkdir build
cd build
cmake ..
```
It requires compilers that support C++17 (gcc version >= 8, clang version >= 7, Apple Clang version >= 11.0, MSVC version >= 19.14).

# Scenes
You should also download the scenes we will use in later homeworks from the following Google drive link: 
[https://drive.google.com/file/d/1SrGaw6AbyfhPs1NAuRjxSEQmf34DPKhm/view?usp=sharing](https://drive.google.com/file/d/1SrGaw6AbyfhPs1NAuRjxSEQmf34DPKhm/view?usp=sharing).

I download a wavefront file .obj in cbox directory I designed for both of the point light and the area light

# Run
Try 
```
cd build
./torrey -hw 1_1
```
This will generate an image "hw1_1.exr".

To view the image, use [hdrview](https://github.com/wkjarosz/hdrview), or [tev](https://github.com/Tom94/tev).

# Acknowledgement
The renderer is heavily inspired by [pbrt](https://pbr-book.org/), [mitsuba](http://www.mitsuba-renderer.org/index_old.html), [the ray tracing series](https://raytracing.github.io/), and [darts](https://cs87-dartmouth.github.io/Fall2022/darts-overview.html).

We use [pugixml](https://pugixml.org/) to parse XML files.

We use [stb_image](https://github.com/nothings/stb) and [tinyexr](https://github.com/syoyo/tinyexr) for reading & writing images.

We use [miniz](https://github.com/richgel999/miniz) for compression & decompression.

We use [tinyply](https://github.com/ddiakopoulos/tinyply) for parsing PLY files.

Many scenes in the scenes folder are downloaded from:
- [http://www.mitsuba-renderer.org/download.html](http://www.mitsuba-renderer.org/download.html)
- [https://benedikt-bitterli.me/resources/](https://benedikt-bitterli.me/resources/)
- [https://casual-effects.com/data/](https://casual-effects.com/data/)



HW3 Bonus 8.  Schlick Fresnel vs actual Fresnel
    Natty Hoffman's video states that the acutal Fresnel is long and approximation of it will suffice. I agree on the idea. I think that as long as the user cannot recognize the difference, we should choose speed instead of accuracy
HW3 Bonus 10: sample one light at a time
    I used random number to make possible for sampling one light at a time in hw3.cpp:55~56
HW3 Bonus 11. I have used cone sampling for picking a point on a     sphere at shape.h