<?php
/**
 * This is the PDFreactor 6.3 PHP API
 * 
 * RealObjects(R) PDFreactor(R) is a powerful formatting processor for converting HTML and XML documents
 * into PDF.<br/>
 * <br/>
 * The simplest sample for the PDFreactor PHP API:<br/>
 * <br/>
 * <code>
 * require_once('PDFreactor.class.php'); 
 * $pdfReactor = new PDFreactor();
 * $result = $pdfReactor->renderDocumentFromURL("http://www.realobjects.com");
 * header("Content-Type: application/pdf");
 * echo $result;
 * </code>
 * @package PDFreactor-PHP
 */
 
//======================================//
// Cleanup constants                    //
// Have to be defined outside of class  //
// as PHP 4 cant handle class constants //
//======================================//

/**
 * Indicates that no cleanup will be performed when loading a document. If the 
 * loaded document is not well-formed, an exception will be thrown.
 * 
 * @see setCleanupTool()
 */
define("CLEANUP_NONE",0);

/**
 * Indicates that JTidy will be used to perform a cleanup when loading a 
 * non-well-formed document.
 * 
 * @see setCleanupTool()
 */
define("CLEANUP_JTIDY",1);

/**
 * Indicates that the CyberNeko HTML parser will be used to perform a 
 * cleanup when loading a non-well-formed document.
 * 
 * @see setCleanupTool()
 */
define("CLEANUP_CYBERNEKO",2);

/**
 * Indicates that the TagSoup HTML parser will be used to perform a 
 * cleanup when loading a non-well-formed document.
 * 
 * @see setCleanupTool().
 */
define("CLEANUP_TAGSOUP",3);  

/**
 * The default cleanup setting. It is set to {@link CLEANUP_CYBERNEKO}.
 * 
 * @see setCleanupTool()
 * 
 * @ignore
 */
define("CLEANUP_DEFAULT",CLEANUP_CYBERNEKO);

//-----------------------------------------
// Document type constants
//-----------------------------------------

/**
 * Indicates that the document type will be detected automatically.
 * 
 * A document has the type {@link DOCTYPE_HTML5} if the name of the root element
 * is "html" (ignoring case considerations). In all other cases the
 * document type is {@link DOCTYPE_XML}.
 * 
 * @see setDocumentType()
 */
define("DOCTYPE_AUTODETECT",0);

/**
 * Indicates that the document type will be set to generic XML. No default 
 * style sheet is used and the document is loaded as is without regards 
 * to style elements or attributes.
 * 
 * @see setDocumentType()
 */
define("DOCTYPE_XML",1);

/**
 * Indicates that the document type will be set to XHTML. The HTML default 
 * style sheet is used and the document is loaded regarding style 
 * elements, style attributes and link style sheets.
 * 
 * @see setDocumentType()
 */
define("DOCTYPE_XHTML",2);

/**
 * Indicates that the document type will be set to HTML5. The HTML default 
 * style sheet is used and the document is loaded regarding style 
 * elements, style attributes and link style sheets.
 * 
 * @see setDocumentType()
 */
define("DOCTYPE_HTML5",3);

/**
 * The default document type setting. It is set to {@link DOCTYPE_AUTODETECT}.
 * 
 * @see setDocumentType()
 * 
 * @ignore
 */
define("DOCTYPE_DEFAULT",DOCTYPE_AUTODETECT);

//------------------------------------
// Document default language constant
//------------------------------------
//TODO reorder to match with PDFreactor constants order.
    
/**
 * Indicates the Default language used for a document
 * if no other language is set in the document itself. 
 */
define("DOCUMENT_DEFAULT_LANGUAGE_DEFAULT", NULL);

//-----------------------------------------
// Encryption constants
//-----------------------------------------

/**
 * Indicates that the document will not be encrypted. If encryption is disabled
 * then no user password and no owner password can be used.
 * 
 * @see setEncryption()
 */
define("ENCRYPTION_NONE",0);

/**
 * Indicates that the document will be encrypted using RC4 40 bit encryption.
 * 
 * @see setEncryption()
 */
define("ENCRYPTION_40",1);

/**
 * Indicates that the document will be encrypted using RC4 128 bit encryption.
 * For normal purposes this value should be used. 
 * 
 * @see setEncryption()
 */
define("ENCRYPTION_128",2);

/**
 * The default encryption setting. It is set to {@link ENCRYPTION_NONE}.
 * 
 * @see setEncryption()
 * 
 * @ignore
 */
define("ENCRYPTION_DEFAULT",ENCRYPTION_NONE);

//-----------------------------------------
// PDF signing mode constants
//-----------------------------------------

/**
 * Keystore type "pkcs12"
 * 
 * @see setSignPDF()
 */
define("KEYSTORE_TYPE_PKCS12", "pkcs12");

/**
 * Keystore type "jks"
 *
 * @see setSignPDF()
 */
define("KEYSTORE_TYPE_JKS", "jks");

/**
 * Signing mode for self-signed certificates
 * 
 * @see setSignPDF()
 */
define("SIGNING_MODE_SELF_SIGNED", 1);

/**
 * Signing mode for VeriSign certificates
 * 
 * @see setSignPDF()
 */
define("SIGNING_MODE_VERISIGN_SIGNED", 2);

/**
 * Signing mode for Windows certificates
 * 
 * @see setSignPDF()
 */
define("SIGNING_MODE_WINCER_SIGNED", 3);

//-----------------------------------------
// Log level constants
//-----------------------------------------

/**
 * Indicates that no log events will be logged.
 * 
 * @see setLogLevel()
 */
define("LOG_LEVEL_NONE",4);

/**
 * Indicates that only fatal log events will be logged.
 * 
 * @see setLogLevel()
 */
define("LOG_LEVEL_FATAL",3);

/**
 * Indicates that warn and fatal log events will be logged.
 * 
 * @see setLogLevel()
 */
define("LOG_LEVEL_WARN",2);

/**
 * Indicates that info, warn and fatal log events will be logged.
 * 
 * @see setLogLevel()
 */
define("LOG_LEVEL_INFO",1);

/**
 * Indicates that debug, info, warn and fatal log events will be logged.
 * 
 * @see setLogLevel()
 */
define("LOG_LEVEL_DEBUG",0);

/**
 * Indicates that all log events will be logged.
 * 
 * @see setLogLevel()
 */
define("LOG_LEVEL_PERFORMANCE",-1);

/**
 * The default log level setting. It is set to {@link LOG_LEVEL_NONE}.
 * 
 * @see setLogLevel()
 * 
 * @ignore
 */
define("LOG_LEVEL_DEFAULT",LOG_LEVEL_NONE);

//===================
// Viewerpreferences 
//===================

/** 
 * Display one page at a time. (default)
 * 
 * @see setViewerPreferences()
 */
define("VIEWER_PREFERENCES_PAGE_LAYOUT_SINGLE_PAGE",1);

/** 
 * Display the pages in one column.
 * 
 * @see setViewerPreferences()
 */
define("VIEWER_PREFERENCES_PAGE_LAYOUT_ONE_COLUMN",2);

/** 
 * Display the pages in two columns, with odd numbered pages on the left.
 * 
 * @see setViewerPreferences() 
 */
define("VIEWER_PREFERENCES_PAGE_LAYOUT_TWO_COLUMN_LEFT",4);

/** 
 * Display the pages in two columns, with odd numbered pages on the right.
 * 
 * @see setViewerPreferences()
 */
define("VIEWER_PREFERENCES_PAGE_LAYOUT_TWO_COLUMN_RIGHT",8);

/** 
 * Display two pages at a time, with odd numbered pages on the left.
 * 
 * @see setViewerPreferences()
 */
define("VIEWER_PREFERENCES_PAGE_LAYOUT_TWO_PAGE_LEFT",16);

/** 
 * Display two pages at a time, with odd numbered pages on the right.
 * 
 * @see setViewerPreferences()
 */
define("VIEWER_PREFERENCES_PAGE_LAYOUT_TWO_PAGE_RIGHT",32);

/** 
 * Show no panel on startup.
 * 
 * @see setViewerPreferences()
 */
define("VIEWER_PREFERENCES_PAGE_MODE_USE_NONE",64);

/** 
 * Show document outline panel on startup.
 * 
 * @see setViewerPreferences()
 */
define("VIEWER_PREFERENCES_PAGE_MODE_USE_OUTLINES",128);

/** 
 * Show thumbnail images panel on startup.
 * 
 * @see setViewerPreferences() 
 */
define("VIEWER_PREFERENCES_PAGE_MODE_USE_THUMBS",256);

/** 
 * Switch to fullscreen mode on startup.
 * 
 * @see setViewerPreferences()
 */
define("VIEWER_PREFERENCES_PAGE_MODE_FULLSCREEN",512);

/** 
 * Show optional content group panel on startup.
 * 
 * @see setViewerPreferences() 
 */
define("VIEWER_PREFERENCES_PAGE_MODE_USE_OC",1024);

/** 
 * Show attachments panel on startup.
 * 
 * @see setViewerPreferences() 
 */
define("VIEWER_PREFERENCES_PAGE_MODE_USE_ATTACHMENTS",2048);

/** 
 * Hide the viewer application's tool bars when the document is active.
 * 
 * @see setViewerPreferences()
 */
define("VIEWER_PREFERENCES_HIDE_TOOLBAR",1 << 12);

/** 
 * Hide the viewer application's menu bar when the document is active.
 * 
 * @see setViewerPreferences()
 */
define("VIEWER_PREFERENCES_HIDE_MENUBAR",1 << 13);

/** 
 * Hide user interface elements in the document's window.
 * 
 * @see setViewerPreferences()
 */
define("VIEWER_PREFERENCES_HIDE_WINDOW_UI",1 << 14);

/** 
 * Resize the document's window to fit the size of the first displayed page
 * 
 * @see setViewerPreferences()
 */
define("VIEWER_PREFERENCES_FIT_WINDOW",1 << 15);

/** 
 * Position the document's window in the center of the screen.
 * 
 * @see setViewerPreferences()
 */
define("VIEWER_PREFERENCES_CENTER_WINDOW",1 << 16);

/** 
 * Display the document's title in the top bar.
 * 
 * @see setViewerPreferences()
 */
define("VIEWER_PREFERENCES_DISPLAY_DOC_TITLE",1 << 17);

/** 
 * Show document outline panel on exiting full-screen mode. Has to be combined with {@link VIEWER_PREFERENCES_PAGE_MODE_FULLSCREEN}.
 * 
 * @see setViewerPreferences()
 */
define("VIEWER_PREFERENCES_NON_FULLSCREEN_PAGE_MODE_USE_NONE",1 << 18);

/** 
 * Show thumbnail images panel on exiting full-screen mode. Has to be combined with {@link VIEWER_PREFERENCES_PAGE_MODE_FULLSCREEN}.
 * 
 * @see setViewerPreferences()
 */
define("VIEWER_PREFERENCES_NON_FULLSCREEN_PAGE_MODE_USE_OUTLINES",1 << 19);

/** 
 * Show optional content group panel on exiting full-screen mode. Has to be combined with {@link VIEWER_PREFERENCES_PAGE_MODE_FULLSCREEN}.
 * 
 * @see setViewerPreferences()
 */
define("VIEWER_PREFERENCES_NON_FULLSCREEN_PAGE_MODE_USE_THUMBS",1 << 20);

/** 
 * Show attachments panel on exiting full-screen mode. Has to be combined with {@link VIEWER_PREFERENCES_PAGE_MODE_FULLSCREEN}.
 * 
 * @see setViewerPreferences()
 */
define("VIEWER_PREFERENCES_NON_FULLSCREEN_PAGE_MODE_USE_OC",1 << 21);

/** 
 * Position pages in ascending order from left to right.
 * 
 * @see setViewerPreferences()
 */
define("VIEWER_PREFERENCES_DIRECTION_L2R",1 << 22);

/** 
 * Position pages in ascending order from right to left.
 * 
 * @see setViewerPreferences()
 */
define("VIEWER_PREFERENCES_DIRECTION_R2L",1 << 23);

/** 
 * Print dialog default setting: disabled scaling
 * 
 * @see setViewerPreferences()
 */
define("VIEWER_PREFERENCES_PRINTSCALING_NONE", 1 << 24);

/** 
 * Print dialog default setting: set scaling to application default value.
 * 
 * @see setViewerPreferences()
 */
define("VIEWER_PREFERENCES_PRINTSCALING_APPDEFAULT", 1 << 25);

/** 
 * Print dialog default setting: simplex
 * 
 * @see setViewerPreferences()
 */
define("VIEWER_PREFERENCES_DUPLEX_SIMPLEX", 1 << 26);

/** 
 * Print dialog default setting: duplex (short edge)
 * 
 * @see setViewerPreferences()
 */
define("VIEWER_PREFERENCES_DUPLEX_FLIP_SHORT_EDGE", 1 << 27);

/** 
 * Print dialog default setting: duplex (long edge)
 * 
 * @see setViewerPreferences()
 */
define("VIEWER_PREFERENCES_DUPLEX_FLIP_LONG_EDGE", 1 << 28);

/** 
 * Print dialog default setting: do not pick tray by PDF size
 * 
 * @see setViewerPreferences()
 */
define("VIEWER_PREFERENCES_PICKTRAYBYPDFSIZE_FALSE", 1 << 29);

/** 
 * Print dialog default setting: pick tray by PDF size
 * 
 * @see setViewerPreferences()
 */
define("VIEWER_PREFERENCES_PICKTRAYBYPDFSIZE_TRUE", 1 << 30);

//-----------------------------------------
// Color Space Constants
//-----------------------------------------
//TODO reorder to match with PDFreactor constants order.

/** 
 * The color space RGB
 */
define("COLOR_SPACE_RGB", 0);

/** 
 * The color space CMYK
 */
define("COLOR_SPACE_CMYK", 1);


//=====================
// Conformance constants
//=====================
    
/** 
 * PDF with no additional restrictions (default) 
 * 
 * @see setConformance()
 */
define("CONFORMANCE_PDF", 0);

/** 
 * PDF/A-1a (ISO 19005-1 Level A)
 * 
 * @see setConformance()
 */
define("CONFORMANCE_PDFA", 1);

/**
 * The default value of the conformance property which is {@link CONFORMANCE_PDF}.
 * 
 * @see setConformance()
 * 
 * @ignore
 */
define("CONFORMANCE_DEFAULT", CONFORMANCE_PDF);

//-----------------------------------------
// Processing Preferences Constants
//-----------------------------------------
//TODO reorder to match with PDFreactor constants order.

