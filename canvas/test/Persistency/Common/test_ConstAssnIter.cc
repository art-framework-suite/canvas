#include "canvas/Persistency/Common/Assns.h"
#include "canvas/Persistency/Common/AssnsIter.h"
#include "canvas/Persistency/Common/Ptr.h"

#include <cassert>
#include <forward_list>
#include <type_traits>

typedef std::vector<int>         intvec_t;
typedef std::vector<short>       shortvec_t;
typedef std::vector<std::string> strvec_t;
typedef std::vector<float>       floatvec_t;
typedef art::Assns<int, float> assns_ab_t;
typedef art::Assns<int, float, short> assns_abd_t;
typedef assns_abd_t::const_iterator ci_t;

int main() {

    //initialization stuff
    art::ProductID const viid(2);
    art::ProductID const vfid(3);
    //auto const vs = strvec_t {"one", "one-a", "two", "two-a", "three", "three-a"};
    auto const vi = intvec_t {1, 2, 3};
    auto const vf = floatvec_t {1.0, 1.1, 2.0, 2.1, 3.0, 3.1 };
    auto const vs = shortvec_t {10, 11, 20, 21, 30, 31 };
    auto const vtest = intvec_t {1, 1, 2, 2, 3, 3};

    int k = 0;

    assns_abd_t assns;
    assns_ab_t a1;

    for (long unsigned int i=0; i < 3; ++i) {
        auto p1 = art::Ptr<int>(viid, &vi[i], i);
        for(long unsigned int j=0; j < 2; ++j) {
            auto p2 = art::Ptr<float>(vfid, &vf[k], k);
            assns.addSingle(p1, p2, vs[k]);
            a1.addSingle(p1, p2);
            ++k;
        }
    }

    //iterator increment and dereference test
    auto my_begin = assns.begin();
    auto my_end = assns.end();

    auto check_values = [](auto r, short s, float f, int j) {
        assert(*(r.data)==s);
        assert(*(r.second.get())==f);
        assert(*(r.first.get())==j);
    };

    k = 0;
    for(auto p = my_begin; p!=my_end; ++p) {
        auto const &r = *p;
        check_values(r, vs[k], vf[k], vtest[k]);
        ++k;
    }

    //range-for loop test
    k=0;
    for(auto const &p:assns) {
        check_values(p, vs[k], vf[k], vtest[k]);
        ++k;
    }

    //std::for_each test
    auto print = [](const auto & r) { std::cout << " " << *(r.data); };
    std::for_each(assns.begin(), assns.end(), print);
    std::cout << std::endl;

//    std::cout << std::boolalpha;
    static_assert(std::is_copy_constructible<ci_t>::value, "Not copy constructible");
    static_assert(std::is_copy_assignable<ci_t>::value, "Not copy assignable");
    static_assert(std::is_move_assignable<ci_t>::value, "Not move assignable");

    assert(*((*(my_begin+4)).data)== 30);
    my_begin += 4;
    assert(*((*my_begin).data) == 30);
    assert(my_begin-my_end == 2);
    assert(*(my_begin[0].data) == 30);
    assert(*(my_begin->data) ==30);
    assert((my_begin < my_end) == true);
    assert((my_begin <= my_end) ==true);
    assert((my_begin > my_end) ==false);
    assert((my_begin >= my_end) ==false);

    return 0;
}
