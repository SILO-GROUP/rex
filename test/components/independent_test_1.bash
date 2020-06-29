whoami
id

touch /home/bagira/testfile

stat /home/bagira/testfile

#dialog --stdout --title "Interact with me!" \
#  --backtitle "This is user interaction." \
#  --yesno "Yes: pass, No:  fail" 7 60
exit $?