/** 
 * Processing preferences flag for the memory saving mode for images.
 * 
 * @see setProcessingPreferences()
 */
define("PROCESSING_PREFERENCES_SAVE_MEMORY_IMAGES", 1);

//-----------------------------------------
// Merge Mode Constants
//-----------------------------------------
//TODO reorder to match with PDFreactor constants order.

/**
 * Default merge mode: Append converted document to existing PDF.
 * 
 * @see setMergeMode()
 */
define("MERGE_MODE_APPEND", 1);

/**
 * Alternate merge mode: Prepend converted document to existing PDF.
 * 
 * @see setMergeMode()
 */
define("MERGE_MODE_PREPEND", 2);

/**
 * Alternate merge mode (overlay): Adding converted document above the existing PDF.
 * 
 * @see setMergeMode()
 */
define("MERGE_MODE_OVERLAY", 3);

/**
 * Alternate merge mode (overlay): Adding converted document below the existing PDF.
 * 
 * @see setMergeMode()
 */
define("MERGE_MODE_OVERLAY_BELOW", 4);

//-----------------------------------------
// Overlay Repeat Constants
//-----------------------------------------
//TODO reorder to match with PDFreactor constants order.

/**
 * No pages of the shorter document are repeated, leaving some pages of the longer document without overlay.
 * 
 * @see setMergeMode()
 */
define("OVERLAY_REPEAT_NONE", 0);

/**
 * Last page of the shorter document is repeated, to overlay all pages of the longer document.
 * 
 * @see setMergeMode()
 */
define("OVERLAY_REPEAT_LAST_PAGE", 1);

/**
 * All pages of the shorter document are repeated, to overlay all pages of the longer document.
 * 
 * @see setMergeMode()
 */
define("OVERLAY_REPEAT_ALL_PAGES", 2);

//-----------------------------------------
// Page Order Constants
//-----------------------------------------

/**
 * Page order mode to reverse the page order.
 * 
 * @see setPageOrder()
 */
define("PAGE_ORDER_REVERSE", "REVERSE");

/**
 * Page order mode to keep odd pages only.
 * 
 * @see setPageOrder()
 */
define("PAGE_ORDER_ODD", "ODD");

/**
 * Page order mode to keep even pages only.
 * 
 * @see setPageOrder()
 */
define("PAGE_ORDER_EVEN", "EVEN");

/**
 * Page order mode to arrange all pages in booklet order.
 * 
 * @see setPageOrder()
 */
define("PAGE_ORDER_BOOKLET", "BOOKLET");

/**
 * Page order mode to arrange all pages in right-to-left booklet order.
 * 
 * @see setPageOrder()
 */
define("PAGE_ORDER_BOOKLET_RTL", "BOOKLET_RTL");

//-----------------------------------------
// Pages Per Sheet Constants
//-----------------------------------------
//TODO reorder to match with PDFreactor constants order.

/**
 * Arranges the pages on a sheet from left to right and top to bottom.
 * 
 * @see setPagesPerSheetProperties()
 */
define("PAGES_PER_SHEET_DIRECTION_RIGHT_DOWN", 0);

/**
 * Arranges the pages on a sheet from right to left and top to bottom.
 * 
 * @see setPagesPerSheetProperties()
 */
define("PAGES_PER_SHEET_DIRECTION_LEFT_DOWN", 1);

/**
 * Arranges the pages on a sheet from left to right and bottom to top.
 * 
 * @see setPagesPerSheetProperties()
 */
define("PAGES_PER_SHEET_DIRECTION_RIGHT_UP", 2);

/**
 * Arranges the pages on a sheet from right to left and bottom to top.
 * 
 * @see setPagesPerSheetProperties()
 */
define("PAGES_PER_SHEET_DIRECTION_LEFT_UP", 3);

/**
 * Arranges the pages on a sheet from top to bottom and left to right.
 * 
 * @see setPagesPerSheetProperties()
 */
define("PAGES_PER_SHEET_DIRECTION_DOWN_RIGHT", 4);

/**
 * Arranges the pages on a sheet from top to bottom and right to left.
 * 
 * @see setPagesPerSheetProperties()
 */
define("PAGES_PER_SHEET_DIRECTION_DOWN_LEFT", 5);

/**
 * Arranges the pages on a sheet from bottom to top and left to right.
 * 
 * @see setPagesPerSheetProperties()
 */
define("PAGES_PER_SHEET_DIRECTION_UP_RIGHT", 6);

/**
 * Arranges the pages on a sheet from bottom to top and right to left.
 * 
 * @see setPagesPerSheetProperties()
 */
define("PAGES_PER_SHEET_DIRECTION_UP_LEFT", 7);

//-------------------------------------
// Exceeding content logging constants
//-------------------------------------
//TODO reorder to match with PDFreactor constants order.

/**
 * Do not log exceeding content.
 * 
 * @see setLogExceedingContent()
 */
define("EXCEEDING_CONTENT_ANALYZE_NONE", 0);

/**
 * Log exceeding content.
 * 
 * @see setLogExceedingContent()
 */
define("EXCEEDING_CONTENT_ANALYZE_CONTENT", 1);

/**
 * Log exceeding content and boxes without absolute positioning.
 * 
 * @see setLogExceedingContent()
 */
define("EXCEEDING_CONTENT_ANALYZE_CONTENT_AND_STATIC_BOXES", 2);

/**
 * Log exceeding content and all boxes.
 * 
 * @see setLogExceedingContent()
 */
define("EXCEEDING_CONTENT_ANALYZE_CONTENT_AND_BOXES", 3);

/**
 * Do not log exceeding content.
 * 
 * @see setLogExceedingContent()
 */
define("EXCEEDING_CONTENT_AGAINST_NONE", 0);

/**
 * Log content exceeding the edges of its page.
 * 
 * @see setLogExceedingContent()
 */
define("EXCEEDING_CONTENT_AGAINST_PAGE_BORDERS", 1);

/**
 * Log content exceeding its page content area (overlaps the page margin).
 * 
 * @see setLogExceedingContent()
 */
define("EXCEEDING_CONTENT_AGAINST_PAGE_CONTENT", 2);

/**
 * Log content exceeding its container.
 * 
 * @see setLogExceedingContent()
 */
define("EXCEEDING_CONTENT_AGAINST_PARENT", 3);

//-------------------------------------
// JavaScript mode Constants
//-------------------------------------

/**
 * Indicates that JavaScript is disabled.
 * 
 * @see setJavaScriptMode()
 */
define("JAVASCRIPT_MODE_DISABLED", 0);

/**
 * Indicates that JavaScript is enabled.
 * 
 * @see setJavaScriptMode()
 */
define("JAVASCRIPT_MODE_ENABLED", 1);

/**
 * Indicates that JavaScript is enabled, without access to layout data.
 * 
 * @see setJavaScriptMode()
 */
define("JAVASCRIPT_MODE_ENABLED_NO_LAYOUT", 2);

/**
 * Indicates that JavaScript is enabled, without converter-specific 
 * optimizations to timeouts and intervals. This mode is significantly more 
 * time consuming and should only be used when no other mode provides the
 * expected results.
 * 
 * @see setJavaScriptMode()
 */
define("JAVASCRIPT_MODE_ENABLED_REAL_TIME", 3);

/**
 * Indicates that JavaScript is enabled, with time stamps increasing more quickly.
 * This makes some kinds of JS based animations (e.g. when using jQuery)
 * finish immediately, which has the same effect as JAVASCRIPT_MODE_ENABLED_REAL_TIME,
 * but is significantly faster. Only Date.getTime() is affected by this.
 * 
 * @see setJavaScriptMode()
 */
define("JAVASCRIPT_MODE_ENABLED_TIME_LAPSE", 4);

/**
 * @ignore
 */
define("JAVASCRIPT_MODE_DEFAULT", JAVASCRIPT_MODE_DISABLED);

//-------------------------------------
// HTTPS mode Constants
//-------------------------------------

 /**
  * Indicates strict HTTPS behavior. This matches the default behavior of Java.
  * 
  * @see setHTTPSMode()
  */
define("HTTPS_MODE_STRICT", 0);


/**
 * Indicates lenient HTTPS behavior. This means that many certificate issues are ignored.
 * 
 * @see setHTTPSMode()
 */
define("HTTPS_MODE_LENIENT", 1);

/**
 * @ignore
 */
define("HTTPS_MODE_DEFAULT", HTTPS_MODE_STRICT);
    
//-----------------------------------------
// Output Format constants
//-----------------------------------------
    
/**
 * PDF output format
 * @see setOutputFormat()
 */
define("OUTPUT_FORMAT_PDF", 0);

/**
 * JPEG output format
 * @see setOutputFormat()
 */
define("OUTPUT_FORMAT_JPEG", 1);

/**
 * PNG output format (using Java Image I/O)
 * @see setOutputFormat()
 */
define("OUTPUT_FORMAT_PNG", 2);

/**
 * Transparent PNG output format (using Java Image I/O)
 * @see setOutputFormat()
 */
define("OUTPUT_FORMAT_PNG_TRANSPARENT", 3);

/**
 * BMP output format
 * @see setOutputFormat()
 */
define("OUTPUT_FORMAT_BMP", 4);

/**
 * GIF output format
 * @see setOutputFormat()
 */
define("OUTPUT_FORMAT_GIF", 5);

/**
 * PNG output format (using Apache Imaging)
 * @see setOutputFormat()
 */
define("OUTPUT_FORMAT_PNG_AI", 6);

/**
 * Transparent PNG output format (using Apache Imaging)
 * @see setOutputFormat()
 */
define("OUTPUT_FORMAT_PNG_TRANSPARENT_AI", 7);

/**
 * LZW compressed TIFF output format
 * @see setOutputFormat()
 */
define("OUTPUT_FORMAT_TIFF_LZW", 8);

/**
 * PackBits compressed TIFF output format
 * @see setOutputFormat()
 */
define("OUTPUT_FORMAT_TIFF_PACKBITS", 9);

/**
 * Uncompressed TIFF output format
 * @see setOutputFormat()
 */
define("OUTPUT_FORMAT_TIFF_UNCOMPRESSED", 10);

/**
 * Monochrome CCITT 1D compressed TIFF output format
 * @see setOutputFormat()
 */
define("OUTPUT_FORMAT_TIFF_CCITT_1D", 11);

/**
 * Monochrome CCITT Group 3 compressed TIFF output format
 * @see setOutputFormat()
 */
define("OUTPUT_FORMAT_TIFF_CCITT_GROUP_3", 12);

/**
 * Monochrome CCITT Group 4 compressed TIFF output format
 * @see setOutputFormat()
 */
define("OUTPUT_FORMAT_TIFF_CCITT_GROUP_4", 13);
    
class PDFreactor {

    /**
     * SOAP Client Object
     * @ignore
     */
    var $client;
    
    /**
     * Configuration Object
     * @ignore
     */
    var $conf_obj;
    
    /**
     * Userstyle Object
     * @ignore
     */
    var $style_obj;
    
    /**
     * Result Object
     * @ignore
     */
    var $result;
    
    //=======================
    // Constructor
    //=======================
    
    /**
     * Constructs a new PDFreactor object.
     */
  function PDFreactor($host = "", $port = "", $debug = false) {
    if ($host == "") $host = "127.0.0.1";
    if ($port == "") $port = "9423";
    $this->debug = $debug;

    if ($this->debug) {
      $this->client = new ROClient($host, $port, "/pdfreactor/services/PDFreactor", true);
    } else {
      $this->client = new ROClient($host, $port, "/pdfreactor/services/PDFreactor");
    }
    $this->conf_obj = new PDFreactorWebserviceConfiguration();
  }
    
    //-----------------------------------------
    // System Property Setter
    //-----------------------------------------
    
    /**
     * Sets the license key using a string.
     * If no license key could be found then PDFreactor runs in evaluation mode. 
     * 
     * The default value is <samp>null</samp>.
     * 
     * @param string $content The content of the license key as a string.
     */
    function setLicenseKey($content) {
        if (!is_string($content)) {
            die("<b>setLicenseKey:</b> Value has to be of type string");
        } else {
            $this->conf_obj->licenseKey = $content;
        }
    }
    
    /**
     * Enables or disables the font registration. If font registration is 
     * enabled and a valid font cache exists, then this font cache will be used. 
     * If font registration is disabled, any existing font cache will be 
     * ignored and the font directories will be scanned for font information.
     * 
     * The default value is <samp>false</samp>.
     * 
     * @param boolean $value The new value of the font registration setting.
     * 
     * @see setCacheFonts()
     * @see setFontCachePath()
     * @see addFontDirectory()
     */
    function setDisableFontRegistration($value) {
        if (!is_bool($value)) { 
            die("<b>setDisableFontRegistration:</b> Value has to be of type boolean");
        } else {
            $this->conf_obj->disableFontRegistration = $value;
        }
    }
    
    /**
     * Enables or disables caching of font information. 
     * 
     * During the PDF creation PDFreactor requires information about 
     * fonts in the system. The process to get this information takes a 
     * long time, thus PDFreactor offers an option to cache the collected 
     * information about fonts. 
     * 
     * A font cache can be reused by PDFreactor on every PDF creation 
     * process but only if font registration is enabled. If font registration 
     * is disabled then the font cache will be ignored. (See {@link setDisableFontRegistration()} for more information).
     * 
     * The default value is <samp>true</samp>. 
     * 
     * @param boolean $value The new value of the cache fonts setting.
     * 
     * @see setDisableFontRegistration()
     * @see setFontCachePath()
     * @see addFontDirectory()
     */
    function setCacheFonts($value) {
        if (!is_bool($value)) { 
            die("<b>setCacheFonts:</b> Value has to be of type boolean");
        } else {
            $this->conf_obj->cacheFonts = $value;
        }
    }
    
    /**
     * Sets the path of the font cache.
     * 
     * This path will be used to read and write the font cache. If the font cache path is set to
     * <samp>null</samp> then the "user.home" directory will be used. 
     * 
     * The default value is <samp>null</samp>.
     * 
     * @param string $location The path of the font cache.
     * 
     * @see setDisableFontRegistration()
     * @see setCacheFonts()
     * @see addFontDirectory()
     */
    function setFontCachePath($location) {
        if (!is_string($location)) { 
            die("<b>setFontCachePath:</b> Value has to be of type string");
        } else {
            $this->conf_obj->fontCachePath = $location;
        }
    }
    
