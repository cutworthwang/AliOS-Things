#include "esp_common.h"
#include "lwip/inet.h"
#include "lwip/err.h"
#include "lwip/pbuf.h"
#include "lwip/udp.h"
#include "lwip/mem.h"
#include "lwip/dhcpserver.h"

#ifndef LWIP_OPEN_SRC
#include "net80211/ieee80211_var.h"
#endif
#include "netif/wlan_lwip_if.h"

#ifdef MEMLEAK_DEBUG
static const char mem_debug_file[] ICACHE_RODATA_ATTR STORE_ATTR = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////////
//static const uint8_t xid[4] = {0xad, 0xde, 0x12, 0x23};
//static u8_t old_xid[4] = {0};
static const u32_t magic_cookie ICACHE_RODATA_ATTR STORE_ATTR = 0x63538263;
static struct udp_pcb *pcb_dhcps = NULL;
static struct ip_addr broadcast_dhcps;
static struct ip_addr server_address;
static struct ip_addr client_address;//added
static struct ip_addr client_address_plus;

static struct dhcps_lease dhcps_lease;
//static bool dhcps_lease_flag = true;
static list_node *plist = NULL;
static u8_t offer = 0xFF;
static bool renew = false;
#define DHCPS_LEASE_TIME_DEF	(120)
u32_t dhcps_lease_time = DHCPS_LEASE_TIME_DEF;  //minute
/******************************************************************************
 * FunctionName : node_insert_to_list
 * Description  : insert the node to the list
 * Parameters   : arg -- Additional argument to pass to the callback function
 * Returns      : none
*******************************************************************************/
void node_insert_to_list(list_node **phead, list_node* pinsert)
{
	list_node *plist = NULL;
	struct dhcps_pool *pdhcps_pool = NULL;
	struct dhcps_pool *pdhcps_node = NULL;
	if (*phead == NULL)
		*phead = pinsert;
	else {
		plist = *phead;
		pdhcps_node = pinsert->pnode;
		pdhcps_pool = plist->pnode;

		if(pdhcps_node->ip.addr < pdhcps_pool->ip.addr) {
		    pinsert->pnext = plist;
		    *phead = pinsert;
		} else {
            while (plist->pnext != NULL) {
                pdhcps_pool = plist->pnext->pnode;
                if (pdhcps_node->ip.addr < pdhcps_pool->ip.addr) {
                    pinsert->pnext = plist->pnext;
                    plist->pnext = pinsert;
                    break;
                }
                plist = plist->pnext;
            }

            if(plist->pnext == NULL) {
                plist->pnext = pinsert;
            }
		}
	}
//	pinsert->pnext = NULL;
}

/******************************************************************************
 * FunctionName : node_delete_from_list
 * Description  : remove the node from list
 * Parameters   : arg -- Additional argument to pass to the callback function
 * Returns      : none
*******************************************************************************/
void node_remove_from_list(list_node **phead, list_node* pdelete)
{
	list_node *plist = NULL;

	plist = *phead;
	if (plist == NULL){
		*phead = NULL;
	} else {
		if (plist == pdelete){
			*phead = plist->pnext;
            pdelete->pnext = NULL;
		} else {
			while (plist != NULL) {
				if (plist->pnext == pdelete){
					plist->pnext = pdelete->pnext;
                    pdelete->pnext = NULL;
				}
				plist = plist->pnext;
			}
		}
	}
}
///////////////////////////////////////////////////////////////////////////////////
/*
 * ��DHCP msg��Ϣ�ṹ����������
 *
 * @param optptr -- DHCP msg��Ϣλ��
 * @param type -- Ҫ��ӵ�����option
 *
 * @return uint8_t* ����DHCP msgƫ�Ƶ�ַ
 */
///////////////////////////////////////////////////////////////////////////////////
static u8_t* add_msg_type(u8_t *optptr, u8_t type)
{

        *optptr++ = DHCP_OPTION_MSG_TYPE;
        *optptr++ = 1;
        *optptr++ = type;
        return optptr;
}
///////////////////////////////////////////////////////////////////////////////////
/*
 * ��DHCP msg�ṹ������offerӦ������
 *
 * @param optptr -- DHCP msg��Ϣλ��
 *
 * @return uint8_t* ����DHCP msgƫ�Ƶ�ַ
 */
