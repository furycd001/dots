#!/bin/bash
sudo clamscan -ir --exclude-dir=^/sys --exclude-dir=^/dev --exclude-dir=^/proc && echo  && echo [  S C A N  C O M P L E T E  ] && echo
