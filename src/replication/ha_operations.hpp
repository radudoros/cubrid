/*
 * Copyright (C) 2008 Search Solution Corporation. All rights reserved by Search Solution.
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 */

#ifndef _HA_OPERATIONS_HPP_
#define _HA_OPERATIONS_HPP_

#include "ha_server_state.hpp"
#include "thread_entry.hpp"

namespace ha_operations
{
  int change_server_state (cubthread::entry *thread_p, SERVER_STATE state, bool force, int timeout, bool heartbeat);
  SERVER_STATE transit_server_state (cubthread::entry *thread_p, SERVER_STATE req_state);
  void finish_transit (cubthread::entry *thread_p, bool force, SERVER_STATE req_state);
}

static auto css_change_ha_server_state = ha_operations::change_server_state;
static auto css_finish_transit = ha_operations::finish_transit;
static auto css_transit_ha_server_state = ha_operations::transit_server_state;

#endif // !_HA_OPERATIONS_HPP_