///////////////////////////////////////////////////////////////////////////////////
static u8_t* add_offer_options(u8_t *optptr)
{
        struct ip_addr ipadd;

        ipadd.addr = *( (u32_t *) &server_address);

#ifdef USE_CLASS_B_NET
        *optptr++ = DHCP_OPTION_SUBNET_MASK;
        *optptr++ = 4;  //length
        *optptr++ = 255;
        *optptr++ = 240;	
        *optptr++ = 0;
        *optptr++ = 0;
#else
        *optptr++ = DHCP_OPTION_SUBNET_MASK;
        *optptr++ = 4;  
        *optptr++ = 255;
        *optptr++ = 255;	
        *optptr++ = 255;
        *optptr++ = 0;
#endif

        *optptr++ = DHCP_OPTION_LEASE_TIME;
        *optptr++ = 4;  
        *optptr++ = ((DHCPS_LEASE_TIMER * 60) >> 24) & 0xFF;
        *optptr++ = ((DHCPS_LEASE_TIMER * 60) >> 16) & 0xFF;
        *optptr++ = ((DHCPS_LEASE_TIMER * 60) >> 8) & 0xFF;
        *optptr++ = ((DHCPS_LEASE_TIMER * 60) >> 0) & 0xFF;

        *optptr++ = DHCP_OPTION_SERVER_ID;
        *optptr++ = 4;  
        *optptr++ = ip4_addr1( &ipadd);
        *optptr++ = ip4_addr2( &ipadd);
        *optptr++ = ip4_addr3( &ipadd);
        *optptr++ = ip4_addr4( &ipadd);

        if (dhcps_router_enabled(offer)){
        	struct ip_info if_ip;
			bzero(&if_ip, sizeof(struct ip_info));
			wifi_get_ip_info(SOFTAP_IF, &if_ip);

			*optptr++ = DHCP_OPTION_ROUTER;
			*optptr++ = 4;
			*optptr++ = ip4_addr1( &if_ip.gw);
			*optptr++ = ip4_addr2( &if_ip.gw);
			*optptr++ = ip4_addr3( &if_ip.gw);
			*optptr++ = ip4_addr4( &if_ip.gw);
        }

#ifdef USE_DNS
	    *optptr++ = DHCP_OPTION_DNS_SERVER;
	    *optptr++ = 4;
	    *optptr++ = ip4_addr1( &ipadd);
		*optptr++ = ip4_addr2( &ipadd);
		*optptr++ = ip4_addr3( &ipadd);
		*optptr++ = ip4_addr4( &ipadd);
#endif

#ifdef CLASS_B_NET
        *optptr++ = DHCP_OPTION_BROADCAST_ADDRESS;
        *optptr++ = 4;  
        *optptr++ = ip4_addr1( &ipadd);
        *optptr++ = 255;
        *optptr++ = 255;
        *optptr++ = 255;
#else
        *optptr++ = DHCP_OPTION_BROADCAST_ADDRESS;
        *optptr++ = 4;  
        *optptr++ = ip4_addr1( &ipadd);
        *optptr++ = ip4_addr2( &ipadd);
        *optptr++ = ip4_addr3( &ipadd);
        *optptr++ = 255;
#endif

        *optptr++ = DHCP_OPTION_INTERFACE_MTU;
        *optptr++ = 2;  
#ifdef CLASS_B_NET
        *optptr++ = 0x05;	
        *optptr++ = 0xdc;
#else
        *optptr++ = 0x02;	
        *optptr++ = 0x40;
#endif

        *optptr++ = DHCP_OPTION_PERFORM_ROUTER_DISCOVERY;
        *optptr++ = 1;  
        *optptr++ = 0x00; 

        *optptr++ = 43;	
        *optptr++ = 6;	

        *optptr++ = 0x01;	
        *optptr++ = 4;  
        *optptr++ = 0x00;
        *optptr++ = 0x00;
        *optptr++ = 0x00;
        *optptr++ = 0x02; 	

        return optptr;
}
///////////////////////////////////////////////////////////////////////////////////
/*
 * ��DHCP msg�ṹ����ӽ����־����
 *
 * @param optptr -- DHCP msg��Ϣλ��
 *
 * @return uint8_t* ����DHCP msgƫ�Ƶ�ַ
 */
///////////////////////////////////////////////////////////////////////////////////
static u8_t* add_end(u8_t *optptr)
{

        *optptr++ = DHCP_OPTION_END;
        return optptr;
}
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
static void create_msg(struct dhcps_msg *m)
{
        struct ip_addr client;

        client.addr = *( (uint32_t *) &client_address);

        m->op = DHCP_REPLY;
        m->htype = DHCP_HTYPE_ETHERNET;
        m->hlen = 6;  
        m->hops = 0;
//        os_memcpy((char *) xid, (char *) m->xid, sizeof(m->xid));
        m->secs = 0;
        m->flags = htons(BOOTP_BROADCAST); 

        memcpy((char *) m->yiaddr, (char *) &client.addr, sizeof(m->yiaddr));

        memset((char *) m->ciaddr, 0, sizeof(m->ciaddr));
        memset((char *) m->siaddr, 0, sizeof(m->siaddr));
        memset((char *) m->giaddr, 0, sizeof(m->giaddr));
        memset((char *) m->sname, 0, sizeof(m->sname));
        memset((char *) m->file, 0, sizeof(m->file));

        memset((char *) m->options, 0, sizeof(m->options));

		u32_t magic_cookie1 = magic_cookie;
        memcpy((char *) m->options, &magic_cookie1, sizeof(magic_cookie1));
}
///////////////////////////////////////////////////////////////////////////////////
/*
 * ����һ��OFFER
 *
 * @param -- m ָ����Ҫ���͵�DHCP msg����
 */
