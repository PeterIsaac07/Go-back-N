import time, socket, sys, pickle
import random


s = socket.socket()
host = '127.0.0.1'
port = 1122

s.connect((host, port))
print("Connected...\n")


while True:

    #intial_mess= s.recv(1024)
    #intial_mess = intial_mess.decode()
    intial_mess_len= s.recv(1024)
    intial_mess_len = intial_mess_len.decode()
    intial_mess_len = int(intial_mess_len)
    i = 0
    a = ""
    Ack = ""
    #f = random.randint(0, 1)
    message = ""
    while i != intial_mess_len/4:

        status_ack = random.randint(0, 1)
        status_ack=1
        if (status_ack == 0):
            Ack = "ACK Lost"
            message = s.recv(1024)
            message = message.decode()
            s.send(Ack.encode())

        elif (status_ack == 1):
            Ack = "ACK " + str(i)
            message = s.recv(1024)
            message = message.decode()

            s.send(Ack.encode())
            a = a + message
            i = i + 1
    #n = int(a, 2)
    #decoded = n.to_bytes((n.bit_length() + 7) // 8, 'big').decode()
    #print(decoded)
    print(a)
    #print("The message received is :", m)
