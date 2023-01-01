# lsd_modern (WIP)
 C++ code to try to modernize the original Line Segment Detect code

## Line Detection Comparison
Image comparison between pytlsd C++ (left) and Opencv C++ (right).
![LSD](https://github.com/ibaiGorordo/lsd_modern/blob/main/doc/img/lines.jpg)


Table with the comparison (ms) of the different parts of the code:

|                     | OpenCV (C++) | pytlsd (C++) | lsd_modern |
|---------------------|--------------|--------------|------------|
| Gaussian Blur       | 6            | 47           | 15         |
| Gaussian Downsample | 6            | 39           | 16         |
| Total               | 48           | 214          |            |

For a more detailed comparison check the [COMPARISONS.md](https://github.com/ibaiGorordo/lsd_modern/blob/main/COMPARISONS.md) file.

# Ref:
- LSD paper: [LSD: A Line Segment Detector](https://www.ipol.im/pub/art/2012/gjmr-lsd/)
- Original C code: [lsd c code](https://github.com/theWorldCreator/LSD)
- pytlsd repo: [pytlsd](https://github.com/rpautrat/pytlsd)
- Fast Gaussian blur: [Fast Gaussian blur](http://blog.ivank.net/fastest-gaussian-blur.html)
- Separable Convolution: [Separable Convolution](https://github.com/chaowang15/fast-image-convolution-cpp)


