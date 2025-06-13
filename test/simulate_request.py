import socket

# Modify this SIP request as needed
SIP_METHOD = "INVITE"
SIP_REQUEST = (
    f"{SIP_METHOD} sip:bob@example.com SIP/2.0\r\n"
    "Via: SIP/2.0/UDP pc33.example.com;branch=z9hG4bKbranch123\r\n"
    "Max-Forwards: 70\r\n"
    "To: <sip:bob@example.com>\r\n"
    "From: <sip:alice@example.com>;tag=1928301774\r\n"
    "Call-ID: a84b4c76e66710\r\n"
    f"CSeq: 314159 {SIP_METHOD}\r\n"
    "Contact: <sip:alice@pc33.example.com>\r\n"
    "Content-Type: text/plain\r\n"
    "Content-Length: 13\r\n"
    "\r\n"
    "Hello, Bob!\r\n"
)


def send_udp_request(port=7337):
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    addr = ("127.0.0.1", port)
    print(f"Sending to UDP {addr}...")

    while True:
        sock.sendto(SIP_REQUEST.encode(), addr)
        print(f"Sent SIP {SIP_METHOD} to {addr}")
        data, sender_addr = sock.recvfrom(2048)
        print(f"Received from {sender_addr}:\n{data.decode(errors='ignore')}")


if __name__ == "__main__":
    send_udp_request()
