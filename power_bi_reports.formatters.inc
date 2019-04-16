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
  $power_bi_reports_data = variable_get('power_bi_reports_data', FALSE); // @TODO use drupal_static, here and everywhere else
  if (!$power_bi_reports_data) {
    $power_bi_reports_data = power_bi_reports_sync_reports();
  }

  $element = [];
  foreach ($items as $delta => $item) {
    $report_id = $item['report_id'];
    $report_data = _power_bi_reports_get_report_data($report_id, $power_bi_reports_data);
    $group_id = $report_data['group_id'];

    if (empty($report_data)) {
      throw new RuntimeException("Report '$report_id' not found."); // @TODO handle properly
    }

    $element[$delta] = [
      '#type' => 'html_tag',
      '#tag' => 'div',
      '#attributes' => [
//        'allowfullscreen' => 'true',
//        'width' => 1200,
//        'frameborder' => 0,
//        'height' => 1250,
//        'scrolling' => "no",
//        'src' => $report_data["report_data"]["embedUrl"],
        'class' => [
          "power-bi-report",
        ],
        'data-power-bi-delta' => $delta,
      ],
    ];

    dpm($report_data["report_data"], 'report_data');

    // @TODO handle this
    $embed_token = power_bi_reports_get_report_embed_token($group_id, $report_id);
    $embed_token_decoded = json_decode($embed_token);
    dpm($embed_token_decoded, '$embed_token_decoded');

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
