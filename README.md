# lsd_modern (WIP)
 C++ code to try to modernize the original Line Segment Detect code

## Line Detection Comparison
![LSD](https://github.com/ibaiGorordo/lsd_modern/blob/main/doc/img/lines.jpg)

- **pytlsd C++** (left): 214 ms per call
- **Opencv C++** (right): 48 ms per call

## Gaussian Blur Comparison
![Gaussian blur](https://github.com/ibaiGorordo/lsd_modern/blob/main/doc/img/gaussian_blur_diff.png)

- **pytlsd C++** (left diff with Opencv): 60 ms per call
- **Fast Gaussian blue** (right diff with Opencv): 41 ms per call
- **Opencv C++**: 6 ms per call

# Ref:
- LSD paper: [LSD: A Line Segment Detector](https://www.ipol.im/pub/art/2012/gjmr-lsd/)
- Original C code: [lsd c code](https://github.com/theWorldCreator/LSD)
- pytlsd repo: [pytlsd](https://github.com/rpautrat/pytlsd)
- Fast Gaussian blur: [Fast Gaussian blur](http://blog.ivank.net/fastest-gaussian-blur.html)


