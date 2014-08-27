<?php


     
/**
 * File: doc_import.inc
 * Library functions used by all document_import processes
 */

function protect_special_characters(&$doc){
	// First, replace UTF-8 characters.
	$doc = str_replace(
		//left ' quote	right ' quote	left " quote	right " quote	en-dash		em-dash		ellipse		hyphen		bullet		TM		white bullet	em-space	check mark	heavy check mark
	 array("\xe2\x80\x98", 	"\xe2\x80\x99",	"\xe2\x80\x9c", "\xe2\x80\x9d", "\xe2\x80\x93", "\xe2\x80\x94", "\xe2\x80\xa6", "\xe2\x80\x90", "\xe2\x80\xa2", "\xe2\x84\xa2", "\xe2\x97\xa6", "\xe2\x80\x83", "\xe2\x9c\x93", "\xe2\x9c\x94"),
	 array("&lsquo;", 	"&rsquo;", 	"&ldquo;", 	"&rdquo;", 	"&ndash;", 	"&mdash;", 	"&hellip;", 	"&#8208;", 	"&bull;", 	"&trade;", 	"&#9702;", 	"&#8195;",	"&#x2713;",	"&#x2714;"),
	 $doc);


	 // This next command was causing certain doc strings to be 0 length. Temporarily disabled. Do we still need it?
	 
	////Next, replace their Windows-1252 equivalents.
	//$doc = str_replace( array(chr(145), chr(146), chr(147), chr(148), chr(150), chr(151), chr(133)),
	//		    array("'", "'", '"', '"', '-', '--', '...'),
	//		    $doc);

	//Workaround for QP special entity parsing problems. Inject a "%%%" into the HTML code to keep it from being converted prematurely.
	$doc = str_replace( array("&lsquo;", 	"&rsquo;", 	"&ldquo;", 	"&rdquo;", 	"&ndash;", 	"&mdash;", 	"&hellip;", 	"&#8208;", 	"&bull;", 	"&trade;", 	"&#9702;",	"&#8195;", "&#10003;", "&#10004;"),
			    array("%%%lsquo;", 	"%%%rsquo;", "%%%ldquo;", "%%%rdquo;", "%%%ndash;", "%%%mdash;", "%%%hellip;", 	"%%%#8208;", "%%%bull;", 	"%%%trade;", "%%%#9702;",	"%%%#8195;", "%%%#x2713;", "%%%#x2714;"),
			    $doc);
	
	//Workaround for Mif2Go-processed documents (these output "&#xxxx;" codes);
	$doc = str_replace( array("&#8216;", 	"&#8217;", 	"&#8220", 	"&#8221;", 	"&#8211;", 	"&#8212;", 	"&#8230;", 		"&#8208;", "&#8226;", "&#8482;", 	"&#9702;", "&#8221;", "&#10003;", "&#10004;"),
			    array("%%%lsquo;", "%%%rsquo;", "%%%ldquo;", "%%%rdquo;", "%%%ndash;", "%%%mdash;", "%%%hellip;", "%%%#8208;", "%%%bull;", "%%%trade;", "%%%#9702;", "%%%#x2713;", "%%%#x2713;", "%%%#x2714;"),
			    $doc);

	//Other special Word characters
	$doc = str_replace('&#8209;','-', $doc);
	
	//globally encode any other & characters hanging around.
	$doc = str_replace('&','%%%',$doc);
	return $doc;
}

/**
 * Remove "%" from HTML codes so they display properly.
 */

function unprotect_special_characters(&$doc){
	$doc = str_replace(array('%%%','%%'),'&',$doc);
	return $doc;
}


/**
 * find the parent book node
 */
function _doc_import_get_book_parent(&$book_node) {
  $sql = "SELECT nid FROM {book} WHERE mlid = %d ";
  $parent_nid = db_query("SELECT nid FROM {book} WHERE mlid = :mlid ", array(':mlid' => $book_node->book['plid']))->fetchField();
  $book_node = node_load($parent_nid);
}

