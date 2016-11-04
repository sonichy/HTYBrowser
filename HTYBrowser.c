#include <webkit/webkit.h>
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <string.h>
GError *error = NULL;
gint context_id;
GtkWidget *urientry,*searchEntry,*SNEntry,*window,*searchWindow,*progressbar, *statusbar,*notebook,*menubutton;
char *path;
int isShowSearch=0;
WebKitWebView* createWebView(WebKitWebView  *webview, WebKitWebFrame *frame, gpointer user_data);
void addTab();
GString *path1;

void setTabURI()
{    
  int page_num=gtk_notebook_get_current_page (GTK_NOTEBOOK(notebook));
  g_print ("setTab %d\n",page_num);
  GtkWidget *child= gtk_notebook_get_nth_page (GTK_NOTEBOOK(notebook), page_num);
  GtkWidget *webview= gtk_bin_get_child (GTK_BIN(child));
  const gchar *title=webkit_web_view_get_title(WEBKIT_WEB_VIEW(webview));
  g_print ("Page Title: %s\n",title);
  //gtk_notebook_set_tab_label_text (GTK_NOTEBOOK(notebook),child,title);
  gtk_entry_set_text (GTK_ENTRY (urientry), webkit_web_view_get_uri (WEBKIT_WEB_VIEW(webview)));
  const gchar *favicon=webkit_web_view_get_icon_uri (WEBKIT_WEB_VIEW(webview));
  g_print ("favicon= %s\n",favicon);
  gdouble p=webkit_web_view_get_progress (WEBKIT_WEB_VIEW(webview));
  if(p=1){gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (progressbar), 0);}
  else{gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (progressbar), p);}
  //GtkWidget *tab=gtk_notebook_get_menu_label (GTK_NOTEBOOK(notebook), GtkWidget *child);
}

void closeCTab()
{ 
  gtk_notebook_remove_page (GTK_NOTEBOOK(notebook), gtk_notebook_get_current_page (GTK_NOTEBOOK(notebook))); 
	gint n=gtk_notebook_get_n_pages (GTK_NOTEBOOK(notebook));
  if(n<1)addTab();
  gtk_widget_queue_draw (GTK_WIDGET (notebook));
}

void closeTab(GtkButton *button, GtkWidget *notebook)
{  
  GtkWidget *page= GTK_WIDGET (g_object_get_data (G_OBJECT (button), "page"));
  int page_num = gtk_notebook_page_num (GTK_NOTEBOOK(notebook), page);
  gtk_notebook_remove_page (GTK_NOTEBOOK(notebook), page_num);
  gint n=gtk_notebook_get_n_pages (GTK_NOTEBOOK(notebook));
  if(n<1)addTab();
  gtk_widget_queue_draw (GTK_WIDGET (notebook));
}

void titleChanged (WebKitWebView* webview, WebKitWebFrame* web_frame, const gchar* title, gpointer data)
{
  printf ("titleChanged\n");
  GString *wtitle=g_string_new(title);
  g_string_append(wtitle, " - 海天鹰浏览器");
  gtk_window_set_title (GTK_WINDOW (window), wtitle->str);
  int page_num=gtk_notebook_get_current_page (GTK_NOTEBOOK(notebook));
  GtkWidget *page= gtk_notebook_get_nth_page (GTK_NOTEBOOK(notebook), page_num);
  GtkWidget *tab=gtk_notebook_get_tab_label(GTK_NOTEBOOK(notebook),GTK_WIDGET(page));
  GList *tabChild=gtk_container_get_children(GTK_CONTAINER(tab)); 
  GList *lili=g_list_nth(tabChild,1);
  GtkLabel *label=lili->data;
  gtk_label_set_text(label,title);
}

void progress (WebKitWebView * webview, gint progress, gpointer user_data)
{
  //printf ("%d%%\n", progress);
  gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (progressbar), progress / 100.0);
}

void load_finished (WebKitWebView * webview, gpointer data)
{
  printf ("load_fininshed\n");
  printf ("get_uri:%s\n", webkit_web_view_get_uri (webview));
  printf ("get_title:%s\n", webkit_web_view_get_title (webview));
  gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (progressbar), 0);
  gtk_entry_set_text (GTK_ENTRY (urientry), webkit_web_view_get_uri (webview));
  int page_num=gtk_notebook_get_current_page (GTK_NOTEBOOK(notebook));
  GtkWidget *page= gtk_notebook_get_nth_page (GTK_NOTEBOOK(notebook), page_num);
  GtkWidget *webviewn= gtk_bin_get_child (GTK_BIN(page));
  if(webkit_web_view_get_view_source_mode(WEBKIT_WEB_VIEW(webviewn))){   
   GtkWidget *tab=gtk_notebook_get_tab_label(GTK_NOTEBOOK(notebook),GTK_WIDGET(page));
   GList *tabChild=gtk_container_get_children(GTK_CONTAINER(tab));
   GtkImage *image = g_list_first(tabChild)->data;
   gtk_image_set_from_file(image,"favicon.ico");
  }
}

