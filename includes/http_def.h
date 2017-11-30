/*
 * http_def.h
 *
 *  Created on: 2017-1-13
 *      Author: root
 */

#ifndef _HTTP_DEF_H_
#define _HTTP_DEF_H_

#include "includes.h"

//webserver
#include "h_000.h"//登录界面
#include "h_001.h"//框架界面
#include "h_002.h"//概要信息
#include "h_003.h"//储能逆变器信息
#include "h_004.h"//系统参数
#include "h_005.h"//运行参数
#include "h_006.h"//保护参数
#include "h_007.h"//运行信息
#include "h_008.h"//事件信息
#include "h_009.h"//故障信息
#include "h_010.h"//系统信息
#include "h_011.h"//保留
#include "h_012.h"//logo区
#include "h_013.h"//菜单界面
#include "h_014.h"//替换子界面为登陆界面
#include "h_015.h"//光伏逆变器信息
#include "h_016.h"//光伏汇流箱信息
#include "h_017.h"//DCDC信息
#include "h_018.h"//BMS信息
#include "h_019.h"//系统配置界面
#include "h_020.h"//系统配置主界面
#include "h_021.h"//7777菜单界面
#include "h_022.h"//7777基本配置界面
#include "h_023.h"//7777功能配置界面
#include "h_024.h"//7777运行参数配置界面
#include "h_025.h"//实时监控界面
#include "h_026.h"//电表界面
#include "h_027.h"//Modbus地址配置
#include "h_028.h"//保护参数配置
#include "h_029.h"//告警信息
#include "h_030.h"//KPI设置
#include "h_031.h"//框架调用
#include "h_032.h"//分时电价
#include "h_033.h"//消峰填谷
#include "h_034.h"//交流限发
#include "h_035.h"//输出平滑
#include "h_036.h"//调频
#include "h_037.h"//高级设置界面
#include "h_038.h"//故障界面
#include "h_039.h"//告警界面
#include "h_040.h"//运行示意图
#include "h_041.h"//状态栏
#include "h_042.h"//微网曲线
#include "h_043.h"//详细信息
#include "h_044.h"//辅助设置
#include "h_045.h"//环境仪信息
#include "h_046.h"//复位记录信息
#include "h_047.h"//config.h logo区
#include "h_048.h"//保留
#include "h_049.h"//保留
#include "h_050.h"//pcs设备参数
#include "h_051.h"//pv设备参数
#include "h_052.h"//dcdc设备参数
#include "h_053.h"//bms设备参数
#include "h_054.h"//ammeter设备参数
#include "h_055.h"//pvs设备参数
#include "h_056.h"//env设备参数

#define  MAX_PAGES		56  //最大网页数

#include "j_000.h"
#include "j_001.h"
#include "j_002.h"
#include "j_003.h"//jquery.flot.min.js
#include "j_004.h"///jquery.min.js
#include "j_005.h"//jquery.flot.time.js
#include "j_006.h"//excanvas.min.js
#include "j_007.h"//j_highcharts.js
#include "j_008.h"//j_exporting.js
#include "j_009.h"//j_canvas-tools.js
#include "j_010.h"//j_export-csv.js
#include "j_011.h"//j_highcharts-export-clientside.js
#include "j_012.h"//j_zDialog.js
#include "j_013.h"//j_encoding.js
#include "j_014.h"//j_encoding-indexes.js
#include "j_015.h"//j_filesaver.js     不能出现大写字母

#include "http_image.h"
#include "http_css.h"
#include "http_interface.h"

#endif