    /**
     * @deprecated  As of PDFreactor 6.0, replaced by {@link addFontDirectory()}
     */
    function setFontDirectory($location) {
        if (!is_string($location)) { 
            die("<b>setFontDirectory:</b> Value has to be of type string");
        } else {
            $this->conf_obj->fontDirectory = $location;
        }
    }
    
    /**
     * Registers an additional font directory to load fonts from.
     * 
     * @param string $fontDirectory The path of the font directory.
     */
    function addFontDirectory($fontDirectory) {
        if (!isset($this->conf_obj->fontDirectories)) {
            $this->conf_obj->fontDirectories = array($fontDirectory);
        } else {
            array_push($this->conf_obj->fontDirectories,$fontDirectory);
        }
    }
    
    /**
     * Loads a font from a URL which can be used via the CSS property "font-family".
     * 
     * @param string $source The URL to load the font from.
     * @param string $family The font family name the loaded font will be labeled as.
     * @param boolean $bold Whether the font will be labeled bold.
     * @param boolean $italic Whether the font will be labeled italic.
     */
    function addFont($source, $family, $bold, $italic) {
        $this->font_obj = new PDFreactorWebserviceFontData($source, $family, $bold, $italic);
        
        if (!isset($this->conf_obj->fonts))
        {
            $this->conf_obj->fonts = array($this->font_obj);
        }
        else {array_push($this->conf_obj->fonts,$this->font_obj);}
        $this->font_obj = NULL;
    }
    
    /**
     * Registers an alias font family for an existing font.
     * 
     * This function is limited to fonts loaded automatically from system folders.
     * 
     * @param string $source The name of an existing font family.
     * @param string $family The alias name for that font.
     * @param boolean $bold Whether the alias will be labeled bold.
     * @param boolean $italic Whether the alias will be labeled italic.
     */
    function addFontAlias($source, $family, $bold, $italic) {
        $this->fontalias_obj = new PDFreactorWebserviceFontData($source, $family, $bold, $italic);
        
        if (!isset($this->conf_obj->fontAliases))
        {
            $this->conf_obj->fontAliases = array($this->fontalias_obj);
        }
        else {array_push($this->conf_obj->fontAliases,$this->fontalias_obj);}
        $this->fontalias_obj = NULL;
    }
    
    /**
     * Sets the log level. 
     * 
     * Use one of the following <samp>LOG_LEVEL_</samp> constants to specify the log level:
     * 
     * {@link LOG_LEVEL_NONE}<br/>
     * {@link LOG_LEVEL_FATAL}<br/>
     * {@link LOG_LEVEL_WARN}<br/>
     * {@link LOG_LEVEL_INFO}<br/>
     * {@link LOG_LEVEL_DEBUG}<br/>
     * {@link LOG_LEVEL_PERFORMANCE}<br/>
     * 
     * The default value is {@link LOG_LEVEL_NONE}.
     * 
     * @param int $value The new log level setting.
     * 
     * @see getLog()
     */
    function setLogLevel($value) {
        if (!is_numeric($value)) { 
            die("<b>setLogLevel:</b> Value has to be of type int");
        } else {
            $this->conf_obj->logLevel = $value;
        }
    }
    
    /**
     * Whether to log content exceeding the page.
     * 
     * Use <samp>EXCEEDING_CONTENT_ANALYZE_</samp> constants to specify which kind of content should be observed for exceeding content logging.
     * The default value is {@link EXCEEDING_CONTENT_ANALYZE_NONE}.
     * 
     * {@link EXCEEDING_CONTENT_ANALYZE_NONE}<br/>
     * {@link EXCEEDING_CONTENT_ANALYZE_CONTENT}<br/>
     * {@link EXCEEDING_CONTENT_ANALYZE_CONTENT_AND_BOXES}<br/>
     * {@link EXCEEDING_CONTENT_ANALYZE_CONTENT_AND_STATIC_BOXES}<br/>
     * 
     * Use <samp>EXCEEDING_CONTENT_AGAINST_</samp> constants to specify that exceeding this content starts exceeding content logging.
     * The default value is {@link EXCEEDING_CONTENT_AGAINST_NONE}.
     * 
     * {@link EXCEEDING_CONTENT_AGAINST_NONE}<br/>
     * {@link EXCEEDING_CONTENT_AGAINST_PARENT}<br/>
     * {@link EXCEEDING_CONTENT_AGAINST_PAGE_CONTENT}<br/>
     * {@link EXCEEDING_CONTENT_AGAINST_PAGE_BORDERS}<br/>
     * 
     * @param int $logExceedingContentAnalyze Enables logging of exceeding content 
     *                                   and optionally of boxes.
     * @param int $logExceedingContentAgainst Enables logging of exceeding content 
     *                                   either against the page edges, page 
     *                                   content areas or containers.
     */
    function setLogExceedingContent($logExceedingContentAnalyze, $logExceedingContentAgainst) {
        if (!is_numeric($logExceedingContentAnalyze) && !is_numeric($logExceedingContentAgainst)) { 
            die("<b>setLogLevel:</b> Value has to be of type int");
        } else {
            $this->conf_obj->logExceedingContentAnalyze = $logExceedingContentAnalyze;
            $this->conf_obj->logExceedingContentAgainst = $logExceedingContentAgainst;
        }
    }
    
    /**
     * Whether an exception should be thrown when no legal full license key is set. 
     * 
     * This allows to programmatically ensure that documents are not altered due to license issues.
     * 
     * The default value is <samp>false</samp>.
     */
    function setThrowLicenseExceptions($throwLicenseExceptions) {
        if (!is_bool($throwLicenseExceptions)) { 
            die("<b>setThrowLicenseExceptions:</b> Value has to be of type boolean");
        } else {
            $this->conf_obj->throwLicenseExceptions = $throwLicenseExceptions;
        }
    }
    
    //-----------------------------------------
    // XML document properties
    //-----------------------------------------
    
    /**
     * Sets the base URL of the XML document. 
     * 
     * To resolve relative URLs to absolute URLs a reference (base) URL is required. This
     * reference URL is usually the system id of the document. 
     * 
     * This method can be used to specify another reference URL. If this URL is not <samp>null</samp> then it will be used
     * instead of the system id.
     * 
     * The default value is <samp>null</samp>.
     * 
     * @param string $value The base URL for the document.
     */
    function setBaseURL($value) {
        if (!is_string($value)) { 
            die("<b>setBaseURL:</b> Value has to be of type string");
        } else {
            $this->conf_obj->baseURL = $value;
        }
    }
    
    /**
     * Enables or disables XSLT transformations.
     *  
     * Set this value to <samp>true</samp> to enable XSLT transformations or to <samp>false</samp> to disable
     * XSLT transformations. 
     * 
     * The default value is <samp>false</samp>.
     * 
     * @param boolean $value The new XSLT mode.
     */
    function setXSLTMode($value) {
        if (!is_bool($value)) { 
            die("<b>setXSLTMode:</b> Value has to be of type boolean");
        } else {
            $this->conf_obj->XSLTMode = $value;
        }
    }
    
    /**
     * Sets the encoding of the document.
     * 
     * If this value is set to <samp>null</samp> or it is empty then the encoding will be detected
     * automatically. 
     * 
     * The default value is <samp>null</samp>.
     * 
     * @param string $value The encoding of the document or <samp>null</samp> for autodetection.
     */
    function setEncoding($value) {
        if (!is_string($value)) { 
            die("<b>setEncoding:</b> Value has to be of type string");
        } else {
            $this->conf_obj->encoding = $value;
        }
    }
    
    /**
     * Sets the cleanup tool to use for documents with unparsable content.
     * 
     * The <samp>CLEANUP_</samp> constants can be used as value. 
     * The default value specified is {@link CLEANUP_CYBERNEKO}.
     * 
     * The cleanup tool is only used for {@link DOCTYPE_XHTML}. HTML5 utilizes an internal cleanup.
     * 
     * {@link CLEANUP_NONE}<br/>
     * {@link CLEANUP_JTIDY}<br/>
     * {@link CLEANUP_CYBERNEKO}<br/>
     * {@link CLEANUP_TAGSOUP}<br/>
     * 
     * @param int $value The new cleanup tool.
     */
    function setCleanupTool($value) {
        if (!is_numeric($value)) { 
            die("<b>setCleanup:</b> Value has to be of type int");
        } else {
            $this->conf_obj->cleanupTool = $value;
        }
    }
    
    /**
     * Sets the JavaScript mode.
     * 
     * {@link JAVASCRIPT_MODE_ENABLED}<br/>
     * {@link JAVASCRIPT_MODE_DISABLED}<br/>
     * {@link JAVASCRIPT_MODE_ENABLED_NO_LAYOUT}<br/>
     * {@link JAVASCRIPT_MODE_ENABLED_REAL_TIME}<br/>
     * {@link JAVASCRIPT_MODE_ENABLED_TIME_LAPSE}<br/>
     * 
     * The default value specified is {@link JAVASCRIPT_MODE_DISABLED}.
     * 
     * @param int $value The JavaScript mode.
     */
    function setJavaScriptMode($value) {
        if (!is_numeric($value)) { 
            die("<b>setJavaScriptMode:</b> Value has to be of type int");
        } else {
            $this->conf_obj->javaScriptMode = $value;
        }
    }
    
    /**
     * Sets the HTTPS mode.
     * 
     * In closed environment lenient can be the preferred setting to avoid HTTPS issues that are not security critical.
     * 
     * {@link HTTPS_MODE_LENIENT}<br/>
     * {@link HTTPS_MODE_STRICT}<br/>
     * 
     * The default value specified is {@link HTTPS_MODE_STRICT}.
     * 
     * @param int $value The HTTPS mode.
     */
    function setHTTPSMode($value) {
        if (!is_numeric($value)) { 
            die("<b>setHTTPSMode:</b> Value has to be of type int");
        } else {
            $this->conf_obj->httpsMode = $value;
        }
    }
    
    /**
     * Sets the document type. 
     * 
     * Use one of the following <samp>DOCTYPE_</samp> constants to specify the document type:
     * 
     * {@link DOCTYPE_AUTODETECT}<br/>
     * {@link DOCTYPE_XML}<br/>
     * {@link DOCTYPE_XHTML}<br/>
     * {@link DOCTYPE_HTML5}<br/>
     * 
     * The default value specified is {@link DOCTYPE_AUTODETECT}.
     * 
     * @param int $value The new document type setting.
     */
    function setDocumentType($value) {
        if (!is_numeric($value)) { 
            die("<b>setDocumentType:</b> Value has to be of type int");
        } else {
            $this->conf_obj->documentType = $value;
        }
    }
    
    /**
     * Sets the document type after the XSL-Transformations have been applied.
     * 
     * The <samp>DOCTYPE_</samp> constants can be used to specify the document type. 
     * 
     * The default value specified is {@link DOCTYPE_AUTODETECT}.
     * 
     * @param int $value The new document type setting.
     * 
     * @see DOCTYPE_AUTODETECT
     * @see DOCTYPE_XML
     * @see DOCTYPE_XHTML
     * @see DOCTYPE_HTML5
     * @see setDocumentType()
     */
    function setPostTransformationDocumentType($value) {
        if (!is_numeric($value)) { 
            die("<b>setPostTransformationDocumentType:</b> Value has to be of type int");
        } else {
            $this->conf_obj->postTransformationDocumentType = $value;
        }
    }
    
    //-----------------------------------------
    // Document Default Language
    //-----------------------------------------
    
    /**
     * Sets the language used for documents having no explicit language attribute set.
     * The language code is used to resolve the lang() selector correct and
     * to determine the correct language used for hyphenation.
     * 
     * @param string $languageCode the default ISO 639 language code used for documents.
     */
    function setDocumentDefaultLanguage($languageCode) {
        if (!is_string($languageCode)) { 
            die("<b>setDocumentDefaultLanguage:</b> langauageCode has to be of type sting");
        } else {
            $this->conf_obj->documentDefaultLanguage = $languageCode;
        }
    }
    
    //-----------------------------------------
    // XML document properties - Part 2
    //-----------------------------------------
    
    /**
     * Adds a user style sheet to the document. 
     * 
     * There are two ways to specify the style sheet:<br/>
     * <ol>
     * <li>Specifying the stylesheet only using an URI.</li>
     * <li>Specifying the stylesheet by the content of the style sheet and 
     * alternatively setting a URI to resolve relative elements. If no URI 
     * is specified then the system id/base URL of the document will be used.</li>
     * </ol>
     * 
     * @param string $content The content of the style sheet.
     * @param string $media The media type of the style sheet.
     * @param string $title The title of the stylesheet.
     * @param string $uri The URI of the style sheet.
     * 
     * @see removeAllUserStyleSheets()
     */
    function addUserStyleSheet($content, $media, $title, $uri) {
        $this->style_obj = new PDFreactorWebserviceStyleSheetData($content, $media, $title, $uri);
        
        if (!isset($this->conf_obj->userStyleSheets)) {
            $this->conf_obj->userStyleSheets = array($this->style_obj);
        } else {
            array_push($this->conf_obj->userStyleSheets,$this->style_obj);
        }
        $this->style_obj = NULL;
    }
    
    /**
     * Removes all user style sheets.
     * 
     * @see addUserStyleSheets()
     */
    function removeAllUserStyleSheets() {
        $this->conf_obj->userStyleSheets = NULL;
    }
    
    /**
     * Adds a XSLT style sheet to the document. 
     * 
     * There are two ways to specify the style sheet:<br/>
     * <ol>
     * <li>Specifying the stylesheet only using an URI.</li>
     * <li>Specifying the stylesheet by the content of the stylesheet and alternatively setting a URI to
     * resolve relative elements. If no URI is specified then the system id/base URL of the document will be
     * used.</li>
     * </ol>
     * 
     * @param string $content The content of the style sheet.
     * @param string $uri The URI of the style sheet.
     * 
     * @see removeAllXSLTStyleSheets()
     */
    function addXSLTStyleSheet($content, $uri) {
        $this->style_obj = new PDFreactorWebserviceXSLTStyleSheetData($content, $uri);
        
        if (!isset($this->conf_obj->XSLTStyleSheets))
        {
            $this->conf_obj->XSLTStyleSheets = array($this->style_obj);
        }
        else {array_push($this->conf_obj->XSLTStyleSheets,$this->style_obj);}
        $this->style_obj = NULL;
    }
    
    /**
     * Removes all XSLT style sheets.
     * 
     * @see addXSLTStyleSheet()
     */
    function removeAllXSLTStyleSheets() {
        $this->conf_obj->XSLTStyleSheets = NULL;
    }
    
