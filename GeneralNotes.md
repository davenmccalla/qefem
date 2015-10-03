# Introduction #

Qt based filemanager with two panels. The file actions are running in a separate thread,
the spinning icon on the left indicates running actions. It is some kind of extension of command line, it is for power users.

NOTE: To use zip functionality on Win please install 7z from www.7-zip.org.

NOTE: On Ubuntu Linux it can be started by executing /usr/bin/Qefem

# New features in 0.6 #
  * ALT+e Opens the current file in editor
  * ALT+t Opens a terminal in the current directory (on OSX the current dir doesn't work)
  * ALT+f Searches for files

# Features #

  * History tab (the last 128 dirs are saved into home dir/.Qefem/.lhistory or .rhistory depending on left or right panel)
  * Bookmark tab ( bookmarks are saved to home dir/.Qefem/.bookmarks )
  * Drive tab removed from Maemo release (compiles and works on Maemo)
  * OSX and Win and Ubuntu release
  * CTRL+C and CTRL+V support
  * Drag and Drop support
  * Multiselection
  * .zip files are recognized by dubbleclicking and unziped to the other panels current folder

# Shortcuts #

  * ALT+1 Left drive tab
  * ALT+2 Right drive list
  * ALT+3 Right file list
  * ALT+4 Left file list
  * ALT+5 Right History tab
  * ALT+6 Right History tab
  * ALT+7 Left Bookmarks tab
  * ALT+8 Right Bookmarks tab
  * ALT+c copy
  * ALT+r rename
  * ALT+d delete
  * ALT+m make dir
  * ALT+z zip
  * ALT+s switch to status

# Plans #

  * Maemo release
  * network drive support
  * checking the aviable size before copy
  * Changing path in path editor
  * CTRL-X support
  * Xml config file
  * Tooltip of the file should show the properties.
  * Shortcut for editor or view or for opening a terminal
  * Preferences in XML
  * Implement file move
  * Sorting by name size date
  * adding error console to see errors
  * ?S60 release?

# Development blog #

[http://qefem.blogspot.com/](http://qefem.blogspot.com/)


# Thanks #

Thanks to Z0z0 and other anonymous contributors for help.