/*
 *
 * (C) 2013-19 - ntop.org
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 */

#include "ntop_includes.h"

#ifndef HAVE_NEDGE

/* **************************************************** */

/* IMPORTANT: keep it in sync with flow_fields_description part of flow_utils.lua */
ZMQParserInterface::ZMQParserInterface(const char *endpoint, const char *custom_interface_type) : ParserInterface(endpoint, custom_interface_type) {
  zmq_initial_bytes = 0, zmq_initial_pkts = 0;
  zmq_remote_stats = zmq_remote_stats_shadow = NULL;
  zmq_remote_initial_exported_flows = 0;
  once = false;
#ifdef NTOPNG_PRO
  custom_app_maps = NULL;
#endif
  num_companion_interfaces = 0;
  companion_interfaces = new (std::nothrow) NetworkInterface*[MAX_NUM_COMPANION_INTERFACES]();

  /* Populate defaults for @NTOPNG@ nProbe templates. No need to populate
     all the fields as nProbe will sent them periodically.

  This minimum set is required for backward compatibility. */
  addMapping("IN_SRC_MAC", IN_SRC_MAC);
  addMapping("OUT_SRC_MAC", OUT_SRC_MAC);
  addMapping("IN_DST_MAC", IN_DST_MAC);
  addMapping("OUT_DST_MAC", OUT_DST_MAC);
  addMapping("SRC_VLAN", SRC_VLAN);
  addMapping("DST_VLAN", DST_VLAN);
  addMapping("DOT1Q_SRC_VLAN", DOT1Q_SRC_VLAN);
  addMapping("DOT1Q_DST_VLAN", DOT1Q_DST_VLAN);
  addMapping("INPUT_SNMP", INPUT_SNMP);
  addMapping("OUTPUT_SNMP", OUTPUT_SNMP);
  addMapping("IPV4_SRC_ADDR", IPV4_SRC_ADDR);
  addMapping("IPV4_DST_ADDR", IPV4_DST_ADDR);
  addMapping("SRC_TOS", SRC_TOS);
  addMapping("DST_TOS", DST_TOS);
  addMapping("L4_SRC_PORT", L4_SRC_PORT);
  addMapping("L4_DST_PORT", L4_DST_PORT);
  addMapping("IPV6_SRC_ADDR", IPV6_SRC_ADDR);
  addMapping("IPV6_DST_ADDR", IPV6_DST_ADDR);
  addMapping("IP_PROTOCOL_VERSION", IP_PROTOCOL_VERSION);
  addMapping("PROTOCOL", PROTOCOL);
  addMapping("L7_PROTO", L7_PROTO, NTOP_PEN);
  addMapping("L7_PROTO_NAME", L7_PROTO_NAME, NTOP_PEN);
  addMapping("IN_BYTES", IN_BYTES);
  addMapping("IN_PKTS", IN_PKTS);
  addMapping("OUT_BYTES", OUT_BYTES);
  addMapping("OUT_PKTS", OUT_PKTS);
  addMapping("FIRST_SWITCHED", FIRST_SWITCHED);
  addMapping("LAST_SWITCHED", LAST_SWITCHED);
  addMapping("EXPORTER_IPV4_ADDRESS", EXPORTER_IPV4_ADDRESS);
  addMapping("EXPORTER_IPV6_ADDRESS", EXPORTER_IPV6_ADDRESS);
  addMapping("NPROBE_IPV4_ADDRESS", NPROBE_IPV4_ADDRESS, NTOP_PEN);
  addMapping("TCP_FLAGS", TCP_FLAGS);
  addMapping("INITIATOR_PKTS", INITIATOR_PKTS);
  addMapping("INITIATOR_OCTETS", INITIATOR_OCTETS);
  addMapping("RESPONDER_PKTS", RESPONDER_PKTS);
  addMapping("RESPONDER_OCTETS", RESPONDER_OCTETS);
  addMapping("SAMPLING_INTERVAL", SAMPLING_INTERVAL);
  addMapping("DIRECTION", DIRECTION);
  addMapping("POST_NAT_SRC_IPV4_ADDR", POST_NAT_SRC_IPV4_ADDR);
  addMapping("POST_NAT_DST_IPV4_ADDR", POST_NAT_DST_IPV4_ADDR);
  addMapping("POST_NAPT_SRC_TRANSPORT_PORT", POST_NAPT_SRC_TRANSPORT_PORT);
  addMapping("POST_NAPT_DST_TRANSPORT_PORT", POST_NAPT_DST_TRANSPORT_PORT);
  addMapping("INGRESS_VRFID", INGRESS_VRFID);
  addMapping("IPV4_SRC_MASK", IPV4_SRC_MASK);
  addMapping("IPV4_DST_MASK", IPV4_DST_MASK);
  addMapping("IPV4_NEXT_HOP", IPV4_NEXT_HOP);
  addMapping("OOORDER_IN_PKTS", OOORDER_IN_PKTS, NTOP_PEN);
  addMapping("OOORDER_OUT_PKTS", OOORDER_OUT_PKTS, NTOP_PEN);
  addMapping("RETRANSMITTED_IN_PKTS", RETRANSMITTED_IN_PKTS, NTOP_PEN);
  addMapping("RETRANSMITTED_OUT_PKTS", RETRANSMITTED_OUT_PKTS, NTOP_PEN);
  addMapping("DNS_QUERY", DNS_QUERY, NTOP_PEN);
  addMapping("DNS_QUERY_TYPE", DNS_QUERY_TYPE, NTOP_PEN);
  addMapping("DNS_RET_CODE", DNS_RET_CODE, NTOP_PEN);
  addMapping("HTTP_URL", HTTP_URL, NTOP_PEN);
  addMapping("HTTP_SITE", HTTP_SITE, NTOP_PEN);
  addMapping("HTTP_RET_CODE", HTTP_RET_CODE, NTOP_PEN);
  addMapping("SSL_SERVER_NAME", SSL_SERVER_NAME, NTOP_PEN);
  addMapping("SSL_CIPHER", SSL__CIPHER, NTOP_PEN);
  addMapping("SSL_UNSAFE_CIPHER", SSL_UNSAFE_CIPHER, NTOP_PEN);
  addMapping("JA3C_HASH", JA3C_HASH, NTOP_PEN);
  addMapping("JA3S_HASH", JA3S_HASH, NTOP_PEN);
  addMapping("BITTORRENT_HASH", BITTORRENT_HASH, NTOP_PEN);
  addMapping("SRC_FRAGMENTS", SRC_FRAGMENTS, NTOP_PEN);
  addMapping("DST_FRAGMENTS", DST_FRAGMENTS, NTOP_PEN);
  addMapping("CLIENT_NW_LATENCY_MS", CLIENT_NW_LATENCY_MS, NTOP_PEN);
  addMapping("SERVER_NW_LATENCY_MS", SERVER_NW_LATENCY_MS, NTOP_PEN);
}

/* **************************************************** */

ZMQParserInterface::~ZMQParserInterface() {
  if(zmq_remote_stats)        free(zmq_remote_stats);
  if(zmq_remote_stats_shadow) free(zmq_remote_stats_shadow);
#ifdef NTOPNG_PRO
  if(custom_app_maps)         delete(custom_app_maps);
#endif
  if(companion_interfaces)
    delete []companion_interfaces;
}

/* **************************************************** */

void ZMQParserInterface::reloadCompanions() {
  char key[CONST_MAX_LEN_REDIS_KEY];
  int num_companions;
  char **companions = NULL;
  bool found;

  if(!ntop->getRedis()) return;

  snprintf(key, sizeof(key), CONST_IFACE_COMPANIONS_SET, get_id());
  num_companions = ntop->getRedis()->smembers(key, &companions);

  companions_lock.lock(__FILE__, __LINE__);

  if(num_companion_interfaces > 0) {
    /* Check and possibly remove old companions */
    for(int i = 0; i < MAX_NUM_COMPANION_INTERFACES; i++) {
      if(!companion_interfaces[i]) continue;

      found = false;
      for(int j = 0; j < num_companions; j++) {
	if(companion_interfaces[i]->get_id() == atoi(companions[j])) {
	  found = true;
	  break;
	}
      }

      if(!found) {
	// ntop->getTrace()->traceEvent(TRACE_NORMAL, "Removed companion interface [interface: %s][companion: %s]",
	// 			     get_name(), companion_interfaces[i]->get_name());
	companion_interfaces[i] = NULL;
	num_companion_interfaces--;
      }
    }
  }

  if(num_companions > 0) {
    /* Check and possibly add new companions */
    for(int i = 0; i < num_companions; i++) {
      found = false;
      for(int j = 0; j < MAX_NUM_COMPANION_INTERFACES; j++) {
	if(companion_interfaces[j] && companion_interfaces[j]->get_id() == atoi(companions[i])) {
	  found = true;
	  break;
	}
      }

      if(!found) {
	if(num_companion_interfaces < MAX_NUM_COMPANION_INTERFACES) {
	  for(int j = 0; j < MAX_NUM_COMPANION_INTERFACES; j++) {
	    if(!companion_interfaces[j]) {
	      companion_interfaces[j] = ntop->getInterfaceById(atoi(companions[i]));

	      if(companion_interfaces[j]) {
		num_companion_interfaces++;
		// ntop->getTrace()->traceEvent(TRACE_NORMAL, "Added new companion interface [interface: %s][companion: %s]",
		// 			     get_name(), companion_interfaces[j]->get_name());
	      }

	      break;
	    }
	  }
	} else
	  ntop->getTrace()->traceEvent(TRACE_ERROR, "Too many companion interfaces defined [interface: %s]", get_name());
      }

      free(companions[i]);
    }
  }

  companions_lock.unlock(__FILE__, __LINE__);

  if(companions)
    free(companions);

  // ntop->getTrace()->traceEvent(TRACE_NORMAL, "Companion interface reloaded [interface: %s][companion: %s]",
  // 			       get_name(), companion_interface ? companion_interface->get_name() : "NULL");
}

