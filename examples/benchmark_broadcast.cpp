#include <iostream>
#include <chrono>
#include "broadcasting.hpp"

int main() {
    using Shape = shadic::ShapeTracker::Shape;
    
    Shape s1 = {2, 1, 4};
    Shape s2 = {3, 4};

    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 1000000; ++i) {
        volatile auto res = shadic::ShapeTracker::broadcast_shapes(s1, s2);
        (void)res;
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;

    Shape final_shape = shadic::ShapeTracker::broadcast_shapes(s1, s2);
    
    std::cout << "Broadcast Benchmark Completed successfully!\n";
    std::cout << "Time taken for 1M iterations: " << elapsed.count() << " ms\n";
    std::cout << "Resulting Broadcast Shape: [ ";
    for (size_t dim : final_shape) {
        std::cout << dim << " ";
    }
    std::cout << "]\n";

    return 0;
}
