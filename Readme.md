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

- [ ] more symbolic simplification including different node types
- [~] Nodes with Matrix in/output
    - [x] addition
    - [x] addition with scalar
    - [x] multiplication
    - [x] mult with scalar
    - [x] transpose
    - [ ] subtraction
    - [ ] cross
    - [ ] dot: this means to connect it with scalar nodes
    - [ ] division/inverse
    - [ ] connect everything neatly with scalar nodes/rectype
- [ ] instead of writing code, evaluating expression graph
- [ ] make benchmarks
- [ ] check out template meta programming
- [ ] autodiff/autogen objective in scp

