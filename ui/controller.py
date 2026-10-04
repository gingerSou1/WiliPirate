"""UI selection/history over the unchanged application; synthetic buttons only."""
from wilipirate.application import Application
from wilipirate.model import Mode, MAX_COMMAND_LENGTH

COLUMNS = 60
HISTORY_LIMIT = 256
VISIBLE_LINES = 10
MENU_LABELS = ("Help", "Info", "Mode", "Run", "Exit")
# Official read_buttons bit positions, not GUI event IDs.
GRAY, YELLOW, GREEN, BLUE, RED = (1 << n for n in range(5))
UP, DOWN, LEFT, RIGHT, CENTER, OK, CANCEL, HOME, PAGE = (1 << n for n in range(5, 14))
COMMANDS = ("help", "info", "mode") + tuple("mode " + m.value.lower() for m in Mode) + (
    "scan", "read", "write AA", "transfer FF", "exit",
)


def ascii_text(text):
    return "".join(c if " " <= c <= "~" else "?" for c in text)


class Controller:
    def __init__(self):
        self.application = Application()
        self.selection = 0
        self.offset = 0
        self._history = []
        self._append("Host panel preview | hardware backends disabled")
        self._append("HiZ is logical only; physical pin state is unknown.")
        self.submit("help")

    @property
    def view(self):
        return self.application.view

    @property
    def selected(self):
        return COMMANDS[self.selection]

    @property
    def history(self):
        return tuple(self._history)

    @property
    def visible(self):
        end = len(self._history) - self.offset
        return tuple(self._history[max(0, end - VISIBLE_LINES):end])

    def _append(self, text):
        for line in text.splitlines() or [""]:
            line = ascii_text(line)
            self._history.extend(line[i:i + COLUMNS] for i in range(0, len(line), COLUMNS))
            if not line:
                self._history.append("")
        del self._history[:-HISTORY_LIMIT]
        self.offset = 0

    def submit(self, command):
        if self.view.closed:
            return None
        # Parser remains authoritative; bound the UI echo for rejected long input.
        self._append(self.view.prompt + command[:MAX_COMMAND_LENGTH])
        reply = self.application.submit(command)
        if reply.text:
            self._append(reply.text)
        return reply

    def scroll(self, delta):
        self.offset = max(0, min(self.offset + delta, max(0, len(self._history) - VISIBLE_LINES)))

    def buttons(self, mask):
        """One synthetic latch read. Cancel wins; ambiguous multi-press is ignored."""
        if self.view.closed or type(mask) is not int or mask < 0 or mask >= (1 << 14):
            return
        if mask & (RED | CANCEL):
            self.submit("exit")
        elif mask == 0 or mask & (mask - 1):
            return
        elif mask == GRAY:
            self.submit("help")
        elif mask == YELLOW:
            self.submit("info")
        elif mask == GREEN:
            modes = tuple(Mode)
            next_mode = modes[(modes.index(self.view.mode) + 1) % len(modes)]
            self.submit("mode " + next_mode.value.lower())
        elif mask in (BLUE, CENTER, OK):
            self.submit(self.selected)
        elif mask == UP:
            self.selection = (self.selection - 1) % len(COMMANDS)
        elif mask == DOWN:
            self.selection = (self.selection + 1) % len(COMMANDS)
        elif mask == LEFT:
            self.scroll(VISIBLE_LINES)
        elif mask in (RIGHT, PAGE):
            self.scroll(-VISIBLE_LINES)
        elif mask == HOME:
            self.submit("mode hiz")

    def close(self):
        if not self.view.closed:
            self.submit("exit")
