////
//// Created by ibaig on 1/1/2023.
////
//#include "chrono"
//#include <opencv2/opencv.hpp>
//
//#include "lsd.h"
//#include "GaussianDownsampler.h"
//
//using namespace std::chrono;
//
//std::unique_ptr<GaussianDownsampler> gaussian_downsampler;
//
//
//cv::Mat test(const std::function<cv::Mat(cv::Mat&)>& angle_gradient,
//             const std::string& func_name,
//             int num_tests,
//             bool show=false)
//{
//    cv::Mat gray_img = cv::imread("assets/bathroom.jpg",
//                                  cv::IMREAD_GRAYSCALE);
//    cv::Mat out_img;
//    cv::Mat ang_img = cv::Mat::zeros(gray_img.size(), CV_64F);
//    auto start = high_resolution_clock::now();
//    for (int i = 0; i < num_tests; ++i)
//    {
//        out_img = angle_gradient(gray_img);
//    }
//    auto end = high_resolution_clock::now();
//    auto duration = duration_cast<milliseconds>(end - start);
//    auto mean_time = duration.count() / static_cast<long>(num_tests);
//    printf("%s: %lld ms per call\n", func_name.c_str(), mean_time);
//
//    if (show)
//    {
//        cv::namedWindow(func_name, cv::WINDOW_NORMAL);
//        cv::imshow(func_name, out_img);
//    }
//
//    return out_img;
//}
//
//cv::Mat draw_image_diff(cv::Mat& img1,
//                        cv::Mat& img2,
//                        const std::string& title="diff",
//                        bool show=false)
//{
//    cv::Mat diff;
//    cv::absdiff(img1, img2, diff);
//
//    // Normalize the difference with the min max to apply a colormap
//    double min, max;
//    cv::minMaxLoc(diff, &min, &max);
//    cv::Mat adjMap;
//    diff.convertTo(adjMap, CV_8UC1, 255 / (max-min), -min);
//
//    // Apply the colormap
//    cv::Mat diff_color;
//    cv::applyColorMap(adjMap, diff_color, cv::COLORMAP_JET);
//
//    if(show)
//    {
//        cv::namedWindow(title, cv::WINDOW_NORMAL);
//        cv::imshow(title, diff_color);
//    }
//    return diff_color;
//}
//
//cv::Mat opencv_angle_gradient(const cv::Mat& gray) {
//    // Test Opencv Gaussian Blur
//    cv::Mat opencv_gaussian_img;
//    auto sigma = 0.6/0.8;
//    const double sprec = 3;
//    const auto h =  (int)(ceil(sigma * sqrt(2 * sprec * log(10.0))));
//    cv::Size ksize(1 + 2 * h, 1 + 2 * h); // kernel size
//    cv::GaussianBlur(gray, opencv_gaussian_img, ksize, sigma);
//
//    return opencv_gaussian_img;
//}
//
//void pytlsd_angle_gradient(const cv::Mat& gray,
//                           cv::Mat& angles,
//                           cv::Mat& magnitudes) {
//
//    cv::Mat img_flt;
//    gray.convertTo(img_flt, CV_64F);
//    auto *imagePtr = reinterpret_cast<double *>(img_flt.data);
//
//    image_double image;
//    image->data = imagePtr;
//    image->xsize = gray.cols;
//    image->ysize = gray.rows;
//
//    auto *angPtr = reinterpret_cast<double *>(ang_img.data);
//
//    image_double g;
//    g->data = angPtr;
//    g->xsize = gray.cols;
//    g->ysize = gray.rows;
//    image_double modgrad;
//
//    cv::Mat grad_img;
//    grad_img = cv::Mat::zeros(gray.size(), CV_64F);
//    auto *gradPtr = reinterpret_cast<double *>(grad_img.data);
//
//    modgrad->data = gradPtr;
//    modgrad->xsize = gray.cols;
//    modgrad->ysize = gray.rows;
//
//    auto threshold = 5.2262518595055063;
//    grad_angle_orientation(image, threshold, g, modgrad);
//
//    cv::Mat out_img_8u;
//    out_img.convertTo(out_img_8u, CV_8U);
//    free((void *) out_image_double->data);
//    free((void *) out_image_double);
//    return out_img_8u;
//}
//
//cv::Mat custom_angle_gradient(const cv::Mat& gray) {
//    auto *imagePtr = reinterpret_cast<unsigned char *>(gray.data);
//    cv::Mat out_img;
//    out_img = cv::Mat::zeros(gray.size(), CV_8U);
//    auto *outPtr = reinterpret_cast<unsigned char *>(out_img.data);
//
//    gaussian_downsampler->blur(imagePtr,
//                               outPtr,
//                               gray.cols, gray.rows);
//    return out_img;
//}
//
//int main() {
//    auto num_test = 10;
////    auto opencv_gaussian_img = test(opencv_angle_gradient,
////                                    "opencv_angle_gradient",
////                                    num_test);
////
////    gaussian_downsampler = std::make_unique<GaussianDownsampler>(0.8f, 0.6f);
////    auto sepconv_gaussian_img = test(custom_angle_gradient,
////                                     "custom_angle_gradientr",
////                                     num_test);
//
//    auto pytlsd_gaussian_img = test(pytlsd_angle_gradient,
//                                    "pytlsd_angle_gradient",
//                                    num_test,
//                                    true);
//
////    auto diff1 = draw_image_diff(pytlsd_gaussian_img,
////                                 opencv_gaussian_img,
////                                 "Pytlsd Vs Opencv",
////                                 true);
////
////    auto diff2 = draw_image_diff(sepconv_gaussian_img,
////                                 opencv_gaussian_img,
////                                 "SepConv Vs Opencv",
////                                 true);
////
////    cv::Mat combined_diff;
////    cv::hconcat(diff1, diff2, combined_diff);
////    cv::imwrite("../../doc/img/gaussian_blur_diff.png", combined_diff);
//
//    cv::waitKey(0);
//    return 0;
//}