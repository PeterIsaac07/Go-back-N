import time, socket, sys, pickle


def decimalToBinary(n):
    n = n.replace("0b", "")
    while len(n)<8:
        n = "0" + n
    return n






def binarycode(s):
    a_byte_array = bytearray(s, "utf8")

    byte_list = []

    for byte in a_byte_array:
        binary_representation = bin(byte)
        byte_list.append(decimalToBinary(binary_representation))


    #print(byte_list)
    a = ""
    for i in byte_list:
        a = a + i
    return a

s = socket.socket()
host = '127.0.0.1'
port = 1122
s.bind((host, port))

s.listen(1)
conn, addr = s.accept()

while True:
    # message = input(str("enter message :"))
    message = 'b'
    #conn.send(message.encode())
    message = binarycode(message)
    print(message.encode())
    messagelen = str(len(message))
    conn.send(messagelen.encode())

    packet_to_send = 0
    window_size = int(input("Enter the window size -> "))

    buffer = ""

    window_size = window_size - 1
    messagelen = int(messagelen)
    end_of_window = window_size
    while packet_to_send != messagelen:
        while (packet_to_send != (messagelen/4 - window_size)):
            conn.send(message[packet_to_send*4:(packet_to_send*4)+4].encode())
            print('Sending packet number {}, which is {}'.format(packet_to_send, message[packet_to_send*4:(packet_to_send*4)+4]))
            buffer = conn.recv(1024)
            buffer = buffer.decode()
            if (buffer != "ACK Lost"):

                print('{} is received, sliding window is in the range of {} to {} '.format(buffer,
                                                                                           str(packet_to_send + 1),
                                                                                           str(end_of_window + 1)))
                packet_to_send = packet_to_send + 1
                end_of_window = end_of_window + 1

            else:

                print('Ack is lost, sliding window is in the range of {} to {} '.format(str(packet_to_send + 1),
                                                                                        str(end_of_window + 1)))

        while (packet_to_send != messagelen):

            conn.send(message[packet_to_send].encode())
            print('Sending packet number {}, which is {}'.format(packet_to_send, message[packet_to_send]))
            buffer = conn.recv(1024)
            buffer = buffer.decode()
            if (buffer != "ACK Lost"):

                print('{} is received, sliding window is in the range of {} to {} '.format(buffer,
                                                                                           str(packet_to_send + 1),
                                                                                           str(end_of_window + 1)))
                packet_to_send = packet_to_send + 1

            else:

                print('Ack is lost, sliding window is in the range of {} to {} '.format(str(packet_to_send + 1),
                                                                                        str(end_of_window + 1)))

