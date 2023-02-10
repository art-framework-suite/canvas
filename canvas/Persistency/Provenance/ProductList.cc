#include "canvas/Persistency/Provenance/ProductList.h"
#include "range/v3/view.hpp"

art::ProductDescriptions
art::make_product_descriptions(ProductList const& productList)
{
  using namespace ::ranges;
  return productList | views::values | to<ProductDescriptions>();
}