/* **************************************************** */

void ZMQParserInterface::addMapping(const char *sym, u_int32_t num, u_int32_t pen) {
  string label(sym);
  labels_map_t::iterator it;

  if((it = labels_map.find(label)) == labels_map.end())
    labels_map.insert(make_pair(label, make_pair(pen, num)));
  else
    it->second.first = pen, it->second.second = num;
}

/* **************************************************** */

bool ZMQParserInterface::getKeyId(char *sym, u_int32_t * const pen, u_int32_t * const field) const {
  u_int32_t cur_pen, cur_field;
  string label(sym);
  labels_map_t::const_iterator it;

  *pen = UNKNOWN_PEN, *field = UNKNOWN_FLOW_ELEMENT;

  if(sscanf(sym, "%u.%u", &cur_pen, &cur_field) == 2)
    *pen = cur_pen, *field = cur_field;
  else if(sscanf(sym, "%u", &cur_field) == 1)
    *pen = 0, *field = cur_field;
  else if((it = labels_map.find(label)) != labels_map.end())
    *pen = it->second.first, *field = it->second.second;
  else
    return false;

  return true;
}

/* **************************************************** */

u_int8_t ZMQParserInterface::parseEvent(const char * const payload, int payload_size,
				     u_int8_t source_id, void *data) {
  json_object *o;
  enum json_tokener_error jerr = json_tokener_success;
  ZMQ_RemoteStats *zrs = NULL;

  // payload[payload_size] = '\0';

  // ntop->getTrace()->traceEvent(TRACE_NORMAL, "%s", payload);
  o = json_tokener_parse_verbose(payload, &jerr);

  if(o && (zrs = (ZMQ_RemoteStats*)calloc(1, sizeof(ZMQ_RemoteStats)))) {
    json_object *w, *z;

    if(json_object_object_get_ex(o, "time", &w))    zrs->remote_time  = (u_int32_t)json_object_get_int64(w);
    if(json_object_object_get_ex(o, "bytes", &w))   zrs->remote_bytes = (u_int64_t)json_object_get_int64(w);
    if(json_object_object_get_ex(o, "packets", &w)) zrs->remote_pkts  = (u_int64_t)json_object_get_int64(w);

    if(json_object_object_get_ex(o, "iface", &w)) {
      if(json_object_object_get_ex(w, "name", &z))
	snprintf(zrs->remote_ifname, sizeof(zrs->remote_ifname), "%s", json_object_get_string(z));
      if(json_object_object_get_ex(w, "speed", &z))
	zrs->remote_ifspeed = (u_int32_t)json_object_get_int64(z);
      if(json_object_object_get_ex(w, "ip", &z))
	snprintf(zrs->remote_ifaddress, sizeof(zrs->remote_ifaddress), "%s", json_object_get_string(z));
    }

    if(json_object_object_get_ex(o, "probe", &w)) {
      if(json_object_object_get_ex(w, "public_ip", &z))
	snprintf(zrs->remote_probe_public_address, sizeof(zrs->remote_probe_public_address), "%s", json_object_get_string(z));
      if(json_object_object_get_ex(w, "ip", &z))
	snprintf(zrs->remote_probe_address, sizeof(zrs->remote_probe_address), "%s", json_object_get_string(z));
    }

    if(json_object_object_get_ex(o, "avg", &w)) {
      if(json_object_object_get_ex(w, "bps", &z))
	zrs->avg_bps = (u_int32_t)json_object_get_int64(z);
      if(json_object_object_get_ex(w, "pps", &z))
	zrs->avg_pps = (u_int32_t)json_object_get_int64(z);
    }

    if(json_object_object_get_ex(o, "timeout", &w)) {
      if(json_object_object_get_ex(w, "lifetime", &z))
	zrs->remote_lifetime_timeout = (u_int32_t)json_object_get_int64(z);
      if(json_object_object_get_ex(w, "idle", &z))
	zrs->remote_idle_timeout = (u_int32_t)json_object_get_int64(z);
    }

    if(json_object_object_get_ex(o, "drops", &w)) {
      if(json_object_object_get_ex(w, "export_queue_full", &z))
	zrs->export_queue_full = (u_int32_t)json_object_get_int64(z);

      if(json_object_object_get_ex(w, "too_many_flows", &z))
	zrs->too_many_flows = (u_int32_t)json_object_get_int64(z);

      if(json_object_object_get_ex(w, "elk_flow_drops", &z))
	zrs->elk_flow_drops = (u_int32_t)json_object_get_int64(z);

      if(json_object_object_get_ex(w, "sflow_pkt_sample_drops", &z))
	zrs->sflow_pkt_sample_drops = (u_int32_t)json_object_get_int64(z);

      if(json_object_object_get_ex(w, "flow_collection_drops", &z))
	zrs->flow_collection_drops = (u_int32_t)json_object_get_int64(z);
    }

    if(json_object_object_get_ex(o, "zmq", &w)) {
      if(json_object_object_get_ex(w, "num_flow_exports", &z))
	zrs->num_flow_exports = (u_int64_t)json_object_get_int64(z);

      if(json_object_object_get_ex(w, "num_exporters", &z))
	zrs->num_exporters = (u_int8_t)json_object_get_int(z);
    }

#ifdef ZMQ_EVENT_DEBUG
    ntop->getTrace()->traceEvent(TRACE_NORMAL, "Event parsed "
				 "[iface: {name: %s, speed: %u, ip:%s}]"
				 "[probe: {public_ip: %s, ip: %s}]"
				 "[avg: {bps: %u, pps: %u}]"
				 "[remote: {time: %u, bytes: %u, packets: %u, idle_timeout: %u, lifetime_timeout:%u}]"
				 "[zmq: {num_exporters: %u, num_flow_exports: %u}]",
				 zrs->remote_ifname, zrs->remote_ifspeed, zrs->remote_ifaddress,
				 zrs->remote_probe_public_address, zrs->remote_probe_address,
				 zrs->avg_bps, zrs->avg_pps,
				 zrs->remote_time, (u_int32_t)zrs->remote_bytes, (u_int32_t)zrs->remote_pkts,
				 zrs->remote_idle_timeout, zrs->remote_lifetime_timeout,
				 zrs->num_exporters, zrs->num_flow_exports);
#endif

    /* ntop->getTrace()->traceEvent(TRACE_WARNING, "%u/%u", avg_bps, avg_pps); */

    /* Process Flow */
    setRemoteStats(zrs);

    if(flowHashing) {
      FlowHashing *current, *tmp;
      ZMQParserInterface *current_iface;

      HASH_ITER(hh, flowHashing, current, tmp) {
	if((current_iface = dynamic_cast<ZMQParserInterface*>(current->iface))) {
          ZMQ_RemoteStats *zrscopy = (ZMQ_RemoteStats*)malloc(sizeof(ZMQ_RemoteStats));

	  if(zrscopy) {
	    memcpy(zrscopy, zrs, sizeof(ZMQ_RemoteStats));
	    current_iface->setRemoteStats(zrscopy);
          }
        }
      }
    }

    /* Dispose memory */
    json_object_put(o);
  } else {
    // if o != NULL
    if(!once) {
      ntop->getTrace()->traceEvent(TRACE_WARNING,
				   "Invalid message received: "
				   "your nProbe sender is outdated, data encrypted, invalid JSON, or oom?");
      ntop->getTrace()->traceEvent(TRACE_WARNING, "JSON Parse error [%s] payload size: %u payload: %s",
				   json_tokener_error_desc(jerr),
				   payload_size,
				   payload);
    }
    once = true;
    if(o) json_object_put(o);
    return -1;
  }

  return 0;
}

