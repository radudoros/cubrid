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
 * load_scanner.hpp - subclass of yyFlexLexer, provides the main scanner function.
 */

#ifndef _LOAD_SCANNER_HPP_
#define _LOAD_SCANNER_HPP_

#include "load_error_handler.hpp"
#include "load_semantic_helper.hpp"
#include "load_grammar.hpp"
#include "utility.h"

#if !defined (yyFlexLexerOnce)
#include <FlexLexer.h>
#endif
#include <istream>

namespace cubload
{

  class scanner : public yyFlexLexer
  {
    public:
      scanner (semantic_helper &semantic_helper, error_handler &error_handler)
	: yyFlexLexer ()
	, m_semantic_helper (semantic_helper)
	, m_error_handler (error_handler)
      {
	//
      };

      ~scanner () override = default;

      void set_lineno (int line_offset)
      {
	yylineno = line_offset;
      }

      /*
       * The main scanner function.
       * See load_lexer.l file for method declaration
       */
      virtual int yylex (parser::semantic_type *yylval, parser::location_type *yylloc);

      /*
       * Lexer error function
       * @param msg a description of the lexer error.
       */
      void LexerError (const char *msg) override
      {
	/* TODO: We need a better approach since this should in fact throw an irrecoverable error
	 * which will fail the session. However, for syntax checking, this proves to abort all
	 * other batches that are being checked, which is not entirely correct since we want to
	 * use the parallelism to check for all errors in the file.
	 */
	m_error_handler.on_error_with_line (LOADDB_MSG_LEX_ERROR);
	m_error_handler.on_syntax_failure ();
      }

    private:
      semantic_helper &m_semantic_helper;
      error_handler &m_error_handler;
  };

} // namespace cubload

#endif /* _LOAD_SCANNER_HPP_ */
