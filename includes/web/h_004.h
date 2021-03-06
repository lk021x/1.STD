#ifndef _HTML_H_004_H_
#define _HTML_H_004_H_
static const char h_004[] = {
"HTTP/1.1 200 OK\r\nContent-Type:text/html\r\nAccept-Ranges:bytes\r\n\r\n"
"<!DOCTYPE html PUBLIC '-//W3C//DTD XHTML 1.0 Transitional//EN' 'http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd'>\n"
"<html xmlns='http://www.w3.org/1999/xhtml'>\n"
    "<head>\n"
        "<meta http-equiv='Content-Type' content='text/html; charset=GB2312'/>\n"
        "<link type='text/css' href='c_000.css' rel='stylesheet'/>\n"
        "<script src='j_001.js' type='text/javascript'></script>\n"
        "<script type='text/javascript'>\n"
            "function SelectYear()\n"
            "{\n"
                "for(i=2000;i<=2099;i++)\n"
                "{\n"
                     //需要将 '\\'转换为'\\\\'
                    "document.write('<option value=\\''+i+'\\'>'+i+'</option>');\n"
                "}\n"
            "}\n"
            "function SelectMonth()\n"
            "{\n"
                "for(i=1;i<=12;i++)\n"
                "{\n"
                     //需要将 '\\'转换为'\\\\'
                    "if(i<10)document.write('<option value=\\''+i+'\\'>0'+i+'</option>');\n"
                    "else document.write('<option value=\\''+i+'\\'>'+i+'</option>');\n"
                "}\n"
            "}\n"
            "function SelectHour()\n"
            "{\n"
                "for(i=0;i<=23;i++)\n"
                "{\n"
                     //需要将 '\\'转换为'\\\\'
                    "if(i<10)document.write('<option value=\\''+i+'\\'>0'+i+'</option>');\n"
                    "else document.write('<option value=\\''+i+'\\'>'+i+'</option>');\n"
                "}\n"
            "}\n"
            "function SelectMinute()\n"
            "{\n"
                "for(i=0;i<60;i++)\n"
                "{\n"
                     //需要将 '\\'转换为'\\\\'
                    "if(i<10)document.write('<option value=\\''+i+'\\'>0'+i+'</option>');\n"
                    "else document.write('<option value=\\''+i+'\\'>'+i+'</option>')\n"
                "}\n"
            "}\n"
            "function CheckDay(Y,M,D)\n"
            "{\n"
                "var YearNo,MonthNo,DayNo;\n"
                "try\n"
                "{\n"
                    "YearNo=$(Y).value;\n"
                    "MonthNo=$(M).value;\n"
                "}\n"
                "catch(e)\n"
                "{\n"
                    "var datenow=new Date();\n"
                    "YearNo=datenow.getFullYear();\n"
                    "MonthNo=datenow.getMonth()+1;\n"
                "}\n"
                "var i,DayCount;\n"
                "if(MonthNo==1||MonthNo==3||MonthNo==5||MonthNo==7||MonthNo==8||MonthNo==10||MonthNo==12)\n"
                "{\n"
                    "DayCount=31;\n"
                "}\n"
                "if(MonthNo==4||MonthNo==6||MonthNo==9||MonthNo==11 )\n"
                "{\n"
                    "DayCount=30;\n"
                "}\n"
                "if(MonthNo==2)\n"
                "{\n"
                    "if(IsRunY(YearNo))\n"
                    "{\n"
                        "DayCount=29;\n"
                    "}\n"
                    "else\n"
                    "{\n"
                        "DayCount=28;\n"
                    "}\n"
                "}\n"
                "$(D).length=DayCount;\n"
                "var currentDay=new Date().getDate();\n"
                "for(i=0;i<DayCount;i++)\n"
                "{\n"
                    "$(D)[i]=new Option(i+1);\n"
                    "$(D)[i].value=i+1;\n"
                    "$(D)[i].innerHTML=i<9?'0'+(i+1):(i+1);\n"
                    "if(currentDay==(i+1))\n"
                    "$(D)[i].selected=true;\n"
                "}\n"
            "}\n"
            "function IsRunY(YearNo)\n"
            "{\n"
                "if((YearNo%4==0&&YearNo%100!=0)||YearNo%400==0)return true;\n"
                "else return false;\n"
            "}\n"
        "</script>\n"
        "<style type='text/css'>\n"
            "tr{height:27px;}\n"
            ".select{width:180px;}\n"
            ".time{width:80px;text-align: center;}\n"
            ".td_value{text-align:right;padding-right:20px;}\n"
            ".td1{padding-left:40px;text-align:left;}\n"
            ".td2{padding-left:20px;text-align:left;}\n"
            ".td3{padding-right:20px;text-align:right;}\n"
            ".txt{font-size:14px;text-align:center;padding-right:2px;}\n"
        "</style>\n"
    "</head>\n"
    "<body onload='Init()'>\n"
        "<script type='text/javascript'>\n"
            "show_T('N01');\n"
            "show_sT('N02');\n"
        "</script>\n"
        "<table width='90%' border='0'>\n"
            "<tr>\n"
                "<td width='21%' class='td1 odd'><span id='N03'></span></td>\n"
                "<td width='79%' class='td2 odd'>\n"
                    "<select class='time' id='ID_0x0170' onchange='CheckDay(\"ID_0x0170\",\"ID_0x0171\",\"ID_0x0172\")'>\n"
                        "<script type='text/javascript'>SelectYear();</script>\n"
                    "</select>&nbsp;<b>/</b> \n"
                    "<select class='time' id='ID_0x0171' onchange='CheckDay(\"ID_0x0170\",\"ID_0x0171\",\"ID_0x0172\");'>\n"
                        "<script type='text/javascript'>SelectMonth();</script>\n"
                    "</select>&nbsp;<b>/</b> \n"
                    "<select class='time' id='ID_0x0172' value='1'></select>&nbsp;&nbsp;&nbsp;\n"
                    "<select class='time' id='ID_0x0173'>\n"
                        "<script type='text/javascript'>SelectHour();</script>\n"
                    "</select>&nbsp;<b>:</b> \n"
                    "<select class='time' id='ID_0x0174'>\n"
                        "<script type='text/javascript'>SelectMinute();</script>\n"
                    "</select><span>&nbsp;(YYYY/MM/DD hh:mm)</span>\n"
                "</td>\n"
            "</tr>\n"
        "</table>\n"
        "<table width='90%' border='0'>\n"
            "<tr>\n"
                "<td width='50%' class='even' align='left'><span id='tip1' class='tip'>&nbsp;</span></td>\n"
                "<td width='50%' class='even' align='right'>\n"
                    "<button class='btn' onclick='doSubmit(this)' id='btn1' style='display:none;'><span id='N04'></span></button>\n"
                "</td>\n"
            "</tr>\n"
        "</table>\n"
        "<div class='line_long'></div>\n"
        "<div class='blank'></div>\n"
        "<script type='text/javascript'>\n"
            "show_sT('N05');\n"
        "</script>\n"
        "<table width='90%' border='0'>\n"
            "<tr>\n"
                "<td width='20%' class='td1 odd'><span id='N31'></span></td>\n"
                "<td width='10%' class='td1 odd'><span id='ID_0x0341'>0kWh</span></td>\n"
                "<td width='20%' class='td1 odd'><span id='N06'></span></td>\n"
                "<td width='50%' class='td2 odd'>\n"
                    "<input id='ID_0x017C' value='0' type='text' class='txt' maxlength='5' onkeyup='Verify(this)'/>kWh&nbsp;&nbsp;&nbsp;&nbsp;(-9999~9999)\n"
                "</td>\n"
            "</tr>\n"
        "</table>\n"
        "<table width='90%' border='0'>\n"
            "<tr>\n"
                "<td width='50%' class='even' align='left'><span id='tip2' class='tip'>&nbsp;</span></td>\n"
                "<td width='50%' class='even' align='right'>\n"
                    "<button class='btn' onclick='doSubmit(this)' id='btn2' style='display:none;'><span id='N07'></span></button>\n"
                "</td>\n"
            "</tr>\n"
        "</table>\n"
        "<div class='line_long'></div>\n"
        "<div class='blank'></div>\n"
        "<script type='text/javascript'>\n"
            "show_sT('N08');\n"
        "</script>\n"
        "<table width='90%' border='0'>\n"
            "<tr>\n"
                "<td width='21%' class='td1 odd'><span id='N09'></span></td>\n"
                "<td width='79%' class='td2 odd'>\n"
                    "<input id='ID_0x0208' value='9999' type='password' class='txt' maxlength='5' onkeyup='Verify(this)'/>&nbsp;&nbsp;&nbsp;&nbsp;(0~9999)\n"
                "</td>\n"
            "</tr>\n"
        "</table>\n"
        "<table width='90%' border='0'>\n"
            "<tr>\n"
                "<td width='50%' class='even' align='left'><span id='tip3' class='tip'>&nbsp;</span></td>\n"
                "<td width='50%' class='even' align='right'>\n"
                    "<button class='btn' onclick='doSubmit(this)' id='btn3' style='display:none;'><span id='N10'></span></button>\n"
                "</td>\n"
            "</tr>\n"
        "</table>\n"
        "<div class='line_long'></div>\n"
        "<div class='blank'></div>\n"
        "<script type='text/javascript'>\n"
            "show_sT('N11');\n"
        "</script>\n"
        "<table width='90%' border='0'>\n"
            "<tr>\n"
                "<td width='21%' class='td1 odd'><span id='N12'></span></td>\n"
                "<td width='79%' class='td2 odd'>\n"
                    "<input id='ID_0x0209' value='9999' type='password' class='txt' maxlength='5' onkeyup='Verify(this)'/>&nbsp;&nbsp;&nbsp;&nbsp;(0~9999)\n"
              "</td>\n"
            "</tr>\n"
        "</table>\n"
        "<table width='90%' border='0'>\n"
            "<tr>\n"
                "<td width='50%' class='even' align='left'><span id='tip4' class='tip'>&nbsp;</span></td>\n"
                "<td width='50%' class='even' align='right'>\n"
                    "<button class='btn' onclick='doSubmit(this)' id='btn4' style='display:none;'><span id='N13'></span></button>\n"
                "</td>\n"
            "</tr>\n"
        "</table>\n"
        "<div class='line_long'></div>\n"
        "<div class='blank'></div>\n"
        "<script type='text/javascript'>\n"
            "show_sT('N14');\n"
        "</script>\n"
        "<table width='90%' border='0'>\n"
            "<tr>\n"
                "<td width='21%' class='td1 odd'><span id='N15'></span></td>\n"
                "<td width='79%' class='td2 odd'>\n"
                    "<input id='ID_0x020A' value='9999' type='password' class='txt' maxlength='5' onkeyup='Verify(this)'/>&nbsp;&nbsp;&nbsp;&nbsp;(0~9999)\n"
                "</td>\n"
            "</tr>\n"
        "</table>\n"
        "<table width='90%' border='0'>\n"
            "<tr>\n"
                "<td width='50%' class='even' align='left'><span id='tip5' class='tip'>&nbsp;</span></td>\n"
                "<td width='50%' class='even' align='right'>\n"
                    "<button class='btn' onclick='doSubmit(this)' id='btn5' style='display:none;'><span id='N16'></span></button>\n"
                "</td>\n"
            "</tr>\n"
        "</table>\n"
        "<div class='line_long'></div>\n"
        "<div class='blank'></div>\n"
        "<script type='text/javascript'>\n"
            "show_sT('N28');\n"
        "</script>\n"
        "<table width='90%' border='0'>\n"
            "<tr>\n"
                "<td width='21%' class='td1 odd'><span id='N29'></span></td>\n"
                "<td width='79%' class='td2 odd'>\n"
                    "<input id='ID_0x020C' value='9999' type='password' class='txt' maxlength='5' onkeyup='Verify(this)'/>&nbsp;&nbsp;&nbsp;&nbsp;(0~9999)\n"
                "</td>\n"
            "</tr>\n"
        "</table>\n"
        "<table width='90%' border='0'>\n"
            "<tr>\n"
                "<td width='50%' class='even' align='left'><span id='N58' class='tip'>&nbsp;</span><span id='tip7' class='td1 tip'>&nbsp;</span></td>\n"
                "<td width='50%' class='even' align='right'>\n"
                    "<button class='btn' onclick='doSubmit(this)' id='btn7' style='display:none;'><span id='N30'></span></button>\n"
                "</td>\n"
            "</tr>\n"
        "</table>\n"
        "<div class='line_long'></div>\n"
        "<div class='blank'></div>\n"
        "<script type='text/javascript'>\n"
            "show_sT('N17');\n"
        "</script>\n"
        "<table width='90%' border='0'>\n"
            "<tr>\n"
              "<td width='21%' class='td1 even'><input type='checkbox' id='NTP-SEL' onclick='ntp_change(this)'/><span id='N20'></span></td>\n"
              "<td width='13%' class='td2 even'></td>\n"
              "<td width='10%' class='td1 even'></td>\n"
              "<td width='15%' class='td2 even'></td>\n"
              "<td width='41%' class='td2 even'></td>\n"
            "</tr>\n"
        "</table>\n"
        "<table width='90%' border='0'>\n"
            "<tr>\n"
                "<td width='21%' class='td1 odd'><span id='N18'></span></td>\n"
              "<td width='13%' class='td2 odd'><input class='txt' style='text-align:center;' id='IP' onkeyup='IPverify(this)' type='text'/></td>\n"
                "<td width='10%' class='td1 odd'><span id='N19'></span></td>\n"
              "<td width='15%' class='td2 odd'><input class='txt' id='ID_0x0224' maxlength='5' onkeyup='Verify(this)' type='text'/></td>\n"
                "<td width='41%' class='td2 odd'>(1~65535)</td>\n"
            "</tr>\n"
        "</table>\n"
        "<table width='90%' border='0'>\n"
            "<tr>\n"
                "<td width='50%' class='even' align='left'><span id='tip6' class='tip'>&nbsp;</span></td>\n"
                "<td width='50%' class='even' align='right'>\n"
                    "<button class='btn' onclick='doSubmit(this)' id='btn6' style='display:none;'><span id='N21'></span></button>\n"
                "</td>\n"
            "</tr>\n"
        "</table>\n"
        "<div class='line_long'></div>\n"
        "<div class='blank'></div>\n"
        "<div id='trans'></div>\n"
        "<!--[if IE 6]><script type='text/javascript'>document.execCommand('BackgroundImageCache',false,true);</script><![endif]-->\n"
    "</body>\n"
    "<script type='text/javascript'>\n"
        "window.history.forward(1);\n"
        "var language=0;\n"
        "var Json;\n"
        "var lang=\n"
        "{\n"
            "N01:['系统参数','System Parameters'],\n"
            "N02:['时间设置','Time Setting'],\n"
            "N03:['时间','Time'],\n"
            "N04:['设&nbsp;&nbsp;置','Set'],\n"
            "N05:['电量补偿','Energy Adjust'],\n"
            "N06:['负载总电量校正值','Total E-Load Adjust Value'],\n"
            "N07:['设&nbsp;&nbsp;置','Set'],\n"
            "N08:['恢复出厂参数','Load Default Param'],\n"
            "N09:['请输入密码: ','Please input password: '],\n"
            "N10:['设&nbsp;&nbsp;置','Set'],\n"
            "N11:['用户参数备份','User Param Bak'],     \n"
            "N12:['请输入密码: ','Please input password: '],\n"
            "N13:['设&nbsp;&nbsp;置','Set'],    \n"
            "N14:['用户参数恢复','User Param Recovery'], \n"
            "N15:['请输入密码: ','Please input password: '],\n"
            "N16:['设&nbsp;&nbsp;置','Set'],\n"
            "N17:['NTP参数','NTP Parameters'],\n"
            "N18:['服务器地址','Server IP'],\n"
            "N19:['端口号','Port'],\n"
            "N20:['启动NTP对时','Start NTP Timing'],\n"
            "N21:['设&nbsp;&nbsp;置','Set'],\n"
            "N22:['转发云服务器设置','Forwarding Cloud Server Settings'],\n"
            "N23:['启动云服务','Start Cloud Server'],\n"
            "N24:['域名(IP)','DomainName(IP)'],\n"
            "N25:['端口号','Port'],\n"
            "N26:['传输周期', 'Transmission Cycle'],\n"
            "N27:['设&nbsp;&nbsp;置','Set'],\n"
            "N28:['重启系统','System Reboot'],\n"
            "N29:['请输入密码: ','Please input password: '],\n"
            "N30:['重启','Reboot'],\n"
            "N31:['负载总电量','Total E-Load'],\n"
"\n"
            "N49:['设置成功!','Set successfully!'],\n"
            "N50:['设置失败!','Set failed!'],\n"
            "N51:['格式错误!','Format error!'],\n"
            "N52:['范围错误!','Range error!'],\n"
            "N53:['请稍等，正在恢复中...','Please wait, now is recoverying...'],\n"
            "N54:['恢复成功!','Recovery Successfully!'],\n"
            "N55:['密码错误!','Password error!'],\n"
            "N56:['请稍等，正在备份中...','Please wait, now is backuping...'],\n"
            "N57:['备份成功!','Backup Successfully!'],\n"
            "N58:['*运行状态请不要重启系统!','*Please do not reboot the system in running!'],\n"
            "N59:['重启中 ...','Rebooting ...'],\n"
            "N60:['请先停机!','Please Stop First!']\n"
        "};\n"
"\n"
        "function Init()\n"
        "{\n"
            "var URL='s_004.html?0x0000&time='+Math.random();\n"
            "var xmlhttp;\n"
            "if(window.XMLHttpRequest)xmlhttp=new XMLHttpRequest();\n"
            "else xmlhttp=new ActiveXObject('Microsoft.XMLHTTP');\n"
            "xmlhttp.onreadystatechange=function()\n"
            "{\n"
                "if(xmlhttp.readyState==4 && xmlhttp.status==200)\n"
                "{\n"
                    "eval('Json='+xmlhttp.responseText);\n"
         		"if(Json.ID_0x0301==0x55)language=0;else language=1;\n"
         		"lang_init_ex(1,21);\n"
                    "lang_init_ex(28,31);\n"
                    "lang_init_ex(58,58);\n"
"\n"
         		"$('ID_0x0170').value=Json.ID_0x0170;\n"
         		"$('ID_0x0171').value=Json.ID_0x0171;\n"
         		"CheckDay('ID_0x0170','ID_0x0171','ID_0x0172');\n"
         		"$('ID_0x0172').value=Json.ID_0x0172;\n"
         		"$('ID_0x0173').value=Json.ID_0x0173;\n"
         		"$('ID_0x0174').value=Json.ID_0x0174;\n"
                    "$('ID_0x017C').value=Json.ID_0x017C;\n"
                    "$('ID_0x0341').innerHTML=Json.ID_0x0341+'kWh';\n"
                    "$('IP').value=String(Json.ID_0x0220)+'.'+String(Json.ID_0x0221)+'.'+String(Json.ID_0x0222)+'.'+String(Json.ID_0x0223);\n"
                    "$('ID_0x0224').value=Json.ID_0x0224;\n"
"\n"
                    "if(Json.ID_0x0225 == 0x55){\n"
                        "$('NTP-SEL').checked=false;\n"
                        "$('IP').disabled = true;\n"
                        "$('ID_0x0224').disabled = true;\n"
                        "$('btn6').disabled = true;\n"
                    "}else if(Json.ID_0x0225 == 0xaa){\n"
                        "$('NTP-SEL').checked=true;\n"
                        "$('IP').disabled = false;\n"
                        "$('ID_0x0224').disabled = false;\n"
                        "$('btn6').disabled = false;\n"
                    "}\n"
"\n"
         		"if(Json.ID_0x0282==0x11)\n"
         		"{\n"
         			"$('btn1').style.display='none';\n"
                        "$('btn2').style.display='none';\n"
                        "$('btn3').style.display='none';\n"
                        "$('btn4').style.display='none';\n"
                        "$('btn5').style.display='none';\n"
                        "$('btn6').style.display='none';\n"
                        "$('btn7').style.display='none';\n"
         			"$('trans').style.zIndex=9999;	\n"
         		"}\n"
         		"else\n"
         		"{\n"
         			"$('btn1').style.display='inline';\n"
                        "$('btn2').style.display='inline';\n"
                        "$('btn3').style.display='inline';\n"
                        "$('btn4').style.display='inline';\n"
                        "$('btn5').style.display='inline';\n"
                        "$('btn6').style.display='inline';\n"
                        "$('btn7').style.display='inline';\n"
         			"$('trans').style.zIndex=-9999;\n"
         		"}\n"
                "}\n"
            "};\n"
            "xmlhttp.open('GET',URL,true);\n"
            "xmlhttp.send();\n"
        "}\n"
        "function show_tip_ex(id, obj){eval('$(\\'tip'+id+'\\').innerHTML=obj;');}\n"
        "function read_back(addr)\n"
        "{\n"
            "var url='s_004.html?';\n"
            "if(addr == 0x0170){url=url+'0x0170'+'&0x0171'+'&0x0172'+'&0x0173'+'&0x0174';}\n"
            "else if(addr==0x0220){url=url+'0x0220'+'&0x0221'+'&0x0222'+'&0x0223'+'&0x0224';}\n"
            "url=url+'&time='+Math.random();\n"
             //alert(url);
"\n"
            "var xmlhttp;\n"
            "if(window.XMLHttpRequest)xmlhttp=new XMLHttpRequest();\n"
            "else xmlhttp=new ActiveXObject('Microsoft.XMLHTTP');\n"
            "xmlhttp.onreadystatechange=function()\n"
            "{\n"
                "if(xmlhttp.readyState==4 && xmlhttp.status==200)\n"
                "{\n"
                    "eval('Json='+xmlhttp.responseText); \n"
                    "if(addr == 0x170)\n"
                    "{\n"
                        "var pr_year, fe_year, pr_mon, fe_mon, pr_day, fe_day, pr_hour, fe_min, pr_min, fe_min;\n"
                        "pr_year=parseInt($('ID_0x0170').value);\n"
                        "pr_mon=$('ID_0x0171').value;\n"
                        "pr_day=$('ID_0x0172').value;\n"
                        "pr_hour=$('ID_0x0173').value;\n"
                        "pr_min=$('ID_0x0174').value;\n"
                        "fe_year=Json.ID_0x0170;\n"
                        "fe_mon=Json.ID_0x0171;\n"
                        "fe_day=Json.ID_0x0172;\n"
                        "fe_hour=Json.ID_0x0173;\n"
                        "fe_min=Json.ID_0x0174;\n"
                        "if(pr_year!=fe_year || pr_mon!=fe_mon || pr_day!=fe_day || pr_hour!=fe_hour || pr_min!=fe_min)\n"
                        "{\n"
                            "$('ID_0x0170').value=Json.ID_0x0170;\n"
                            "$('ID_0x0171').value=Json.ID_0x0171;\n"
                            "$('ID_0x0172').value=Json.ID_0x0172;\n"
                            "$('ID_0x0173').value=Json.ID_0x0173;\n"
                            "$('ID_0x0174').value=Json.ID_0x0174;\n"
                        "}\n"
                        "show_tip_ex(1,'&nbsp;');\n"
                        "if(pr_year!=fe_year || pr_mon!=fe_mon || pr_day!=fe_day || \n"
                            "pr_hour!=fe_hour || pr_min!=fe_min){setTimeout('show_tip_ex(1,lang.N50[language])',500);}\n"
                        "else {setTimeout('show_tip_ex(1,lang.N49[language])',500);}\n"
                        "setTimeout('show_tip_ex(1,\\'&nbsp;\\')',2500);\n"
                    "}\n"
                    "else if(addr == 0x17C)\n"
                    "{\n"
                        "var pr=0,fe=0;\n"
                        "pr=$('ID_0x017C').value;\n"
                        "fe=Json.ID_0x017C;\n"
                        "if(pr!=fe){$('ID_0x017C').value=0}\n"
                        "$('ID_0x0341').innerHTML=Json.ID_0x0341+'kWh';\n"
                        "show_tip_ex(2,'&nbsp;');\n"
                        "if(pr!=fe){setTimeout('show_tip_ex(2,lang.N50[language])',500);}\n"
                        "else {setTimeout('show_tip_ex(2,lang.N49[language])',500);}\n"
                        "setTimeout('show_tip_ex(2,\\'&nbsp;\\')',2500);\n"
                    "}\n"
                    "else if(addr == 0x0220)\n"
                    "{\n"
                        "var ip=$('IP').value;\n"
                        "var ip_get=String(Json.ID_0x0220)+'.'+String(Json.ID_0x0221)+'.'+String(Json.ID_0x0222)+'.'+String(Json.ID_0x0223);\n"
                        "var port=$('ID_0x0224').value;\n"
                        "var port_get=Json.ID_0x0224;\n"
                        "show_tip_ex(6,'&nbsp;');\n"
                        "if(ip!=ip_get || port!=port_get){\n"
                            "$('IP').value=ip_get;\n"
                            "$('ID_0x0224').value=port_get;\n"
                            "setTimeout('show_tip_ex(6,lang.N50[language])',500);        \n"
                        "}else{\n"
                            "setTimeout('show_tip_ex(6,lang.N49[language])',500);\n"
                        "}\n"
                        "setTimeout('show_tip_ex(6,\\'&nbsp;\\')',2500);                        \n"
                    "}\n"
                    "else if(addr == 0x0225)\n"
                    "{\n"
                        "if(Json.ID_0x0225 == 0x55){\n"
                            "$('IP').disabled = true;\n"
                            "$('ID_0x0224').disabled = true;\n"
                            "$('btn6').disabled = true;\n"
                        "}else if(Json.ID_0x0225 == 0xaa){\n"
                            "$('IP').disabled = false;\n"
                            "$('ID_0x0224').disabled = false;\n"
                            "$('btn6').disabled = false;\n"
                        "}\n"
                    "}\n"
                    "else if(addr == 0x0208 || addr == 0x020A || addr == 0x020C)\n"
                    "{\n"
                        "if(Json.ID_0x020C == 0){\n"
                            "var URL='a_004.html?';\n"
                            "URL=URL+'&0x020C='+'170';\n"
                            "URL=URL+'&time='+Math.random();\n"
                            "sendHttpReq(URL,null);\n"
                            "setTimeout('system_logout()',500); \n"
                        "}else{\n"
                            "show_tip_ex(7,'&nbsp;');\n"
                            "show_tip_ex(7,lang.N60[language]);\n"
                            "setTimeout('show_tip_ex(7,\"&nbsp;\")',2000);\n"
                            "setTimeout('$(\"ID_0x020C\").value=9999',2000);\n"
                        "}\n"
                    "}\n"
                "}\n"
            "};\n"
            "xmlhttp.open('GET',url,true);\n"
            "xmlhttp.send();  \n"
        "}\n"
        "function system_logout()\n"
        "{\n"
            "parent.document.location.href='Login.html';\n"
        "}\n"
        "function doSubmit(obj)\n"
        "{\n"
            "var URL='a_004.html?';\n"
            "var readBack=0;\n"
"\n"
            "switch(obj.id)\n"
            "{\n"
                "case 'btn1':\n"
                    "URL=URL+'&0x0170='+(parseInt($('ID_0x0170').value))+'&0x0171='+$('ID_0x0171').value+'&0x0172='+$('ID_0x0172').value+'&0x0173='+$('ID_0x0173').value+'&0x0174='+$('ID_0x0174').value;\n"
                    "readBack=0x0170;\n"
                "break;\n"
                "case 'btn2':\n"
                    "URL=URL+'&0x017C='+parseInt($('ID_0x017C').value);\n"
                    "readBack=0x017C;\n"
                "break;\n"
                "case 'btn3':\n"
                    "if(parseInt($('ID_0x0208').value)==1111)\n"
                    "{\n"
                        "setTimeout('show_tip_ex(3,lang.N53[language])',10);\n"
                        "URL=URL+'&0x0208='+'170';\n"
                        //"show_tip_ex(3,'&nbsp;');                    \n"
                        //"setTimeout('show_tip_ex(3,lang.N54[language])',5000);\n"
                        //"setTimeout('show_tip_ex(3,\"&nbsp;\")',7000);\n"
                        //"setTimeout('$(\"ID_0x0208\").value=9999',7000);      \n"
                        "readBack=0x0208;\n"
                    "}else{\n"
                        "show_tip_ex(3,lang.N55[language]);\n"
                        "setTimeout('show_tip_ex(3,\"&nbsp;\")',2000);\n"
                        "setTimeout('$(\"ID_0x0208\").value=9999',2000);\n"
                        "return;\n"
                    "}\n"
                "break;\n"
                "case 'btn4':\n"
                    "if(parseInt($('ID_0x0209').value)==1111)\n"
                    "{\n"
                        "setTimeout('show_tip_ex(4,lang.N56[language])',10);\n"
                        "URL=URL+'&0x0209='+'170';\n"
                        "show_tip_ex(4,'&nbsp;');                    \n"
                        "setTimeout('show_tip_ex(4,lang.N57[language])',5000);\n"
                        "setTimeout('show_tip_ex(4,\"&nbsp;\")',7000);\n"
                        "setTimeout('$(\"ID_0x0209\").value=9999',7000);          \n"
                    "}else{\n"
                        "show_tip_ex(4,lang.N55[language]);\n"
                        "setTimeout('show_tip_ex(4,\"&nbsp;\")',2000);\n"
                        "setTimeout('$(\"ID_0x0209\").value=9999',2000);\n"
                        "return;\n"
                    "}\n"
                "break;\n"
                "case 'btn5':\n"
                    "if(parseInt($('ID_0x020A').value)==1111)\n"
                    "{\n"
                        "setTimeout('show_tip_ex(5,lang.N53[language])',10);\n"
                        "URL=URL+'&0x020A='+'170';\n"
                        //"show_tip_ex(5,'&nbsp;');                    \n"
                        //"setTimeout('show_tip_ex(5,lang.N54[language])',5000);\n"
                        //"setTimeout('show_tip_ex(5,\"&nbsp;\")',7000);\n"
                        //"setTimeout('$(\"ID_0x020A\").value=9999',7000);    \n"
                        "readBack=0x020A;      \n"
                    "}else{\n"
                        "show_tip_ex(5,lang.N55[language]);\n"
                        "setTimeout('show_tip_ex(5,\"&nbsp;\")',2000);\n"
                        "setTimeout('$(\"ID_0x020A\").value=9999',2000);\n"
                        "return;\n"
                    "}\n"
                "break;\n"
                "case 'btn6':\n"
                    "var ip=$('IP').value.split('.');\n"
                    "URL=URL+'&0x0220='+ip[0]+'&0x0221='+ip[1]+'&0x0222='+ip[2]+'&0x0223='+ip[3]+'&0x0224='+$('ID_0x0224').value;\n"
                    "readBack=0x0220;\n"
                    "break;\n"
                "break;\n"
                "case 'btn7':\n"
                    "if(parseInt($('ID_0x020C').value)==1111)\n"
                    "{\n"
                        "readBack=0x020C; \n"
                    "}else{\n"
                        "show_tip_ex(7,lang.N55[language]);\n"
                        "setTimeout('show_tip_ex(7,\"&nbsp;\")',2000);\n"
                        "setTimeout('$(\"ID_0x020C\").value=9999',2000);\n"
                        "return;\n"
                    "}\n"
                "break;\n"
            "}\n"
            "URL=URL+'&time='+Math.random();\n"
             //alert(URL);
"\n"
            "function success()\n"
            "{\n"
            	"setTimeout('read_back('+readBack+')', 100);\n"
            "}            \n"
"\n"
             //测试
             //success();
            "\n"
            "sendHttpReq(URL,success);\n"
        "}\n"
        "function ntp_change(obj)\n"
        "{\n"
            "var URL='a_004.html?';\n"
            "var readBack=0;\n"
            "if(obj.checked == false){\n"
                "URL=URL+'&0x0225='+85;\n"
            "}else{\n"
                "URL=URL+'&0x0225='+170;\n"
            "}\n"
            "readBack=0x0225;\n"
            "URL=URL+'&time='+Math.random();\n"
             //alert(URL);
"\n"
             //测试
             //success();
"\n"
            "function success()\n"
            "{\n"
                "setTimeout('read_back('+readBack+')', 100);\n"
            "}   \n"
            "sendHttpReq(URL,success);\n"
        "}\n"
        "function IPverify(obj)\n"
        "{\n"
             //需要将 '\\'转换为'\\\\'
            "var reg=/^(\\d{1,2}|1\\d\\d|2[0-4]\\d|25[0-5])(\\.(\\d{1,2}|1\\d\\d|2[0-4]\\d|25[0-5])){3}$/;\n"
            "if(!obj.value.match(reg))\n"
            "{\n"
                "$('btn6').disabled=true;\n"
                "obj.style.color='#F00';\n"
                "show_tip_ex(6,lang.N51[language]);\n"
            "}\n"
            "else\n"
            "{\n"
                "$('btn6').disabled=false;\n"
                "obj.style.color='#000';\n"
                "show_tip_ex(6,'&nbsp;');\n"
            "}\n"
        "}\n"
        "function Verify(obj)\n"
        "{\n"
            "var reg=/^-?[0-9][0-9]*$/;\n"
            "var reg2=/^-?\\d+$/; \n"
            "var range=parseInt(obj.value);\n"
"\n"
            "switch(obj.id)\n"
            "{\n"
                "case 'ID_0x017C':\n"
                    "if(!obj.value.match(reg2)){$('btn2').disabled=true;obj.style.color='#F00';show_tip_ex(2,lang.N51[language]);}\n"
                    "else if(range<-9999||range>9999){$('btn2').disabled=true;obj.style.color='#F00';show_tip_ex(2,lang.N52[language]);}\n"
                    "else{\n"
                        "obj.style.color='#000';\n"
                        "$('btn2').disabled=false;\n"
                        "show_tip_ex(2,'&nbsp;');\n"
                    "}                \n"
                "break;\n"
                "case 'ID_0x0208':\n"
                    "if(!obj.value.match(reg)){$('btn3').disabled=true;obj.style.color='#F00';show_tip_ex(3,lang.N51[language]);}\n"
                    "else if(range<0||range>9999){$('btn3').disabled=true;obj.style.color='#F00';show_tip_ex(3,lang.N52[language]);}\n"
                    "else{\n"
                        "obj.style.color='#000';\n"
                        "$('btn3').disabled=false;\n"
                        "show_tip_ex(3,'&nbsp;');\n"
                    "}\n"
                "break;\n"
                "case 'ID_0x0209':\n"
                    "if(!obj.value.match(reg)){$('btn4').disabled=true;obj.style.color='#F00';show_tip_ex(4,lang.N51[language]);}\n"
                    "else if(range<0||range>9999){$('btn4').disabled=true;obj.style.color='#F00';show_tip_ex(4,lang.N52[language]);}\n"
                    "else{\n"
                        "obj.style.color='#000';\n"
                        "$('btn4').disabled=false;\n"
                        "show_tip_ex(4,'&nbsp;');\n"
                    "}\n"
                "break;\n"
                "case 'ID_0x020A':\n"
                    "if(!obj.value.match(reg)){$('btn5').disabled=true;obj.style.color='#F00';show_tip_ex(5,lang.N51[language]);}\n"
                    "else if(range<0||range>9999){$('btn5').disabled=true;obj.style.color='#F00';show_tip_ex(5,lang.N52[language]);}\n"
                    "else{\n"
                        "obj.style.color='#000';\n"
                        "$('btn5').disabled=false;\n"
                        "show_tip_ex(5,'&nbsp;');\n"
                    "}\n"
                "break;\n"
                "case 'ID_0x0224':\n"
                    "if(!obj.value.match(reg)){$('btn6').disabled=true;obj.style.color='#F00';show_tip_ex(6,lang.N51[language]);}\n"
                    "else if(range<0||range>9999){$('btn6').disabled=true;obj.style.color='#F00';show_tip_ex(6,lang.N52[language]);}\n"
                    "else{\n"
                        "obj.style.color='#000';\n"
                        "$('btn6').disabled=false;\n"
                        "show_tip_ex(6,'&nbsp;');\n"
                    "}\n"
                "break;\n"
            "}         \n"
        "}\n"
    "</script>\n"
"</html>\n"
};
#endif
