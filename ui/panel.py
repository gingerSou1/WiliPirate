"""Pure CM0-style text-cell layout. No GUI client, device object or API calls."""
from dataclasses import dataclass
from .controller import COLUMNS, MENU_LABELS, ascii_text

ROWS, CHUNK = 17, 15
CELL_WIDTH, LINE_HEIGHT = 7, 17


@dataclass(frozen=True)
class Cell:
    index: int
    x: int
    y: int
    text: str
    caption: str


@dataclass(frozen=True)
class Frame:
    cells: tuple[Cell, ...]
    menu: tuple[str, ...]
    closed: bool


def frame(controller):
    """Produce 68 text cells fitting the documented htop panel text contract."""
    view = controller.view
    lines = ["WiliPirate | HOST PREVIEW", "STUB ONLY - no hardware access", "-" * COLUMNS]
    lines.extend(controller.visible)
    lines.extend([""] * (13 - len(lines)))
    lines += ["-" * COLUMNS,
              "Session closed" if view.closed else view.prompt + controller.selected + " _",
              "Up/Down select | OK run | Left/Right scroll",
              f"{view.mode.value} | {view.backend} | HW OFF | scroll {controller.offset}"]
    cells = []
    for row, line in enumerate(lines):
        text = ascii_text(line)[:COLUMNS].ljust(COLUMNS)
        for column in range(COLUMNS // CHUNK):
            part = text[column * CHUNK:(column + 1) * CHUNK]
            # Model the official panel caption escaping without sending a command.
            caption = part.replace("#", "##").replace("`", "``")
            cells.append(Cell(len(cells), 30 + column * CHUNK * CELL_WIDTH,
                              2 + row * LINE_HEIGHT, part, caption))
    return Frame(tuple(cells), MENU_LABELS, view.closed)
