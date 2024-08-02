from moire.default import Default, DefaultTeX, DefaultMarkdown
from moire.main import main
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
            \\newfontface\\ko{Noto Sans KR}
            \\usetikzlibrary{positioning}
            \\begin{document}
            """
        )
        s += self.parse(arg[0], in_block=True)
        s += "\\end {document}"
        return s

    def figure(self, arg) -> str:
        s = "\\begin{figure}[h]\n"
        s += "\\centering\n"
        s += self.parse(arg[1])
        s += f"\\caption{{{self.parse(arg[0])}}}"
        s += "\\end{figure}"
        return s

    def table(self, arg) -> str:
        s = dedent(
            """\
            \\begin{table}[h]
            \\begin{center}
            \\begin{tabular}{|"""
        )
        max_tds = 0
        for tr in arg[1:]:
            if isinstance(tr, list):
                tds = sum([1 for td in tr if isinstance(td, list)])
                max_tds = max(max_tds, tds)
        for k in range(max_tds):
            s += "c|"
        s += "}\n\\hline\n"
        for tr in arg[1:]:
            if isinstance(tr, list):
                tds = []
                for td in tr:
                    if isinstance(td, list):
                        tds.append(td)
                for td in tds[:-1]:
                    s += self.parse(td) + " & "
                s += self.parse(tds[-1])
                s += " \\\\\n\\hline\n"
        s += dedent(
            f"""\
            \\end{{tabular}}
            \\caption{{{self.parse(arg[0])}}}
            \\end{{center}}
            \\end{{table}}"""
        )
        return s

    def ipa(self, arg) -> str:
        return f"{{\\doulos{{{self.parse(arg[0])}}}}}"

    def ko(self, arg) -> str:
        return f"{{\\ko{{{self.parse(arg[0])}}}}}"

    def symbol(self, arg) -> str:
        proc = subprocess.Popen(
            ["build/language", "symbol"] + self.clear(arg[0]).split(" "),
            stdout=subprocess.PIPE,
        )
        return proc.stdout.read().decode()

    def symbol_table(self, arg) -> str:
        rows: str = ",".join([x[0] for x in arg[0] if isinstance(x, list)])
        columns: str = ",".join([x[0] for x in arg[1] if isinstance(x, list)])
        proc = subprocess.Popen(
            ["build/language", "table", rows, columns],
            stdout=subprocess.PIPE,
        )
        return proc.stdout.read().decode()


class LanguageMarkdown(Language, DefaultMarkdown):
    def author(self, arg) -> str:
        return ""

    def abstract(self, arg) -> str:
        return self.parse(arg[0])

    def ipa(self, arg) -> str:
        return f"{{\\doulos{{{self.parse(arg[0])}}}}}"

    def symbol(self, arg) -> str:
        proc = subprocess.Popen(
            ["build/language", "symbol"] + self.clear(arg[0]).split(" "),
            stdout=subprocess.PIPE,
        )
        return proc.stdout.read().decode()

    def symbol_table(self, arg) -> str:
        rows: str = ",".join([x[0] for x in arg[0]])
        columns: str = ",".join([x[0] for x in arg[1]])
        proc = subprocess.Popen(
            ["build/language", "table", rows, columns],
            stdout=subprocess.PIPE,
        )
        return proc.stdout.read().decode()


if __name__ == "__main__":
    main(sys.argv[1:], Language)
