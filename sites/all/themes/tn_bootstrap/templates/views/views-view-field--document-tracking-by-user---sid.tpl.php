<?php

/**
 * @file
 * This template is used to print a single field in a view.
 *
 * It is not actually used in default Views, as this is registered as a theme
 * function which has better performance. For single overrides, the template is
 * perfectly okay.
 *
 * Variables available:
 * - $view: The view object
 * - $field: The field handler object that can process the input
 * - $row: The raw SQL result that can be used
 * - $output: The processed output that will normally be used.
 *
 * When fetching output from the $row, this construct should be used:
 * $data = $row->{$field->field_alias}
 *
 * The above will guarantee that you'll always get the correct data,
 * regardless of any changes in the aliasing that might happen if
 * the view is modified.
 */
?>
<?php
    $assignments = array(
	'21' => 'state-created text-danger bg-danger',
	'26' => 'state-importing text-warning',
	'31' => 'state-reviewing text-warning',
	'5' => 'state-approved text-success',
	'6' => 'state-published text-success bg-success',
    );
    $class = $assignments[$row->workflow_node_sid] ? $assignments[$row->workflow_node_sid] : 'state-unknown btn-tb-fixed';?>
    
    <div class="btn btn-block <?php print $class; ?> disabled"> <?php $assignments[$row->workflow_node_sid] ? print $row->_field_data['nid']['entity']->workflow_state_name : print '<b>Unknown</b>';?></div>