void load_status (WebKitWebView * webview, WebKitLoadStatus status) {
 printf ("called_load_status\n");
}

void hoverLink (WebKitWebView *web_view, gchar *title, gchar *uri, gpointer user_data)
{
  gtk_statusbar_pop(GTK_STATUSBAR(statusbar),context_id);
  if(uri)gtk_statusbar_push (GTK_STATUSBAR(statusbar), context_id, uri);
}

gboolean moveCursor (WebKitWebView  *web_view, GtkMovementStep step, gint count, gpointer user_data)
{
  g_message ("moveCursor");
  //gtk_statusbar_push (GTK_STATUSBAR (statusbar), context_id, "");
  return TRUE;
}
static gboolean receivedDataCallback(WebKitDownload* download, guint64 dataLength, gpointer user_data)
{
  gdouble p=webkit_download_get_progress(download);
  g_print("down:%f\n",p);
  return TRUE;
}
static void downloadStatus(GObject* object, GParamSpec* pspec, gpointer data)
{
  WebKitDownload* download = WEBKIT_DOWNLOAD(object);
  switch (webkit_download_get_status(download))
  {
    case WEBKIT_DOWNLOAD_STATUS_CREATED:
      g_print("Download Created\n");
      break;
    case WEBKIT_DOWNLOAD_STATUS_STARTED:
      g_print("Download Started\n");
      //g_signal_connect(download, "received-data", G_CALLBACK(receivedDataCallback),NULL);
      break;
    case WEBKIT_DOWNLOAD_STATUS_FINISHED:
      g_print("Download Finished\n");
      break;
    case WEBKIT_DOWNLOAD_STATUS_ERROR:
      g_print("Download Error\n");
      break;
    case WEBKIT_DOWNLOAD_STATUS_CANCELLED:
      g_print("Download Canceled\n");
      break;
    default:
      g_print("Download Status Other\n");
      break;
    }
  gdouble p=webkit_download_get_progress(download);
  g_print("down:%f\n",p);
}

gboolean downloadRequested(WebKitWebView *webview, WebKitDownload *download, gpointer user_data)
{
  g_message("downloadRequested");
  const gchar *filename = webkit_download_get_suggested_filename(download);
  g_message (filename);
  GtkWidget *FCD = gtk_file_chooser_dialog_new ("保存文件", NULL, GTK_FILE_CHOOSER_ACTION_SAVE, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,	GTK_STOCK_SAVE,	GTK_RESPONSE_ACCEPT, NULL);
  gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(FCD),filename);
  //gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(FCD),g_get_home_dir());
  if(gtk_dialog_run (GTK_DIALOG (FCD)) == GTK_RESPONSE_ACCEPT)
  {
    gchar *filepath = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(FCD));
    g_message (filepath);
    GString *statusText=g_string_new(filepath);
    g_string_prepend(statusText, "文件保存到 ");
    gtk_statusbar_push (GTK_STATUSBAR (statusbar), context_id, statusText->str);  
    gchar *dest = g_filename_to_uri (filepath, NULL, NULL);
    g_print("%s\n",dest);
    webkit_download_set_destination_uri(download, dest);
    webkit_download_start(download);
    g_signal_connect(download, "notify::status", G_CALLBACK(downloadStatus), NULL);
  }
  gtk_widget_destroy (FCD);
  return TRUE;
}

gboolean contextMenu (WebKitWebView *webview, GtkWidget *default_menu, WebKitHitTestResult *hit_test_result, gboolean triggered_with_keyboard, gpointer user_data)
{
  g_print("contextMenu\n");
  //return TRUE;
}

void iconLoaded (WebKitWebView *webview, gchar *iconUri, gpointer user_data)
{
  g_print("iconLoaded : %s\n",iconUri);
  if(iconUri!=""){
  int page_num=gtk_notebook_get_current_page (GTK_NOTEBOOK(notebook));
  GtkWidget *page= gtk_notebook_get_nth_page (GTK_NOTEBOOK(notebook), page_num);  
  GtkWidget *tab=gtk_notebook_get_tab_label(GTK_NOTEBOOK(notebook),GTK_WIDGET(page));
  GList *tabChild=gtk_container_get_children(GTK_CONTAINER(tab));  
  GtkImage *image = g_list_first(tabChild)->data;
  GdkPixbuf *pixbuf=webkit_web_view_get_icon_pixbuf (WEBKIT_WEB_VIEW(webview));
  pixbuf=gdk_pixbuf_scale_simple(pixbuf,16,16,GDK_INTERP_HYPER);
  gtk_image_set_from_pixbuf(image,pixbuf);
  }
}

