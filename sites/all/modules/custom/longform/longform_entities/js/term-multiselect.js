/**
 * Created by ericlotze on 14-10-29.
 *
 * Requires the underscore.js library
 *
 */



(function ($) {

    function enable_multiselect(item) {

        // Set this form ID (found in the hidden form input elements)
        var formId = item.find('input[name="form_id"]').attr('value');

        // Settings for this form
        item.data('settings', Drupal.settings.longformEntities[formId]);
        settings = item.data('settings');

        // Settings:
        // existingTerms = (object) An object containing already-selected terms, if any
        // selectDeepestOnly = (boolean) Whether to force selection of the deepest term
        // minimumSelectionCount = (integer) The minimum number of selections allowed
        // maximumSelectionCount = (integer) The maximum number of selections allowed
        // allowMultipleInstances = (boolean) Whether to allow the same term to be selected more than once

        // Global declarations and initializations
        itemNone = $('<li class="list-item item-none disabled"><a>Add items by selecting them from the list below.</a></li>');
        addedMessage = $('&nbsp;<span class="added-message text-success glyphicon glyphicon-ok pull-right"></span>');
        if (settings.selectDeepestOnly == true) {
            item.find('.term-selector.deepest').addClass('valid');
            item.find('.term-selector.not-deepest').addClass('invalid');
        }
        else {
            item.find('.term-selector').addClass('valid');
        }

        //Initialize UI

        // Insert the "none" placeholder.
        itemNone.prependTo(item.find('.build-container-list'));

        // Initialize selectors
        initializeSelectors(item);

        // shut off invalid selectors
        item.find('.taxonomy-switcher>li a.invalid, .taxonomy-switcher>li.disabled > a').on('click', function (event) {
            event.preventDefault();
        });

        // Clicking on a valid term adds it to the form return element and creates a new build-list item
        item.find('.taxonomy-switcher>li .term-selector.valid').on('click', function (event) {
            var clicked = $(this);
            addItem(item, clicked.attr('data-tid'), clicked.attr('data-term'));
        });

        // Reset everything on cancel
        item.find('.ticket-add-cancel').on('click', function (event) {
            initializeSelectors(item);
        });
    }

    /**
     * Deselect a term
     */
    function deselectTermId(scope, tid) {

        // Remove the selection from the term selectors and the build container
        scope.find('a.term-selector[data-tid="' + tid +'"]').parent('li').removeClass('bg-warning').removeClass('disabled').find('.added-message').remove();
        scope.find('.build-container-list a[data-tid="' + tid + '"]').parent('li').tooltip('destroy').end().remove();

        // Remove the term from dataStore
        var dataStore = scope.find('.data-store');
        dataStore.attr('value', _.without(dataStore.attr('value').split(','), tid).join(','));

        // Reset the submit button state
        setSubmitState(scope);

        // Set the item-none visibility state
        setItemNoneState(scope);
    }

    /**
     * Set the submit button state
     * @param scope
     */
    function setSubmitState(scope) {
        var minimumSelectionCount = scope.data('settings').minimumSelectionCount;
        var dataStore = scope.find('.data-store').attr('value');
        var submit = scope.find('.form-submit');
        if (_.isEmpty(dataStore) && !submit.hasClass('disabled')) {
            submit.addClass('disabled');
        }
        else if (dataStore.split(',').length < minimumSelectionCount && !submit.hasClass('disabled')) {
            submit.addClass('disabled');
        }
        else {
            submit.removeClass('disabled');
        }
    }

    /**
     * Set the itemNone state
     * @param scope
     */
    function setItemNoneState(scope) {
        var dataStore = scope.find('.data-store').attr('value');
        var itemNone = scope.find('.item-none');
        if (_.isEmpty(dataStore)) {
            itemNone.show();
        }
        else {
            itemNone.hide();
        }
    }

    function initializeSelectors(scope) {

        // destroy any existing selections
        scope.find('a.term-selector').parent('li').removeClass('bg-warning').removeClass('disabled').find('.added-message').remove();
        scope.find('.build-container-list li:not(.item-none) a').parent('li').tooltip('destroy').end().remove();
        scope.find('.data-store').attr('value', '');

        var initialState = scope.data('settings').existingTerms;
        if (_.isEmpty(initialState)) {

        }
        else {
            _.each(initialState, function(element, index){
                addItem(scope, index, element);
            });
        }
    }


    /**
     * Add an item to the selection set.
     *
     * Adds the new term button to the build container, attaches removal listeners,
     * adds the term to the data-store element (which is sent back to Drupal via form API),
     * and updates the UI.
     *
     * @param tid
     *  The numeric term ID (must be unique, which it should already be since it's a taxonomy term!)
     *
     * @param label
     *  The label to show in the button.
     *
     */
    function addItem(scope, tid, label) {

        var tidArray = [];

        var dataStore = scope.find('.data-store');
        
        // Set dataStore
        if (dataStore.attr('value') == '') {
            dataStore.attr('value', tid);
        }
        else {
            tidArray = dataStore.attr('value').split(',');

            // Only allow the term to be added once
            if (tidArray.indexOf(tid) != -1) {
                console.log('Term is already selected.');
                return;
            }
            // Add the term to the dataStore.
            tidArray.push(tid);
            dataStore.attr('value', tidArray.join(','))
        }

        // Update selection list item
        var listItem = scope.find('a[data-tid="' + tid + '"]');
        listItem.append(addedMessage.clone()).parents('li').removeClass('active').addClass('bg-warning').addClass('disabled');

        // Create, add, and show new 'selected' button
        var newButton = $('<li class="list-item bg-warning"><a href="#" class="click-remove" data-tid="'
        + tid + '"'
        + '>'
        + label
        + ' <span class="glyphicon glyphicon-remove-circle"></span></a></li>')
            .tooltip({'title':'Click to remove'})
            .find('.click-remove')
            .on('click', function (event) {
                deselectTermId(scope, $(this).attr('data-tid'));
            })
            .end();

        $(newButton).hide().appendTo(scope.find('.build-container-list')).fadeIn(200);

        // Update item none and submit states
        setItemNoneState(scope);
        setSubmitState(scope);
    }

    Drupal.behaviors.longformEntities = {
        attach: function(context,settings){

            if (context === document) {

                // Enable all of the multiselect elements on the page
                $('form.multiselect').each(function () {
                    enable_multiselect($(this));
                });
            }
        }
    }
})(jQuery)
