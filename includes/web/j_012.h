#ifndef _J_012_H_
#define _J_012_H_

static const char j_012[]={
"HTTP/1.1 200 OK\r\nContent-Type:text/javascript\r\nAccept-Ranges:bytes\r\n\r\n"
"﻿// zDialog 2.0\n"
 // 最后修正：2009-12-18\n"
 // 窗体图片路径配置\n"
"var IMAGESPATH = ''; \n"
 // 一些公用方法和属性\n"
"var isIE = navigator.userAgent.indexOf('MSIE') != -1;\n"
"var isIE6 = navigator.userAgent.indexOf('MSIE 6.0') != -1;\n"
"var isIE8 = !!window.XDomainRequest && !!document.documentMode;\n"
"if(isIE)\n"
 "try{ document.execCommand('BackgroundImageCache',false,true); }catch(e){}\n"
"\n"
"var $id = function (id) {\n"
    "return typeof id == \"string\" ? document.getElementById(id) : id;\n"
"};\n"
 //if (!$) var $ = $id;\n"
"\n"
 //如果dust为ture，则返回被删除的元素\n"
"Array.prototype.remove = function (s, dust) { \n"
    "if (dust) {\n"
        "var dustArr = [];\n"
        "for (var i = 0; i < this.length; i++) {\n"
            "if (s == this[i]) {\n"
                "dustArr.push(this.splice(i, 1)[0]);\n"
            "}\n"
        "}\n"
        "return dustArr;\n"
    "}\n"
    "for (var i = 0; i < this.length; i++) {\n"
        "if (s == this[i]) {\n"
            "this.splice(i, 1);\n"
        "}\n"
    "}\n"
    "return this;\n"
"}\n"
"\n"
"var $topWindow = function () {\n"
    "var parentWin = window;\n"
    "while (parentWin != parentWin.parent) {\n"
        "if (parentWin.parent.document.getElementsByTagName(\"FRAMESET\").length > 0) break;\n"
        "parentWin = parentWin.parent;\n"
    "}\n"
    "return parentWin;\n"
"};\n"
"var $bodyDimensions = function (win) {\n"
    "win = win || window;\n"
    "var doc = win.document;\n"
    "var cw = doc.compatMode == \"BackCompat\" ? doc.body.clientWidth : doc.documentElement.clientWidth;\n"
    "var ch = doc.compatMode == \"BackCompat\" ? doc.body.clientHeight : doc.documentElement.clientHeight;\n"
    "var sl = Math.max(doc.documentElement.scrollLeft, doc.body.scrollLeft);\n"
     //考虑滚动的情况\n"
    "var st = Math.max(doc.documentElement.scrollTop, doc.body.scrollTop); \n"
    "var sw = Math.max(doc.documentElement.scrollWidth, doc.body.scrollWidth);\n"
     //考虑滚动的情况\n"
    "var sh = Math.max(doc.documentElement.scrollHeight, doc.body.scrollHeight); \n"
     //取scrollWidth和clientWidth中的最大值\n"
    "var w = Math.max(sw, cw); \n"
     //IE下在页面内容很少时存在scrollHeight<clientHeight的情况\n"
    "var h = Math.max(sh, ch); \n"
    "return {\n"
        "\"clientWidth\": cw,\n"
        "\"clientHeight\": ch,\n"
        "\"scrollLeft\": sl,\n"
        "\"scrollTop\": st,\n"
        "\"scrollWidth\": sw,\n"
        "\"scrollHeight\": sh,\n"
        "\"width\": w,\n"
        "\"height\": h\n"
    "}\n"
"};\n"
 //透明度渐变：start:开始透明度 0-100；end:结束透明度 0-100；speed:速度1-100\n"
"var fadeEffect = function(element, start, end, speed, callback){\n"
 "if(!element.effect)\n"
 	"element.effect = {fade:0, move:0, size:0};\n"
 "clearInterval(element.effect.fade);\n"
 "var speed=speed||20;\n"
 "element.effect.fade = setInterval(function(){\n"
 	"start = start < end ? Math.min(start + speed, end) : Math.max(start - speed, end);\n"
 	"element.style.opacity =  start / 100;\n"
 	"element.style.filter = \"alpha(opacity=\" + start + \")\";\n"
 	"if(start == end){\n"
 		"clearInterval(element.effect.fade);\n"
 		"if(callback)\n"
 			"callback.call(element);\n"
 	"}\n"
 "}, 20);\n"
"};\n"
"\n"
 // 弹出框类实现\n"
"var topWin = $topWindow();\n"
"var topDoc = topWin.document;\n"
"var Dialog = function () {\n"
     // 以下属性以大写开始，可以在调用show()方法前设置值\n"
    "this.ID = null;\n"
    "this.Width = null;\n"
    "this.Height = null;\n"
    "this.URL = null;\n"
 "this.OnLoad=null;\n"
    "this.InnerHtml = \"\"\n"
    "this.InvokeElementId = \"\"\n"
    "this.Top = \"50%\";\n"
    "this.Left = \"50%\";\n"
    "this.Title = \"\";\n"
     // 点击确定后调用的方法\n"
    "this.OKEvent = null; \n"
     // 点击取消及关闭后调用的方法\n"
    "this.CancelEvent = null; \n"
    "this.ShowButtonRow = false;\n"
    "this.MessageIcon = \"\";\n"
    "this.MessageTitle = \"\";\n"
    "this.Message = \"\";\n"
    "this.ShowMessageRow = false;\n"
    "this.Modal = true;\n"
    "this.Drag = true;\n"
    "this.AutoClose = null;\n"
    "this.ShowCloseButton = true;\n"
 "this.Animator = true;\n"
     // 以下属性以小写开始，不要自行改变\n"
    "this.dialogDiv = null;\n"
 "this.bgDiv=null;\n"
    "this.parentWindow = null;\n"
    "this.innerFrame = null;\n"
    "this.innerWin = null;\n"
    "this.innerDoc = null;\n"
    "this.zindex = 900;\n"
    "this.cancelButton = null;\n"
    "this.okButton = null;\n"
     //兼容旧写法\n"
    "if (arguments.length > 0 && typeof(arguments[0]) == \"string\") { \n"
        "this.ID = arguments[0];\n"
    "} else if (arguments.length > 0 && typeof(arguments[0]) == \"object\") {\n"
        "Dialog.setOptions(this, arguments[0])\n"
    "}\n"
 "if(!this.ID)\n"
        "this.ID = topWin.Dialog._Array.length + \"\";\n"
"\n"
"};\n"
"Dialog._Array = [];\n"
"Dialog.bgDiv = null;\n"
"Dialog.setOptions = function (obj, optionsObj) {\n"
    "if (!optionsObj) return;\n"
    "for (var optionName in optionsObj) {\n"
        "obj[optionName] = optionsObj[optionName];\n"
    "}\n"
"};\n"
"Dialog.attachBehaviors = function () {\n"
    "if (isIE) {\n"
        "document.attachEvent(\"onkeydown\", Dialog.onKeyDown);\n"
        "window.attachEvent('onresize', Dialog.resetPosition);\n"
    "} else {\n"
        "document.addEventListener(\"keydown\", Dialog.onKeyDown, false);\n"
        "window.addEventListener('resize', Dialog.resetPosition, false);\n"
    "}\n"
"};\n"
"Dialog.prototype.attachBehaviors = function () {\n"
     //注册拖拽方法\n"
    "if (this.Drag && topWin.Drag) topWin.Drag.init(topWin.$id(\"_Draghandle_\" + this.ID), topWin.$id(\"_DialogDiv_\" + this.ID)); \n"
     //非ie浏览器下在拖拽时用一个层遮住iframe，以免光标移入iframe失去拖拽响应\n"
    "if (!isIE && this.URL) { \n"
        "var self = this;\n"
        "topWin.$id(\"_DialogDiv_\" + this.ID).onDragStart = function () {\n"
            "topWin.$id(\"_Covering_\" + self.ID).style.display = \"\"\n"
        "}\n"
        "topWin.$id(\"_DialogDiv_\" + this.ID).onDragEnd = function () {\n"
            "topWin.$id(\"_Covering_\" + self.ID).style.display = \"none\"\n"
        "}\n"
    "}\n"
"};\n"
"Dialog.prototype.displacePath = function () {\n"
    "if (this.URL.substr(0, 7) == \"http://\" || this.URL.substr(0, 1) == \"/\" || this.URL.substr(0, 11) == \"javascript:\") {\n"
        "return this.URL;\n"
    "} else {\n"
        "var thisPath = this.URL;\n"
        "var locationPath = window.location.href;\n"
        "locationPath = locationPath.substring(0, locationPath.lastIndexOf('/'));\n"
        "while (thisPath.indexOf('../') >= 0) {\n"
            "thisPath = thisPath.substring(3);\n"
            "locationPath = locationPath.substring(0, locationPath.lastIndexOf('/'));\n"
        "}\n"
        "return locationPath + '/' + thisPath;\n"
    "}\n"
"};\n"
"Dialog.prototype.setPosition = function () {\n"
    "var bd = $bodyDimensions(topWin);\n"
    "var thisTop = this.Top,\n"
        "thisLeft = this.Left,\n"
 	"thisdialogDiv=this.getDialogDiv();\n"
    "if (typeof this.Top == \"string\" && this.Top.substring(this.Top.length - 1, this.Top.length) == \"%\") {\n"
        "var percentT = this.Top.substring(0, this.Top.length - 1) * 0.01;\n"
        "thisTop = bd.clientHeight * percentT - thisdialogDiv.scrollHeight * percentT + bd.scrollTop;\n"
    "}\n"
    "if (typeof this.Left == \"string\" && this.Left.substring(this.Left.length - 1, this.Left.length) == \"%\") {\n"
        "var percentL = this.Left.substring(0, this.Left.length - 1) * 0.01;\n"
        "thisLeft = bd.clientWidth * percentL - thisdialogDiv.scrollWidth * percentL + bd.scrollLeft;\n"
    "}\n"
    "thisdialogDiv.style.top = Math.round(thisTop) + \"px\";\n"
    "thisdialogDiv.style.left = Math.round(thisLeft) + \"px\";\n"
"};\n"
"Dialog.setBgDivSize = function () {\n"
    "var bd = $bodyDimensions(topWin);\n"
 "if(Dialog.bgDiv){\n"
 		"if(isIE6){\n"
 			"Dialog.bgDiv.style.height = bd.clientHeight + \"px\";\n"
 			"Dialog.bgDiv.style.top=bd.scrollTop + \"px\";\n"
                 //让div重渲染,修正IE6下尺寸bug\n"
 			"Dialog.bgDiv.childNodes[0].style.display = \"none\";\n"
 			"Dialog.bgDiv.childNodes[0].style.display = \"\";\n"
 		"}else{\n"
 			"Dialog.bgDiv.style.height = bd.scrollHeight + \"px\";\n"
 		"}\n"
 	"}\n"
"};\n"
"Dialog.resetPosition = function () {\n"
    "Dialog.setBgDivSize();\n"
    "for (var i = 0, len = topWin.Dialog._Array.length; i < len; i++) {\n"
        "topWin.Dialog._Array[i].setPosition();\n"
    "}\n"
"};\n"
"Dialog.prototype.create = function () {\n"
    "var bd = $bodyDimensions(topWin);\n"
    "if (typeof(this.OKEvent)== \"function\") this.ShowButtonRow = true;\n"
    "if (!this.Width) this.Width = Math.round(bd.clientWidth * 4 / 10);\n"
    "if (!this.Height) this.Height = Math.round(this.Width / 2);\n"
    "if (this.MessageTitle || this.Message) this.ShowMessageRow = true;\n"
    "var DialogDivWidth = this.Width + 13 + 13;\n"
    "var DialogDivHeight = this.Height + 33 + 13 + (this.ShowButtonRow ? 40 : 0) + (this.ShowMessageRow ? 50 : 0);\n"
"\n"
    "if (DialogDivWidth > bd.clientWidth) this.Width = Math.round(bd.clientWidth - 26);\n"
    "if (DialogDivHeight > bd.clientHeight) this.Height = Math.round(bd.clientHeight - 46 - (this.ShowButtonRow ? 40 : 0) - (this.ShowMessageRow ? 50 : 0));\n"
"\n"
    "var html = '\\\n"
  "<table id=\"_DialogTable_' + this.ID + '\" width=\"' + (this.Width + 26) + '\" cellspacing=\"0\" cellpadding=\"0\" border=\"0\" onselectstart=\"return false;\" style=\"-moz-user-select: none; font-size:12px; line-height:1.4;\">\\\n"
    "<tr id=\"_Draghandle_' + this.ID + '\" style=\"' + (this.Drag ? \"cursor: move;\" : \"\") + '\">\\\n"
      "<td width=\"13\" height=\"33\" style=\"background-image: url(' + IMAGESPATH + 'i_029.png) !important;background: url(' + IMAGESPATH + 'i_029.png) no-repeat 0 0;\"><div style=\"width: 13px;\"></div></td>\\\n"
      "<td height=\"33\" style=\"background-image:url(' + IMAGESPATH + 'i_027.png) !important;background: url(' + IMAGESPATH + 'i_027.png) repeat-x top;\"><div style=\"padding: 9px 0 0 4px; float: left; font-weight: bold; color:#fff;\"><img align=\"absmiddle\" src=\"' + IMAGESPATH + 'i_035.gif\"/><span id=\"_Title_' + this.ID + '\">' + this.Title + '</span></div>\\\n"
        "<div onclick=\"Dialog.getInstance(\\'' + this.ID + '\\').cancelButton.onclick.apply(Dialog.getInstance(\\'' + this.ID + '\\').cancelButton,[]);\" onmouseout=\"this.style.backgroundImage=\\'url(' + IMAGESPATH + 'i_025.gif)\\'\" onmouseover=\"this.style.backgroundImage=\\'url(' + IMAGESPATH + 'i_026.gif)\\'\" style=\"margin-top:5px; position: relative; cursor: pointer; float: right; height: 17px; width: 28px; background-image: url(' + IMAGESPATH + 'i_025.gif);' + (this.ShowCloseButton ? \"\" : \"display:none;\") + '\"></div></td>\\\n"
      "<td width=\"13\" height=\"33\" style=\"background-image: url(' + IMAGESPATH + 'i_033.png) !important;background: url(' + IMAGESPATH + 'i_033.png) no-repeat right 0;\"><div style=\"width: 13px;\"><a id=\"_forTab_' + this.ID + '\" href=\"#;\"></a></div></td>\\\n"
    "</tr>\\\n"
    "<tr valign=\"top\">\\\n"
      "<td width=\"13\" style=\"background-image: url(' + IMAGESPATH + 'i_030.png) !important;background: url(' + IMAGESPATH + 'i_030.png) repeat-y left;\"></td>\\\n"
      "<td align=\"center\"><table width=\"100%\" cellspacing=\"0\" cellpadding=\"0\" border=\"0\" bgcolor=\"#ffffff\">\\\n"
          "<tr id=\"_MessageRow_' + this.ID + '\" style=\"' + (this.ShowMessageRow ? \"\" : \"display:none\") + '\">\\\n"
            "<td valign=\"top\" height=\"50\"><table width=\"100%\" cellspacing=\"0\" cellpadding=\"0\" border=\"0\" style=\"background:#eaece9 url(' + IMAGESPATH + 'dialog_bg.jpg) no-repeat scroll right top;\" id=\"_MessageTable_' + this.ID + '\">\\\n"
                "<tr>\\\n"
                  "<td width=\"50\" height=\"50\" align=\"center\"><img width=\"32\" height=\"32\" src=\"' + IMAGESPATH + this.MessageIcon + '\" id=\"_MessageIcon_' + this.ID + '\"/></td>\\\n"
                  "<td align=\"left\" style=\"line-height: 16px;\"><div id=\"_MessageTitle_' + this.ID + '\" style=\"font-weight:bold\">' + this.MessageTitle + '</div>\\\n"
                    "<div id=\"_Message_' + this.ID + '\">' + this.Message + '</div></td>\\\n"
                "</tr>\\\n"
              "</table></td>\\\n"
          "</tr>\\\n"
          "<tr>\\\n"
            "<td valign=\"top\" align=\"center\"><div id=\"_Container_' + this.ID + '\" style=\"position: relative; width: ' + this.Width + 'px; height: ' + this.Height + 'px;\">\\\n"
                "<div style=\"position: absolute; height: 100%; width: 100%; display: none; background-color:#fff; opacity: 0.5;\" id=\"_Covering_' + this.ID + '\">&nbsp;</div>\\\n"
 "' + (function (obj) {\n"
        "if (obj.InnerHtml) return obj.InnerHtml;\n"
        "if (obj.URL) return '<iframe width=\"100%\" height=\"100%\" frameborder=\"0\" style=\"border:none 0;\" allowtransparency=\"true\" id=\"_DialogFrame_' + obj.ID + '\" src=\"' + obj.displacePath() + '\"></iframe>';\n"
        "return \"\";\n"
    "})(this) + '\\\n"
              "</div></td>\\\n"
          "</tr>\\\n"
          "<tr id=\"_ButtonRow_' + this.ID + '\" style=\"' + (this.ShowButtonRow ? \"\" : \"display:none\") + '\">\\\n"
            "<td height=\"36\"><div id=\"_DialogButtons_' + this.ID + '\" style=\"border-top: 1px solid #DADEE5; padding: 8px 20px; text-align: right; background-color:#f6f6f6;\">\\\n"
                "<input type=\"button\" class=\"buttonStyle\" value=\"确 定\" id=\"_ButtonOK_' + this.ID + '\"/>\\\n"
                "<input type=\"button\" class=\"buttonStyle\" value=\"取 消\" onclick=\"Dialog.getInstance(\\'' + this.ID + '\\').close();\" id=\"_ButtonCancel_' + this.ID + '\"/>\\\n"
              "</div></td>\\\n"
          "</tr>\\\n"
        "</table></td>\\\n"
      "<td width=\"13\" style=\"background-image: url(' + IMAGESPATH + 'i_031.png) !important;background: url(' + IMAGESPATH + 'i_031.png) repeat-y right;\"></td>\\\n"
    "</tr>\\\n"
    "<tr>\\\n"
      "<td width=\"13\" height=\"13\" style=\"background-image: url(' + IMAGESPATH + 'i_028.png) !important;background: url(' + IMAGESPATH + 'i_028.png) no-repeat 0 bottom;\"></td>\\\n"
      "<td style=\"background-image: url(' + IMAGESPATH + 'i_024.png) !important;background: url(' + IMAGESPATH + 'i_024.png) repeat-x bottom;\"></td>\\\n"
      "<td width=\"13\" height=\"13\" style=\"background-image: url(' + IMAGESPATH + 'i_032.png) !important;background: url(' + IMAGESPATH + 'i_032.png) no-repeat right bottom;\"><a onfocus=\\'$id(\"_forTab_' + this.ID + '\").focus();\\' href=\"#;\"></a></td>\\\n"
    "</tr>\\\n"
  "</table>\\\n"
"</div>\\\n"
"'\n"
    "var div = topWin.$id(\"_DialogDiv_\" + this.ID);\n"
    "if (!div) {\n"
        "div = topDoc.createElement(\"div\");\n"
        "div.id = \"_DialogDiv_\" + this.ID;\n"
        "topDoc.getElementsByTagName(\"BODY\")[0].appendChild(div);\n"
    "}\n"
    "div.style.position = \"absolute\";\n"
    "div.style.left = \"-9999px\";\n"
    "div.style.top = \"-9999px\";\n"
    "div.innerHTML = html;\n"
    "if (this.InvokeElementId) {\n"
        "var element = $id(this.InvokeElementId);\n"
        "element.style.position = \"\";\n"
        "element.style.display = \"\";\n"
        "if (isIE) {\n"
            "var fragment = topDoc.createElement(\"div\");\n"
            "fragment.innerHTML = element.outerHTML;\n"
            "element.outerHTML = \"\";\n"
            "topWin.$id(\"_Covering_\" + this.ID).parentNode.appendChild(fragment)\n"
        "} else {\n"
            "topWin.$id(\"_Covering_\" + this.ID).parentNode.appendChild(element)\n"
        "}\n"
    "}\n"
    "this.parentWindow = window;\n"
    "if (this.URL) {\n"
        "if (topWin.$id(\"_DialogFrame_\" + this.ID)) {\n"
            "this.innerFrame = topWin.$id(\"_DialogFrame_\" + this.ID);\n"
        "};\n"
        "var self = this;\n"
        "innerFrameOnload = function () {\n"
            "try {\n"
 			"self.innerWin = self.innerFrame.contentWindow;\n"
 			"self.innerWin.parentDialog = self;\n"
                "self.innerDoc = self.innerWin.document;\n"
                "if (!self.Title && self.innerDoc && self.innerDoc.title) {\n"
                    "if (self.innerDoc.title) topWin.$id(\"_Title_\" + self.ID).innerHTML = self.innerDoc.title;\n"
                "};\n"
            "} catch(e) {\n"
                "if (console && console.log) console.log(\"可能存在访问限制，不能获取到iframe中的对象。\")\n"
            "}\n"
            "if (typeof(self.OnLoad)== \"function\")self.OnLoad();\n"
        "};\n"
        "if (this.innerFrame.attachEvent) {\n"
            "this.innerFrame.attachEvent(\"onload\", innerFrameOnload);\n"
        "} else {\n"
            "this.innerFrame.onload = innerFrameOnload;\n"
        "};\n"
    "};\n"
    "topWin.$id(\"_DialogDiv_\" + this.ID).dialogId = this.ID;\n"
    "topWin.$id(\"_DialogDiv_\" + this.ID).dialogInstance = this;\n"
    "this.attachBehaviors();\n"
    "this.okButton = topWin.$id(\"_ButtonOK_\" + this.ID);\n"
    "this.cancelButton = topWin.$id(\"_ButtonCancel_\" + this.ID);\n"
 "div=null;\n"
"};\n"
"Dialog.prototype.setSize = function (w, h) {\n"
    "if (w && +w > 20) {\n"
        "this.Width = +w;\n"
        "topWin.$id(\"_DialogTable_\" + this.ID).width = this.Width + 26;\n"
        "topWin.$id(\"_Container_\" + this.ID).style.width = this.Width + \"px\";\n"
    "}\n"
    "if (h && +h > 10) {\n"
        "this.Height = +h;\n"
        "topWin.$id(\"_Container_\" + this.ID).style.height = this.Height + \"px\";\n"
    "}\n"
    "this.setPosition();\n"
"};\n"
"Dialog.prototype.show = function () {\n"
    "this.create();\n"
    "var bgdiv = Dialog.getBgdiv(),\n"
 	"thisdialogDiv=this.getDialogDiv();\n"
    "this.zindex = thisdialogDiv.style.zIndex = Dialog.bgDiv.style.zIndex + 1;\n"
    "if (topWin.Dialog._Array.length > 0) {\n"
        "this.zindex = thisdialogDiv.style.zIndex = topWin.Dialog._Array[topWin.Dialog._Array.length - 1].zindex + 2;\n"
    "} else {\n"
        "var topWinBody = topDoc.getElementsByTagName(topDoc.compatMode == \"BackCompat\" ? \"BODY\" : \"HTML\")[0];\n"
         //topWinBody.styleOverflow = topWinBody.style.overflow; 去除滚动条\n"
         //topWinBody.style.overflow = \"hidden\"; 去除滚动条\n"
        "bgdiv.style.display = \"none\";\n"
    "}\n"
    "topWin.Dialog._Array.push(this);\n"
    "if (this.Modal) {\n"
        "bgdiv.style.zIndex = topWin.Dialog._Array[topWin.Dialog._Array.length - 1].zindex - 1;\n"
        "Dialog.setBgDivSize();\n"
 	"if(bgdiv.style.display == \"none\"){\n"
 		"if(this.Animator){\n"
 			"var bgMask=topWin.$id(\"_DialogBGMask\");\n"
 			"bgMask.style.opacity = 0;\n"
 			"bgMask.style.filter = \"alpha(opacity=0)\";\n"
         	"bgdiv.style.display = \"\";\n"
 			"fadeEffect(bgMask,0,40,isIE6?20:10);\n"
 			"bgMask=null;\n"
 		"}else{\n"
         	"bgdiv.style.display = \"\";\n"
 		"}\n"
 	"}\n"
    "}\n"
    "this.setPosition();\n"
    "if (this.CancelEvent) {\n"
        "this.cancelButton.onclick = this.CancelEvent;\n"
        "if(this.ShowButtonRow)this.cancelButton.focus();\n"
    "}\n"
    "if (this.OKEvent) {\n"
        "this.okButton.onclick = this.OKEvent;\n"
        "if(this.ShowButtonRow)this.okButton.focus();\n"
    "}\n"
    "if (this.AutoClose && this.AutoClose > 0) this.autoClose();\n"
    "this.opened = true;\n"
 "bgdiv=null;\n"
"};\n"
"Dialog.prototype.close = function () {\n"
 "var thisdialogDiv=this.getDialogDiv();\n"
    "if (this == topWin.Dialog._Array[topWin.Dialog._Array.length - 1]) {\n"
        "var isTopDialog = topWin.Dialog._Array.pop();\n"
    "} else {\n"
        "topWin.Dialog._Array.remove(this)\n"
    "}\n"
    "if (this.InvokeElementId) {\n"
        "var innerElement = topWin.$id(this.InvokeElementId);\n"
        "innerElement.style.display = \"none\";\n"
        "if (isIE) {\n"
             //ie下不能跨窗口拷贝元素，只能跨窗口拷贝html代码\n"
            "var fragment = document.createElement(\"div\");\n"
            "fragment.innerHTML = innerElement.outerHTML;\n"
            "innerElement.outerHTML = \"\";\n"
            "document.getElementsByTagName(\"BODY\")[0].appendChild(fragment)\n"
        "} else {\n"
            "document.getElementsByTagName(\"BODY\")[0].appendChild(innerElement)\n"
        "}\n"
"\n"
    "}\n"
    "if (topWin.Dialog._Array.length > 0) {\n"
        "if (this.Modal && isTopDialog) Dialog.bgDiv.style.zIndex = topWin.Dialog._Array[topWin.Dialog._Array.length - 1].zindex - 1;\n"
    "} else {\n"
        "Dialog.bgDiv.style.zIndex = \"900\";\n"
        "Dialog.bgDiv.style.display = \"none\";\n"
        "var topWinBody = topDoc.getElementsByTagName(topDoc.compatMode == \"BackCompat\" ? \"BODY\" : \"HTML\")[0];\n"
         //if (topWinBody.styleOverflow != undefined) topWinBody.style.overflow = topWinBody.styleOverflow; 去除滚动条\n"
    "}\n"
    "if (isIE) {\n"
 		//释放引用，以便浏览器回收内存\n"
 	"thisdialogDiv.dialogInstance=null;\n"
 	"if(this.innerFrame)this.innerFrame.detachEvent(\"onload\", innerFrameOnload);\n"
 	"this.innerFrame=null;\n"
 	"this.parentWindow=null;\n"
 	"this.bgDiv=null;\n"
 	"if (this.CancelEvent){this.cancelButton.onclick = null;};\n"
 	"if (this.OKEvent){this.okButton.onclick = null;};\n"
 	"topWin.$id(\"_DialogDiv_\" + this.ID).onDragStart=null;\n"
 	"topWin.$id(\"_DialogDiv_\" + this.ID).onDragEnd=null;\n"
 	"topWin.$id(\"_Draghandle_\" + this.ID).onmousedown=null;\n"
 	"topWin.$id(\"_Draghandle_\" + this.ID).root=null;\n"
 		//end释放引用\n"
        "thisdialogDiv.outerHTML = \"\";\n"
 	"CollectGarbage();\n"
    "} else {\n"
        "var RycDiv = topWin.$id(\"_RycDiv\");\n"
        "if (!RycDiv) {\n"
            "RycDiv = topDoc.createElement(\"div\");\n"
            "RycDiv.id = \"_RycDiv\";\n"
        "}\n"
        "RycDiv.appendChild(thisdialogDiv);\n"
        "RycDiv.innerHTML = \"\";\n"
 	"RycDiv=null;\n"
    "}\n"
 "thisdialogDiv=null;\n"
    "this.closed = true;\n"
"};\n"
"Dialog.prototype.autoClose = function () {\n"
    "if (this.closed) {\n"
        "clearTimeout(this._closeTimeoutId);\n"
        "return;\n"
    "}\n"
    "this.AutoClose -= 1;\n"
    "topWin.$id(\"_Title_\" + this.ID).innerHTML = this.AutoClose + \" 秒后自动关闭\";\n"
    "if (this.AutoClose <= 0) {\n"
        "this.close();\n"
    "} else {\n"
        "var self = this;\n"
        "this._closeTimeoutId = setTimeout(function () {\n"
            "self.autoClose();\n"
        "},\n"
        "1000);\n"
    "}\n"
"};\n"
"Dialog.getInstance = function (id) {\n"
    "var dialogDiv = topWin.$id(\"_DialogDiv_\" + id);\n"
    "if (!dialogDiv) alert(\"没有取到对应ID的弹出框页面对象\");\n"
 "try{\n"
     "return dialogDiv.dialogInstance;\n"
 "}finally{\n"
 	"dialogDiv = null;\n"
 "}\n"
"};\n"
"Dialog.prototype.addButton = function (id, txt, func) {\n"
    "topWin.$id(\"_ButtonRow_\" + this.ID).style.display = \"\";\n"
    "this.ShowButtonRow = true;\n"
    "var button = topDoc.createElement(\"input\");\n"
    "button.id = \"_Button_\" + this.ID + \"_\" + id;\n"
    "button.type = \"button\";\n"
    "button.style.cssText = \"margin-right:5px\";\n"
    "button.value = txt;\n"
    "button.onclick = func;\n"
    "var input0 = topWin.$id(\"_DialogButtons_\" + this.ID).getElementsByTagName(\"INPUT\")[0];\n"
    "input0.parentNode.insertBefore(button, input0);\n"
    "return button;\n"
"};\n"
"Dialog.prototype.removeButton = function (btn) {\n"
    "var input0 = topWin.$id(\"_DialogButtons_\" + this.ID).getElementsByTagName(\"INPUT\")[0];\n"
    "input0.parentNode.removeChild(btn);\n"
"};\n"
"Dialog.getBgdiv = function () {\n"
    "if (Dialog.bgDiv) return Dialog.bgDiv;\n"
    "var bgdiv = topWin.$id(\"_DialogBGDiv\");\n"
    "if (!bgdiv) {\n"
        "bgdiv = topDoc.createElement(\"div\");\n"
        "bgdiv.id = \"_DialogBGDiv\";\n"
        "bgdiv.style.cssText = \"position:absolute;left:0px;top:0px;width:100%;height:100%;z-index:900\";\n"
        "var bgIframeBox = '<div style=\"position:relative;width:100%;height:100%;\">';\n"
 	"var bgIframeMask = '<div id=\"_DialogBGMask\" style=\"position:absolute;background-color:#333;opacity:0.4;filter:alpha(opacity=40);width:100%;height:100%;\"></div>';\n"
 	"var bgIframe = isIE6?'<iframe src=\"about:blank\" style=\"filter:alpha(opacity=0);\" width=\"100%\" height=\"100%\"></iframe>':'';\n"
 	"bgdiv.innerHTML=bgIframeBox+bgIframeMask+bgIframe+'</div>';\n"
        "topDoc.getElementsByTagName(\"BODY\")[0].appendChild(bgdiv);\n"
        "if (isIE6) {\n"
            "var bgIframeDoc = bgdiv.getElementsByTagName(\"IFRAME\")[0].contentWindow.document;\n"
            "bgIframeDoc.open();\n"
            "bgIframeDoc.write(\"<body style='background-color:#333' oncontextmenu='return false;'></body>\");\n"
            "bgIframeDoc.close();\n"
 		"bgIframeDoc=null;\n"
        "}\n"
    "}\n"
    "Dialog.bgDiv = bgdiv;\n"
 "bgdiv=null;\n"
    "return Dialog.bgDiv;\n"
"};\n"
"Dialog.prototype.getDialogDiv = function () {\n"
 "var dialogDiv=topWin.$id(\"_DialogDiv_\" + this.ID)\n"
 "if(!dialogDiv)alert(\"获取弹出层页面对象出错！\");\n"
 "try{\n"
 	"return dialogDiv;\n"
 "}finally{\n"
 	"dialogDiv = null;\n"
 "}\n"
"};\n"
"Dialog.onKeyDown = function (event) {\n"
     //shift键\n"
    "if (event.shiftKey && event.keyCode == 9) { \n"
        "if (topWin.Dialog._Array.length > 0) {\n"
            "stopEvent(event);\n"
            "return false;\n"
        "}\n"
    "}\n"
     //ESC键\n"
    "if (event.keyCode == 27) { \n"
        "Dialog.close();\n"
    "}\n"
"};\n"
"Dialog.close = function (id) {\n"
    "if (topWin.Dialog._Array.length > 0) {\n"
        "var diag = topWin.Dialog._Array[topWin.Dialog._Array.length - 1];\n"
        "diag.cancelButton.onclick.apply(diag.cancelButton, []);\n"
    "}\n"
"};\n"
"Dialog.alert = function (msg, func, w, h) {\n"
    "var w = w || 300,\n"
        "h = h || 110;\n"
    "var diag = new Dialog({\n"
        "Width: w,\n"
        "Height: h\n"
    "});\n"
    "diag.ShowButtonRow = true;\n"
    "diag.Title = \"系统提示\";\n"
    "diag.CancelEvent = function () {\n"
        "diag.close();\n"
        "if (func) func();\n"
    "};\n"
    "diag.InnerHtml = '<table height=\"100%\" border=\"0\" align=\"center\" cellpadding=\"10\" cellspacing=\"0\">\\\n"
 	"<tr><td align=\"right\"><img id=\"Icon_' + this.ID + '\" src=\"' + IMAGESPATH + 'i_034.gif\" width=\"34\" height=\"34\" align=\"absmiddle\"></td>\\\n"
 		"<td align=\"left\" id=\"Message_' + this.ID + '\" style=\"font-size:9pt\">' + msg + '</td></tr>\\\n"
 "</table>';\n"
    "diag.show();\n"
    "diag.okButton.parentNode.style.textAlign = \"center\";\n"
    "diag.okButton.style.display = \"none\";\n"
    "diag.cancelButton.value = \"确 定\";\n"
    "diag.cancelButton.focus();\n"
"};\n"
"Dialog.confirm = function (msg, funcOK, funcCal, w, h) {\n"
    "var w = w || 300,\n"
        "h = h || 110;\n"
    "var diag = new Dialog({\n"
        "Width: w,\n"
        "Height: h\n"
    "});\n"
    "diag.ShowButtonRow = true;\n"
    "diag.Title = \"信息确认\";\n"
    "diag.CancelEvent = function () {\n"
        "diag.close();\n"
        "if (funcCal) {\n"
            "funcCal();\n"
        "}\n"
    "};\n"
    "diag.OKEvent = function () {\n"
        "diag.close();\n"
        "if (funcOK) {\n"
            "funcOK();\n"
        "}\n"
    "};\n"
    "diag.InnerHtml = '<table height=\"100%\" border=\"0\" align=\"center\" cellpadding=\"10\" cellspacing=\"0\">\\\n"
 	"<tr><td align=\"right\"><img id=\"Icon_' + this.ID + '\" src=\"' + IMAGESPATH + 'i_036.gif\" width=\"34\" height=\"34\" align=\"absmiddle\"></td>\\\n"
 		"<td align=\"left\" id=\"Message_' + this.ID + '\" style=\"font-size:9pt\">' + msg + '</td></tr>\\\n"
 "</table>';\n"
    "diag.show();\n"
    "diag.okButton.parentNode.style.textAlign = \"center\";\n"
    "diag.okButton.focus();\n"
"};\n"
"Dialog.open = function (arg) {\n"
    "var diag = new Dialog(arg);\n"
    "diag.show();\n"
    "return diag;\n"
"};\n"
"if (isIE) {\n"
    "window.attachEvent(\"onload\", Dialog.attachBehaviors);\n"
"} else {\n"
    "window.addEventListener(\"load\", Dialog.attachBehaviors, false);\n"
"}\n"
};
#endif
