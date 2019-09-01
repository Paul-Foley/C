
#include "contiki.h"
#include "lib/list.h"
#include "lib/memb.h"
#include "lib/random.h"
#include "net/rime/rime.h"
#include <stdio.h>
#include "dev/button-sensor.h"
#include "broadcast_message.h" /* Structure of broadcast messages. */

/* Structure that holds information about neighbors. */
struct neighbor {
  /* The next pointer is needed since we are placing these on a list. */
  struct neighbor *next;

  /* The addr field holds the Rime address of the neighbor. */
  linkaddr_t addr;

  /* The last_rssi and last_lqi fields hold the Received Signal Strength
   * Indicator (RSSI) and CC2420 Link Quality Indicator (LQI) values that are
   * received for the incoming broadcast packets. */
  uint16_t last_rssi, last_lqi;

  /* Each broadcast packet contains a sequence number (seqno). The last_seqno
   * field holds the last sequenuce number we saw from this neighbor. */
  uint8_t last_seqno;

  /* The avg_gap contains the average seqno gap that we have seen from this
   * neighbor. */
  uint32_t avg_seqno_gap;
};

/* This #define defines the maximum amount of neighbors we can remember. */
#define MAX_NEIGHBORS 16

/* This MEMB() definition defines a memory pool from which we allocate neighbor
 * entries. */
MEMB(neighbors_memb, struct neighbor, MAX_NEIGHBORS);

/* The neighbors_list is a Contiki list that holds the neighbors we have seen
 * thus far. */
LIST(neighbors_list);

/* These hold the broadcast structure. */
static struct broadcast_conn broadcast;

/* These two defines are used for computing the moving average for the broadcast
 * sequence number gaps. */
#define SEQNO_EWMA_UNITY 0x100
#define SEQNO_EWMA_ALPHA 0x040

/*---------------------------------------------------------------------------*/
PROCESS(sink, "Sink process");
AUTOSTART_PROCESSES(&sink);
/*---------------------------------------------------------------------------*/

/* This function is called whenever a broadcast message is received. */
static void broadcast_recv(struct broadcast_conn *c, const linkaddr_t *from) {
  struct neighbor *n;
  struct broadcast_message *m;
  uint8_t seqno_gap;

  /* The packetbuf_dataptr() returns a pointer to the first data byte in the
   * received packet. */
  m = packetbuf_dataptr();

  /* Check if we already know this neighbor. */
  for (n = list_head(neighbors_list); n != NULL; n = list_item_next(n)) {
    /* We break out of the loop if the address of the neighbor matches the
     * address of the neighbor from which we received this broadcast message. */
    if (linkaddr_cmp(&n->addr, from)) {
      break;
    }
  }

  /* If n is NULL, this neighbor was not found in our list, and we allocate a
   * new struct neighbor from the neighbors_memb memory pool. */
  if (n == NULL) {
    n = memb_alloc(&neighbors_memb);

    /* If we could not allocate a new neighbor entry, we give up. We could have
     * reused an old neighbor entry, but we do not do this for now. */
    if (n == NULL) {
      return;
    }

    /* Initialize the fields. */
    linkaddr_copy(&n->addr, from);
    n->last_seqno = m->seqno - 1;
    n->avg_seqno_gap = SEQNO_EWMA_UNITY;

    /* Place the neighbor on the neighbor list. */
    list_add(neighbors_list, n);
  }

  /* We can now fill in the fields in our neighbor entry. */
  n->last_rssi = packetbuf_attr(PACKETBUF_ATTR_RSSI);
  n->last_lqi = packetbuf_attr(PACKETBUF_ATTR_LINK_QUALITY);

  /* Compute the average sequence number gap we have seen from this neighbor. */
  seqno_gap = m->seqno - n->last_seqno;
  n->avg_seqno_gap =
      (((uint32_t)seqno_gap * SEQNO_EWMA_UNITY) * SEQNO_EWMA_ALPHA) /
          SEQNO_EWMA_UNITY +
      ((uint32_t)n->avg_seqno_gap * (SEQNO_EWMA_UNITY - SEQNO_EWMA_ALPHA)) /
          SEQNO_EWMA_UNITY;

  /* Remember last seqno we heard. */
  n->last_seqno = m->seqno;
}
/* This is where we define what function to be called when a broadcast is
 * received. We pass a pointer to this structure in the broadcast_open() call
 * below. */
static const struct broadcast_callbacks broadcast_call = {broadcast_recv};
uint8_t seqnum = 0;

PROCESS_THREAD(sink, ev, data) {
  static struct etimer et;
  struct broadcast_message msg;
  uint8_t sink_hop = 0;
  PROCESS_EXITHANDLER(broadcast_close(&broadcast);)

  PROCESS_BEGIN();

  broadcast_open(&broadcast, 129, &broadcast_call);

  /* If i wanted to use a button followed by a timer, I would use this code below but it would not recognise my button press in cooja
while(1){

   PROCESS_WAIT_EVENT_UNTIL(data == &button_sensor);
   printf("In\n");
   msg.seqno=seqnum;
   msg.hop_count=0;
   packetbuf_copyfrom(&msg, sizeof(struct broadcast_message));
   broadcast_send(&broadcast);

   while(1){
    Send a broadcast randomly based on timer
   etimer_set(&et, CLOCK_SECOND * 20 + random_rand() % (CLOCK_SECOND * 20));
   PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
   seqnum++;
   msg.seqno=seqnum;
   msg.hop_count=0;
   packetbuf_copyfrom(&msg, sizeof(struct broadcast_message));
   broadcast_send(&broadcast);
   etimer_reset(&et);
}*/

  while (1) {
    /* Send a broadcast every 16 - 32 seconds */
    etimer_set(&et, CLOCK_SECOND * 8 + random_rand() % (CLOCK_SECOND * 8));
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
    msg.seqno = seqnum;
    msg.hop_count = 0;
    /*if sequence number is higher than 0, means that it wants initate update.*/
    if (seqnum > 0) {
      printf("Requesting Update\n");
    }
    packetbuf_copyfrom(&msg, sizeof(struct broadcast_message));
    broadcast_send(&broadcast);
    etimer_reset(&et);

    seqnum++;
  }

  PROCESS_END();
}