/**
 * save book info
 */
function _doc_import_save_book_node($book_node, $heading_type = 0) {

  $node_id = $book_node->nid;  
  $nid = db_insert('doc_import_book_nodes')
    ->fields(array(
      'book_nid' => $book_node->nid,
      'book_root_nid' => $book_node->book['bid'],
      'weight' => $book_node->book['weight'],
      'heading_type' => $heading_type,
      'title' => $book_node->title,
    ))
    ->execute();
}

/**
 * get heading type of a book node
 */
function _doc_import_get_book_heading_type($book_node) {
  $sql = "SELECT heading_type FROM {doc_import_book_nodes} WHERE book_nid = %d ";
  $heading_type = db_query("SELECT heading_type FROM {doc_import_book_nodes} WHERE book_nid = :book_nid ", array(':book_nid' => $book_node->nid))->fetchField();

  return $heading_type;
}

/**
 * get weight of a book node
 */
function _doc_import_get_book_weight($book_node) {
  $sql = "SELECT weight FROM {doc_import_book_nodes} WHERE book_nid = %d ";
  $weight = db_query("SELECT weight FROM {doc_import_book_nodes} WHERE book_nid = :book_nid ", array(':book_nid' => $book_node->nid))->fetchField();

  return $weight;
}

/**
 * get all applied vocabularies of a content type
 */
function _doc_import_get_applied_vocabularies($content_type) {

  $fields = field_info_fields();
  $vocabs = array();
  foreach ($fields as $field_name => $field) {


    if (!empty($field['bundles']['node']) && $field['type'] == 'taxonomy_term_reference' && in_array($content_type,$field['bundles']['node'])){
        $vocabs[] = $field['id'];
        $vocab_machine_name=$field['settings']['allowed_values'][0]['vocabulary'];
        $vocab = taxonomy_vocabulary_machine_name_load($vocab_machine_name);
        $vid[] = $vocab->vid;
    }
  }
  return $vid;
}

/**
 * return an array of taxonomy reference and date fields used in the specified content type
 */
function _doc_import_get_node_fields($content_type) {
  $node_fields = array();
  foreach (field_info_fields() as $field_name => $field_attrs) {
    if (in_array($content_type,$field_attrs['bundles']['node']) && ($field_attrs['module'] == 'taxonomy' || $field_attrs['module'] == 'date')){
      $node_fields[$field_name] = $field_attrs;
    }
  }
  return $node_fields;
}


///**
// * Extracts content between <body></body> tags, exclusive
// */
//function extract_body(&$content){
//  preg_match('/<body[^>]*>(.*)<\/body>/si', $content, $matches);
//  if (count($matches) > 0){
//    $content = $matches[1];
//  }
//  return $content;
//}

/**
 * Swap out old tags for new tags. Takes a QueryPath object and a new set of tags to wrap the object in.
 */
function swap_tags($qp, $open, $close){
	if($qp->size() > 0){
		foreach ($qp as $tag){
			$content = $tag->innerHTML();
			$replace_string = $open . $content . $close;
			if(!empty($content)){
				$tag->replaceWith($replace_string);
			}
		}
	}
}

function strallpos($haystack,$needle,$offset = 0){ 
    $result = array(); 
    for($i = $offset; $i<strlen($haystack); $i++){ 
        $pos = strpos($haystack,$needle,$i); 
        if($pos !== FALSE){ 
            $offset =  $pos; 
            if($offset >= $i){ 
                $i = $offset; 
                $result[] = $offset; 
            } 
        } 
    } 
    return $result; 
}

function recursive_array_search($needle,$haystack) {
    foreach($haystack as $key=>$value) {
        $current_key=$key;
        if($needle===$value OR (is_array($value) && recursive_array_search($needle,$value) !== false)) {
            return $current_key;
        }
    }
    return false;
}


