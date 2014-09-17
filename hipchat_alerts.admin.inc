<?php
/**
 * @file
 * Admin config pages
 * @author rkimball
 * @package hipchat_alerts
 * @version 0.0.1
 */

/**
 * Implements hook_form()
 * @param $form
 * @param $form_state
 * @return mixed
 */
function hipchat_alerts_config_form($form, &$form_state) {

  $form['header'] = array(
    '#markup' => '<div style="padding:10px;border:1px solid #BEBFB9;margin-bottom:10px;"><p>The HipChat Alerts module allows Drupal to publish certain alerts to a HipChat room using the v1 API. This module requires a <a href="http://hipchat.com/account/api">HipChat API Key</a>. </p></div>',
  );

  $form['hipchat_api_version'] = array(
    '#type' => 'radios',
    '#title' => t('API Version'),
    '#description' => t('Please select which version of the HipChat API your lobby will use.'),
    '#default_value' => variable_get('hipchat_api_version', null),
    '#required' => TRUE,
    '#options' => array(
      t('v1'),
      t('v2'),
    ),

  );

  $form['v1'] = array(
    '#type' => 'fieldset',
    '#states' => array(
      'visible' => array(
        ':input[name="hipchat_api_version"]' => array('value' => 0),
      ),
    ),
  );

  $form['v1']['header'] = array(
    '#markup' => '<h2>HipChat API v1 Settings</h2>',
  );

  $form['v1']['hipchat_v1_api_token'] = array(
    '#type' => 'textfield',
    '#title' => t('API Token'),
    '#description' => t('To generate a HipChat API v1 token, you will need to be a group administrator'),
    '#default_value' => variable_get('hipchat_v1_api_token',''), // fe2e7baec2d954125ce22937ff4c4a
    '#size' => 60,
    '#maxlength' => 128,
    '#required' => FALSE,
  );


  $form['v2'] = array(
    '#type' => 'fieldset',
    '#states' => array(
      'visible' => array(
        ':input[name="hipchat_api_version"]' => array('value' => 1),
      ),
    ),
  );

  $form['v2']['header'] = array(
    '#markup' => '<h2>HipChat API v2 Settings</h2> HipChat API v2 is not yet supported.',
  );

  $form['v2']['hipchat_v2_api_token'] = array(
    '#type' => 'textfield',
    '#title' => t('OAuth2 Token'),
    '#default_value' => variable_get('hipchat_v2_api_token',''),
    '#size' => 60,
    '#maxlength' => 128,
    '#required' => FALSE,
    '#disabled' => TRUE,
  );

  $form['v2']['hipchat_v2_api_key'] = array(
    '#type' => 'textfield',
    '#title' => t('OAuth2 Key'),
    '#default_value' => variable_get('hipchat_v2_api_key',''),
    '#size' => 60,
    '#maxlength' => 128,
    '#required' => FALSE,
    '#disabled' => TRUE,
  );

  $form['v2']['hipchat_v2_api_secret'] = array(
    '#type' => 'textfield',
    '#title' => t('OAuth2 Secret'),
    '#default_value' => variable_get('hipchat_v2_api_secret',''),
    '#size' => 60,
    '#maxlength' => 128,
    '#required' => FALSE,
    '#disabled' => TRUE,
  );

  $form['hipchat_room_id'] = array(
    '#type' => 'textfield',
    '#title' => t('Room ID'),
    '#default_value' => variable_get('hipchat_room_id',''),
    '#size' => 25,
    '#required' => TRUE,
    '#description' => t('All notifications sent from this module will be posted in this room.'),
  );

  $form['hipchat_watchdog_severity'] = array(
    '#type' => 'select',
    '#title' => t('Watchdog Severity Reporting'),
    '#default_value' => variable_get('hipchat_watchdog_severity', WATCHDOG_ERROR),
    '#description' => t('Choose the minimum level of severity to send watchdog notices to HipChat.'),
    '#options' => array(
      WATCHDOG_EMERGENCY => 'WATCHDOG_EMERGENCY ('.WATCHDOG_EMERGENCY.')',
      WATCHDOG_ALERT => 'WATCHDOG_ALERT ('.WATCHDOG_ALERT.')',
      WATCHDOG_CRITICAL => 'WATCHDOG_CRITICAL ('.WATCHDOG_CRITICAL.')',
      WATCHDOG_ERROR => 'WATCHDOG_ERROR ('.WATCHDOG_ERROR.')',
      WATCHDOG_WARNING => 'WATCHDOG_WARNING ('.WATCHDOG_WARNING.')',
      WATCHDOG_NOTICE => 'WATCHDOG_NOTICE ('.WATCHDOG_NOTICE.')',
      WATCHDOG_INFO => 'WATCHDOG_INFO ('.WATCHDOG_INFO.')',
      WATCHDOG_DEBUG => 'WATCHDOG_DEBUG ('.WATCHDOG_DEBUG.')',
    )
  );

  $form['site_environment'] = array(
    '#type' => 'checkbox',
    '#title' => 'Development environment?',
    '#description' => t('HipChat Alerts will not send messages if this is checked. Alerts are only relevant from production environments.'),
    '#default_value' => variable_get('site_environment',null),
  );

  return system_settings_form($form);
}

function hipchat_alerts_config_info($apivers,$token,$room) {

  if(variable_get('site_environment') == 1) {
    drupal_set_message(t('The site environment is currently set to development. HipChat Alerts will not send any messages.'),'warning');
  }

  $content = '';
  $form = array();

  if(!is_null($apivers)) {

    $content .= '<h2>Group Users</h2>';
    $content .= hipchat_users_table();

  } else {
    drupal_set_message(t('HipChat Alerts has not yet been configured. Please provide your API Key.'), 'error');
  }

  return $content;
  return system_settings_form($form);
}

function hipchat_alerts_config_sandbox_form($form, &$form_state) {
  $form['test_result'] = array(
    '#type' => 'markup',
    '#markup' => '<div id="test-result"></div>',
  );

  $form['send_content'] = array(
    '#type' => 'textarea',
  );

  $form['send_test'] = array(
    '#type' => 'submit',
    '#value' => t('Send Test'),
    '#submit' => array('hipchat_alerts_config_sandbox_form_submit'),
    '#ajax' => array(
      'callback' => 'hipchat_alerts_config_sandbox_form_callback',
      'wrapper' => 'test-result',
      'method' => 'replace',
      'effect' => 'fade',
    ),
  );

  return $form;
}

function hipchat_alerts_config_sandbox_form_submit($form, &$form_state) {
  $message = $form_state['values']['send_content'];
  _hipchat_alerts_config_sandbox_test($message);
  return $form;
}

function hipchat_alerts_config_sandbox_form_callback($form, &$form_state) {
  return $form['test_result']['#markup'];
}

function _hipchat_alerts_config_sandbox_test($message) {
  $success = hipchat_alerts_message($message);

  $message = ($success) ? 'Success' : 'Message was unable to send';
  $status = ($success) ? 'status' : 'error';

  watchdog('hipchat-alerts', $message, array(), WATCHDOG_ERROR);
  drupal_set_message($message, $status);
}
