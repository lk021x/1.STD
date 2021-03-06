#ifndef _HTML_H_052_H_
#define _HTML_H_052_H_
static const char h_052[] = {
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
 		".PagerView{font-size:12px;font-family:tahoma,arial;color:#333;text-align:center;padding:8px;line-height:100%;}\n"
 	 	".PagerView span{color:#999;margin:0 1px;padding:3px 6px;border:1px solid #ccc;}\n"
 	 	".PagerView span.on{color:#fff;font-weight:bold;border:1px solid #666;background:#39f;}\n"
 	 	".PagerView a{color:#00f;text-decoration:none;}\n"
 	 	".PagerView a span{color:#33f;border:1px solid #66c;}\n"
 	 	".pager{width:780px;margin:0 auto;}\n"
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
 		"}	\n"
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
 			"this.maxButtons=10;\n"
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
 	"<script type='text/javascript'>\n"
 		"var language=0;\n"
 			//正整数+0
 		"var reg=/^(0|[1-9]\\d*)$/;\n"
 		"var Json;\n"
 		"var flg_dcdc = [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0];\n"
 		"var lang=\n"
 		"{\n"
 				//DCDC, bnt ID 从200开始
 			"N3200:['设&nbsp;&nbsp;置','Set'],\n"
 			"N3201:['设&nbsp;&nbsp;置','Set'],\n"
 			"N3202:['设&nbsp;&nbsp;置','Set'],\n"
 			"N3203:['设&nbsp;&nbsp;置','Set'],\n"
 			"N3204:['设&nbsp;&nbsp;置','Set'],\n"
 			"N3205:['设&nbsp;&nbsp;置','Set'],\n"
 			"N3206:['设&nbsp;&nbsp;置','Set'],\n"
 			"N3207:['设&nbsp;&nbsp;置','Set'],\n"
 			"N3208:['设&nbsp;&nbsp;置','Set'],\n"
 			"N3209:['设&nbsp;&nbsp;置','Set'],\n"
 			"N3210:['设&nbsp;&nbsp;置','Set'],\n"
 			"N3211:['设&nbsp;&nbsp;置','Set'],\n"
 			"N3211:['设&nbsp;&nbsp;置','Set'],\n"
 			"N3212:['设&nbsp;&nbsp;置','Set'],\n"
 			"N3213:['设&nbsp;&nbsp;置','Set'],\n"
 			"N3214:['设&nbsp;&nbsp;置','Set'],\n"
 			"N3215:['设&nbsp;&nbsp;置','Set'],\n"
 			"N3216:['设&nbsp;&nbsp;置','Set'],\n"
 			"N3217:['设&nbsp;&nbsp;置','Set'],\n"
 			"N3218:['设&nbsp;&nbsp;置','Set'],\n"
 			"N3219:['设&nbsp;&nbsp;置','Set'],\n"
 			"N3220:['设&nbsp;&nbsp;置','Set'],\n"
 			"N3221:['设&nbsp;&nbsp;置','Set'],\n"
 			"N3222:['设&nbsp;&nbsp;置','Set'],\n"
 			"N3223:['设&nbsp;&nbsp;置','Set'],\n"
 			"N3224:['设&nbsp;&nbsp;置','Set'],\n"
 			"N3225:['设&nbsp;&nbsp;置','Set'],\n"
 			"N3226:['设&nbsp;&nbsp;置','Set'],\n"
 			"N3227:['设&nbsp;&nbsp;置','Set'],\n"
 			"N3228:['设&nbsp;&nbsp;置','Set'],\n"
 			"N3229:['设&nbsp;&nbsp;置','Set'],\n"
 			"N3230:['设&nbsp;&nbsp;置','Set'],\n"
 			"N3231:['设&nbsp;&nbsp;置','Set'],\n"
 			"N3232:['设&nbsp;&nbsp;置','Set'],\n"
 			"N3233:['设&nbsp;&nbsp;置','Set'],\n"
 			"N3234:['设&nbsp;&nbsp;置','Set'],\n"
 			"N3235:['设&nbsp;&nbsp;置','Set'],\n"
 			"N3236:['设&nbsp;&nbsp;置','Set'],\n"
 			"N3237:['设&nbsp;&nbsp;置','Set'],\n"
 			"N3238:['设&nbsp;&nbsp;置','Set'],\n"
 			"N3239:['设&nbsp;&nbsp;置','Set'],\n"
 			"N3240:['设&nbsp;&nbsp;置','Set'],\n"
 			"N3241:['设&nbsp;&nbsp;置','Set'],\n"
 			"N3242:['设&nbsp;&nbsp;置','Set'],\n"
 			"N3243:['设&nbsp;&nbsp;置','Set'],\n"
 			"N3244:['设&nbsp;&nbsp;置','Set'],\n"
 			"N3245:['设&nbsp;&nbsp;置','Set'],\n"
 			"N3246:['设&nbsp;&nbsp;置','Set'],\n"
 			"N3247:['设&nbsp;&nbsp;置','Set'],\n"
 			"N3248:['设&nbsp;&nbsp;置','Set'],\n"
 			"N3249:['设&nbsp;&nbsp;置','Set'],\n"
 			"N3250:['设&nbsp;&nbsp;置','Set'],\n"
 			"N3251:['设&nbsp;&nbsp;置','Set'],\n"
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
 "<body onload='Init()'>	\n"
 	"<!-- DCDC -->\n"
 	"<table id='tbl_dcdc' width='90%' border='1' bordercolor=\"#E0E0E0\" style='display:none;'>\n"
 		"<script type='text/javascript'>\n"
 		"show_Set(0x6F0, 3200, 3, 52);\n"
 		"lang_init_ex(3200,3251);\n"
 		"</script>\n"
 	"</table>\n"
 	"<div class='tip' id='tip2' style='width:90%;'><span>&nbsp;</span></div>	\n"
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
 		//dcdc
 	"function choose_dcdc()\n"
 	"{\n"
 		"var lin=0;\n"
 		"if(Json.ID_0x0392==1){ $('tbl_dcdc').style.display='table'; $('tip2').style.display='block';} else { $('tbl_dcdc').style.display='none';  $('tip2').style.display='none';}\n"
 			//dcdc
 		"for(var i=0; i<52; i++){\n"
 			"if(i>=Json.ID_0x0352){\n"
 				"eval('$(\\'ID_0x0'+(0x6F0+i).toString(16).toUpperCase()+'\\').value=\\'\\';');\n"
 				"eval('$(\\'ID_0x0'+(0x6F0+i).toString(16).toUpperCase()+'\\').disabled=\\'true\\';');\n"
 				"eval('$(\\'N'+(3200+i)+'\\').disabled=true;');\n"
 				"eval('$(\\'N'+(3200+i)+'\\').style.background=\"#F0F0F0\";');\n"
 				"eval('$(\\'text'+(3200+i)+'\\').innerHTML=\\'No.--\\';');\n"
 			"}else{\n"
 				"if(Json.ID_0x0282==0x22){\n"
 					"eval('$(\\'N'+(3200+i)+'\\').disabled=false;');\n"
 					"eval('$(\\'N'+(3200+i)+'\\').style.background=\"#99D9EA\";');\n"
 				"}else{\n"
 					"eval('$(\\'N'+(3200+i)+'\\').style.background=\"#F0F0F0\";');\n"
 				"} \n"
 			"}\n"
 		"}\n"
 		"if(Json.ID_0x0352%4==0){lin=parseInt(Json.ID_0x0352/4);}\n"
 		"else {lin=parseInt(Json.ID_0x0352/4)+1;}\n"
 		"for(var j=0; j<13; j++)\n"
 		"{\n"
 			"if(j>=lin) eval('$(\\'t'+(3200+j)+'\\').style.display=\\'none\\';');\n"
 			"else eval('$(\\'t'+(3200+j)+'\\').style.display=\\'table\\';');\n"
 		"}\n"
 		"Set_DCDC_For_Serial();	\n"
 	"}\n"
 "\n"
 		//设置串口DCDC数据
 	"function Set_DCDC_For_Serial()\n"
 	"{\n"
 		"var URL='s_027.html?0x06F0&time='+Math.random();\n"
 		"var xmlhttp;\n"
 		"if(window.XMLHttpRequest)xmlhttp=new XMLHttpRequest();\n"
 		"else xmlhttp=new ActiveXObject('Microsoft.XMLHTTP');\n"
 		"xmlhttp.onreadystatechange=function()\n"
 		"{\n"
 			"if(xmlhttp.readyState==4 && xmlhttp.status==200)\n"
 			"{\n"
 				"eval('Json='+xmlhttp.responseText);			\n"
 				"for(var i=0; i<Json.ID_0x0352; i++)\n"
 				"{\n"
 					"eval('$(\\'ID_0x0'+(0x6F0+i).toString(16).toUpperCase()+'\\').value=Json.ID_0x0'+(0x6F0+i).toString(16).toUpperCase()+';');\n"
 				"}\n"
 			"}\n"
 		"};\n"
 		"xmlhttp.open('GET',URL,true);\n"
 		"xmlhttp.send();		\n"
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
 				"lang_init_ex(3200,3251);\n"
"\n"
 					//设备选择
 				"choose_dcdc();\n"
"\n"
 				"if(Json.ID_0x0282==0x11)\n"
 				"{\n"
 					"$('trans').style.zIndex=-9999;\n"
 					"for(var i=0; i<52; i++) eval('$(\\'ID_0x0'+(0x6F0+i).toString(16).toUpperCase()+'\\').disabled=\\'true\\';');\n"
 					"for(var i=0; i<52; i++) eval('$(\\'N'+(3200+i)+'\\').disabled=true;');\n"
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
 	            	//回读DCDC modbus地址
 	               "if(addr >= 0x6F0 && addr <= 0x721)\n"
 	            "{\n"
 					"var pr,fe;\n"
 					"eval('pr=$(\\'ID_0x0'+addr.toString(16).toUpperCase()+'\\').value;');\n"
 					"eval('fe=Json.ID_0x0'+addr.toString(16).toUpperCase()+';');\n"
 					"if(pr!=fe) eval('$(\\'ID_0x0'+addr.toString(16).toUpperCase()+'\\').value=Json.ID_0x0'+addr.toString(16).toUpperCase()+';');	\n"
 		     		"show_tip_ex(2, '&nbsp;');\n"
 		    		"if(pr==fe){setTimeout('show_tip_ex(2,lang.N542[language])',500);}\n"
 		    		"else {setTimeout('show_tip_ex(2,lang.N541[language])',500);}\n"
 			           "setTimeout('show_tip_ex(2, \\'&nbsp;\\')',2500);               		\n"
 	            "}		\n"
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
             	//serial dcdc设置
             "case 'N3200': case 'N3201': case 'N3202': case 'N3203': case 'N3204': case 'N3205': case 'N3206': case 'N3207': case 'N3208': case 'N3209': \n"
             "case 'N3210': case 'N3211': case 'N3212': case 'N3213': case 'N3214': case 'N3215': case 'N3216': case 'N3217': case 'N3218': case 'N3219':\n"
             "case 'N3220': case 'N3221': case 'N3222': case 'N3223': case 'N3224': case 'N3225': case 'N3226': case 'N3227': case 'N3228': case 'N3229':\n"
             "case 'N3230': case 'N3231': case 'N3232': case 'N3233': case 'N3234': case 'N3235': case 'N3236': case 'N3237': case 'N3238': case 'N3239':\n"
             "case 'N3240': case 'N3241': case 'N3242': case 'N3243': case 'N3244': case 'N3245': case 'N3246': case 'N3247': case 'N3248': case 'N3249':\n"
             "var str = obj.id;\n"
            	"var cutstr = str.substring(str.length-4,str.length);\n"
            	"var index = parseInt(cutstr)-3200;\n"
             "if(flg_dcdc[index]){return;}\n"
             "eval('URL=URL+\\'&0x0'+(0x6F0+index).toString(16).toUpperCase()+'=\\'+$(\\'ID_0x0'+(0x6F0+index).toString(16).toUpperCase()+'\\').value;');\n"
             "eval('readBack=0x0'+(0x6F0+index).toString(16).toUpperCase()+';');\n"
             "break;\n"
             "default:break;\n"
            "}\n"
"\n"
            "URL=URL+'&time='+Math.random();\n"
             //alert(URL);          
"\n"
            "function success()\n"
            "{\n"
             "if(readBack >= 0x6F0 && readBack <= 0x721)\n"
             "{\n"
 	     		"show_tip_ex(2, '&nbsp;');\n"
 	    		"setTimeout('read_back('+readBack+')', 100);\n"
 		           "setTimeout('show_tip_ex(2, \\'&nbsp;\\')',2500);               		\n"
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
 			//dcdc	
 		"switch(obj.id)\n"
 		"{\n"
 			"case \"ID_0x06F0\": case \"ID_0x06F1\": case \"ID_0x06F2\": case \"ID_0x06F3\": case \"ID_0x06F4\": case \"ID_0x06F5\":\n"
 			"case \"ID_0x06F6\": case \"ID_0x06F7\": case \"ID_0x06F8\": case \"ID_0x06F9\": case \"ID_0x06FA\": case \"ID_0x06FB\": case \"ID_0x06FC\": case \"ID_0x06FD\": case \"ID_0x06FE\": case \"ID_0x06FF\": \n"
 			"case \"ID_0x0700\": case \"ID_0x0701\": case \"ID_0x0702\": case \"ID_0x0703\": case \"ID_0x0704\": case \"ID_0x0705\":\n"
 			"case \"ID_0x0706\": case \"ID_0x0707\": case \"ID_0x0708\": case \"ID_0x0709\": case \"ID_0x070A\": case \"ID_0x070B\": case \"ID_0x070C\": case \"ID_0x070D\": case \"ID_0x070E\": case \"ID_0x070F\": \n"
 			"case \"ID_0x0710\": case \"ID_0x0711\": case \"ID_0x0712\": case \"ID_0x0713\": case \"ID_0x0714\": case \"ID_0x0715\":\n"
 			"case \"ID_0x0716\": case \"ID_0x0717\": case \"ID_0x0718\": case \"ID_0x0719\": case \"ID_0x071A\": case \"ID_0x071B\": case \"ID_0x071C\": case \"ID_0x071D\": case \"ID_0x071E\": case \"ID_0x071F\": \n"
 			"case \"ID_0x0720\": case \"ID_0x0721\":\n"
 			"if(err==1||err==2)\n"
 			"{\n"
 				"for(var i=0; i<Json.ID_0x0352; i++)\n"
 				"{\n"
 					"if(eval('$(\\'ID_0x0'+(0x6F0+i).toString(16).toUpperCase()+'\\').id')==obj.id) {flg_dcdc[i]=1;eval('$(\\'N'+(3200+i)+'\\').disabled=true;');break;}\n"
 				"}					\n"
 			"}\n"
 			"if(err==1)show_tip_ex(2,lang.N544[language]);\n"
 			"else if(err==2)show_tip_ex(2,lang.N543[language]);\n"
 			"else\n"
 			"{\n"
 				"for(var i=0; i<Json.ID_0x0352; i++)\n"
 				"{\n"
 					"if(eval('$(\\'ID_0x0'+(0x6F0+i).toString(16).toUpperCase()+'\\').id')==obj.id) {flg_dcdc[i]=0;eval('$(\\'N'+(3200+i)+'\\').disabled=false;');break;}\n"
 				"}						\n"
 				"var a=0;\n"
 				"for(var i=0; i<50; i++) a|=flg_dcdc[i];\n"
 				"if(a==0)show_tip_ex(2,\"&nbsp;\");						\n"
 			"}\n"
 			"break;\n"
 		"}\n"
 	"}\n"
 "</script>\n"
"</html>\n"
};
#endif
