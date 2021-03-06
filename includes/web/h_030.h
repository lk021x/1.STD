#ifndef _HTML_H_030_H_
#define _HTML_H_030_H_
static const char h_030[] = {
"HTTP/1.1 200 OK\r\nContent-Type:text/html\r\nAccept-Ranges:bytes\r\n\r\n"
"<!DOCTYPE html PUBLIC '-//W3C//DTD XHTML 1.0 Transitional//EN' 'http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd'>\n"
"<html xmlns='http://www.w3.org/1999/xhtml'>\n"
 "<head>\n"
 	"<meta http-equiv='Content-Type' content='text/html; charset=GB2312'/>\n"
 	"<link type='text/css' href='c_000.css' rel='stylesheet'/>\n"
 	"<script type='text/javascript'>\n"
            "var $=function(o){return document.getElementById(o);};\n"
            "function lang_init_ex(st,end){for(var i=st;i<=end;i++)eval('$(\\'N'+(i<10?'0'+i:i)+'\\').innerHTML=lang.N'+(i<10?'0'+i:i)+'[language];');}\n"
            "function show_tip_ex(id, obj){eval('$(\\'tip'+id+'\\').innerHTML=obj;');}\n"
            "function show_T(tid)\n"
            "{\n"
                "var str='';\n"
                "str+=\"<div class='title' style='width:90%;'><div class='r1 color'></div><div class='r2 color'></div><div class='r3 color'></div><div class='r4 color'></div>\";\n"
                "str+=\"<div class='content color'><span id='\"+tid+\"'></span></div>\";\n"
                "str+=\"<div class='r4 color'></div><div class='r3 color'></div><div class='r2 color'></div><div class='r1 color'></div></div><div class='blank'></div>\";\n"
                "document.write(str);\n"
            "}\n"
            "function show_sT(tid)\n"
            "{\n"
                "var str='';\n"
                "str+=\"<div class='title' style='width:90%;'><div class='subtitle'><div class='r1 subcolor'></div><div class='r2 subcolor'></div><div class='r3 subcolor'></div><div class='r4 subcolor'></div>\";\n"
                "str+=\"<div class='content subcolor subcontent'><span id='\"+tid+\"'></span></div>\";\n"
                "str+=\"<div class='r4 subcolor'></div><div class='r3 subcolor'></div><div class='r2 subcolor'></div><div class='r1 subcolor'></div></div><div class='line_short'></div></div>\";\n"
                "document.write(str);\n"
            "}\n"
            "function show_sT_ex(tid, id)\n"
            "{\n"
                "var str='';\n"
                "str+=\"<div class='title' id='\"+id+\"' style='display:none;width:90%'><div class='subtitle'><div class='r1 subcolor'></div><div class='r2 subcolor'></div><div class='r3 subcolor'></div><div class='r4 subcolor'></div>\";\n"
                "str+=\"<div class='content subcolor subcontent'><span id='\"+tid+\"'></span></div>\";\n"
                "str+=\"<div class='r4 subcolor'></div><div class='r3 subcolor'></div><div class='r2 subcolor'></div><div class='r1 subcolor'></div></div><div class='line_short'></div></div>\";\n"
                "document.write(str);\n"
            "}\n"
            "function sendHttpReq(url, successFn)\n"
            "{\n"
                 //alert(url);   
                "var xmlhttp;\n"
                "if(window.XMLHttpRequest)xmlhttp=new XMLHttpRequest();\n"
                "else xmlhttp=new ActiveXObject('Microsoft.XMLHTTP');\n"
                "xmlhttp.onreadystatechange=function()\n"
                "{\n"
                    "if(xmlhttp.readyState==4 && xmlhttp.status==200)\n"
                    "{\n"
                        "if(xmlhttp.responseText=='1'){successFn();}\n"
                    "}\n"
                "};\n"
                "xmlhttp.open('GET',url,true);\n"
                "xmlhttp.send();\n"
            "}\n"
        "</script>\n"
 	"<style type='text/css'>\n"
 		"tr{height:27px;}\n"
 		".td1{padding-left:40px;text-align:left;}\n"
 		".td2{text-align:left;}\n"
 		".td3{padding-right:40px;text-align:right;}\n"
 		".txt{width:100px;text-align:right;}\n"
 	"</style>\n"
 "</head>\n"
 "<body onload='Init()'>\n"
 	"<script type='text/javascript'>\n"
 		"show_T('N01');\n"
            "show_sT('N02');\n"
 	"</script>\n"
        "<table width='90%' border='0'>\n"
            "<tr>\n"
                "<td width='30%' class='td1 odd'><span id='N03'></span></td>\n"
                "<td width='20%' class='td1 odd'><input class='txt' id='ID_0x03A0' onkeyup='Verify(this)' type='text' maxlength='5'/>&nbsp;</td>\n"
                "<td width='15%' class='td2 odd'>[0.01 ~ 60]</td>\n"
                "<td width='35%' class='td3 odd'></td>\n"
            "</tr>\n"
        "</table>\n"
        "<table width='90%' border='0'>\n"
            "<tr>\n"
                "<td width='50%' class='even tip'><span id='tip1'>&nbsp;</span></td>\n"
                "<!-- 保存 -->\n"
                "<td width='50%' align='right' class='even'><button class='btn' onclick='doSubmit(this)' id='btn1' style='display:block;'><span id='N400'>Save</span></button></td>\n"
            "</tr>\n"
        "</table>\n"
        "<table width='90%' border='0'>\n"
            "<tr>\n"
                "<td width='30%' class='td1 odd'><span id='N04'></span></td>\n"
                "<td width='20%' class='td1 odd'><input class='txt' id='ID_0x03A1' onkeyup='Verify(this)' type='text' maxlength='5'/>&nbsp;</td>\n"
                "<td width='15%' class='td2 odd'>[0.01 ~ 60]</td>\n"
                "<td width='35%' class='td3 odd'></td>\n"
            "</tr>\n"
        "</table>\n"
        "<table width='90%' border='0'>\n"
            "<tr>\n"
                "<td width='50%' class='even tip'><span id='tip2'>&nbsp;</span></td>\n"
                "<!-- 保存 -->\n"
                "<td width='50%' align='right' class='even'><button class='btn' onclick='doSubmit(this)' id='btn2' style='display:block;'><span id='N401'>Save</span></button></td>\n"
            "</tr>\n"
        "</table>\n"
        "<table width='90%' border='0'>\n"
            "<tr>\n"
                "<td width='30%' class='td1 odd'><span id='N05'></span></td>\n"
                "<td width='20%' class='td1 odd'><input class='txt' id='ID_0x03A2' onkeyup='Verify(this)' type='text' maxlength='5'/>&nbsp;</td>\n"
                "<td width='15%' class='td2 odd'>[0.01 ~ 60]</td>\n"
                "<td width='35%' class='td3 odd'></td>\n"
            "</tr>\n"
        "</table>\n"
        "<table width='90%' border='0'>\n"
            "<tr>\n"
                "<td width='50%' class='even tip'><span id='tip3'>&nbsp;</span></td>\n"
                "<!-- 保存 -->\n"
                "<td width='50%' align='right' class='even'><button class='btn' onclick='doSubmit(this)' id='btn3' style='display:block;'><span id='N402'>Save</span></button></td>\n"
            "</tr>\n"
        "</table>\n"
        "<table width='90%' border='0'>\n"
            "<tr>\n"
                "<td width='30%' class='td1 odd'><span id='N06'></span></td>\n"
                "<td width='20%' class='td1 odd'><input class='txt' id='ID_0x03A3' onkeyup='Verify(this)' type='text' maxlength='5'/>&nbsp;</td>\n"
                "<td width='15%' class='td2 odd'>[0.01 ~ 60]</td>\n"
                "<td width='35%' class='td3 odd'></td>\n"
            "</tr>\n"
        "</table>\n"
        "<table width='90%' border='0'>\n"
            "<tr>\n"
                "<td width='50%' class='even tip'><span id='tip4'>&nbsp;</span></td>\n"
                "<!-- 保存 -->\n"
                "<td width='50%' align='right' class='even'><button class='btn' onclick='doSubmit(this)' id='btn4' style='display:block;'><span id='N403'>Save</span></button></td>\n"
            "</tr>\n"
        "</table>\n"
        "<table width='90%' border='0'>\n"
            "<tr>\n"
                "<td width='30%' class='td1 odd'><span id='N07'></span></td>\n"
                "<td width='20%' class='td1 odd'><input class='txt' id='ID_0x03A4' onkeyup='Verify(this)' type='text' maxlength='5'/>&nbsp;</td>\n"
                "<td width='15%' class='td2 odd'>[0.1 ~ 100]</td>\n"
                "<td width='35%' class='td3 odd'></td>\n"
            "</tr>\n"
        "</table>\n"
        "<table width='90%' border='0'>\n"
            "<tr>\n"
                "<td width='50%' class='even tip'><span id='tip5'>&nbsp;</span></td>\n"
                "<!-- 保存 -->\n"
                "<td width='50%' align='right' class='even'><button class='btn' onclick='doSubmit(this)' id='btn5' style='display:block;'><span id='N404'>Save</span></button></td>\n"
            "</tr>\n"
        "</table>\n"
        "<table width='90%' border='0'>\n"
            "<tr>\n"
                "<td width='30%' class='td1 odd'><span id='N08'></span></td>\n"
                "<td width='20%' class='td1 odd'><input class='txt' id='ID_0x03A5' onkeyup='Verify(this)' type='text' maxlength='4'/>&nbsp;</td>\n"
                "<td width='15%' class='td2 odd'>[5.0 ~ 50.0]</td>\n"
                "<td width='35%' class='td3 odd'></td>\n"
            "</tr>\n"
        "</table>\n"
        "<table width='90%' border='0'>\n"
            "<tr>\n"
                "<td width='50%' class='even tip'><span id='tip6'>&nbsp;</span></td>\n"
                "<!-- 保存 -->\n"
                "<td width='50%' align='right' class='even'><button class='btn' onclick='doSubmit(this)' id='btn6' style='display:block;'><span id='N405'>Save</span></button></td>\n"
            "</tr>\n"
        "</table>\n"
        "<table width='90%' border='0'>\n"
            "<tr>\n"
                "<td width='30%' class='td1 odd'><span id='N09'></span></td>\n"
                "<td width='20%' class='td1 odd'><input class='txt' id='ID_0x03A6' onkeyup='Verify(this)' type='text' maxlength='6'/>&nbsp;</td>\n"
                "<td width='15%' class='td2 odd'>[1.01 ~ 100]</td>\n"
                "<td width='35%' class='td3 odd'></td>\n"
            "</tr>\n"
        "</table>\n"
        "<table width='90%' border='0'>\n"
            "<tr>\n"
                "<td width='50%' class='even tip'><span id='tip7'>&nbsp;</span></td>\n"
                "<!-- 保存 -->\n"
                "<td width='50%' align='right' class='even'><button class='btn' onclick='doSubmit(this)' id='btn7' style='display:block;'><span id='N406'>Save</span></button></td>\n"
            "</tr>\n"
        "</table>\n"
        "<table width='90%' border='0'>\n"
            "<tr>\n"
                "<td width='30%' class='td1 odd'><span id='N10'></span></td>\n"
                "<td width='20%' class='td1 odd'><input class='txt' id='ID_0x03A7' onkeyup='Verify(this)' type='text' maxlength='3'/>&nbsp;</td>\n"
                "<td width='15%' class='td2 odd'>[1 ~ 100]</td>\n"
                "<td width='35%' class='td3 odd'></td>\n"
            "</tr>\n"
        "</table>\n"
        "<table width='90%' border='0'>\n"
            "<tr>\n"
                "<td width='50%' class='even tip'><span id='tip8'>&nbsp;</span></td>\n"
                "<!-- 保存 -->\n"
                "<td width='50%' align='right' class='even'><button class='btn' onclick='doSubmit(this)' id='btn8' style='display:block;'><span id='N407'>Save</span></button></td>\n"
            "</tr>\n"
        "</table>\n"
        "<div class='line_long'></div>\n"
        "<div class='btm_blank'></div>\n"
        "<script type='text/javascript'>\n"
            "show_sT('N11');\n"
        "</script>\n"
        "<table width='90%' border='0'>\n"
            "<tr>\n"
                "<td width='30%' class='td1 odd'><input name='PV-BROADCAST' type='radio' value='85' id='PV-BROADCAST-DIS'/><span id='N50'></span></td>\n"
                "<td width='30%' class='td1 odd'><input name='PV-BROADCAST' type='radio' value='170' id='PV-BROADCAST-EN'/><span id='N51'></span></td>\n"
            "</tr>\n"
        "</table>\n"
        "<table width='90%' border='0'>\n"
            "<tr>\n"
                "<td width='50%' class='even tip'><span id='tip9'>&nbsp;</span></td>\n"
                "<!-- 保存 -->\n"
                "<td width='50%' align='right' class='even'><button class='btn' onclick='doSubmit(this)' id='btn9' style='display:block;'><span id='N408'>Save</span></button></td>\n"
            "</tr>\n"
        "</table>\n"
        "<div class='line_long'></div>\n"
        "<div class='btm_blank'></div>\n"
 	"<div id='trans'></div>\n"
 	"<!--[if IE 6]><script type='text/javascript'>document.execCommand('BackgroundImageCache',false,true);</script><![endif]-->\n"
 "</body>\n"
    "<script type='text/javascript'>\n"
        "window.history.forward(1);\n"
        "var language=0;\n"
        "var Json; \n"
        "var lang=\n"
        "{\n"
            "N01:['专家参数',' Proficient Settings'],\n"
            "N02:['KPI设置','KPI Settings'],\n"
            "N03:['电池电压保护参数KP','Battery V KP'],\n"
            "N04:['电池电压保护参数KI','Battery V KI'],\n"
            "N05:['PV限功率比例KP','PV limit P KP'], \n"
          "N06:['PV限功率比例KI','PV limit P KI'],\n"
          "N07:['均衡控制比例','SOC balancing KP'],\n"
          "N08:['零电压最大值(V)','Zero V Max(V)'],\n"
          "N09:['回调比例','Para Callback KP'],\n"
          "N10:['回调延时','Para Callback delay'],\n"
            "N11:['光伏广播使能','PV broadcast enable'],\n"
"\n"
            "N50:['禁止','Disable'],\n"
            "N51:['使能','Enable'], \n"
"\n"
            "N400:['设&nbsp;&nbsp;置','Set'], \n"
            "N401:['设&nbsp;&nbsp;置','Set'], \n"
            "N402:['设&nbsp;&nbsp;置','Set'], \n"
            "N403:['设&nbsp;&nbsp;置','Set'], \n"
            "N404:['设&nbsp;&nbsp;置','Set'], \n"
            "N405:['设&nbsp;&nbsp;置','Set'],  	          \n"
            "N406:['设&nbsp;&nbsp;置','Set'], \n"
            "N407:['设&nbsp;&nbsp;置','Set'], \n"
            "N408:['设&nbsp;&nbsp;置','Set'], \n"
"\n"
            "N499:['设置成功！','Set successfully!'],\n"
            "N500:['设置失败！','Set Failed!'],\n"
            "N501:['格式错误！','Format error!'],\n"
            "N502:['范围错误！','Range error!']      \n"
        "};\n"
        "function Init()\n"
        "{\n"
            "var URL='s_030.html?0x0000&time='+Math.random();\n"
            "var xmlhttp;\n"
            "if(window.XMLHttpRequest)xmlhttp=new XMLHttpRequest();\n"
            "else xmlhttp=new ActiveXObject('Microsoft.XMLHTTP');\n"
            "xmlhttp.onreadystatechange=function()\n"
            "{\n"
                "if(xmlhttp.readyState==4 && xmlhttp.status==200)\n"
                "{            \n"
                    "eval('Json='+xmlhttp.responseText);				\n"
"\n"
     			"if(Json.ID_0x0301==0x55)language=0;else language=1;\n"
                     //初始化语言
     			"lang_init_ex(1,11);\n"
                    "lang_init_ex(50,51);\n"
                    "lang_init_ex(400,408); \n"
"\n"
     			"doSetValue(Json);\n"
"\n"
     				//user用户不能配置，install用户可以配置
     			"if(Json.ID_0x0282==0x11)\n"
     			"{\n"
     				"$('btn1').style.display='none';\n"
     				"$('trans').style.zIndex=9999;\n"
     			"}\n"
     			"else\n"
     			"{\n"
     				"$('btn1').style.display='inline';\n"
     				"$('trans').style.zIndex=-9999;\n"
     			"}\n"
                "}\n"
            "};\n"
            "xmlhttp.open('GET',URL,true);\n"
            "xmlhttp.send();\n"
        "}\n"
        "function read_back(addr)\n"
        "{\n"
            "var url='s_030.html?';\n"
            "url=url+'0x0'+addr.toString(16).toUpperCase();\n"
            "url=url+'&time='+Math.random();\n"
            "var xmlhttp;\n"
            "if(window.XMLHttpRequest)xmlhttp=new XMLHttpRequest();\n"
            "else xmlhttp=new ActiveXObject('Microsoft.XMLHTTP');\n"
            "xmlhttp.onreadystatechange=function()\n"
            "{\n"
                "if(xmlhttp.readyState==4 && xmlhttp.status==200)\n"
                "{\n"
                    "eval('Json='+xmlhttp.responseText); \n"
                    "if(addr == 0x03A0)\n"
                    "{\n"
                        "var pr=0,fe=0;\n"
                        "pr=($('ID_0x03A0').value*100).toFixed(0);\n"
                        "fe=Json.ID_0x03A0;\n"
                        "if(pr!=fe)$('ID_0x03A0').value=(Json.ID_0x03A0/100.0).toFixed(2);\n"
                        "show_tip_ex(1,'&nbsp;');\n"
                        "if(pr!=fe){setTimeout('show_tip_ex(1,lang.N500[language])',500);}\n"
                        "else {setTimeout('show_tip_ex(1,lang.N499[language])',500);}\n"
                        "setTimeout('show_tip_ex(1,\\'&nbsp;\\')',2500);\n"
                    "}\n"
                    "else if(addr == 0x03A1)\n"
                    "{\n"
                        "var pr=0,fe=0;\n"
                        "pr=($('ID_0x03A1').value*100).toFixed(0);\n"
                        "fe=Json.ID_0x03A1;\n"
                        "if(pr!=fe)$('ID_0x03A1').value=(Json.ID_0x03A1/100.0).toFixed(2);\n"
                        "show_tip_ex(2,'&nbsp;');\n"
                        "if(pr!=fe){setTimeout('show_tip_ex(2,lang.N500[language])',500);}\n"
                        "else {setTimeout('show_tip_ex(2,lang.N499[language])',500);}\n"
                        "setTimeout('show_tip_ex(2,\\'&nbsp;\\')',2500);\n"
                    "}\n"
                    "else if(addr == 0x03A2)\n"
                    "{\n"
                        "var pr=0,fe=0;\n"
                        "pr=($('ID_0x03A2').value*100).toFixed(0);\n"
                        "fe=Json.ID_0x03A2;\n"
                        "if(pr!=fe)$('ID_0x03A2').value=(Json.ID_0x03A2/100.0).toFixed(2);\n"
                        "show_tip_ex(3,'&nbsp;');\n"
                        "if(pr!=fe){setTimeout('show_tip_ex(3,lang.N500[language])',500);}\n"
                        "else {setTimeout('show_tip_ex(3,lang.N499[language])',500);}\n"
                        "setTimeout('show_tip_ex(3,\\'&nbsp;\\')',2500);\n"
                    "}\n"
                    "else if(addr == 0x03A3)\n"
                    "{\n"
                        "var pr=0,fe=0;\n"
                        "pr=($('ID_0x03A3').value*100).toFixed(0);\n"
                        "fe=Json.ID_0x03A3;\n"
                        "if(pr!=fe)$('ID_0x03A3').value=(Json.ID_0x03A3/100.0).toFixed(2);\n"
                        "show_tip_ex(4,'&nbsp;');\n"
                        "if(pr!=fe){setTimeout('show_tip_ex(4,lang.N500[language])',500);}\n"
                        "else {setTimeout('show_tip_ex(4,lang.N499[language])',500);}\n"
                        "setTimeout('show_tip_ex(4,\\'&nbsp;\\')',2500);\n"
                    "}\n"
                    "else if(addr == 0x03A4)\n"
                    "{\n"
                        "var pr=0,fe=0;\n"
                        "pr=($('ID_0x03A4').value*10).toFixed(0);\n"
                        "fe=Json.ID_0x03A4;\n"
                        "if(pr!=fe)$('ID_0x03A4').value=(Json.ID_0x03A4/10.0).toFixed(1);\n"
                        "show_tip_ex(5,'&nbsp;');\n"
                        "if(pr!=fe){setTimeout('show_tip_ex(5,lang.N500[language])',500);}\n"
                        "else {setTimeout('show_tip_ex(5,lang.N499[language])',500);}\n"
                        "setTimeout('show_tip_ex(5,\\'&nbsp;\\')',2500);\n"
                    "}\n"
                    "else if(addr == 0x03A5)\n"
                    "{\n"
                        "var pr=0,fe=0;\n"
                        "pr=($('ID_0x03A5').value*10).toFixed(0);\n"
                        "fe=Json.ID_0x03A5;\n"
                        "if(pr!=fe)$('ID_0x03A5').value=(Json.ID_0x03A5/10.0).toFixed(1);\n"
                        "show_tip_ex(6,'&nbsp;');\n"
                        "if(pr!=fe){setTimeout('show_tip_ex(6,lang.N500[language])',500);}\n"
                        "else {setTimeout('show_tip_ex(6,lang.N499[language])',500);}\n"
                        "setTimeout('show_tip_ex(6,\\'&nbsp;\\')',2500);\n"
                    "}\n"
                    "else if(addr == 0x03A6)\n"
                    "{\n"
                        "var pr=0,fe=0;\n"
                        "pr=($('ID_0x03A6').value*100).toFixed(0);\n"
                        "fe=Json.ID_0x03A6;\n"
                        "if(pr!=fe)$('ID_0x03A6').value=(Json.ID_0x03A6/100.0).toFixed(2);\n"
                        "show_tip_ex(7,'&nbsp;');\n"
                        "if(pr!=fe){setTimeout('show_tip_ex(7,lang.N500[language])',500);}\n"
                        "else {setTimeout('show_tip_ex(7,lang.N499[language])',500);}\n"
                        "setTimeout('show_tip_ex(7,\\'&nbsp;\\')',2500);\n"
                    "}\n"
                    "else if(addr == 0x03A7)\n"
                    "{\n"
                        "var pr=0,fe=0;\n"
                        "pr=$('ID_0x03A7').value;\n"
                        "fe=Json.ID_0x03A7;\n"
                        "if(pr!=fe)$('ID_0x03A7').value=Json.ID_0x03A7;\n"
                        "show_tip_ex(8,'&nbsp;');\n"
                        "if(pr!=fe){setTimeout('show_tip_ex(8,lang.N500[language])',500);}\n"
                        "else {setTimeout('show_tip_ex(8,lang.N499[language])',500);}\n"
                        "setTimeout('show_tip_ex(8,\\'&nbsp;\\')',2500);\n"
                    "}\n"
                    "else if(addr == 0x03B0)\n"
                    "{\n"
                        "var pr=0,fe=0;\n"
                        "if($('PV-BROADCAST-DIS').checked){pr=$('PV-BROADCAST-DIS').value;}    \n"
                        "else if($('PV-BROADCAST-EN').checked){pr=$('PV-BROADCAST-EN').value;}\n"
                        "fe=Json.ID_0x03B0;\n"
                        "if(pr!=fe){\n"
                            "if(Json.ID_0x03B0==0x55){\n"
                                "$('PV-BROADCAST-DIS').checked=true;\n"
                            "}else if(Json.ID_0x03B0==0xAA){\n"
                                "$('PV-BROADCAST-EN').checked=true;\n"
                            "}\n"
                        "}\n"
                        "show_tip_ex(9,'&nbsp;');\n"
                        "if(pr!=fe){setTimeout('show_tip_ex(9,lang.N500[language])',500);}\n"
                        "else {setTimeout('show_tip_ex(9,lang.N499[language])',500);}\n"
                        "setTimeout('show_tip_ex(9,\\'&nbsp;\\')',2500);\n"
                    "}\n"
                "}\n"
            "};\n"
            "xmlhttp.open('GET',url,true);\n"
            "xmlhttp.send();  \n"
        "}\n"
        "function doSubmit(obj)\n"
        "{\n"
            "var pv_broadcast_en;\n"
"\n"
            "var URL='a_030.html?';\n"
            "switch(obj.id)\n"
            "{\n"
             "case 'btn1': \n"
                    "URL=URL+'&0x03A0='+($('ID_0x03A0').value*100).toFixed(0);\n"
                    "readBack=0x03A0;\n"
             "break;\n"
                "case 'btn2': \n"
                    "URL=URL+'&0x03A1='+($('ID_0x03A1').value*100).toFixed(0);\n"
                    "readBack=0x03A1;\n"
                "break;\n"
             "case 'btn3': \n"
                    "URL=URL+'&0x03A2='+($('ID_0x03A2').value*100).toFixed(0);\n"
                    "readBack=0x03A2;\n"
             "break;\n"
             "case 'btn4': \n"
                    "URL=URL+'&0x03A3='+($('ID_0x03A3').value*100).toFixed(0);\n"
                    "readBack=0x03A3;\n"
             "break;\n"
             "case 'btn5': \n"
                    "URL=URL+'&0x03A4='+($('ID_0x03A4').value*10).toFixed(0);\n"
                    "readBack=0x03A4;\n"
             "break;\n"
             "case 'btn6': \n"
                    "URL=URL+'&0x03A5='+($('ID_0x03A5').value*10).toFixed(0);\n"
                    "readBack=0x03A5;\n"
             "break;\n"
                "case 'btn7':\n"
                    "URL=URL+'&0x03A6='+($('ID_0x03A6').value*100).toFixed(0);\n"
                    "readBack=0x03A6;\n"
                "break;\n"
                "case 'btn8':\n"
                    "URL=URL+'&0x03A7='+$('ID_0x03A7').value;\n"
                    "readBack=0x03A7;\n"
                "break;\n"
                "case 'btn9':\n"
                    "if($('PV-BROADCAST-DIS').checked){pv_broadcast_en=$('PV-BROADCAST-DIS').value;}    \n"
                    "else if($('PV-BROADCAST-EN').checked){pv_broadcast_en=$('PV-BROADCAST-EN').value;}\n"
                    "URL=URL+'&0x03B0='+pv_broadcast_en;\n"
                    "readBack=0x03B0; \n"
                "break;\n"
             "default:break;\n"
            "}\n"
            "URL=URL+'&time='+Math.random();\n"
             //alert(URL);
"\n"
            "function success()\n"
            "{\n"
     		"setTimeout('read_back('+readBack+')', 100);         			\n"
            "}\n"
"\n"
             //测试
             //success();  
"\n"
 		"sendHttpReq(URL, success);\n"
        "}\n"
"\n"
        "function doSetValue(Json)\n"
        "{\n"
            "$('ID_0x03A0').value=(Json.ID_0x03A0/100.0).toFixed(2);\n"
            "$('ID_0x03A1').value=(Json.ID_0x03A1/100.0).toFixed(2);\n"
            "$('ID_0x03A2').value=(Json.ID_0x03A2/100.0).toFixed(2);\n"
            "$('ID_0x03A3').value=(Json.ID_0x03A3/100.0).toFixed(2);\n"
            "$('ID_0x03A4').value=(Json.ID_0x03A4/10.0).toFixed(1);\n"
            "$('ID_0x03A5').value=(Json.ID_0x03A5/10.0).toFixed(1);\n"
            "$('ID_0x03A6').value=(Json.ID_0x03A6/100.0).toFixed(2);\n"
            "$('ID_0x03A7').value= Json.ID_0x03A7;\n"
"\n"
             //电表协议
            "if(Json.ID_0x03B0 == 0x55)\n"
            "{\n"
                "$('PV-BROADCAST-DIS').checked=true;\n"
                "$('PV-BROADCAST-EN').checked=false;\n"
            "}\n"
            "else if(Json.ID_0x03B0 == 0xAA)\n"
            "{\n"
                "$('PV-BROADCAST-DIS').checked=false;\n"
                "$('PV-BROADCAST-EN').checked=true;\n"
            "}       \n"
        "}\n"
        "function Verify(obj)\n"
        "{\n"
            "var reg=/^-?[0-9][0-9]*$/;\n"
             //正整数、带1-2位小数的正浮点数
            "var reg2=/^((0(\\.\\d{1,2})?)|([1-9]\\d*(\\.\\d{1,2})?))$/;\n"
             //正整数、带一位小数的正浮点数
            "var reg3=/^((0(\\.\\d{1})?)|([1-9]\\d*(\\.\\d{1})?))$/;\n"
            "var range=parseInt(obj.value);\n"
            "var range2=parseFloat(obj.value);\n"
"\n"
            "switch(obj.id)\n"
            "{\n"
                "case 'ID_0x03A0':\n"
                    "if(!obj.value.match(reg2)){$('btn1').disabled=true;obj.style.color='#F00';show_tip_ex(1,lang.N501[language]);}\n"
                    "else if(range2<0.01||range2>60){$('btn1').disabled=true;obj.style.color='#F00';show_tip_ex(1,lang.N502[language]);}\n"
                    "else{\n"
                        "obj.style.color='#000';\n"
                        "$('btn1').disabled=false;\n"
                        "show_tip_ex(1,'&nbsp;');\n"
                    "}\n"
                "break;\n"
                "case 'ID_0x03A1':\n"
                    "if(!obj.value.match(reg2)){$('btn2').disabled=true;obj.style.color='#F00';show_tip_ex(2,lang.N501[language]);}\n"
                    "else if(range2<0.01||range2>60){$('btn2').disabled=true;obj.style.color='#F00';show_tip_ex(2,lang.N502[language]);}\n"
                    "else{\n"
                        "obj.style.color='#000';\n"
                        "$('btn2').disabled=false;\n"
                        "show_tip_ex(2,'&nbsp;');\n"
                    "}\n"
                "break;          \n"
                "case 'ID_0x03A2':\n"
                    "if(!obj.value.match(reg2)){$('btn3').disabled=true;obj.style.color='#F00';show_tip_ex(3,lang.N501[language]);}\n"
                    "else if(range2<0.01||range2>60){$('btn3').disabled=true;obj.style.color='#F00';show_tip_ex(3,lang.N502[language]);}\n"
                    "else{\n"
                        "obj.style.color='#000';\n"
                        "$('btn3').disabled=false;\n"
                        "show_tip_ex(3,'&nbsp;');\n"
                    "}\n"
                "break;\n"
                "case 'ID_0x03A3':\n"
                    "if(!obj.value.match(reg2)){$('btn4').disabled=true;obj.style.color='#F00';show_tip_ex(4,lang.N501[language]);}\n"
                    "else if(range2<0.01||range2>60){$('btn4').disabled=true;obj.style.color='#F00';show_tip_ex(4,lang.N502[language]);}\n"
                    "else{\n"
                        "obj.style.color='#000';\n"
                        "$('btn4').disabled=false;\n"
                        "show_tip_ex(4,'&nbsp;');\n"
                    "}\n"
                "break;    \n"
                "case 'ID_0x03A4':\n"
                    "if(!obj.value.match(reg3)){$('btn5').disabled=true;obj.style.color='#F00';show_tip_ex(5,lang.N501[language]);}\n"
                    "else if(range2<0.1||range2>100){$('btn5').disabled=true;obj.style.color='#F00';show_tip_ex(5,lang.N502[language]);}\n"
                    "else{\n"
                        "obj.style.color='#000';\n"
                        "$('btn5').disabled=false;\n"
                        "show_tip_ex(5,'&nbsp;');\n"
                    "}\n"
                "break;  \n"
                "case 'ID_0x03A5':\n"
                    "if(!obj.value.match(reg3)){$('btn6').disabled=true;obj.style.color='#F00';show_tip_ex(6,lang.N501[language]);}\n"
                    "else if(range2<5.0||range2>50){$('btn6').disabled=true;obj.style.color='#F00';show_tip_ex(6,lang.N502[language]);}\n"
                    "else{\n"
                        "obj.style.color='#000';\n"
                        "$('btn6').disabled=false;\n"
                        "show_tip_ex(6,'&nbsp;');\n"
                    "}\n"
                "break; \n"
                "case 'ID_0x03A6':\n"
                    "if(!obj.value.match(reg2)){$('btn7').disabled=true;obj.style.color='#F00';show_tip_ex(7,lang.N501[language]);}\n"
                    "else if(range2<1.01||range2>100){$('btn7').disabled=true;obj.style.color='#F00';show_tip_ex(7,lang.N502[language]);}\n"
                    "else{\n"
                        "obj.style.color='#000';\n"
                        "$('btn7').disabled=false;\n"
                        "show_tip_ex(7,'&nbsp;');\n"
                    "}\n"
                "break;   \n"
                "case 'ID_0x03A7':\n"
                    "if(!obj.value.match(reg)){$('btn8').disabled=true;obj.style.color='#F00';show_tip_ex(8,lang.N501[language]);}\n"
                    "else if(range<1||range>100){$('btn8').disabled=true;obj.style.color='#F00';show_tip_ex(8,lang.N502[language]);}\n"
                    "else{\n"
                        "obj.style.color='#000';\n"
                        "$('btn8').disabled=false;\n"
                        "show_tip_ex(8,'&nbsp;');\n"
                    "}\n"
                "break; \n"
            "}\n"
        "}\n"
    "</script>\n"
"</html>\n"
};
#endif
