#include <gtkmm/application.h>
#include "window.h"

int main(int argc, char *argv[]) {
  auto app = Gtk::Application::create(argc, argv);

  Window window;

  // Shows the window and returns when it is closed.
  return app->run(window);
}
