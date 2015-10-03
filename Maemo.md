# Maemo specific notes #

Due to the smaller screen the UI required some modification. It became fullscreen and therefor got an "Exit" button. It also got an "Open" button to open files or directories, when ".." is selected then it moves one directory upward.
The path editing edit box, has been removed due to lack of space, like the history tab too.
The file list contains one extra line at the bottom which shows the location of the current directory.

# Development for Maemo #

## Code ##

In defines.h it is possible to define QEFEM\_MAEMO\_DEV , when it is defined qefem behaves the same way on desktop like on Maemo device. Makes the development easier.

## Package making for Maemo ##

http://wiki.forum.nokia.com/index.php/Creating_Debian_packages_for_Maemo_5_Qt_applications_and_showing_in_the_application_menu

To build the package:

dpkg-buildpackage -rfakeroot -uc -b

To list the packge content:

dpkg --contents ../qefem\_0.5-1\_armel.deb