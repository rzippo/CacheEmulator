# CacheEmulator
This project was made during my studying of computer architectures. Its purpose is to test the logic of the cache to see how it works.
Using C++ classes I implemented the logic for direct mapped, set-associative and full-associative caches. The main executes some write-then-read tests, first on each cache type and then on a 3-level cache hierarchy.

As with all with my recent projects, I worked on code quality focusing on meaningful names, class hierarchy and interactions.
To try out some advanced C++, I strived to avoid dynamic allocation using templates to define parameters of the caches, that is tag size, index size etc., allowing for static allocation of data structures.
