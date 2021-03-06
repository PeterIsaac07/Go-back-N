// Server side C/C++ program to demonstrate Socket programming
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <iostream>
#include "header-1.h"
#define MAX_SEQ 7
#define PORT 8080
#define sending_data 0
#define recieving_Data 1
#include <string>
#include <fcntl.h>

int current_state = sending_data;

packet first = {"hey"};
packet second = {"there"};
packet third = {"iam"};
packet fourth = {"a"};
packet fifth = {"packet"};
packet sixth = {"to"};
packet seventh = {"check"};


packet array_of_packets[7] = {first,second,third,fourth,fifth,sixth,seventh};
frame physical_buffer;




void stop_timer(seq_nr k);


void from_network_layer(packet* p){

static int packet_counter = 0;
*p = array_of_packets[packet_counter];
packet_counter =  (packet_counter + 1) % 7  ;

}

void to_physical_layer(frame* s, int socket){

    send(socket , s->info.data , 50 , MSG_DONTWAIT );
    send(socket , &(s->seq), 50 , MSG_DONTWAIT );
    send(socket , &(s->ack) , 50 , MSG_DONTWAIT);

}



void start_timer(seq_nr k){}





void from_physical_layer(frame* r,int socket){

      read( socket , r->info.data, sizeof(r->info.data));
      read( socket , &(r->seq), sizeof((r->seq)));
      read( socket , &(r->ack), sizeof(r->ack));
}




void wait_for_event(event_type* event){
    if (current_state == sending_data ){
        *event=network_layer_ready;
    }
    if (current_state == recieving_Data ){
        *event=frame_arrival;
    }
    if (current_state == 3 ){
        *event=cksum_err;
    }


}




static boolean between(seq_nr a, seq_nr b, seq_nr c)
{
/* Return true if a <= b < c circularly; false otherwise. */
    if (((a <= b) && (b < c)) || ((c < a) && (a <= b)) || ((b < c) && (c < a)))
        return(True);
    else
        return(False);
}

void enable_network_layer(void){
        current_state = sending_data;
}

void to_network_layer(packet* p){}
void stop_timer(seq_nr k){}
void disable_network_layer(void){
        current_state = 3;

}

static void send_data(seq_nr frame_nr, seq_nr frame_expected, packet buffer[],int socket)
{
    /* Construct and send a data frame. */
    frame s; 							                    /* scratch variable */
    s.info = buffer[frame_nr]; 			                	/* insert packet into frame */
    s.seq = frame_nr; 					                	/* insert sequence number into frame */
    s.ack = (frame_expected + MAX_SEQ) % (MAX_SEQ + 1);	    /* piggyback ack */
    to_physical_layer(&s,socket); 					        /* transmit the frame */
    start_timer(frame_nr); 				                	/* start the timer running */
    std::cout<<"sending Packet: "<<s.info.data<<std::endl;
}



using namespace std;



int main(int argc, char const *argv[])
{
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char inbuffer[1024] ;
    seq_nr next_frame_to_send; 				/* MAX_SEQ > 1; used for outbound stream */
    seq_nr ack_expected; 					/* oldest frame as yet unacknowledged */
    seq_nr frame_expected; 					/* next frame expected on inbound stream */
    frame r; 							/* scratch variable */

    packet buffer[MAX_SEQ + 1]; 				/* buffers for the outbound stream */
    seq_nr nbuffered; 						/* number of output buffers currently in use */
    seq_nr i; 							/* used to index into the buffer array */
    event_type event;

    enable_network_layer(); 					/* allow network layer ready events */
    ack_expected = 0; 						/* next ack expected inbound */
    next_frame_to_send = 0; 					/* next frame going out */
    frame_expected = 0; 					/* number of frame expected inbound */
    nbuffered = 0;						/* initially no packets are buffered */

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                                                  &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );

    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&address,
                                 sizeof(address))<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                       (socklen_t*)&addrlen))<0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }
        fcntl(new_socket, F_SETFL, O_NONBLOCK);

    while(1){

    //valread = read( new_socket , buffer, 1024);
    //printf("%s\n",buffer );
    //send(new_socket , "hello client,its server" , 50 , 0 );


        wait_for_event(&event);
        					/* four possibilities: see event type above */
        switch(event) {
        case network_layer_ready: 					/* the network layer has a packet to send */
            /* Accept, save, and transmit a new frame. */
            from_network_layer(&buffer[next_frame_to_send]); 	/* fetch new packet */
            nbuffered = nbuffered + 1; 				/* expand the sender’s window */
            send_data(next_frame_to_send,frame_expected, buffer,new_socket);	/* transmit the frame */
            inc(next_frame_to_send); 					/* advance sender’s upper window edge */
        break;

        case frame_arrival: 						/* a data or control frame has arrived */
            from_physical_layer(&r,new_socket); 					/* get incoming frame from physical layer */

            if (r.seq == frame_expected) {

                /*Frames are accepted only in order.*/
                to_network_layer(&r.info); 				/* pass packet to network layer */
                inc(frame_expected); 					/* advance lower edge of receiver’s window */
            }
            std::cout<<r.ack<<std::endl;
            /* Ack n implies n − 1, n − 2, etc. Check for this. */
            while (between(ack_expected, r.ack, next_frame_to_send)) {
            /* Handle piggybacked ack. */
                std::cout<<"frame"<<nbuffered<<"is acknowledged"<<std::endl;
                nbuffered = nbuffered - 1; 				/* one frame fewer buffered */
                stop_timer(ack_expected); 				/* frame arrived intact; stop timer */
                inc(ack_expected); 					/* contract sender’s window */
            }
            next_frame_to_send = r.ack+1;
        break;

        case cksum_err: break; 					/* just ignore bad frames */

        case timeout: 							/* trouble; retransmit all outstanding frames */
        next_frame_to_send = ack_expected; 				/* start retransmitting here */
        for (i = 1; i <= nbuffered; i++) {
            send_data(next_frame_to_send, frame_expected, buffer,new_socket);	/* resend frame */
            inc(next_frame_to_send); 					/* prepare to send the next one */
    }
}

    if (nbuffered < MAX_SEQ){
        enable_network_layer();
    }
    else
    {
     disable_network_layer();
    cout<<"frame arival on";
    event = frame_arrival;
    }


    usleep(100000);
    }
    return 0;
}
