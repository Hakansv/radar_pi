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

#include "NavicoCommon.h"
#include "RadarReceive.h"
#include "socketutil.h"

PLUGIN_BEGIN_NAMESPACE

//
// An intermediary class that implements the common parts of any Navico radar.
//

class NavicoReceive : public RadarReceive {
 public:
  NavicoReceive(radar_pi *pi, RadarInfo *ri, NetworkAddress reportAddr, NetworkAddress dataAddr) : RadarReceive(pi, ri) {
    m_data_addr = dataAddr;
    m_report_addr = reportAddr;
    m_next_spoke = -1;
    m_radar_status = 0;
    m_shutdown_time_requested = 0;
    m_is_shutdown = false;
    m_first_receive = true;
    m_interface_addr = m_pi->GetRadarInterfaceAddress(ri->m_radar);
    m_receive_socket = GetLocalhostServerTCPSocket();
    m_send_socket = GetLocalhostSendTCPSocket(m_receive_socket);
    SetInfoStatus(wxString::Format(wxT("%s: %s"), m_ri->m_name.c_str(), _("Initializing")));
	SetPriority(wxPRIORITY_MAX);
    LOG_INFO(wxT("radar_pi: %s receive thread created, prio= %i"), m_ri->m_name.c_str(), GetPriority());
  };

  ~NavicoReceive() {}

  void *Entry(void);
  void Shutdown(void);
  wxString GetInfoStatus();

  NetworkAddress m_interface_addr;
  NetworkAddress m_data_addr;
  NetworkAddress m_report_addr;

  wxLongLong m_shutdown_time_requested;  // Main thread asks this thread to stop
  volatile bool m_is_shutdown;

 private:
  void ProcessFrame(const uint8_t *data, int len);
  bool ProcessReport(const uint8_t *data, int len);

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
  bool m_first_receive;

  wxString m_addr;  // Radar's IP address

  wxCriticalSection m_lock;  // Protects m_status
  wxString m_status;         // Userfriendly string
  wxString m_firmware;       // Userfriendly string #2

  void SetInfoStatus(wxString status) {
    wxCriticalSectionLocker lock(m_lock);
    m_status = status;
  }
  void SetFirmware(wxString s) {
    wxCriticalSectionLocker lock(m_lock);
    m_firmware = s;
  }
};

PLUGIN_END_NAMESPACE

#endif /* _NAVICORECEIVE_H_ */
