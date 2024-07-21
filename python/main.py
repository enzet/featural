from pathlib import Path
from collections import defaultdict


def parse(path: Path) -> None:
    with path.open() as input_file:
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

        print("Phonemes:", len(all_phonemes))
        print("Allophones:", len(all_allophones))

        print("Languages with phoneme:")
        print(
            [
                (a, f"{len(b) / len(languages) * 100:.2f}%")
                for a, b in sorted(
                    list(phoneme_languages.items()), key=lambda x: -len(x[1])
                )[:10]
            ]
        )

        print("Languages with allophone:")
        print(
            [
                (a, f"{len(b) / len(languages) * 100:.2f}%")
                for a, b in sorted(
                    list(allophone_languages.items()), key=lambda x: -len(x[1])
                )[:10]
            ]
        )


if __name__ == "__main__":
    parse(Path("data/phoible.csv"))
