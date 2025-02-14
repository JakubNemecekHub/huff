from pathlib import Path


def combine_tests(headers: list[str], outfile: str) -> None:
    with open(outfile, "w", encoding="utf-8") as output:
        for header in headers:
            with open(header, "r", encoding="utf-8") as input:
                output.write(input.read())
                output.write("\n")


def locate_tests(location: str = "..\\src") -> list[str]:
    path = Path(location)
    res = []
    for file in path.iterdir():
        if file.is_file() and "test.hpp" in file.name:
            res.append(file.absolute())
    return res


if __name__ == "__main__":
    headers: list[str] = locate_tests(".\\test\\src")
    outfile: str = ".\\test\\src\\_tests.hpp"
    combine_tests(headers, outfile)
