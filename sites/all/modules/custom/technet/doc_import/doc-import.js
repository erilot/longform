// $Id: doc-import.js $

/**
 * @file doc-import.js
 * effects of doe import module
 */

(function ($) {
	Drupal.Doc_Import = {};

	Drupal.Doc_Import.nodeTypeFields = function() {
		$('#edit-content-type option').each(function() {
			var nodeType = $(this).val();
			// alert(nodeType);
			var optionIsSelected = $(this).attr('selected');
			// alert(optionIsSelected);
			if (optionIsSelected) {
				// alert('selected');
				$('#node_type_' + nodeType + '_fieldset_wrapper').show();
			}
			else {
				// alert('not selected');
				$('#node_type_' + nodeType + '_fieldset_wrapper').hide();
			}
		});
	};

	Drupal.Doc_Import.nodeTypeFieldsOnChange = function() {
		$('#edit-content-type').change(function() {
			$(Drupal.Doc_Import.nodeTypeFields);
		});
	};

	$(Drupal.Doc_Import.nodeTypeFields);
	$(Drupal.Doc_Import.nodeTypeFieldsOnChange);
})(jQuery);

