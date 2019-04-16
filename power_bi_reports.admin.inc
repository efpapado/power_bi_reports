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
  $form['power_bi_reports_password'] = [ // @TODO fix this
    '#type' => 'password',
    '#title' => t('Password'),
    '#default_value' => variable_get('power_bi_reports_password'),
    '#required' => TRUE,
  ];

  return system_settings_form($form);
}

/**
 * Menu callback to get module status.
 */
function power_bi_reports_status_form($form, &$form_state) {
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

  // Sync reports button.
  $form['sync_reports'] = [
    '#type' => 'submit',
    '#name' => 'sync_reports',
    '#value' => t('Sync reports'),
  ];

  // List of reports, groupped by group.
  $form['groups_reports'] = [
    '#type' => 'container',
    '#prefix' => "<h2>" . t('Available reports') . "</h2>",
  ];
  $local_reports = variable_get('power_bi_reports_data');
  foreach ($local_reports as $group_id => $group_data) {
    $group_element = _power_bi_reports_status_get_group_element($group_data, $validate = FALSE);
    $form['groups_reports'][] = $group_element;
  }

  // Reports form element.
  $form['reports'] = power_bi_reports_get_form_element($form, $form_state);

  return $form;
}

function power_bi_reports_status_form_submit($form, &$form_state) {
  $trigger = $form_state['triggering_element'];

  if ($trigger["#name"] == 'sync_reports') {
    power_bi_reports_sync_reports();
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

function _power_bi_reports_status_get_group_element($group_data, $validate = FALSE) {
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
    $element[$group_id][$report_id] = [
      '#type' => 'fieldset',
      '#title' => $report_name,
      '#description' => "Report ID: $report_id",
    ];
    $element[$group_id][$report_id]['details'] = [
      '#type' => 'markup',
    ];
    if ($validate) {
      $embed_token = power_bi_reports_generate_report_embed_token($group_id, $report_id);
      if ($embed_token) {
        $element[$group_id][$report_id]['details']['#markup'] = 'Report is OK'; // @TODO fix
      }
      else {
        $element[$group_id][$report_id]['details']['#markup'] = '<strong>Report is NOT OK</strong>'; // @TODO fix
      }
    }
  }
  return $element;
}
