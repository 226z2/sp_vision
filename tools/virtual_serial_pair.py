#!/usr/bin/env python3
import os
import pty
import selectors
import signal
import sys
import termios
import tty


def make_raw(fd: int) -> None:
    attrs = termios.tcgetattr(fd)
    tty.setraw(fd)
    attrs[0] = 0
    attrs[1] = 0
    attrs[2] |= termios.CLOCAL | termios.CREAD
    attrs[2] &= ~(termios.PARENB | termios.CSTOPB | termios.CSIZE)
    attrs[2] |= termios.CS8
    attrs[3] = 0
    attrs[6][termios.VMIN] = 1
    attrs[6][termios.VTIME] = 0
    termios.tcsetattr(fd, termios.TCSANOW, attrs)


def main() -> int:
    a_master, a_slave = pty.openpty()
    b_master, b_slave = pty.openpty()

    make_raw(a_master)
    make_raw(a_slave)
    make_raw(b_master)
    make_raw(b_slave)

    a_path = os.ttyname(a_slave)
    b_path = os.ttyname(b_slave)
    print(f"SERIAL_A={a_path}")
    print(f"SERIAL_B={b_path}")
    print("Press Ctrl+C to stop relay.", file=sys.stderr)
    sys.stdout.flush()

    stop = False

    def on_sigint(signum, frame):
        nonlocal stop
        stop = True

    signal.signal(signal.SIGINT, on_sigint)
    signal.signal(signal.SIGTERM, on_sigint)

    selector = selectors.DefaultSelector()
    selector.register(a_master, selectors.EVENT_READ, b_master)
    selector.register(b_master, selectors.EVENT_READ, a_master)

    try:
      while not stop:
        for key, _ in selector.select(timeout=0.2):
            src = key.fileobj
            dst = key.data
            try:
                data = os.read(src, 4096)
            except OSError:
                stop = True
                break
            if not data:
                continue
            os.write(dst, data)
    finally:
        selector.close()
        for fd in (a_master, a_slave, b_master, b_slave):
            try:
                os.close(fd)
            except OSError:
                pass

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
