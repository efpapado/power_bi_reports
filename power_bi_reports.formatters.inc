<?php

/**
 * Implements hook_field_formatter_info().
 */
function _power_bi_reports_field_formatter_info() {
  $info = [];
  $info['power_bi_reports_simple_embed_formatter'] = [
    'label' => t('JS formatter'),
    'field types' => ['power_bi_reports_report'],
  ];
  return $info;
}

/**
 * Implements hook_field_formatter_view().
 */
function _power_bi_reports_field_formatter_view($entity_type, $entity, $field, $instance, $langcode, $items, $display) {
  $element = [];

  // https://community.powerbi.com/t5/Service/How-to-Embed-Power-BI-Reports-into-Drupal-based-website/td-p/575822
  // https://docs.microsoft.com/en-us/power-bi/developer/embed-sample-for-customers

  switch ($display['type']) {
    case 'power_bi_reports_simple_embed_formatter':
      $element = _power_bi_reports_simple_embed_formatter($entity_type, $entity, $field, $instance, $langcode, $items, $display);
      break;
  }

  return $element;
}

function _power_bi_reports_simple_embed_formatter($entity_type, $entity, $field, $instance, $langcode, $items, $display) {
  $element = [];
  foreach ($items as $delta => $item) {
    $report_id = $item['report_id'];
    $report_data = _power_bi_reports_get_report_data($report_id);
    $group_id = $report_data['group_id'];

    if (empty($report_data)) {
      throw new RuntimeException("Report '$report_id' not found."); // @TODO handle properly
    }

    $element[$delta] = [
      '#theme' => 'power_bi_report',
      '#report_id' => $report_id,
      '#group_id' => $group_id,
      '#embed_url' => $report_data["report_data"]["embedUrl"],
      '#delta' => $delta,
    ];
  }

  // Add the js file of the library.
  $element['#attached']['libraries_load'][] = ['powerbi_javascript'];
  // Add our custom js file.
  $filepath = drupal_get_path('module', 'power_bi_reports') . '/js/power_bi_reports.js';
  $element['#attached']['js'][$filepath] = ['weight' => 5000];

  return $element;
}