///////////////////////////////////////////////////////////////////////////////////
static void send_offer(struct dhcps_msg *m)
{
        u8_t *end;
	    struct pbuf *p, *q;
	    u8_t *data;
	    u16_t cnt=0;
	    u16_t i;
		err_t SendOffer_err_t;
        create_msg(m);

        end = add_msg_type(&m->options[4], DHCPOFFER);
        end = add_offer_options(end);
        end = add_end(end);

	    p = pbuf_alloc(PBUF_TRANSPORT, sizeof(struct dhcps_msg), PBUF_RAM);
#if DHCPS_DEBUG
		os_printf("udhcp: send_offer>>p->ref = %d\n", p->ref);
#endif
	    if(p != NULL){
	       
#if DHCPS_DEBUG
	        os_printf("dhcps: send_offer>>pbuf_alloc succeed\n");
	        os_printf("dhcps: send_offer>>p->tot_len = %d\n", p->tot_len);
	        os_printf("dhcps: send_offer>>p->len = %d\n", p->len);
#endif
	        q = p;
	        while(q != NULL){
	            data = (u8_t *)q->payload;
	            for(i=0; i<q->len; i++)
	            {
	                data[i] = ((u8_t *) m)[cnt++];
#if DHCPS_DEBUG
					os_printf("%02x ",data[i]);
					if((i+1)%16 == 0){
						os_printf("\n");
					}
#endif
	            }

	            q = q->next;
	        }
	    }else{
	        
#if DHCPS_DEBUG
	        os_printf("dhcps: send_offer>>pbuf_alloc failed\n");
#endif
	        return;
	    }
        SendOffer_err_t = udp_sendto( pcb_dhcps, p, &broadcast_dhcps, DHCPS_CLIENT_PORT );
#if DHCPS_DEBUG
	        os_printf("dhcps: send_offer>>udp_sendto result %x\n",SendOffer_err_t);
#endif
	    if(p->ref != 0){	
#if DHCPS_DEBUG
	        os_printf("udhcp: send_offer>>free pbuf\n");
#endif
	        pbuf_free(p);
	    }
}
///////////////////////////////////////////////////////////////////////////////////
/*
 * ����һ��NAK��Ϣ
 *
 * @param m ָ����Ҫ���͵�DHCP msg����
 */
///////////////////////////////////////////////////////////////////////////////////
static void send_nak(struct dhcps_msg *m)
{

    	u8_t *end;
	    struct pbuf *p, *q;
	    u8_t *data;
	    u16_t cnt=0;
	    u16_t i;
		err_t SendNak_err_t;
        create_msg(m);

        end = add_msg_type(&m->options[4], DHCPNAK);
        end = add_end(end);

	    p = pbuf_alloc(PBUF_TRANSPORT, sizeof(struct dhcps_msg), PBUF_RAM);
#if DHCPS_DEBUG
		os_printf("udhcp: send_nak>>p->ref = %d\n", p->ref);
#endif
	    if(p != NULL){
	        
#if DHCPS_DEBUG
	        os_printf("dhcps: send_nak>>pbuf_alloc succeed\n");
	        os_printf("dhcps: send_nak>>p->tot_len = %d\n", p->tot_len);
	        os_printf("dhcps: send_nak>>p->len = %d\n", p->len);
#endif
	        q = p;
	        while(q != NULL){
	            data = (u8_t *)q->payload;
	            for(i=0; i<q->len; i++)
	            {
	                data[i] = ((u8_t *) m)[cnt++];
#if DHCPS_DEBUG					
					os_printf("%02x ",data[i]);
					if((i+1)%16 == 0){
						os_printf("\n");
					}
#endif
	            }

	            q = q->next;
	        }
	    }else{
	        
#if DHCPS_DEBUG
	        os_printf("dhcps: send_nak>>pbuf_alloc failed\n");
#endif
	        return;
    	}
        SendNak_err_t = udp_sendto( pcb_dhcps, p, &broadcast_dhcps, DHCPS_CLIENT_PORT );
#if DHCPS_DEBUG
	        os_printf("dhcps: send_nak>>udp_sendto result %x\n",SendNak_err_t);
#endif
 	    if(p->ref != 0){
#if DHCPS_DEBUG			
	        os_printf("udhcp: send_nak>>free pbuf\n");
#endif
	        pbuf_free(p);
	    }
}
///////////////////////////////////////////////////////////////////////////////////
/*
 * ����һ��ACK��DHCP�ͻ���
 *
 * @param m ָ����Ҫ���͵�DHCP msg����
 */
