#include "MarketData.h"
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)

namespace py = pybind11;

PYBIND11_MODULE(_core, m) {
  m.doc() = "Market Price Simulator";

  py::class_<Regime, std::shared_ptr<Regime>>(m, "Regime");

  py::class_<RandomWalkRegime, Regime, std::shared_ptr<RandomWalkRegime>>(
      m, "RandomWalk")
      .def(py::init<float>(), py::arg("volatility") = 0.01f);

  py::class_<SineWaveRegime, Regime, std::shared_ptr<SineWaveRegime>>(
      m, "SineWave")
      .def(py::init<float, float, float>(), py::arg("volatility") = 0.01f,
           py::arg("amplitude") = 1.0f, py::arg("phase") = 0.0f);

  py::class_<DropRegime, Regime, std::shared_ptr<DropRegime>>(m, "Drop")
      .def(py::init<float>(), py::arg("rate") = 0.01f);

  py::class_<SpikeRegime, Regime, std::shared_ptr<SpikeRegime>>(m, "Spike")
      .def(py::init<float>(), py::arg("rate") = 0.05f);

  py::class_<GBMRegime, Regime, std::shared_ptr<GBMRegime>>(m, "GBM")
      .def(py::init<float, float>(), py::arg("mu") = 0.0005f,
           py::arg("sigma") = 0.02f);

  py::class_<MeanReversionRegime, Regime,
             std::shared_ptr<MeanReversionRegime>>(m, "MeanReversion")
      .def(py::init<float, float, float>(), py::arg("mu") = 100.0f,
           py::arg("theta") = 0.1f, py::arg("sigma") = 0.5f);

  py::class_<JumpDiffusionRegime, Regime,
             std::shared_ptr<JumpDiffusionRegime>>(m, "JumpDiffusion")
      .def(py::init<float, float, float, float>(), py::arg("mu") = 0.0f,
           py::arg("sigma") = 0.02f, py::arg("jump_intensity") = 0.1f,
           py::arg("jump_size") = 0.05f);

  py::class_<MomentumRegime, Regime, std::shared_ptr<MomentumRegime>>(
      m, "Momentum")
      .def(py::init<float, float, float>(), py::arg("mu") = 0.0f,
           py::arg("sigma") = 0.02f, py::arg("momentum") = 0.0f);

  py::class_<TrendingMeanReversionRegime, Regime,
             std::shared_ptr<TrendingMeanReversionRegime>>(
      m, "TrendingMeanReversion")
      .def(py::init<float, float, float, float>(), py::arg("mu") = 100.0f,
           py::arg("drift") = 0.0f, py::arg("theta") = 0.1f,
           py::arg("sigma") = 0.5f);

  py::class_<RegimeAssignment>(m, "RegimeAssignment")
      .def(py::init<std::shared_ptr<Regime>, int, int>(), py::arg("regime"),
           py::arg("start_day"), py::arg("end_day"));

  py::class_<MarketData>(m, "_MarketData")
      .def(py::init<float, float, std::vector<RegimeAssignment>,
                    std::optional<unsigned int>>(),
           py::arg("start_buy_price"), py::arg("start_sell_price"),
           py::arg("regimes"), py::arg("seed") = py::none())
      .def("getBuyPrices", &MarketData::getBuyPrices, py::arg("start") = 0,
           py::arg("end") = -1)
      .def("getSellPrices", &MarketData::getSellPrices, py::arg("start") = 0,
           py::arg("end") = -1)
      .def("getTotalDays", &MarketData::getTotalDays);

#ifdef VERSION_INFO
  m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
#else
  m.attr("__version__") = "dev";
#endif
}
