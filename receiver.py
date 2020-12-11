import time, socket, sys, pickle
import random


s = socket.socket()
host = '127.0.0.1'
port = 1122

s.connect((host, port))
print("Connected...\n")


while True:

    m = s.recv(1024)
    m = m.decode()
    k = s.recv(1024)
    k = k.decode()
    k = int(k)
    i = 0
    a = ""
    Ack = ""
    f = random.randint(0, 1)
    message = ""
    while i != k:

        f = random.randint(0, 1)
        f = 1
        if (f == 0):
            Ack = "ACK Lost"
            message = s.recv(1024)
            message = message.decode()
            s.send(Ack.encode())

        elif (f == 1):
            Ack = "ACK " + str(i)
            message = s.recv(1024)
            message = message.decode()

            s.send(Ack.encode())
            a = a + message
            i = i + 1
    n = int(a, 2)
    decoded = n.to_bytes((n.bit_length() + 7) // 8, 'big').decode()
    print(decoded)
    print("The message received is :", m)