/* **************************************************** */

bool ZMQParserInterface::parsePENZeroField(ParsedFlow * const flow, u_int32_t field, ParsedValue *value) const {
  IpAddress ip_aux; /* used to check empty IPs */

  switch(field) {
  case IN_SRC_MAC:
  case OUT_SRC_MAC:
    /* Format 00:00:00:00:00:00 */
    Utils::parseMac(flow->src_mac, value->string);
    break;
  case IN_DST_MAC:
  case OUT_DST_MAC:
    Utils::parseMac(flow->dst_mac, value->string);
    break;
  case SRC_TOS:
    flow->src_tos = value->uint_num;
    break;
  case DST_TOS:
    flow->dst_tos = value->uint_num;
    break;
  case IPV4_SRC_ADDR:
  case IPV6_SRC_ADDR:
    /*
      The following check prevents an empty ip address (e.g., ::) to
      to overwrite another valid ip address already set.
      This can happen for example when nProbe is configured (-T) to export
      both %IPV4_SRC_ADDR and the %IPV6_SRC_ADDR. In that cases nProbe can
      export a valid ipv4 and an empty ipv6. Without the check, the empty
      v6 address may overwrite the non empty v4.
    */
    if(flow->src_ip.isEmpty()) {
      flow->src_ip.set((char *) value->string);
    } else {
      ip_aux.set((char *) value->string);
      if(!ip_aux.isEmpty()  && !ntop->getPrefs()->do_override_src_with_post_nat_src())
	/* tried to overwrite a non-empty IP with another non-empty IP */
	ntop->getTrace()->traceEvent(TRACE_WARNING,
				     "Attempt to set source ip multiple times. "
				     "Check exported fields");
    }
    break;
  case IP_PROTOCOL_VERSION:
    flow->version = value->uint_num;
    break;

  case IPV4_DST_ADDR:
  case IPV6_DST_ADDR:
    if(flow->dst_ip.isEmpty()) {
      flow->dst_ip.set((char *) value->string);
    } else {
      ip_aux.set((char *) value->string);
      if(!ip_aux.isEmpty()  && !ntop->getPrefs()->do_override_dst_with_post_nat_dst())
	ntop->getTrace()->traceEvent(TRACE_WARNING,
				     "Attempt to set destination ip multiple times. "
				     "Check exported fields");
    }
    break;
  case L4_SRC_PORT:
    if(!flow->src_port)
      flow->src_port = htons((u_int32_t) value->uint_num);
    break;
  case L4_DST_PORT:
    if(!flow->dst_port)
      flow->dst_port = htons((u_int32_t) value->uint_num);
    break;
  case SRC_VLAN:
  case DST_VLAN:
    flow->vlan_id = value->uint_num;
    break;
  case DOT1Q_SRC_VLAN:
  case DOT1Q_DST_VLAN:
    if (flow->vlan_id == 0) {
      /* as those fields are the outer vlans in q-in-q
	 we set the vlan_id only if there is no inner vlan
	 value set
      */
      flow->vlan_id = value->uint_num;
    }
    break;
  case PROTOCOL:
    flow->l4_proto = value->uint_num;
    break;
  case TCP_FLAGS:
    flow->tcp.tcp_flags = value->uint_num;
    break;
  case INITIATOR_PKTS:
    flow->absolute_packet_octet_counters = true;
    /* Don't break */
  case IN_PKTS:
    flow->in_pkts = value->uint_num;
    break;
  case INITIATOR_OCTETS:
    flow->absolute_packet_octet_counters = true;
    /* Don't break */
  case IN_BYTES:
    flow->in_bytes = value->uint_num;
    break;
  case RESPONDER_PKTS:
    flow->absolute_packet_octet_counters = true;
    /* Don't break */
  case OUT_PKTS:
    flow->out_pkts = value->uint_num;
    break;
  case RESPONDER_OCTETS:
    flow->absolute_packet_octet_counters = true;
    /* Don't break */
  case OUT_BYTES:
    flow->out_bytes = value->uint_num;
    break;
  case FIRST_SWITCHED:
    if (value->string != NULL)
      flow->first_switched = atoi(value->string);
    else
      flow->first_switched = value->uint_num;
    break;
  case LAST_SWITCHED:
    if (value->string != NULL)
      flow->last_switched = atoi(value->string);
    else
      flow->last_switched = value->uint_num;
    break;
  case SAMPLING_INTERVAL:
    flow->pkt_sampling_rate = value->uint_num;
    break;
  case DIRECTION:
    if (value->string != NULL) 
      flow->direction = atoi(value->string);
    else     
      flow->direction = value->uint_num;
    break;
  case EXPORTER_IPV4_ADDRESS:
    /* Format: a.b.c.d, possibly overrides NPROBE_IPV4_ADDRESS */
    if(ntohl(inet_addr(value->string)) && (flow->deviceIP = ntohl(inet_addr(value->string))))
      return false;
    break;
  case INPUT_SNMP:
    flow->inIndex = value->uint_num;
    break;
  case OUTPUT_SNMP:
    flow->outIndex = value->uint_num;
    break;
  case POST_NAT_SRC_IPV4_ADDR:
    if(ntop->getPrefs()->do_override_src_with_post_nat_src())
      flow->src_ip.set((char *) value->string);
    break;
  case POST_NAT_DST_IPV4_ADDR:
    if(ntop->getPrefs()->do_override_dst_with_post_nat_dst())
      flow->dst_ip.set((char *) value->string);
    break;
  case POST_NAPT_SRC_TRANSPORT_PORT:
    if(ntop->getPrefs()->do_override_src_with_post_nat_src())
      flow->src_port = htons((u_int16_t) value->uint_num);
    break;
  case POST_NAPT_DST_TRANSPORT_PORT:
    if(ntop->getPrefs()->do_override_dst_with_post_nat_dst())
      flow->dst_port = htons((u_int16_t) value->uint_num);
    break;
  case INGRESS_VRFID:
    flow->vrfId = value->uint_num;
    break;
  case IPV4_SRC_MASK:
  case IPV4_DST_MASK:
    if(value->uint_num != 0)
      return false;
    break;
  case IPV4_NEXT_HOP:
    if(strcmp(value->string, "0.0.0.0"))
      return false;
    break;
  default:
    return false;
  }

  return true;
}

/* **************************************************** */

