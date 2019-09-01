#include "contiki.h"
#include "lib/list.h"
#include "lib/memb.h"
#include "lib/random.h"
#include "net/rime/rime.h"
#include <stdio.h>
#include "cc2420.h"
#include "broadcast_message.h" /* Structure of broadcast messages. */

/* Structure that holds information about neighbors. */
struct neighbor {
  /* The next pointer is needed since we are placing these on a list. */
  struct neighbor* next;
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
uint8_t this_Node_Hop = -1;  // default hop count for initial beacon
uint8_t seqno = -1;          // default sequence number for initial beacon
const linkaddr_t parent;

/* These two defines are used for computing the moving average for the broadcast
 * sequence number gaps. */
#define SEQNO_EWMA_UNITY 0x100
#define SEQNO_EWMA_ALPHA 0x040

/*---------------------------------------------------------------------------*/
PROCESS(nodes, "Nodes process");
AUTOSTART_PROCESSES(&nodes);
/*---------------------------------------------------------------------------*/

/* This function is called whenever a broadcast message is received. */
static void broadcast_recv(struct broadcast_conn* c, const linkaddr_t* from) {
  struct neighbor* n;
  struct broadcast_message* m;  /*used for getting the struct*/
  struct broadcast_message msg; /*used for broadcasting a new struct*/
  uint8_t seqno_gap;

  /* The packetbuf_dataptr() returns a pointer to the first data byte in the
   * received packet. */
  m = packetbuf_dataptr();

  /* Used to get the real RSSI*/
  static signed char rss;
  static signed char rss_val;
  static signed char rss_offset;
  rss_val = cc2420_last_rssi;
  rss_offset = -45;
  rss = rss_val; /* + rss_offset; not using offset*/

  /* if rssi is less than 95, drop the message as is poor value*/
  if (rss < -95) {
    return;
  }

  /*Checks against its current hop count + RSSI*/
  if (m->hop_count + 1 < this_Node_Hop) {
    /*Assigns info to this Nodes variables*/
    seqno = m->seqno;
    this_Node_Hop = m->hop_count;
    this_Node_Hop++;
    memcpy(parent.u8, from->u8, 2);
    printf(
        "My Parent is %d.%d and my hop count is %u and my sequence # is %d and "
        "my "
        "RSSI is %d \n",
        parent.u8[0], parent.u8[1], m->hop_count + 1, m->seqno, rss);
  }
  /*This is used for an update beacon with a different sequence number, useful
     as if changes occur in network e.g. node is moved, this will update the
     tree*/
  else if (seqno != m->seqno) {
    seqno = m->seqno;
    this_Node_Hop = m->hop_count;
    this_Node_Hop++;
    memcpy(parent.u8, from->u8, 2);
    printf(
        "My Parent is %d.%d and my hop count is %u and my sequence # is %d and "
        "my "
        "RSSI is %d \n",
        parent.u8[0], parent.u8[1], m->hop_count + 1, m->seqno, rss);
    /*Resends the update beacon, placed in here in order to prevent broadcast
     * loops*/
    msg.seqno = m->seqno;
    msg.hop_count = m->hop_count + 1;
    packetbuf_copyfrom(&msg, sizeof(struct broadcast_message));
    broadcast_send(&broadcast);
  }
  /*Adds neighbours to thw list*/
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
    msg.seqno = m->seqno;
    msg.hop_count = m->hop_count + 1;
    packetbuf_copyfrom(&msg, sizeof(struct broadcast_message));
    broadcast_send(&broadcast);
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

PROCESS_THREAD(nodes, ev, data) {
  static struct etimer et;
  static uint8_t seqno;
  struct broadcast_message msg;

  PROCESS_EXITHANDLER(broadcast_close(&broadcast);)

  PROCESS_BEGIN();

  broadcast_open(&broadcast, 129, &broadcast_call);
  /*Big timer used to keep to process alive with some random maths in there*/
  while (1) {
    etimer_set(&et, CLOCK_SECOND * 1000000000000);

    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
  }

  PROCESS_END();
}
