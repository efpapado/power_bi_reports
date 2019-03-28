<?php

/**
 * Implements hook_field_formatter_info().
 */
function _power_bi_reports_field_formatter_info() {
  $info = [];
  $info['power_bi_reports_simple_embed_formatter'] = [
    'label' => t('Simple text-based formatter'),
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
  $power_bi_reports_data = variable_get('power_bi_reports_data', FALSE);
  if (!$power_bi_reports_data) {
    $power_bi_reports_data = power_bi_reports_sync_reports();
  }


  $element = [];
  foreach ($items as $delta => $item) {
    $report_id = $item['report_id'];
    $report_data = _power_bi_reports_get_report_data($report_id, $power_bi_reports_data);
    if (empty($report_data)) {
      throw new RuntimeException("Report '$report_id' not found."); // @TODO handle properly
    }

    $aa = 'aa';

    $element[$delta] = [
      // We create a render array to produce the desired markup,
      // "<p style="color: #hexcolor">The color code ... #hexcolor</p>".
      // See theme_html_tag().
      '#type' => 'html_tag',
      '#tag' => 'p',
      '#attributes' => [
        'style' => 'color: ' . $item['rgb'],
      ],
      '#value' => t('The color code in this field is code'),
    ];
  }
  return $element;
}

function _power_bi_reports_get_report_data($report_id, $power_bi_reports_data) {
  foreach ($power_bi_reports_data as $group_id => $group) {
    foreach ($group['reports'] as $report) {
      if ($report['id'] == $report_id) {
        return [
          'group_id' => $group_id,
          'report_data' => $report,
        ];
      }
    }
  }
  return [];
}
