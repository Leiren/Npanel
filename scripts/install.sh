if [ "$EUID" -ne 0 ]
  then echo "Please run as root"
  exit
fi
echo nameserver 8.8.8.8 | sudo tee /etc/resolv.conf

apt-get update -y

REQUIRED_PKG="unzip"
PKG_OK=$(dpkg-query -W --showformat='${Status}\n' $REQUIRED_PKG|grep "install ok installed")
echo Checking for $REQUIRED_PKG: $PKG_OK
if [ "" = "$PKG_OK" ]; then
  echo "Setting up $REQUIRED_PKG."
  sudo apt-get --yes install $REQUIRED_PKG
fi

REQUIRED_PKG="wget"
PKG_OK=$(dpkg-query -W --showformat='${Status}\n' $REQUIRED_PKG|grep "install ok installed")
echo Checking for $REQUIRED_PKG: $PKG_OK
if [ "" = "$PKG_OK" ]; then
  echo "Setting up $REQUIRED_PKG."
  sudo apt-get --yes install $REQUIRED_PKG
fi

REQUIRED_PKG="iptables"
PKG_OK=$(dpkg-query -W --showformat='${Status}\n' $REQUIRED_PKG|grep "install ok installed")
echo Checking for $REQUIRED_PKG: $PKG_OK
if [ "" = "$PKG_OK" ]; then
  echo "Setting up $REQUIRED_PKG."
  sudo apt-get --yes install $REQUIRED_PKG
fi

printf  "\n"
printf  "\n"

DIR=/opt/Npanel
if [ -d "$DIR" ];
then
  
  echo "This will update Npanel to the latest version."
  printf  "\n"


  SUPDATE=1

else
	echo "This script will install Npanel."

fi
printf  "\n"

echo "continue?"
read check__


if [[ -v SUPDATE ]];
then
    service npanel stop
fi



cd /opt
# rm -rf Npanel
mkdir -p Npanel
cd Npanel
wget "https://github.com/Leiren/Npanel/releases/download/v0.2.8alpha/linux-amd64.zip" -O npanel_linux_amd64.zip
unzip -o npanel_linux_amd64.zip
mv -f npanel.service /etc/systemd/system
sudo systemctl daemon-reload
sudo systemctl enable npanel
sudo chmod +x Npanel
cd linux
sudo chmod +x trojan-go
sudo chmod +x gost-linux-amd64

printf  "\n"
printf  "\n"


echo "start Npanel with:"

printf  "\n"
echo "	service npanel start"

printf  "\n"

if [[ -v SUPDATE ]];
then
    echo "after that nothing else should be done. open your domain and use the panel."
else
    echo "after start open your ip with port 2060 example: http://1.2.3.4:2060"
fi



printf  "\n"

echo "if you want to check logs:"

printf  "\n"
echo "service npanel status"

printf  "\n"
echo "if you really want to stop Npanel:"

printf  "\n"
echo "service npanel stop"
printf  "\n"
printf  "\n"
echo "Installed at /opt/Npanel"

printf  "\n"
echo "Install completed. Npanel will auto start after system boot."
printf  "\n"


