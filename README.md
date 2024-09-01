# Dependency Injection Example

This is a simple(ish) C++ example that demonstrates a dependency injection
container.

The container is capable of automatically deducing the required dependency
types, from a registered factory function, and creating the requested type.


# Configure and Build

To configure and build the project:

```
cmake --preset=windows-x64-debug
cmake --build --preset=windows-x64-debug
```

You may also use linux and/or release presets.

The executable will be placed into the `bin` directory. Program output:

```
Created
```

# How it works

The `di` namespace contains a `container` class which stores dependencies
in an unordered map, indexed by `std::type_index`. Types are added to the
container via the `instantiate` function (`add_type` would have been a
better name).

To create a type with the dependency container, you must inform the
dependency injection code how to create the type. This is done by
specializing the `injectable_traits` struct for your type. The
specialization must contain a a static `make_instance` member function.
The dependency injection code will inspect the signature of this function
to determine what dependencies should be passed. This function will also
be called when creating your type.

To create a type, first create a `di::container`, instantiate your
dependencies, and then call `make_injected`.

A full example is in `main`.
