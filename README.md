## OpenVPN Manager
### Installing

Enable OpenVPN management server from menuconfig and recompile:
```bash
make menuconfig
make -j$(($(nproc)+1)) package/network/services/openvpn/compile
```
## Connecting to OpenVPN server:
```
sudo openvpn client.ovpn
```

client.ovpn template:
```
dev tun
nobind
client
remote 192.168.1.1 1194 udp
auth-nocache

<key>
----- key -----
</key>

<cert>
----- cert -----
</cert>

<ca>
----- ca -----
</ca>
```

TODO: improve deployment script for better patch support and add option to just copy file(s) instead of recompiling