<?php

/**
 * Menu callback to configure module settings.
 */
function power_bi_reports_admin_form($form, &$form_state) {
  $form['power_bi_reports_tenant_id'] = [
    '#type' => 'textfield',
    '#title' => t('Tenant ID'),
    '#default_value' => variable_get('power_bi_reports_tenant_id'),
    '#required' => TRUE,
  ];
  $form['power_bi_reports_resource'] = [
    '#type' => 'textfield',
    '#title' => t('Resource'),
    '#default_value' => variable_get('power_bi_reports_resource', 'https://analysis.windows.net/powerbi/api'),
    '#required' => TRUE,
  ];
  $form['power_bi_reports_client_id'] = [
    '#type' => 'textfield',
    '#title' => t('Client ID'),
    '#default_value' => variable_get('power_bi_reports_client_id'),
    '#required' => TRUE,
  ];
  $form['power_bi_reports_username'] = [
    '#type' => 'textfield',
    '#title' => t('Username'),
    '#default_value' => variable_get('power_bi_reports_username'),
    '#required' => TRUE,
  ];
  $existing_password = variable_get('power_bi_reports_password', '');
  $form['power_bi_reports_password'] = [
    '#type' => 'password',
    '#title' => t('Password'),
    '#required' => !$existing_password,
  ];

  $form['#submit'][] = 'power_bi_reports_admin_form_submit';

  return system_settings_form($form);
}

/**
 * Admin form submit callback.
 *
 * If there's already a stored password and the user left the field empty, make
 * sure that the password will not be deleted.
 *
 * @param $form
 * @param $form_state
 */
function power_bi_reports_admin_form_submit($form, &$form_state) {
  $existing_password = variable_get('power_bi_reports_password', '');
  $typed_password = $form_state['values']['power_bi_reports_password'];
  if ($existing_password && !$typed_password) {
    $form_state['values']['power_bi_reports_password'] = $existing_password;
  }
}

/**
 * Menu callback to get module status.
 */
function power_bi_reports_status_form($form, &$form_state) {
  if (!power_bi_reports_is_configured()) {
    $msg = t('You have been redirected to the settings form.');
    drupal_set_message($msg, 'warning');
    drupal_goto('admin/config/services/power-bi-reports/settings');
  }
  $form = [];

  $form['token_renew'] = [
    '#type' => 'submit',
    '#name' => 'token_renew',
    '#value' => t('Renew access token'),
  ];

  // Sync reports message.
  $reports_last_sync = variable_get('power_bi_reports_last_sync_reports');
  $form['reports_last_sync'] = [
    '#type' => 'markup',
    '#prefix' => '<div>',
    '#suffix' => '</div>',
  ];
  if ($reports_last_sync) {
    $form['reports_last_sync']['#markup'] = t('The reports were last synced at: !when', ['!when' => date('d/m/Y H:i', $reports_last_sync)]);
  }
  else {
    $form['reports_last_sync']['#markup'] = t('The reports have never been synced.');
  }

  // Batch sync reports button.
  $form['batch_sync_reports'] = [
    '#type' => 'submit',
    '#name' => 'batch_sync_reports',
    '#value' => t('Batch sync reports'),
  ];

  // Batch sync reports button.
  $form['renew_embed_tokens'] = [
    '#type' => 'submit',
    '#name' => 'renew_embed_tokens',
    '#value' => t('Renew embed tokens'),
  ];

  // List of reports, groupped by group.
  $form['groups_reports'] = [
    '#type' => 'container',
    '#prefix' => "<h2>" . t('Available reports') . "</h2>",
  ];
  $local_reports = variable_get('power_bi_reports_data');
  foreach ($local_reports as $group_id => $group_data) {
    $group_element = _power_bi_reports_status_get_group_element($group_data);
    $form['groups_reports'][] = $group_element;
  }

  // Reports form element.
//  $form['reports'] = power_bi_reports_get_form_element($form, $form_state);

  return $form;
}

function power_bi_reports_status_form_submit($form, &$form_state) {
  $trigger = $form_state['triggering_element'];

  if ($trigger["#name"] == 'batch_sync_reports') {
    power_bi_reports_batch_sync_reports_init();
  }
  elseif ($trigger["#name"] == 'renew_embed_tokens') {
    power_bi_reports_renew_embed_tokens_init();
  }
  elseif ($trigger["#name"] == 'token_renew') {
    $token_renew = power_bi_reports_get_auth_token(TRUE);
    if ($token_renew) {
      drupal_set_message(t('The access token has successfully been renewed.'));
    }
    else {
      drupal_set_message(t('There was a problem on the access token renewal.'), 'error');
    }

  }
}

function _power_bi_reports_status_get_group_element($group_data) {
  $group_id = $group_data['id'];
  $element[$group_id] = [
    '#type' => 'fieldset',
    '#title' => $group_data['name'],
    '#description' => "Group ID: $group_id",
  ];

  $group_reports = $group_data['reports'];
  foreach ($group_reports as $report_data) {
    $report_id = $report_data['id'];
    $report_name = $report_data['name'];

    $embed_token = power_bi_reports_get_local_report_embed_token($report_id, $check_expiration = FALSE);

    $element[$group_id][$report_id] = [
      '#type' => 'fieldset',
      '#title' => $report_name,
      '#description' => "Report ID: $report_id",
    ];
    $element[$group_id][$report_id]['details'] = [
      '#type' => 'markup',
    ];
    if (!$embed_token) {
      $msg = t('Possible report problem');
      $element[$group_id][$report_id]['details']['#markup'] = '<div class="messages warning">' . $msg . '</div>';
      $msg = t('There is not a locally cached embed token for this report. If the message persists after you renew the embed tokens, it might indicate that you don\'t have the necessary access to embed this report.');
      $element[$group_id][$report_id]['details']['#markup'] .= '<em>' . $msg . '</em>';
    }
  }
  return $element;
}
