#ifndef _HTML_H_001_H_
#define _HTML_H_001_H_
static const char h_001[] = {
"HTTP/1.1 200 OK\r\nContent-Type:text/html\r\nAccept-Ranges:bytes\r\n\r\n"
"<!DOCTYPE html PUBLIC '-//W3C//DTD XHTML 1.0 Transitional//EN' 'http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd'>\n"
"<html xmlns='http://www.w3.org/1999/xhtml'>\n"
"<head>\n"
        "<meta http-equiv='Content-Type' content='text/html; charset=GB2312'/>\n"
        "<title></title>\n"
        "<script type='text/javascript'>\n"
            "window.history.forward(1);\n"
            "var Json;\n"
            "setInterval('feed()',5000);\n"
            "function Init()\n"
            "{\n"
                "var URL='s_001.html?'+'time='+Math.random();\n"
                "var xmlhttp;\n"
                "if(window.XMLHttpRequest)xmlhttp=new XMLHttpRequest();\n"
                "else xmlhttp=new ActiveXObject('Microsoft.XMLHTTP');\n"
                "xmlhttp.onreadystatechange=function()\n"
                "{\n"
                    "if(xmlhttp.readyState==4&&xmlhttp.status==200)\n"
                    "{\n"
                        "eval('Json='+xmlhttp.responseText);\n"
                         //Json={ID_0x0202:'EMS200'};
                        "document.title=Json.ID_0x0202;\n"
                    "}\n"
                "};\n"
                "xmlhttp.open('GET',URL,true);\n"
                "xmlhttp.send();\n"
            "}\n"
            "function feed()\n"
            "{\n"
                "var URL='Feed.html?'+'time='+Math.random();\n"
                "var xmlhttp;\n"
                "if(window.XMLHttpRequest)xmlhttp=new XMLHttpRequest();\n"
                "else xmlhttp=new ActiveXObject('Microsoft.XMLHTTP');\n"
                "xmlhttp.onreadystatechange=function()\n"
                "{\n"
                    "if(xmlhttp.readyState==4&&xmlhttp.status==200)\n"
                    "{\n"
                        "if(xmlhttp.responseText=='1')\n"
                        "{\n"
                            "parent.document.location.href='Login.html';\n"
                        "}\n"
 				"}\n"
                "};\n"
                "xmlhttp.open('GET',URL,true);\n"
                "xmlhttp.send();\n"
            "}\n"
        "</script>\n"
    "</head>\n"
        "<frameset rows='89,*,20' cols='*' border='0' onload='Init()'>\n"
            "<frame noresize='noresize' src='h_012.html' name='logo' id='logo'>\n"
            "<frameset rows='*' cols='250,*' border='0'>\n"
                "<frame noresize='noresize' src='h_013.html' name='menu' id='menu'>\n"
                "<frame noresize='noresize' name='main' id='main'>\n"
            "</frameset>\n"
            "<frame noresize='noresize' src='h_041.html' name='stat' id='stat'>\n"
        "</frameset>\n"
        "<noframes>\n"
        "</noframes>)\n"
"</html>\n"
};
#endif
