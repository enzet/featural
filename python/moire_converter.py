from moire.default import DefaultTeX
from pathlib import Path
import subprocess
import sys


class LanguageTeX(DefaultTeX):
    def ipa(self, arg) -> str:
        return f"{{\\doulos{{{self.parse(arg[0])}}}}}"

    def symbol(self, arg) -> str:
        proc = subprocess.Popen(
            ["build/language", "symbol"] + self.clear(arg[0]).split(" "),
            stdout=subprocess.PIPE,
        )
        return proc.stdout.read().decode()

    def symbol_table(self, arg) -> str:
        rows = ",".join([self.clear(x) for x in arg[0]])
        columns = ",".join([self.clear(x) for x in arg[1]])
        proc = subprocess.Popen(
            ["build/language", "table", rows, columns],
            stdout=subprocess.PIPE,
        )
        return proc.stdout.read().decode()
        

if __name__ == "__main__":
    with Path(sys.argv[1]).open() as input_file:
        with Path(sys.argv[2]).open("w") as output_file:
            output_file.write(
                LanguageTeX().convert(input_file.read(), wrap=False)
            )
