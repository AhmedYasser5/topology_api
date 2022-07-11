## Features

* Library is written in C++ for the following reasons:
  1. C++ is one of the fastest programming languages when it comes to performance metrics.
	1. C++ is an object oriented language which suits the requirements of the library.
	1. The standard template library along with shared pointers provided there are just suitable for the requirements of the library.
	1. The library can be configured to make the API available for nearly any other programming language.
	1. I have a better knowledge of this programming language than any other language since I use it most of the time.

* The library is built using Makefiles into a shared library.
* The documentation can be found in [wiki](http://github.com/AhmedYasser5/topology_api/wiki).

## Getting Started

To get started, you need to compile the library into a shared library using the following command:

```shell
make -C lib RELEASE=1
```

After that, you can run the demo code using `make RELEASE=1 run`, or just compile it using `make RELEASE=1`.