void loadCommit(WebKitWebView* webview, WebKitWebFrame* frame, gpointer data)
{
  g_print("loadCommit\n");
  int page_num=gtk_notebook_get_current_page (GTK_NOTEBOOK(notebook));
  GtkWidget *page= gtk_notebook_get_nth_page (GTK_NOTEBOOK(notebook), page_num);  
  GtkWidget *tab=gtk_notebook_get_tab_label(GTK_NOTEBOOK(notebook),GTK_WIDGET(page));
  GList *tabChild=gtk_container_get_children(GTK_CONTAINER(tab));
  GtkImage *image = g_list_first(tabChild)->data;
  gtk_image_set_from_file(image,"loading.gif");
  const gchar* uri = webkit_web_frame_get_uri(frame);
  if (uri) gtk_entry_set_text (GTK_ENTRY (urientry), uri);
  g_signal_connect (webview, "load-progress-changed", G_CALLBACK (progress), NULL);
  g_signal_connect (webview, "load-finished", G_CALLBACK (load_finished), NULL);
  g_signal_connect (webview, "hovering-over-link", G_CALLBACK (hoverLink), NULL);
  g_signal_connect (webview, "create-web-view", G_CALLBACK (createWebView), NULL);
  g_signal_connect (webview, "title-changed", G_CALLBACK (titleChanged), NULL);
  g_signal_connect(webview, "download-requested", G_CALLBACK(downloadRequested),NULL);
  //g_signal_connect(webview, "context-menu", G_CALLBACK(contextMenu),NULL);
  g_signal_connect(webview, "icon-loaded", G_CALLBACK(iconLoaded),NULL);
}

void GotoURI()
{
  int page_num=gtk_notebook_get_current_page (GTK_NOTEBOOK(notebook));
  GtkWidget *scrolled= gtk_notebook_get_nth_page (GTK_NOTEBOOK(notebook), page_num);
  GtkWidget *webview= gtk_bin_get_child (GTK_BIN(scrolled));
  //webkit_web_view_set_view_source_mode(WEBKIT_WEB_VIEW(webview),FALSE);
  webkit_web_view_load_uri (WEBKIT_WEB_VIEW (webview), gtk_entry_get_text (GTK_ENTRY (urientry)));
}

void history()
{
  addTab();
  GString *filename=g_string_new(path);
  g_string_prepend(filename, "file://");
  g_string_append(filename, "/about.htm");
  gtk_entry_set_text(GTK_ENTRY (urientry),filename->str);
  GotoURI();
}

void favorite()
{
  addTab();
  GString *filename=g_string_new(path);
  g_string_prepend(filename, "file://");
  g_string_append(filename, "/webfavls.htm");
  gtk_entry_set_text(GTK_ENTRY (urientry),filename->str);
  GotoURI();
}

void GoForward()
{
  int page_num=gtk_notebook_get_current_page (GTK_NOTEBOOK(notebook));
  GtkWidget *scrolled= gtk_notebook_get_nth_page (GTK_NOTEBOOK(notebook), page_num);
  GtkWidget *webview= gtk_bin_get_child (GTK_BIN(scrolled));
  if(webkit_web_view_can_go_forward (WEBKIT_WEB_VIEW(webview)))webkit_web_view_go_back_or_forward (WEBKIT_WEB_VIEW(webview),1);
}

void GoBack ()
{
  int page_num=gtk_notebook_get_current_page (GTK_NOTEBOOK(notebook));
  GtkWidget *scrolled= gtk_notebook_get_nth_page (GTK_NOTEBOOK(notebook), page_num);
  GtkWidget *webview= gtk_bin_get_child (GTK_BIN(scrolled));
  if(webkit_web_view_can_go_back ( WEBKIT_WEB_VIEW(webview) ) ) webkit_web_view_go_back (WEBKIT_WEB_VIEW(webview));
}


WebKitWebView* createWebView(WebKitWebView  *webview, WebKitWebFrame *frame, gpointer user_data);