    /**
     * Adds an user script to the document.
     * 
     * There are two ways to specify the script:<br/>
     * 
     * <ol>
     * <li>Specifying the script only by an URI.</li>
     * <li>Specifying the script by the content of the script and
     * alternatively setting a URI to resolve relative elements. If no URI
     * is specified then the system id/base URL of the document will be used.
     * </li>
     * </ol>
     * 
     * @param string $content The content of the script.
     * @param string $uri The URI of the script.
     * @param boolean $beforeDocumentScripts Use <samp>true</samp> to cause PDFreactor to run the
     * script before all scripts inside the document and <samp>false</samp>
     * to run it after.
     * 
     * @see removeAllUserScripts()
     */
    function addUserScript($content, $uri, $beforeDocumentScripts) {
        $this->script_obj = new PDFreactorWebserviceUserScriptData($content, $uri, $beforeDocumentScripts);
        
        if (!isset($this->conf_obj->userScripts))
        {
            $this->conf_obj->userScripts = array($this->script_obj);
        }
        else {array_push($this->conf_obj->userScripts,$this->script_obj);}
        $this->script_obj = NULL;
    }
    
    /**
     * Removes all user scripts.
     * 
     * @see addUserScript()
     */
    function removeAllUserScripts() {
        $this->conf_obj->UserScripts = NULL;
    }
    
    //=========================
    // PDF document properties
    //=========================
    
    /**
     * Sets the value of the author field of the PDF document.
     * 
     * @param string $value The author of the document.
     */
    function setAuthor($value) {
        if (!is_string($value)) { 
            die("<b>setAuthor:</b> Value has to be of type string");
        } else {
            $this->conf_obj->author = $value;
        }
    }
    
    /**
     * Sets the value of creator field of the PDF document.
     * 
     * @param string $value The creator of the document.
     */
    function setCreator($value) {
        if (!is_string($value)) { 
            die("<b>setCreator:</b> Value has to be of type string");
        } else {
            $this->conf_obj->creator = $value;
        }
    }
    
    /**
     * Sets the value of the keywords field of the PDF document.
     * 
     * @param string $value The keywords of the document.
     */
    function setKeywords($value) {
        if (!is_string($value)) { 
            die("<b>setKeywords:</b> Value has to be of type string");
        } else {
            $this->conf_obj->keywords = $value;
        }
    }
    
    /**
     * Sets the value of the title field of the PDF document.
     * 
     * @param string $value The title of the document.
     */
    function setTitle($value) {
        if (!is_string($value)) { 
            die("<b>setTitle:</b> Value has to be of type string");
        } else {
            $this->conf_obj->title = $value;
        }
    }
    
    /**
     * Sets the value of the subject field of the PDF document.
     * 
     * @param string $value The subject of the document.
     */
    function setSubject($value) {
        if (!is_string($value)) { 
            die("<b>setSubject:</b> Value has to be of type sting");
        } else {
            $this->conf_obj->subject = $value;
        }
    }
    
    /**
     * Adds a custom property to the PDF document. An existing property 
     * of the same name will be replaced.
     * 
     * @param string $name The name of the property.
     * @param string $value The value of the property. 
     */
    function addCustomDocumentProperty($name, $value) {
        $this->customDocumentPropertyArray = array($name, $value);
        
        if (!isset($this->conf_obj->customDocumentProperties)) {
            $this->conf_obj->customDocumentProperties = array($this->customDocumentPropertyArray);
        } else {
            array_push($this->conf_obj->customDocumentProperties,$this->customDocumentPropertyArray);
        }
        $this->customDocumentPropertyArray = NULL;
    }
    
    /**
     * Adds a file attachment to PDF document.
     * 
     * @param string $data The base64encoded binary content of the attachment. May be <samp>null</samp>.
     * @param string $url If <samp>data</samp> is <samp>null</samp>, the attachment will be retrieved from this <samp>URL</samp>. 
     *            If this is "#" the input document URL is used instead.
     * @param string $name The file name associated with the attachment. It is recommended to specify
     *             the correct file extension. If this is <samp>null</samp> the name is derived from the <samp>URL</samp>.
     * @param string $description The description of the attachment. If this is <samp>null</samp> the <samp>name</samp> is used.
     */
    function addAttachment($data, $url, $name, $description) {
        $this->attachment_obj = new PDFreactorWebserviceAttachmentData($data, $url, $name, $description);
        
        if (!isset($this->conf_obj->attachments)) {
            $this->conf_obj->attachments = array($this->attachment_obj);
        } else {
            array_push($this->conf_obj->attachments,$this->attachment_obj);
        }
        $this->attachment_obj = NULL;
    }
    
    /**
     * Sets the encryption.
     * 
     * Use one of the following <samp>ENCRYPTION_</samp> constants to specify the encryption. 
     * The default value is {@link ENCRYPTION_NONE}.<br/>
     * <br/>
     * {@link ENCRYPTION_NONE}<br/>
     * {@link ENCRYPTION_40}<br/>
     * {@link ENCRYPTION_128}<br/>
     * <br/>
     * 
     * @param int $value The new encryption setting.
     * 
     * @see setOwnerPassword()
     * @see setUserPassword()
     * @see setAllowAnnotations()
     * @see setAllowAssembly()
     * @see setAllowCopy()
     * @see setAllowDegradedPrinting()
     * @see setAllowFillIn()
     * @see setAllowModifyContents()
     * @see setAllowPrinting()
     * @see setAllowScreenReaders()
     */
    function setEncryption($value) {
        if (!is_numeric($value)) { 
            die("<b>setEncryption:</b> Value has to be of type int");
        } else {
            $this->conf_obj->encryption = $value;
        }
    }
    
    /**
     * Enables or disables full compression of the PDF document. 
     * 
     * The default value is <samp>false</samp>.
     * 
     * @param boolean $value <samp>true</samp> to enable full compression of the document and
     * <samp>false</samp> to disable full compression.
     */
    function setFullCompression($value) {
        if (!is_bool($value)) { 
            die("<b>setFullCompression:</b> Value has to be of type boolean");
        } else {
            $this->conf_obj->fullCompression = $value;
        }
    }
    
    /**
     * Sets the owner password of the PDF document.
     * 
     * The default value is <samp>null</samp>
     * 
     * @param string $value The owner password.
     * 
     * @see setEncryption()
     * @see setUserPassword()
     */
    function setOwnerPassword($value) {
        if (!is_string($value)) { 
            die("<b>setOwnerPassword:</b> Value has to be of type string");
        } else {
            $this->conf_obj->ownerPassword = $value;
        }
    }
    
    /**
     * Sets the user password of the PDF document.
     * 
     * The default value is <samp>null</samp>.
     * 
     * @param string $value The user password.
     * 
     * @see setEncryption()
     * @see setOwnerPassword()
     */
    function setUserPassword($value) {
        if (!is_string($value)) { 
            die("<b>setUserPassword:</b> Value has to be of type string");
        } else {
            $this->conf_obj->userPassword = $value;
        }
    }
    
    //-----------------------------------------
    // Authentication
    //-----------------------------------------
    
    /**
     * Enables access to resources that are secured via Basic or Digest authentication.
     * 
     * @param string $authenticationUsername The user name for the secured realm.
     * @param string $authenticationPassword The password for the secured realm.
     */
    function setAuthenticationCredentials($authenticationUsername, $authenticationPassword) {
        $this->conf_obj->authenticationUsername = $authenticationUsername;
        $this->conf_obj->authenticationPassword = $authenticationPassword;
    }
    
    //-----------------------------------------
    // Request headers and cookies
    //-----------------------------------------
    
    /**
     * Adds a request header to all outgoing HTTP connections. If the key already exists, the pair is overwritten.
     * 
     * @param string $key The key of the header.
     * @param string $value The value of the header.
     */
    function setRequestHeader($key, $value) {
        $this->requestHeaderArray = array($key, $value);
        
        if (!isset($this->conf_obj->requestHeaderMap)) {
            $this->conf_obj->requestHeaderMap = array($this->requestHeaderArray);
        }
        else {
            array_push($this->conf_obj->requestHeaderMap,$this->requestHeaderArray);
        }
        $this->requestHeaderArray = NULL;
    }
    
    /**
     * Adds a cookie to all outgoing HTTP connections. If the key already exists, the pair is overwritten.
     * 
     * @param string $key The key of the cookie.
     * @param string $value The value of the cookie.
     */
    function setCookie($key, $value) {
        $this->cookieArray = array($key, $value);
        
        if (!isset($this->conf_obj->cookies)) {
            $this->conf_obj->cookies = array($this->cookieArray);
        } else {
            array_push($this->conf_obj->cookies,$this->cookieArray);
        }
        $this->cookieArray = NULL;
    }
    
    //-----------------------------------------
    // PDF document properties - Part 2
    //-----------------------------------------
    
    /**
     * Enables or disables links in the PDF document.
     * 
     * The default value is <samp>false</samp>.
     * 
     * @param boolean $value Use <samp>true</samp> to enable links in the document and <samp>false</samp> to
     * disable links.
     */
    function setAddLinks($value) {
        if (!is_bool($value)) { 
            die("<b>setAddLinks:</b> Value has to be of type boolean");
        } else {
            $this->conf_obj->addLinks = $value;
        }
    }
    
    /**
     * Enables or disables bookmarks in the PDF document.
     * 
     * The default value is <samp>false</samp>.
     * 
     * @param boolean $value Use <samp>true</samp> to enable bookmarks in the document and <samp>false</samp>
     * to disable bookmarks.
     */
    function setAddBookmarks($value) {
        if (!is_bool($value)) { 
            die("<b>setAddBookmarks:</b> Value has to be of type boolean");
        } else {
            $this->conf_obj->addBookmarks = $value;
        }
    }
    
    /**
     * Enables or disables tagging of PDF document.
     * 
     * The default value is <samp>false</samp>.
     * 
     * @param boolean $value Use <samp>true</samp> to enable tagging of the document and
     * <samp>false</samp> to disable tagging.
     */
    function setAddTags($value) {
        if (!is_bool($value)) { 
            die("<b>setAddTags:</b> Value has to be of type boolean");
        } else {
            $this->conf_obj->addTags = $value;
        }
    }
    
    /**
     * Enables or disables embedding of image previews per page in the PDF document.
     * 
     * The default value is <samp>false</samp>.
     * 
     * @param boolean $value Use <samp>true</samp> to enable embedding of image previews per page in
     * the document and <samp>false</samp> to disable embedding of image previews per page.
     */
    function setAddPreviewImages($value) {
        if (!is_bool($value)) { 
            die("<b>setAddPreviewImages:</b> Value has to be of type boolean");
        } else {
            $this->conf_obj->addPreviewImages = $value;
        }
    }
    
    /**
     * Enables or disables attachments specified in style sheets.
     * 
     * The default value is <samp>false</samp>.
     * 
     * @param boolean $value Whether to enable CSS based attachments.
     */
    function setAddAttachments($value) {
        if (!is_bool($value)) { 
            die("<b>setAddAttachments:</b> Value has to be of type boolean");
        } else {
            $this->conf_obj->addAttachments = $value;
        }
    }
    
    /**
     * Enables or disables overprinting.
     * 
     * The default value is <samp>false</samp>.
     * 
     * @param boolean $value Whether to enable overprinting.
     */
    function setAddOverprint($value) {
        if (!is_bool($value)) { 
            die("<b>setAddOverprint:</b> Value has to be of type boolean");
        } else {
            $this->conf_obj->addOverprint = $value;
        }
    }
    
    /**
     * Enables or disables a print dialog to be shown upon opening the generated PDF document by a
     * PDF viewer.
     * 
     * The default value is <samp>false</samp>.
     * 
     * @param boolean $value Use <samp>true</samp> to enable a print dialog to be shown upon document
     * opening by a PDF viewer and <samp>false</samp> to disable the print dialog prompt.
     */
    function setPrintDialogPrompt($value) {
        if (!is_bool($value)) { 
            die("<b>setPrintDialogPromt:</b> Value has to be of type boolean");
        } else {
            $this->conf_obj->printDialogPrompt = $value;
        }
    }
    
    /**
     * Specifies whether or not the log data should be added to the
     * PDF document.
     * 
     * The default value is <samp>false</samp>.
     * 
     * @param boolean $value Use <samp>true</samp> to add the log data to the document and
     * <samp>false</samp> if the log data should not be added.
     */
    function setAppendLog($value) {
        if (!is_bool($value)) { 
            die("<b>setAppendLog:</b> Value has to be of type boolean");
        } else {
            $this->conf_obj->appendLog = $value;
        }
    }
    
    /**
     * Enables or disables the 'annotations' restriction in the PDF document.
     * 
     * The default value is <samp>false</samp>.
     * 
     * @param boolean $value Use <samp>true</samp> to enable 'annotations' in the document and
     * <samp>false</samp> to disable 'annotations'.
     * 
     * @see setEncryption()
     */
    function setAllowAnnotations($value) {
        if (!is_bool($value)) { 
            die("<b>setAllowAnnotations:</b> Value has to be of type boolean");
        } else {
            $this->conf_obj->allowAnnotations = $value;
        }
    }
    
    /**
     * Enables or disables the 'assembly' restriction in the PDF document.
     * 
     * The default value is <samp>false</samp>.
     * 
     * @param boolean $value Use <samp>true</samp> to enable 'assembly' in the document and
     * <samp>false</samp> to disable 'assembly'.
     * 
     * @see setEncryption()
     */
    function setAllowAssembly($value) {
        if (!is_bool($value)) { 
            die("<b>setAllowAssembly:</b> Value has to be of type boolean");
        } else {
            $this->conf_obj->allowAssembly = $value;
        }
    }
    
    /**
     * Enables or disables the 'copy' restriction in the PDF document.
     * 
     * The default value is <samp>false</samp>.
     * 
     * @param boolean $value Use <samp>true</samp> to enable 'copy' in the document and
     * <samp>false</samp> to disable 'copy'.
     * 
     * @see setEncryption()
     */
    function setAllowCopy($value) {
        if (!is_bool($value)) { 
            die("<b>setAllowCopy:</b> Value has to be of type boolean");
        } else {
            $this->conf_obj->allowCopy = $value;
        }
    }
    
