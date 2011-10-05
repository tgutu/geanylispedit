/*  geanylispedit.c - a Geany plugin
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

#include "geanylispedit.h"

//Variables required by Geany
GeanyPlugin     *geany_plugin;
GeanyData       *geany_data;
GeanyFunctions  *geany_functions;

PLUGIN_VERSION_CHECK(201);
PLUGIN_SET_INFO(PLUGIN_NAME, PLUGIN_DESCRIPTION, "1.0", "Tapiwa Gutu");
PLUGIN_KEY_GROUP(lispedit_shortcuts, KB_COUNT)//Set the keybinding group.

/**
 * name: init_vte
 * 
 * This function locates the message window notebook widget and calls
 * set_vte() to search for the VTE terminal from the widget.
 * 
 * @return void
 * @see set_vte()
 **/
static void init_vte(void)
{
    GtkNotebook *nb;
    GtkWidget *vte_frame = NULL;
    
    nb = GTK_NOTEBOOK(geany_data->main_widgets->message_window_notebook);
    vte_frame = gtk_notebook_get_nth_page(nb, MSG_VTE);
    
    //If the frame contains the vte, go get the VTE and set it to GtkWidget *vte
    if (vte_frame != NULL)
		set_vte(vte_frame);
}

/** 
 * name: cb_eval
 * 
 * This callback function is invoked when the 'LispEdit: eval' menu option is selected.
 * If the cursor is placed after a closing parenthesis the function will look for the matching
 * opening parenthesis. If there is no matching parenthesis it will display a warning dialog box.
 *  
 * If the closing parenthesis has a matching opening parenthesis, all the characters between the parentheses
 * including the parentheses are sent to the child process running in the VTE.
 * 
 * @param menuitem GtkMenuItem.
 * @param gdata gpointer.
 * @return void
 **/
static void cb_eval(G_GNUC_UNUSED GtkMenuItem *menuitem, G_GNUC_UNUSED gpointer gdata)
{
	if (have_vte)
    {
		doc = document_get_current();
		end_pos = sci_get_current_position(doc->editor->sci);
		if (end_pos > 0) end_pos--;
		gchar letter = sci_get_char_at(doc->editor->sci, end_pos);
		
		switch (letter)
		{ 
			case ')':	
						start_pos = sci_find_matching_brace(doc->editor->sci, end_pos);
						if (start_pos < 0)
						{
							dialogs_show_msgbox(GTK_MESSAGE_WARNING, "Found an isolated closing brace!!!");
						}
						else if (start_pos >= 0)
						{
							sci_get_text_range(doc->editor->sci, start_pos, ++end_pos, cmd_string);
							vte_terminal_feed_child(vte, "\n", strlen("\n"));
							vte_terminal_feed_child(vte, cmd_string, strlen(cmd_string));
							vte_terminal_feed_child(vte, "\n", strlen("\n"));
						}
						break;						
		}
    }
	else
	{
		show_error_message();
	}	
}

/**
 * name: cb_macroexpand_1
 * 
 * This callback function is invoked when the 'LispEdit: macroexpand-1' menu option is selected.
 * If the cursor is placed after a closing parenthesis the function will look for the matching
 * opening parenthesis. If there is no matching parenthesis it will display a warning dialog box.
 *  
 * If the closing parenthesis has a matching opening parenthesis, the function will send
 * (macroexpand-1 '__PARENTHESIS_&_CONTENTS_GO_HERE__) to the child process running in the VTE.
 * 
 * @param menuitem GtkMenuItem.
 * @param gdata gpointer.
 * @return void
 **/
static void cb_macroexpand_1(G_GNUC_UNUSED GtkMenuItem *menuitem, G_GNUC_UNUSED gpointer gdata)
{
	if (have_vte)
    {
		doc = document_get_current();
		end_pos = sci_get_current_position(doc->editor->sci);
		if (end_pos > 0) end_pos--;
		gchar letter = sci_get_char_at(doc->editor->sci, end_pos );
		
		switch (letter)
		{ 
			case ')':	start_pos = sci_find_matching_brace(doc->editor->sci, end_pos);
						
						if (start_pos < 0)
							dialogs_show_msgbox(GTK_MESSAGE_WARNING, "Found an isolated closing brace!!!");
						else
						{
							sci_get_text_range(doc->editor->sci, start_pos, ++end_pos, cmd_string);
							vte_terminal_feed_child(vte, "\n(macroexpand-1 '", strlen("\n(macroexpand-1 '"));
							vte_terminal_feed_child(vte, cmd_string, strlen(cmd_string));
							vte_terminal_feed_child(vte, ")\n", strlen(")\n"));
						}
						break;
		}
    }
	else
	{
		show_error_message();
	}
}

/**
 * name: on_eval_key
 *  
 * This callback function is invoked when the shorcut keys Shift + Ctrl + Enter are pressed.
 * The function invokes the cb_eval() function.
 *   
 * @param key_id guint.
 * @return void
 * @see cb_eval()
 **/
