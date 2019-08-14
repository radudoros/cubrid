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

/*
 * replication_node_manager.cpp
 */

#include "replication_node_manager.hpp"

#include "internal_tasks_worker_pool.hpp"
#include "log_impl.h"
#include "multi_thread_stream.hpp"
#include "replication_master_node.hpp"
#include "replication_slave_node.hpp"
#include "stream_file.hpp"
#include "thread_manager.hpp"
#include "thread_task.hpp"

namespace cubreplication
{
  std::string g_hostname;

  cubstream::multi_thread_stream *g_stream = NULL;
  cubstream::stream_file *g_stream_file = NULL;

  cubreplication::master_node *g_master_node = NULL;
  cubreplication::slave_node *g_slave_node = NULL;

  namespace replication_node_manager
  {
    enum commute_state
    {
      SLAVE,
      TO_SLAVE,
      NOT_INITED,
      TO_MASTER,
      MASTER
    };
    commute_state current_state;

    void init (const char *server_name)
    {
      current_state = NOT_INITED;
      g_hostname = server_name;

      INT64 buffer_size = prm_get_bigint_value (PRM_ID_REPL_BUFFER_SIZE);
      int num_max_appenders = log_Gl.trantable.num_total_indices + 1;
      g_stream = new cubstream::multi_thread_stream (buffer_size, num_max_appenders);
      g_stream->set_name ("repl" + g_hostname);
      g_stream->set_trigger_min_to_read_size (stream_entry::compute_header_size ());
      g_stream->init (0);

      log_generator::set_global_stream (g_stream);

      std::string replication_path;
      replication_node::get_replication_file_path (replication_path);
      g_stream_file = new cubstream::stream_file (*g_stream, replication_path);
    }

    void finalize ()
    {
      g_hostname.clear ();

      delete g_slave_node;
      g_slave_node = NULL;
      delete g_master_node;
      g_master_node = NULL;

      // need to first stop the stream before destroying stream_file
      g_stream->stop ();
      delete g_stream_file;
      g_stream_file = NULL;
      delete g_stream;
      g_stream = NULL;
    }

    void commute_to_master_state ()
    {
      if (current_state == TO_SLAVE)
	{
	  assert (false);
	  // we do not support chaining conflicting commutations
	  // todo: ERROR
	  return;
	}
      if (current_state >= TO_MASTER)
	{
	  // already in or commuting to desired state
	  return;
	}
      current_state = TO_MASTER;

      cubthread::entry_task *promote_task = new cubthread::entry_callable_task ([] (
		  cubthread::entry &context)
      {
	if (g_slave_node != NULL)
	  {
	    g_slave_node->finish_fetch_from_stream ();
	  }
	delete g_slave_node;
	g_slave_node = NULL;

	assert (g_master_node == NULL);
	g_master_node = new master_node (g_hostname.c_str (), g_stream, g_stream_file);
	current_state = MASTER;
      }, true);

      auto wp = cubthread::internal_tasks_worker_pool::get_instance ();
      cubthread::get_manager ()->push_task (wp, promote_task);
    }

    void commute_to_slave_state ()
    {
      if (current_state == TO_MASTER)
	{
	  assert (false);
	  // we do not support chaining conflicting commutations
	  // todo: ERROR
	  return;
	}
      if (current_state <= TO_SLAVE)
	{
	  // already in or commuting to desired state
	  return;
	}
      current_state = TO_SLAVE;

      cubthread::entry_task *demote_task = new cubthread::entry_callable_task ([] (
		  cubthread::entry &context)
      {
	// todo: remove after master -> slave transitions is properly handled
	assert (g_master_node == NULL);

	delete g_master_node;
	g_master_node = NULL;

	assert (g_slave_node == NULL);
	g_slave_node = new slave_node (g_hostname.c_str (), g_stream, g_stream_file);
	current_state = SLAVE;
      }, true);

      auto wp = cubthread::internal_tasks_worker_pool::get_instance ();
      cubthread::get_manager ()->push_task (wp, demote_task);
    }

    master_node *get_master_node ()
    {
      assert (g_master_node != NULL);
      return g_master_node;
    }

    slave_node *get_slave_node ()
    {
      assert (g_slave_node != NULL);
      return g_slave_node;
    }
  }
}