gboolean showWebView (WebKitWebView *webview, gpointer user_data)
{
  printf ("showWebView\n");
  g_signal_connect (webview, "load-committed", G_CALLBACK (loadCommit), NULL); 
  GtkWidget *scrolled = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC); 
  gtk_container_add(GTK_CONTAINER(scrolled), GTK_WIDGET(webview));
  int cp=gtk_notebook_get_current_page (GTK_NOTEBOOK(notebook));
  printf ("notebook page %d\n",cp);
  GtkWidget *hbox=gtk_hbox_new (FALSE,0);
  gtk_widget_set_size_request (hbox, 100, 20);
  GtkWidget *image=gtk_image_new_from_file("loading.gif");
  gtk_box_pack_start (GTK_BOX(hbox),image,FALSE,FALSE,0);
  GtkWidget *label=gtk_label_new (NULL);
  gtk_label_set_text (GTK_LABEL(label),"新标签");
  gtk_label_set_width_chars (GTK_LABEL(label),7);
  gtk_box_pack_start (GTK_BOX(hbox),label,FALSE,TRUE,0);  
  GtkWidget *button=gtk_button_new();
  gtk_button_set_relief(GTK_BUTTON(button), GTK_RELIEF_NONE);
  image=gtk_image_new_from_stock(GTK_STOCK_CLOSE,GTK_ICON_SIZE_MENU);
  gtk_button_set_image(GTK_BUTTON(button),image);
  g_object_set_data (G_OBJECT (button), "page", scrolled);
  g_signal_connect (button, "clicked", G_CALLBACK (closeTab), notebook);
  gtk_box_pack_end(GTK_BOX (hbox), button, FALSE, TRUE, 0);  
  gtk_box_pack_start (GTK_BOX(hbox),image,FALSE,TRUE,0);
  gtk_widget_show_all (hbox);
  gtk_notebook_insert_page(GTK_NOTEBOOK(notebook),scrolled,hbox,-1);
  gtk_widget_show_all(notebook);
  int n=gtk_notebook_get_n_pages (GTK_NOTEBOOK(notebook));
  gtk_notebook_set_current_page (GTK_NOTEBOOK(notebook), n-1);
  return TRUE;
}

WebKitWebView* createWebView (WebKitWebView  *webview, WebKitWebFrame *frame, gpointer user_data)
{
	printf ("createWebView\n");
	GtkWidget* newWebView = webkit_web_view_new();
	g_signal_connect (G_OBJECT (newWebView), "web-view-ready", G_CALLBACK (showWebView), NULL);
	return WEBKIT_WEB_VIEW(newWebView);
}

gboolean changeCurrentPage (GtkNotebook *notebook, gint arg1, gpointer user_data)
{
  //g_message ("Page %d\n",arg1);
  return TRUE;
}

void addTab()
{
	printf ("addTab\n");
	GtkWidget *scrolled = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  GtkWidget *webview = webkit_web_view_new ();
  GString *filepath=g_string_new(path);
  g_string_prepend(filepath, "file://");
  g_string_append(filepath, "/webfavls.htm");
  printf ("fav path=%s\n", path);
  webkit_web_view_load_uri (WEBKIT_WEB_VIEW (webview), filepath->str);
  g_signal_connect (webview, "load-committed", G_CALLBACK (loadCommit), NULL);
  gtk_container_add (GTK_CONTAINER (scrolled), webview);
  GtkWidget *hbox=gtk_hbox_new (FALSE,0);
  gtk_widget_set_size_request (hbox, 100, 20);
  GtkWidget *image=gtk_image_new_from_file("favicon.ico");
  gtk_box_pack_start (GTK_BOX(hbox),image,FALSE,FALSE,0);
  GtkWidget *label=gtk_label_new (NULL);
  gtk_label_set_text (GTK_LABEL(label),"新标签");
  gtk_label_set_width_chars (GTK_LABEL(label),7);
  gtk_box_pack_start (GTK_BOX(hbox),label,FALSE,TRUE,1);
  GtkWidget *button=gtk_button_new();
  gtk_button_set_relief(GTK_BUTTON(button), GTK_RELIEF_NONE);
  image=gtk_image_new_from_stock(GTK_STOCK_CLOSE,GTK_ICON_SIZE_MENU); 
  gtk_button_set_image(GTK_BUTTON(button),image);
  g_object_set_data (G_OBJECT (button), "page", scrolled);
  g_signal_connect (button, "clicked", G_CALLBACK (closeTab), notebook);
  gtk_box_pack_end(GTK_BOX (hbox), button, FALSE, TRUE, 0); 
  gtk_box_pack_start (GTK_BOX(hbox),image,FALSE,TRUE,0);
  gtk_widget_show_all (hbox);
  gtk_notebook_insert_page(GTK_NOTEBOOK(notebook),scrolled,hbox,-1);
  gtk_widget_show_all(GTK_WIDGET(notebook));
  gtk_widget_queue_draw (GTK_WIDGET (notebook));
  int n=gtk_notebook_get_n_pages (GTK_NOTEBOOK(notebook));
  gtk_notebook_set_current_page (GTK_NOTEBOOK(notebook), n-1);
  gtk_widget_show_all(notebook);
}

void switchPage (GtkNotebook *notebook, GtkNotebookPage *page, guint page_num, gpointer user_data)
{  
  g_print ("switchPage %d\n",page_num);
  GtkWidget *webview= gtk_bin_get_child (GTK_BIN(page));
  GString *title=g_string_new(webkit_web_view_get_title(WEBKIT_WEB_VIEW (webview)));  
  g_string_append(title, " - 海天鹰浏览器");
  gtk_window_set_title (GTK_WINDOW (window), title->str);
  g_print ("Switch Page Title : %s\n",title->str);
  gtk_entry_set_text (GTK_ENTRY (urientry), webkit_web_view_get_uri (WEBKIT_WEB_VIEW(webview)));  
  gdouble p=webkit_web_view_get_progress (WEBKIT_WEB_VIEW(webview));
  if(p=1){gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (progressbar), 0);}
  else{gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (progressbar), p);}
}

