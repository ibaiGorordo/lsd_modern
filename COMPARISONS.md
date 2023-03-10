## Gaussian Blur Comparison
![Gaussian blur](https://github.com/ibaiGorordo/lsd_modern/blob/main/doc/img/gaussian_blur_diff.png)

- **pytlsd C++** (left diff with Opencv): 43 ms per call
- **Separable Convolution Gaussian blur** (right diff with Opencv): 8 ms per call
- **Opencv C++**: 0.5 ms per call

## Gaussian Downsample Comparison
![Gaussian downsample](https://github.com/ibaiGorordo/lsd_modern/blob/main/doc/img/gaussian_blur_resize_diff.png)

- **pytlsd C++** (left diff with Opencv): 31 ms per call
- **Custom Gaussian Downsample** (right diff with Opencv): 10 ms per call
- **Opencv C++**: 1.5 ms per call

## Angle Gradient Comparison
![Angle Gradient](https://github.com/ibaiGorordo/lsd_modern/blob/main/doc/img/gradient_diff.png)

- **pytlsd C++** (center diff with Opencv): 32.5 ms per call
- **Custom Gaussian Downsample** (right diff with Opencv): 2.9 ms per call
- **Opencv C++** (left): 5.6 ms per call