    /**
     * Enables or disables the 'degraded printing' restriction in the PDF
     * document.
     * 
     * The default value is <samp>false</samp>.
     * 
     * @param boolean $value Use <samp>true</samp> to enable 'degraded printing' in the document and
     * <samp>false</samp> to disable 'degraded printing'.
     * 
     * @see setEncryption()
     */
    function setAllowDegradedPrinting($value) {
        if (!is_bool($value)) { 
            die("<b>setAllowDegradedPrinting:</b> Value has to be of type boolean");
        } else {
            $this->conf_obj->allowDegradedPrinting = $value;
        }
    }
    
     /**
     * Enables or disables the 'fill in' restriction in the PDF document.
     * 
     * The default value is <samp>false</samp>.
     * 
     * @param boolean $value Use <samp>true</samp> to enable 'fill in' in the document and
     * <samp>false</samp> to disable 'fill in'.
     * 
     * @see setEncryption()
     */
    function setAllowFillIn($value) {
        if (!is_bool($value)) { 
            die("<b>setAllowFillIn:</b> Value has to be of type boolean");
        } else {
            $this->conf_obj->allowFillIn = $value;
        }
    }
    
    /**
     * Enables or disables the 'modify contents' restriction in the PDF
     * document.
     * 
     * The default value is <samp>false</samp>.
     * 
     * @param boolean $value Use <samp>true</samp> to enable 'modify contents' in the document and
     * <samp>false</samp> to disable 'modify contents'.
     * 
     * @see setEncryption()
     */
    function setAllowModifyContents($value) {
        if (!is_bool($value)) { 
            die("<b>setAllowModifyContents:</b> Value has to be of type boolean");
        } else {
            $this->conf_obj->allowModifyContents = $value;
        }
    }
    
    /**
     * Enables or disables the 'printing' restriction in the PDF document.
     * 
     * The default value is <samp>false</samp>.
     * 
     * @param boolean $value Use <samp>true</samp> to enable 'printing' in the document and
     * <samp>false</samp> to disable 'printing'.
     * 
     * @see setEncryption()
     */
    function setAllowPrinting($value) {
        if (!is_bool($value)) { 
            die("<b>setAllowPrinting:</b> Value has to be of type boolean");
        } else {
            $this->conf_obj->allowPrinting = $value;
        }
    }
    
    /**
     * Enables or disables the 'screen readers' restriction in the PDF
     * document.
     * 
     * The default value is <samp>false</samp>.
     * 
     * @param boolean $value Use <samp>true</samp> to enable 'screen readers' in the document and
     * <samp>false</samp> to disable 'screen readers'.
     * 
     * @see setEncryption()
     */
    function setAllowScreenReaders($value) {
       if (!is_bool($value)) { 
           die("<b>setAllowScreenReaders:</b> Value has to be of type boolean");
       } else {
           $this->conf_obj->allowScreenReaders = $value;
       }
    }
    
    //-----------------------------------------
    // Processing Preferences
    //-----------------------------------------
    
    /**
     * Preferences that influence the conversion process 
     * without changing the output.
     * 
     * Use the <samp>PROCESSING_PREFERENCES_</samp> constants to specify the 
     * processing preferences. By default no processing preference is set.
     * 
     * {@link PROCESSING_PREFERENCES_SAVE_MEMORY_IMAGES}
     * 
     * @param int $processingPreferences A value of ORed processing preferences flags.
     */
    function setProcessingPreferences($processingPreferences) {
        if (!is_numeric($processingPreferences)) {
            die("<b>setProcessingPreferences:</b> Value has to be of type int");
        } else {
            $this->conf_obj->processingPreferences = $processingPreferences;
        }
    }
    
    //-----------------------------------------
    // Viewer Preferences, Conformance, 
    // Color Space, Output Intent
    //-----------------------------------------
    
    /**
     * Sets the page layout and page mode preferences of the PDF.
     * 
     * Use the <samp>VIEWER_PREFERENCES_</samp> constants to specify the 
     * viewer preferences. By default no viewer preference is set.
     * 
     * {@link VIEWER_PREFERENCES_CENTER_WINDOW}<br/>
     * {@link VIEWER_PREFERENCES_DIRECTION_L2R}<br/>
     * {@link VIEWER_PREFERENCES_DIRECTION_R2L}<br/>
     * {@link VIEWER_PREFERENCES_DISPLAY_DOC_TITLE}<br/>
     * {@link VIEWER_PREFERENCES_DUPLEX_FLIP_LONG_EDGE}<br/>
     * {@link VIEWER_PREFERENCES_DUPLEX_FLIP_SHORT_EDGE}<br/>
     * {@link VIEWER_PREFERENCES_DUPLEX_SIMPLEX}<br/>
     * {@link VIEWER_PREFERENCES_FIT_WINDOW}<br/>
     * {@link VIEWER_PREFERENCES_HIDE_MENUBAR}<br/>
     * {@link VIEWER_PREFERENCES_HIDE_TOOLBAR}<br/>
     * {@link VIEWER_PREFERENCES_HIDE_WINDOW_UI}<br/>
     * {@link VIEWER_PREFERENCES_NON_FULLSCREEN_PAGE_MODE_USE_NONE}<br/>
     * {@link VIEWER_PREFERENCES_NON_FULLSCREEN_PAGE_MODE_USE_OC}<br/>
     * {@link VIEWER_PREFERENCES_NON_FULLSCREEN_PAGE_MODE_USE_OUTLINES}<br/>
     * {@link VIEWER_PREFERENCES_NON_FULLSCREEN_PAGE_MODE_USE_THUMBS}<br/>
     * {@link VIEWER_PREFERENCES_PAGE_LAYOUT_ONE_COLUMN}<br/>
     * {@link VIEWER_PREFERENCES_PAGE_LAYOUT_SINGLE_PAGE}<br/>
     * {@link VIEWER_PREFERENCES_PAGE_LAYOUT_TWO_COLUMN_LEFT}<br/>
     * {@link VIEWER_PREFERENCES_PAGE_LAYOUT_TWO_COLUMN_RIGHT}<br/>
     * {@link VIEWER_PREFERENCES_PAGE_LAYOUT_TWO_PAGE_LEFT}<br/>
     * {@link VIEWER_PREFERENCES_PAGE_LAYOUT_TWO_PAGE_RIGHT}<br/>
     * {@link VIEWER_PREFERENCES_PAGE_MODE_FULLSCREEN}<br/>
     * {@link VIEWER_PREFERENCES_PAGE_MODE_USE_NONE}<br/>
     * {@link VIEWER_PREFERENCES_PAGE_MODE_USE_OC}<br/>
     * {@link VIEWER_PREFERENCES_PAGE_MODE_USE_OUTLINES}<br/>
     * {@link VIEWER_PREFERENCES_PAGE_MODE_USE_THUMBS}<br/>
     * {@link VIEWER_PREFERENCES_PICKTRAYBYPDFSIZE_FALSE}<br/>
     * {@link VIEWER_PREFERENCES_PICKTRAYBYPDFSIZE_TRUE}<br/>
     * {@link VIEWER_PREFERENCES_PRINTSCALING_APPDEFAULT}<br/>
     * {@link VIEWER_PREFERENCES_PRINTSCALING_NONE}<br/>
     * 
     * @param int $viewerPreferences A value of ORed <samp>VIEWER_PREFERENCES_</samp> constants.
     */
    function setViewerPreferences($viewerPreferences) {
        if (!is_numeric($viewerPreferences)) { 
            die("<b>viewerPreferences:</b> Value has to be of type int");
        } else {
            $this->conf_obj->viewerPreferences = $viewerPreferences;
        }
    }
    
    /**
     * Sets the conformance of the PDF.
     * 
     * The <samp>CONFORMANCE_</samp> constants can be used as value. The default 
     * value specified is {@link CONFORMANCE_PDF}.
     * 
     * {@link CONFORMANCE_PDF}<br/>
     * {@link CONFORMANCE_PDFA}<br/>
     * 
     * @param int $conformance conformance constants.
     */
    function setConformance($conformance) {
        if (!is_numeric($conformance)) { 
            die("<b>conformance:</b> Value has to be of type int");
        } else {
            $this->conf_obj->conformance = $conformance;
        }
    }
    
    /**
     * Sets whether to convert color key words to CMYK instead of RGB.
     * 
     * The <samp>COLOR_SPACE_</samp> constants can be used as value.
     * The default value specified is {@link COLOR_SPACE_RGB}.
     * 
     * {@link COLOR_SPACE_CMYK}<br/>
     * {@link COLOR_SPACE_RGB}<br/> 
     * 
     * @param $defaultColorSpace The default color space of the PDF
     */
    function setDefaultColorSpace($defaultColorSpace) {
        if (!is_int($preferCmyk)) { 
            die("<b>setDefaultColorSpace:</b> Value has to be of type int");
        } else {
            $this->conf_obj->defaultColorSpace = $defaultColorSpace;
        }
    }
    
    /**
     * Sets the ouput intent including the identifier and the ICC profile to be embedded into the PDF.
     * 
     * @param string $identifier The OutputConditionIdentifier
     * @param string $profileUrl The DestOutputProfile
     */
    function setOutputIntentFromURL($identifier, $profileUrl) {
        $this->conf_obj->outputIntentIdentifier = $identifier;
        $this->conf_obj->outputIntentICCProfileURL = $profileUrl;
        $this->conf_obj->outputIntentICCProfileData = NULL;
    }
    
    /**
     * Sets the ouput intent including the identifier and the ICC profile to be embedded into the PDF.
     * 
     * @param string $identifier The OutputConditionIdentifier
     * @param string $profileData The base64encoded binary content of the DestOutputProfile
     */
    function setOutputIntentFromByteArray($identifier, $profileData) {
        $this->conf_obj->outputIntentIdentifier = $identifier;
        $this->conf_obj->outputIntentICCProfileURL = NULL;
        $this->conf_obj->outputIntentICCProfileData = $profileData;
    }
    
    //-----------------------------------------
    // Font Embedding, Font Fallback
    // Pixel Per Inch
    //-----------------------------------------
    
    /**
     * Sets whether fonts will not be embedded into the resulting PDF. 
     * Setting this to true will reduce the file size of the output document.
     * However, the resulting PDF documents are no longer guaranteed to look 
     * identical on all systems.
     * 
     * @param boolean $disableFontEmbedding
     * 
     * The default value is <samp>false</samp>.
     */
    function setDisableFontEmbedding($disableFontEmbedding) {
        if (!is_bool($disableFontEmbedding)) { 
            die("<b>setDisableFontEmbedding:</b> Value has to be of type boolean");
        } else {
            $this->conf_obj->disableFontEmbedding = $disableFontEmbedding;
        }
    }
    
    /**
     * Sets a list of fallback font families used for character substitution. This 
     * list is iterated for characters that can not be displayed with any of the fonts 
     * specified via the CSS property <samp>font-family</samp>.
     * 
     * @param array $fontArray An array of font family names
     */
    function setFontFallback($fontArray) {
        if (!is_array($fontArray)) { 
            die("<b>setFontFallback:</b> Value has to be of type array");
        } else {
            $this->conf_obj->fontFallback = $fontArray;
        }
    }
    
    /**
     * Sets the pixels per inch. 
     * Changing this value changes the physical length of sizes specified in px (including those 
     * specified via HTML attributes).
     * 
     * @param int $ppi
     * 
     * The default value is <samp>96ppi</samp>.</p>
     */
    function setPixelsPerInch($ppi) {
        if (!is_numeric($ppi)) { 
            die("<b>setPixelsPerInch:</b> Value has to be of type int");
        } else {
            $this->conf_obj->pixelsPerInch = $ppi;
        }
    }
    
    /**
     * Whether the pixels per inch should be adapted automatically to avoid content exceeding pages.
     * 
     * @param boolean $pixelsPerInchShrinkToFit
     */
    function setPixelsPerInchShrinkToFit($pixelsPerInchShrinkToFit) {
        if (!is_bool($pixelsPerInchShrinkToFit)) { 
            die("<b>setPixelsPerInchShrinkToFit:</b> Value has to be of type int");
        } else {
            $this->conf_obj->pixelsPerInchShrinkToFit = $pixelsPerInchShrinkToFit;
        }
    }
    
    //-----------------------------------------
    // Page Order, Page Per Sheet, 
    // Booklet Mode
    //-----------------------------------------
    
    /**
     * Sets the page order of the direct result of the conversion.
     * 
     * {@link PAGE_ORDER_EVEN}<br/>
     * {@link PAGE_ORDER_ODD}<br/>
     * {@link PAGE_ORDER_BOOKLET}<br/>
     * {@link PAGE_ORDER_BOOKLET_RTL}<br/>
     * 
     * @param string $order A comma-separated list of page numbers and ranges is 
     *              intended as parameter. Alternatively, <samp>PAGE_ORDER_</samp> 
     *              constants can be used.
     */
    function setPageOrder($order) {
        if (!is_string($order)) { 
            die("<b>setPageOrder:</b> Value has to be of type string");
        } else {
            $this->conf_obj->pageOrder = $order;
        }
    }
    
    /**
     * Sets the properties of a sheet on which multiple pages are being arranged.
     * 
     * If <samp>cols</samp> or <samp>rows</samp> is less than 1, no 
     * pages-per-sheet processing is done. This is the case by default.
     * 
     * Use one of the <samp>PAGES_PER_SHEET_DIRECTION_</samp> constants to 
     * specify the the direction. The default value is {@link PAGES_PER_SHEET_DIRECTION_RIGHT_DOWN}.
     * 
     * {@link PAGES_PER_SHEET_DIRECTION_DOWN_LEFT}<br/>
     * {@link PAGES_PER_SHEET_DIRECTION_DOWN_RIGHT}<br/>
     * {@link PAGES_PER_SHEET_DIRECTION_LEFT_DOWN}<br/>
     * {@link PAGES_PER_SHEET_DIRECTION_LEFT_UP}<br/>
     * {@link PAGES_PER_SHEET_DIRECTION_RIGHT_DOWN}<br/>
     * {@link PAGES_PER_SHEET_DIRECTION_RIGHT_UP}<br/>
     * {@link PAGES_PER_SHEET_DIRECTION_UP_LEFT}<br/>
     * {@link PAGES_PER_SHEET_DIRECTION_UP_RIGHT}<br/>
     * 
     * @param int $cols The number of columns per sheet.
     * @param int $rows The number of rows per sheet.
     * @param string $sheetSize The sheet size as CSS size, e.g. <samp>A4</samp>, <samp>letter landscape</samp>, <samp>15in 20in</samp>,
     *  <samp>20cm 30cm</samp>.
     * @param string $sheetMargin The sheet margin as CSS margin, e.g. <samp>1in</samp>, <samp>1cm 1.5cm</samp>, <samp>10mm 20mm 10mm 30mm</samp>. <samp>null</samp> is 
     * interpreted as <samp>0mm</samp>.
     * @param string $spacing The horizontal and vertical space between pages on a sheet as CSS value, e.g. <samp>0.1in</samp>, <samp>5mm 2mm</samp>. <samp>null</samp> is 
     * interpreted as <samp>0mm</samp>.
     * @param int $direction The direction in which the pages are ordered on a sheet.
     */
    function setPagesPerSheetProperties($cols, $rows, $sheetSize, $sheetMargin, $spacing, $direction) {
        $this->conf_obj->pagesPerSheetCols = $cols;
        $this->conf_obj->pagesPerSheetRows = $rows;
        $this->conf_obj->pagesPerSheetSheetSize = $sheetSize;
        $this->conf_obj->pagesPerSheetSheetMargin = $sheetMargin;
        $this->conf_obj->pagesPerSheetSpacing = $spacing;
        $this->conf_obj->pagesPerSheetDirection = $direction;
    }
    
