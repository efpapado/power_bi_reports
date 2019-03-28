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
      break;
  }

  return $element;
}

function _power_bi_reports_get_grouped_options($power_bi_reports_data) {
  $options = [];
  foreach ($power_bi_reports_data as $group) {
    foreach ($group['reports'] as $report) {
      $options[$group['name']][$report['id']] = $report['name'];
    }
  }
  return $options;
}