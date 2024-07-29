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

## Language utility

Language utility has two commands: `table` and `symbol`:


  *  `table <rows> <columns>`, where `rows` is the list of phoneme parameters separated by `,`. E.g. `table "dental,alveolar" "trill;voiceless,trill;voiced"`. 
  *  `symbol <descriptors>`, where `descriptors` is the list of symbol element descriptors. E.g. `symbol vc hc`. 

## Code and commit style

All C++ code should be formatted with clang-format, configuration file is `.clang-format`. All Python code should be formatted with Black, default configuration with `--line-length 80`.

