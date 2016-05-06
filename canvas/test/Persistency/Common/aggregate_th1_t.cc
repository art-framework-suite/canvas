#define BOOST_TEST_MODULE ( th1Product aggregation Test )
#include "boost/test/auto_unit_test.hpp"
#include "canvas/test/Persistency/Common/MockRun.h"
#include "cetlib/container_algorithms.h"

#include "TH1F.h"

#include <vector>

using arttest::MockRun;

namespace {
  double constexpr tolerance {std::numeric_limits<float>::epsilon()};
}

BOOST_AUTO_TEST_SUITE (aggregate_th1_t)

BOOST_AUTO_TEST_CASE (th1f)
{
  TH1F h1 {"title1", "name1", 10, 0, 10.}; h1.Sumw2();
  TH1F h2 {"title2", "name2", 10, 0, 10.}; h2.Sumw2();

  BOOST_REQUIRE_EQUAL(h1.GetNbinsX(), h2.GetNbinsX());

  std::vector<float> h1d {123., 4., 12491., 125., 5., 4., 4., 12., 0., 5454.};
  std::vector<float> h2d {5., 4.,6., 6., 4121., 4.55, 1.7, 448., 15., 0.};

  BOOST_REQUIRE_EQUAL(h1.GetNbinsX(), h1d.size());
  BOOST_REQUIRE_EQUAL(h1d.size(), h2d.size());

  auto error = [](float val) { return std::sqrt(val); };

  std::vector<float> h1err;
  std::vector<float> h2err;
  cet::transform_all(h1d, std::back_inserter(h1err), error);
  cet::transform_all(h2d, std::back_inserter(h2err), error);

  BOOST_REQUIRE_EQUAL(h1d.size(), h2err.size());
  BOOST_REQUIRE_EQUAL(h1err.size(), h2err.size());

  for (std::size_t i{0} ; i != h1d.size(); ++i) {
    h1.SetBinContent(i+1, h1d[i]);
    h2.SetBinContent(i+1, h2d[i]);

    h1.SetBinError(i+1, h1err[i]);
    h2.SetBinError(i+1, h2err[i]);
  }

  MockRun r;
  r.put<TH1F>(h1);
  r.put<TH1F>(h2);

  auto linear_add    = [](float const val1, float const val2){ return val1+val2; };
  auto quadratic_add = [](float const val1, float const val2){ return std::sqrt(val1*val1+val2*val2); };

  std::vector<float> h3d;
  std::vector<float> h3err;
  cet::transform_all(h1d, h2d, std::back_inserter(h3d), linear_add);
  cet::transform_all(h1err, h2err, std::back_inserter(h3err), quadratic_add);

  BOOST_REQUIRE_EQUAL(h2d.size(), h3d.size());
  BOOST_REQUIRE_EQUAL(h3d.size(), h3err.size());

  // Aggregation happens here
  auto const& h3 = r.get<TH1F>();
  BOOST_REQUIRE_EQUAL(h3d.size(), h3.GetNbinsX());

  for (std::size_t i{0} ; i != h3d.size() ; ++i){
    BOOST_CHECK_CLOSE_FRACTION( h3.GetBinContent(i+1), h3d[i], tolerance );
    BOOST_CHECK_CLOSE_FRACTION( h3.GetBinError(i+1), h3err[i], tolerance );
  }

}

BOOST_AUTO_TEST_SUITE_END()