///////////////////////////////////////////////////////////////////////////////////
static void send_ack(struct dhcps_msg *m)
{

		u8_t *end;
	    struct pbuf *p, *q;
	    u8_t *data;
	    u16_t cnt=0;
	    u16_t i;
		err_t SendAck_err_t;
        create_msg(m);

        end = add_msg_type(&m->options[4], DHCPACK);
        end = add_offer_options(end);
        end = add_end(end);
	    
	    p = pbuf_alloc(PBUF_TRANSPORT, sizeof(struct dhcps_msg), PBUF_RAM);
#if DHCPS_DEBUG
		os_printf("udhcp: send_ack>>p->ref = %d\n", p->ref);
#endif
	    if(p != NULL){
	        
#if DHCPS_DEBUG
	        os_printf("dhcps: send_ack>>pbuf_alloc succeed\n");
	        os_printf("dhcps: send_ack>>p->tot_len = %d\n", p->tot_len);
	        os_printf("dhcps: send_ack>>p->len = %d\n", p->len);
#endif
	        q = p;
	        while(q != NULL){
	            data = (u8_t *)q->payload;
	            for(i=0; i<q->len; i++)
	            {
	                data[i] = ((u8_t *) m)[cnt++];
#if DHCPS_DEBUG					
					os_printf("%02x ",data[i]);
					if((i+1)%16 == 0){
						os_printf("\n");
					}
#endif
	            }

	            q = q->next;
	        }
	    }else{
	    
#if DHCPS_DEBUG
	        os_printf("dhcps: send_ack>>pbuf_alloc failed\n");
#endif
	        return;
	    }
        SendAck_err_t = udp_sendto( pcb_dhcps, p, &broadcast_dhcps, DHCPS_CLIENT_PORT );
#if DHCPS_DEBUG
	        os_printf("dhcps: send_ack>>udp_sendto result %x\n",SendAck_err_t);
#endif
	    
	    if(p->ref != 0){
#if DHCPS_DEBUG
	        os_printf("udhcp: send_ack>>free pbuf\n");
#endif
	        pbuf_free(p);
	    }
}
///////////////////////////////////////////////////////////////////////////////////
/*
 * ����DHCP�ͻ��˷�����DHCP����������Ϣ�����Բ�ͬ��DHCP��������������Ӧ��Ӧ��
 *
 * @param optptr DHCP msg�е���������
 * @param len ��������Ĵ��?(byte)
 *
 * @return uint8_t ���ش�����DHCP Server״ֵ̬
 */
