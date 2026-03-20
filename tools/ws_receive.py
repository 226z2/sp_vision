#!/usr/bin/env python3
import argparse
import asyncio
import json
import sys


async def run(uri: str, msg_type: str | None) -> int:
    try:
        import websockets
    except ImportError:
        print("missing dependency: pip install websockets", file=sys.stderr)
        return 2

    async with websockets.connect(uri, max_size=None) as websocket:
        print(f"connected: {uri}", file=sys.stderr)
        async for raw in websocket:
            try:
                data = json.loads(raw)
            except json.JSONDecodeError:
                print(raw)
                continue

            if msg_type and data.get("type") != msg_type:
                continue

            print(
                json.dumps(
                    {
                        "type": data.get("type"),
                        "timestamp_ns": data.get("timestamp_ns"),
                        "source": data.get("source"),
                        "payload": data.get("payload"),
                    },
                    ensure_ascii=False,
                )
            )

    return 0


def main() -> int:
    parser = argparse.ArgumentParser(description="Receive sp_vision pure WebSocket JSON messages")
    parser.add_argument("--uri", default="ws://127.0.0.1:8878", help="WebSocket URI")
    parser.add_argument("--type", dest="msg_type", help="Only print messages of this type")
    args = parser.parse_args()
    return asyncio.run(run(args.uri, args.msg_type))


if __name__ == "__main__":
    raise SystemExit(main())
