//
// Created by ibaig on 12/30/2022.
//

#include "GaussianDownsampler.h"

void test_create_kernel()
{
    auto kernel = GaussianDownsampler::calculate_kernel(0.8, 0.6);

    printf("Kernel: \n");
    for (auto& row : kernel) {
        for (auto& val : row) {
            printf("%f ", val);
        }
        printf("\n");
    }

}

int main() {
    test_create_kernel();
    return 0;
}
