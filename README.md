stmm-games
==========

Game library for C++ (C++14).

For more info visit http://www.efanomars.com/libraries/stmm-games

This source package contains:

- libstmm-games:
    library that defines the base classes of the model and the
    interfaces of the view of a game

- libstmm-games-fake:
    library meant for developers that need to test their game
    based on libstmm-games

- libstmm-games-file:
    library that defines base classes for loaders

- libstmm-games-gtk:
    library that implements a view of a game with gtkmm, providing
    a Gtk::Window subclass that automatically generates preferences,
    highscore dialogs based on parameters passed to it

- libstmm-games-xml-base:
    library that provides common functionality for xml loaders

- libstmm-games-xml-game:
    library that provides a game definition language based
    on xml and persistency for preferences and highscores

- libstmm-games-xml-gtk:
    library that provides a theme definition language based
    on xml and bindings to gtk

Read the INSTALL file for installation instructions.


Warning
-------
The APIs of the libraries aren't stable yet.