void closeSearchWindow()
{
  isShowSearch=0;
  int page_num=gtk_notebook_get_current_page (GTK_NOTEBOOK(notebook));
  //g_print ("search Tab %d\n",page_num);
  GtkWidget *child= gtk_notebook_get_nth_page (GTK_NOTEBOOK(notebook), page_num);
  GtkWidget *webview= gtk_bin_get_child (GTK_BIN(child));
  //webkit_web_view_set_highlight_text_matches (WEBKIT_WEB_VIEW(webview), FALSE);
  webkit_web_view_unmark_text_matches(WEBKIT_WEB_VIEW(webview));
  gtk_entry_set_text(GTK_ENTRY(SNEntry),"");
  gtk_widget_hide_all(searchWindow);
}

void searchPrev(GtkButton *button , gpointer user_data)
{
  //GtkWidget *entry1 = (GtkWidget *)user_data;
  int page_num=gtk_notebook_get_current_page (GTK_NOTEBOOK(notebook));
  g_print ("searchPrev Tab %d\n",page_num);
  GtkWidget *child= gtk_notebook_get_nth_page (GTK_NOTEBOOK(notebook), page_num);
  GtkWidget *webview= gtk_bin_get_child (GTK_BIN(child));
  guint hits = webkit_web_view_mark_text_matches (WEBKIT_WEB_VIEW(webview), gtk_entry_get_text (GTK_ENTRY (searchEntry)), FALSE, 0);
  webkit_web_view_set_highlight_text_matches (WEBKIT_WEB_VIEW(webview), TRUE);
  webkit_web_view_search_text (WEBKIT_WEB_VIEW(webview), gtk_entry_get_text (GTK_ENTRY (searchEntry)), FALSE, FALSE, TRUE);
}

void search(GtkEntry *entry , gpointer user_data)
{
  int page_num=gtk_notebook_get_current_page (GTK_NOTEBOOK(notebook));
  g_print ("search Tab %d\n",page_num);
  GtkWidget *child= gtk_notebook_get_nth_page (GTK_NOTEBOOK(notebook), page_num);
  GtkWidget *webview= gtk_bin_get_child (GTK_BIN(child));
  guint hits = webkit_web_view_mark_text_matches (WEBKIT_WEB_VIEW(webview), gtk_entry_get_text (GTK_ENTRY (searchEntry)), FALSE, 0);
  char num[50];
  sprintf(num,"共%d个",hits);
  gtk_entry_set_text(GTK_ENTRY(SNEntry),num);
  webkit_web_view_set_highlight_text_matches (WEBKIT_WEB_VIEW(webview), TRUE);
  webkit_web_view_search_text (WEBKIT_WEB_VIEW(webview), gtk_entry_get_text (GTK_ENTRY (searchEntry)), FALSE, TRUE, TRUE);
}

void createSearchWindow()
{
  g_print("createSearchWindow\n");
  GtkWidget *button,*image,*hbox;
  GClosure* closure;
  GtkAccelGroup* accel_group;
  searchWindow=gtk_window_new(GTK_WINDOW_TOPLEVEL);
  accel_group = gtk_accel_group_new();
  closure = g_cclosure_new(closeSearchWindow, 0, 0);
  gtk_accel_group_connect(accel_group,GDK_KEY_Escape,0,0,closure);
  gtk_window_add_accel_group(GTK_WINDOW(searchWindow), accel_group);
  gtk_window_set_decorated(GTK_WINDOW(searchWindow),FALSE);
  hbox = gtk_hbox_new (FALSE, 0);
  searchEntry = gtk_entry_new ();
  gtk_entry_set_has_frame (GTK_ENTRY (searchEntry),FALSE);
  g_signal_connect(searchEntry, "activate", G_CALLBACK(search), NULL);
  gtk_box_pack_start (GTK_BOX (hbox), searchEntry, FALSE, TRUE, 0);
  SNEntry = gtk_entry_new ();
  gtk_box_pack_start (GTK_BOX (hbox), SNEntry, TRUE, FALSE, 0);
  gtk_entry_set_editable (GTK_ENTRY (SNEntry),FALSE);
  gtk_entry_set_has_frame (GTK_ENTRY (SNEntry),FALSE);
  gtk_entry_set_width_chars (GTK_ENTRY (SNEntry),5);
  button=gtk_button_new();
  image=gtk_image_new_from_stock(GTK_STOCK_MEDIA_PREVIOUS,GTK_ICON_SIZE_SMALL_TOOLBAR);
  gtk_button_set_image(GTK_BUTTON(button),image);
  gtk_box_pack_start (GTK_BOX (hbox), button, FALSE, TRUE, 0);  
  g_signal_connect(button, "button_press_event", G_CALLBACK (searchPrev), NULL);
  button=gtk_button_new();
  image=gtk_image_new_from_stock(GTK_STOCK_MEDIA_NEXT,GTK_ICON_SIZE_SMALL_TOOLBAR);
  gtk_button_set_image(GTK_BUTTON(button),image);
  gtk_box_pack_start (GTK_BOX (hbox), button, FALSE, TRUE, 0);  
  g_signal_connect(button, "button_press_event", G_CALLBACK (search),NULL);
  button=gtk_button_new();
  gtk_button_set_relief(GTK_BUTTON(button), GTK_RELIEF_NONE);
  image=gtk_image_new_from_stock(GTK_STOCK_CLOSE,GTK_ICON_SIZE_SMALL_TOOLBAR);
  gtk_button_set_image(GTK_BUTTON(button),image);
  gtk_box_pack_start (GTK_BOX (hbox), button, FALSE, TRUE, 0);  
  g_signal_connect(button, "button_press_event", G_CALLBACK (closeSearchWindow), NULL);
  gtk_container_add (GTK_CONTAINER (searchWindow), hbox);
  gtk_window_set_keep_above(GTK_WINDOW(searchWindow),TRUE);
  gtk_window_set_skip_taskbar_hint(GTK_WINDOW(searchWindow),TRUE);
}

