"""Run only the host panel preview; no hardware or network connection options."""
from pathlib import Path
import sys

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT))
sys.path.insert(0, str(ROOT / "apps/wilipirate"))


def main():
    if len(sys.argv) != 1:
        raise SystemExit("Usage: python -B tools/ui_preview.py (host-only preview)")
    from ui.tk_preview import main as preview
    preview()


if __name__ == "__main__":
    main()
