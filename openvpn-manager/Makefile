include $(TOPDIR)/rules.mk

PKG_NAME:=openvpn-manager
PKG_RELEASE:=1
PKG_VERSION:=1.0.0

include $(INCLUDE_DIR)/package.mk

define Package/openvpn-manager
	CATEGORY:=Base system
	TITLE:=openvpn-manager
	DEPENDS:=+libubus +libubox +libblobmsg-json +libjson-c
endef

define Package/openvpn-manager/description
	OpenVPN server manager
endef

define Package/openvpn-manager/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/openvpn-manager $(1)/usr/bin
endef

$(eval $(call BuildPackage,openvpn-manager))