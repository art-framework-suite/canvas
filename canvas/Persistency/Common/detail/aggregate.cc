#include "canvas/Persistency/Common/detail/aggregate.h"

#include "CLHEP/Vector/TwoVector.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Matrix/Matrix.h"
#include "CLHEP/Matrix/SymMatrix.h"
#include "CLHEP/Matrix/Vector.h"

void
art::detail::aggregate(std::string&, std::string const&)
{
  throw art::Exception(art::errors::ProductCannotBeAggregated)
    << "Products of type \""
    << cet::demangle_symbol(typeid(std::string).name())
    << "\" cannot be aggregated.\n"
    << "Please contact artists@fnal.gov.\n";
}

void
art::detail::aggregate(CLHEP::HepVector& p, CLHEP::HepVector const& other)
{
  p += other;
}

void
art::detail::aggregate(CLHEP::Hep2Vector& p, CLHEP::Hep2Vector const& other)
{
  p += other;
}

void
art::detail::aggregate(CLHEP::Hep3Vector& p, CLHEP::Hep3Vector const& other)
{
  p += other;
}

void
art::detail::aggregate(CLHEP::HepLorentzVector& p, CLHEP::HepLorentzVector const& other)
{
  p += other;
}

void
art::detail::aggregate(CLHEP::HepMatrix& p, CLHEP::HepMatrix const& other)
{
  p += other;
}

void
art::detail::aggregate(CLHEP::HepSymMatrix& p, CLHEP::HepSymMatrix const& other)
{
  p += other;
}
