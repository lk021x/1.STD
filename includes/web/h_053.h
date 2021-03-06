#ifndef _HTML_H_053_H_
#define _HTML_H_053_H_
static const char h_053[] = {
"HTTP/1.1 200 OK\r\nContent-Type:text/html\r\nAccept-Ranges:bytes\r\n\r\n"
"<!DOCTYPE html PUBLIC '-//W3C//DTD XHTML 1.0 Transitional//EN' 'http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd'>\n"
"<html xmlns='http://www.w3.org/1999/xhtml'>\n"
 "<head>\n"
 	"<meta http-equiv='Content-Type' content='text/html; charset=GB2312'/>\n"
 	"<link type='text/css' href='c_000.css' rel='stylesheet'/>\n"
 	"<style type='text/css'>\n"
 		"tr{height:27px;}\n"
 		".select{width:180px;_width:150px;}\n"
 		".time{width:65px;_width:100px;}\n"
 		".txt{width:70px;text-align:center;}\n"
 		".short_txt{width:50px;}\n"
 		".td_value{text-align:right;padding-right:20px;}\n"
 		".td1{padding-left:20px;padding-right:20px;text-align:left;}\n"
 		".td2{padding-left:0px;text-align:center;}\n"
 		".td3{padding-left:60px;text-align:left;}\n"
 		".td4{padding-right:5px;text-align:right;}\n"
 		".td5{padding-right:10px;text-align:right;}\n"
 		".radio{padding-left:30px;}\n"
 		".btnStyle{width:70px;height:24px;}\n"
 		".submenu{line-height:32px;padding-left:20px;width:225px;height:32px;color:#000;background-color:#eee;font-weight:normal;font-size:14px;}\n"
 	"</style>	\n"
 	"<script type='text/javascript'>\n"
 		"var $=function(o){return document.getElementById(o);};\n"
 		"function lang_init_ex(st,end){for(var i=st;i<=end;i++)eval('$(\\'N'+(i<10?'0'+i:i)+'\\').innerHTML=lang.N'+(i<10?'0'+i:i)+'[language];');}\n"
 		"function show_tip(obj){$('tip').innerHTML=obj;}\n"
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
 	"<script type='text/javascript'>\n"
 		"var language=0;\n"
 			//正整数+0
 		"var reg=/^(0|[1-9]\\d*)$/;\n"
 		"var Json;\n"
 		"var flg_bms = [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0];\n"
 		"var lang=\n"
 		"{\n"
 				//BMS, bnt ID 从300开始
 			"N4250:['设&nbsp;&nbsp;置','Set'],\n"
 			"N4251:['设&nbsp;&nbsp;置','Set'],\n"
 			"N4252:['设&nbsp;&nbsp;置','Set'],\n"
 			"N4253:['设&nbsp;&nbsp;置','Set'],\n"
 			"N4254:['设&nbsp;&nbsp;置','Set'],\n"
 			"N4255:['设&nbsp;&nbsp;置','Set'],\n"
 			"N4256:['设&nbsp;&nbsp;置','Set'],\n"
 			"N4257:['设&nbsp;&nbsp;置','Set'],\n"
 			"N4258:['设&nbsp;&nbsp;置','Set'],\n"
 			"N4259:['设&nbsp;&nbsp;置','Set'],\n"
 			"N4260:['设&nbsp;&nbsp;置','Set'],\n"
 			"N4261:['设&nbsp;&nbsp;置','Set'],\n"
 			"N4262:['设&nbsp;&nbsp;置','Set'],\n"
 			"N4263:['设&nbsp;&nbsp;置','Set'],\n"
 			"N4264:['设&nbsp;&nbsp;置','Set'],\n"
 			"N4265:['设&nbsp;&nbsp;置','Set'],\n"
 			"N4266:['设&nbsp;&nbsp;置','Set'],\n"
 			"N4267:['设&nbsp;&nbsp;置','Set'],\n"
 			"N4268:['设&nbsp;&nbsp;置','Set'],\n"
 			"N4269:['设&nbsp;&nbsp;置','Set'],\n"
 			"N4270:['设&nbsp;&nbsp;置','Set'],\n"
 			"N4271:['设&nbsp;&nbsp;置','Set'],\n"
 			"N4272:['设&nbsp;&nbsp;置','Set'],\n"
 			"N4273:['设&nbsp;&nbsp;置','Set'],\n"
 			"N4274:['设&nbsp;&nbsp;置','Set'],\n"
 			"N4275:['设&nbsp;&nbsp;置','Set'],\n"
 			"N4276:['设&nbsp;&nbsp;置','Set'],\n"
 			"N4277:['设&nbsp;&nbsp;置','Set'],\n"
 			"N4278:['设&nbsp;&nbsp;置','Set'],\n"
 			"N4279:['设&nbsp;&nbsp;置','Set'],\n"
 			"N4280:['设&nbsp;&nbsp;置','Set'],\n"
 			"N4281:['设&nbsp;&nbsp;置','Set'],\n"
 			"N4282:['设&nbsp;&nbsp;置','Set'],\n"
 			"N4283:['设&nbsp;&nbsp;置','Set'],\n"
 			"N4284:['设&nbsp;&nbsp;置','Set'],\n"
 			"N4285:['设&nbsp;&nbsp;置','Set'],\n"
 			"N4286:['设&nbsp;&nbsp;置','Set'],\n"
 			"N4287:['设&nbsp;&nbsp;置','Set'],\n"
 			"N4288:['设&nbsp;&nbsp;置','Set'],\n"
 			"N4289:['设&nbsp;&nbsp;置','Set'],\n"
 			"N4290:['设&nbsp;&nbsp;置','Set'],\n"
 			"N4291:['设&nbsp;&nbsp;置','Set'],\n"
 			"N4292:['设&nbsp;&nbsp;置','Set'],\n"
 			"N4293:['设&nbsp;&nbsp;置','Set'],\n"
 			"N4294:['设&nbsp;&nbsp;置','Set'],\n"
 			"N4295:['设&nbsp;&nbsp;置','Set'],\n"
 			"N4296:['设&nbsp;&nbsp;置','Set'],\n"
 			"N4297:['设&nbsp;&nbsp;置','Set'],\n"
 			"N4298:['设&nbsp;&nbsp;置','Set'],\n"
 			"N4299:['设&nbsp;&nbsp;置','Set'],\n"
 			"N4300:['设&nbsp;&nbsp;置','Set'],\n"
 			"N4301:['设&nbsp;&nbsp;置','Set'],\n"
 			"N4302:['设&nbsp;&nbsp;置','Set'],\n"
 			"N4303:['设&nbsp;&nbsp;置','Set'],\n"
 			"N4304:['设&nbsp;&nbsp;置','Set'],\n"
 			"N4305:['设&nbsp;&nbsp;置','Set'],\n"
 			"N4306:['设&nbsp;&nbsp;置','Set'],\n"
 			"N4307:['设&nbsp;&nbsp;置','Set'],\n"
 			"N4308:['设&nbsp;&nbsp;置','Set'],\n"
 			"N4309:['设&nbsp;&nbsp;置','Set'],\n"
"\n"
 	 		"N500:['一共','Total '],\n"
 	 		"N501:['页，共',' Pages, '],\n"
 	 		"N502:['页，共',' Page, '],\n"
 	 		"N503:['台',' Nums'],\n"
 	 		"N504:['台',' Num'],\n"
"\n"
 			"N541:['设置失败！','Set failed!'],\n"
 			"N542:['设置成功！','Set successfully!'],\n"
 			"N543:['格式错误！','Format error!'],\n"
 			"N544:['范围错误！','Range error!'],\n"
"\n"
 			"N550:['设&nbsp;&nbsp;置','Set'],\n"
 			"N551:['设&nbsp;&nbsp;置','Set']\n"
 		"};\n"
"\n"
 			//显示设置Mdobus地址设置框 nid -- text框id(实际的)，bid -- 确认按钮id(>=1)，tid -- 提示信息区id(>=1), num -- 添加的个数
 		"function show_Set(nid,bid,tid,num)\n"
 		"{\n"
 			"var loop=0;\n"
 			"var str='';\n"
 			"var style=\"odd\";\n"
 			"var more=0, all=0;\n"
 			"var line=0;\n"
 			"more=num%4;\n"
 			"if(more!=0){all=num+(4-more);}\n"
 			"else{all=num;}\n"
 			"str+=\"<tr id='t\"+(bid+line)+\"' style='display:none'>\";\n"
 			"for(var i=0; i<all; i++)\n"
 			"{\n"
 				"if(i%8<4){style=\"odd\"}else{style=\"even\"}\n"
 				"if(i>=num){\n"
 					"str+=\"<td width='12%' class='td1 odd \"+style+\"'><span id='text\"+(bid+i)+\"'>No.Na</span></td><td width='9%' class='td2 \"+style+\"'><input class='txt' type='text' value='Na' disabled='true'/></td><td width='8%' class='td2 \"+style+\"'><button class='btnStyle' disabled='true' style='display:block;'></td>\";\n"
 				"}else{						\n"
 					"str+=\"<td width='12%' class='\"+style+\"'><span id='text\"+(bid+i)+\"'>No.\"+(i<9?'0'+(i+1):(i+1))+\"</span></td><td width='9%' class='td2 \"+style+\"'><input class='txt' id='ID_0x0\"+(nid+i).toString(16).toUpperCase()+\"' value='\"+(i+1)+\"' maxlength='3' onkeyup='doRangeCheck(this,reg)' type='text'/></td><td width='%8' class='td2 \"+style+\"'><button class='btnStyle' onclick='doSubmit(this)' id='N\"+(bid+i)+\"' style='display:block;'></td>\";	\n"
 				"}	\n"
 				"if(i%4==3){\n"
 					"loop=0;\n"
 					"line++;\n"
 					"if(i!=all-1){str+=\"</tr><tr id='t\"+(bid+line)+\"' style='display:none'>\"};\n"
"\n"
 				"}\n"
 				"loop++;\n"
 			"}\n"
 			"str+=\"</tr>\";\n"
 			"document.write(str);\n"
 		"}				\n"
 	"</script>\n"
 "</head>\n"
 "<body onload='Init()'>\n"
 	"<!-- BMS -->\n"
 	"<table id='tbl_bms' width='90%' border='1' bordercolor=\"#E0E0E0\" style='display:none;'>\n"
 		"<script type='text/javascript'>\n"
 		"show_Set(0x840, 4250, 4, 60);\n"
 		"lang_init_ex(4250,4309);\n"
 		"</script>\n"
 	"</table>\n"
 	"<div class='tip' id='tip3' style='width:90%;'><span>&nbsp;</span></div>	\n"
 	"<div class='btm_blank'></div>\n"
 	"<div id='trans'></div>\n"
 	"<!--[if IE 6]><script type='text/javascript'>document.execCommand('BackgroundImageCache',false,true);</script><![endif]-->\n"
 "</body>\n"
 "<script language='javascript' type='text/javascript'>\n"
 "\n"
 	"function change_color(obj)\n"
 	"{\n"
 		"if(current_id!=obj.id)\n"
 		"{\n"
 			"obj.style.background='#007b3e';\n"
 			"obj.style.color='#fff';\n"
 		"}\n"
 	"}\n"
 	"function resume_color(obj)\n"
 	"{\n"
 		"if(current_id!=obj.id)\n"
 		"{\n"
 			"obj.style.background='#aaa';\n"
 			"obj.style.color='#000';\n"
 		"}\n"
 	"}\n"
 	"\n"
 		//bms
 	"function choose_bms()\n"
 	"{\n"
 		"var lin=0;\n"
 		"if(Json.ID_0x0394==1){ $('tbl_bms').style.display='table';$('tip3').style.display='block';} else { $('tbl_bms').style.display='none';  $('tip3').style.display='none';}\n"
 			//bms
 		"for(var i=0; i<60; i++){\n"
 			"if(i>=Json.ID_0x0354){\n"
 				"eval('$(\\'ID_0x0'+(0x840+i).toString(16).toUpperCase()+'\\').value=\\'\\';');\n"
 				"eval('$(\\'ID_0x0'+(0x840+i).toString(16).toUpperCase()+'\\').disabled=\\'true\\';');\n"
 				"eval('$(\\'N'+(4250+i)+'\\').disabled=true;');\n"
 				"eval('$(\\'N'+(4250+i)+'\\').style.background=\"#F0F0F0\";');\n"
 				"eval('$(\\'text'+(4250+i)+'\\').innerHTML=\\'No.--\\';');\n"
 			"}else{				\n"
 				"if(Json.ID_0x0282==0x22){\n"
 					"eval('$(\\'N'+(4250+i)+'\\').disabled=false;');\n"
 					"eval('$(\\'N'+(4250+i)+'\\').style.background=\"#99D9EA\";');\n"
 				"}else{\n"
 					"eval('$(\\'N'+(4250+i)+'\\').style.background=\"#F0F0F0\";');\n"
 				"}		\n"
 			"}\n"
 		"}	\n"
 		"if(Json.ID_0x0354%4==0){lin=parseInt(Json.ID_0x0354/4);}\n"
 		"else {lin=parseInt(Json.ID_0x0354/4)+1;}\n"
 		"for(var j=0; j<15; j++)\n"
 		"{\n"
 			"if(j>=lin) eval('$(\\'t'+(4250+j)+'\\').style.display=\\'none\\';');\n"
 			"else eval('$(\\'t'+(4250+j)+'\\').style.display=\\'table\\';');\n"
 		"}\n"
 		"Set_BMS_For_Serial();		\n"
 	"}\n"
 "\n"
 		//设置串口BMS数据
 	"function Set_BMS_For_Serial()\n"
 	"{\n"
 		"var URL='s_027.html?0x0840&time='+Math.random();\n"
 		"var xmlhttp;\n"
 		"if(window.XMLHttpRequest)xmlhttp=new XMLHttpRequest();\n"
 		"else xmlhttp=new ActiveXObject('Microsoft.XMLHTTP');\n"
 		"xmlhttp.onreadystatechange=function()\n"
 		"{\n"
 			"if(xmlhttp.readyState==4 && xmlhttp.status==200)\n"
 			"{\n"
 				"eval('Json='+xmlhttp.responseText);			\n"
 				"for(var i=0; i<Json.ID_0x0354; i++)\n"
 				"{\n"
 					"eval('$(\\'ID_0x0'+(0x840+i).toString(16).toUpperCase()+'\\').value=Json.ID_0x0'+(0x840+i).toString(16).toUpperCase()+';');\n"
 				"}\n"
 			"}\n"
 		"};\n"
 		"xmlhttp.open('GET',URL,true);\n"
 		"xmlhttp.send();					\n"
 	"}\n"
 	"\n"
 	"function Init()\n"
 	"{\n"
 			//参数=0读所有设备地址
 		"var URL='s_027.html?0x0000&time='+Math.random();\n"
 		"var xmlhttp;\n"
 		"if(window.XMLHttpRequest)xmlhttp=new XMLHttpRequest();\n"
 		"else xmlhttp=new ActiveXObject('Microsoft.XMLHTTP');\n"
 		"xmlhttp.onreadystatechange=function()\n"
 		"{\n"
 			"if(xmlhttp.readyState==4 && xmlhttp.status==200)\n"
 			"{\n"
 				"eval('Json='+xmlhttp.responseText);\n"
"\n"
 				"if(Json.ID_0x0301==0x55)language=0;else language=1;\n"
 				"lang_init_ex(4250,4309);\n"
"\n"
 					//设备选择
 				"choose_bms();\n"
"\n"
 				"if(Json.ID_0x0282==0x11)\n"
 				"{\n"
 					"$('trans').style.zIndex=-9999;\n"
 					"for(var i=0; i<60; i++) eval('$(\\'ID_0x0'+(0x840+i).toString(16).toUpperCase()+'\\').disabled=\\'true\\';');\n"
 					"for(var i=0; i<60; i++) eval('$(\\'N'+(4250+i)+'\\').disabled=true;');\n"
 				"}\n"
 				"else\n"
 				"{\n"
 					"$('trans').style.zIndex=-9999;\n"
 				"}					\n"
 			"}\n"
 		"};\n"
 		"xmlhttp.open('GET',URL,true);\n"
 		"xmlhttp.send();\n"
 	"}\n"
 	 "function read_back(addr)\n"
 	"{ \n"
 		"var url='s_027.html?';\n"
"\n"
 		"url=url+'0xFFFF'+'&0x0'+addr.toString(16).toUpperCase();\n"
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
 				"eval('Json='+xmlhttp.responseText);\n"
 	            	//回读BMS modbus地址
 	            "if(addr >= 0x840 && addr <= 0x87B)\n"
 	            "{\n"
 					"var pr,fe;\n"
 					"eval('pr=$(\\'ID_0x0'+addr.toString(16).toUpperCase()+'\\').value;');\n"
 					"eval('fe=Json.ID_0x0'+addr.toString(16).toUpperCase()+';');\n"
 					"if(pr!=fe) eval('$(\\'ID_0x0'+addr.toString(16).toUpperCase()+'\\').value=Json.ID_0x0'+addr.toString(16).toUpperCase()+';');\n"
 		     		"show_tip_ex(3, '&nbsp;');\n"
 		    		"if(pr==fe){setTimeout('show_tip_ex(3,lang.N542[language])',500);}\n"
 		    		"else {setTimeout('show_tip_ex(3,lang.N541[language])',500);}\n"
 			           "setTimeout('show_tip_ex(3, \\'&nbsp;\\')',2500);               		\n"
 	            "}\n"
 			"}\n"
 		"};\n"
 		"xmlhttp.open('GET',url,true);\n"
 		"xmlhttp.send();     \n"
        "} 	\n"
"\n"
 	"function doSubmit(obj)\n"
 	"{\n"
            "var URL='a_027.html?';\n"
"\n"
            "var readBack=0;\n"
            "switch(obj.id)\n"
            "{\n"
             	//serial bms设置
             "case 'N4250': case 'N4251': case 'N4252': case 'N4253': case 'N4254': case 'N4255': case 'N4256': case 'N4257': case 'N4258': case 'N4259':\n"
             "case 'N4260': case 'N4261': case 'N4262': case 'N4263': case 'N4264': case 'N4265': case 'N4266': case 'N4267': case 'N4268': case 'N4269':\n"
             "case 'N4270': case 'N4271': case 'N4272': case 'N4273': case 'N4274': case 'N4275': case 'N4276': case 'N4277': case 'N4278': case 'N4279':\n"
             "case 'N4280': case 'N4281': case 'N4282': case 'N4283': case 'N4284': case 'N4285': case 'N4286': case 'N4287': case 'N4288': case 'N4289':\n"
             "case 'N4290': case 'N4291': case 'N4292': case 'N4293': case 'N4294': case 'N4295': case 'N4296': case 'N4297': case 'N4298': case 'N4299':\n"
             "case 'N4300': case 'N4301': case 'N4302': case 'N4303': case 'N4304': case 'N4305': case 'N4306': case 'N4307': case 'N4308': case 'N4309':\n"
             "var str = obj.id;\n"
            	"var cutstr = str.substring(str.length-4,str.length);\n"
            	"var index = parseInt(cutstr)-4250;\n"
             "if(flg_bms[index]){return;}\n"
             "eval('URL=URL+\\'&0x0'+(0x840+index).toString(16).toUpperCase()+'=\\'+$(\\'ID_0x0'+(0x840+index).toString(16).toUpperCase()+'\\').value;');\n"
             "eval('readBack=0x0'+(0x840+index).toString(16).toUpperCase()+';');\n"
             "break;   	\n"
             "default:break;\n"
            "}\n"
"\n"
            "URL=URL+'&time='+Math.random();\n"
             //alert(URL);          
"\n"
            "function success()\n"
            "{\n"
             "if(readBack >= 0x840 && readBack <= 0x87B)\n"
             "{\n"
 	     		"show_tip_ex(3, '&nbsp;');\n"
 	    		"setTimeout('read_back('+readBack+')', 100);\n"
 		           "setTimeout('show_tip_ex(3, \\'&nbsp;\\')',2500);               		\n"
             "}			\n"
            "} \n"
"\n"
             //测试
             //success();  
"\n"
 		"sendHttpReq(URL, success);\n"
 	"}\n"
"\n"
 	"function show_tip_ex(id, obj){eval('$(\\'tip'+id+'\\').innerHTML=obj;');}\n"
 	"function doRangeCheck(obj,reg)\n"
 	"{\n"
 		"var range=parseInt(obj.value);\n"
 		"var err, err2;			\n"
"\n"
 		"if(!obj.value.match(reg)){err=2;}\n"
 		"else if(range<1||range>247){err=1;}\n"
 		"else{err=0;}\n"
 "\n"
 			//bms	
 		"switch(obj.id)\n"
 		"{\n"
 			"case \"ID_0x0840\": case \"ID_0x0841\": case \"ID_0x0842\": case \"ID_0x0843\": case \"ID_0x0844\": case \"ID_0x0845\": case \"ID_0x0846\": case \"ID_0x0847\": case \"ID_0x0848\": case \"ID_0x0849\": \n"
 			"case \"ID_0x084A\": case \"ID_0x084B\": case \"ID_0x084C\": case \"ID_0x084D\": case \"ID_0x084E\": case \"ID_0x084F\": case \"ID_0x0850\": case \"ID_0x0851\": case \"ID_0x0852\": case \"ID_0x0853\": \n"
 			"case \"ID_0x0854\": case \"ID_0x0855\": case \"ID_0x0856\": case \"ID_0x0857\": case \"ID_0x0858\": case \"ID_0x0859\": case \"ID_0x085A\": case \"ID_0x085B\": case \"ID_0x085C\": case \"ID_0x085D\": \n"
 			"case \"ID_0x085E\": case \"ID_0x085F\": case \"ID_0x0860\": case \"ID_0x0861\": case \"ID_0x0862\": case \"ID_0x0863\": case \"ID_0x0864\": case \"ID_0x0865\": case \"ID_0x0866\": case \"ID_0x0867\": \n"
 			"case \"ID_0x0868\": case \"ID_0x0869\": case \"ID_0x086A\": case \"ID_0x086B\": case \"ID_0x086C\": case \"ID_0x086D\": case \"ID_0x086E\": case \"ID_0x086F\": case \"ID_0x0870\": case \"ID_0x0871\":\n"
 			"case \"ID_0x0872\": case \"ID_0x0873\": case \"ID_0x0874\": case \"ID_0x0875\": case \"ID_0x0876\": case \"ID_0x0877\": case \"ID_0x0878\": case \"ID_0x0879\": case \"ID_0x087A\": case \"ID_0x087B\":\n"
 			"if(err==1||err==2)\n"
 			"{\n"
 				"for(var i=0; i<Json.ID_0x0354; i++)\n"
 				"{\n"
 					"if(eval('$(\\'ID_0x0'+(0x840+i).toString(16).toUpperCase()+'\\').id')==obj.id) {flg_bms[i]=1;eval('$(\\'N'+(4250+i)+'\\').disabled=true;');break;}\n"
 				"}			\n"
 			"}\n"
 			"if(err==1)show_tip_ex(3,lang.N544[language]);\n"
 			"else if(err==2)show_tip_ex(3,lang.N543[language]);\n"
 			"else{\n"
 				"for(var i=0; i<Json.ID_0x0354; i++)\n"
 				"{\n"
 					"if(eval('$(\\'ID_0x0'+(0x840+i).toString(16).toUpperCase()+'\\').id')==obj.id) {flg_bms[i]=0;eval('$(\\'N'+(4250+i)+'\\').disabled=false;');break;}\n"
 				"}						\n"
 				"var a=0;\n"
 				"for(var i=0; i<60; i++) a|=flg_bms[i];\n"
 				"if(a==0)show_tip_ex(3,\"&nbsp;\");						\n"
 			"}\n"
 			"break;\n"
 		"}\n"
 	"}\n"
 "</script>\n"
"</html>\n"
};
#endif