/**
 * Library Functions from public domain
 */

 
/**
 * Recursive directory delete
 * 
 */
 function _rmdir_recursive($dir) {
  foreach(scandir($dir) as $file) {
    if ('.' === $file || '..' === $file) continue;
    if (is_dir("$dir/$file")) _rmdir_recursive("$dir/$file");
    else unlink("$dir/$file");
  }
  rmdir($dir);
 }
 
 
/**
 * Function that inserts $element into a provided &$array at $position.
 * 
 */
function array_insert(&$array,$element,$position=null) {
  if (count($array) == 0) {
    $array[] = $element;
  }
  elseif (is_numeric($position) && $position < 0) {
    if((count($array)+position) < 0) {
      $array = array_insert($array,$element,0);
    }
    else {
      $array[count($array)+$position] = $element;
    }
  }
  elseif (is_numeric($position) && isset($array[$position])) {
    $part1 = array_slice($array,0,$position,true);
    $part2 = array_slice($array,$position,null,true);
    $array = array_merge($part1,array($position=>$element),$part2);
    foreach($array as $key=>$item) {
      if (is_null($item)) {
        unset($array[$key]);
      }
    }
  }
  elseif (is_null($position)) {
    $array[] = $element;
  }  
  elseif (!isset($array[$position])) {
    $array[$position] = $element;
  }
  $array = array_merge($array);
  return $array;
}

/**
 * Reads a delmited data file into an array
 *
 */
function csv_in_array($url,$delm=";",$encl="\"",$head=false) { 
    
    $csvxrow = file($url);   // ---- csv rows to array ----
    
    $csvxrow[0] = chop($csvxrow[0]); 
    $csvxrow[0] = str_replace($encl,'',$csvxrow[0]); 
    $keydata = explode($delm,$csvxrow[0]); 
    $keynumb = count($keydata); 
    
    if ($head === true) { 
    $anzdata = count($csvxrow); 
    $z=0; 
    for($x=1; $x<$anzdata; $x++) { 
        $csvxrow[$x] = chop($csvxrow[$x]); 
        $csvxrow[$x] = str_replace($encl,'',$csvxrow[$x]); 
        $csv_data[$x] = explode($delm,$csvxrow[$x]); 
        $i=0; 
        foreach($keydata as $key) { 
            $out[$z][$key] = $csv_data[$x][$i]; 
            $i++;
            }    
        $z++;
        }
    }
    else { 
        $i=0;
        foreach($csvxrow as $item) { 
            $item = chop($item); 
            $item = str_replace($encl,'',$item); 
            $csv_data = explode($delm,$item); 
            for ($y=0; $y<$keynumb; $y++) { 
               $out[$i][$y] = $csv_data[$y]; 
            }
        $i++;
        }
    }

return $out; 
}


/*
Copyright (c) 2008 Sebastián Grignoli
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
3. Neither the name of copyright holders nor the names of its
   contributors may be used to endorse or promote products derived
   from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL COPYRIGHT HOLDERS OR CONTRIBUTORS
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
*/

/**
 * @author   "Sebastián Grignoli" <grignoli@framework2.com.ar>
 * @package  Encoding
 * @version  1.2
 * @link     https://github.com/neitanod/forceutf8
 * @example  https://github.com/neitanod/forceutf8
 * @license  Revised BSD
  */

//namespace ForceUTF8;

class Encoding {
    