bool ZMQParserInterface::parsePENNtopField(ParsedFlow * const flow, u_int32_t field, ParsedValue *value) const {
 
  /* Check for backward compatibility to handle cases like field = 123 (CLIENT_NW_LATENCY_MS)
   * instead of field = 57595 (NTOP_BASE_ID + 123) */ 
  if (field < NTOP_BASE_ID)
    field += NTOP_BASE_ID;

  switch(field) {
  case L7_PROTO:
    if (value->string) {
      if(!strchr(value->string, '.')) {
        /* Old behaviour, only the app protocol */
        flow->l7_proto.app_protocol = atoi(value->string);
      } else {
        char *proto_dot;

        flow->l7_proto.master_protocol = (u_int16_t)strtoll(value->string, &proto_dot, 10);
        flow->l7_proto.app_protocol    = (u_int16_t)strtoll(proto_dot + 1, NULL, 10);
      }
    } else {
      flow->l7_proto.app_protocol = value->uint_num;
    }
#if 0
    ntop->getTrace()->traceEvent(TRACE_NORMAL, "[value: %s][master: %u][app: %u]",
				 value->string ? value->string : "(int)",
				 flow->l7_proto.master_protocol,
				 flow->l7_proto.app_protocol);
#endif
    break;
  case L7_PROTO_NAME:
    break;
  case OOORDER_IN_PKTS:
    flow->tcp.ooo_in_pkts = value->uint_num;
    break;
  case OOORDER_OUT_PKTS:
    flow->tcp.ooo_out_pkts = value->uint_num;
    break;
  case RETRANSMITTED_IN_PKTS:
    flow->tcp.retr_in_pkts = value->uint_num;
    break;
  case RETRANSMITTED_OUT_PKTS:
    flow->tcp.retr_out_pkts = value->uint_num;
    break;
    /* TODO add lost in/out to nProbe and here */
  case CLIENT_NW_LATENCY_MS:
    {
      float client_nw_latency;
      client_nw_latency = value->double_num; 
      flow->tcp.clientNwLatency.tv_sec = client_nw_latency / 1e3;
      flow->tcp.clientNwLatency.tv_usec = 1e3 * (client_nw_latency - flow->tcp.clientNwLatency.tv_sec * 1e3);
      break;
    }
  case SERVER_NW_LATENCY_MS:
    {
      float server_nw_latency;
      server_nw_latency = value->double_num;
      flow->tcp.serverNwLatency.tv_sec = server_nw_latency / 1e3;
      flow->tcp.serverNwLatency.tv_usec = 1e3 * (server_nw_latency - flow->tcp.serverNwLatency.tv_sec * 1e3);
      break;
    }
  case CLIENT_TCP_FLAGS:
    flow->tcp.client_tcp_flags = value->uint_num;
    break;
  case SERVER_TCP_FLAGS:
    flow->tcp.server_tcp_flags = value->uint_num;
    break;
  case APPL_LATENCY_MS:
    flow->tcp.applLatencyMsec = value->double_num;
    break;
  case DNS_QUERY:
    flow->dns_query = (char *) value->string;
    break;
  case DNS_QUERY_TYPE:
    flow->dns_query_type = value->uint_num;
    break;
  case DNS_RET_CODE:
    flow->dns_ret_code = value->uint_num;
    break;
  case HTTP_URL:
    flow->http_url = (char *) value->string;
    break;
  case HTTP_SITE:
    flow->http_site = (char *) value->string;
    break;
  case HTTP_RET_CODE:
    flow->http_ret_code = value->uint_num;
    break;
  case SSL_SERVER_NAME:
    flow->ssl_server_name = (char *) value->string;
    break;
  case JA3C_HASH:
    flow->ja3c_hash = (char *) value->string;
    break;
  case JA3S_HASH:
    flow->ja3s_hash = (char *) value->string;
    break;
  case SSL__CIPHER:
    flow->ssl_cipher = value->uint_num;
    break;
  case SSL_UNSAFE_CIPHER:
    flow->ssl_unsafe_cipher = value->uint_num;
    break;
  case BITTORRENT_HASH:
    flow->bittorrent_hash = (char *) value->string;
    break;
  case NPROBE_IPV4_ADDRESS:
    /* Do not override EXPORTER_IPV4_ADDRESS */
    if(flow->deviceIP == 0 && (flow->deviceIP = ntohl(inet_addr(value->string))))
      return false;
    break;
  case SRC_FRAGMENTS:
    flow->in_fragments = value->uint_num;
    break;
  case DST_FRAGMENTS:
    flow->out_fragments = value->uint_num;
    break;
  default:
    return false;
  }

  return true;
}

/* **************************************************** */

bool ZMQParserInterface::parseNProbeAgentField(ParsedFlow * const flow, const char * const key, ParsedValue *value, json_object * const jvalue) const {
  bool ret = false;
  json_object *obj;

  if(!strncmp(key, "timestamp", 9)) {
    u_int32_t seconds, nanoseconds /* nanoseconds not currently used */;
    if(sscanf(value->string, "%u.%u", &seconds, &nanoseconds) == 2) {
      flow->first_switched = flow->last_switched = seconds;
      ret = true;
    }
  } else if(!strncmp(key, "IPV4_LOCAL_ADDR", 15)
	    || !strncmp(key, "IPV6_LOCAL_ADDR", 15)) {
    flow->src_ip.set(value->string); /* FIX: do not always assume Local == Client */
    ret = true;
  } else if(!strncmp(key, "IPV4_REMOTE_ADDR", 16)
	    || !strncmp(key, "IPV6_REMOTE_ADDR", 16)) {
    flow->dst_ip.set(value->string); /* FIX: do not always assume Remote == Server */
    ret = true;
  } else if(!strncmp(key, "L4_LOCAL_PORT", 13)) {
    flow->src_port = htons((u_int32_t) value->uint_num);
    ret = true;
  } else if(!strncmp(key, "L4_REMOTE_PORT", 14)) {
    flow->dst_port = htons((u_int32_t) value->uint_num);
    ret = true;
  } else if(!strncmp(key, "INTERFACE_NAME", 7) && strlen(key) == 14) {
    flow->ifname = (char*)json_object_get_string(jvalue);
    ret = true;
  } else if(strlen(key) >= 14 && !strncmp(&key[strlen(key) - 14], "FATHER_PROCESS", 14)) {
    if(json_object_object_get_ex(jvalue, "PID", &obj))   flow->process_info.father_pid = (u_int32_t)json_object_get_int64(obj);
    if(json_object_object_get_ex(jvalue, "UID", &obj))      flow->process_info.father_uid = (u_int32_t)json_object_get_int64(obj);
    if(json_object_object_get_ex(jvalue, "UID_NAME", &obj))    flow->process_info.father_uid_name = (char*)json_object_get_string(obj);
    if(json_object_object_get_ex(jvalue, "GID", &obj))     flow->process_info.father_gid = (u_int32_t)json_object_get_int64(obj);
    if(json_object_object_get_ex(jvalue, "VM_SIZE", &obj))     flow->process_info.actual_memory = (u_int32_t)json_object_get_int64(obj);
    if(json_object_object_get_ex(jvalue, "VM_PEAK", &obj))     flow->process_info.peak_memory = (u_int32_t)json_object_get_int64(obj);
    if(json_object_object_get_ex(jvalue, "PROCESS_PATH", &obj)) flow->process_info.father_process_name = (char*)json_object_get_string(obj);
    if(!flow->process_info_set) flow->process_info_set = true;
    ret = true;

    // ntop->getTrace()->traceEvent(TRACE_NORMAL, "Father Process [pid: %u][uid: %u][gid: %u][path: %s]",
    //					 flow->process_info.father_pid, flow->process_info.father_uid,
    //				 flow->process_info.father_gid,
    //				 flow->process_info.father_process_name);
  } else if(strlen(key) >= 7 && !strncmp(&key[strlen(key) - 7], "PROCESS", 7)) {
    if(json_object_object_get_ex(jvalue, "PID", &obj))   flow->process_info.pid = (u_int32_t)json_object_get_int64(obj);
    if(json_object_object_get_ex(jvalue, "UID", &obj))      flow->process_info.uid = (u_int32_t)json_object_get_int64(obj);
    if(json_object_object_get_ex(jvalue, "UID_NAME", &obj))    flow->process_info.uid_name = (char*)json_object_get_string(obj);
    if(json_object_object_get_ex(jvalue, "GID", &obj))     flow->process_info.gid = (u_int32_t)json_object_get_int64(obj);
    if(json_object_object_get_ex(jvalue, "VM_SIZE", &obj))     flow->process_info.actual_memory = (u_int32_t)json_object_get_int64(obj);
    if(json_object_object_get_ex(jvalue, "VM_PEAK", &obj))     flow->process_info.peak_memory = (u_int32_t)json_object_get_int64(obj);
    if(json_object_object_get_ex(jvalue, "PROCESS_PATH", &obj)) flow->process_info.process_name = (char*)json_object_get_string(obj);
    if(!flow->process_info_set) flow->process_info_set = true;
    ret = true;

    // ntop->getTrace()->traceEvent(TRACE_NORMAL, "Process [pid: %u][uid: %u][gid: %u][size/peak vm: %u/%u][path: %s]",
    //				 flow->process_info.pid, flow->process_info.uid, flow->process_info.gid,
    //				 flow->process_info.actual_memory, flow->process_info.peak_memory,
    //				 flow->process_info.process_name);
  } else if(strlen(key) >= 9 && !strncmp(&key[strlen(key) - 9], "CONTAINER", 9)) {
    if((ret = parseContainerInfo(jvalue, &flow->container_info)))
      flow->container_info_set = true;
  } else if(!strncmp(key, "TCP", 3) && strlen(key) == 3) {
    if(json_object_object_get_ex(jvalue, "CONN_STATE", &obj))     flow->tcp_info.conn_state = Utils::tcpStateStr2State(json_object_get_string(obj));

    if(json_object_object_get_ex(jvalue, "SEGS_IN", &obj))        flow->tcp_info.in_segs = (u_int32_t)json_object_get_int64(obj);
    if(json_object_object_get_ex(jvalue, "SEGS_OUT", &obj))       flow->tcp_info.out_segs = (u_int32_t)json_object_get_int64(obj);
    if(json_object_object_get_ex(jvalue, "UNACK_SEGMENTS", &obj)) flow->tcp_info.unacked_segs = (u_int32_t)json_object_get_int64(obj);
    if(json_object_object_get_ex(jvalue, "RETRAN_PKTS", &obj))    flow->tcp_info.retx_pkts = (u_int32_t)json_object_get_int64(obj);
    if(json_object_object_get_ex(jvalue, "LOST_PKTS", &obj))      flow->tcp_info.lost_pkts = (u_int32_t)json_object_get_int64(obj);

    if(json_object_object_get_ex(jvalue, "RTT", &obj))            flow->tcp_info.rtt = json_object_get_double(obj);
    if(json_object_object_get_ex(jvalue, "RTT_VARIANCE", &obj))   flow->tcp_info.rtt_var = json_object_get_double(obj);

    if(json_object_object_get_ex(jvalue, "BYTES_RCVD", &obj))
      flow->out_bytes = flow->tcp_info.rcvd_bytes = (u_int64_t)json_object_get_int64(obj);

    if(json_object_object_get_ex(jvalue, "BYTES_ACKED", &obj))
      flow->in_bytes = flow->tcp_info.sent_bytes = (u_int64_t)json_object_get_int64(obj);

    if(!flow->tcp_info_set) flow->tcp_info_set = true;
    flow->absolute_packet_octet_counters = true;
    ret = true;

    // ntop->getTrace()->traceEvent(TRACE_NORMAL, "TCP INFO [conn state: %s][rcvd_bytes: %u][retx_pkts: %u][lost_pkts: %u]"
    //				 "[in_segs: %u][out_segs: %u][unacked_segs: %u]"
    //				 "[rtt: %f][rtt_var: %f]",
    //				 Utils::tcpState2StateStr(flow->tcp_info.conn_state),
    //				 flow->tcp_info.rcvd_bytes,
    //				 flow->tcp_info.retx_pkts,
    //				 flow->tcp_info.lost_pkts,
    //				 flow->tcp_info.in_segs,
    //				 flow->tcp_info.out_segs,
    //				 flow->tcp_info.unacked_segs,
    //				 flow->tcp_info.rtt,
    //				 flow->tcp_info.rtt_var);
  } else if((!strncmp(key, "TCP_EVENT_TYPE", 14) && strlen(key) == 14)
	    || (!strncmp(key, "UDP_EVENT_TYPE", 14) && strlen(key) == 14)) {
    flow->event_type = Utils::eBPFEventStr2Event(value->string);

    // ntop->getTrace()->traceEvent(TRACE_NORMAL, "Event Type [type: %s]", Utils::eBPFEvent2EventStr(flow->event_type));
  }

  return ret;
}