void showSearchWindow()
{
  g_print("showSearchWindow\n");
  isShowSearch=1;
  gint wx, wy,mx,my;
  gdk_window_get_origin(gtk_widget_get_window (window), &wx, &wy);
  gdk_window_get_origin(gtk_widget_get_window (menubutton), &mx, &my);
  gtk_window_move(GTK_WINDOW(searchWindow),wx+window->allocation.width-searchWindow->allocation.width, my+ menubutton->allocation.height);
  gtk_widget_show_all(searchWindow);
}

void about (GtkWidget *widget, gpointer data)
{
  g_message ("about...");
  GtkWidget *dialog;
  const gchar *authors[] = { "sonichy@163.com", NULL };
  const gchar *documenters[] = { "https://src.chromium.org/blink/branches/chromium/648/Tools/GtkLauncher/main.c", NULL };
  dialog = gtk_about_dialog_new ();
  //gtk_widget_set_size_request(GTK_WIDGET(window),200,100);
  gtk_about_dialog_set_logo (GTK_ABOUT_DIALOG (dialog), NULL);
  gtk_about_dialog_set_name (GTK_ABOUT_DIALOG (dialog), "海天鹰浏览器");
  gtk_about_dialog_set_version (GTK_ABOUT_DIALOG (dialog), "2.0");
  gtk_about_dialog_set_copyright (GTK_ABOUT_DIALOG (dialog), "Copyright © 2016 黄颖");
  gtk_about_dialog_set_comments (GTK_ABOUT_DIALOG (dialog), "海天鹰浏览器Linux平台下一款基于WebkitGtk+的多标签浏览器。");
  gtk_about_dialog_set_website (GTK_ABOUT_DIALOG (dialog), "http://sonichy.96.lt");
  gtk_about_dialog_set_authors (GTK_ABOUT_DIALOG (dialog), authors);
  gtk_about_dialog_set_documenters (GTK_ABOUT_DIALOG (dialog), documenters);
  gtk_about_dialog_set_translator_credits (GTK_ABOUT_DIALOG (dialog), "黄颖\nsonichy");
  gtk_about_dialog_set_license (GTK_ABOUT_DIALOG (dialog), "GNU GPL");
  gtk_dialog_run (GTK_DIALOG (dialog));
  gtk_widget_destroy (dialog);
}

void set_position (GtkMenu *menu, gint *px, gint *py, gboolean *push_in, gpointer data)  
{
  gint wx, wy;
  gdk_window_get_origin (gtk_widget_get_window (menubutton), &wx, &wy);
  //*px=wx; 
  *py =wy + menubutton->allocation.height;
}

void viewSource()
{
  g_print ("viewSource\n");
  const gchar *uri=gtk_entry_get_text (GTK_ENTRY (urientry));
  addTab();
  int page_num=gtk_notebook_get_current_page (GTK_NOTEBOOK(notebook));
  GtkWidget *page= gtk_notebook_get_nth_page (GTK_NOTEBOOK(notebook), page_num);
  GtkWidget *tab=gtk_notebook_get_tab_label(GTK_NOTEBOOK(notebook),GTK_WIDGET(page));
  GList *tabChild=gtk_container_get_children(GTK_CONTAINER(tab));
  GtkImage *image = g_list_first(tabChild)->data;
  gtk_image_set_from_file(image,"favicon.ico");
  GList *lili=g_list_nth(tabChild,1);
  GtkLabel *label=lili->data;
  GString *title=g_string_new(uri);
  g_string_prepend(title, "源码：");
  gtk_label_set_text(label,title->str);
  gtk_window_set_title (GTK_WINDOW (window), title->str);
  GtkWidget *webview= gtk_bin_get_child (GTK_BIN(page));
  webkit_web_view_set_view_source_mode(WEBKIT_WEB_VIEW(webview),TRUE);
  webkit_web_view_load_uri (WEBKIT_WEB_VIEW (webview), uri);
}