///////////////////////////////////////////////////////////////////////////////////
static u8_t parse_options(u8_t *optptr, s16_t len)
{
        struct ip_addr client;
    	bool is_dhcp_parse_end = false;
    	struct dhcps_state s;

        client.addr = *( (uint32_t *) &client_address);// Ҫ�����DHCP�ͻ��˵�IP

        u8_t *end = optptr + len;
        u16_t type = 0;

        s.state = DHCPS_STATE_IDLE;

        while (optptr < end) {
#if DHCPS_DEBUG
        	os_printf("dhcps: (s16_t)*optptr = %d\n", (s16_t)*optptr);
#endif
        	switch ((s16_t) *optptr) {

                case DHCP_OPTION_MSG_TYPE:	//53
                        type = *(optptr + 2);
                        break;

                case DHCP_OPTION_REQ_IPADDR://50
                        if( memcmp( (char *) &client.addr, (char *) optptr+2,4)==0 ) {
#if DHCPS_DEBUG
                    		os_printf("dhcps: DHCP_OPTION_REQ_IPADDR = 0 ok\n");
#endif
                            s.state = DHCPS_STATE_ACK;
                        }else {
#if DHCPS_DEBUG
                    		os_printf("dhcps: DHCP_OPTION_REQ_IPADDR != 0 err\n");
#endif
                            s.state = DHCPS_STATE_NAK;
                        }
                        break;
                case DHCP_OPTION_END:
			            {
			                is_dhcp_parse_end = true;
			            }
                        break;
            }

		    if(is_dhcp_parse_end){
		            break;
		    }

            optptr += optptr[1] + 2;
        }

        switch (type){
        
        	case DHCPDISCOVER://1
                s.state = DHCPS_STATE_OFFER;
#if DHCPS_DEBUG
            	os_printf("dhcps: DHCPD_STATE_OFFER\n");
#endif
                break;

        	case DHCPREQUEST://3
                if ( !(s.state == DHCPS_STATE_ACK || s.state == DHCPS_STATE_NAK) ) {
                    if(renew == true) {
                        s.state = DHCPS_STATE_ACK;
                    } else {
                        s.state = DHCPS_STATE_NAK;
                    }
#if DHCPS_DEBUG
                		os_printf("dhcps: DHCPD_STATE_NAK\n");
#endif
                }
                break;

			case DHCPDECLINE://4
                s.state = DHCPS_STATE_IDLE;
#if DHCPS_DEBUG
            	os_printf("dhcps: DHCPD_STATE_IDLE\n");
#endif
                break;

        	case DHCPRELEASE://7
                s.state = DHCPS_STATE_RELEASE;
#if DHCPS_DEBUG
            	os_printf("dhcps: DHCPD_STATE_IDLE\n");
#endif
                break;
        }
#if DHCPS_DEBUG
    	os_printf("dhcps: return s.state = %d\n", s.state);
#endif
        return s.state;
}
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
static s16_t parse_msg(struct dhcps_msg *m, u16_t len)
{
	u32_t lease_timer = (dhcps_lease_time * 60)/DHCPS_COARSE_TIMER_SECS;
		if(memcmp((char *)m->options,
              &magic_cookie,
              sizeof(magic_cookie)) == 0){
#if DHCPS_DEBUG
        	os_printf("dhcps: len = %d\n", len);
#endif
	        /*
         	 * ��¼��ǰ��xid���ﴦ���?
         	 * �˺�ΪDHCP�ͻ����������û�ͳһ��ȡIPʱ��
         	*/
//	        if((old_xid[0] == 0) &&
//	           (old_xid[1] == 0) &&
//	           (old_xid[2] == 0) &&
//	           (old_xid[3] == 0)){
//	            /*
//	             * old_xidδ��¼�κ����?
//	             * �϶��ǵ�һ��ʹ��
//	            */
//	            memcpy((char *)old_xid, (char *)m->xid, sizeof(m->xid));
//	        }else{
//	            /*
//	             * ���δ����DHCP msg��Я���xid���ϴμ�¼�Ĳ�ͬ��
//	             * �϶�Ϊ��ͬ��DHCP�ͻ��˷��ͣ���ʱ����Ҫ����Ŀͻ���IP
//	             * ���� 192.168.4.100(0x6404A8C0) <--> 192.168.4.200(0xC804A8C0)
//	             *
//	            */
//	            if(memcmp((char *)old_xid, (char *)m->xid, sizeof(m->xid)) != 0){
	                /*
                 	 * ��¼���ε�xid�ţ�ͬʱ�����IP����
                 	*/
	                struct ip_addr addr_tmp;    
//	                memcpy((char *)old_xid, (char *)m->xid, sizeof(m->xid));

//	                {
						struct dhcps_pool *pdhcps_pool = NULL;
						list_node *pnode = NULL;
						list_node *pback_node = NULL;
						struct ip_addr first_address;
						bool flag = false;

//						POOL_START:
						first_address.addr = dhcps_lease.start_ip.addr;
						client_address.addr = client_address_plus.addr;
						renew = false;
//							addr_tmp.addr =  htonl(client_address_plus.addr);
//							addr_tmp.addr++;
//							client_address_plus.addr = htonl(addr_tmp.addr);
						if (plist != NULL){
						for (pback_node = plist; pback_node != NULL;pback_node = pback_node->pnext) {
							pdhcps_pool = pback_node->pnode;
							if (memcmp(pdhcps_pool->mac, m->chaddr, sizeof(pdhcps_pool->mac)) == 0){
//									os_printf("the same device request ip\n");
								if (memcmp(&pdhcps_pool->ip.addr, m->ciaddr, sizeof(pdhcps_pool->ip.addr)) == 0) {
								    renew = true;
								}
								client_address.addr = pdhcps_pool->ip.addr;
								pdhcps_pool->lease_timer = lease_timer;
								pnode = pback_node;
								goto POOL_CHECK;
							} else if (pdhcps_pool->ip.addr == client_address_plus.addr){
//									client_address.addr = client_address_plus.addr;
//									os_printf("the ip addr has been request\n");
								addr_tmp.addr = htonl(client_address_plus.addr);
								addr_tmp.addr++;
								client_address_plus.addr = htonl(addr_tmp.addr);
								client_address.addr = client_address_plus.addr;
							}

							if(flag == false) { // search the fisrt unused ip
                                if(first_address.addr < pdhcps_pool->ip.addr) {
                                    flag = true;
                                } else {
                                    addr_tmp.addr = htonl(first_address.addr);
                                    addr_tmp.addr++;
                                    first_address.addr = htonl(addr_tmp.addr);
                                }
							}
						}
						}else{
							client_address.addr = dhcps_lease.start_ip.addr;
						}

						if (client_address_plus.addr > dhcps_lease.end_ip.addr) {
						    client_address.addr = first_address.addr;
						}
						if (client_address.addr > dhcps_lease.end_ip.addr) {
						    client_address_plus.addr = dhcps_lease.start_ip.addr;
						    pdhcps_pool = NULL;
						    pnode = NULL;
						} else {
						pdhcps_pool = (struct dhcps_pool *)os_zalloc(sizeof(struct dhcps_pool));
						pdhcps_pool->ip.addr = client_address.addr;
						memcpy(pdhcps_pool->mac, m->chaddr, sizeof(pdhcps_pool->mac));
						pdhcps_pool->lease_timer = lease_timer;
						pnode = (list_node *)os_zalloc(sizeof(list_node ));
						pnode->pnode = pdhcps_pool;
						    pnode->pnext = NULL;
						    node_insert_to_list(&plist,pnode);
						    if (client_address.addr == dhcps_lease.end_ip.addr) {
						        client_address_plus.addr = dhcps_lease.start_ip.addr;
						    } else {
                                addr_tmp.addr = htonl(client_address.addr);
                                addr_tmp.addr++;
                                client_address_plus.addr = htonl(addr_tmp.addr);
						    }
						}

						POOL_CHECK:
						if ((client_address.addr > dhcps_lease.end_ip.addr) || (ip_addr_isany(&client_address))){
						    if(pnode != NULL) {
						        node_remove_from_list(&plist,pnode);
						        os_free(pnode);
						        pnode = NULL;
						    }

						    if (pdhcps_pool != NULL) {
						        os_free(pdhcps_pool);
						        pdhcps_pool = NULL;
						    }
//							client_address_plus.addr = dhcps_lease.start_ip.addr;
							return 4;
						}

						s16_t ret = parse_options(&m->options[4], len);;

						if(ret == DHCPS_STATE_RELEASE) {
						    if(pnode != NULL) {
						        node_remove_from_list(&plist,pnode);
						        os_free(pnode);
						        pnode = NULL;
						    }

						    if (pdhcps_pool != NULL) {
						        os_free(pdhcps_pool);
						        pdhcps_pool = NULL;
						    }
						    memset(&client_address,0x0,sizeof(client_address));
						}

						if (wifi_softap_set_station_info(m->chaddr, &client_address) == false) {
						    return 0;
						}
//	                }

#if DHCPS_DEBUG
	                os_printf("dhcps: xid changed\n");
	                os_printf("dhcps: client_address.addr = %x\n", client_address.addr);
#endif
	               
//	            }
	            
//	        }
	        return ret;
	    }
        return 0;
}
///////////////////////////////////////////////////////////////////////////////////
/*
 * DHCP ��������ݰ���մ���ص�����˺�����LWIP UDPģ������ʱ������
 * ��Ҫ����udp_recv()������LWIP����ע��.
 *
 * @param arg
 * @param pcb ���յ�UDP��Ŀ��ƿ�?
 * @param p ���յ���UDP�е��������?
 * @param addr ���ʹ�UDP���Դ�����IP��ַ
 * @param port ���ʹ�UDP���Դ�����UDPͨ���˿ں�
 */