/* **************************************************** */

void ZMQParserInterface::deliverFlowToCompanions(ParsedFlow * const flow) {
  if(num_companion_interfaces > 0
     && (flow->process_info_set || flow->container_info_set || flow->tcp_info_set)) {
    NetworkInterface *flow_interface = flow->ifname ? ntop->getNetworkInterface(flow->ifname) : NULL;

    for(int i = 0; i < MAX_NUM_COMPANION_INTERFACES; i++) {
      NetworkInterface *cur_companion = companion_interfaces[i];

      if(!cur_companion) continue;

      if(cur_companion->isTrafficMirrored())
	cur_companion->enqueueeBPFFlow(flow, true /* Skip loopback traffic */);
      else if(cur_companion == flow_interface)
	cur_companion->enqueueeBPFFlow(flow, false /* do NOT skip loopback traffic */);
    }
  }
}

/* **************************************************** */

void ZMQParserInterface::preprocessFlow(ParsedFlow *flow, NetworkInterface *iface) {
  bool invalid_flow = false;

  if(flow->vlan_id && ntop->getPrefs()->do_ignore_vlans())
    flow->vlan_id = 0;

  /* Handle zero IPv4/IPv6 discrepacies */
  if(!flow->hasParsedeBPF()) {
    if(flow->version == 0) {
      if(flow->src_ip.getVersion() != flow->dst_ip.getVersion()) {
	if(flow->dst_ip.isIPv4() && flow->src_ip.isIPv6() && flow->src_ip.isEmpty())
	  flow->src_ip.setVersion(4);
	else if(flow->src_ip.isIPv4() && flow->dst_ip.isIPv6() && flow->dst_ip.isEmpty())
	  flow->dst_ip.setVersion(4);
	else if(flow->dst_ip.isIPv6() && flow->src_ip.isIPv4() && flow->src_ip.isEmpty())
	  flow->src_ip.setVersion(6);
	else if(flow->src_ip.isIPv6() && flow->dst_ip.isIPv4() && flow->dst_ip.isEmpty())
	  flow->dst_ip.setVersion(6);
	else {
	  invalid_flow = true;
	  ntop->getTrace()->traceEvent(TRACE_WARNING,
				       "IP version mismatch: client:%d server:%d - flow will be ignored",
				       flow->src_ip.getVersion(), flow->dst_ip.getVersion());
	}
      }
    } else
      flow->src_ip.setVersion(flow->version), flow->dst_ip.setVersion(flow->version);
  }

  if(!invalid_flow) {

    if(flow->hasParsedeBPF()) {
      /* Direction already reliable when the event is an accept or a connect.
         Heuristic is only used in the other cases. */
      if(flow->event_type != ebpf_event_type_tcp_accept
	 && flow->event_type != ebpf_event_type_tcp_connect
	 && ntohs(flow->src_port) < ntohs(flow->dst_port))
	flow->swap();
    } else if(ntop->getPrefs()->do_use_ports_to_determine_src_and_dst()) {
      /* Attempt to determine flow client and server using port numbers 
       useful when exported flows are mono-directional
       https://github.com/ntop/ntopng/issues/1978 */
      if(ntohs(flow->src_port) < ntohs(flow->dst_port))
	flow->swap();
    }

    /* Process Flow */
    PROFILING_SECTION_ENTER("processFlow", 30);
    iface->processFlow(flow, true);
    PROFILING_SECTION_EXIT(30);
    deliverFlowToCompanions(flow);
  }
}

/* **************************************************** */

void ZMQParserInterface::parseSingleJSONFlow(json_object *o,
					     u_int8_t source_id,
					     NetworkInterface *iface) {
  ParsedFlow flow;
  struct json_object_iterator it = json_object_iter_begin(o);
  struct json_object_iterator itEnd = json_object_iter_end(o);

  /* Reset data */
  flow.source_id = source_id;

  while(!json_object_iter_equal(&it, &itEnd)) {
    const char *key     = json_object_iter_peek_name(&it);
    json_object *jvalue = json_object_iter_peek_value(&it);
    json_object *additional_o = NULL;
    enum json_type type = json_object_get_type(jvalue);
    ParsedValue value = { 0 };
    bool add_to_additional_fields = false;

    switch(type) {
    case json_type_int:
      value.uint_num = json_object_get_int64(jvalue);
      value.double_num = value.uint_num;
      break;
    case json_type_double:
      value.double_num = json_object_get_double(jvalue);
      break;
    case json_type_string:
      value.string = json_object_get_string(jvalue);
      if (strcmp(key,"json") == 0)
	additional_o = json_tokener_parse(value.string);
      break;
    case json_type_object:
      /* This is handled by parseNProbeAgentField or addAdditionalField */
      break;
	
    default:
      ntop->getTrace()->traceEvent(TRACE_WARNING, "JSON type %u not supported\n", type);
      break;
    }

    if(key != NULL && jvalue != NULL) {
      u_int32_t pen, key_id;
      bool res;

      getKeyId((char*)key, &pen, &key_id);

      switch(pen) {
      case 0: /* No PEN */
	res = parsePENZeroField(&flow, key_id, &value);
	if(res)
	  break;
	/* Dont'break when res == false for backward compatibility: attempt to parse Zero-PEN as Ntop-PEN */
      case NTOP_PEN:
	res = parsePENNtopField(&flow, key_id, &value);
	break;
      case UNKNOWN_PEN:
      default:
	res = false;
	break;
      }

      if(!res) {
	switch(key_id) {
	case 0: //json additional object added by Flow::serialize()
	  if(additional_o != NULL) {
	    struct json_object_iterator additional_it = json_object_iter_begin(additional_o);
	    struct json_object_iterator additional_itEnd = json_object_iter_end(additional_o);

	    while(!json_object_iter_equal(&additional_it, &additional_itEnd)) {

	      const char *additional_key   = json_object_iter_peek_name(&additional_it);
	      json_object *additional_v    = json_object_iter_peek_value(&additional_it);
	      const char *additional_value = json_object_get_string(additional_v);

	      if((additional_key != NULL) && (additional_value != NULL)) {
                //ntop->getTrace()->traceEvent(TRACE_NORMAL, "Additional field: %s", additional_key);
		flow.addAdditionalField(additional_key,
				        json_object_new_string(additional_value));
	      }
	      json_object_iter_next(&additional_it);
	    }
	  }
	  break;
	case UNKNOWN_FLOW_ELEMENT:
	  /* Attempt to parse it as an nProbe mini field */
	  if(parseNProbeAgentField(&flow, key, &value, jvalue)) {
	    if(!flow.hasParsedeBPF()) {
	      flow.setParsedeBPF();
	      flow.absolute_packet_octet_counters = true;
	    }
	    break;
	  }
	default:
#ifdef NTOPNG_PRO
	  if(custom_app_maps || (custom_app_maps = new(std::nothrow) CustomAppMaps()))
	    custom_app_maps->checkCustomApp(key, &value, &flow);
#endif
	  ntop->getTrace()->traceEvent(TRACE_DEBUG, "Not handled ZMQ field %u/%s", key_id, key);
	  add_to_additional_fields = true;
	  break;
	} /* switch */
      }

      if(add_to_additional_fields) {
        //ntop->getTrace()->traceEvent(TRACE_NORMAL, "Additional field: %s", key);
	flow.addAdditionalField(key, json_object_get(jvalue));
      }

      if(additional_o) json_object_put(additional_o);
    } /* if */

    /* Move to the next element */
    json_object_iter_next(&it);
  } // while json_object_iter_equal

  preprocessFlow(&flow, iface);
}