  protected static $win1252ToUtf8 = array(
        128 => "\xe2\x82\xac",

        130 => "\xe2\x80\x9a",
        131 => "\xc6\x92",
        132 => "\xe2\x80\x9e",
        133 => "\xe2\x80\xa6",
        134 => "\xe2\x80\xa0",
        135 => "\xe2\x80\xa1",
        136 => "\xcb\x86",
        137 => "\xe2\x80\xb0",
        138 => "\xc5\xa0",
        139 => "\xe2\x80\xb9",
        140 => "\xc5\x92",

        142 => "\xc5\xbd",


        145 => "\xe2\x80\x98",
        146 => "\xe2\x80\x99",
        147 => "\xe2\x80\x9c",
        148 => "\xe2\x80\x9d",
        149 => "\xe2\x80\xa2",
        150 => "\xe2\x80\x93",
        151 => "\xe2\x80\x94",
        152 => "\xcb\x9c",
        153 => "\xe2\x84\xa2",
        154 => "\xc5\xa1",
        155 => "\xe2\x80\xba",
        156 => "\xc5\x93",

        158 => "\xc5\xbe",
        159 => "\xc5\xb8"
  );
  
    protected static $brokenUtf8ToUtf8 = array(
        "\xc2\x80" => "\xe2\x82\xac",
        
        "\xc2\x82" => "\xe2\x80\x9a",
        "\xc2\x83" => "\xc6\x92",
        "\xc2\x84" => "\xe2\x80\x9e",
        "\xc2\x85" => "\xe2\x80\xa6",
        "\xc2\x86" => "\xe2\x80\xa0",
        "\xc2\x87" => "\xe2\x80\xa1",
        "\xc2\x88" => "\xcb\x86",
        "\xc2\x89" => "\xe2\x80\xb0",
        "\xc2\x8a" => "\xc5\xa0",
        "\xc2\x8b" => "\xe2\x80\xb9",
        "\xc2\x8c" => "\xc5\x92",
        
        "\xc2\x8e" => "\xc5\xbd",
        
        
        "\xc2\x91" => "\xe2\x80\x98",
        "\xc2\x92" => "\xe2\x80\x99",
        "\xc2\x93" => "\xe2\x80\x9c",
        "\xc2\x94" => "\xe2\x80\x9d",
        "\xc2\x95" => "\xe2\x80\xa2",
        "\xc2\x96" => "\xe2\x80\x93",
        "\xc2\x97" => "\xe2\x80\x94",
        "\xc2\x98" => "\xcb\x9c",
        "\xc2\x99" => "\xe2\x84\xa2",
        "\xc2\x9a" => "\xc5\xa1",
        "\xc2\x9b" => "\xe2\x80\xba",
        "\xc2\x9c" => "\xc5\x93",
        
        "\xc2\x9e" => "\xc5\xbe",
        "\xc2\x9f" => "\xc5\xb8"
  );
    
  protected static $utf8ToWin1252 = array(
       "\xe2\x82\xac" => "\x80",
       
       "\xe2\x80\x9a" => "\x82",
       "\xc6\x92"     => "\x83",
       "\xe2\x80\x9e" => "\x84",
       "\xe2\x80\xa6" => "\x85",
       "\xe2\x80\xa0" => "\x86",
       "\xe2\x80\xa1" => "\x87",
       "\xcb\x86"     => "\x88",
       "\xe2\x80\xb0" => "\x89",
       "\xc5\xa0"     => "\x8a",
       "\xe2\x80\xb9" => "\x8b",
       "\xc5\x92"     => "\x8c",
       
       "\xc5\xbd"     => "\x8e",
       
       
       "\xe2\x80\x98" => "\x91",
       "\xe2\x80\x99" => "\x92",
       "\xe2\x80\x9c" => "\x93",
       "\xe2\x80\x9d" => "\x94",
       "\xe2\x80\xa2" => "\x95",
       "\xe2\x80\x93" => "\x96",
       "\xe2\x80\x94" => "\x97",
       "\xcb\x9c"     => "\x98",
       "\xe2\x84\xa2" => "\x99",
       "\xc5\xa1"     => "\x9a",
       "\xe2\x80\xba" => "\x9b",
       "\xc5\x93"     => "\x9c",
       
       "\xc5\xbe"     => "\x9e",
       "\xc5\xb8"     => "\x9f"
    );

