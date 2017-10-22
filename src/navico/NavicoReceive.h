/******************************************************************************
 *
 * Project:  OpenCPN
 * Purpose:  Radar Plugin
 * Author:   David Register
 *           Dave Cowell
 *           Kees Verruijt
 *           Douwe Fokkema
 *           Sean D'Epagnier
 ***************************************************************************
 *   Copyright (C) 2010 by David S. Register              bdbcat@yahoo.com *
 *   Copyright (C) 2012-2013 by Dave Cowell                                *
 *   Copyright (C) 2012-2016 by Kees Verruijt         canboat@verruijt.net *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************
 */

#ifndef _NAVICORECEIVE_H_
#define _NAVICORECEIVE_H_

#include "RadarReceive.h"
#include "socketutil.h"

PLUGIN_BEGIN_NAMESPACE

//
// An intermediary class that implements the common parts of any Navico radar.
//

class NavicoReceive : public RadarReceive {
 public:
  NavicoReceive(radar_pi *pi, RadarInfo *ri, NetworkAddress dataAddr, NetworkAddress reportAddr) : RadarReceive(pi, ri) {
    m_data_addr = dataAddr;
    m_report_addr = reportAddr;
    m_next_spoke = -1;
    CLEAR_STRUCT(m_mcast_addr);
    m_radar_status = 0;
    m_new_ip_addr = false;
    m_shutdown_time_requested = 0;
    m_is_shutdown = false;

    m_mcast_addr = m_pi->GetMcastIPAddress(ri->m_radar);

    m_receive_socket = GetLocalhostServerTCPSocket();
    m_send_socket = GetLocalhostSendTCPSocket(m_receive_socket);

    LOG_RECEIVE(wxT("radar_pi: %s receive thread created"), m_ri->m_name.c_str());
  };

  ~NavicoReceive() {}

  void *Entry(void);
  void Shutdown(void);

  NetworkAddress m_mcast_addr;
  wxIPV4address m_ip_addr;
  bool m_new_ip_addr;

  NetworkAddress m_data_addr;
  NetworkAddress m_report_addr;

  wxLongLong m_shutdown_time_requested;  // Main thread asks this thread to stop
  volatile bool m_is_shutdown;

 private:
  void ProcessFrame(const UINT8 *data, int len);
  bool ProcessReport(const UINT8 *data, int len);

  SOCKET PickNextEthernetCard();
  SOCKET GetNewReportSocket();
  SOCKET GetNewDataSocket();

  wxString m_ip;

  SOCKET m_receive_socket;  // Where we listen for message from m_send_socket
  SOCKET m_send_socket;     // A message to this socket will interrupt select() and allow immediate shutdown

  struct ifaddrs *m_interface_array;
  struct ifaddrs *m_interface;

  int m_next_spoke;
  char m_radar_status;
};

PLUGIN_END_NAMESPACE

#endif /* _NAVICORECEIVE_H_ */