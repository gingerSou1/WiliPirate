"""Run only inside the isolated M1E root; exercise the production socket path."""
from pathlib import Path
import socket
import subprocess
import sys
import threading
import unittest

SOCKET = Path('/run/fwcm0-bridge.sock')
COMMAND = ['qemu-aarch64', '-L', '/usr/aarch64-linux-gnu', sys.argv.pop(1)]


class BridgeTests(unittest.TestCase):
    def exercise(self, failure=None):
        self.assertFalse(SOCKET.exists(), 'Refusing to replace an existing bridge')
        commands, errors = [], []
        released = threading.Event()
        server = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
        server.bind(str(SOCKET))
        server.listen(1)
        server.settimeout(10)

        def serve():
            try:
                with server.accept()[0] as client:
                    client.settimeout(10)
                    header = b''
                    while len(header) < 5:
                        data = client.recv(5 - len(header))
                        if not data:
                            raise AssertionError('Incomplete OP_API header')
                        header += data
                    self.assertEqual(header, b'\x01\0\0\0\x07')
                    incoming = b''
                    buttons = iter([1, 2, 2, 2, 2, 2, 16])
                    while True:
                        data = client.recv(4096)
                        if not data:
                            released.set()
                            break
                        incoming += data
                        while b'\n' in incoming:
                            line, incoming = incoming.split(b'\n', 1)
                            self.assertTrue(line.startswith(b'\x02'))
                            command = line[1:].decode('ascii').strip()
                            if not command:
                                continue
                            commands.append(command)
                            path = command.split()[0]
                            self.assertIn(path, {'h\\a\\g', 'g\\c\\a', 'g\\c\\f',
                                                 'g\\b\\e', 'g\\c\\c', 'g\\c\\e',
                                                 'g\\e\\a', 'g\\t'})
                            payload = ''
                            if path == 'h\\a\\g':
                                payload = 'main 1 0 150000000 none'
                            elif path == 'g\\c\\e':
                                if failure == 'disconnect':
                                    return
                                payload = format(next(buttons), 'X')
                                if failure == 'malformed':
                                    payload = 'nothex'
                                if failure == 'unexpected':
                                    # Valid framing, wrong response command: must not invent input.
                                    path = 'i\\g\\u'
                                    payload = '10'
                                if failure == 'unexpected-mode' and not any(
                                        item.startswith('g\\e\\a') for item in commands):
                                    path = 'i\\g\\u'
                                    payload = '2'
                                elif failure == 'unexpected-mode':
                                    payload = '10'
                            if failure == 'probe' and command == 'h\\a\\g':
                                payload = 'invalid'
                            if failure == 'unexpected-probe' and command == 'h\\a\\g':
                                path = 'g\\c\\e'
                            response = f'[{path} 1 1 {payload} 1]\n'.encode()
                            if failure == 'malformed-frame' and path == 'g\\c\\e':
                                response = b'[g\\c\\e 1]\n'
                            # Fragment replies across transport reads, as the BSP test does.
                            client.sendall(response[:3])
                            client.sendall(response[3:])
            except BaseException as error:
                errors.append(error)

        thread = threading.Thread(target=serve)
        thread.start()
        try:
            result = subprocess.run(COMMAND, capture_output=True, text=True, timeout=20)
        finally:
            thread.join(12)
            server.close()
            SOCKET.unlink()
        self.assertFalse(thread.is_alive())
        if errors:
            raise errors[0]
        return result, commands, released.is_set()

    def test_success_help_all_modes_exit_cleanup(self):
        result, commands, released = self.exercise()
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertIn('HiZ; hardware disabled', result.stdout)
        self.assertIn('g\\e\\a 3 "Modes are logical only."', commands)
        for mode in ('UART', 'I2C', 'SPI', 'GPIO', 'HiZ'):
            self.assertIn(f'g\\e\\a 1 "Mode: {mode}"', commands)
        self.assertIn('g\\b\\e 2 20 105 0 1 white black "Hardware: DISABLED"', commands)
        self.assertEqual(commands[-1], 'g\\t')
        self.assertTrue(released)

    def test_unavailable(self):
        self.assertFalse(SOCKET.exists())
        result = subprocess.run(COMMAND, capture_output=True, text=True, timeout=10)
        self.assertEqual(result.returncode, 1)
        self.assertIn('No fallback', result.stderr)

    def test_disconnect(self):
        result, commands, _ = self.exercise('disconnect')
        self.assertEqual(result.returncode, 1)
        self.assertIn('without retry or fallback', result.stderr)
        self.assertEqual(commands[-1], 'g\\c\\e')

    def test_malformed_button_response(self):
        result, commands, released = self.exercise('malformed')
        self.assertEqual(result.returncode, 1)
        self.assertIn('button read unavailable', result.stderr)
        self.assertEqual(commands[-2:], ['g\\c\\e', 'g\\t'])
        self.assertTrue(released)

    def test_malformed_probe(self):
        result, commands, released = self.exercise('probe')
        self.assertEqual(result.returncode, 1)
        self.assertIn('No fallback', result.stderr)
        self.assertEqual(commands, ['h\\a\\g'])
        self.assertTrue(released)

    def test_unexpected_response_path_fails_closed(self):
        result, commands, released = self.exercise('unexpected')
        self.assertEqual(result.returncode, 1, 'Wrong-path response was accepted as button input')
        self.assertTrue(released)

    def test_wrong_command_cannot_change_logical_mode(self):
        result, commands, released = self.exercise('unexpected-mode')
        self.assertFalse(any(item.startswith('g\\e\\a') for item in commands),
                         'Wrong-command payload caused a UI/model update')
        self.assertEqual(result.returncode, 1)
        self.assertTrue(released)

    def test_wrong_command_probe_cannot_start_ui(self):
        result, commands, released = self.exercise('unexpected-probe')
        self.assertEqual(commands, ['h\\a\\g'], 'Wrong-command probe allowed UI startup')
        self.assertEqual(result.returncode, 1)
        self.assertTrue(released)

    def test_malformed_frame(self):
        result, commands, released = self.exercise('malformed-frame')
        self.assertEqual(result.returncode, 1)
        self.assertEqual(commands[-2:], ['g\\c\\e', 'g\\t'])
        self.assertTrue(released)


if __name__ == '__main__':
    if not Path('/M1E_ISOLATED_ROOT').exists():
        sys.exit('Refusing to test outside the explicitly marked isolated build root')
    unittest.main(verbosity=2)
