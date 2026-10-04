"""UI-independent command entry point: parse, then dispatch to logical state."""
from .model import Reply
from .parser import ParseError, parse
from .state import Session


class Application:
    def __init__(self):
        self._session = Session()

    @property
    def view(self):
        return self._session.view

    def submit(self, line: str) -> Reply:
        if self.view.closed:
            return Reply("Session closed.", False, True)
        try:
            command = parse(line)
        except ParseError as error:
            return Reply(str(error), False)
        return self._session.handle(command)

    def close(self) -> None:
        self._session.close()