/* **************************************************** */

int ZMQParserInterface::parseSingleTLVFlow(ndpi_deserializer *deserializer,
						u_int8_t source_id,
						NetworkInterface *iface) {
  ndpi_serialization_element_type et;
  ParsedFlow flow;
  int ret = 0, rc;

  /* Reset data */
  flow.source_id = source_id;

  while ((et = ndpi_deserialize_get_nextitem_type(deserializer)) != ndpi_serialization_unknown) {
    ParsedValue value = { 0 };
    u_int32_t pen = 0, key_id;
    u_int32_t v32 = 0;
    int32_t i32 = 0;
    float f = 0;
    u_int64_t v64 = 0;
    ndpi_string key, vs;
    char key_str[64];
    u_int8_t vbkp, kbkp;
    bool add_to_additional_fields = false;
    bool key_is_string = false, value_is_string = false;

    // ntop->getTrace()->traceEvent(TRACE_NORMAL, "TLV Type %u", et);
      
    switch(et) {
    case ndpi_serialization_uint32_uint32:
      ndpi_deserialize_uint32_uint32(deserializer, &key_id, &v32);
      value.double_num = value.uint_num = v32;
      break;

    case ndpi_serialization_uint32_uint64:
      ndpi_deserialize_uint32_uint64(deserializer, &key_id, &v64);
      value.double_num = value.uint_num = v64;
      break;

    case ndpi_serialization_uint32_string:
      ndpi_deserialize_uint32_string(deserializer, &key_id, &vs);
      value.string = vs.str;
      value_is_string = true;
      break;
	
    case ndpi_serialization_string_float:
      ndpi_deserialize_string_float(deserializer, &key, &f);
      value.double_num = f;
      value.double_num = f;
      key_is_string = true;
      break;
      
    case ndpi_serialization_string_int32:
      ndpi_deserialize_string_int32(deserializer, &key, &i32);
      value.double_num = value.uint_num = v32;
      key_is_string = true;
      break;

    case ndpi_serialization_string_uint32:
      ndpi_deserialize_string_uint32(deserializer, &key, &v32);
      value.double_num = value.uint_num = v32;
      key_is_string = true;
      break;

    case ndpi_serialization_string_uint64:
      ndpi_deserialize_string_uint64(deserializer, &key, &v64);
      value.double_num = value.uint_num = v64;
      key_is_string = true;
      break;

    case ndpi_serialization_string_string:
      ndpi_deserialize_string_string(deserializer, &key, &vs);
      value.string = vs.str;
      key_is_string = value_is_string = true;
      break;

    case ndpi_serialization_end_of_record:
      ndpi_deserialize_end_of_record(deserializer);
      goto end_of_record;
      break;

    default:
      ntop->getTrace()->traceEvent(TRACE_WARNING, "Unsupported TLV type %u\n", et);
      ret = -1;
      goto error;
      break;
    }

    /* Adding '\0' to the end of the string, backing up the character */
    if (key_is_string) {
      kbkp = key.str[key.str_len];
      key.str[key.str_len] = '\0';
      getKeyId(key.str, &pen, &key_id);
    }
    if (value_is_string) {
      vbkp = vs.str[vs.str_len];
      vs.str[vs.str_len] = '\0';
    }

    switch(pen) {
      case 0: /* No PEN */
        rc = parsePENZeroField(&flow, key_id, &value);
        if (rc)
          break;
        /* Dont'break when rc == false for backward compatibility: attempt to parse Zero-PEN as Ntop-PEN */
      case NTOP_PEN:
        rc = parsePENNtopField(&flow, key_id, &value);
      break;
      case UNKNOWN_PEN:
      default:
        rc = false;
      break;
    }

    if (key_is_string) snprintf(key_str, sizeof(key_str),    "%s", key.str);
    else if (pen)      snprintf(key_str, sizeof(key_str), "%u.%u", pen, key_id);
    else               snprintf(key_str, sizeof(key_str),    "%u",      key_id);

    if (!rc) { /* Not handled */
      switch (key_id) {
	case 0: //json additional object added by Flow::serialize()
          if (strcmp(key_str,"json") == 0 && value_is_string) { 
            json_object *additional_o = json_tokener_parse(vs.str);

            if (additional_o) {
  	      struct json_object_iterator additional_it = json_object_iter_begin(additional_o);
  	      struct json_object_iterator additional_itEnd = json_object_iter_end(additional_o);

	      while(!json_object_iter_equal(&additional_it, &additional_itEnd)) {

	        const char *additional_key   = json_object_iter_peek_name(&additional_it);
	        json_object *additional_v    = json_object_iter_peek_value(&additional_it);
	        const char *additional_value = json_object_get_string(additional_v);

	        if((additional_key != NULL) && (additional_value != NULL)) {
                  //ntop->getTrace()->traceEvent(TRACE_NORMAL, "Additional field: %s", additional_key);
		  flow.addAdditionalField(additional_key, json_object_new_string(additional_value));
	        }
	        json_object_iter_next(&additional_it);
              }

              json_object_put(additional_o);
	    }
	  }
	  break;
	case UNKNOWN_FLOW_ELEMENT:
#if 0 // TODO 
	  /* Attempt to parse it as an nProbe mini field */
	  if(parseNProbeAgentField(&flow, key_str, &value)) {
	    if(!flow.hasParsedeBPF()) {
	      flow.setParsedeBPF();
	      flow.absolute_packet_octet_counters = true;
	    }
	    break;
	  }
#endif
	default:
#ifdef NTOPNG_PRO
	  if(custom_app_maps || (custom_app_maps = new(std::nothrow) CustomAppMaps()))
	    custom_app_maps->checkCustomApp(key_str, &value, &flow);
#endif
	  ntop->getTrace()->traceEvent(TRACE_DEBUG, "Not handled ZMQ field %u.%u", pen, key_id);
	  add_to_additional_fields = true;
	  break;
      } /* switch */
    }

    if (add_to_additional_fields) {
      //ntop->getTrace()->traceEvent(TRACE_NORMAL, "Additional field: %s (Key-ID: %u PEN: %u)", key_str, key_id, pen);
      flow.addAdditionalField(key_str,  
        value_is_string ? json_object_new_string(value.string) : json_object_new_int64(value.uint_num));
    }

    /* Restoring backed up character at the end of the string in place of '\0' */
    if (key_is_string) key.str[key.str_len] = kbkp;
    if (value_is_string) vs.str[vs.str_len] = vbkp;
    
  } /* while */

 end_of_record:

  preprocessFlow(&flow, iface);

 error:
  return ret;
}

/* **************************************************** */

u_int8_t ZMQParserInterface::parseJSONFlow(const char * const payload, int payload_size, u_int8_t source_id, void *data) {
  json_object *f;
  enum json_tokener_error jerr = json_tokener_success;
  NetworkInterface *iface = (NetworkInterface*)data;

#if 0
  // ntop->getTrace()->traceEvent(TRACE_NORMAL, "JSON: '%s' [len=%lu]", payload, strlen(payload));
  printf("\n\n%s\n\n", payload);
#endif

  f = json_tokener_parse_verbose(payload, &jerr);

  if(f != NULL) {
    int rc;

    if(json_object_get_type(f) == json_type_array) {
      /* Flow array */
      int id, num_elements = json_object_array_length(f);

      for(id = 0; id < num_elements; id++)
	parseSingleJSONFlow(json_object_array_get_idx(f, id), source_id, iface);

      rc = num_elements;
    } else {
      parseSingleJSONFlow(f, source_id, iface);
      rc = 1;
    }

    json_object_put(f);
    return(rc);
  } else {
    // if o != NULL
    if(!once) {
      ntop->getTrace()->traceEvent(TRACE_WARNING,
				   "Invalid message received: your nProbe sender is outdated, data encrypted or invalid JSON?");
      ntop->getTrace()->traceEvent(TRACE_WARNING, "JSON Parse error [%s] payload size: %u payload: %s",
				   json_tokener_error_desc(jerr),
				   payload_size,
				   payload);
    }

    once = true;
    return 0;
  }

  return 0;
}

