"""Logical session state. Changing mode never calls a backend."""
from . import VERSION
from .backends import stub_backends
from .model import Command, Mode, Reply, SAFETY, ViewState

COMMON_HELP = (
    "help                       Show commands for the current mode\n"
    "info                       Show mode and backend status\n"
    "mode                       List logical modes\n"
    "mode hiz|uart|i2c|spi|gpio   Select a logical mode (no hardware setup)\n"
    "exit                       Close this session"
)
OPERATION_HELP = {
    "scan": "scan                       Request I2C scan (STUB only)",
    "read": "read                       Request read (STUB only)",
    "write": "write <arguments...>       Request write (opaque arguments; STUB only)",
    "transfer": "transfer <arguments...>    Request SPI transfer (STUB only)",
}


class Session:
    def __init__(self):
        self._mode = Mode.HIZ
        self._closed = False
        self._backends = stub_backends()

    @property
    def view(self) -> ViewState:
        backend = self._backends.get(self._mode)
        return ViewState(self._mode, backend.label if backend else "UNAVAILABLE",
                         backend.available if backend else False, self._closed)

    def close(self) -> None:
        self._closed = True
        self._mode = Mode.HIZ

    def handle(self, command: Command) -> Reply:
        if self._closed:
            return Reply("Session closed.", False, True)
        name, arguments = command.name, command.arguments
        if not name:
            return Reply("")
        if name == "exit":
            self.close()
            return Reply("WiliPirate closed; no hardware operations performed.", exit_requested=True)
        if name == "info":
            view = self.view
            return Reply(f"WiliPirate {VERSION} | FREE-WILi 2 CM0 application\n"
                         f"Mode: {view.mode.value}\nBackend: {view.backend}\n"
                         f"Hardware: not enabled\n{SAFETY}")
        if name == "help":
            backend = self._backends.get(self._mode)
            operations = backend.operations if backend else ()
            extra = "\n".join(OPERATION_HELP[item] for item in operations)
            return Reply(COMMON_HELP + ("\n" + extra if extra else "") +
                         "\nAll hardware backends are stubs; no bus data is produced.")
        if name == "mode":
            if not arguments:
                return Reply("\n".join(
                    f"{index}. {mode.value}" + (" (current)" if mode == self._mode else "")
                    for index, mode in enumerate(Mode, 1)
                ) + "\nLogical selection only; hardware remains disabled.")
            selected = next((mode for mode in Mode if mode.value.lower() == arguments[0].lower()), None)
            if selected is None:
                return Reply(f"Unknown mode. Use mode to list names; remaining in {self._mode.value}.", False)
            self._mode = selected
            return Reply(f"Mode: {self._mode.value}")
        backend = self._backends.get(self._mode)
        if backend is None:
            return Reply(f"{self._mode.value} backend unavailable; no fallback attempted.", False)
        if name not in backend.operations:
            return Reply(f"Command unavailable in {self._mode.value}. Type help.", False)
        result = backend.request(name, arguments)
        return Reply(result.message, result.available)