gboolean mainmenu(GtkWidget *widget, GdkEventButton *event, gpointer data)
{
  if(event->button == 1){
  GtkWidget *menu,*item;
  menu = gtk_menu_new();
  item = gtk_menu_item_new_with_label("查找                 Ctrl+F");
  //item = gtk_image_menu_item_new_from_stock(GTK_STOCK_FIND,NULL);
  g_signal_connect(G_OBJECT(item), "activate", G_CALLBACK(showSearchWindow), NULL);
  gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
  item = gtk_menu_item_new_with_label("查看源码");
  g_signal_connect(G_OBJECT(item), "activate", G_CALLBACK(viewSource), NULL);
  gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
  item = gtk_menu_item_new_with_label("收藏夹");
  g_signal_connect(G_OBJECT(item), "activate", G_CALLBACK(favorite), NULL);
  gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
  item = gtk_menu_item_new_with_label("更新历史");
  g_signal_connect(G_OBJECT(item), "activate", G_CALLBACK(history), NULL);
  gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
  item = gtk_menu_item_new_with_label("关于                F1");
  //item = gtk_image_menu_item_new_from_stock(GTK_STOCK_ABOUT,NULL);
  g_signal_connect(G_OBJECT(item), "activate", G_CALLBACK(about), NULL);
  gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
  gtk_widget_show_all(GTK_WIDGET(menu));
  gtk_menu_popup(GTK_MENU(menu), NULL, NULL, set_position, NULL, event->button, event->time);
  }
  return TRUE;
}

gboolean windowState(GtkWidget *widget, GdkEventWindowState *event, gpointer user_data)
{  
  switch(gdk_window_get_state(GDK_WINDOW(widget->window)))
  {
    case GDK_WINDOW_STATE_WITHDRAWN:
      printf("GDK_WINDOW_STATE_WITHDRAWN:the window is not shown.\n");
      break;
    case GDK_WINDOW_STATE_ICONIFIED:
      printf("window minimized\n");
      if(isShowSearch)closeSearchWindow();
      break;
    case GDK_WINDOW_STATE_MAXIMIZED:
      printf("window maximized , isShowSearch=%d\n",isShowSearch);
      if(isShowSearch)showSearchWindow();
      break;
    case GDK_WINDOW_STATE_STICKY: printf("GDK_WINDOW_STATE_STICKY:the window is sticky.\n");break;
    case GDK_WINDOW_STATE_FULLSCREEN: printf("GDK_WINDOW_STATE_FULLSCREEN:the window is maximized without decorations.\n");break;							
	  case GDK_WINDOW_STATE_ABOVE: printf("GDK_WINDOW_STATE_ABOVE:the window is kept above other windows. \n");break;
    case GDK_WINDOW_STATE_BELOW: printf("GDK_WINDOW_STATE_BELOW:the window is kept below other windows. \n");break;
    default:break;
  }
  return TRUE;
}

gboolean expose(GtkWidget *widget, GdkEventExpose *event, gpointer user_data)
{
  if(isShowSearch)showSearchWindow();
  return TRUE;
}

