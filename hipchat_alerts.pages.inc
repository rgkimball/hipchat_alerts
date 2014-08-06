<?php
/**
 * Created by PhpStorm.
 * User: mlander
 * Date: 2/4/14
 * Time: 6:20 PM
 */

function hipchat_node_rets_page($node) {
  $output = '';
  //@todo: fix to get by mls or matrix id
  if (($items = field_get_items('node', $node, 'field_property_id'))
     && ($property = hipchat_query_get_by_mls_id($items[0]['value']))
     ) {

  }
  elseif (($items = field_get_items('node', $node, 'field_matrix_unique_id'))
    && ($property = hipchat_query_get_by_matrix_id($items[0]['value']))
  ) {

  }
  else {
    drupal_set_message('MLS ID not available for this property.', 'warning');
  }

  if(!empty($property)) {
    $output .= '<h2>Default Query Select</h2>';
    $output .= kprint_r($property, TRUE);
    $property_full = hipchat_query_get_by_matrix_id($property['Matrix_Unique_ID'], '');
    $output .= '<h2>All Query Select</h2>';
    $output .= kprint_r($property_full, TRUE);
    $form = drupal_get_form('hipchat_node_rets_form', $node, $property);
    $output .= drupal_render($form);
  }

//@todo: load property id
  return $output;
}

function hipchat_node_rets_form($form, $form_state, $node, $property) {

  $plugins = ctools_get_plugins('hipchat', 'field_map');
  $options = array();
  $default_value = array();
  $op = 'update';
  //add property data to verify exists
  $node->hipchat['data'] = $property;
  foreach($plugins as $field => $plugin) {
    $options[$plugin['name']] = $plugin['name'];
    if(isset($plugin[$op])
      && ($plugin[$op] === TRUE || (($function = ctools_plugin_load_function('hipchat', 'field_map', $field, $op)) && $function($node)))) {
      $default_value[] = $plugin['name'];
    }
  }
  $form['override_enable'] = array(
    '#type' => 'checkbox',
    '#title' => 'Override Default Maps',
  );
  $form['override'] = array(
    '#type' => 'checkboxes',
    '#title' => 'Override Default Maps',
    '#options' => $options,
    '#default_value' => $default_value,
    '#states' => array(

      // Hide the settings when the cancel notify checkbox is disabled.

      'invisible' => array(

        ':input[name="override_enable"]' => array('checked' => FALSE),
      ),
    ),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Update Node'),
  );
  return $form;
}

function hipchat_node_rets_form_submit($form, $form_state) {
  list($node, $property) = $form_state['build_info']['args'];
  $node = node_load($node->nid, null, true);
  $options = array();
  if($form_state['values']['override_enable'] === 1) {
    $options['override'] = $form_state['values']['override'];
  }
  hipchat_property_save_node($node, $property, $options);
  drupal_set_message('Operation Completed');
}