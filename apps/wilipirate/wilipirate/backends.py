"""Hardware abstraction and explicit stubs. No transport or real driver exists here."""
from abc import ABC, abstractmethod
from dataclasses import dataclass

from .model import Mode


@dataclass(frozen=True)
class BackendResult:
    available: bool
    message: str


class Backend(ABC):
    """Future adapters must report unavailable capability without any fallback."""
    mode: Mode
    operations: tuple[str, ...] = ()
    label = "STUB"
    available = False

    @abstractmethod
    def request(self, operation: str, arguments: tuple[str, ...]) -> BackendResult:
        """Return a result; never silently choose another transport/backend."""


class StubBackend(Backend):
    def request(self, operation: str, arguments: tuple[str, ...]) -> BackendResult:
        # Arguments remain opaque. No reads, writes, probe, initialization or fake data.
        return BackendResult(False, f"{self.mode.value} hardware backend not enabled.")


class HiZStub(StubBackend):
    mode = Mode.HIZ


class UARTStub(StubBackend):
    mode = Mode.UART
    operations = ("read", "write")


class I2CStub(StubBackend):
    mode = Mode.I2C
    operations = ("scan", "read", "write")


class SPIStub(StubBackend):
    mode = Mode.SPI
    operations = ("transfer",)


class GPIOStub(StubBackend):
    mode = Mode.GPIO
    operations = ("read", "write")


def stub_backends() -> dict[Mode, Backend]:
    """Fixed composition; no dynamic imports, environment switches or discovery."""
    return {
        Mode.HIZ: HiZStub(), Mode.UART: UARTStub(), Mode.I2C: I2CStub(),
        Mode.SPI: SPIStub(), Mode.GPIO: GPIOStub(),
    }
