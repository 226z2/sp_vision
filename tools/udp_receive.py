#!/usr/bin/env python3
import argparse
import json
import socket


def main() -> int:
    parser = argparse.ArgumentParser(description="Receive UDP JSON packets from spv_supervisor")
    parser.add_argument("--host", default="0.0.0.0", help="Bind host")
    parser.add_argument("--port", type=int, default=18878, help="UDP port")
    parser.add_argument("--type", dest="msg_type", help="Only print packets of this type")
    args = parser.parse_args()

    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.bind((args.host, args.port))
    print(f"listening: udp://{args.host}:{args.port}")

    while True:
        payload, peer = sock.recvfrom(65535)
        try:
            message = json.loads(payload.decode("utf-8"))
        except Exception:
            print(f"{peer[0]}:{peer[1]} {payload!r}")
            continue

        if args.msg_type and message.get("type") != args.msg_type:
            continue

        print(
            json.dumps(
                {
                    "peer": f"{peer[0]}:{peer[1]}",
                    "type": message.get("type"),
                    "service": message.get("service"),
                    "target": message.get("target"),
                    "reason_code": message.get("reason_code"),
                    "reason": message.get("reason"),
                    "timestamp_ns": message.get("timestamp_ns"),
                    "details": message.get("details"),
                },
                ensure_ascii=False,
            )
        )


if __name__ == "__main__":
    raise SystemExit(main())