/* **************************************************** */

u_int8_t ZMQParserInterface::parseTLVFlow(const char * const payload, int payload_size, u_int8_t source_id, void *data) {
  ndpi_deserializer deserializer;
  NetworkInterface *iface = (NetworkInterface *) data;
  int rc;

  rc = ndpi_init_deserializer_buf(&deserializer, (u_int8_t *) payload, payload_size);

  if (rc == -1)
    return 0;

  rc = 0;
  while (ndpi_deserialize_get_nextitem_type(&deserializer) != ndpi_serialization_unknown) {
    if (parseSingleTLVFlow(&deserializer, source_id, iface) != 0)
      break;
    rc++;
  }

  return rc;
}

/* **************************************************** */

bool ZMQParserInterface::parseContainerInfo(json_object *jo, ContainerInfo * const container_info) {
  json_object *obj, *obj2;

  if(json_object_object_get_ex(jo, "ID", &obj)) container_info->id = (char*)json_object_get_string(obj);

  if(json_object_object_get_ex(jo, "K8S", &obj)) {
    if(json_object_object_get_ex(obj, "POD", &obj2))  container_info->data.k8s.pod  = (char*)json_object_get_string(obj2);
    if(json_object_object_get_ex(obj, "NS", &obj2))   container_info->data.k8s.ns   = (char*)json_object_get_string(obj2);
    container_info->data_type = container_info_data_type_k8s;
  } else if(json_object_object_get_ex(jo, "DOCKER", &obj)) {
    container_info->data_type = container_info_data_type_k8s;
  } else
    container_info->data_type = container_info_data_type_unknown;

  if(obj) {
    if(json_object_object_get_ex(obj, "NAME", &obj2)) container_info->name = (char*)json_object_get_string(obj2);
  }

  // ntop->getTrace()->traceEvent(TRACE_NORMAL, "Container [id: %s] [%s] [k8s.name: %s][k8s.pod: %s][k8s.ns: %s][docker.name: %s]",
  // 			       container_info->id ? container_info->id : "",
  // 			       container_info->data_type == container_info_data_type_k8s ? "K8S" : container_info->data_type == container_info_data_type_docker ? "DOCKER" : "UNKNOWN",
  // 			       container_info->data_type == container_info_data_type_k8s && container_info->data.k8s.name ? container_info->data.k8s.name : "",
  // 			       container_info->data_type == container_info_data_type_k8s && container_info->data.k8s.pod ? container_info->data.k8s.pod : "",
  // 			       container_info->data_type == container_info_data_type_k8s && container_info->data.k8s.ns ? container_info->data.k8s.ns : "",
  // 			       container_info->data_type == container_info_data_type_docker && container_info->data.docker.name ? container_info->data.docker.name : "");

  return true;
}

/* **************************************************** */

u_int8_t ZMQParserInterface::parseCounter(const char * const payload, int payload_size, u_int8_t source_id, void *data) {
  json_object *o;
  enum json_tokener_error jerr = json_tokener_success;
  NetworkInterface * iface = (NetworkInterface*)data;
  sFlowInterfaceStats stats;

  // payload[payload_size] = '\0';
  // ntop->getTrace()->traceEvent(TRACE_NORMAL, "%s", payload);

  memset(&stats, 0, sizeof(stats));
  o = json_tokener_parse_verbose(payload, &jerr);

  if(o != NULL) {
    struct json_object_iterator it = json_object_iter_begin(o);
    struct json_object_iterator itEnd = json_object_iter_end(o);

    /* Reset data */
    memset(&stats, 0, sizeof(stats));

    while(!json_object_iter_equal(&it, &itEnd)) {
      const char *key   = json_object_iter_peek_name(&it);
      json_object *v    = json_object_iter_peek_value(&it);
      const char *value = json_object_get_string(v);

      if((key != NULL) && (value != NULL)) {
	if(!strcmp(key, "deviceIP")) stats.deviceIP = ntohl(inet_addr(value));
	else if(!strcmp(key, "ifIndex")) stats.ifIndex = (u_int32_t)json_object_get_int64(v);
	else if(!strcmp(key, "ifName")) stats.ifName = (char*)json_object_get_string(v);
	else if(!strcmp(key, "ifType")) stats.ifType = (u_int32_t)json_object_get_int64(v);
	else if(!strcmp(key, "ifSpeed")) stats.ifSpeed = (u_int32_t)json_object_get_int64(v);
	else if(!strcmp(key, "ifDirection")) stats.ifFullDuplex = (!strcmp(value, "Full")) ? true : false;
	else if(!strcmp(key, "ifAdminStatus")) stats.ifAdminStatus = (!strcmp(value, "Up")) ? true : false;
	else if(!strcmp(key, "ifOperStatus")) stats.ifOperStatus = (!strcmp(value, "Up")) ? true : false;
	else if(!strcmp(key, "ifInOctets")) stats.ifInOctets = json_object_get_int64(v);
	else if(!strcmp(key, "ifInPackets")) stats.ifInPackets = json_object_get_int64(v);
	else if(!strcmp(key, "ifInErrors")) stats.ifInErrors = json_object_get_int64(v);
	else if(!strcmp(key, "ifOutOctets")) stats.ifOutOctets = json_object_get_int64(v);
	else if(!strcmp(key, "ifOutPackets")) stats.ifOutPackets = json_object_get_int64(v);
	else if(!strcmp(key, "ifOutErrors")) stats.ifOutErrors = json_object_get_int64(v);
	else if(!strcmp(key, "ifPromiscuousMode")) stats.ifPromiscuousMode = (!strcmp(value, "1")) ? true : false;
	else if(strlen(key) >= 9 && !strncmp(&key[strlen(key) - 9], "CONTAINER", 9)) {
	  if(parseContainerInfo(v, &stats.container_info))
	    stats.container_info_set = true;
	}
      } /* if */

      /* Move to the next element */
      json_object_iter_next(&it);
    } // while json_object_iter_equal

    /* Process Flow */
    iface->processInterfaceStats(&stats);

    json_object_put(o);
  } else {
    // if o != NULL
    if(!once)
{      ntop->getTrace()->traceEvent(TRACE_WARNING,
				   "Invalid message received: your nProbe sender is outdated, data encrypted or invalid JSON?");
      ntop->getTrace()->traceEvent(TRACE_WARNING, "JSON Parse error [%s] payload size: %u payload: %s",
				   json_tokener_error_desc(jerr),
				   payload_size,
				   payload);
    }
    once = true;
    return -1;
  }

  return 0;
}

/* **************************************************** */

u_int8_t ZMQParserInterface::parseTemplate(const char * const payload, int payload_size, u_int8_t source_id, void *data) {
  /* The format that is currently defined for templates is a JSON as follows:

     [{"PEN":0,"field":1,"len":4,"format":"formatted_uint","name":"IN_BYTES","descr":"Incoming flow bytes (src->dst)"},{"PEN":0,"field":2,"len":4,"format":"formatted_uint","name":"IN_PKTS","descr":"Incoming flow packets (src->dst)"},]
  */

  // ntop->getTrace()->traceEvent(TRACE_NORMAL, "%s", payload);

  ZMQ_Template zmq_template;
  json_object *obj, *w, *z;
  enum json_tokener_error jerr = json_tokener_success;

  memset(&zmq_template, 0, sizeof(zmq_template));
  obj = json_tokener_parse_verbose(payload, &jerr);

  if(obj) {
    if(json_object_get_type(obj) == json_type_array) {
      int i, num_elements = json_object_array_length(obj);

      for(i = 0; i < num_elements; i++) {
	memset(&zmq_template, 0, sizeof(zmq_template));

	w = json_object_array_get_idx(obj, i);

	if(json_object_object_get_ex(w, "PEN", &z))
	  zmq_template.pen = (u_int32_t)json_object_get_int(z);

	if(json_object_object_get_ex(w, "field", &z))
	  zmq_template.field = (u_int32_t)json_object_get_int(z);

	if(json_object_object_get_ex(w, "format", &z))
	  zmq_template.format = json_object_get_string(z);

	if(json_object_object_get_ex(w, "name", &z))
	  zmq_template.name = json_object_get_string(z);

	if(json_object_object_get_ex(w, "descr", &z))
	  zmq_template.descr = json_object_get_string(z);

	if(zmq_template.name)
	  addMapping(zmq_template.name, zmq_template.field, zmq_template.pen);

	// ntop->getTrace()->traceEvent(TRACE_NORMAL, "Template [PEN: %u][field: %u][format: %s][name: %s][descr: %s]",
	//			     zmq_template.pen, zmq_template.field, zmq_template.format, zmq_template.name, zmq_template.descr)
	  ;
      }
    }
    json_object_put(obj);
  } else {
    // if o != NULL
    if(!once) {
      ntop->getTrace()->traceEvent(TRACE_WARNING,
				   "Invalid message received: your nProbe sender is outdated, data encrypted or invalid JSON?");
      ntop->getTrace()->traceEvent(TRACE_WARNING, "JSON Parse error [%s] payload size: %u payload: %s",
				   json_tokener_error_desc(jerr),
				   payload_size,
				   payload);
    }
    once = true;
    return -1;
  }

  return 0;
}

