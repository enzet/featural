# Featural alternative to international phonetic alphabet

Goals:

  *  the alphabet is featural, meaning each element should represent some phonological feature of a sound, 
  *  symbols for most frequently used symbols should be as simple as possible, 
  *  the character elements must have the same line width so that they can be written by hand with a pen. 

## Build and run

Requirements:

  * C++20 compiler,
  * CMake version 3.30 or higher,
  * `xelatex` compiler ([XeTeX](https://tug.org/xetex/)),
  * [Doulos SIL](https://software.sil.org/doulos/download/) font.

To get a PDF file with the alphabet, run

```shell
./build.sh
```

It will create `build` directory and `out` directory with output PDF file.

