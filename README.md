# *N*panel-UI

A user interface designed to implement the **[Trojan-Go](https://github.com/p4gefau1t/trojan-go) API** to manage users or monitor the server.

I am currently working on telegram bot with many features.general testing is done. it will be released today!

![image](https://user-images.githubusercontent.com/123581768/221713162-05470cbf-f118-44f5-ac91-af825c42a229.png)

Written with C++ and compiles to webassembly which allows fast encryption/decryption of packets being 
sent/received from server &  uses a  modern bloat free ui-  library [Dear ImGui](https://github.com/ocornut/imgui)

Features:
- all users on same port
- reverse proxy with customizable pre-made templates
- panel and reverse-proxy and users all on same port
- real-time data such as speed, traffic usage (sortable)
- telegram bot with admin notifications and users status support
- by the telegram bot , you can change domain/ip when ever you needed and the bot sends the latest updated configs
  to all users
- user limits possible: speed , ip count , traffic total , duration(days)
- panel and backend developed with native programing languages without any scripting language, allows a fast and cheap server side with less cpu usage
- panel omits any identifiable resource with the name Npanel even in html,js contents (thanks to webassembly)
- panel uses 1 additional layer of AES-256 encryption for all kinds of data being transfered
- panel uses multiple separated process which allows the user configs to work even if Npanel crashes by a bug!

# Install
## Linux
>**Tested on Ubuntu 20**


```sh
wget  "https://raw.githubusercontent.com/Leiren/Npanel/master/scripts/install.sh" -O install.sh && chmod +x install.sh && bash install.sh
```

>This script is also used to update. but you should always backup before executing.

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
 Npanel on Linux is placed inside /opt folder so
 - In order to backup your users: /opt/Npanel/users.db
 - In order to backup your settings: /opt/Npanel/panel.json
 - It is suggested to Backup the folder /opt/Npanel entirely.
 
 on Windows The Npanel folder is your backup indeed.
 # Transfer
on Linux you should backup /opt/Npanel Folder on old server then
- install npanel using install script on destination server.
- on destination server replace /opt/Npanel with your backup from old server
- service npanel restart

on Windows you can just copy the Npanel folder and move it to destination server since it is portable.


# Uninstall
on Linux:
- service npanel stop
- rm -rf /opt/Npanel
- rm /etc/systemd/system/npanel.service
# Notes
 - This UI is only a front-end application and it is useless by it self, you must implement your server side functions by your favorite language like python/c/etc.. or use the provided server which has conditions.
 
 - current server support always disables ipv6 , enables bbr , disables ufw to avoid problems. (Linux only)
 
-  current server support  always kills ports 80,443 (for global serving) and ports 2060,2061 for local listening at startup. 
- during the setup wizard, you configure the server by connecting to port 2060; but after setup is finished only ports 80 , 443 are opened globaly and ports 2060,2061 are used locally.
 - This project is under active development. you are welcome to report bugs.
 
-  If you want to reduce the risk of your server getting blocked, tell your users to use clients that support uTls. (lastest version of most clients do it)