  static function toUTF8($text){
  /**
   * Function Encoding::toUTF8
   *
   * This function leaves UTF8 characters alone, while converting almost all non-UTF8 to UTF8.
   * 
   * It assumes that the encoding of the original string is either Windows-1252 or ISO 8859-1.
   *
   * It may fail to convert characters to UTF-8 if they fall into one of these scenarios:
   *
   * 1) when any of these characters:   ÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏÐÑÒÓÔÕÖ×ØÙÚÛÜÝÞß
   *    are followed by any of these:  ("group B")
   *                                    ¡¢£¤¥¦§¨©ª«¬­®¯°±²³´µ¶•¸¹º»¼½¾¿
   * For example:   %ABREPRESENT%C9%BB. «REPRESENTÉ»
   * The "«" (%AB) character will be converted, but the "É" followed by "»" (%C9%BB) 
   * is also a valid unicode character, and will be left unchanged.
   *
   * 2) when any of these: àáâãäåæçèéêëìíîï  are followed by TWO chars from group B,
   * 3) when any of these: ðñòó  are followed by THREE chars from group B.
   *
   * @name toUTF8
   * @param string $text  Any string.
   * @return string  The same string, UTF8 encoded
   *
   */

    if(is_array($text))
    {
      foreach($text as $k => $v)
      {
        $text[$k] = self::toUTF8($v);
      }
      return $text;
    } elseif(is_string($text)) {
    
      $max = strlen($text);
      $buf = "";
      for($i = 0; $i < $max; $i++){
          $c1 = $text{$i};
          if($c1>="\xc0"){ //Should be converted to UTF8, if it's not UTF8 already
            $c2 = $i+1 >= $max? "\x00" : $text{$i+1};
            $c3 = $i+2 >= $max? "\x00" : $text{$i+2};
            $c4 = $i+3 >= $max? "\x00" : $text{$i+3};
              if($c1 >= "\xc0" & $c1 <= "\xdf"){ //looks like 2 bytes UTF8
                  if($c2 >= "\x80" && $c2 <= "\xbf"){ //yeah, almost sure it's UTF8 already
                      $buf .= $c1 . $c2;
                      $i++;
                  } else { //not valid UTF8.  Convert it.
                      $cc1 = (chr(ord($c1) / 64) | "\xc0");
                      $cc2 = ($c1 & "\x3f") | "\x80";
                      $buf .= $cc1 . $cc2;
                  }
              } elseif($c1 >= "\xe0" & $c1 <= "\xef"){ //looks like 3 bytes UTF8
                  if($c2 >= "\x80" && $c2 <= "\xbf" && $c3 >= "\x80" && $c3 <= "\xbf"){ //yeah, almost sure it's UTF8 already
                      $buf .= $c1 . $c2 . $c3;
                      $i = $i + 2;
                  } else { //not valid UTF8.  Convert it.
                      $cc1 = (chr(ord($c1) / 64) | "\xc0");
                      $cc2 = ($c1 & "\x3f") | "\x80";
                      $buf .= $cc1 . $cc2;
                  }
              } elseif($c1 >= "\xf0" & $c1 <= "\xf7"){ //looks like 4 bytes UTF8
                  if($c2 >= "\x80" && $c2 <= "\xbf" && $c3 >= "\x80" && $c3 <= "\xbf" && $c4 >= "\x80" && $c4 <= "\xbf"){ //yeah, almost sure it's UTF8 already
                      $buf .= $c1 . $c2 . $c3;
                      $i = $i + 2;
                  } else { //not valid UTF8.  Convert it.
                      $cc1 = (chr(ord($c1) / 64) | "\xc0");
                      $cc2 = ($c1 & "\x3f") | "\x80";
                      $buf .= $cc1 . $cc2;
                  }
              } else { //doesn't look like UTF8, but should be converted
                      $cc1 = (chr(ord($c1) / 64) | "\xc0");
                      $cc2 = (($c1 & "\x3f") | "\x80");
                      $buf .= $cc1 . $cc2;
              }
          } elseif(($c1 & "\xc0") == "\x80"){ // needs conversion
                if(isset(self::$win1252ToUtf8[ord($c1)])) { //found in Windows-1252 special cases
                    $buf .= self::$win1252ToUtf8[ord($c1)];
                } else {
                  $cc1 = (chr(ord($c1) / 64) | "\xc0");
                  $cc2 = (($c1 & "\x3f") | "\x80");
                  $buf .= $cc1 . $cc2;
                }
          } else { // it doesn't need convesion
              $buf .= $c1;
          }
      }
      return $buf;
    } else {
      return $text;
    }
  }

