/*  geanylispedit.h - a Geany plugin
 *
 *  Copyright 2011 Tapiwa Gutu
 *	
 * 	Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 * 
 *		* Redistributions of source code must retain the above copyright
 * 		  notice, this list of conditions and the following disclaimer.
 * 		
 * 		* Redistributions in binary form must reproduce the above
 * 		  copyright notice, this list of conditions and the following
 * 		  disclaimer in the documentation and/or other materials provided
 * 		  with the distribution.
 * 		
 * 		* Neither the name of the {company} nor the names of its
 * 		  contributors may be used to endorse or promote products derived
 * 		  from this software without specific prior written permission.
 * 
 * 	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * 	"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * 	LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * 	A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * 	OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * 	SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * 	LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * 	DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * 	THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * 	(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * 	OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */

#ifndef __GEANYLISPEDIT_H__
#define __GEANYLISPEDIT_H__

#include <vte/vte.h>
#include <geanyplugin.h>
#include <gdk/gdkkeysyms.h>
#include <string.h>

#define PLUGIN_NAME 				_("LispEdit")
#define PLUGIN_DESCRIPTION			_("A Geany plugin that provides shortcuts for sending s-expressions (symbolic expressions) to a Lisp process running in Geany's embedded terminal.")

/* Macro definitions for values required by the plugin.
 * The macros define the strings displayed on the menu and the identifier name for the menu entry.
 * The keyboard shorcut keys for invoking the plugin callback functions are also defined.
 * */
#define EVAL_ID_STR					"lisp_eval"
#define MACROEXPAND_ID_STR			"lisp_macroexpand_1"
#define EVAL_MENU_STR				_("LispEdit: eval")
#define MACROEXPAND_MENU_STR		_("LispEdit: macroexpand-1")
#define EVAL_KEY_SEQ				(GDK_CONTROL_MASK | GDK_SHIFT_MASK) //The eval function will be triggered by    Shift + Ctrl + Enter
#define MACROEXPAND_KEY_SEQ			(1 << 3 | GDK_SHIFT_MASK)    //macroexpand callback will be triggered by Shift + Alt + Enter
#define MAX_CMD_LENGTH				4096	//The maximum characters that can be captured in a string.

//Variables used by the plugin.
static GtkWidget *macroexpand_1_menu_item = NULL;
static GtkWidget *eval_menu_item = NULL;
static VteTerminal *vte = NULL;
static gboolean have_vte = FALSE;
static GeanyDocument *doc = NULL;
static gint start_pos, end_pos;
gchar cmd_string[MAX_CMD_LENGTH];

//Functions required by Geany
void plugin_cleanup(void);
void plugin_init(G_GNUC_UNUSED GeanyData *data);

//Callback functions used by the plugin
static void cb_eval(G_GNUC_UNUSED GtkMenuItem *menuitem, G_GNUC_UNUSED gpointer gdata);
static void cb_macroexpand_1(G_GNUC_UNUSED GtkMenuItem *menuitem, G_GNUC_UNUSED gpointer gdata);
static void on_eval_key(G_GNUC_UNUSED guint key_id);
static void on_macroexpand_1_key(G_GNUC_UNUSED guint key_id);

//Functions
static void init_vte(void);
static void show_error_message(void);
static void set_vte(GtkWidget *widget);

// Keybinding(s) 
enum{
   KB_EVAL,
   KB_MACROEXPAND,
   KB_COUNT
   };
   
#endif
