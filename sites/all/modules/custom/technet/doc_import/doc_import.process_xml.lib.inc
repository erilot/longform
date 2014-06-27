<?php
// $Id$
/**
 * File: doc_import.process_xml.lib.inc
 * Library functions for processing the imports via XML
 */

/**
 * validate settings before importing documents
 */
function _doc_import_validate_xml_settings() {
  $selected_node_types = variable_get('doc_import_content_types', array());
  $target_path_xml = variable_get('doc_import_path_target_xml', '');
  $pending_path_xml = variable_get('doc_import_path_pending_xml', '');
  $finished_path_xml = variable_get('doc_import_path_finished_xml', '');
  if (empty($selected_node_types) || empty($target_path_xml) || empty($pending_path_xml) || empty($finished_path_xml)) {
    return FALSE;
  }
  else {
    return TRUE;
  }
}

/**
 * get all pending documents
 */
function _doc_import_get_pending_docs_xml($pending_path) {
  $docs = array();
  $files = file_scan_directory($pending_path, '/(.*)\.xml/');
  // dsm($files, '$files');
  foreach ($files as $file_path => $file_obj) {
    $key = substr($file_path, strlen($pending_path) + 1);
    // $temp = explode('/', $key);
    // $path_to_file = '';
    // while (count($temp) > 1) {
    // $path_to_file .= array_shift($temp) .' - ';
    // }
    // $docs[$key] = $path_to_file . $file_obj->name;
    $docs[$key] = $key;
  }

  return $docs;
}

/**
 * convert XML tags to HTML tags
 */