///////////////////////////////////////////////////////////////////////////////////
static void handle_dhcp(void *arg,
									struct udp_pcb *pcb, 
									struct pbuf *p, 
									struct ip_addr *addr, 
									u16_t port)
{
		struct dhcps_msg *pmsg_dhcps = NULL;
		s16_t tlen;
        u16_t i;
	    u16_t dhcps_msg_cnt=0;
	    u8_t *p_dhcps_msg = NULL;
	    u8_t *data;

#if DHCPS_DEBUG
    	os_printf("dhcps: handle_dhcp-> receive a packet\n");
#endif
	    if (p==NULL) return;

	    pmsg_dhcps = (struct dhcps_msg *)os_zalloc(sizeof(struct dhcps_msg));
	    if (NULL == pmsg_dhcps){
	    	pbuf_free(p);
	    	return;
	    }
	    p_dhcps_msg = (u8_t *)pmsg_dhcps;
		tlen = p->tot_len;
	    data = p->payload;

#if DHCPS_DEBUG
	    os_printf("dhcps: handle_dhcp-> p->tot_len = %d\n", tlen);
	    os_printf("dhcps: handle_dhcp-> p->len = %d\n", p->len);
#endif		

	    for(i=0; i<p->len; i++){
	        p_dhcps_msg[dhcps_msg_cnt++] = data[i];
#if DHCPS_DEBUG					
			os_printf("%02x ",data[i]);
			if((i+1)%16 == 0){
				os_printf("\n");
			}
#endif
	    }
		
		if(p->next != NULL) {
#if DHCPS_DEBUG
	        os_printf("dhcps: handle_dhcp-> p->next != NULL\n");
	        os_printf("dhcps: handle_dhcp-> p->next->tot_len = %d\n",p->next->tot_len);
	        os_printf("dhcps: handle_dhcp-> p->next->len = %d\n",p->next->len);
#endif
			
	        data = p->next->payload;
	        for(i=0; i<p->next->len; i++){
	            p_dhcps_msg[dhcps_msg_cnt++] = data[i];
#if DHCPS_DEBUG					
				os_printf("%02x ",data[i]);
				if((i+1)%16 == 0){
					os_printf("\n");
				}
#endif
			}
		}

		/*
	     * DHCP �ͻ���������Ϣ����
	    */
#if DHCPS_DEBUG
    	os_printf("dhcps: handle_dhcp-> parse_msg(p)\n");
#endif
		
        switch(parse_msg(pmsg_dhcps, tlen - 240)) {

	        case DHCPS_STATE_OFFER://1
#if DHCPS_DEBUG            
            	 os_printf("dhcps: handle_dhcp-> DHCPD_STATE_OFFER\n");
#endif			
	             send_offer(pmsg_dhcps);
	             break;
	        case DHCPS_STATE_ACK://3
#if DHCPS_DEBUG
            	 os_printf("dhcps: handle_dhcp-> DHCPD_STATE_ACK\n");
#endif			
	             send_ack(pmsg_dhcps);
	             break;
	        case DHCPS_STATE_NAK://4
#if DHCPS_DEBUG            
            	 os_printf("dhcps: handle_dhcp-> DHCPD_STATE_NAK\n");
#endif
	             send_nak(pmsg_dhcps);
	             break;
			default :
				 break;
        }
#if DHCPS_DEBUG
    	os_printf("dhcps: handle_dhcp-> pbuf_free(p)\n");
#endif
        pbuf_free(p);
        os_free(pmsg_dhcps);
        pmsg_dhcps = NULL;
}
///////////////////////////////////////////////////////////////////////////////////
static void wifi_softap_init_dhcps_lease(u32_t ip)
{
	u32_t softap_ip = 0,local_ip = 0;
	u32_t start_ip = 0;
	u32_t end_ip = 0;
	if (dhcps_lease.enable == TRUE) {
		softap_ip = htonl(ip);
		start_ip = htonl(dhcps_lease.start_ip.addr);
		end_ip = htonl(dhcps_lease.end_ip.addr);
		/*config ip information can't contain local ip*/
		if ((start_ip <= softap_ip) && (softap_ip <= end_ip)) {
			dhcps_lease.enable = FALSE;
		} else {
			/*config ip information must be in the same segment as the local ip*/
			softap_ip >>= 8;
			if (((start_ip >> 8 != softap_ip) || (end_ip >> 8 != softap_ip))
					|| (end_ip - start_ip > DHCPS_MAX_LEASE)) {
				dhcps_lease.enable = FALSE;
			}
		}
	}

	if (dhcps_lease.enable == FALSE) {
		local_ip = softap_ip = htonl(ip);
		softap_ip &= 0xFFFFFF00;
		local_ip &= 0xFF;
		if (local_ip >= 0x80)
			local_ip -= DHCPS_MAX_LEASE;
		else
			local_ip ++;

		bzero(&dhcps_lease, sizeof(dhcps_lease));
		dhcps_lease.start_ip.addr = softap_ip | local_ip;
		dhcps_lease.end_ip.addr = softap_ip | (local_ip + DHCPS_MAX_LEASE - 1);
		dhcps_lease.start_ip.addr = htonl(dhcps_lease.start_ip.addr);
		dhcps_lease.end_ip.addr= htonl(dhcps_lease.end_ip.addr);
	}
//	os_printf("start_ip = 0x%x, end_ip = 0x%x\n",dhcps_lease.start_ip, dhcps_lease.end_ip);
}
///////////////////////////////////////////////////////////////////////////////////
void dhcps_start(struct ip_info *info)
{
	struct netif * apnetif = NULL;
	apnetif = (struct netif *)wifi_get_netif(SOFTAP_IF);
    
	if(apnetif->dhcps_pcb != NULL) {
        udp_remove(apnetif->dhcps_pcb);
    }

	pcb_dhcps = udp_new();
	if (pcb_dhcps == NULL || info ==NULL) {
		os_printf("dhcps_start(): could not obtain pcb\n");
	}
	apnetif->dhcps_pcb = pcb_dhcps;

	IP4_ADDR(&broadcast_dhcps, 255, 255, 255, 255);

	server_address = info->ip;
	wifi_softap_init_dhcps_lease(server_address.addr);
	client_address_plus.addr = dhcps_lease.start_ip.addr;
	udp_bind(pcb_dhcps, IP_ADDR_ANY, DHCPS_SERVER_PORT);
	udp_recv(pcb_dhcps, handle_dhcp, NULL);
#if DHCPS_DEBUG
	os_printf("dhcps:dhcps_start->udp_recv function Set a receive callback handle_dhcp for UDP_PCB pcb_dhcps\n");
#endif
		
}

