#ifndef _HTML_H_027_H_
#define _HTML_H_027_H_
static const char h_027[] = {
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
 	 	".color{background-color:#F58E03;}\n"
 	 	".color2{background-color:#aaa;}\n"
            ".Ftab{width:100%;height:22px;line-height:26px;padding-left:20px;color:#EEE;text-align:center;}\n"
            ".Ftab span{margin:0;padding:0;width:90%;float:left;width:14%;height:22px;line-height:22px;font-size:18px;color:#EEE;}\n"
            ".Ftab .select{background: #007B3E;}\n"
            ".Frame{width:100%; height:680px;}\n"
 	"</style>	\n"
 	"<script type=\"text/javascript\" src=\"j_001.js\"></script>\n"
 "</head>\n"
 "<body onload='Init()'>\n"
 	"<script type='text/javascript'>\n"
 		"show_T('N01');\n"
 	"</script>\n"
 	"<div class='title' style='width:90%;'>\n"
 	"<div class='r1 color2'></div><div class='r2 color2'></div><div class='r3 color2'></div><div class='r4 color2'></div>\n"
        "<div class='Ftab color2' id=\"tabFunc\">\n"
            "<span class='select' style='width:13%;' id='N02'></span><span style='width:13%;' id='N03'></span><span style='width:13%;' id='N04'></span><span style='width:13%;' id='N05'></span><span style='width:13%;' id='N06'></span><span style='width:13%;' id='N07'></span><span style='width:20%;' id='N08'></span>\n"
        "</div>	\n"
        "<div class='r4 color2'></div><div class='r3 color2'></div><div class='r2 color2'></div><div class='r1 color2'></div>\n"
     "</div>\n"
     "<div class='blank'></div>\n"
        "<iframe class='Frame' frameborder=\"0\" id=\"frame\"></iframe>	\n"
 	"<div class='btm_blank'></div>\n"
 	"<!--[if IE 6]><script type='text/javascript'>document.execCommand('BackgroundImageCache',false,true);</script><![endif]-->\n"
 "</body>\n"
 "<script type='text/javascript'>\n"
 	"var language=0;\n"
 		//������+0
 	"var reg=/^(0|[1-9]\\d*)$/;\n"
 	"var Json;\n"
 	"var pcs_mode=0, pv_mode=0;\n"
 	"var lang=\n"
 	"{\n"
 		"N01:['���豸ͨѶ����','Salve Modbus Setting'],\n"
 		"N02:['���������','PCS'],\n"
 		"N03:['��������','PV'],\n"
 		"N04:['���������','DCDC'],\n"
 		"N05:['���','BMS'],\n"
 		"N06:['���','Ammeter'],\n"
 		"N07:['������','PVS'],\n"
 		"N08:['������','Env-Detector']\n"
 	"};\n"
 		//h_048.html -- pcs net
 		//h_049.html -- pv net
 		//h_050.html -- pcs uart
 		//h_051.html -- pv uart
 		//h_052.html -- dcdc
 		//h_053.html -- bms
 		//h_054.html -- ammeter
 		//h_055.html -- pvs
 		//h_056.html -- env
        "var tab_index=0;\n"
        "var btns = document.getElementById('tabFunc').getElementsByTagName('span');\n"
        "var frame = document.getElementById('frame');\n"
        "for (var i = 0; i < btns.length; i++) {\n"
            "btns[i].index = i;\n"
            "btns[i].onclick = function(){\n"
                "tab_index = this.index;\n"
                "Switch_Tab(tab_index, pcs_mode, pv_mode);\n"
            "}\n"
        "};\n"
"\n"
        "function Switch_Tab(index, pcs_mode, pv_mode)\n"
        "{\n"
            "for (var i = 0; i < btns.length; i++) { btns[i].className = ''; };\n"
            "btns[index].className = 'select';\n"
            "if(tab_index <= 6){\n"
             "if(tab_index == 0)\n"
             "{\n"
             	"if(pcs_mode == 0){\n"
             			//pcs ����
             		"frame.src = 'h_050.html';   \n"
             	"}else{\n"
             			//pcs ����
             		"frame.src = 'h_048.html'; \n"
             	"}\n"
             "}\n"
             "else if(tab_index == 1)\n"
             "{\n"
             	"if(pv_mode == 0){\n"
             			//pv ����
             		"frame.src = 'h_051.html';   \n"
             	"}else{\n"
             			//pv ����
             		"frame.src = 'h_049.html'; \n"
             	"}\n"
             "}\n"
             "else\n"
             "{\n"
             	"frame.src = 'h_0'+(btns[tab_index].index+50)+'.html';\n"
             "}               \n"
            "}\n"
        "}\n"
    "</script>\n"
 "<script language='javascript' type='text/javascript'>\n"
"\n"
 	"function Init()\n"
 	"{\n"
 			//����=0�������豸��ַ
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
 				"lang_init_ex(1,8);\n"
 				"pcs_mode = Json.ID_0x038D;\n"
 				"pv_mode = Json.ID_0x0387;\n"
 					//�豸ѡ��
 				"Switch_Tab(tab_index, pcs_mode, pv_mode);\n"
"\n"
 				"if(Json.ID_0x0390==1){$('N02').style.display='block';}else{$('N02').style.display='none';}\n"
 				"if(Json.ID_0x0391==1){$('N03').style.display='block';}else{$('N03').style.display='none';}\n"
 				"if(Json.ID_0x0392==1){$('N04').style.display='block';}else{$('N04').style.display='none';}\n"
 				"if(Json.ID_0x0394==1){$('N05').style.display='block';}else{$('N05').style.display='none';}\n"
 				"if(Json.ID_0x0395==1){$('N06').style.display='block';}else{$('N06').style.display='none';}\n"
 				"if(Json.ID_0x0393==1){$('N07').style.display='block';}else{$('N07').style.display='none';}\n"
 				"if(Json.ID_0x039A==1){$('N08').style.display='block';}else{$('N08').style.display='none';}				\n"
 			"}\n"
 		"};\n"
 		"xmlhttp.open('GET',URL,true);\n"
 		"xmlhttp.send();\n"
 	"}\n"
 "</script>\n"
"</html>\n"
};
#endif