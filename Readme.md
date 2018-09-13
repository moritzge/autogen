# Autogen

[![pipeline status](https://gitlab.inf.ethz.ch/moritzge/autogen/badges/master/pipeline.svg)](https://gitlab.inf.ethz.ch/moritzge/autogen/commits/master)

## Build

### Thirdparty

To build the tests, install [Gtest](https://github.com/google/googletest).

```
mkdir build && cd build
cmake ..
make
```

This compiles the libraries and the examples.

## Usage
Check out the examples in the `examples` directory to see how to use this library.

## Presentation

There is a presentation explaining the basics of AutoDiff and CodeGen:
[presentation](https://docs.google.com/presentation/d/1PGR5WiA2m16bUKGwrXlcyfAsUH97gMyQpcSJWcFpP1A/edit?usp=sharing)

And a repo with the examples: [repo](https://gitlab.inf.ethz.ch/moritzge/autogen-presentation)

## Todo

- [ ] instead of writing code, evaluating expression graph
- [ ] make benchmarks
- [ ] check out template meta programming
- [ ] more symbolic simplification including different node types
- [ ] autodiff/autogen objective in scp
