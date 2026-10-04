"""Stage the stub-only M1B app locally; never install packages or contact a device."""
import argparse
from pathlib import Path
import shutil

ROOT = Path(__file__).resolve().parents[1]


def stage(output: Path) -> Path:
    destination = output.resolve() / "wilipirate"
    files = {
        "app.py": ROOT / "apps/wilipirate/app.py",
        "run.sh": ROOT / "apps/wilipirate/run.sh",
        "README.md": ROOT / "apps/wilipirate/README.md",
        "THIRD_PARTY.md": ROOT / "docs/THIRD_PARTY.md",
    }
    for module in ("__init__", "model", "parser", "backends", "state", "application", "console"):
        name = f"wilipirate/{module}.py"
        files[name] = ROOT / "apps/wilipirate" / name
    for source in files.values():
        if not source.is_file():
            raise FileNotFoundError(source)
    # Exclusive mkdir also refuses a dangling symlink. Never merge/overwrite.
    destination.mkdir(parents=True, exist_ok=False)
    for name, source in files.items():
        target = destination / name
        target.parent.mkdir(parents=True, exist_ok=True)
        shutil.copyfile(source, target)
    (destination / "run.sh").chmod(0o755)
    return destination


def main(argv=None) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--output", type=Path, default=ROOT / "dist/apps",
                        help="local staging parent (default: dist/apps)")
    args = parser.parse_args(argv)
    try:
        destination = stage(args.output)
    except OSError as error:
        parser.exit(1, f"Staging failed; no existing destination is overwritten: {error}\n")
    print(f"Staged {destination}. No device accessed; deployment requires approval.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
