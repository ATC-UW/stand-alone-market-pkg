#include "MarketData.h"
#include <pybind11/pybind11.h>

#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)

namespace py = pybind11;

PYBIND11_MODULE(_core, m) {
  //   m.doc() = "pybind11 example plugin"; // optional module docstring

  //   m.def("add", &add, "A function which adds two numbers");

  m.doc() = "Market Maker Game"; // optional module docstring

  py::class_<MarketData>(m, "MarketData")
      .def(py::init<float, float>())
      .def("getNextBuyPrice", &MarketData::getNextBuyPrice)
      .def("getNextSellPrice", &MarketData::getNextSellPrice);

#ifdef VERSION_INFO
  m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
#else
  m.attr("__version__") = "dev";
#endif
}