// Client side C/C++ program to demonstrate Socket programming
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#define PORT 8080
#include "header-1.h"
#define MAX_SEQ 7
#define sending_data 0
#define recieving_Data 1
#include <iostream>
#include <fcntl.h>

using namespace std;
int current_state = sending_data;

packet first = {"dummy"};
packet second = {"there"};
packet third = {"iam"};
packet fourth = {"a"};
packet fifth = {"packet"};
packet sixth = {"to"};
packet seventh = {"check"};


packet array_of_packets[7] = {first,second,third,fourth,fifth,sixth,seventh};
frame physical_buffer;




void stop_timer(seq_nr k);

void start_timer(seq_nr k){

}

void from_network_layer(packet* p){

static int packet_counter = 0;
*p = array_of_packets[packet_counter];
//packet_counter =  (packet_counter + 1) % 7  ;

}

void to_physical_layer(frame* s, int socket){

    send(socket , s->info.data , 50 , 0 );
    send(socket , &(s->seq) , 50 , 0 );
    send(socket , &(s->ack) , 50 , 0 );

    /*

    physical_buffer.ack = s->ack;
    physical_buffer.info = s->info;
    physical_buffer.kind = s->kind;
    physical_buffer.seq = s->seq;


    */
}

void from_physical_layer(frame* r,int socket){

    // read( socket , r->info.data, 1024);
      read( socket , &(r->seq), 1024);
      read( socket , &(r->ack), 1024);

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
    frame s; 							/* scratch variable */
    s.info = buffer[frame_nr]; 				/* insert packet into frame */
    s.seq = frame_nr; 						/* insert sequence number into frame */
    s.ack = (frame_expected + MAX_SEQ) % (MAX_SEQ + 1);	/* piggyback ack */
    to_physical_layer(&s,socket); 					/* transmit the frame */
    start_timer(frame_nr); 					/* start the timer running */
    //std::cout<<"sending Packet: "<<s.info.data<<std::endl;
}





int main(int argc, char const *argv[])
{
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char *hello = "Hello from client";
    char inbuffer[1024] = {0};
     seq_nr next_frame_to_send; 				/* MAX_SEQ > 1; used for outbound stream */
    seq_nr ack_expected; 					/* oldest frame as yet unacknowledged */
    seq_nr frame_expected; 					/* next frame expected on inbound stream */
    frame r; 							/* scratch variable */

    packet buffer[MAX_SEQ + 1]; 				/* buffers for the outbound stream */
    seq_nr nbuffered; 						/* number of output buffers currently in use */
    seq_nr i; 							/* used to index into the buffer array */
    event_type event=cksum_err;

    //enable_network_layer(); 					/* allow network layer ready events */
    ack_expected = 0; 						/* next ack expected inbound */
    next_frame_to_send = 0; 					/* next frame going out */
    frame_expected = 0; 					/* number of frame expected inbound */
    nbuffered = 0;						/* initially no packets are buffered */
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }
    fcntl(sock, F_SETFL, O_NONBLOCK);

    while(1){

    valread = recv( sock , inbuffer, sizeof(inbuffer),0);
    //printf("%s\n",inbuffer );
       //wait_for_event(&event);
       if (valread>0){
            event = frame_arrival;
       } 	/* four possibilities: see event type above */
       else{
       event = network_layer_ready;
       }
        switch(event) {
        case network_layer_ready: 				            	/* the network layer has a packet to send */
            /* Accept, save, and transmit a new frame. */
            from_network_layer(&buffer[next_frame_to_send]); 	/* fetch new packet */
            nbuffered = nbuffered + 1; 				        /* expand the sender’s window */
            cout<<"sending ack"<<next_frame_to_send;
            send_data(next_frame_to_send,frame_expected, buffer,sock);	/* transmit the frame */
            inc(next_frame_to_send); 					/* advance sender’s upper window edge */
            event = cksum_err;

        break;

        case frame_arrival: 						/* a data or control frame has arrived */
            from_physical_layer(&r,sock); 					/* get incoming frame from physical layer */

            event = network_layer_ready;
            //event = cksum_err;

            if (r.seq == frame_expected) {
                cout<<"recieved frame :"<<inbuffer<<endl;               /*Frames are accepted only in order.*/
                to_network_layer(&r.info); 			                	/* pass packet to network layer */
                inc(frame_expected); 					                /* advance lower edge of receiver’s window */
            }
            else{
            cout<<"wrong frame no";
            }

            /* Ack n implies n − 1, n − 2, etc. Check for this. */
            while (between(ack_expected, r.ack, next_frame_to_send)) {
            /* Handle piggybacked ack. */
                nbuffered = nbuffered - 1; 				/* one frame fewer buffered */
                stop_timer(ack_expected); 				/* frame arrived intact; stop timer */
                inc(ack_expected); 					/* contract sender’s window */
                cout<<"inc ack expected";
            }
        break;

        case cksum_err: break; 					/* just ignore bad frames */

        case timeout: 							/* trouble; retransmit all outstanding frames */
        next_frame_to_send = ack_expected; 				/* start retransmitting here */
        for (i = 1; i <= nbuffered; i++) {
            send_data(next_frame_to_send, frame_expected, buffer,sock);	/* resend frame */
            inc(next_frame_to_send); 					/* prepare to send the next one */
    }
}

    if (nbuffered < MAX_SEQ)
    enable_network_layer();
    else
    disable_network_layer();





    }

    return 0;
}