void dhcps_stop(void)
{
	struct netif * apnetif = (struct netif *)wifi_get_netif(0x01);

	udp_disconnect(pcb_dhcps);
//	dhcps_lease_flag = true;
    if(apnetif->dhcps_pcb != NULL) {
        udp_remove(apnetif->dhcps_pcb);
        apnetif->dhcps_pcb = NULL;
    }

	//udp_remove(pcb_dhcps);
	list_node *pnode = NULL;
	list_node *pback_node = NULL;
	pnode = plist;
	while (pnode != NULL) {
		pback_node = pnode;
		pnode = pback_node->pnext;
		node_remove_from_list(&plist, pback_node);
		os_free(pback_node->pnode);
		pback_node->pnode = NULL;
		os_free(pback_node);
		pback_node = NULL;
	}
}

bool wifi_softap_set_dhcps_lease(struct dhcps_lease *please)
{
	struct ip_info info;
	u32_t softap_ip = 0;
	u32_t start_ip = 0;
	u32_t end_ip = 0;

	uint8 opmode = wifi_get_opmode();

	if (opmode == STATION_MODE || opmode == NULL_MODE) {
		return false;
	}

	if (please == NULL || wifi_softap_dhcps_status() == DHCP_STARTED)
		return false;

	if(please->enable) {
    	bzero(&info, sizeof(struct ip_info));
    	wifi_get_ip_info(SOFTAP_IF, &info);
    	softap_ip = htonl(info.ip.addr);
    	start_ip = htonl(please->start_ip.addr);
    	end_ip = htonl(please->end_ip.addr);

    	/*config ip information can't contain local ip*/
    	if ((start_ip <= softap_ip) && (softap_ip <= end_ip))
    		return false;

    	/*config ip information must be in the same segment as the local ip*/
    	softap_ip >>= 8;
    	if ((start_ip >> 8 != softap_ip)
    			|| (end_ip >> 8 != softap_ip)) {
    		return false;
    	}

    	if (end_ip - start_ip > DHCPS_MAX_LEASE)
    		return false;

	    bzero(&dhcps_lease, sizeof(dhcps_lease));
//		dhcps_lease.start_ip.addr = start_ip;
//		dhcps_lease.end_ip.addr = end_ip;
		dhcps_lease.start_ip.addr = please->start_ip.addr;
		dhcps_lease.end_ip.addr = please->end_ip.addr;
	}
	dhcps_lease.enable = please->enable;
//	dhcps_lease_flag = false;
	return true;
}

