#pragma once

#include<any>
#include<stdexcept>
#include<tuple>
#include<typeindex>
#include<unordered_map>

namespace di {

// Injectable traits defines the factory function to create an injectable type. An
// injectable type is any type that depends on other types.
template<typename T>
struct injectable_traits
{
    // To register your type with the dependency injection system, create a
    // specialization of this type. It must have a static `make_instance` function
    // which creates an object of type T and returns it. Any dependencies should
    // be passed as references.
    //
    // For example:
    //
    //   static auto make_instance(dep1 const& d1, dep2& d2) {
    //     return obj { d1, d2 };
    //   }
    //
    // TODO: Allow conversion from stored ref, std::unique_ptr, shared_ptr, or
    //       pointer to reference, pointr, or shared_ptr when the conversion is safe?
    //       For example, unique_ptr -> ref or ptr is safe. shared_ptr to ref or
    //       ptr is not. Add hint type in signature of make_instance to indicate
    //       unsafe call is okay?
};

// A dependency injection container. Owns dependencies stored in it.
//
// TODO: Add method for registering existing dependencies not owned by the container.
class container
{
public:
    container() noexcept = default;
    container(container&&) noexcept = default;
    container(container const&) noexcept = default;

    // TODO: Add constructor that takes a parent container. See
    //       `detail::get_dependencies` for use information.

public:
    ~container() noexcept = default;

public:
    container& operator=(container&&) noexcept = default;
    container& operator=(container const&) noexcept = default;

public:
    // Instantiate and register type T with the dependency container.
    // T must be copy constructable (only because std::any doesn't
    // support move-only types)
    //
    // TODO: Support move-only types.
    // TODO: Support checking self for instantiating types that are
    //       registered (e.g. don't require args if we have what we
    //       need in our dependencies_ container)
    template<typename T, typename... TArgs>
    T& instantiate(TArgs&&... args)
    {
        auto const tid = std::type_index { typeid(T) };
        auto i = dependencies_.find(tid);
        if (i != dependencies_.cend()) {
            throw std::runtime_error("Type already registered");
        }
        auto obj = T { std::forward<TArgs>(args)... };
        auto result = dependencies_.emplace(tid, std::move(obj));
        return std::any_cast<T&>(result.first->second);
    }

    // Retrieve a reference to a type T that has been previously registered
    template<typename T>
    T& get_dependency()
    {
        auto const i = dependencies_.find(std::type_index { typeid(T) });
        if (i == dependencies_.cend()) {
            throw std::runtime_error("Type not registered");
        }
        return std::any_cast<T&>(i->second);
    }

private:
    using container_map = std::unordered_map<std::type_index, std::any>;

private:
    container_map dependencies_;
};

namespace detail {
    // Helper struct to identify function parameter types.
    template<typename TFn>
    struct instance_parameters { };

    template<typename T, typename... TArgs>
    struct instance_parameters<T(TArgs...)>
    {
        // We remove all const/volatile and reference modifiers so we
        // can be explicit about storing a reference.
        using parameter_tuple_t = std::tuple<std::remove_cvref_t<TArgs>&...>;
    };

    // Invokes container::get_dependency for all types in TTuple. Returns
    // a tuple containing references to those types.
    //
    // TODO: If container has a parent, and the type is not registered in c,
    //       search the parent, recursively, for the type. This allows type
    //       replacement during dependency injection.
    //
    // TODO: Allow this to work for stored const types.
    template<typename TTuple, std::size_t... I>
    TTuple get_dependencies(container& c, std::index_sequence<I...>)
        { return std::tie(c.get_dependency<
            // The dependency manager's get_dependency doesn't include
            // reference in the type, so make sure we remove that.
            std::remove_reference_t<std::tuple_element_t<I, TTuple>>>()... ); }
}

// Create an object of type T
template<typename T>
inline static auto make_injected(container& container)
{
    using parameters_t = typename detail::instance_parameters<
        decltype(injectable_traits<T>::make_instance)>::parameter_tuple_t;
    auto parameters = detail::get_dependencies<parameters_t>(
        container, std::make_index_sequence<std::tuple_size_v<parameters_t>>());
    return std::apply(injectable_traits<T>::make_instance, parameters);
}

}
