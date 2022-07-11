## Features

* Library is written in C++ for the following reasons:
  1. C++ is one of the best programming languages when it comes to performance, and it is also an OOP language.
	1. The standard template library along with smart pointers provided there are suitable for the requirements of the library.

* The library is built using Makefiles into a shared library.
* The documentation can be found in [wiki](http://github.com/AhmedYasser5/topology_api/wiki).

## Getting Started

To get started, you need to compile the library into a shared library using the following command:

```shell
make -C lib RELEASE=1
```

After that, you can run the demo code using `make RELEASE=1 run`, or just compile it using `make RELEASE=1`.
