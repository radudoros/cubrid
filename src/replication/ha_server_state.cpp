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

#include "ha_server_state.hpp"

namespace ha_operations
{
  const char *SERVER_STATE_NA_STR            = "na";
  const char *SERVER_STATE_IDLE_STR          = "idle";
  const char *SERVER_STATE_ACTIVE_STR        = "active";
  const char *SERVER_STATE_TO_BE_ACTIVE_STR  = "to-be-active";
  const char *SERVER_STATE_STANDBY_STR       = "standby";
  const char *SERVER_STATE_TO_BE_STANDBY_STR = "to-be-standby";
  const char *SERVER_STATE_MAINTENANCE_STR   = "maintenance";
  const char *SERVER_STATE_DEAD_STR          = "dead";

  const char *
  server_state_string (SERVER_STATE state)
  {
    switch (state)
      {
      case SERVER_STATE_NA:
	return "na";
      case SERVER_STATE_IDLE:
	return SERVER_STATE_IDLE_STR;
      case SERVER_STATE_ACTIVE:
	return SERVER_STATE_ACTIVE_STR;
      case SERVER_STATE_TO_BE_ACTIVE:
	return SERVER_STATE_TO_BE_ACTIVE_STR;
      case SERVER_STATE_STANDBY:
	return SERVER_STATE_STANDBY_STR;
      case SERVER_STATE_TO_BE_STANDBY:
	return SERVER_STATE_TO_BE_STANDBY_STR;
      case SERVER_STATE_MAINTENANCE:
	return SERVER_STATE_MAINTENANCE_STR;
      case SERVER_STATE_DEAD:
	return SERVER_STATE_DEAD_STR;
      }
    return "invalid";
  }

  SERVER_STATE ha_Server_state = SERVER_STATE_IDLE;

  SERVER_STATE &
  get_server_state ()
  {
    return ha_Server_state;
  }
}