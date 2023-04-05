#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>
#include <pybind11/eigen.h>
#include <iostream>

namespace py = pybind11;

// wrap c++ function with Numpy array IO
void wavefront_converter(const py::array_t<float>& obj_array, py::array_t<float>& output) {
    auto buf = obj_array.request();
    const float* const ptr = (float*)buf.ptr;

    auto result_shape = obj_array.shape();

    size_t num_triangles = result_shape[0];

    std::vector<Eigen::Matrix3f> results(num_triangles, Eigen::Matrix3f::Zero());

    size_t pos = 0;
    for (size_t i = 0; i < results.size(); i++) {
        float* mat_data = results[i].data();
        for (size_t j = 0; j < 9; j++) {
            mat_data[j] = ptr[pos];
            pos++;
        }
    }

    float record = 0.0;
    for (Eigen::Matrix3f& mat: results) {
        mat += Eigen::Matrix3f::Ones() * record;
        record += 1.0;
    }

    auto output_buf = output.request();
    float* out_ptr = (float*)output_buf.ptr;
    pos = 0;
    for (size_t i = 0; i < results.size(); i++) {
        float* mat_data = results[i].data();
        for (size_t j = 0; j < 9; j++) {
            out_ptr[pos] = mat_data[j];
            pos++;
        }
    }
}

PYBIND11_MODULE(wavefront_cpp, m) {
    // optional module docstring
    m.doc() = "pybind11 wavefront processing plugin";

    m.def("wavefront_convert", &wavefront_converter, "Wavefront parser C++ API");
}