function _doc_import_convert_tags(&$doc) {
  // convert <A ID=""> tags (Anchors)
  // sample: <A ID="pgfId-1013435"></A> => <a id="pgfId-1013435" name="pgfId-1013435"></a>
  $fcn = '
	// dsm($matches, \'$matches\');
	$regex = \'/< *A[^>]*ID *= *["\\\']?([^"\\\']*)[^<\/]>(.*)/i\';
	preg_match($regex, $matches[0], $matches1);
	// dsm($matches1, \'$matches1\');
	if (isset($matches1[1])) {
		$text = trim($matches1[2], "</A>");
		return "<a id=\"{$matches1[1]}\" name=\"{$matches1[1]}\">{$text}</a>";
	}
	return $matches[0];
';
  $regex = '#<A ID="[^"]+"[^>]*>[^/A>]*</A>#si';
  // preg_match($regex, $doc, $matches);
  // dsm($matches, '$matches');
  $doc = preg_replace_callback($regex, create_function('$matches', $fcn), $doc);
  // dsm($doc, '$doc');

  // convert <A ...>...</A> tags (Hyperlinks)
  // sample:
  // <A href="IP7000_HDX_IntegrationGuide.xml#id(30650)" xml:link="simple" show="replace" actuate="user" CLASS="XRef">
//     <Cross-reference>Parts List</Cross-reference></A>
  // =>
  // <a href="#30650" xml:link="simple" show="replace" actuate="user" CLASS="XRef"><Cross-reference>Parts List</Cross-reference></a>
  $fcn = '
	// dsm($matches, \'$matches\');
	$regex = \'/<A href="([^"]*)" ([^>]*)>/i\';
	preg_match($regex, $matches[0], $matches1);
	// dsm($matches1, \'$matches1\');
	if (isset($matches1[1])) {
		$text = trim($matches[1]);
		$href = $matches1[1];
		if (($pos = stripos($href, "#id(")) !== FALSE) {
			$href = "#". trim(substr($href, $pos + 4), ")");
		}
		$matches1[2] = str_replace(" CLASS=", " class=", $matches1[2]);
		return "<a href=\"{$href}\" {$matches1[2]}>{$text}</a>";
	}
	return $matches[0];
';
  $regex = '#<A href="[^"]*"[^>]*>(.*?)</A>#si';
  // preg_match_all($regex, $doc, $matches);
  // dsm($matches, '$matches');
  $doc = preg_replace_callback($regex, create_function('$matches', $fcn), $doc);
  // var_dump($doc);
  // dsm($doc, '$doc');

  // convert XML tags to <span>
  // tags:
  // <Cross-reference> => <span class="Cross-reference">
  // </Cross-reference> => </span>
  // <TableHeading> => <span class="TableHeading">
  // </TableHeading> => </span>
  $doc = str_replace('<Cross-reference>', '<span class="Cross-reference">', $doc);
  $doc = str_replace('</Cross-reference>', '</span>', $doc);
  $doc = str_replace('<TableHeading>', '<span class="TableHeading">', $doc);
  $doc = str_replace('</TableHeading>', '</span>', $doc);

  // convert super script tags
  // tags:
  // <Superscript> => <sup>
  // </Superscript> => </sup>
  $doc = str_replace('<Superscript>', '<sup>', $doc);
  $doc = str_replace('</Superscript>', '</sup>', $doc);

  // convert table tags
  // tags:
  // <TABLE> => <table class="TABLE">
  // </TABLE> => </table>
  // <ROW> => <tr class="ROW">
  // </ROW> => </tr>
  // <TH ROWSPAN="1" COLSPAN="2"> => <th rospan="1" colspan="2>
  // </TH> => </th>
  // <CELL ROWSPAN="1" COLSPAN="1"> => <td rospan="1" colspan="1">
  // </CELL> => </td>
  $doc = str_replace('<TABLE>', '<table class="TABLE">', $doc);
  $doc = str_replace('</TABLE>', '</table>', $doc);
  $doc = str_replace('<ROW>', '<tr class="ROW">', $doc);
  $doc = str_replace('</ROW>', '</tr>', $doc);
  $regex = '#<TH [^>]*>#si';
  $doc = preg_replace_callback($regex, create_function('$matches', 'return strtolower($matches[0]);'), $doc);
  $doc = str_replace('</TH>', '</th>', $doc);
  $regex = '#<CELL [^>]*>#si';
  $doc = preg_replace_callback($regex, create_function('$matches', 'return strtolower(str_replace("<CELL", "<td", $matches[0]));'), $doc);
  $doc = str_replace('</CELL>', '</td>', $doc);

  // convert image tags
  // tags:
  // <IMAGE xml:link="simple" href="IP7000_HDX_IntegrationGuide-14.gif" show="embed" actuate="auto"/>
  // =>
  // <img src="IP7000_HDX_IntegrationGuide-14.gif" xml:link="simple" show="embed" actuate="auto" alt="" />
  $fcn = '
	// dsm($matches, \'$matches\');
	$regex = \'/<IMAGE (.*) href="([^"]*)" ([^>]*)>/i\';
	preg_match($regex, $matches[0], $matches1);
	// dsm($matches1, \'$matches1\');
	if (isset($matches1[2])) {
		$src = $matches1[2];
		$attributes = trim($matches1[1]) ." ". trim($matches1[3], "/");
		return "<img src=\"{$src}\" {$attributes} alt=\"\" />";
	}
	return $matches[0];
';
  $regex = '/<IMAGE[^>]+>/i';
  // preg_match_all($regex, $doc, $matches);
  // dsm($matches, '$matches');
  $doc = preg_replace_callback($regex, create_function('$matches', $fcn), $doc);


  // convert other tags to <div>'s
  $regex = '#<[^\/>]*>#';
  // preg_match_all($regex, $doc, $matches);
  // dsm($matches, '$matches');
  $fcn = '
	// dsm($matches, \'$matches\');
	if (stripos($matches[0], "<a ") !== FALSE) { /* no-op */ }
	elseif (stripos($matches[0], "<span") !== FALSE) { /* no-op */ }
	elseif (stripos($matches[0], "<sup") !== FALSE) { /* no-op */ }
	elseif (stripos($matches[0], "<img") !== FALSE) { /* no-op */ }
	elseif (stripos($matches[0], "<table ") !== FALSE || stripos($matches[0], "<table>") !== FALSE) { /* no-op */ }
	elseif (stripos($matches[0], "<tr") !== FALSE) { /* no-op */ }
	elseif (stripos($matches[0], "<th") !== FALSE) { /* no-op */ }
	elseif (stripos($matches[0], "<td") !== FALSE) { /* no-op */ }
	else {
		$class = trim($matches[0], "<>");
		return "<div class=\"{$class}\">";
	}
	return $matches[0];
';
  $doc = preg_replace_callback($regex, create_function('$matches', $fcn), $doc);
  $regex = '#</[^>]*>#';
  // preg_match_all($regex, $doc, $matches);
  // dsm($matches, '$matches');
  $fcn = '
	// dsm($matches, \'$matches\');
	if (stripos($matches[0], "</a>") !== FALSE) { /* no-op */ }
	elseif (stripos($matches[0], "</span>") !== FALSE) { /* no-op */ }
	elseif (stripos($matches[0], "</sup>") !== FALSE) { /* no-op */ }
	elseif (stripos($matches[0], "</table>") !== FALSE) { /* no-op */ }
	elseif (stripos($matches[0], "</tr>") !== FALSE) { /* no-op */ }
	elseif (stripos($matches[0], "</th>") !== FALSE) { /* no-op */ }
	elseif (stripos($matches[0], "</td>") !== FALSE) { /* no-op */ }
	else {
		return "</div>";
	}
	return $matches[0];
';
  $doc = preg_replace_callback($regex, create_function('$matches', $fcn), $doc);

  return $doc;
}