  static function toWin1252($text) {
    if(is_array($text)) {
      foreach($text as $k => $v) {
        $text[$k] = self::toWin1252($v);
      }
      return $text;
    } elseif(is_string($text)) {
      return utf8_decode(str_replace(array_keys(self::$utf8ToWin1252), array_values(self::$utf8ToWin1252), self::toUTF8($text)));
    } else {
      return $text;
    }
  }

  static function toISO8859($text) {
    return self::toWin1252($text);
  }

  static function toLatin1($text) {
    return self::toWin1252($text);
  }

  static function fixUTF8($text){
    if(is_array($text)) {
      foreach($text as $k => $v) {
        $text[$k] = self::fixUTF8($v);
      }
      return $text;
    }

    $last = "";
    while($last <> $text){
      $last = $text;
      $text = self::toUTF8(utf8_decode(str_replace(array_keys(self::$utf8ToWin1252), array_values(self::$utf8ToWin1252), $text)));
    }
    $text = self::toUTF8(utf8_decode(str_replace(array_keys(self::$utf8ToWin1252), array_values(self::$utf8ToWin1252), $text)));
    return $text;
  }
  
  static function UTF8FixWin1252Chars($text){
    // If you received an UTF-8 string that was converted from Windows-1252 as it was ISO8859-1 
    // (ignoring Windows-1252 chars from 80 to 9F) use this function to fix it.
    // See: http://en.wikipedia.org/wiki/Windows-1252
    
    return str_replace(array_keys(self::$brokenUtf8ToUtf8), array_values(self::$brokenUtf8ToUtf8), $text);
  }
  
  static function removeBOM($str=""){
    if(substr($str, 0,3) == pack("CCC",0xef,0xbb,0xbf)) {
      $str=substr($str, 3);
    }
    return $str;
  }
  
  public static function normalizeEncoding($encodingLabel)
  {
    $encoding = strtoupper($encodingLabel);
    $enc = preg_replace('/[^a-zA-Z0-9\s]/', '', $encoding);
    $equivalences = array(
        'ISO88591' => 'ISO-8859-1',
        'ISO8859'  => 'ISO-8859-1',
        'ISO'      => 'ISO-8859-1',
        'LATIN1'   => 'ISO-8859-1',
        'LATIN'    => 'ISO-8859-1',
        'UTF8'     => 'UTF-8',
        'UTF'      => 'UTF-8',
        'WIN1252'  => 'ISO-8859-1',
        'WINDOWS1252' => 'ISO-8859-1'
    );
    
    if(empty($equivalences[$encoding])){
      return 'UTF-8';
    }
   
    return $equivalences[$encoding];
  }

  public static function encode($encodingLabel, $text)
  {
    $encodingLabel = self::normalizeEncoding($encodingLabel);
    if($encodingLabel == 'UTF-8') return Encoding::toUTF8($text);
    if($encodingLabel == 'ISO-8859-1') return Encoding::toLatin1($text);
  }

}

///////////////////////////////////////////////////////////////////////////
// GRAVEYARD
// These functions are most likely dead and not used -- verify and remove
///////////////////////////////////////////////////////////////////////////




