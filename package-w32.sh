#!/bin/bash
zip -r athgame-w32--`date -u +"%Y-%m-%d--%H-%M-%S--"``git log -1 --format="%H"`.zip *.bmp main.exe *.agg fonts