/* **************************************************** */

void ZMQParserInterface::setFieldMap(const ZMQ_FieldMap * const field_map) const {
  char hname[CONST_MAX_LEN_REDIS_KEY], key[32];
  snprintf(hname, sizeof(hname), CONST_FIELD_MAP_CACHE_KEY, get_id(), field_map->pen);
  snprintf(key, sizeof(key), "%u", field_map->field);

  ntop->getRedis()->hashSet(hname, key, field_map->map);
}

/* **************************************************** */

void ZMQParserInterface::setFieldValueMap(const ZMQ_FieldValueMap * const field_value_map) const {
  char hname[CONST_MAX_LEN_REDIS_KEY], key[32];
  snprintf(hname, sizeof(hname), CONST_FIELD_VALUE_MAP_CACHE_KEY, get_id(), field_value_map->pen, field_value_map->field);
  snprintf(key, sizeof(key), "%u", field_value_map->value);

  ntop->getRedis()->hashSet(hname, key, field_value_map->map);
}

/* **************************************************** */

u_int8_t ZMQParserInterface::parseOptionFieldMap(json_object * const jo) const {
  int arraylen = json_object_array_length(jo);
  json_object *w, *z;
  ZMQ_FieldMap field_map;
  memset(&field_map, 0, sizeof(field_map));

  for(int i = 0; i < arraylen; i++) {
    w = json_object_array_get_idx(jo, i);

    if(json_object_object_get_ex(w, "PEN", &z))
      field_map.pen = (u_int32_t)json_object_get_int(z);

    if(json_object_object_get_ex(w, "field", &z)) {
      field_map.field = (u_int32_t)json_object_get_int(z);

      if(json_object_object_get_ex(w, "map", &z)) {
	field_map.map = json_object_to_json_string(z);

	setFieldMap(&field_map);

#ifdef CUSTOM_APP_DEBUG
	ntop->getTrace()->traceEvent(TRACE_NORMAL, "Option FieldMap [PEN: %u][field: %u][map: %s]",
				     field_map.pen, field_map.field, field_map.map);
#endif
      }
    }
  }

  return 0;
}

/* **************************************************** */

u_int8_t ZMQParserInterface::parseOptionFieldValueMap(json_object * const w) const {
  json_object *z;
  ZMQ_FieldValueMap field_value_map;
  memset(&field_value_map, 0, sizeof(field_value_map));

  if(json_object_object_get_ex(w, "PEN", &z))
    field_value_map.pen = (u_int32_t)json_object_get_int(z);

  if(json_object_object_get_ex(w, "field", &z)) {
    field_value_map.field = (u_int32_t)json_object_get_int(z);

    if(json_object_object_get_ex(w, "value", &z)) {
      field_value_map.value = (u_int32_t)json_object_get_int(z);

      if(json_object_object_get_ex(w, "map", &z)) {
	field_value_map.map = json_object_to_json_string(z);

	setFieldValueMap(&field_value_map);

#ifdef CUSTOM_APP_DEBUG
	ntop->getTrace()->traceEvent(TRACE_NORMAL, "Option FieldValueMap [PEN: %u][field: %u][value: %u][map: %s]",
				     field_value_map.pen, field_value_map.field, field_value_map.value, field_value_map.map);
#endif
      }
    }
  }

  return 0;
}

/* **************************************************** */

u_int8_t ZMQParserInterface::parseOption(const char * const payload, int payload_size, u_int8_t source_id, void *data) {
  /* The format that is currently defined for options is a JSON as follows:

    char opt[] = "
    "{\"PEN\":8741, \"field\": 22, \"value\":1, \"map\":{\"name\":\"Skype\"}},"
    "{\"PEN\":8741, \"field\": 22, \"value\":3, \"map\":{\"name\":\"Winni\"}}";

    parseOption(opt, strlen(opt), source_id, this);
  */

  // ntop->getTrace()->traceEvent(TRACE_NORMAL, "%s", payload);

  json_object *o;
  enum json_tokener_error jerr = json_tokener_success;

  o = json_tokener_parse_verbose(payload, &jerr);

  if(o != NULL) {
    parseOptionFieldValueMap(o);
    json_object_put(o);
  } else {
    // if o != NULL
    if(!once) {
      ntop->getTrace()->traceEvent(TRACE_WARNING,
				   "Invalid message received: your nProbe sender is outdated, data encrypted or invalid JSON?");
      ntop->getTrace()->traceEvent(TRACE_WARNING, "JSON Parse error [%s] payload size: %u payload: %s",
				   json_tokener_error_desc(jerr),
				   payload_size,
				   payload);
    }
    once = true;
    return -1;
  }

  return 0;
}

/* **************************************** */

void ZMQParserInterface::setRemoteStats(ZMQ_RemoteStats *zrs) {
  if(!zrs) return;

  ifSpeed = zrs->remote_ifspeed, last_pkt_rcvd = 0, last_pkt_rcvd_remote = zrs->remote_time,
    last_remote_pps = zrs->avg_pps, last_remote_bps = zrs->avg_bps;

  if((zmq_initial_pkts == 0) /* ntopng has been restarted */
     || (zrs->remote_bytes < zmq_initial_bytes) /* nProbe has been restarted */
     ) {
    /* Start over */
    zmq_initial_bytes = zrs->remote_bytes, zmq_initial_pkts = zrs->remote_pkts;
  }

  if(zmq_remote_initial_exported_flows == 0 /* ntopng has been restarted */
     || zrs->num_flow_exports < zmq_remote_initial_exported_flows) /* nProbe has been restarted */
    zmq_remote_initial_exported_flows = zrs->num_flow_exports;

  if(zmq_remote_stats_shadow) free(zmq_remote_stats_shadow);
  zmq_remote_stats_shadow = zmq_remote_stats;
  zmq_remote_stats = zrs;

  /*
   * Don't override ethStats here, these stats are properly updated
   * inside NetworkInterface::processFlow for ZMQ interfaces.
   * Overriding values here may cause glitches and non-strictly-increasing counters
   * yielding negative rates.
   ethStats.setNumBytes(zrs->remote_bytes), ethStats.setNumPackets(zrs->remote_pkts);
   *
   */
}

/* **************************************************** */

#ifdef NTOPNG_PRO
bool ZMQParserInterface::getCustomAppDetails(u_int32_t remapped_app_id, u_int32_t *const pen, u_int32_t *const app_field, u_int32_t *const app_id) {
  return custom_app_maps && custom_app_maps->getCustomAppDetails(remapped_app_id, pen, app_field, app_id);
}
#endif

/* **************************************************** */

void ZMQParserInterface::lua(lua_State* vm) {
  ZMQ_RemoteStats *zrs = zmq_remote_stats;

  NetworkInterface::lua(vm);

  if(zrs) {
    if(zrs->remote_ifname[0] != '\0')
      lua_push_str_table_entry(vm, "remote.name", zrs->remote_ifname);
    if(zrs->remote_ifaddress[0] != '\0')
      lua_push_str_table_entry(vm, "remote.if_addr",zrs->remote_ifaddress);
    if(zrs->remote_probe_address[0] != '\0')
      lua_push_str_table_entry(vm, "probe.ip", zrs->remote_probe_address);
    if(zrs->remote_probe_public_address[0] != '\0')
      lua_push_str_table_entry(vm, "probe.public_ip", zrs->remote_probe_public_address);

    lua_push_uint64_table_entry(vm, "zmq.num_flow_exports", zrs->num_flow_exports - zmq_remote_initial_exported_flows);
    lua_push_uint64_table_entry(vm, "zmq.num_exporters", zrs->num_exporters);

    if(zrs->export_queue_full > 0)
      lua_push_uint64_table_entry(vm, "zmq.drops.export_queue_full", zrs->export_queue_full);
    lua_push_uint64_table_entry(vm, "zmq.drops.flow_collection_drops", zrs->flow_collection_drops);

    lua_push_uint64_table_entry(vm, "timeout.lifetime", zrs->remote_lifetime_timeout);
    lua_push_uint64_table_entry(vm, "timeout.idle", zrs->remote_idle_timeout);
  }
}

/* **************************************************** */

#endif
