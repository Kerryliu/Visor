#include <gtkmm.h>
#include <gdkmm.h>
#include <cairo.h>
#include <iostream>

Gtk::Window* pWindow = nullptr;

static
gboolean drawCallback(const Cairo::RefPtr<Cairo::Context>& context, int x) {
}


static
void on_button_clicked() {
  if(pWindow)
    pWindow->hide(); //hide() will cause main::run() to end.
}

int main (int argc, char **argv) {
  auto app = Gtk::Application::create(argc, argv, "org.gtkmm.example");

  //Load the GtkBuilder file and instantiate its widgets:
  auto refBuilder = Gtk::Builder::create();
  try {
    refBuilder->add_from_file("ui.glade");
  }
  catch(const Glib::FileError& ex) {
    std::cerr << "FileError: " << ex.what() << std::endl;
    return 1;
  }
  catch(const Glib::MarkupError& ex) {
    std::cerr << "MarkupError: " << ex.what() << std::endl;
    return 1;
  }
  catch(const Gtk::BuilderError& ex) {
    std::cerr << "BuilderError: " << ex.what() << std::endl;
    return 1;
  }

  //Get the GtkBuilder-instantiated Dialog:
  refBuilder->get_widget("Visor", pWindow);
  if(pWindow) {
    //Get the GtkBuilder-instantiated Button, and connect a signal handler:
    Gtk::Button *pButton = nullptr;
    Gtk::DrawingArea *pDrawingArea = nullptr;
    refBuilder->get_widget("quit_button", pButton);
    if(pButton) {
      pButton->signal_clicked().connect( sigc::ptr_fun(on_button_clicked) );
    }
    refBuilder->get_widget("Graph", pDrawingArea);
    if(pDrawingArea) {
      pDrawingArea
        ->signal_draw()
        .connect( sigc::ptr_fun(drawCallback));
    }

    app->run(*pWindow);
  }



  delete pWindow;

  return 0;
}
