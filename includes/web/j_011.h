#ifndef _J_011_H_
#define _J_011_H_

static const char j_011[]={
"HTTP/1.1 200 OK\r\nContent-Type:text/javascript\r\nAccept-Ranges:bytes\r\n\r\n"
"(function (H) {\n"
    "if (!H.exporting) {\n"
        "H.exporting = function () { };\n"
    "}\n"
     // This will be redefined later;\n"
    "var oldExport = H.Chart.prototype.exportChart;\n"
    "H.Chart.prototype.exportChart = function () { };\n"
     // Set the URL of the export server to a non-existant one, just to be sure.\n"
    "H.getOptions().exporting.url = \"http://127.0.0.1:666/\";\n"
    "var MIME_TYPES = {\n"
        "\"PDF\": \"application/pdf\",\n"
        "\"PNG\": \"image/png\",\n"
        "\"JPEG\": \"image/jpeg\",\n"
        "\"SVG\": \"image/svg+xml\",\n"
        "\"CSV\": \"text/csv\",\n"
        "\"XLS\": \"application/vnd.ms-excel\"\n"
    "};\n"
    "var MIME_TYPE_TO_EXTENSION = {\n"
        "\"application/pdf\": \".pdf\",\n"
        "\"image/png\": \".png\",\n"
        "\"image/jpeg\": \".jpeg\",\n"
        "\"image/svg+xml\": \".svg\",\n"
        "\"text/csv\": \".csv\",\n"
        "\"application/vnd.ms-excel\": \".xls\"\n"
    "};\n"
    "var TRANSLATION_KEY_TO_MIME_TYPES = {\n"
        "\"downloadPDF\": \"application/pdf\",\n"
        "\"downloadPNG\": \"image/png\",\n"
        "\"downloadJPEG\": \"image/jpeg\",\n"
        "\"downloadSVG\": \"image/svg+xml\"\n"
    "};\n"
    "TRANSLATION_KEY_TO_MIME_TYPES[H.getOptions().lang.downloadCSV || 'Download CSV'] = \"text/csv\";\n"
    "TRANSLATION_KEY_TO_MIME_TYPES[H.getOptions().lang.downloadXLS || 'Download XLS'] = \"application/vnd.ms-excel\";\n"
     // This var indicates if the browser supports HTML5 download feature\n"
    "var browserSupportDownload = false;\n"
    "var a = document.createElement('a');\n"
    "if (typeof window.btoa != \"undefined\" && typeof a.download != \"undefined\") {\n"
        "browserSupportDownload = true;\n"
    "}\n"
     // This is for IE support of Blob\n"
    "var browserSupportBlob = window.Blob && window.navigator.msSaveOrOpenBlob;\n"
    "/**\n"
     "* Describes the MIME types that this module supports.\n"
     "* Additionnally, you can call `support(mimeType)` to check\n"
     "* that this type is available on the current platform.\n"
     "*/\n"
    "H.exporting.MIME_TYPES = MIME_TYPES;\n"
    "var supportStatus = {};\n"
    "var buildSupportStatus = function () {\n"
        "var hasDownloadOrBlob = browserSupportDownload || browserSupportBlob;\n"
        "supportStatus[MIME_TYPES.CSV] = hasDownloadOrBlob && (H.Chart.prototype.getCSV !== undefined);\n"
        "supportStatus[MIME_TYPES.XLS] = hasDownloadOrBlob && (H.Chart.prototype.getTable !== undefined);\n"
        "var svgSupport = (H.Chart.prototype.getSVG !== undefined);\n"
        "supportStatus[MIME_TYPES.SVG] = hasDownloadOrBlob && svgSupport && (window.btoa !== undefined);\n"
         // Canvg uses a function named RGBColor, but it's also a not widely known standard object\n"
         // http://www.w3.org/TR/2000/REC-DOM-Level-2-Style-20001113/css.html#CSS-RGBColor\n"
         // Fugly, but heh.\n"
        "var rbgColorSupport = false;\n"
        "try {\n"
            "rbgColorSupport = (new RGBColor(\"\").ok) !== undefined;\n"
        "}\n"
        "catch (e) { }\n"
         // We also check that a canvas element can be created.\n"
        "var canvas = document.createElement('canvas');\n"
        "var canvgSupport = typeof canvg !== \"undefined\" && typeof RGBColor != \"undefined\" &&\n"
        "rbgColorSupport && canvas.getContext && canvas.getContext('2d');\n"
        "supportStatus[MIME_TYPES.PNG] = hasDownloadOrBlob && svgSupport && canvgSupport;\n"
         // On IE, it relies on canvas.msToBlob() which always returns PNG\n"
        "supportStatus[MIME_TYPES.JPEG] = /* useless, see last param: hasDownloadOrBlob && */\n"
            "svgSupport && canvgSupport && browserSupportDownload;\n"
        "supportStatus[MIME_TYPES.PDF] = hasDownloadOrBlob && svgSupport && canvgSupport && (typeof jsPDF !== \"undefined\");\n"
    "};\n"
    "buildSupportStatus();\n"
    "/**\n"
     "* Checks if the supplied MIME type is available on the\n"
     "* current platform for a chart to be exported in.\n"
     "* @param mimeType {String} The MIME type.\n"
     "* @returns {boolean} <code>true</code> if the MIME type is available on the\n"
     "*    current platform.\n"
     "*/\n"
    "H.exporting.supports = function (mimeType) {\n"
        "if (supportStatus[mimeType]) {\n"
            "return supportStatus[mimeType];\n"
        "}\n"
        "else {\n"
            "return false;\n"
        "}\n"
    "};\n"
     // Remove unsupported download features from the menu\n"
    "var menuItems = H.getOptions().exporting.buttons.contextButton.menuItems,\n"
        "menuItem,\n"
        "textKey,\n"
        "text,\n"
        "mimeType,\n"
        "handlerBuilder = function (mimeType) {\n"
            "return function () {\n"
                "this.exportChartLocal({\n"
                    "type: mimeType,\n"
                    "csv: {\n"
                        "itemDelimiter: ';'\n"
                    "}\n"
                "});\n"
            "}\n"
        "};\n"
    "for (var i in menuItems) {\n"
        "menuItem = menuItems[i];\n"
        "textKey = menuItems[i].textKey;\n"
        "text = menuItems[i].text; // export-csv do not use a textKey attribute\n"
        "mimeType = TRANSLATION_KEY_TO_MIME_TYPES[textKey] || TRANSLATION_KEY_TO_MIME_TYPES[text];\n"
        "if (mimeType) {\n"
            "if (!H.exporting.supports(mimeType)) {\n"
                 // Setting enabled = false isn't enough.\n"
                "delete menuItems[i];\n"
            "}\n"
            "else {\n"
                 // Redefines click handler to use our method.\n"
                "menuItems[i].onclick = handlerBuilder(mimeType);\n"
            "}\n"
        "}\n"
    "}\n"
    "/*\n"
     "* Converts a SVG string to a canvas element\n"
     "* thanks to canvg.\n"
     "* @param svg {String} A SVG string.\n"
     "* @param width {Integer} The rasterized width.\n"
     "* @param height {Integer} The rasterized height.\n"
     "* @return {DOMNode} a canvas element.\n"
     "*/\n"
    "var svgToCanvas = function (svg, width, height) {\n"
        "var canvas = document.createElement('canvas');\n"
        "canvas.setAttribute('width', width);\n"
        "canvas.setAttribute('height', height);\n"
        "canvas.getContext('2d').drawSvg(svg, 0, 0, width, height);\n"
        "return canvas;\n"
    "};\n"
    "/**\n"
     "* An object to simplifies the retrieval of options in\n"
     "* multiple bundles.\n"
     "* @param opts {Object} Multiple, an object containing options.\n"
     "*/\n"
    "var Opt = function (opts1, opt2, dotdotdot) {\n"
        "this.bundles = arguments;\n"
    "};\n"
    "/**\n"
     "* Fetch the value associated with the specified key in the bundles.\n"
     "* First one defined is the one returned.\n"
     "* @param key {String} The key.\n"
     "* @param value {mixed} The first defined value in the bundles or\n"
     "*    <code>undefined</code> if none is found.\n"
     "*/\n"
    "Opt.prototype.get = function (key) {\n"
        "for (var i = 0; i < this.bundles.length; i++) {\n"
            "if (this.bundles[i] && this.bundles[i][key] !== undefined) {\n"
                "return this.bundles[i][key];\n"
            "}\n"
        "}\n"
        "return undefined;\n"
    "};\n"
     // Default options.\n"
    "var defaultExportOptions = {\n"
        "type: MIME_TYPES.PNG,\n"
        "scale: 2,\n"
        "filename: \"chart\",\n"
        "csv: {\n"
            "useLocalDecimalPoint: true\n"
        "}\n"
    "};\n"
    "/**\n"
     "* Redefines the export function of the official exporting module.\n"
     "* @param options {Object} Overload the export options defined in the chart.\n"
     "* @param chartOptions {Object} Additionnal chart options.\n"
     "*/\n"
    "H.Chart.prototype.exportChartLocal = function (options, chartOptions) {\n"
        "var opt = new Opt(options, this.options.exporting, defaultExportOptions);\n"
        "var type = opt.get(\"type\");\n"
        "if (!H.exporting.supports(type)) {\n"
            "throw new Error(\"Unsupported export format on this platform: \" + type);\n"
        "}\n"
        "var filename = opt.get(\"filename\") + MIME_TYPE_TO_EXTENSION[type];\n"
        "var data = {\n"
            "content: undefined,\n"
            "datauri: undefined,\n"
            "blob: undefined\n"
        "};\n"
        "if (type == MIME_TYPES.CSV) {\n"
             // Copies some values from the options, so we can set it and change those\n"
             // through the options argument.\n"
            "var hasCSVOptions = this.options.exporting && this.options.exporting.csv;\n"
            "var csvOpt = new Opt((options || {}).csv, (this.options.exporting || {}).csv, defaultExportOptions.csv);\n"
            "var oldOptions = {},\n"
            "optionsToCopy = [\"dateFormat\", \"itemDelimiter\", \"lineDelimiter\"],\n"
            "optionToCopy;\n"
            "for (var i in optionsToCopy) {\n"
                "optionToCopy = optionsToCopy[i];\n"
                "if (csvOpt.get(optionToCopy)) {\n"
                    "if (!this.options.exporting) {\n"
                        "this.options.exporting = {};\n"
                    "}\n"
                    "if (!this.options.exporting.csv) {\n"
                        "this.options.exporting.csv = {};\n"
                    "}\n"
                    "oldOptions[optionToCopy] = this.options.exporting.csv[optionToCopy];\n"
                    "this.options.exporting.csv[optionToCopy] = csvOpt.get(optionToCopy);\n"
                "}\n"
            "}\n"
            "var useLocalDecimalPoint = csvOpt.get(\"useLocalDecimalPoint\");\n"
            "var csv = this.getCSV(useLocalDecimalPoint);\n"
            "data.content = csv;\n"
            "if (hasCSVOptions) {\n"
                "for (var i in optionsToCopy) {\n"
                    "optionToCopy = optionsToCopy[i];\n"
                    "if (csvOpt.get(optionToCopy)) {\n"
                        "this.options.exporting.csv[optionToCopy] = oldOptions[optionToCopy];\n"
                    "}\n"
                "}\n"
            "}\n"
            "else {\n"
                "delete this.options.exporting.csv;\n"
            "}\n"
        "}\n"
        "else if (type == MIME_TYPES.XLS) {\n"
             // Same as above\n"
            "var hasCSVOptions = this.options.exporting && this.options.exporting.csv;\n"
            "var csvOpt = new Opt((options || {}).csv, (this.options.exporting || {}).csv, defaultExportOptions.csv);\n"
            "var oldOptions = {},\n"
                "optionsToCopy = [\"dateFormat\"],\n"
                "optionToCopy;\n"
            "for (var i in optionsToCopy) {\n"
                "optionToCopy = optionsToCopy[i];\n"
                "if (csvOpt.get(optionToCopy)) {\n"
                    "if (!this.options.exporting) {\n"
                        "this.options.exporting = {};\n"
                    "}\n"
                    "if (!this.options.exporting.csv) {\n"
                        "this.options.exporting.csv = {};\n"
                    "}\n"
                    "oldOptions[optionToCopy] = this.options.exporting.csv[optionToCopy];\n"
                    "this.options.exporting.csv[optionToCopy] = csvOpt.get(optionToCopy);\n"
                "}\n"
            "}\n"
            "var useLocalDecimalPoint = csvOpt.get(\"useLocalDecimalPoint\");\n"
            "var xls = '<html xmlns:o=\"urn:schemas-microsoft-com:office:office\" xmlns:x=\"urn:schemas-microsoft-com:office:excel\" xmlns=\"http://www.w3.org/TR/REC-html40\">' +\n"
              "'<head><!--[if gte mso 9]><xml><x:ExcelWorkbook><x:ExcelWorksheets><x:ExcelWorksheet>' +\n"
              "'<x:Name>Sheet</x:Name>' +\n"
              "'<x:WorksheetOptions><x:DisplayGridlines/></x:WorksheetOptions></x:ExcelWorksheet></x:ExcelWorksheets></x:ExcelWorkbook></xml><![endif]-->' +\n"
              "'<style>td{border:none;font-family: Calibri, sans-serif;} .number{mso-number-format:\"0.00\";}</style>' +\n"
              "'<meta name=ProgId content=Excel.Sheet>' +\n"
              "'</head><body>' +\n"
              "this.getTable(useLocalDecimalPoint) +\n"
              "'</body></html>';\n"
            "data.content = xls;\n"
            "if (hasCSVOptions) {\n"
                "for (var i in optionsToCopy) {\n"
                    "optionToCopy = optionsToCopy[i];\n"
                    "if (csvOpt.get(optionToCopy)) {\n"
                        "this.options.exporting.csv[optionToCopy] = oldOptions[optionToCopy];\n"
                    "}\n"
                "}\n"
            "}\n"
            "else {\n"
                "delete this.options.exporting.csv;\n"
            "}\n"
        "}\n"
             // Image processing\n"
        "else {\n"
            "var scale = opt.get(\"scale\"),\n"
            "sourceWidth = this.options.width || opt.get(\"sourceWidth\") || this.chartWidth,\n"
            "sourceHeight = this.options.height || opt.get(\"sourceHeight\") || this.chartHeight,\n"
            "destWidth = sourceWidth * scale,\n"
            "destHeight = sourceHeight * scale;\n"
            "var cChartOptions = chartOptions || this.options.exporting && this.options.exporting.chartOptions || {};\n"
            "if (!cChartOptions.chart) {\n"
                "cChartOptions.chart = { width: sourceWidth, height: sourceHeight };\n"
            "}\n"
            "else {\n"
                "cChartOptions.chart.width = destWidth;\n"
                "cChartOptions.chart.height = destHeight;\n"
            "}\n"
            "var svg = this.getSVG(cChartOptions);\n"
            "if (type == MIME_TYPES.SVG) {\n"
                "data.content = svg;\n"
            "}\n"
            "else if (type == MIME_TYPES.PNG || type == MIME_TYPES.JPEG) {\n"
                "var canvas = svgToCanvas(svg, destWidth, destHeight);\n"
                "data.datauri = browserSupportDownload && canvas.toDataURL && canvas.toDataURL(type);\n"
                "data.blob = (type == MIME_TYPES.PNG) && !browserSupportDownload && canvas.msToBlob && canvas.msToBlob();\n"
            "}\n"
            "else if (type == MIME_TYPES.PDF) {\n"
                "var canvas = svgToCanvas(svg, destWidth, destHeight);\n"
                "var doc = new jsPDF('l', 'mm', [destWidth, destHeight]);;\n"
                "doc.addImage(canvas, 'JPEG', 0, 0, destWidth, destHeight);\n"
                "data.datauri = browserSupportDownload && doc.output('datauristring');\n"
                "data.blob = !browserSupportDownload && doc.output('blob');\n"
            "}\n"
        "}\n"
        "if (!data.content && !(data.datauri || data.blob)) {\n"
            "throw new Error(\"Something went wrong while exporting the chart\");\n"
        "}\n"
        "if (browserSupportDownload && (data.datauri || data.content)) {\n"
            "a = document.createElement('a');\n"
            "a.href = data.datauri || ('data:' + type + ';base64,' + window.btoa(unescape(encodeURIComponent(data.content))));\n"
            "a.download = filename;\n"
            "document.body.appendChild(a);\n"
            "a.click();\n"
            "a.remove();\n"
        "}\n"
        "else if (browserSupportBlob && (data.blob || data.content)) {\n"
            "blobObject = data.blob || new Blob([data.content], { type: type });\n"
            "window.navigator.msSaveOrOpenBlob(blobObject, filename);\n"
        "}\n"
        "else {\n"
            "window.open(data);\n"
        "}\n"
    "}\n"
    "H.Chart.prototype.getImageBase64 = function (options, chartOptions) {\n"
        "var opt = new Opt(options, this.options.exporting, defaultExportOptions);\n"
        "var type = opt.get(\"type\");\n"
        "if (!H.exporting.supports(type)) {\n"
            "throw new Error(\"Unsupported export format on this platform: \" + type);\n"
        "}\n"
        "var filename = opt.get(\"filename\") + MIME_TYPE_TO_EXTENSION[type];\n"
        "var data = {\n"
            "content: undefined,\n"
            "datauri: undefined,\n"
            "blob: undefined\n"
        "};\n"
         // Image processing\n"
        "var\n"
        "sourceWidth = this.options.width || opt.get(\"sourceWidth\") || this.chartWidth,\n"
        "sourceHeight = this.options.height || opt.get(\"sourceHeight\") || this.chartHeight,\n"
        "destWidth = sourceWidth,\n"
        "destHeight = sourceHeight;\n"
        "var cChartOptions = chartOptions || this.options.exporting && this.options.exporting.chartOptions || {};\n"
        "if (!cChartOptions.chart) {\n"
            "cChartOptions.chart = { width: sourceWidth, height: sourceHeight };\n"
        "}\n"
        "else {\n"
            "cChartOptions.chart.width = destWidth;\n"
            "cChartOptions.chart.height = destHeight;\n"
        "}\n"
        "var svg = this.getSVG(cChartOptions);\n"
        "var canvas = svgToCanvas(svg, destWidth, destHeight);\n"
         //if (type == MIME_TYPES.PNG || type == MIME_TYPES.JPEG) {\n"
        "data.content = svg;\n"
        "data.datauri = browserSupportDownload && canvas.toDataURL && canvas.toDataURL(type);\n"
        "data.blob = (type == MIME_TYPES.PNG) && !browserSupportDownload && canvas.msToBlob && canvas.msToBlob();\n"
         //a = document.createElement('a');\n"
         //a.href = 'data:' + type + ';base64,' + window.btoa(unescape(encodeURIComponent(data.content)));\n"
         //a.download = filename;\n"
         //document.body.appendChild(a);\n"
         //a.click();\n"
         //a.remove();\n"
         //}\n"
        "return canvas.toDataURL(type);\n"
    "}\n"
     // Forces method from export module to use the local version\n"
    "H.Chart.prototype.exportChart = H.Chart.prototype.exportChartLocal;\n"
"}(Highcharts));\n"
};
#endif