    /**
     * Convenience method to set pages-per-sheet properties and page order in one step to create a booklet.
     * 
     * @param string $sheetSize The size of the sheet as CSS value, e.g. <samp>A3</samp>, <samp>letter landscape</samp>, <samp>15in 20in</samp>,
     *  <samp>20cm 30cm</samp>.
     * @param string $sheetMargin The sheet margin as CSS margin, e.g. <samp>1in</samp>, <samp>1cm 1.5cm</samp>, <samp>10mm 20mm 10mm 30mm</samp>. <samp>null</samp> is 
     * interpreted as <samp>0mm</samp>.
     * @param boolean $rtl Whether or not the reading order of the booklet should be right-to-left .
     */
    function setBookletMode($sheetSize, $sheetMargin, $rtl) {
        $this->conf_obj->bookletSheetSize = $sheetSize;
        $this->conf_obj->bookletRTL = $rtl;
        $this->conf_obj->bookletSheetMargin = $sheetMargin;
        $this->conf_obj->bookletModeEnabled = true;
    }
    
    //-----------------------------------------
    // Merge methods, Overlay Repeat
    //-----------------------------------------
    
    /**
     * This methods sets a URL of an external PDF document which will be merged with the PDF
     * document generated by the XML source.
     * 
     * The default value is <samp>null</samp>
     * 
     * @param string $url A URL of a PDF document.
     * 
     * @see setMergeURLs()
     * @see setMergeByteArray()
     * @see setMergeByteArrays()
     * @see setMergeMode()
     */
    function setMergeURL($url) {
        if (!is_string($url)) { 
            die("<b>setMergeURL:</b> Value has to be of type string");
        } else {
            $this->conf_obj->mergeURL = $url;
        }
    }
    
    /**
     * This method sets an array of URLs of external PDF documents which will be
     * merged with the PDF document generated by the XML source.
     * 
     * @param array $urlArray An array of URL strings
     * 
     * @see setMergeURL()
     * @see setMergeByteArray()
     * @see setMergeByteArrays()
     * @see setMergeMode()
     */
    function setMergeURLs($urlArray) {
        if (!is_array($urlArray)) { 
            die("<b>setMergeURLs:</b> Value has to be of type array");
        } else {
            $this->conf_obj->mergeURLs = $urlArray;
        }
    }
    
    /**
     * This methods sets a byte array containing an external PDF document which will be merged with
     * the PDF document generated by the XML source.
     * 
     * The default value is <samp>null</samp>
     * 
     * @param string $base64string A base64encoded binary string containing the PDF RAW data.
     * 
     * @see setMergeByteArrays()
     * @see setMergeURL()
     * @see setMergeURLs()
     * @see setMergeMode()
     */
    function setMergeByteArray($base64string) {
        if (preg_match("/%PDF/", $base64string)) {
            $base64string = base64_encode($base64string);
        }
        $this->conf_obj->mergeByteArray = $base64string;
    }
    
    /**
     * This method sets an array of byte arrays that contain multiple external PDFs which will be merged with
     * the PDF document generated by the XML source.
     * 
     * The default value is <samp>null</samp>
     * 
     * @param array $byteArrays An array of base64encoded binary strings containing the PDF RAW data.
     * 
     * @see setMergeByteArray()
     * @see setMergeURL()
     * @see setMergeURLs()
     * @see setMergeMode()
     */
    function setMergeByteArrays($byteArrays) {
        if (!is_array($byteArrays)) { 
            die("<b>setMergeByteArrays:</b> Value has to be of type array");
        } else {
            for ($i=0; $i<count($byteArrays); $i++) {
                if (preg_match("/%PDF/", $byteArrays[$i])) {
                    $byteArrays[$i] = base64_encode($byteArrays[$i]);
                }
            }
            $this->conf_obj->mergeByteArrays = $byteArrays;
        }
    }
    
    /**
     * @deprecated  As of PDFreactor 5.0, replaced by {@link setMergeMode()}
     */
    function setMergeBeforePDF($mergeXMLBeforePDF) {
        if (!is_bool($mergeXMLBeforePDF)) { 
            die("<b>setMergeBeforePDF:</b> Value has to be of type boolean");
        } else {
            if ($mergeXMLBeforePDF) {
                $this->setMergeMode(MERGE_MODE_PREPEND);
            } else {
                $this->setMergeMode(MERGE_MODE_APPEND);
            }
        }
    }
    
    /**
     * Sets the merge mode.
     * 
     * The following merge methods can be used:
     * 
     * <ul>
     *  <li>append ({@link MERGE_MODE_APPEND})</li>
     *  <li>prepend  ({@link MERGE_MODE_PREPEND})</li>
     *  <li>overlay above the content of the generated PDF ({@link MERGE_MODE_OVERLAY})</li>
     *  <li>overlay below the content of the generated PDF ({@link MERGE_MODE_OVERLAY_BELOW})</li>
     * </ul>
     * 
     * @param int $mergeMode The merge mode.
     * 
     * The default value is {@link MERGE_MODE_APPEND}.
     */
    function setMergeMode($mergeMode) {
        if (!is_numeric($mergeMode)) { 
            die("<b>setMergeMode:</b> Value has to be of type int");
        } else {
            $this->conf_obj->mergeMode = $mergeMode;
        }
    }
    
    /**
     * If one of the documents of an overlay process is shorter than the other, this method 
     * allows repeating either its last page or all of its pages in order to overlay all pages of 
     * the longer document.
     * 
     * Use one of the <samp>OVERLAY_REPEAT_</samp> constants to specify the 
     * overlay repeat. The default value is {@link OVERLAY_REPEAT_NONE}.
     * 
     * {@link OVERLAY_REPEAT_NONE}<br/>
     * {@link OVERLAY_REPEAT_LAST_PAGE}<br/>
     * {@link OVERLAY_REPEAT_ALL_PAGES}<br/>
     * 
     * @param int $overlayRepeat
     */
    function setOverlayRepeat($overlayRepeat) {
        if (!is_numeric($overlayRepeat)) { 
            die("<b>setOverlayRepeat:</b> Value has to be of type int");
        } else {
            $this->conf_obj->overlayRepeat = $overlayRepeat;
        }
    }
    
    //-----------------------------------------
    // Digitally Signing
    //-----------------------------------------
    
    /**
     * Sets a digital certificate to sign the newly created PDF.
     * 
     * Requires a keystore file. The included certificate may be self-signed.
     * 
     * Use the <samp>KEYSTORE_TYPE_</samp> constants to specify the keystore type.
     * 
     * {@link KEYSTORE_TYPE_JKS}<br/>
     * {@link KEYSTORE_TYPE_PKCS12}<br/>
     * 
     * Use the <samp>SIGNING_MODE_</samp> constants to specify the signing mode.
     * 
     * {@link SIGNING_MODE_SELF_SIGNED}<br/>
     * {@link SIGNING_MODE_VERISIGN_SIGNED}<br/>
     * {@link SIGNING_MODE_WINCER_SIGNED}<br/>
     * 
     * @param string $keystoreURL The URL to the keystore file.
     * @param string $keyAlias The alias of the certificate included in the keystore to be used to sign the PDF.
     * @param string $keystorePassword The password of the keystore.
     * @param string $keystoreType The format of the keystore, i.e. <samp>pkcs12</samp> or <samp>jks</samp>.
     * @param int $signingMode The mode that is used to sign the PDF, i.e. <samp>self-signed</samp>, <samp>Windows certificate</samp> or <samp>VeriSign</samp>.
     */
    function setSignPDF($keystoreURL, $signPdfKeyAlias, $keystorePassword, $keystoreType, $signingMode) {
        $this->conf_obj->signPdfKeystoreURL = $keystoreURL;
        $this->conf_obj->signPdfKeyAlias = $signPdfKeyAlias;
        $this->conf_obj->signPdfKeystorePassword = $keystorePassword;
        $this->conf_obj->signPdfKeystoreType = $keystoreType;
        $this->conf_obj->signPdfSigningMode = $signingMode;
    }
    
    //-----------------------------------------
    // Output Format
    //-----------------------------------------
    
    /**
     * Sets the output format. The default value is <samp>PDF</samp>. 
     * For image formats the width or height in pixels must be specified. 
     * When either dimension is <1 it is computed based on the other dimension and the aspect ratio of the input document.
     * 
     * @param int $format The output format. See <samp>OUTPUT_FORMAT_</samp> constants
     * @param int $outputWidth The with of the output in pixels (image formats only). Values <1 will be computed based on the specified height and the aspect ratio of the input document.
     * @param int $outputHeight The height of the output in pixels (image formats only). Values <1 will be computed based on the specified width and the aspect ratio of the input document.
     * 
     * @see OUTPUT_FORMAT_PDF
     * @see OUTPUT_FORMAT_JPEG
     * @see OUTPUT_FORMAT_PNG
     * @see OUTPUT_FORMAT_PNG_TRANSPARENT
     * @see OUTPUT_FORMAT_BMP
     * @see OUTPUT_FORMAT_GIF
     * @see OUTPUT_FORMAT_PNG_AI
     * @see OUTPUT_FORMAT_PNG_TRANSPARENT_AI
     * @see OUTPUT_FORMAT_TIFF_LZW
     * @see OUTPUT_FORMAT_TIFF_PACKBITS
     * @see OUTPUT_FORMAT_TIFF_UNCOMPRESSED
     * @see OUTPUT_FORMAT_TIFF_CCITT_1D
     * @see OUTPUT_FORMAT_TIFF_CCITT_GROUP_3
     * @see OUTPUT_FORMAT_TIFF_CCITT_GROUP_4
     */
   function setOutputFormat($format, $outputWidth, $outputHeight) {
        $this->conf_obj->outputFormat = $format;
        $this->conf_obj->outputWidth = $outputWidth;
        $this->conf_obj->outputHeight = $outputHeight;
    }
    
    /**
     * Enables the conversion of the input document into one image. 
     * @param int $width Equivalent to the width of a browser window (view port). Values <1 enable paginated output
     * @param int $height Equivalent to the height of a browser window (view port). For values <1 the entire height of the document is used.
     */
    function setContinuousOutput($width, $height) {
        $this->conf_obj->continuousWidth = $width;
        $this->conf_obj->continuousHeight = $height;
    }
    
    //-----------------------------------------
    // Render methods
    //-----------------------------------------
    
    /**
     * Generates a PDF document or an image from an HTML or XML document at a URL.
     * 
     * @param string $url A URL pointing to the XML document.
     * @return binary The PDF document as PDF RAW data.
     * 
     * @see renderDocumentFromByteArray()
     * @see renderDocumentFromContent()
     */
    function renderDocumentFromURL($url) {
        
        $arguments = array("in0" => $url, "in1" => $this->conf_obj);
        $this->result = $this->client->ROCall("renderDocumentFromURL",$arguments);
                
        if (isset($this->result)) {
            if (isset($this->result->message)) {
                die($this->result->message."(Possibly Jetty is not running!)");
            } else {
                return $this->result->PDF;
            }
        } else {
            die("No result was received");
        }
    }
    
    /**
     * Generates an array of byte-arrays each containing an image representing one page of the document from an HTML or XML document at a URL.
     *  
     * @param string $url A URL pointing to the input document.
     * 
     * @return array An array of byte-arrays each containing an image representing one page of the document
     * 
     * @see renderDocumentFromByteArrayAsArray()
     * @see renderDocumentFromContentAsArray()
     * @see renderDocumentFromURL()
     */
    function renderDocumentFromURLAsArray($url) {
        $arguments = array("in0" => $url, "in1" => $this->conf_obj);
        $this->result = $this->client->ROCall("renderDocumentFromURLAsArray",$arguments);
                
        if (isset($this->result)) {
            if (isset($this->result->message)) {
                die($this->result->message."(Possibly Jetty is not running!)");
            } else {
                return $this->result->outputArray;
            }
        } else {
            die("No result was received");
        }
    }
    
    /**
     * Generates a PDF document or an image from an HTML or XML document inside a <samp>byte</samp> array.
     * 
     * @param string $byteArray A base64encoded binary string containing the content of the XML
     * document.
     * 
     * @return binary The PDF document as PDF RAW data.
     * 
     * @see renderDocumentFromURL()
     * @see renderDocumentFromContent()
     * @see renderDocumentFromByteArrayAsArray()
     */
    function renderDocumentFromByteArray($byteArray) {
                    
        $arguments = array("in0" => $byteArray, "in1" => $this->conf_obj);
        $this->result = $this->client->ROCall("renderDocumentFromByteArray",$arguments);
                
        if (isset($this->result)) {
            if (isset($this->result->message)) {
                die($this->result->message."(Possibly Jetty is not running!)");
            } else {
                return $this->result->PDF;
            }
        } else {
            die("No result was received");
        }
    }
    
    /**
     * Generates an array of byte-arrays each containing an image representing one page of the document inside a <samp>byte</samp> array.
     * 
     * @param string $byteArray A base64encoded binary string containing the content of the XML
     * document.
     * 
     * @return array An array of byte-arrays each containing an image representing one page of the document
     * 
     * @see renderDocumentFromURLAsArray()
     * @see renderDocumentFromContentAsArray()
     * @see renderDocumentFromByteArray()
     */
    function renderDocumentFromByteArrayAsArray($byteArray) {
                    
        $arguments = array("in0" => $byteArray, "in1" => $this->conf_obj);
        $this->result = $this->client->ROCall("renderDocumentFromByteArrayAsArray",$arguments);
                
        if (isset($this->result)) {
            if (isset($this->result->message)) {
                die($this->result->message."(Possibly Jetty is not running!)");
            } else {
                return $this->result->outputArray;
            }
        } else {
            die("No result was received");
        }
    }
    
