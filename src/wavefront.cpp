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

class BVH {
public:
    BVH() {}
    BVH(int num): a(num), b(num), c(num), d(num) {
        mini.resize({3});
        maxi.resize({3});
    }
public:
    py::array_t<float> mini;
    py::array_t<float> maxi;
    int a, b, c, d;
};

py::list get_bvh(int bvh_num) {
    std::vector<BVH> bvhs;
    for (int i = 0; i < bvh_num; i++) {
        BVH new_bvh(i);
        float *const min_ptr = new_bvh.mini.mutable_data(0), *const max_ptr =  new_bvh.maxi.mutable_data(0);
        for (int j = 0; j < 3; j++) {
            min_ptr[j] = float(i);
            max_ptr[j] = float(i + 1);
        }
        bvhs.push_back(new_bvh);
    }
    return py::cast(bvhs);
}

PYBIND11_MODULE(wavefront_cpp, m) {
    // optional module docstring
    m.doc() = "pybind11 wavefront processing plugin";

    m.def("wavefront_convert", &wavefront_converter, "Wavefront parser C++ API");
    m.def("get_bvh", &get_bvh, "Get bvh");

    py::class_<BVH>(m, "BVH")
        .def_readwrite("mini", &BVH::mini)
        .def_readwrite("maxi", &BVH::maxi)
        .def_readwrite("a", &BVH::a)
        .def_readwrite("b", &BVH::b)
        .def_readwrite("c", &BVH::c)
        .def_readwrite("d", &BVH::d)
        .def(py::init<int>());
}