#!/usr/bin/env bash
echo CURSES DIALOG TEST
echo "This is a test of how curses dialogs are handled.  Expect freaky behaviour."

dialog --title "Dialog title" --inputbox "Enter your name:" 0 0

exit $?