    /**
     * Generates a PDF document or an image from an HTML or XML document inside a String.
     * 
     * @param string $content A string containing the content of the XML document.
     * 
     * @return binary The PDF document as PDF RAW data.
     * 
     * @see renderDocumentFromByteArray()
     * @see renderDocumentFromURL()
     * @see renderDocumentFromContentAsArray()
     */
    function renderDocumentFromContent($content) {
                
        $arguments = array("in0" => $content, "in1" => $this->conf_obj);
        $this->result = $this->client->ROCall("renderDocumentFromContent",$arguments);
                
        if (isset($this->result)) {
            if (isset($this->result->message)) {
                die($this->result->message."(Possibly Jetty is not running!)");
            } else {
                return $this->result->PDF;
            }
        } else {
            die("No result was received");
        }
    }
    
    /**
     * Generates an array of byte-arrays each containing an image representing one page of the document from an HTML or XML document inside a String.
     * 
     * @param string $content A string containing the content of the XML document.
     * 
     * @return array An array of byte-arrays each containing an image representing one page of the document
     * 
     * @see renderDocumentFromByteArrayAsArray()
     * @see renderDocumentFromURLAsArray()
     * @see renderDocumentFromContent()
     */
    function renderDocumentFromContentAsArray($content) {
                
        $arguments = array("in0" => $content, "in1" => $this->conf_obj);
        $this->result = $this->client->ROCall("renderDocumentFromContentAsArray",$arguments);
                
        if (isset($this->result)) {
            if (isset($this->result->message)) {
                die($this->result->message."(Possibly Jetty is not running!)");
            } else {
                return $this->result->outputArray;
            }
        } else {
            die("No result was received");
        }
    }

    //-----------------------------------------
    // Logging methods
    //-----------------------------------------
    
    /**
     * Returns the error messages generated during rendering.
     * 
     * @return string The message string.
     */
    function getError() {
        return $this->result->error;
    }
    
    /**
     * Returns the log messages generated during rendering based on the log level.
     * 
     * @return string The log string.
     * 
     * @see setLogLevel()
     */
    function getLog() {
        return $this->result->log;
    }
    
    /**
     * Provides information about content exceeding its page or parent. Depends on the mode set via {@link setLogExceedingContent()}.
     * 
     * @return array Array of {@link ExceedingContent} objects 
     * or <samp>null</samp> if the logging of exceeding content was not enabled 
     * or no conversion was run, yet.
     */
    function getExceedingContents() {
        return $this->result->exceedingContents;
    }
    
    /**
     * Returns the number of pages of the document after conversion.
     * 
     * The result returned by this method will only be correct if the 
     * document has already been laid out by one of the render methods.
     * 
     * {@link renderDocument()}
     * {@link renderDocumentFromByteArray()}
     * {@link renderDocumentFromContent()}
     * {@link renderDocumentFromURL()}
     * 
     * @param boolean $pdf If <samp>true</samp>, return the number of pages of the resulting output
     * (including, e.g. merge operations), otherwise it will return the number pages of the laid out input document.
     */
    function getNumberOfPages($pdf) {
        if ($pdf) {
            return $this->result->numberOfPagesPDF;
        } else {
            return $this->result->numberOfPages;
        }
    }
}

/**
 * Returns a PDFreactor configuration object.
 * 
 * @return object The configuration object bases on the settings of this PDFreactor.
 * @ignore
 */
class PDFreactorWebserviceConfiguration {
    
    var $version = "6.3";
    
    var $licenseKey = NULL;
    
    var $disableFontRegistration = false;
    var $cacheFonts = true;
    var $fontCachePath = NULL;
    var $fontDirectory = NULL;
    
    var $logLevel = LOG_LEVEL_DEFAULT;
    
    var $baseURL = NULL;
    
    var $XSLTMode = false;
    
    var $javaScriptMode = JAVASCRIPT_MODE_DEFAULT;
    
    var $encoding = NULL;
    
    // document management
    var $cleanupTool = CLEANUP_DEFAULT;
    var $documentType = DOCTYPE_DEFAULT;
    var $postTransformationDocumentType = DOCTYPE_DEFAULT;
    
    // user style sheets
    var $userStyleSheets = NULL;
    
    // user xsl style sheets
    var $XSLTStyleSheets = NULL;
    
    var $userScripts = NULL;
    
    // MetaData
    var $author = NULL;
    var $creator = NULL;
    var $keywords = NULL;
    var $title = NULL;
    var $subject = NULL;
    var $customDocumentProperties = NULL;
    var $attachments = NULL;
    
    var $documentDefaultLanguage = NULL;
    
    var $fullCompression = false;
    var $printDialogPrompt = false;
    var $viewerPreferences = 0;
    
    // encryption and restriction
    var $httpsMode = HTTPS_MODE_DEFAULT;
    
    var $encryption = ENCRYPTION_DEFAULT;
    
    var $ownerPassword = NULL;
    var $userPassword = NULL;
    
    
    var $authenticationUsername = NULL;
    var $authenticationPassword = NULL;
    
    var $addLinks = false;
    var $addBookmarks = false;
    var $addTags = false;
    var $addPreviewImages = false;
    var $addAttachments = false;
    var $addOverprint = false;
    var $appendLog = false;
    
    var $allowAnnotations = false;
    var $allowAssembly = false;
    var $allowCopy = false;
    
    var $allowDegradedPrinting = false;
    var $allowFillIn = false;
    var $allowModifyContents = false;
    var $allowPrinting = false;
    var $allowScreenReaders = false;
    
    var $mergeURL = NULL;
    var $mergeURLs = NULL;
    var $mergeByteArray = NULL;
    var $mergeByteArrays = NULL;
    var $mergeBeforePDF = true;
    var $mergeMode = MERGE_MODE_APPEND;
    var $overlayRepeat = OVERLAY_REPEAT_NONE;
    
    var $pagesPerSheetCols = 0;
    var $pagesPerSheetRows = 0;
    var $pagesPerSheetSheetSize = NULL;
    var $pagesPerSheetSheetMargin = NULL;
    var $pagesPerSheetSpacing = NULL;
    var $pagesPerSheetDirection = 0;
    
    var $bookletModeEnabled = false;
    
    var $bookletSheetSize = NULL;
    var $bookletSheetMargin = NULL;
    var $bookletRTL = false;
    
    var $conformance = -1;
    var $defaultColorSpace = 0;
    var $outputIntentIdentifier = NULL;
    var $outputIntentICCProfileURL = NULL;
    var $outputIntentICCProfileData = NULL;
    
    var $processingPreferences = 0;
    
    var $disableFontEmbedding = false;
    
    var $fontFallback = NULL;
    
    var $signPdfKeystoreURL = NULL;
    var $signPdfKeyAlias = NULL;
    var $signPdfKeystorePassword = NULL;
    var $signPdfKeystoreType = NULL;
    var $signPdfSigningMode = 0;
    
    var $pixelsPerInch = 96;
    var $pixelsPerInchShrinkToFit = false;
    
    var $logExceedingContentAnalyze = -1;
    var $logExceedingContentAgainst = -1;
    var $throwLicenseExceptions = false;
    
    var $cookies = NULL;
    var $requestHeaderMap = NULL;
    
    var $fontDirectories = NULL;
    
    var $fontAliases = NULL;
    var $fonts = NULL;
    
    var $outputFormat = OUTPUT_FORMAT_PDF;
    var $outputWidth = 0;
    var $outputHeight = 0;
    
    var $continuousWidth = 0;
    var $continuousHeight = 0;
}

/**
 * Class to create the PDFreactorWebserviceStyleSheetData Object
 * @ignore
 */
 
class PDFreactorWebserviceStyleSheetData {
    function PDFreactorWebserviceStyleSheetData($cont, $med, $ti, $ur) {
        $this->content = $cont;
        $this->media = $med;
        $this->title = $ti;
        $this->URI = $ur;
    }
}

/**
 * Class to create the PDFreactorWebserviceFontData Object
 * @ignore
 */
 
class PDFreactorWebserviceFontData {
    function PDFreactorWebserviceFontData($source, $family, $bold, $italic) {
        $this->source = $source;
        $this->family = $family;
        $this->bold = $bold;
        $this->italic = $italic;
    }
}

/**
 * Class to create the PDFreactorWebserviceXSLTStyleSheetData Object
 * @ignore
 */
 
class PDFreactorWebserviceXSLTStyleSheetData {
    function PDFreactorWebserviceXSLTStyleSheetData($cont, $ur) {
        $this->content = $cont;
        $this->URI = $ur;
    }
}

/**
 * Class to create the PDFreactorWebserviceUserScriptData Object
 * @ignore
 */
 
class PDFreactorWebserviceUserScriptData {
    function PDFreactorWebserviceUserScriptData($cont, $ur, $bDS) {
        $this->content = $cont;
        $this->URI = $ur;
        $this->beforeDocumentScripts = $bDS;
    }
}

/**
 * Class to create the PDFreactorWebserviceAttachmentData Object
 * @ignore
 */
 
class PDFreactorWebserviceAttachmentData {
    function PDFreactorWebserviceAttachmentData($data, $url, $name, $description) {
        if (preg_match("/%PDF/", $data)) {
            $data = base64_encode($data);
        }
        
        $this->data = $data;
        $this->url = $url;
        $this->name = $name;
        $this->description = $description;
    }
}

class ExceedingContent {
    
    /**
     * @ignore
     */
    function ExceedingContent($pageNumber, $right, $bottom, $left, $top, $description, 
        $box, $html, $path, $pageLeft, $pageTop, $pageRight, $pageBottom, $containerLeft, $containerTop, $containerRight, 
        $containerBottom, $exceedingBoxLeft, $exceedingBoxTop, $exceedingBoxRight, $exceedingBoxBottom, $summary) {
        $this->pageNumber = $pageNumber;
        $this->right = $right;
        $this->bottom = $bottom;
        $this->left = $left;
        $this->top = $top;
        $this->description = $description;
        $this->box = $box;
        $this->html = $html;
        $this->path = $path;
        $this->pageLeft = $pageLeft;
        $this->pageTop = $pageTop;
        $this->pageRight = $pageRight;
        $this->pageBottom = $pageBottom;
        $this->containerLeft = $containerLeft;
        $this->containerTop = $containerTop;
        $this->containerRight = $containerRight;
        $this->containerBottom = $containerBottom;
        $this->exceedingBoxLeft = $exceedingBoxLeft;
        $this->exceedingBoxTop = $exceedingBoxTop;
        $this->exceedingBoxRight = $exceedingBoxRight;
        $this->exceedingBoxBottom = $exceedingBoxBottom;
        $this->summary = $summary;
    }
    
    /**
     * Returns the number of the page that contains the exceeding content.
     * @return int The number of the page that contains the exceeding content.
     */
    function getPageNumber() {
        return $this->pageNumber;
    }
    
    /**
     * Returns whether the content exceeds the page to the right.
     * @return boolean <samp>True</samp> if the content exceeds the page to the right, <samp>false</samp> otherwise.
     */
    function isRight() {
        return $this->right;
    }
    
    /**
     * Returns whether the content exceeds the page at the bottom.
     * @return boolean <samp>True</samp> if the content exceeds the page at the bottom, <samp>false</samp> otherwise.
     */
    function isBottom() {
        return $this->bottom;
    }
    
    /**
     * Returns whether the content exceeds the page to the left.
     * @return boolean <samp>True</samp> if the content exceeds the page to the left, <samp>false</samp> otherwise.
     */
    function isLeft() {
        return $this->left;
    }
    
    /**
     * Returns whether the content exceeds the page at the top.
     * @return boolean <samp>True</samp> if the content exceeds the page at the top, <samp>false</samp> otherwise.
     */
    function isTop() {
        return $this->top;
    }
    
    /**
     * Returns the text that exceeds the page or <samp>null</samp> if this exceeding content is an image.
     * @return string The text that exceeds the page or <samp>null</samp> if this exceeding content is an image.
     */
    function getDescription() {
        return $this->description;
    }
    
    /**
     * Returns whether the exceeding content is a box.
     * @return boolean True if the exceeding content is a box.
     */
    function isBox() {
        return $this->box;
    }
    
    /**
     * Returns the HTML of the box that contains the exceeding content.
     * @return string The HTML of the box that contains the exceeding content.
     */
    function getHtml() {
        return $this->html;
    }
    
    /**
     * Returns an array of integers denoting the indexes of the ancestors of 
     * the DOM node corresponding to the box containing the exceeding content, 
     * starting from below the root node down to the element itself.
     * @return array An array of integers.
     */
    function getPath() {
        return $this->path;
    }
    
    /**
     * Returns the left coordinate of the the page in pixels.
     * @return int The left coordinate of the the page in pixels.
     */
    function getPageLeft() {
        return $this->pageLeft;
    }
    
    /**
     * Returns the top coordinate of the the page in pixels.
     * @return int The top coordinate of the the page in pixels.
     */
    function getPageTop() {
        return $this->pageTop;
    }
    
    /**
     * Returns the right coordinate of the the page in pixels.
     * @return int The right coordinate of the the page in pixels.
     */
    function getPageRight() {
        return $this->pageRight;
    }
    
    /**
     * Returns the bottom coordinate of the the page in pixels.
     * @return int The bottom coordinate of the the page in pixels.
     */
    function getPageBottom() {
        return $this->pageBottom;
    }
    
    /**
     * Returns the left coordinate of the the container box in the page in pixels. Depending on the settings this box may be the page
     * @return int The left coordinate of the the container box in the page in pixels.
     */
    function getContainerLeft() {
        return $this->containerLeft;
    }
    
    /**
     * Returns the top coordinate of the the container box in the page in pixels. Depending on the settings this box may be the page
     * @return int The top coordinate of the the container box in the page in pixels.
     */
    function getContainerTop() {
        return $this->containerTop;
    }
    
    /**
     * Returns the right coordinate of the the container box in the page in pixels. Depending on the settings this box may be the page
     * @return int The right coordinate of the the container box in the page in pixels.
     */
    function getContainerRight() {
        return $this->containerRight;
    }
    
    /**
     * Returns the bottom coordinate of the the container box in the page in pixels. Depending on the settings this box may be the page
     * @return int The bottom coordinate of the the container box in the page in pixels.
     */
    function getContainerBottom() {
        return $this->containerBottom;
    }
    
