#include"dependency_injection.h"
#include<iostream>

struct dependency1 { };
struct dependency2 { };
struct dependency3 { };

class injectable
{
public:
    explicit injectable(dependency3& d3,
                        dependency1 const& d1,
                        dependency2 const& d2)
    {
        std::cout << "Created" << std::endl;
    }
};

// Register our type with the dependency injection container
// by providing a factory function it can inspect for the
// required dependencies.
template<>
struct di::injectable_traits<injectable>
{
    static auto make_instance(dependency3& d3,
                              dependency1 const& d1,
                              dependency2 const& d2)
        { return injectable { d3, d1, d2 }; }
};


int main([[maybe_unused]]int argc,
         [[maybe_unused]]char** argv)
{
    // Create an example container
    auto di = di::container {};
    di.instantiate<dependency1>();
    di.instantiate<dependency2>();
    di.instantiate<dependency3>();

    // Create an `injectable` and automatically pass references
    // to references, 2, and 3 from within the di container.
    auto obj = make_injected<injectable>(di);

    return 0;
}
