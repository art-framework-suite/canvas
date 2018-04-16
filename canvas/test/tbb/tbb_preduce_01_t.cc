// vim: set sw=2 expandtab :
//
// Demonstrate trivial use of parallel_reduce over a vector with
// functors to ascertain the mean value in a vector, and the location
// and value of the minimum value in same.
//

#include "tbb/tbb.h"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <iterator>
#include <limits>
#include <numeric>
#include <sstream>
#include <vector>

using namespace std;

typedef tbb::blocked_range<typename vector<double>::const_iterator> br_t;

// Calculate the mean.
class Meanie {
public:
  Meanie();
  Meanie(Meanie const&, tbb::split);
  void operator()(br_t const& r);
  void join(Meanie const& other);
  size_t count() const;
  double result() const;

private:
  size_t running_count_;
  double running_sum_;
};

// Calculate the min.
class Minnie {
public:
  Minnie();
  Minnie(Minnie const& other, tbb::split);
  void operator()(br_t const& r);
  void join(Minnie const& other);
  double min() const;

private:
  double min_;
};

int
main()
{
  // Setup.
  size_t const n = 500000;
  double const val = 27.125; // Exactly representable as a double in IEEE.
  vector<double> v(n, val);
  // Mean.
  Meanie m;
  tbb::parallel_reduce(br_t(v.cbegin(), v.cend()), m);
  assert(m.count() == n);
  assert(m.result() == val);
  // Setup for min.
  vector<double>::difference_type const loc = 47856;
  double const minval = 22.3;
  v[loc] = minval;
  // Min.
  Minnie mincalc;
  tbb::parallel_reduce(br_t(v.cbegin(), v.cend()), mincalc);
  assert(mincalc.min() == minval);
}

Meanie::Meanie() : running_count_(0), running_sum_(0.0) {}

Meanie::Meanie(Meanie const&, tbb::split) : Meanie() {}

void
Meanie::operator()(br_t const& r)
{
  running_count_ += r.size();
  running_sum_ = accumulate(r.begin(), r.end(), running_sum_);
}

void
Meanie::join(Meanie const& other)
{
  running_count_ += other.running_count_;
  running_sum_ += other.running_sum_;
}

size_t
Meanie::count() const
{
  return running_count_;
}

double
Meanie::result() const
{
  return running_count_ ? (running_sum_ / running_count_) : 0.0;
}

Minnie::Minnie() : min_(numeric_limits<double>::max()) {}

Minnie::Minnie(Minnie const& /*other*/, tbb::split) : Minnie() {}

void
Minnie::operator()(br_t const& range)
{
  for (auto val : range) {
    if (val < min_) {
      min_ = val;
    }
  }
}

void
Minnie::join(Minnie const& other)
{
  if (other.min_ < min_) {
    min_ = other.min_;
  }
}

double
Minnie::min() const
{
  return min_;
}
