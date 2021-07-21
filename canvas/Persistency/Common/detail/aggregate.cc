#include "canvas/Persistency/Common/detail/aggregate.h"

#include "CLHEP/Matrix/Matrix.h"
#include "CLHEP/Matrix/SymMatrix.h"
#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Vector/TwoVector.h"

using namespace CLHEP;

namespace art::detail {
  void
  CanBeAggregated<HepVector>::aggregate(HepVector& p, HepVector const& other)
  {
    p += other;
  }

  void
  CanBeAggregated<Hep2Vector>::aggregate(Hep2Vector& p, Hep2Vector const& other)
  {
    p += other;
  }

  void
  CanBeAggregated<Hep3Vector>::aggregate(Hep3Vector& p, Hep3Vector const& other)
  {
    p += other;
  }

  void
  CanBeAggregated<HepLorentzVector>::aggregate(HepLorentzVector& p,
                                               HepLorentzVector const& other)
  {
    p += other;
  }

  void
  CanBeAggregated<HepMatrix>::aggregate(HepMatrix& p, HepMatrix const& other)
  {
    p += other;
  }

  void
  CanBeAggregated<HepSymMatrix>::aggregate(HepSymMatrix& p,
                                           HepSymMatrix const& other)
  {
    p += other;
  }
}
