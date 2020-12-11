
import time, socket, sys ,pickle


s = socket.socket()
host = '127.0.0.1'
port = 1122
s.bind((host, port))

s.listen(1)
conn, addr = s.accept()



while True:
    # message = input(str("enter message :"))
    message = 'thomas joseph kamil yousef artin'
    conn.send(message.encode())
    print(message.encode())
    messagelen = str(len(message))
    conn.send(messagelen.encode())

    packet_to_send = 0
    window_size = int(input("enter size of window: "))

    buffer = ""

    window_size = window_size - 1
    messagelen = int(messagelen)
    end_of_window = window_size
    while packet_to_send != messagelen:
        while (packet_to_send != (messagelen - window_size)):
            conn.send(message[packet_to_send].encode())
            print('Sending packet number {}, which is {}'.format(packet_to_send,message[packet_to_send]))
            buffer = conn.recv(1024)
            buffer = buffer.decode()
            if (buffer != "ACK Lost"):

                print('{} is received, sliding window is in the range of {} to {} '.format(buffer, str(packet_to_send + 1)
                                                                                           ,str(end_of_window + 1)))
                packet_to_send = packet_to_send + 1
                end_of_window = end_of_window + 1

            else:

                print('Ack is lost, sliding window is in the range of {} to {} '.format(str(packet_to_send + 1)
                                                                                        ,str(end_of_window + 1)))

        while (packet_to_send != messagelen):

            conn.send(message[packet_to_send].encode())
            print('Sending packet number {}, which is {}'.format(packet_to_send, message[packet_to_send]))
            buffer = conn.recv(1024)
            buffer = buffer.decode()
            if (buffer != "ACK Lost"):

                print('{} is received, sliding window is in the range of {} to {} '.format(buffer, str(packet_to_send + 1)
                                                                                           ,str(end_of_window + 1)))
                packet_to_send = packet_to_send + 1

            else:

                print('Ack is lost, sliding window is in the range of {} to {} '.format(str(packet_to_send + 1)
                                                                                        ,str(end_of_window + 1)))