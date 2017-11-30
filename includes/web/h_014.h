#ifndef _HTML_H_014_H_
#define _HTML_H_014_H_
static const char h_014[] = {
"HTTP/1.1 200 OK\r\nContent-Type:text/html\r\nAccept-Ranges:bytes\r\n\r\n"
"<!DOCTYPE html PUBLIC '-//W3C//DTD XHTML 1.0 Transitional//EN' 'http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd'>"
"<html xmlns='http://www.w3.org/1999/xhtml'>"
    "<head>"
        "<meta http-equiv='Content-Type' content='text/html; charset=GB2312'/>"
	"<script type='text/javascript'>"
		"function replace_location(url)"
		"{"
			"var isIe=(document.all)?true:false;"
			"if(isIe)"
			"{"
				"var linka=document.createElement('a');"
				"linka.href=url;"
				"document.body.appendChild(linka);"
				"linka.click();"
			"}"
			"else window.location=url;"
		"}"
	"</script>"
    "</head>"
    "<body onload=\"replace_location('Login.html')\">"
    "</body>"
"</html>"
};
#endif
