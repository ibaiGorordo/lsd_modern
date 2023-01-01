## Gaussian Blur Comparison
![Gaussian blur](https://github.com/ibaiGorordo/lsd_modern/blob/main/doc/img/gaussian_blur_diff.png)

- **pytlsd C++** (left diff with Opencv): 47 ms per call
- **Separable Convolution Gaussian blur** (right diff with Opencv): 15 ms per call
- **Opencv C++**: 6 ms per call

## Gaussian Downsample Comparison
![Gaussian blur](https://github.com/ibaiGorordo/lsd_modern/blob/main/doc/img/gaussian_blur_resize_diff.png)

- **pytlsd C++** (left diff with Opencv): 39 ms per call
- **Custom Gaussian Downsample** (right diff with Opencv): 16 ms per call
- **Opencv C++**: 6 ms per call