    /**
     * Returns the left coordinate of the the exceeding box in the page in pixels.
     * @return int The left coordinate of the the exceeding box in the page in pixels.
     */
    function getExceedingBoxLeft() {
        return $this->exceedingBoxLeft;
    }
    
    /**
     * Returns the top coordinate of the the exceeding box in the page in pixels.
     * @return int The top coordinate of the the exceeding box in the page in pixels.
     */
    function getExceedingBoxTop() {
        return $this->exceedingBoxTop;
    }
    
    /**
     * Returns the right coordinate of the the exceeding box in the page in pixels.
     * @return int The right coordinate of the the exceeding box in the page in pixels.
     */
    function getExceedingBoxRight() {
        return $this->exceedingBoxRight;
    }
    
    /**
     * Returns the bottom coordinate of the the exceeding box in the page in pixels.
     * @return int The bottom coordinate of the the exceeding box in the page in pixels.
     */
    function getExceedingBoxBottom() {
        return $this->exceedingBoxBottom;
    }
    
    /**
     * Returns a summary of this exceeding content object.
     * @return string The summary of this exceeding content object.
     */
    function getSummary() {
        return $this->summary;
    }
}

/**
 * This  is the PHP-SOAP connection extension for PDFreactor. 
 * 
 * @package PDFreactor-PHP
 * @ignore
 */
  
class ROClient {
    
    var $result;
    
    function ROClient($host, $port, $destination, $debug = false) {
        $this->host = $host;
        $this->port = $port;
        $this->destination = $destination;
        $this->debug = $debug;
    }
    
    function ROCall($method, $args) {
        $generatedXML = $this->generateXML($method, $args);
        
        // Debug Output
        if ($this->debug) {
            echo "XML-Request: <br/><br/><pre>";
            echo htmlentities($generatedXML);
            echo "</pre>";
        }
        $result = "";
        $content_length = strlen($generatedXML);
        $fp = fsockopen ($this->host, $this->port, $errno, $errstr, 120);
        if (!$fp) {
            echo "$errstr ($errno)<br />\n";
        } else {
            fputs ($fp, "POST $this->destination HTTP/1.0\r\nContent-Type: text/xml; charset=UTF-8\r\nContent-Length: $content_length\r\nSOAPAction: \"\"\r\n\r\n".$generatedXML."\r\n\r\n");
            while (!feof($fp)) {
                $result.=fgets($fp,4096);
            }
        fclose($fp);
        }
        
        // Debug Output
        if ($this->debug) {
            echo "XML-Response: <br/><br/><pre>";
            echo htmlentities($result);
            echo "</pre>";
        }
        
        // Clean result from HTTP Headers
        $start = strpos($result, "<?xml");
        $result = substr($result,$start);
        
        $result = $this->xml2array($result);
        
        $resultArray = $result['soapenv:Envelope']['soapenv:Body'][$method."Response"][$method."Return"];
        
        if ($this->debug) {
            echo "Result Array: <br/><br/><pre>";
            print_r($resultArray);
            echo "</pre>";
        }
        
        if(is_null($this->result)) {
            $this->result = new stdClass();
        }
        
        if (isset($resultArray['PDF']) && !is_array($resultArray['PDF'])) {
            $this->result->PDF = base64_decode($resultArray['PDF']);
        } else {
            $this->result->PDF = NULL;
        }
        
        if (isset($resultArray['error']) && !is_array($resultArray['error'])) {
            $this->result->error = $resultArray['error'];
        } else {
            $this->result->error = NULL;
        }
        if (isset($resultArray['log']) && !is_array($resultArray['log'])) {
            $this->result->log = $resultArray['log']; 
        } else {
            $this->result->log = NULL;
        }
        if (isset($resultArray['numberOfPages']) && !is_array($resultArray['numberOfPages'])) {
            $this->result->numberOfPages = $resultArray["numberOfPages"]; 
        } else {
            $this->result->numberOfPages = NULL;
        }
        if (isset($resultArray['numberOfPagesPDF']) && !is_array($resultArray['numberOfPagesPDF'])) {
            $this->result->numberOfPagesPDF = $resultArray["numberOfPagesPDF"]; 
        } else {
            $this->result->numberOfPagesPDF = NULL;
        }
        
        if (isset($resultArray['outputArray']['outputArray']) && count($resultArray['outputArray']['outputArray']) > 0) {
            $oA = array();
            foreach ($resultArray['outputArray']['outputArray'] as $key => $value) {
                if (count($resultArray['outputArray']['outputArray']) == 1) {
                    array_push($oA, base64_decode($value));
                } else {
                    array_push($oA, base64_decode($value['byteArray']));
                }
            }
            
            $this->result->outputArray = $oA; 
        } else {
            $this->result->outputArray = NULL;
        }
        
        if (isset($resultArray['exceedingContents']['exceedingContents']) && count($resultArray['exceedingContents']['exceedingContents']) > 0) {
            $exceedingContentArray = array();
            
            if (!isset($resultArray['exceedingContents']['exceedingContents'][0])) {
                $value = $resultArray['exceedingContents']['exceedingContents'];
                $exceedingContentObject = new ExceedingContent($value["pageNumber"], 
                    $value["right"],
                    $value["bottom"],
                    $value["left"],
                    $value["top"],
                    $value["description"],
                    $value["box"],
                    $value["html"],
                    $value["path"],
                    $value["pageLeft"],
                    $value["pageTop"],
                    $value["pageRight"],
                    $value["pageBottom"],
                    $value["containerLeft"],
                    $value["containerTop"],
                    $value["containerRight"],
                    $value["containerBottom"],
                    $value["exceedingBoxLeft"],
                    $value["exceedingBoxTop"],
                    $value["exceedingBoxRight"],
                    $value["exceedingBoxBottom"],
                    $value["summary"]
                    );
                    
                $exceedingContentArray[0] = $exceedingContentObject;
            } else {
                $i = 0;
                foreach ($resultArray['exceedingContents']['exceedingContents'] as $key => $value) {
                    $exceedingContentObject = new ExceedingContent($value["pageNumber"], 
                    $value["right"],
                    $value["bottom"],
                    $value["left"],
                    $value["top"],
                    $value["description"],
                    $value["box"],
                    $value["html"],
                    $value["path"],
                    $value["pageLeft"],
                    $value["pageTop"],
                    $value["pageRight"],
                    $value["pageBottom"],
                    $value["containerLeft"],
                    $value["containerTop"],
                    $value["containerRight"],
                    $value["containerBottom"],
                    $value["exceedingBoxLeft"],
                    $value["exceedingBoxTop"],
                    $value["exceedingBoxRight"],
                    $value["exceedingBoxBottom"],
                    $value["summary"]
                    );
                    
                    $exceedingContentArray[$i] = $exceedingContentObject;
                    $i++;
                
                }
            }
            
            
            $this->result->exceedingContents = $exceedingContentArray;
        } else {
            $this->result->exceedingContents = NULL;
        }
        
        return $this->result;
    }
    
    function generateXML($method,$args) {
        $xml = "<SOAP-ENV:Envelope  xmlns:SOAP-ENV='http://schemas.xmlsoap.org/soap/envelope/' xmlns:xsd='http://www.w3.org/2001/XMLSchema' xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance' xmlns:SOAP-ENC='http://schemas.xmlsoap.org/soap/encoding/' SOAP-ENV:encodingStyle='http://schemas.xmlsoap.org/soap/encoding/'>      
   <SOAP-ENV:Body>";
        $xml .= "<$method>";
        if ($method == "renderDocumentFromByteArray" || $method == "renderDocumentFromByteArrayAsArray") {
            $xml .= "<in0 xsi:type='xsd:base64Binary'>".base64_encode($args['in0'])."</in0>";
        } else {
            $xml .= "<in0 xsi:type='string'>".htmlspecialchars($args['in0'])."</in0>";
        }
        $xml .= "<in1>";
        foreach ($args['in1'] as $key => $value) {                       
            if (gettype($value) === "boolean" && $value === false) {
                $value = "false";
                $type = "boolean";
            }
            elseif (gettype($value) === "boolean" && $value === true) {
                $value = "true";
                $type = "boolean";
            }
            elseif (gettype($value) === "integer") {
                $type = "int";
            }
            else {
                $type = gettype($value);
            }
            if (gettype($value) == "array") {
                $xml .= "<".$key.">";
                for ($i=0; $i < count($value); $i++) {
                    $xml .= "<item>";
                    if (gettype($value[$i]) == "object") {
                        foreach ($value[$i] as $uss_key => $uss_value) {
                            if (gettype($uss_value) === "boolean" && $uss_value === false) {
                                $uss_value = "false";
                                $uss_type = "boolean";
                            }
                            elseif (gettype($uss_value) === "boolean" && $uss_value === true) {
                                $uss_value = "true";
                                $uss_type = "boolean";
                            } if ($uss_key == "data") {
                                $uss_type = "base64Binary";
                            } else {
                                $uss_value = htmlspecialchars($uss_value);
                                $uss_type = "string";
                            }
                            $xml .= "<".$uss_key." xsi:type=\"xsd:".$uss_type."\">".$uss_value."</".$uss_key.">\n";
                        }
                    } elseif (gettype($value[$i]) == "string") {
                        $xml .= htmlspecialchars($value[$i]);
                    } elseif (gettype($value[$i]) == "array") {
                        for ($j=0; $j < count($value[$i]); $j++) {
                            $xml .= "<item>";
                            if (gettype($value[$i][$j]) == "string") {
                                $xml .= utf8_encode($value[$i][$j]);
                            }
                            $xml .= "</item>";    
                        }          
                    }
                    $xml .= "</item>";    
                }                   
                $xml .= "</".$key.">";
            } elseif ($value === NULL || strlen(trim($value)) == 0) {
                $xml .= "<".$key." xsi:nil=\"true\"/>\n";
            } elseif($key == "mergeByteArray" || $key == "outputIntentICCProfileData") {
                $xml .= "<".$key." xsi:type=\"xsd:base64Binary\">".$value."</".$key.">";
            } elseif ($type == "string") {
                $xml .= "<".$key." xsi:type=\"xsd:".$type."\">".htmlspecialchars($value)."</".$key.">";
            } else {
                $xml .= "<".$key." xsi:type=\"xsd:".$type."\">".$value."</".$key.">";
            }
        }
        $xml .= "</in1>";
        $xml .= "</$method>";
        $xml .= "</SOAP-ENV:Body></SOAP-ENV:Envelope>";
        return $xml;
    }

    function xml2array($contents, $get_attributes = 0, $priority = 'tag') {
        if (!function_exists('xml_parser_create')) {
            return array();
        }
        $parser = xml_parser_create('');
        
        xml_parser_set_option($parser, XML_OPTION_TARGET_ENCODING, "UTF-8");
        xml_parser_set_option($parser, XML_OPTION_CASE_FOLDING, 0);
        xml_parser_set_option($parser, XML_OPTION_SKIP_WHITE, 1);
        xml_parse_into_struct($parser, trim($contents), $xml_values);
        xml_parser_free($parser);
        if (!$xml_values) {
            return;
        }
        $xml_array = array();
        $parents = array();
        $opened_tags = array();
        $arr = array();
        $current = &$xml_array;
        $repeated_tag_index = array(); 
        foreach ($xml_values as $data) {
            unset ($attributes, $value);
            extract($data);
            $result = array ();
            $attributes_data = array ();
            if (isset ($value)) {
                if ($priority == 'tag') {
                    $result = $value;
                } else {
                    $result['value'] = $value;
                }
            }
            if (isset ($attributes) and $get_attributes) {
                foreach ($attributes as $attr => $val) {
                    if ($priority == 'tag') {
                        $attributes_data[$attr] = $val;
                    } else {
                        $result['attr'][$attr] = $val; //Set all the attributes in a array called 'attr'
                    }
                }
            }
            if ($type == "open") { 
                $parent[$level -1] = & $current;
                if (!is_array($current) or (!in_array($tag, array_keys($current)))) {
                    $current[$tag] = $result;
                    if ($attributes_data) {
                        $current[$tag . '_attr'] = $attributes_data;
                    }
                    $repeated_tag_index[$tag . '_' . $level] = 1;
                    $current = & $current[$tag];
                } else {
                    if (isset ($current[$tag][0])) {
                        $current[$tag][$repeated_tag_index[$tag . '_' . $level]] = $result;
                        $repeated_tag_index[$tag . '_' . $level]++;
                    } else { 
                        $current[$tag] = array (
                            $current[$tag],
                            $result
                        ); 
                        $repeated_tag_index[$tag . '_' . $level] = 2;
                        if (isset ($current[$tag . '_attr'])) {
                            $current[$tag]['0_attr'] = $current[$tag . '_attr'];
                            unset ($current[$tag . '_attr']);
                        }
                    }
                    $last_item_index = $repeated_tag_index[$tag . '_' . $level] - 1;
                    $current = & $current[$tag][$last_item_index];
                }
            }
            elseif ($type == "complete") {
                if (!isset ($current[$tag])) {
                    $current[$tag] = $result;
                    $repeated_tag_index[$tag . '_' . $level] = 1;
                    if ($priority == 'tag' and $attributes_data)
                        $current[$tag . '_attr'] = $attributes_data;
                } else {
                    if (isset ($current[$tag][0]) and is_array($current[$tag])) {
                        $current[$tag][$repeated_tag_index[$tag . '_' . $level]] = $result;
                        if ($priority == 'tag' and $get_attributes and $attributes_data) {
                            $current[$tag][$repeated_tag_index[$tag . '_' . $level] . '_attr'] = $attributes_data;
                        }
                        $repeated_tag_index[$tag . '_' . $level]++;
                    } else {
                        $current[$tag] = array (
                            $current[$tag],
                            $result
                        ); 
                        $repeated_tag_index[$tag . '_' . $level] = 1;
                        if ($priority == 'tag' and $get_attributes) {
                            if (isset ($current[$tag . '_attr'])) { 
                                $current[$tag]['0_attr'] = $current[$tag . '_attr'];
                                unset ($current[$tag . '_attr']);
                            }
                            if ($attributes_data) {
                                $current[$tag][$repeated_tag_index[$tag . '_' . $level] . '_attr'] = $attributes_data;
                            }
                        }
                        $repeated_tag_index[$tag . '_' . $level]++; //0 and 1 index is already taken
                    }
                }
            } elseif ($type == 'close') {
                $current = & $parent[$level -1];
            }
        }
        return ($xml_array);
    }
}
?>
