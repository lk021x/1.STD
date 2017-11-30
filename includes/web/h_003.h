#ifndef _HTML_H_003_H_
#define _HTML_H_003_H_
static const char h_003[] = {
"HTTP/1.1 200 OK\r\nContent-Type:text/html\r\nAccept-Ranges:bytes\r\n\r\n"
"<!DOCTYPE html PUBLIC '-//W3C//DTD XHTML 1.0 Transitional//EN' 'http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd'>\n"
"<html xmlns='http://www.w3.org/1999/xhtml'>\n"
    "<head>\n"
        "<meta http-equiv='Content-Type' content='text/html; charset=GB2312'/>\n"
        "<link type='text/css' href='c_000.css' rel='stylesheet'/>\n"
        "<script src='j_001.js' type='text/javascript'></script>\n"
        "<style type='text/css'>\n"
        "tr{height:27px;}\n"
        ".td1{text-align:left;padding-left:40px;}\n"
        ".td2{text-align:right;padding-right:5px;}\n"
        ".td3{text-align:right;padding-right:40px;}\n"
        ".div_tab{width:780px;height:45px;margin:0 auto;}\n"
        ".div_btn{width:80px;margin:0 auto;height:30px;text-align:right;margin-top:8px;float:left;}\n"
        ".div_text{width:80px;margin:0 auto;height:25px;text-align:right;margin-top:10px;float:left;}\n"
        ".div_span{width:40px;margin:0 auto;height:25px;font-size:16px;font-weight:bold;text-align:right;margin-top:12px;float:left;}\n"
        ".btn{width:75px;height:30px;}\n"
        ".PagerView{font-size:14px;font-family:tahoma,arial;font-weight:bold;color:#333;text-align:center;padding:8px;line-height:100%;}\n"
        ".PagerView span{color:#999;margin:0 1px;padding:3px 6px;border:1px solid #ccc;}\n"
        ".PagerView span.on{color:#fff;font-weight:bold;border:1px solid #666;background:#888;} \n"
        ".PagerView a{color:#00f;text-decoration:none;}\n"
        ".PagerView a span{color:#33f;border:1px solid #888;}\n"
        ".pager{width:780px;margin:0 auto;}\n"
        "</style>\n"
        "<script type=\"text/javascript\" src=\"jquery.min.js\"></script>\n"
        "<script type=\"text/javascript\" src=\"j_highcharts.js\"></script>\n"
        "<script type='text/javascript'>\n"
            "var ST1=new Array('N024');\n"
            "var ST2=new Array('ID_0x002F');\n"
            "var ST3=new Array('');\n"
"\n"
 		"var DC1=new Array('N003','N004','N005');\n"
 		"var DC2=new Array('ID_0x0020','ID_0x0021','ID_0x0022');\n"
 		"var DC3=new Array('V','A','kW');\n"
 "\n"
 		"var AC1=new Array('N010','N011','N012','N013','N014','N015','N007','N008','N009','N016');\n"
 		"var AC2=new Array('ID_0x0023','ID_0x0027','ID_0x0024','ID_0x0028','ID_0x0025','ID_0x0029',\n"
 							 "'ID_0x002A','ID_0x002B','ID_0x002C','ID_0x0026');\n"
 		"var AC3=new Array('V','A','V','A','V','A','kW','kVar','','Hz');\n"
 							"\n"
 		"var STAC1=new Array('N018','N019');\n"
 		"var STAC2=new Array('ID_0x002D','ID_0x002E');\n"
 		"var STAC3=new Array('kWh','kWh');\n"
            "function show_SUB(TR1,TR2,TR3)\n"
            "{\n"
                "var str='';\n"
                "str+=\"<table width='90%' border='0'>\";\n"
                "for(var i=0;i<TR1.length;i+=2)\n"
                "{\n"
                    "str+=\"<tr id='\"+TR1[i]+i+\"' style='width:100%;height:24;'>\";\n"
                        "str+=\"<td width='25%' class='td1 \"+(i%4?\"even\":\"odd\")+\"'><span id='\"+TR1[i]+\"'></span></td>\";\n"
                        "str+=\"<td width='15%' class='td2 \"+(i%4?\"even\":\"odd\")+\"'><span id='\"+TR2[i]+\"'></span></td>\";\n"
                        "str+=\"<td width='10%' class='td3 \"+(i%4?\"even\":\"odd\")+\"'>\"+TR3[i]+\"</td>\";\n"
 					"if(i+1!=TR1.length)\n"
 					"{\n"
 						"str+=\"<td width='25%' class='td1 \"+(i%4?\"even\":\"odd\")+\"'><span id='\"+TR1[i+1]+\"'></span></td>\";\n"
 						"str+=\"<td width='15%' class='td2 \"+(i%4?\"even\":\"odd\")+\"'><span id='\"+TR2[i+1]+\"'></span></td>\";\n"
 						"str+=\"<td width='10%' class='td3 \"+(i%4?\"even\":\"odd\")+\"'>\"+TR3[i+1]+\"</td>\";\n"
 					"}\n"
 					"else\n"
 					"{\n"
 						"str+=\"<td width='25%' class='td1 \"+(i%4?\"even\":\"odd\")+\"'></td>\";\n"
 						"str+=\"<td width='15%' class='td2 \"+(i%4?\"even\":\"odd\")+\"'></td>\";\n"
 						"str+=\"<td width='10%' class='td3 \"+(i%4?\"even\":\"odd\")+\"'></td>\";\n"
 					"}\n"
 					"\n"
                    "str+=\"</tr>\";\n"
                "}\n"
                "str+=\"</table>\";\n"
                "document.write(str);\n"
            "}\n"
        "var PagerView=function(id,AAA,BBB,CCC)\n"
            "{\n"
                "var self=this;\n"
                "this.id=id;\n"
                "this._pageCount=0;\n"
                "this._start=1;\n"
                "this._end=1;\n"
                "this.container=null;\n"
                "this.index=1;\n"
                "this.size=10;\n"
                "this.maxButtons=18;\n"
                "this.itemCount=0;\n"
                "this.onclick=function(index){return true;};\n"
                "this._onclick=function(index)\n"
                "{\n"
                    "var old=self.index;\n"
                    "self.index=index;\n"
                    "if(self.onclick(index)!==false)self.render();\n"
                    "else self.index=old;\n"
                "};\n"
                "this._calculate=function()\n"
                "{\n"
                    "self._pageCount=parseInt(Math.ceil(self.itemCount/self.size));\n"
                    "self.index=parseInt(self.index);\n"
                    "if(self.index>self._pageCount)self.index=self._pageCount;\n"
                    "if(self.index<1)self.index=1;\n"
                    "self._start=Math.max(1,self.index-parseInt(self.maxButtons/2));\n"
                    "self._end=Math.min(self._pageCount,self._start+self.maxButtons-1);\n"
                    "self._start=Math.max(1,self._end-self.maxButtons+1);\n"
                "};\n"
                "this.page=function(rows)\n"
                "{\n"
                    "self._calculate();\n"
                    "var s_num=(self.index-1)*self.size ;\n"
                    "var e_num=self.index*self.size;\n"
                    "return rows.slice(s_num,e_num);\n"
                "};\n"
                "this.render=function()\n"
                "{\n"
                    "var div=document.getElementById(self.id);\n"
                    "div.view=self;\n"
                    "self.container=div;\n"
                    "self._calculate();\n"
                    "var str='';\n"
                    "str+='<div class=\"PagerView\">\\n';\n"
                    "if(self._pageCount>1)\n"
                    "{\n"
                        "if(self.index!=1)\n"
                        "{\n"
                            "str+='<a href=\"javascript://1\"><span>|&lt;</span></a>';\n"
                            "str+='<a href=\"javascript://'+(self.index-1)+'\"><span>&lt;&lt;</span></a>';\n"
                        "}\n"
                        "else\n"
                        "{\n"
                            "str+='<span>|&lt;</span>';\n"
                            "str+='<span>&lt;&lt;</span>';\n"
                        "}\n"
                    "}\n"
                    "for(var i=self._start;i<=self._end;i++)\n"
                    "{\n"
                        "if(i==this.index)\n"
                        "{\n"
                            "if(i<10)str+='<span class=\"on\">'+'0'+i+\"</span>\";\n"
                            "else str+='<span class=\"on\">'+i+\"</span>\";\n"
                        "}\n"
                        "else\n"
                        "{\n"
                            "if(i<10)str+='<a href=\"javascript://'+i+'\"><span>'+'0'+i+\"</span></a>\";\n"
                            "else str+='<a href=\"javascript://'+i+'\"><span>'+i+\"</span></a>\";\n"
                        "}\n"
                    "}\n"
                    "if(self._pageCount>1)\n"
                    "{\n"
                        "if(self.index!=self._pageCount)\n"
                        "{\n"
                            "str+='<a href=\"javascript://'+(self.index+1)+'\"><span>&gt;&gt;</span></a>';\n"
                            "str+='<a href=\"javascript://'+self._pageCount+'\"><span>&gt;|</span></a>';\n"
                        "}\n"
                        "else\n"
                        "{\n"
                            "str+='<span>&gt;&gt;</span>';\n"
                            "str+='<span>&gt;|</span>';\n"
                        "}\n"
                    "}\n"
                    "str+=AAA+self._pageCount+BBB+self.itemCount+CCC;\n"
                    "str+='</div><!--/.pagerView-->\\n';\n"
                    "self.container.innerHTML=str;\n"
                    "var a_list=self.container.getElementsByTagName('a');\n"
                    "for(var i=0;i<a_list.length;i++)\n"
                    "{\n"
                        "a_list[i].onclick=function()\n"
                        "{\n"
                            "var index=this.getAttribute('href');\n"
                            "if(index!=undefined&&index!='')\n"
                            "{\n"
                                "index=parseInt(index.replace('javascript://',''));\n"
                                "self._onclick(index);\n"
                            "}\n"
                            "return false;\n"
                        "};\n"
                    "}\n"
                "};\n"
            "};\n"
        "</script>\n"
    "</head>\n"
    "<body onload='Init();'>\n"
        "<script type='text/javascript'>\n"
            "show_T('N001');\n"
        "</script>        \n"
        "<div id='pager' width='90%' style='padding-left:5%;float:left;'></div>\n"
        "<div class='line_long'></div>\n"
        "<div class='blank'></div>\n"
        "<script type='text/javascript'>\n"
            "show_sT('N023');\n"
            "show_SUB(ST1,ST2,ST3);\n"
        "</script>\n"
        "<div class='line_long'></div>\n"
        "<div class='blank'></div>\n"
        "<script type='text/javascript'>\n"
            "show_sT('N002');\n"
            "show_SUB(DC1,DC2,DC3);\n"
        "</script>\n"
        "<div class='line_long'></div>\n"
        "<div class='blank'></div>\n"
        "<script type='text/javascript'>\n"
            "show_sT('N006');\n"
            "show_SUB(AC1,AC2,AC3);\n"
        "</script>\n"
        "<div class='line_long'></div>\n"
        "<div class='blank'></div>\n"
        "<script type='text/javascript'>\n"
            "show_sT('N017');\n"
            "show_SUB(STAC1,STAC2,STAC3);\n"
        "</script>\n"
        "<div class='line_long'></div>\n"
        "<div class='blank'></div>\n"
        "<div style='width:100%;' id='fault_code'></div>\n"
        "<div class='btm_blank'></div>\n"
        "<!--[if IE 6]><script type='text/javascript'>document.execCommand('BackgroundImageCache',false,true);</script><![endif]-->\n"
    "</body>\n"
    "<script type='text/javascript'>\n"
        "window.history.forward(1);\n"
         //var $=function(o){return document.getElementById(o);};
        "var language=0;\n"
        "var Json;\n"
        "var CurPcs;\n"
        "var lang=\n"
        "{\n"
            "N001:['�����������Ϣ','PCS Information'],\n"
            "N002:['ֱ������Ϣ','DC Information'],\n"
            "N003:['���������ֱ����ѹ','PCS DC Voltage'],\n"
            "N004:['���������ֱ������','PCS DC Current'],\n"
            "N005:['���������ֱ������','PCS DC Power'],\n"
            "N006:['��������Ϣ','AC Information'],\n"
            "N007:['����������й�����','PCS Active Power'],\n"
            "N008:['����������޹�����','PCS Reactive Power'],\n"
            "N009:['�����������������','PCS Powerfactor'],\n"
            "N010:['���������Vab','Vab'],\n"
            "N011:['���������A�����','Ia'],\n"
            "N012:['���������Vbc','Vbc'],\n"
            "N013:['���������B�����','Ib'],\n"
            "N014:['���������Vca','Vca'],\n"
            "N015:['���������C�����','Ic'],\n"
            "N016:['�������������Ƶ��','PCS Frequency'],\n"
            "N017:['ͳ����Ϣ','Statistics Information'],\n"
            "N018:['����������ۼƳ��','E-Charge PCS'],\n"
            "N019:['����������ۼƷŵ�','E-Discharge PCS'],\n"
            "N020:['�ڣ�','No.��'],\n"
            "N021:['����','All��'],\n"
            "N022:['ʵʱ����', 'Real-time Power'],\n"
            "N023:['������Ϣ','Runing Info'],\n"
            "N024:['����������豸״̬','PCS Status'],\n"
 		"\n"
            "N060:['������Ϣ','Fault Information'],\n"
            "N061:['����&nbsp;','Fault&nbsp;'],\n"
            "N062:['ֱ����ѹ','DC Voltage High'],\n"
            "N063:['ֱ��Ƿѹ','DC Voltage Low'],\n"
            "N064:['ֱ������','DC Current High'],\n"
            "N065:['������ѹ','AC Voltage High'],\n"
            "N066:['����Ƿѹ','AC Voltage Low'],\n"
            "N067:['������Ƶ','AC Frequency High'],\n"
            "N068:['����ǷƵ','AC Frequency Low'],\n"
            "N069:['��������','AC Current High'],\n"
            "N070:['����©��������','I-leak Protection'],\n"
            "N071:['�µ�����','Island Protection'],\n"
            "N072:['��λ�쳣','Phase Fault'],\n"
            "N073:['��������ѹ����','Vind-inv High'],\n"
            "N074:['������ѹ��ƽ��','AC Voltage Unbalanced'],\n"
            "N075:['����������ƽ��','AC Current Unbalanced'],\n"
            "N076:['��������','Temp High'],\n"
            "N077:['ģ��A1����','Module A1 Temp High'],\n"
            "N078:['ģ��B1����','Module B1 Temp High'],\n"
            "N079:['ģ��C1����','Module C1 Temp High'],\n"
            "N080:['��ѹ������','Transformer Temp High'],\n"
            "N081:['�翹��1����','Reactor 1 Temp High'],\n"
            "N082:['�翹��2����','Reactor 2 Temp High'],\n"
            "N083:['ģ�����','Module Fault'],\n"
            "N084:['���ȹ���','Fan Fault'],\n"
            "N085:['������ʧЧ','Sensor Error'],\n"
            "N086:['�������Ӵ�������','AC Main Contactor Fault'],\n"
            "N087:['ֱ�����Ӵ�������','DC Main Contactor Fault'],\n"
            "N088:['A�������������','A Current AD Sample Fault'],\n"
            "N089:['B�������������','B Current AD Sample Fault'],\n"
            "N090:['C�������������','C Current AD Sample Fault'],\n"
            "N091:['©������������','I-leak AD Sample Fault'],\n"
            "N092:['',''],\n"
            "N093:['',''],\n"
            "N094:['A������ѹ��������','A Voltage AD Sample Fault'],\n"
            "N095:['B������ѹ��������','B Voltage AD Sample Fault'],\n"
            "N096:['C������ѹ��������','C Voltage AD Sample Fault'],\n"
            "N097:['A����ݵ�ѹ��������','A Capacity AD Sample Fault'],\n"
            "N098:['B����ݵ�ѹ��������','B Capacity AD Sample Fault'],\n"
            "N099:['C����ݵ�ѹ��������','C Capacity AD Sample Fault'],\n"
            "N100:['',''],\n"
            "N101:['',''],\n"
            "N102:['',''],\n"
            "N103:['��ع���','Battery Fault'],\n"
            "N104:['DSPͨѶ����','DSP Com-fault'],\n"
            "N105:['���ͨѶ����','BMS Com-fault'],\n"
            "N106:['',''],\n"
            "N107:['',''],\n"
            "N108:['',''],\n"
            "N109:['ģ��A2����','Module A2 Temp High'],\n"
            "N110:['ģ��B2����','Module B2 Temp High'],\n"
            "N111:['ģ��C2����','Module C2 Temp High'],\n"
            "N112:['��������������','AC SPD Fault'],\n"
            "N113:['ֱ���۶�������','DC Fuse Fault'],\n"
            "N114:['����۶�������','Detc. Fuse Fault'],\n"
            "N115:['���Է��ӹ���','Reversed Polarity Fault'],\n"
            "N116:['',''],\n"
            "N117:['',''],\n"
            "N118:['',''],\n"
            "N119:['',''],\n"
            "N120:['',''],\n"
            "N121:['',''],\n"
            "N122:['',''],\n"
            "N123:['',''],\n"
            "N124:['',''],\n"
            "N125:['',''],\n"
             //����״̬
            "N200:['',''],\n"
            "N201:['��ʼͣ��','Initial stop'],\n"
            "N202:['����ͣ��','EMG stop'],\n"
            "N203:['����','Standby'],\n"
            "N204:['�������','CC charge'],\n"
            "N205:['��ѹ���','CV charge'],\n"
            "N206:['�㹦�ʳ��(DC)','CP charge(DC)'],\n"
            "N207:['�㹦�ʳ��(AC)','CP charge(AC)'],\n"
            "N208:['',''],\n"
            "N209:['�����ŵ�','CC discharge'],\n"
            "N210:['��ѹ�ŵ�','CV discharge'],\n"
            "N211:['�㹦�ʷŵ�(DC)','CP discharge(DC)'],\n"
            "N212:['�㹦�ʷŵ�(AC)','CP discharge(AC)'],\n"
            "N213:['',''],\n"
            "N214:['�������','Invert'],\n"
            "N215:['',''],\n"
            "N216:['',''],\n"
            "N217:['����','Fault'],\n"
            "N218:['ͣ��','Stop'],\n"
            "N219:['',''],\n"
            "N220:['',''],\n"
            "N221:['',''],\n"
            "N222:['',''],\n"
            "N223:['',''],\n"
            "N224:['',''],\n"
            "N225:['',''],\n"
            "N226:['',''],\n"
            "N227:['',''],\n"
            "N228:['',''],\n"
            "N229:['',''],\n"
            "N230:['',''],\n"
            "N231:['',''],\n"
            "N232:['',''],\n"
"\n"
            "N500:['һ��','Total '],\n"
            "N501:['ҳ����',' Pages, '],\n"
            "N502:['ҳ����',' Page, '],\n"
            "N503:['̨',' Nums'],\n"
            "N504:['̨',' Num']\n"
        "};\n"
"\n"
         //��ȡ����PCS����, ��ȡPCS ��ҳnet����
        "function Get_PCS_Index(index)\n"
        "{           \n"
            "eval('var URL=\\'s_003.html?0x'+(0+index).toString(16).toUpperCase()+'&time=\\'+Math.random();');\n"
            "var xmlhttp;\n"
            "if(window.XMLHttpRequest)xmlhttp=new XMLHttpRequest();\n"
            "else xmlhttp=new ActiveXObject('Microsoft.XMLHTTP');\n"
            "xmlhttp.onreadystatechange=function()\n"
            "{\n"
                "if(xmlhttp.readyState==4 && xmlhttp.status==200)\n"
                "{\n"
                    "eval('Json='+xmlhttp.responseText);    \n"
                    "if(Json.ID_0x02B0 == 0)\n"
                    "{\n"
                        "document.getElementById('ID_0x0020').innerHTML=(Json.ID_0x0020/10.0).toFixed(1);\n"
                        "document.getElementById('ID_0x0021').innerHTML=(Json.ID_0x0021/10.0).toFixed(1);\n"
                        "document.getElementById('ID_0x0022').innerHTML=(Json.ID_0x0022/10.0).toFixed(1);\n"
                        "document.getElementById('ID_0x002A').innerHTML=(Json.ID_0x002A/10.0).toFixed(1);\n"
                        "document.getElementById('ID_0x002B').innerHTML=(Json.ID_0x002B/10.0).toFixed(1);\n"
                        "document.getElementById('ID_0x002C').innerHTML=(Json.ID_0x002C/1000.0).toFixed(3);\n"
                        "document.getElementById('ID_0x0023').innerHTML=(Json.ID_0x0023/10.0).toFixed(1);\n"
                        "document.getElementById('ID_0x0027').innerHTML=(Json.ID_0x0027/10.0).toFixed(1);\n"
                        "document.getElementById('ID_0x0024').innerHTML=(Json.ID_0x0024/10.0).toFixed(1);\n"
                        "document.getElementById('ID_0x0028').innerHTML=(Json.ID_0x0028/10.0).toFixed(1);\n"
                        "document.getElementById('ID_0x0025').innerHTML=(Json.ID_0x0025/10.0).toFixed(1);\n"
                        "document.getElementById('ID_0x0029').innerHTML=(Json.ID_0x0029/10.0).toFixed(1);\n"
                        "document.getElementById('ID_0x0026').innerHTML=(Json.ID_0x0026/100.0).toFixed(2);\n"
                        "document.getElementById('ID_0x002D').innerHTML=Json.ID_0x002D;\n"
                        "document.getElementById('ID_0x002E').innerHTML=Json.ID_0x002E;\n"
                         //����״̬
                        "eval('document.getElementById(\\'ID_0x002F\\').innerHTML=lang.N'+(Json.ID_0x002F+200)+'[language];');\n"
                        "if(Json.ID_0x002F == 0 || Json.ID_0x002F == 2) \n"
                        "{\n"
                            "document.getElementById('ID_0x002F').style.color='#ff6820';\n"
                            "document.getElementById('ID_0x002F').style.fontWeight='bold';                    \n"
                        "}\n"
                        "else if(Json.ID_0x002F == 17)\n"
                        "{\n"
                            "document.getElementById('ID_0x002F').style.color='#ff0000';\n"
                            "document.getElementById('ID_0x002F').style.fontWeight='bold';\n"
                        "}\n"
                        "else\n"
                        "{\n"
                            "document.getElementById('ID_0x002F').style.fontWeight='normal';\n"
                            "document.getElementById('ID_0x002F').style.color='#000';                        \n"
                        "}\n"
                        "Fault_decode(); \n"
                    "}     \n"
                    "else\n"
                    "{\n"
                        "document.getElementById('ID_0x0020').innerHTML='--';\n"
                        "document.getElementById('ID_0x0021').innerHTML='--';\n"
                        "document.getElementById('ID_0x0022').innerHTML='--';\n"
                        "document.getElementById('ID_0x002A').innerHTML='--';\n"
                        "document.getElementById('ID_0x002B').innerHTML='--';\n"
                        "document.getElementById('ID_0x002C').innerHTML='--';\n"
                        "document.getElementById('ID_0x0023').innerHTML='--';\n"
                        "document.getElementById('ID_0x0027').innerHTML='--';\n"
                        "document.getElementById('ID_0x0024').innerHTML='--';\n"
                        "document.getElementById('ID_0x0028').innerHTML='--';\n"
                        "document.getElementById('ID_0x0025').innerHTML='--';\n"
                        "document.getElementById('ID_0x0029').innerHTML='--';\n"
                        "document.getElementById('ID_0x0026').innerHTML='--';\n"
                        "document.getElementById('ID_0x002D').innerHTML='--';\n"
                        "document.getElementById('ID_0x002E').innerHTML='--';\n"
"\n"
                         //����״̬
                        "document.getElementById('ID_0x002F').innerHTML='--';\n"
                        "document.getElementById('ID_0x002F').style.fontWeight='normal';\n"
                        "document.getElementById('ID_0x002F').style.color='#000'; \n"
                        "Fault_decode();\n"
                    "}\n"
                "}\n"
            "};\n"
            "xmlhttp.open('GET',URL,true);\n"
            "xmlhttp.send();\n"
             //alert(URL);   
        "}        \n"
        "function ajax()\n"
        "{\n"
            "var URL='s_003.html?time='+Math.random();\n"
            "var xmlhttp;\n"
            "if(window.XMLHttpRequest)xmlhttp=new XMLHttpRequest();\n"
            "else xmlhttp=new ActiveXObject('Microsoft.XMLHTTP');\n"
            "xmlhttp.onreadystatechange=function()\n"
            "{\n"
                "if(xmlhttp.readyState==4 && xmlhttp.status==200)\n"
                "{\n"
                    "eval('Json='+xmlhttp.responseText);           \n"
         		"if(Json.ID_0x0301==0x55)language=0;else language=1;\n"
         		"for(var i=1;i<=19;i++)\n"
         		"{\n"
         			"eval('document.getElementById(\\'N0'+(i<10?'0'+i:i)+'\\').innerHTML=lang.N0'+(i<10?'0'+i:i)+'[language];');\n"
         		"}\n"
                     //document.getElementById('N022').innerHTML=lang.N022[language];
                    "document.getElementById('N023').innerHTML=lang.N023[language];\n"
                    "document.getElementById('N024').innerHTML=lang.N024[language];\n"
                    "if(Json.ID_0x0350==0){\n"
                        "document.getElementById('pager').style.display='none';\n"
                    "}else{\n"
                        "document.getElementById('pager').style.display='block';\n"
                        "mpager=new PagerView('pager',lang.N500[language],lang.N502[language],lang.N503[language]);\n"
                    "}\n"
  "\n"
                    "mpager.itemCount=Json.ID_0x0350;\n"
                    "mpager.size=1;\n"
                    "mpager.index=1; \n"
                    "CurPcs = mpager.index;\n"
                    "Get_PCS_Index(CurPcs);               \n"
                    "mpager.onclick=function(index)\n"
                    "{  \n"
                        "CurPcs = index;\n"
                         //��ȡPCS�����������Ϣ
                        "Get_PCS_Index(index);\n"
                    "};\n"
                     //index��ʾ����
                    "mpager.render(); \n"
                "}\n"
            "};\n"
            "xmlhttp.open('GET',URL,true);\n"
            "xmlhttp.send();\n"
        "}\n"
        "window.onresize = function()\n"
        "{\n"
            "var bodywidth=document.body.clientWidth;\n"
             //��С����867=780/0.9
            "if(bodywidth<867){document.body.style.width=\"867px\";}else{document.body.style.width=\"auto\";}\n"
        "} \n"
        "function Init()\n"
        "{\n"
            "ajax();\n"
            "setInterval('Get_PCS_Index(CurPcs)', 2000);\n"
        "}\n"
        "function show_fault(F_num)\n"
        "{\n"
            "var loop=Math.ceil(F_num/2);\n"
            "var str='';\n"
            "str+=\"<div class='blank'></div>\";\n"
            "str+=\"<div class='title' style='width:90%;'>\";\n"
            "str+=\"<div class='subtitle'>\";\n"
            "str+=\"<div class='r1 subcolor'></div>\";\n"
            "str+=\"<div class='r2 subcolor'></div>\";\n"
            "str+=\"<div class='r3 subcolor'></div>\";\n"
            "str+=\"<div class='r4 subcolor'></div>\";\n"
            "str+=\"<div class='content subcolor subcontent'>\";\n"
            "str+=\"<span id='N060'></span>\";\n"
            "str+=\"</div>\";\n"
            "str+=\"<div class='r4 subcolor'></div>\";\n"
            "str+=\"<div class='r3 subcolor'></div>\";\n"
            "str+=\"<div class='r2 subcolor'></div>\";\n"
            "str+=\"<div class='r1 subcolor'></div>\";\n"
            "str+=\"<div class='line_short'></div>\";\n"
            "str+=\"</div>\";\n"
            "str+=\"</div>\";\n"
            "str+=\"<table width='90%' border='0'>\";\n"
            "var j=1;\n"
            "for(var i=1;i<=loop;i++,j++)\n"
            "{\n"
                "if(i%2==1)\n"
                "{\n"
                    "str+=\"<tr>\";\n"
 					"str+=\"<td class='td1 odd' width='126'><span id='\"+'FWN_'+(j*2-1)+\"'></span></td>\";\n"
 					"str+=\"<td class='td3 odd' width='255'><span style='color:#F00' id='\"+'FWC_'+(j*2-1)+\"'></span></td>\";\n"
 					"str+=\"<td class='td1 odd' width='126'><span id='\"+'FWN_'+j*2+\"'></span></td>\";\n"
 					"str+=\"<td class='td3 odd' width='255'><span style='color:#F00' id='\"+'FWC_'+j*2+\"'></span></td>\";\n"
                    "str+=\"</tr>\";\n"
                "}\n"
                "else\n"
                "{\n"
                    "str+=\"<tr>\";\n"
 					"str+=\"<td class='td1 even'><span id='\"+'FWN_'+(j*2-1)+\"'></span></td>\";\n"
 					"str+=\"<td class='td3 even'><span style='color:#F00' id='\"+'FWC_'+(j*2-1)+\"'></span></td>\";\n"
 					"if(j*2<=F_num)\n"
 					"{\n"
 						"str+=\"<td class='td1 even'><span id='\"+'FWN_'+j*2+\"'></span></td>\";\n"
 						"str+=\"<td class='td3 even'><span style='color:#F00' id='\"+'FWC_'+j*2+\"'></span></td>\";\n"
 					"}\n"
 					"else\n"
 					"{\n"
 						"str+=\"<td class='td1 even'></td>\";\n"
 						"str+=\"<td class='td3 even'></td>\";\n"
 					"}\n"
                    "str+=\"</tr>\";\n"
                "}\n"
            "}\n"
            "str+=\"</table>\";\n"
            "str+=\"<div class='line_long'></div>\";\n"
            "if((F_num)!=0)\n"
            "{\n"
                "document.getElementById('fault_code').innerHTML=str;\n"
                "document.getElementById('N060').innerHTML=lang.N060[language];\n"
                "for(var F=1;F<=F_num;F++)\n"
                "{\n"
                    "document.getElementById('FWN_'+F).innerHTML=lang.N061[language]+(F<=9?'0'+F:F);\n"
                    "document.getElementById('FWC_'+F).innerHTML=F_W_Code[F-1];\n"
                "}\n"
            "}\n"
 		"else\n"
 		"{\n"
                "document.getElementById('fault_code').innerHTML='';\n"
 		"}\n"
        "}\n"
        "var F_W_Code=new Array();\n"
        "function Fault_decode()\n"
        "{\n"
            "var num=0;\n"
            "var i=0,j=0;\n"
            "var Fcode=new Array(2);\n"
            "var CodeNum=new Array(2);\n"
            "var Total_F_Num=0;\n"
            "Fcode[0]=Json.ID_0x0030;\n"
            "Fcode[1]=Json.ID_0x0031;\n"
            "for(i=0;i<2;i++)\n"
            "{\n"
                "CodeNum[i]=0;\n"
                "for(j=0;j<32;j++)\n"
                "{\n"
                    "if(((Fcode[i]>>j)&1)==1)\n"
                    "{\n"
                        "CodeNum[i]++;\n"
 					"if(i==0)eval('F_W_Code[num++]=lang.N'+pad(j+62,3)+'[language];');\n"
 					"if(i==1)eval('F_W_Code[num++]=lang.N'+pad(j+94,3)+'[language];');\n"
                    "}\n"
                "}\n"
            "}\n"
            "Total_F_Num=CodeNum[0]+CodeNum[1];\n"
            "show_fault(Total_F_Num);\n"
        "}\n"
        "function pad(num,n)\n"
        "{\n"
            "var len=num.toString().length;\n"
            "while(len++<n)num='0'+num;\n"
            "return num;\n"
        "}    \n"
    "</script>\n"
"</html>\n"
};
#endif