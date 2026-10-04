"""Desktop-only drawing adapter. Never imports OneWili or opens a transport."""
import tkinter as tk
from .controller import Controller, GRAY, YELLOW, GREEN, BLUE, RED
from .controller import UP, DOWN, LEFT, RIGHT, CENTER, OK, CANCEL, HOME, PAGE
from .panel import frame

BACKGROUND = "#101820"
FOREGROUND = "#e8eef2"


class Preview:
    def __init__(self, root):
        self.root = root
        self.controller = Controller()
        self.finished = False
        root.title("WiliPirate - host panel preview - STUB ONLY")
        root.resizable(False, False)
        root.configure(bg=BACKGROUND)
        tk.Label(root, text="CM0-style preview | Not the official emulator",
                 bg=BACKGROUND, fg=FOREGROUND).pack(pady=6)
        self.canvas = tk.Canvas(root, width=480, height=320, bg=BACKGROUND,
                                highlightthickness=0)
        self.canvas.pack()
        menu = tk.Frame(root, bg=BACKGROUND)
        menu.pack(fill="x")
        for bit, label, color in zip((GRAY, YELLOW, GREEN, BLUE, RED),
                                     ("Help", "Info", "Mode", "Run", "Exit"),
                                     ("#b7bcc4", "#efcd65", "#86d79c", "#8fc5f0", "#ed9999")):
            tk.Button(menu, text=label, width=10, bg=color,
                      command=lambda value=bit: self.press(value)).pack(side="left", padx=2)
        pad = tk.Frame(root, bg=BACKGROUND)
        pad.pack(pady=8)
        for label, bit in (("Up", UP), ("Down", DOWN), ("Older", LEFT), ("Newer", RIGHT),
                           ("OK", OK), ("HiZ", HOME), ("Cancel", CANCEL)):
            tk.Button(pad, text=label, command=lambda value=bit: self.press(value)).pack(side="left", padx=2)
        tk.Label(root, text="F1-F5: menu | arrows: navigate | Enter: run | Esc: exit\n"
                 "Logical HiZ does not establish electrical isolation.",
                 bg=BACKGROUND, fg=FOREGROUND).pack(pady=6)
        for key, bit in (("Up", UP), ("Down", DOWN), ("Left", LEFT), ("Right", RIGHT),
                         ("Return", CENTER), ("Escape", CANCEL), ("Home", HOME), ("Next", PAGE),
                         ("F1", GRAY), ("F2", YELLOW), ("F3", GREEN), ("F4", BLUE),
                         ("F5", RED), ("F6", HOME), ("F7", OK), ("F8", CANCEL), ("F9", PAGE)):
            root.bind("<" + key + ">", lambda event, value=bit: self.press(value))
        root.protocol("WM_DELETE_WINDOW", self.finish)
        self.render()

    def render(self):
        snapshot = frame(self.controller)
        self.canvas.delete("all")
        for cell in snapshot.cells:
            # Use fixed character positions even if host font metrics differ.
            color = "#a6e3b6" if cell.y in (2, 274) else FOREGROUND
            for index, char in enumerate(cell.text):
                self.canvas.create_text(cell.x + index * 7, cell.y, text=char, anchor="nw",
                                        font=("Consolas", -12), fill=color)
        return snapshot

    def press(self, mask):
        self.controller.buttons(mask)
        self.render()
        if self.controller.view.closed:
            self.finish()
        return "break"

    def finish(self):
        if not self.finished:
            self.controller.close()
            self.finished = True
            self.root.destroy()


def main():
    root = tk.Tk()
    preview = Preview(root)
    try:
        root.mainloop()
    finally:
        preview.controller.close()
