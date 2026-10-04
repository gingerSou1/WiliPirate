"""Values shared across the parser, state manager and user interfaces."""
from dataclasses import dataclass
from enum import Enum

MAX_COMMAND_LENGTH = 256
SAFETY = (
    "Logical modes only; no hardware operations. External pin/power state is unknown; "
    "electrical HiZ is not enforced."
)


class Mode(Enum):
    HIZ = "HiZ"
    UART = "UART"
    I2C = "I2C"
    SPI = "SPI"
    GPIO = "GPIO"


@dataclass(frozen=True)
class Command:
    name: str
    arguments: tuple[str, ...] = ()


@dataclass(frozen=True)
class Reply:
    text: str
    success: bool = True
    exit_requested: bool = False


@dataclass(frozen=True)
class ViewState:
    mode: Mode
    backend: str
    available: bool
    closed: bool

    @property
    def prompt(self) -> str:
        return f"{self.mode.value}> "
