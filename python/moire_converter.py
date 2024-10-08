from moire.default import Default, DefaultTeX
from moire.main import main
from textwrap import dedent
import subprocess
import sys


SYMBOL_GENERATOR_EXECUTABLE: str = "build/language"


class Language(Default):
    def figure(self, arg) -> str:
        raise NotImplementedError()

    def tex_ref(self, arg) -> str:
        raise NotImplementedError()

    def table(self, arg) -> str:
        raise NotImplementedError()

    def ipa(self, arg) -> str:
        """IPA symbol."""
        raise NotImplementedError()

    def ko(self, arg) -> str:
        """Korean text symbol."""
        raise NotImplementedError()

    def ru(self, arg) -> str:
        """Russian text symbol."""
        raise NotImplementedError()

    def symbol(self, arg) -> str:
        raise NotImplementedError()

    def symbol_table(self, arg) -> str:
        raise NotImplementedError()


class LanguageTeX(Language, DefaultTeX):
    def body(self, arg) -> str:
        s = dedent(
            """\
            \\documentclass{article}
            \\usepackage{tikz}
            \\usepackage{fontspec}
            \\usepackage[left=1.25in,right=1.25in,top=1in,bottom=1in]{geometry}
            \\newfontface\\doulos{Doulos SIL}
            \\newfontface\\ko{Noto Sans KR Regular}
            \\newfontface\\ru{CMU Serif Roman}
            \\def\\arraystretch{1.3}
            \\usetikzlibrary{positioning}
            \\begin{document}
            """
        )
        s += self.parse(arg[0], in_block=True)
        s += dedent(
            """
            \\listoffigures
            \\listoftables
            \\end {document}
            """
        )
        return s

    def tikz(self, arg) -> str:
        return (
            "\\begin{tikzpicture}" + self.parse(arg[0]) + "\\end{tikzpicture}"
        )

    def figure(self, arg) -> str:
        s = "\\begin{figure}[ht]\n"
        s += "\\centering\n"
        s += self.parse(arg[2])
        s += f"\\caption{{{self.parse(arg[1])}}}"
        s += f"\\label{{{arg[0][0]}}}"
        s += "\\end{figure}"
        return s

    def figure_wide(self, arg) -> str:
        s = "\\begin{figure}[ht]\n"
        s += "\\noindent\\makebox[\\textwidth]{%\n"
        s += self.parse(arg[2])
        s += "}\n"
        s += f"\\caption{{{self.parse(arg[1])}}}"
        s += f"\\label{{{arg[0][0]}}}"
        s += "\\end{figure}"
        return s

    def tex_ref(self, arg) -> str:
        return f"\\ref{{{arg[0][0]}}}"

    def table(self, arg) -> str:
        max_tds: int = 0
        for tr in arg[2:]:
            if isinstance(tr, list):
                tds = sum([1 for td in tr if isinstance(td, list)])
                max_tds = max(max_tds, tds)

        s = dedent(
            """\
            \\begin{table}[ht]
            \\begin{center}
            \\begin{tabular}{|"""
            + "|".join(["l"] * max_tds)
            + "|}\n"
        )
        s += "\\hline\n"
        for index, tr in enumerate(arg[2:]):
            if isinstance(tr, list):
                tds = []
                for td in tr:
                    if isinstance(td, list):
                        tds.append(td)
                for td in tds[:-1]:
                    s += self.parse(td) + " & "
                s += self.parse(tds[-1])
                s += " \\\\\n"
                s += "\\hline\n"
        s += dedent(
            f"""\
            \\end{{tabular}}
            \\caption{{{self.parse(arg[1])}}}
            \\label{{{arg[0][0]}}}
            \\end{{center}}
            \\end{{table}}"""
        )
        return s

    def ipa(self, arg) -> str:
        return f"{{\\doulos{{{self.parse(arg[0])}}}}}"

    def ko(self, arg) -> str:
        return f"{{\\ko{{{self.parse(arg[0])}}}}}"

    def ru(self, arg) -> str:
        return f"{{\\ru{{{self.parse(arg[0])}}}}}"

    def tikz_symbol(self, arg) -> str:
        proc: subprocess.Popen = subprocess.Popen(
            [SYMBOL_GENERATOR_EXECUTABLE, "symbol"]
            + self.clear(arg[0]).split(" "),
            stdout=subprocess.PIPE,
        )
        return proc.stdout.read().decode()

    def symbol(self, arg) -> str:
        return "\\tikz{" + self.tikz_symbol(arg) + "}"

    def symbol_table(self, arg) -> str:
        rows: str = ",".join([x[0] for x in arg[0] if isinstance(x, list)])
        columns: str = ",".join([x[0] for x in arg[1] if isinstance(x, list)])
        filter_: str = ""
        if len(arg) > 2:
            filter_ = arg[2][0].strip().replace("\n", ",").replace(" ", ",")
        proc: subprocess.Popen = subprocess.Popen(
            [SYMBOL_GENERATOR_EXECUTABLE, "table", rows, columns, filter_],
            stdout=subprocess.PIPE,
        )
        return proc.stdout.read().decode()


if __name__ == "__main__":
    main(sys.argv[1:], Language)
