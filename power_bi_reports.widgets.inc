<?php

/**
 * Implements hook_field_widget_info().
 */
function _power_bi_reports_field_widget_info() {
  $info = [];
  $info['power_bi_reports_select_widget'] = [
    'label' => t('Select'),
    'field types' => ['power_bi_reports_report'],
  ];
  return $info;
}

/**
 * Implements hook_field_widget_form().
 */
function _power_bi_reports_field_widget_form(&$form, &$form_state, $field, $instance, $langcode, $items, $delta, $element) {
  $value = isset($items[$delta]['report_id']) ? $items[$delta]['report_id'] : '';

  $widget = $element;
  $widget['#delta'] = $delta;

  switch ($instance['widget']['type']) {
    case 'power_bi_reports_select_widget':
      $element['report_id'] = power_bi_reports_get_form_element($form, $form_state, $value);
      $reports_last_sync = variable_get('power_bi_reports_last_sync_reports');
      if ($reports_last_sync) {
        $message = t('The reports were last synced at: !when', ['!when' => date('d/m/Y H:i', $reports_last_sync)]) . '<br>';
      }
      else {
        $message = t('The reports have never been synced.') . '<br>';
      }
      $message .= t('Click <a href="@status-page">here</a> to visit the status page.', [
        '@status-page' => url('admin/config/services/power-bi-reports/status'),
      ]);
      $element['message'] = [
        '#type' => 'markup',
        '#prefix' => '<div>',
        '#markup' => $message,
        '#suffix' => '</div>',
      ];
      break;
  }

  return $element;
}
