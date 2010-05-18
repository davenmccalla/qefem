QeFeM

(0.5 beta)
Qt based filemanager with two panels. The file actions are running in a separate thread, the spinning icon on the left indicates running actions.

To use zip functionality on Win please install 7z from www.7-zip.org.

Use case:
Have you ever compared the time difference of the command line and any GUI file manager copy and delete?
When there are thousand of files and hundreds of GB to copy, delete and unzip/zip this can be tens of minutes.
The intention of this application is to make it fast as the command line, but still easy to use.
There are no plans for progressbars, but maybe some more feedback will be on a way.

New features:
- ALT+e opens a file in editor
- ALT+t opens a terminal at the current directory
- Renaming of directories works

Features:
- Tabs are getting shortcuts ALT+1 Left drive tab, ALT+2 Right drive list, ALT+3 Right file list, ALT+4 Left file list, ALT+5 Right History tab, ALT+6 Right History tab, ALT+7 Left Bookmarks tab, ALT+8 Right Bookmarks tab
- Free space is shown
- Changing path from path editor works
- History tab (the last 128 dirs are saved into home dir/.Qefem/.lhistory or .rhistory depending on left or right panel)
- Bookmark tab ( bookmarks are saved to home dir/.Qefem/.bookmarks )
- Drive tab removed from Maemo release
- OSX, Linux and Win release
- CTRL+C and CTRL+V support
- Drag and Drop support
- Multiselection
- ALT+c copy, ALT+r rename, ALT+d delete, ALT+m make dir, ALT+z zip, ALT+s switch to status
- *.zip files are recognized by dubbleclicking and unziped to the other panels current folder

Planned Todos:
- Maemo release
- checking the aviable size before copy
- CTRL-X support
- Xml config file
- Tooltip of the file should show the properties.
- Shortcut for editor or view or for opening a terminal
- Preferences in XML
- Implement file move
- Sorting by name size date
- adding error console to see errors
- ?S60 release?
- Help system
- Add queue functionality