/******************************************************************************
 * FunctionName : wifi_softap_get_dhcps_lease
 * Description  : get the lease information of DHCP server
 * Parameters   : please -- Additional argument to get the lease information,
 * 							Little-Endian.
 * Returns      : true or false
*******************************************************************************/
bool wifi_softap_get_dhcps_lease(struct dhcps_lease *please)
{
	u8_t opmode = wifi_get_opmode();

	if (opmode == STATION_MODE || opmode == NULL_MODE) {
		return false;
	}

	if (NULL == please)
		return false;
	if (dhcps_lease.enable == FALSE){
		if (wifi_softap_dhcps_status() == DHCP_STOPPED)
			return false;
	} else {
	}
	please->start_ip.addr = dhcps_lease.start_ip.addr;
	please->end_ip.addr = dhcps_lease.end_ip.addr;
	return true;
}

static void kill_oldest_dhcps_pool(void)
{
	list_node *pre = NULL, *p = NULL;
	list_node *minpre = NULL, *minp = NULL;
	struct dhcps_pool *pdhcps_pool = NULL, *pmin_pool = NULL;
	pre = plist;
	p = pre->pnext;
	minpre = pre;
	minp = p;
	while (p != NULL){
		pdhcps_pool = p->pnode;
		pmin_pool = minp->pnode;
		if (pdhcps_pool->lease_timer < pmin_pool->lease_timer){
			minp = p;
			minpre = pre;
		}
		pre = p;
		p = p->pnext;
	}
	minpre->pnext = minp->pnext;
	os_free(minp->pnode);
	minp->pnode = NULL;
	os_free(minp);
	minp = NULL;
}

void dhcps_coarse_tmr(void)
{
	u8_t num_dhcps_pool = 0;
	list_node *pback_node = NULL;
	list_node *pnode = NULL;
	struct dhcps_pool *pdhcps_pool = NULL;
	pnode = plist;
	while (pnode != NULL) {
		pdhcps_pool = pnode->pnode;
		pdhcps_pool->lease_timer --;
		if (pdhcps_pool->lease_timer == 0){
			pback_node = pnode;
			pnode = pback_node->pnext;
			node_remove_from_list(&plist,pback_node);
			os_free(pback_node->pnode);
			pback_node->pnode = NULL;
			os_free(pback_node);
			pback_node = NULL;
		} else {
			pnode = pnode ->pnext;
			num_dhcps_pool ++;
		}
	}

	if (num_dhcps_pool >= MAX_STATION_NUM)
		kill_oldest_dhcps_pool();
}

bool wifi_softap_set_dhcps_offer_option(u8_t level, void* optarg)
{
	bool offer_flag = true;
	u8_t option = 0;
	if (optarg == NULL && wifi_softap_dhcps_status() == false)
		return false;

	if (level <= OFFER_START || level >= OFFER_END)
		return false;

	switch (level){
		case OFFER_ROUTER:
			offer = (*(uint8 *)optarg) & 0x01;
			offer_flag = true;
			break;
		default :
			offer_flag = false;
			break;
	}
	return offer_flag;
}

bool wifi_softap_set_dhcps_lease_time(u32_t minute)
{
    u8_t opmode = wifi_get_opmode();

    if (opmode == STATION_MODE || opmode == NULL_MODE) {
        return false;
    }

    if (wifi_softap_dhcps_status() == DHCP_STARTED) {
        return false;
    }

    if(minute == 0) {
        return false;
    }
    dhcps_lease_time = minute;
    return true;
}

bool wifi_softap_reset_dhcps_lease_time(void)
{
    u8_t opmode = wifi_get_opmode();

    if (opmode == STATION_MODE || opmode == NULL_MODE) {
        return false;
    }

    if (wifi_softap_dhcps_status() == DHCP_STARTED) {
        return false;
    }
    dhcps_lease_time = DHCPS_LEASE_TIME_DEF;
    return true;
}

u32_t wifi_softap_get_dhcps_lease_time(void) // minute
{
    return dhcps_lease_time;
}