static void on_eval_key(G_GNUC_UNUSED guint key_id)
{  
   // sanity check
   if (document_get_current() == NULL){
       return;
       }
   
   cb_eval(NULL, NULL);
}

/**
 * name: on_macroexpand_1_key
 * 
 * This callback function is invoked when the shorcut keys Shift + Alt + Enter are pressed.
 * The function invokes the cb_macroexpand() function.
 *   
 * @param key_id guint.
 * @return void
 * @see cb_macroexpand()
 **/
static void on_macroexpand_1_key(G_GNUC_UNUSED guint key_id)
{ 
   // sanity check
   if (document_get_current() == NULL){
       return;
       }
   
   cb_macroexpand_1(NULL, NULL);
} 

/** 
 * name: plugin_init
 * 
 * The Geany plugin initialization function. This is called automatically by Geany when a user installs the plugin.
 *   
 * @param data GeanyData.
 * @return void
 **/
void plugin_init(G_GNUC_UNUSED GeanyData *data)
{
	//Get hold of the Edit menu widget.
	GtkWidget* parent_menu = ui_lookup_widget(GTK_WIDGET(geany->main_widgets->window), "edit1_menu");//"tools1_menu"
	
	// Create the 'LispEdit: eval' menu, assign it to the callback function cb_eval
	// and attach it to parent_menu.
	eval_menu_item = gtk_menu_item_new_with_mnemonic(EVAL_MENU_STR);
	gtk_widget_show(eval_menu_item);
	gtk_container_add(GTK_CONTAINER(parent_menu),							
		eval_menu_item);
	g_signal_connect(eval_menu_item, "activate",
		G_CALLBACK(cb_eval), NULL);
   
    // Create the 'LispEdit: macroexpand-1' menu, assign it to the callback function cb_macroexpand_1
	// and attach it to parent_menu.
	macroexpand_1_menu_item = gtk_menu_item_new_with_mnemonic(MACROEXPAND_MENU_STR);
	gtk_widget_show(macroexpand_1_menu_item);
	gtk_container_add(GTK_CONTAINER(parent_menu),
		macroexpand_1_menu_item);
	g_signal_connect(macroexpand_1_menu_item, "activate",
		G_CALLBACK(cb_macroexpand_1), NULL);

	/* make sure our menu items aren't called when there is no doc open */
	ui_add_document_sensitive(eval_menu_item);
	ui_add_document_sensitive(macroexpand_1_menu_item);

	// setup keybindings for the callback functions.
	keybindings_set_item(plugin_key_group, KB_MACROEXPAND, on_macroexpand_1_key,
		GDK_Return, MACROEXPAND_KEY_SEQ, MACROEXPAND_ID_STR, MACROEXPAND_MENU_STR, macroexpand_1_menu_item);
	keybindings_set_item(plugin_key_group, KB_EVAL, on_eval_key,
		GDK_Return, EVAL_KEY_SEQ, EVAL_ID_STR, EVAL_MENU_STR, eval_menu_item);
	
	//Initialiase the binding to the Geany virtual terminal (VTE).
	init_vte();

}

/** 
 * name: plugin_cleanup
 * 
 * The Geany plugin cleanup function. This is called automatically by Geany when a user uninstalls the plugin.
 * 
 * @return void 
 **/
void plugin_cleanup(void)
{
   gtk_widget_destroy(macroexpand_1_menu_item);
   gtk_widget_destroy(eval_menu_item);
}

/** 
 * name: set_vte
 * 
 * This function searches for the VTE terminal from the widget and sets 
 * it to the variable VteTerminal *vte.
 *   
 * @param widget GtkWidget.
 * @return void
 * @see init_vte()
 **/

static void set_vte(GtkWidget *widget)
{
	if (VTE_IS_TERMINAL(widget))
	{
        have_vte = TRUE;
        vte = VTE_TERMINAL(widget);
    }
    else if (GTK_IS_CONTAINER(widget))
    {
        GList *children, *iter;
        
        children = gtk_container_get_children(GTK_CONTAINER(widget));
        
        for (iter=children; !have_vte && iter; iter=g_list_next(iter))
            set_vte(iter->data);
            
        g_list_free(children);
    }
}

/** 
 * name: show_error_message
 * 
 * This function displays an error function when the user attempts to send
 * commands to a Lisp process running in the terminal if no terminal exists.
 * 
 * @return void
 **/
static void show_error_message(void)
{
    GtkWidget *dlg = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL,
                        GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "%s %s",
                        _("Plugin"), PLUGIN_NAME);
    
    gtk_message_dialog_format_secondary_markup(GTK_MESSAGE_DIALOG(dlg), "%s",
        _("There is currently no terminal loaded in Geany. Enable the terminal "
        "in Geany's prefrences dialog and restart Geany to use the plugin "
        "or disable the plugin to stop seeing this error message."));
    
    gtk_dialog_run(GTK_DIALOG(dlg));
    gtk_widget_destroy(dlg);
    g_free(cmd_string);
}
