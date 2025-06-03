import socket

# SIP 200 OK response (simple)
RESPONSE = (
    "SIP/2.0 200 OK\r\n"
    "Via: SIP/2.0/UDP pc33.example.com;branch=z9hG4bKbranch123\r\n"
    "To: <sip:bob@example.com>;tag=123456\r\n"
    "From: <sip:alice@example.com>;tag=1928301774\r\n"
    "Call-ID: a84b4c76e66710\r\n"
    "CSeq: 314159 MESSAGE\r\n"
    "Content-Length: 0\r\n"
    "\r\n"
)


def start_udp_server(port=8888):
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.bind(("0.0.0.0", port))
    print(f"Listening on UDP port {port}...")

    while True:
        data, addr = sock.recvfrom(2048)
        print(f"Received from {addr}:\n{data}")
        sock.sendto(RESPONSE.encode(), addr)
        print(f"Sent SIP 200 OK to {addr}")


if __name__ == "__main__":
    start_udp_server()
