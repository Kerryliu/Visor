#include "window.h"

Window::Window() : m_VBox(Gtk::ORIENTATION_VERTICAL), m_Button_Quit("Quit") {
  // set_title("Visor");
  set_border_width(1);
  set_default_size(500, 600);

  Gtk::Window::set_titlebar(m_headerBar);
  add(m_VBox);

  // Add the TreeView, inside a ScrolledWindow:
  m_ScrolledWindow_first.add(tree->m_TreeView);
  m_ScrolledWindow_second.add(*graph);

  // Only show the scrollbars when they are necessary:
  m_ScrolledWindow_first.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
  m_ScrolledWindow_second.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

  // Attempt to make that stack:
  m_stack.add(m_ScrolledWindow_first, "status", "Status");
  m_stack.add(m_ScrolledWindow_second, "graphs", "Graphs");
  m_stackSwitcher.set_stack(m_stack);
  m_VBox.pack_end(m_stack);

  // HeaderBar:
  m_headerBar.set_show_close_button(true);
  m_headerBar.set_custom_title(m_stackSwitcher);

  show_all_children();
}

Window::~Window() {
  delete tree;
  delete graph;
}

void Window::on_button_quit() {
  tree->on_quit();
  hide();
}