int main (int argc, char **argv)
{
  GtkWidget *webview,*scrolled, *mainbox, *vbox, *hbox, *button,*image,*tab;
  GClosure* closure;
  GtkAccelGroup* accel_group;
  for(int i=0;i<argc;i++)printf("argv[%d]=%s\n",i,argv[i]);
  path=g_get_current_dir();
  //printf("path=%s\n",path);
  gtk_init (&argc, &argv); 
  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window), "海天鹰浏览器");
  gtk_window_set_default_size (GTK_WINDOW (window), 1024, 768);
  gtk_window_set_position (GTK_WINDOW (window), GTK_WIN_POS_CENTER);
  gtk_window_set_default_icon (gdk_pixbuf_new_from_file ("icon.png", &error));  
  g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
  g_signal_connect(window,"window_state_event",G_CALLBACK(windowState),NULL);
  accel_group = gtk_accel_group_new();
  closure = g_cclosure_new(showSearchWindow, 0, 0);  
  gtk_accel_group_connect(accel_group,GDK_KEY_f,GDK_CONTROL_MASK,0,closure);
  gtk_window_add_accel_group(GTK_WINDOW(window), accel_group);
  closure = g_cclosure_new(GotoURI, 0, 0);  
  gtk_accel_group_connect(accel_group,GDK_KEY_F5,0,0,closure);
  gtk_window_add_accel_group(GTK_WINDOW(window), accel_group);
  closure = g_cclosure_new(closeSearchWindow, 0, 0);  
  gtk_accel_group_connect(accel_group,GDK_KEY_Escape,0,0,closure);
  gtk_window_add_accel_group(GTK_WINDOW(window), accel_group);
  //g_signal_connect(window,"expose-event",G_CALLBACK(expose),NULL);
  
  mainbox = gtk_vbox_new (FALSE, 0);
  hbox = gtk_hbox_new (FALSE, 0); 
  
  button=gtk_button_new();
  image=gtk_image_new_from_stock(GTK_STOCK_GO_BACK,GTK_ICON_SIZE_SMALL_TOOLBAR);
  gtk_button_set_image(GTK_BUTTON(button),image);
  gtk_box_pack_start (GTK_BOX (hbox), button, FALSE, TRUE, 0);
  g_signal_connect (button, "clicked", G_CALLBACK (GoBack), NULL);
  
  button=gtk_button_new();
  image=gtk_image_new_from_stock(GTK_STOCK_GO_FORWARD,GTK_ICON_SIZE_SMALL_TOOLBAR);
  gtk_button_set_image(GTK_BUTTON(button),image);
  gtk_box_pack_start (GTK_BOX (hbox), button, FALSE, TRUE, 0);
  g_signal_connect (button, "clicked", G_CALLBACK (GoForward), NULL);
  
  urientry = gtk_entry_new ();
  //gtk_entry_set_text (GTK_ENTRY (urientry), "http://news.baidu.com/");
  g_signal_connect(urientry, "activate", G_CALLBACK(GotoURI), NULL);
  gtk_box_pack_start (GTK_BOX (hbox), urientry,TRUE, TRUE, 0);

  button=gtk_button_new();
  image=gtk_image_new_from_stock(GTK_STOCK_REFRESH,GTK_ICON_SIZE_SMALL_TOOLBAR);
  gtk_button_set_image(GTK_BUTTON(button),image);
  gtk_box_pack_start (GTK_BOX (hbox), button, FALSE, TRUE, 0);
  g_signal_connect (button, "clicked", G_CALLBACK (GotoURI), NULL);
  
  button=gtk_button_new();
  image=gtk_image_new_from_stock(GTK_STOCK_ADD,GTK_ICON_SIZE_SMALL_TOOLBAR);
  gtk_button_set_image(GTK_BUTTON(button),image);
  gtk_box_pack_start (GTK_BOX (hbox), button, FALSE, TRUE, 0);
  g_signal_connect (button, "clicked", G_CALLBACK (addTab), NULL);
  
  menubutton=gtk_button_new();
  image=gtk_image_new_from_stock(GTK_STOCK_PREFERENCES,GTK_ICON_SIZE_SMALL_TOOLBAR);
  gtk_button_set_image(GTK_BUTTON(menubutton),image);
  gtk_box_pack_start (GTK_BOX (hbox), menubutton, FALSE, TRUE, 0);  
  g_signal_connect(menubutton, "button_press_event", G_CALLBACK (mainmenu), NULL);
  
  gtk_box_pack_start (GTK_BOX (mainbox), hbox, FALSE, TRUE, 0);

  progressbar = gtk_progress_bar_new (); 
  GdkColor color;
  gdk_color_parse ("red", &color);
  gtk_widget_modify_bg (progressbar, GTK_STATE_PRELIGHT, &color);
  gtk_widget_set_size_request (progressbar, progressbar->allocation.width,3);
  vbox = gtk_vbox_new (FALSE, 0);
  gtk_box_pack_start (GTK_BOX (vbox), progressbar, FALSE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX (mainbox), vbox, FALSE, TRUE, 0);

  notebook = gtk_notebook_new();
  gtk_notebook_set_scrollable(GTK_NOTEBOOK(notebook),TRUE);
  g_signal_connect_after(notebook, "switch-page", G_CALLBACK(switchPage), NULL);
    
  gtk_box_pack_start (GTK_BOX (mainbox), notebook, TRUE, TRUE, 0);  

  statusbar = gtk_statusbar_new ();
  context_id = gtk_statusbar_get_context_id (GTK_STATUSBAR (statusbar), "Statusbar");
  gtk_statusbar_push (GTK_STATUSBAR (statusbar), context_id, "欢迎使用海天鹰浏览器");
  gtk_box_pack_start (GTK_BOX (mainbox), statusbar, FALSE, TRUE, 0);

  gtk_container_add (GTK_CONTAINER (window), mainbox);
  gtk_widget_show_all (window);  
  addTab();
  createSearchWindow();
  gtk_statusbar_push (GTK_STATUSBAR (statusbar), context_id,path);
  gtk_main ();
  return 0;
}
