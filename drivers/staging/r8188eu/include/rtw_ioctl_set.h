/* SPDX-License-Identifier: GPL-2.0 OR BSD-3-Clause */
/* Copyright(c) 2007 - 2011 Realtek Corporation. */

#ifndef __RTW_IOCTL_SET_H_
#define __RTW_IOCTL_SET_H_

#include "drv_types.h"

typedef u8 NDIS_802_11_PMKID_VALUE[16];

u8 rtw_set_802_11_add_key(struct adapter *adapt, struct ndis_802_11_key *key);
u8 rtw_set_802_11_authentication_mode(struct adapter *adapt,
				      enum ndis_802_11_auth_mode authmode);
u8 rtw_set_802_11_bssid(struct adapter*adapter, u8 *bssid);
u8 rtw_set_802_11_add_wep(struct adapter *adapter, struct ndis_802_11_wep *wep);
u8 rtw_set_802_11_disassociate(struct adapter *adapter);
u8 rtw_set_802_11_bssid_list_scan(struct adapter*adapter,
				  struct ndis_802_11_ssid *pssid,
				  int ssid_max_num);
u8 rtw_set_802_11_infrastructure_mode(struct adapter *adapter,
				      enum ndis_802_11_network_infra type);
u8 rtw_set_802_11_remove_wep(struct adapter *adapter, u32 keyindex);
u8 rtw_set_802_11_ssid(struct adapter *adapt, struct ndis_802_11_ssid *ssid);
u8 rtw_set_802_11_remove_key(struct adapter *adapt,
			     struct ndis_802_11_remove_key *key);
u8 rtw_validate_ssid(struct ndis_802_11_ssid *ssid);
u16 rtw_get_cur_max_rate(struct adapter *adapter);
int rtw_set_scan_mode(struct adapter *adapter, enum rt_scan_type scan_mode);
int rtw_set_channel_plan(struct adapter *adapter, u8 channel_plan);
int rtw_set_country(struct adapter *adapter, const char *country_code);
int rtw_change_ifname(struct adapter *padapter, const char *ifname);

#endif
