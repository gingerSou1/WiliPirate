"""Strict single-command parser. Arguments are data, never executable code."""
from .model import Command, MAX_COMMAND_LENGTH


class ParseError(ValueError):
    pass


# Argument counts only. Hardware payload encoding is deliberately not implemented.
ARITY = {
    "help": (0, 0), "info": (0, 0), "mode": (0, 1), "exit": (0, 0),
    "scan": (0, 0), "read": (0, 0), "write": (1, None), "transfer": (1, None),
}


def parse(line: str) -> Command:
    if len(line) > MAX_COMMAND_LENGTH:
        raise ParseError("Command too long (maximum 256 characters).")
    if any(not character.isprintable() and character != "\t" for character in line):
        raise ParseError("Control characters are not allowed in commands.")
    if any(character in line for character in ";|`"):
        raise ParseError("Command chaining and shell syntax are not supported.")
    words = line.split()
    if not words:
        return Command("")
    name, arguments = words[0].lower(), tuple(words[1:])
    if name not in ARITY:
        raise ParseError("Unknown command. Type help.")
    minimum, maximum = ARITY[name]
    if len(arguments) < minimum or (maximum is not None and len(arguments) > maximum):
        raise ParseError(f"Invalid arguments for {name}. Type help for usage.")
    return Command(name, arguments)
