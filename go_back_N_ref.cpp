#include "header.h"
#define MAX_SEQ 3
#include <iostream>
#define sending_data 0
#define recieving_Data 1
int current_state = sending_data;

packet first = {{'a','b','c'}};
packet second = {{'d','e','f'}};
packet third = {{'g','h','i'}};
packet array_of_packets[3] = {first,second,third};
frame physical_buffer;

void stop_timer(seq_nr k);


void from_network_layer(packet* p){

static int packet_counter = 0;
*p = array_of_packets[packet_counter];
packet_counter =  (packet_counter + 1) % 3  ;



}

void to_physical_layer(frame* s){

    physical_buffer.ack = s->ack;
    physical_buffer.info = s->info;
    physical_buffer.kind = s->kind;
    physical_buffer.seq = s->seq;

}

void start_timer(seq_nr k){


}

void from_physical_layer(frame* r){

    r->ack=physical_buffer.ack ;
    r->info=physical_buffer.info ;
     r->kind= physical_buffer.kind ;
    r->seq=physical_buffer.seq ;
    std::cout<<r->ack<<std::endl;
    std::cout<<r->info.data<<std::endl;

}

void wait_for_event(event_type* event){
    if (current_state == sending_data ){
        *event=network_layer_ready;
    }
    if (current_state == recieving_Data ){
        *event=frame_arrival;
    }

}




static bool between(seq_nr a, seq_nr b, seq_nr c)
{
/* Return true if a <= b < c circularly; false otherwise. */
    if (((a <= b) && (b < c)) || ((c < a) && (a <= b)) || ((b < c) && (c < a)))
        return(true);
    else
        return(false);
}

void enable_network_layer(void){
        current_state = sending_data;
}

void to_network_layer(packet* p){}
void stop_timer(seq_nr k){}
void disable_network_layer(void){
        current_state = recieving_Data;

}

static void send_data(seq_nr frame_nr, seq_nr frame_expected, packet buffer[])
{
/* Construct and send a data frame. */
    std::cout<<"iam in sending_Data";
    frame s; 						/* scratch variable */
    s.info = buffer[frame_nr]; 				/* insert packet into frame */
    s.seq = frame_nr; 					/* insert sequence number into frame */
    s.ack = (frame_expected + MAX_SEQ) % (MAX_SEQ + 1);	/* piggyback ack */
    to_physical_layer(&s); 					/* transmit the frame */ //a,b,c
    start_timer(frame_nr); 					/* start the timer running */
    std::cout<<"sending packet"<<s.info.data<<std::endl;
}

int main(void)
{
    seq_nr next_frame_to_send; 				/* MAX_SEQ > 1; used for outbound stream */
    seq_nr ack_expected; 					/* oldest frame as yet unacknowledged */
    seq_nr frame_expected; 					/* next frame expected on inbound stream */
    frame r; 						/* scratch variable */

    packet buffer[MAX_SEQ + 1]; 				/* buffers for the outbound stream */
    seq_nr nbuffered; 					/* number of output buffers currently in use */
    seq_nr i; 						/* used to index into the buffer array */
    event_type event;

    enable_network_layer(); 				/* allow network layer ready events */
    ack_expected = 0; 					/* next ack expected inbound */
    next_frame_to_send = 0; 				/* next frame going out */
    frame_expected = 0; 					/* number of frame expected inbound */
    nbuffered = 0;						/* initially no packets are buffered */
    while (true) {
        std::cout<<event<<std::endl;
        wait_for_event(&event); 				/* four possibilities: see event type above */
        switch(event) {
        case network_layer_ready: 				/* the network layer has a packet to send */
            /* Accept, save, and transmit a new frame. */
            from_network_layer(&buffer[next_frame_to_send]); 	/* fetch new packet */
            nbuffered = nbuffered + 1; 				/* expand the sender’s window */
            send_data(next_frame_to_send,frame_expected, buffer);	/* transmit the frame */
            inc(next_frame_to_send); 				/* advance sender’s upper window edge */
        break;

        case frame_arrival: 					/* a data or control frame has arrived */
            from_physical_layer(&r); 				/* get incoming frame from physical layer */

            if (r.seq == frame_expected) {
                std::cout<<"expected"<<std::endl;

                /*Frames are accepted only in order.*/
                to_network_layer(&r.info); 				/* pass packet to network layer */
                inc(frame_expected); 					/* advance lower edge of receiver’s window */
            }

            /* Ack n implies n − 1, n − 2, etc. Check for this. */
            while (between(ack_expected, r.ack, next_frame_to_send)) {
            /* Handle piggybacked ack. */
                std::cout<<"frame"<<nbuffered<<"is acknowledged"<<std::endl;
                nbuffered = nbuffered - 1; 				/* one frame fewer buffered */
                stop_timer(ack_expected); 				/* frame arrived intact; stop timer */
                inc(ack_expected); 					/* contract sender’s window */
            }
        break;

        case cksum_err: break; 					/* just ignore bad frames */

        case timeout: 						/* trouble; retransmit all outstanding frames */
        next_frame_to_send = ack_expected; 			/* start retransmitting here */
        for (i = 1; i <= nbuffered; i++) {
            send_data(next_frame_to_send, frame_expected, buffer);	/* resend frame */
            inc(next_frame_to_send); 				/* prepare to send the next one */
    }
}

    if (nbuffered < MAX_SEQ)
    enable_network_layer();
    else
    disable_network_layer();
}
return 0;
}
