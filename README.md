# *N*panel-UI

A user interface designed to implement the **[Trojan-Go](https://github.com/p4gefau1t/trojan-go) API** to manage users or monitor the server,

* Supports trojan-tcp and trojan-ws

![image](https://user-images.githubusercontent.com/123581768/221713162-05470cbf-f118-44f5-ac91-af825c42a229.png)

Written with C++ and compiles to webassembly which allows fast encryption/decryption of packets being 
sent/received from server &  uses a  modern bloat free ui-  library [Dear ImGui](https://github.com/ocornut/imgui)

Features:
- Reverse proxy with customizable pre-made templates

- Panel and reverse-proxy and users all on same port

- Real-time data such as speed, traffic usage (sortable)

- Telegram bot with admin notifications and users status support

- By using the telegram bot, you can change domain/ip when ever you needed and the bot sends the latest updated configs
  to all users with 1 click
  
- By using the telegram bot, you can broadcast messages to all users , or even singe user

- User limits possible: speed , ip count , traffic total , duration(days)

- Panel and backend developed with native programing languages without any scripting language, allows a fast and cheap server side with less cpu usage

- Panel omits any identifiable resource with the name Npanel even in html,js contents (thanks to webassembly)

- Panel uses 1 additional layer of AES-256 encryption for all kinds of data being transfered (only between the web-ui and server, not affecting trojan-go  tls strategy)

- Panel uses multiple separated process which allows the user configs to work even if Npanel crashes by a bug!

- Real time server resources monitoring with fast update rate 

- Supports windows server or even home pc

- Active support and fast bug fixes, just create a issue or dm me in telegram (link provided in about-page)


# Install
## Linux
>**Tested on Ubuntu 20**


```sh
wget  "https://raw.githubusercontent.com/Leiren/Npanel/master/scripts/install.sh" -O install.sh && chmod +x install.sh && bash install.sh
```

>This command is also used to update. 

## Windows

>**Tested on Windows 10 & server 2022**

>to Install: download the latest release and run .exe

>to Update: download the last release and move panel.json and users.db from old Npanel folder to new version.

>if you want to use it on your home Windows pc rather than a Windows server, you should configure your home router to forward ports 80,443 .

>**Important Notice on Windows:**
on first startup of the program (setup stage) you must allow ports 80,443,2060 in firewall settings by your self!
then open http://yourip:2060 on your browser and finish the setup stage.
after that, only ports 80,443 are required to be allowed in firewall settings.





#	Backup
 > Npanel has auto backup and restore, it is safe to backup but not required

 - on Linux all the data you can backup are in /opt/Npanel/AutoBackup/*
 
 - on Windows The Npanel folder is your backup indeed.
 
 - To restore, stop the panel then put your backup files to /opt/Npanel/* 
 
 # Transfer
on Linux you should backup /opt/Npanel Folder on old server then
- install npanel using install script on destination server.
- on destination server replace /opt/Npanel with your backup from old server
- service npanel restart

Note that you must bring your cert&key files from old server or generatie a new one.

on Windows you can just copy the Npanel folder and move it to destination server since it is portable.

# discussion and groups
Official group for Persian language (IRAN) is [available](https://t.me/Npanel_IR) !

> other languages groups will be added when they reach minimum 100 users.

# Uninstall
on Linux:
- service npanel stop
- rm -rf /opt/Npanel
- rm /etc/systemd/system/npanel.service
# Notes
 - This UI is only a front-end application and it is useless by it self, you must implement your server side functions by your favorite language like python/c/etc.. or use the provided server which has conditions.
 
 - current server support always enables bbr , disables ufw to avoid problems. (Linux only)
 
-  current server support  always kills ports 80,443 (for global serving) and ports 2060,2061 for local listening at startup. 
- during the setup wizard, you configure the server by connecting to port 2060; but after setup is finished only ports 80 , 443 are opened globaly and ports 2060,2061 are used locally.
 - This project is under active development. you are welcome to report bugs.
 
-  If you want to reduce the risk of your server getting blocked, tell your users to use clients that support uTls. (lastest version of most clients do it)
