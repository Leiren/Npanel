if [ "$EUID" -ne 0 ]
  then echo "Please run as root"
  exit
fi

REQUIRED_PKG="unzip"
PKG_OK=$(dpkg-query -W --showformat='${Status}\n' $REQUIRED_PKG|grep "install ok installed")
echo Checking for $REQUIRED_PKG: $PKG_OK
if [ "" = "$PKG_OK" ]; then
  echo "No $REQUIRED_PKG. Setting up $REQUIRED_PKG."
  sudo apt-get --yes install $REQUIRED_PKG
fi

REQUIRED_PKG="wget"
PKG_OK=$(dpkg-query -W --showformat='${Status}\n' $REQUIRED_PKG|grep "install ok installed")
echo Checking for $REQUIRED_PKG: $PKG_OK
if [ "" = "$PKG_OK" ]; then
  echo "No $REQUIRED_PKG. Setting up $REQUIRED_PKG."
  sudo apt-get --yes install $REQUIRED_PKG
fi


echo "This will remove old Npanel completely(if installed) and install latels version"
echo "continue?"
read check__
cd /opt
rm -rf Npanel
mkdir -p Npanel
cd Npanel
wget -c "url" -O npanel_linux_amd64.zip
unzip npanel_linux_amd64.zip
mv -f npanel.service /etc/systemd/system
sudo systemctl daemon-reload
sudo systemctl enable npanel
sudo chmod +x Npanel
cd linux
sudo chmod +x trojan-go


echo "start Npanel with:"
echo "service npanel start"
echo "after start open your ip with port 2060 example: http://1.2.3.4:2060"

echo "if you want to check logs:"
echo "service npanel status"
echo "if you really want to stop Npanel:"
echo "service npanel stop"
echo ""
echo ""

echo "Install completed. Npanel will auto start after system boot."
echo ""

echo "if you want to disable start after boot:"
echo ""