/**
 * show images and add 'Replace' or 'Undo Replace' links if the HTML file is exported in Filtered HTML format
 */
function _doc_import_handle_images_xml(&$doc, $pending_path_xml) {
  $regex = '/< *img[^>]*src *= *["\']?/i';
  // preg_match($regex, $doc, $matches);
  // dsm($matches, '$matches');
  $pending_path_base = dirname($pending_path_xml);
  $doc = preg_replace($regex, "$0" . '/' . $pending_path_base . '/', $doc);
  // dsm($doc, '$doc');

  return $doc;
}

/**
 * copy images from imported XML documents
 */
function _doc_import_copy_images_xml($images, $pending_xml_doc) {
  // dsm($images, '$images');
  // dsm($pending_xml_doc, '$pending_xml_doc');

  $pending_path_xml = variable_get('doc_import_path_pending_xml', '');
  $target_path_xml = variable_get('doc_import_path_target_xml', '');
  if ($pending_path_xml == $target_path_xml) {
    return TRUE;
  }

  $file_directory_path = file_directory_path();
  $image_rel_path = trim(dirname($pending_xml_doc), '/');
  // dsm($image_rel_path, '$image_rel_path');
  $source_path_xml = $file_directory_path . '/' . $pending_path_xml;
  if (!empty($image_rel_path)) {
    $source_path_xml .= '/' . $image_rel_path;
  }
  // dsm($source_path_xml, '$source_path_xml');
  // make sure the target path exists
  $temp = explode('/', $image_rel_path);
  $target_path_xml = $file_directory_path . '/' . $target_path_xml;
  while (!empty($temp)) {
    $target_path_xml .= '/' . array_shift($temp);
    // dsm($target_path_xml, '$target_path_xml');
    if (!file_prepare_directory($target_path_xml, TRUE)) {
      if (!file_create_path($target_path_xml)) {
        form_error($form, t('Cannot create the directory for copying images.'));
      }
    }
  }
  // dsm($target_path_xml, '$target_path_xml');
  foreach ($images as $image) {
    $source = $source_path_xml . '/' . $image;
    $dest = $target_path_xml . '/' . $image;
    // dsm($source, '$source'); dsm($dest, '$dest');
    file_copy($source, $dest, FILE_EXISTS_REPLACE);
  }
}

/**
 * prepare content:
 * - change image paths
 */
function _doc_import_prepare_content_xml(&$content, &$images) {
  $pending_path_xml = variable_get('doc_import_path_pending_xml', '');
  $target_path_xml = variable_get('doc_import_path_target_xml', '');
  if ($pending_path_xml == $target_path_xml) {
    return TRUE;
  }

  // replace image paths
  // dsm($pending_path_xml, '$pending_path_xml');
  $regex = '/<img[^>].*' . str_replace('/', '\/', $pending_path_xml) . '.*\/>/i';
  // dsm($regex, '$regex');
  preg_match_all($regex, $content, $matches);
  // dsm($matches, '$matches');

  if (!is_array($images)) {
    $images = array();
  }
  if (!empty($matches) && !empty($matches[0])) {
    $matches = $matches[0];
    $regex = '/' . str_replace('/', '\/', $pending_path_xml) . '.*["\']/i';
    // dsm($regex, '$regex');
    foreach ($matches as $match) {
      preg_match($regex, $match, $temp);
      // dsm($temp, '$temp');
      if (!empty($temp[0])) {
        $temp1 = explode('"', $temp[0]);
        // dsm($temp1, '$temp1');
        if (!empty($temp1[0])) {
          $temp2 = explode('/', $temp1[0]);
          // dsm($temp2, '$temp2');
          $index = count($temp2) - 1;
          while ($index >= 0) {
            $temp3 = trim($temp2[$index]);
            if (!empty($temp3)) {
              if (!in_array($temp3, $images)) {
                $images[] = $temp3;
              }
              // dsm($images, '$images');
              break;
            }
          }
        }
      }
    }
  }
  // dsm($images, '$images');
}

/**
 * substitute image path by the target path for XML documents
 */
function _doc_import_substitute_xml_image_path(&$content) {
  $pending_path_xml = variable_get('doc_import_path_pending_xml', '');
  $target_path_xml = variable_get('doc_import_path_target_xml', '');
  if ($pending_path_xml == $target_path_xml) {
    return TRUE;
  }

  $regex = '/' . str_replace('/', '\/', $pending_path_xml) . '/';
  // preg_match_all($regex, $content, $matches);
  // dsm($matches, '$matches');
  // dsm($content, '$content');
  $content = preg_replace($regex, $target_path_xml, $content);
  // dsm($content, '$content');
}


