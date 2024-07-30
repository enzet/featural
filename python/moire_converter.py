from moire.default import Default, DefaultTeX
from moire.main import main
from pathlib import Path
from textwrap import dedent
import subprocess
import sys


class Language(Default):
    pass


class LanguageTeX(Language, DefaultTeX):
    def body(self, arg) -> str:
        s = dedent(
            """\
            \\documentclass{article}
            \\usepackage[utf8]{inputenc}
            \\usepackage{tikz}
            \\usepackage{fontspec}
            \\usepackage[left=1.25in,right=1.25in,top=1in,bottom=1in]{geometry}
            \\setmainfont[Ligatures=TeX]{CMU Serif}
            \\newfontface\\doulos{Doulos SIL}
            \\usetikzlibrary{positioning}
            \\begin{document}
            """
        )
        s += self.parse(arg[0], in_block=True)
        s += "\\end {document}"
        return s

    def ipa(self, arg) -> str:
        return f"{{\\doulos{{{self.parse(arg[0])}}}}}"

    def symbol(self, arg) -> str:
        proc = subprocess.Popen(
            ["build/language", "symbol"] + self.clear(arg[0]).split(" "),
            stdout=subprocess.PIPE,
        )
        return proc.stdout.read().decode()

    def symbol_table(self, arg) -> str:
        rows: str = ",".join([self.clear(x) for x in arg[0]])
        columns: str = ",".join([self.clear(x) for x in arg[1]])
        proc = subprocess.Popen(
            ["build/language", "table", rows, columns],
            stdout=subprocess.PIPE,
        )
        return proc.stdout.read().decode()
        

if __name__ == "__main__":
    main(sys.argv[1:], Language)
