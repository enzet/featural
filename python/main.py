from pathlib import Path
from collections import defaultdict


def parse(input_path: Path, output_path: Path) -> None:
    """Parse PHOIBLE data and write phoneme frequency.

    Input file is a PHOIBLE CSV file with usage of phonemes and allophones in
    different languages. Output file is a space-separated data with phoneme, its
    frequency as a phoneme, and its frequency as an allophone.
    """
    with input_path.open() as input_file:
        line: str = input_file.readline()
        all_phonemes: set[str] = set()
        all_allophones: set[str] = set()
        languages: set[str] = set()

        phoneme_languages: dict[str, set[str]] = defaultdict(set)
        allophone_languages: dict[str, set[str]] = defaultdict(set)

        while line := input_file.readline():
            fields: list[str] = []
            in_quotes: bool = False
            field: str = ""

            for c in line:
                if c == '"':
                    in_quotes = not in_quotes
                elif c == "," and not in_quotes:
                    fields.append(field)
                    field = ""
                else:
                    field += c

            language: str = f"{fields[0]}_{fields[1]}_{fields[2]}_{fields[3]}"
            phoneme: str = fields[6]
            allophones: list[str] = (
                [phoneme] if fields[7] == "NA" else fields[7].split(" ")
            )
            if allophones == "NA":
                allophones = phoneme

            languages.add(language)

            phoneme_languages[phoneme].add(language)
            all_phonemes.add(phoneme)

            for allophone in allophones:
                allophone_languages[allophone].add(language)
                all_allophones.add(allophone)

    sorted_phonemes: list[str] = sorted(
        phoneme_languages.keys(), key=lambda x: -len(phoneme_languages[x])
    )
    len_languages: int = len(languages)

    with output_path.open("w") as output_file:
        for phoneme in sorted_phonemes:
            percentage_phonemes: float = (
                len(phoneme_languages[phoneme]) / len_languages
            )

            percentage_allophones: float = (
                len(allophone_languages[phoneme]) / len_languages
            )
            output_file.write(
                f"{phoneme} "
                f"{percentage_phonemes:.6f} "
                f"{percentage_allophones:.6f}\n",
            )


if __name__ == "__main__":
    parse(Path("data/phoible.csv"), Path("out/phoneme_frequency.txt"))
