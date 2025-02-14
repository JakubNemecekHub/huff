# Copy include folders from my header only libraries
from pathlib import Path


def include(lib: str, target: str = "lib") -> None:
    library_folder = Path(lib)
    library_name = library_folder.stem
    include_folder = library_folder.joinpath("include")
    relative_target_folder = Path(target).joinpath(library_name)
    relative_target_folder.mkdir(exist_ok=True)

    for item in include_folder.iterdir():
        relative_target_file = relative_target_folder.joinpath(item.name)
        relative_target_file.write_text(item.read_text())


if __name__ == "__main__":
    include(r"C:\Workspace\cpp\mb")
    include(r"C:\Workspace\cpp\ts", r"test\lib")
