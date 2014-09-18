<?php
/**
 * @file
 * Admin config pages.
 * @author rgkimball/gabesullice
 * @package hipchat_alerts
 * @version 1.0.0
 */

/**
 * Implements hook_form().
 */
function hipchat_alerts_config_form($form, &$form_state) {

  if (variable_get('hipchat_alerts_site_environment') == 1) {
    drupal_set_message(t('The site environment is currently set to development. HipChat Alerts will not send any messages.'), 'warning');
  }

  $form['header'] = array(
    '#markup' => '<div style="padding:10px;border:1px solid #BEBFB9;margin-bottom:10px;"><p>The HipChat Alerts module allows Drupal to publish certain alerts to a HipChat room using the v1 API. This module requires a <a href="http://hipchat.com/account/api">HipChat API Key</a>. </p></div>',
  );

  // Fields are grouped as v1 to prepare for v2 API integration.
  $form['v1'] = array(
    '#type' => 'fieldset',
    '#states' => array(
      'visible' => array(
        ':input[name="hipchat_api_version"]' => array('value' => 0),
      ),
    ),
  );

  $form['v1']['header'] = array(
    '#markup' => '<h2>HipChat API Settings</h2>',
  );

  $form['v1']['hipchat_alerts_v1_api_token'] = array(
    '#type' => 'textfield',
    '#title' => t('API Token'),
    '#description' => t('To generate a HipChat API v1 token, you will need to be a group administrator'),
    '#default_value' => variable_get('hipchat_alerts_v1_api_token', ''),
    '#size' => 60,
    '#maxlength' => 128,
    '#required' => FALSE,
  );

  $form['v1']['hipchat_alerts_room_id'] = array(
    '#type' => 'textfield',
    '#title' => t('Room ID'),
    '#default_value' => variable_get('hipchat_alerts_room_id', ''),
    '#size' => 25,
    '#required' => TRUE,
    '#description' => t('All notifications sent from this module will be posted in this room.'),
  );

  $form['hipchat_alerts_watchdog_severity'] = array(
    '#type' => 'select',
    '#title' => t('Watchdog Severity Reporting'),
    '#default_value' => variable_get('hipchat_alerts_watchdog_severity', WATCHDOG_ERROR),
    '#description' => t('Choose the minimum level of severity to send watchdog notices to HipChat.'),
    '#options' => array(
      WATCHDOG_EMERGENCY => t('@severity: Emergency',
        array('@severity' => WATCHDOG_EMERGENCY)),
      WATCHDOG_ALERT => t('@severity: Alert',
        array('@severity' => WATCHDOG_ALERT)),
      WATCHDOG_CRITICAL => t('@severity: Critical',
        array('@severity' => WATCHDOG_CRITICAL)),
      WATCHDOG_ERROR => t('@severity: Error',
        array('@severity' => WATCHDOG_ERROR)),
      WATCHDOG_WARNING => t('@severity: Warning',
        array('@severity' => WATCHDOG_WARNING)),
      WATCHDOG_NOTICE => t('@severity: Notice',
        array('@severity' => WATCHDOG_NOTICE)),
      WATCHDOG_INFO => t('@severity: Info',
        array('@severity' => WATCHDOG_INFO)),
      WATCHDOG_DEBUG => t('@severity: Debug',
        array('@severity' => WATCHDOG_DEBUG)),
      NULL => t('OFF'),
    ),
  );

  $form['hipchat_alerts_site_environment'] = array(
    '#type' => 'checkbox',
    '#title' => 'Development environment?',
    '#description' => t('HipChat Alerts will not send messages if this is checked. Alerts are only relevant from production environments.'),
    '#default_value' => variable_get('hipchat_alerts_site_environment', NULL),
  );

  return system_settings_form($form);
}

/**
 * Builds admin group info page.
 *
 * @param $api
 * @param $token
 *
 * @return string
 */
function hipchat_alerts_config_info($token, $api) {

  $token = $token ?: variable_get('hipchat_alerts_v1_api_token', '');
  $api = $api ?: HIPCHAT_ALERTS_API_VERSION;

  if (variable_get('hipchat_alerts_site_environment') == 1) {
    drupal_set_message(t('The site environment is currently set to development. HipChat Alerts will not send any messages.'), 'warning');
  }

  $content = '';

  if (isset($api) && strlen($token) > 0) {

    $content .= '<h2>Group Users</h2>';
    $content .= hipchat_alerts_users_table(variable_get('hipchat_alerts_room_id', ''));

  }
  elseif (strlen($token) <= 0) {
    drupal_set_message(t('HipChat Alerts has not yet been configured. Please provide your API Key.'), 'error');
  }


  return $content;
}

/**
 * Implements hook_form().
 *
 * Builds admin sandbox page.
 */
function hipchat_alerts_config_sandbox_form($form, &$form_state) {

  if (variable_get('hipchat_alerts_site_environment') == 1) {
    drupal_set_message(t('The site environment is currently set to development. HipChat Alerts will not send any messages.'), 'warning');
  }

  $form['header'] = array(
    '#markup' => '<div style="padding:10px;border:1px solid #BEBFB9;margin-bottom:10px;"><p>Use this form to test the messaging system before turning on other types of reporting. Check that your messages are sending to the right room.</p></div>',
  );

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

/**
 * Submit sandbox form.
 *
 * @param $form
 * @param $form_state
 *
 * @return mixed
 */
function hipchat_alerts_config_sandbox_form_submit($form, &$form_state) {
  $message = $form_state['values']['send_content'];
  _hipchat_alerts_config_sandbox_test($message);
  return $form;
}

/**
 * Formats results.
 *
 * @param $form
 * @param $form_state
 *
 * @return mixed
 */
function hipchat_alerts_config_sandbox_form_callback($form, &$form_state) {
  return $form['test_result']['#markup'];
}

/**
 * Check if test message was successful.
 *
 * @param $message
 */
function _hipchat_alerts_config_sandbox_test($message) {
  $success = hipchat_alerts_message($message, 'HC-Alerts Test');

  $message = ($success) ? 'Success' : 'Message was unable to send';
  $status = ($success) ? 'status' : 'error';

  drupal_set_message(t('@message', array('@message' => $message)), $status);
}
