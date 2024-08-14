import sys
from argparse import ArgumentParser, Namespace
from typing import Any, Dict, List, Set, Tuple
from textwrap import dedent

from moire.moire import Moire

__author__ = "Sergey Vartanov"
__email__ = "me@enzet.ru"

depth = 0
status = {}
BLOCK_TAGS: Set[str] = {
    "block", "body", "code", "title", "number", "list", "image", "table"
}  # fmt: skip
Arguments = List[Any]


class TagNotImplementedError(NotImplementedError):
    """Tag is not implemented in the parser."""

    def __init__(self, tag: str = "") -> None:
        self.tag: str = tag

    def __str__(self) -> str:
        return f"Tag \\{self.tag} is not implemented in the parser"


class Default(Moire):
    """Default tag declaration."""

    def __init__(self) -> None:
        super().__init__()

    def title(self, arg: Arguments) -> str:
        """Document title."""
        return ""

    def header(self, arg: Arguments, level: int) -> str:
        """Header.

        Arguments: <header text> <header identifier>?
        """
        raise TagNotImplementedError("header")

    def m(self, arg: Arguments) -> str:
        """Monospaced text."""
        raise TagNotImplementedError("m")


class DefaultTeX(Default):
    """TeX syntax."""

    name = "Tex"
    id_: str = "tex"
    extension = "tex"

    escape_symbols = {
        "_": "\\_",
    }
    block_tags = BLOCK_TAGS
    headers: List[str] = [
        "section", "subsection", "subsubsection", "paragraph", "subparagraph"
    ]  # fmt: skip

    def body(self, arg: Arguments) -> str:
        s = dedent(
            """\
            \\documentclass[twoside,psfig]{article}
            \\usepackage[utf8]{inputenc}
            \\usepackage[russian]{babel}
            \\usepackage{enumitem}
            \\usepackage{float}
            \\usepackage[margin=3cm,hmarginratio=1:1,top=32mm,columnsep=20pt]
                {geometry}
            \\usepackage{graphicx}
            \\usepackage{hyperref}
            \\usepackage{multicol}
            \\begin{document}
            """
        )
        s += self.parse(arg[0], in_block=True)
        s += "\\end {document}"
        return s

    def title(self, arg: Arguments) -> str:
        s = f"\\title{{{self.parse(arg[0])}}}\n"
        s += "\\maketitle"
        return s

    def author(self, arg: Arguments) -> str:
        return f"\\author{{{self.parse(arg[0])}}}"

    def header(self, arg: Arguments, number: int) -> str:
        if number < 6:
            return f"\\{self.headers[number - 1]}{{{self.parse(arg[0])}}}"
        return self.parse(arg[0])

    def table(self, arg: Arguments) -> str:
        s = "\\begin{table}[h]\n\\begin{center}\n\\begin{tabular}{|"
        max_tds = 0
        for tr in arg:
            if isinstance(tr, list):
                tds = 0
                for td in tr:
                    if isinstance(td, list):
                        tds += 1
                if tds > max_tds:
                    max_tds = tds
        for k in range(max_tds):
            s += "l|"
        s += "}\n\\hline\n"
        for tr in arg:
            if isinstance(tr, list):
                tds = []
                for td in tr:
                    if isinstance(td, list):
                        tds.append(td)
                for td in tds[:-1]:
                    s += self.parse(td) + " & "
                s += self.parse(tds[-1])
                s += " \\\\\n\\hline\n"
        s += "\\end{tabular}\n\\end{center}\n\\end{table}\n"
        return s

    def list__(self, arg: Arguments) -> str:
        s = "\\begin{itemize}\n"
        for item in arg:
            s += f"\\item {self.parse(item)}\n\n"
        s += "\\end{itemize}\n"
        return s

    def abstract(self, arg: Arguments) -> str:
        return (
            "\\begin{abstract}\n\n"
            + self.parse(arg[0], in_block=True)
            + "\\end{abstract}\n\n"
        )

    def date(self, arg: Arguments) -> str:
        pass

    def text(self, arg: Arguments) -> str:
        return self.parse(arg[0]) + "\n\n"

    def m(self, arg: Arguments) -> str:
        return "{\\tt " + self.parse(arg[0]) + "}"


if __name__ == "__main__":
    parser: ArgumentParser = ArgumentParser()

    parser.add_argument("-i", "--input", help="Moire input file", required=True)
    parser.add_argument("-o", "--output", help="output file", required=True)
    parser.add_argument("-f", "--format", help="output format", required=True)

    options: Namespace = parser.parse_args(sys.argv[1:])

    with open(options.input, "r") as input_file:
        converter: Moire = getattr(sys.modules[__name__], options.format)()
        output: str = converter.convert(input_file.read())

    if not output:
        print("Fatal: output was no produced.")
        sys.exit(1)

    with open(options.output, "w+") as output_file:
        output_file.write(output)
        print(f"Converted to {options.output}.")
