import socket

socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
socket.connect(("10.63.41.2",8080))

socket.send(b"Hello")
print(socket.recv(1024))
