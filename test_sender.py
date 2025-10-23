#!/usr/bin/env python3
"""Test script to send UDP packets to the listener."""

import socket
import time

UDP_IP = "127.0.0.1"  # localhost
UDP_PORT = 4593

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

print("Sending test packets to localhost:4593...")

# Test 1: Send lightson
print("Sending: lightson")
sock.sendto(b"lightson", (UDP_IP, UDP_PORT))
time.sleep(2)

# Test 2: Send lightsoff
print("Sending: lightsoff")
sock.sendto(b"lightsoff", (UDP_IP, UDP_PORT))
time.sleep(2)

# Test 3: Send lightson again
print("Sending: lightson")
sock.sendto(b"lightson", (UDP_IP, UDP_PORT))

print("Done!")
sock.close()
