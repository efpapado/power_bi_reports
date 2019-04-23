(function ($) {

  // Helpers:
  // https://microsoft.github.io/PowerBI-JavaScript/demo/v2-demo/index.html#
  // https://github.com/Microsoft/PowerBI-JavaScript/wiki/Embedding-Basics

  Drupal.behaviors.power_bi_reports = {
    attach: function (context, settings) {
      var that = this;

      var $reports = $("[data-power-bi-report-id]", context);
      if (!$reports.length) {
        return;
      }

      $reports.each(function () {
        var $this = $(this);
        that.initiate($this);
      });
    },

    initiate: function ($el) {
      var reportID = $el.attr('data-power-bi-report-id');
      var groupID = $el.attr('data-power-bi-group-id');
      var embedUrl = $el.attr('data-power-bi-embed-url');

      // Read embed application token from textbox.
      var accessToken = 'AAAAAAAA'; // @TODO

      var endpoint = '/power-bi-reports/get-embed-token/' + reportID;
      var jqxhr = $.get(endpoint)
          .done(function (data) {
            accessToken = data;

            // Get models. models contains enums that can be used.
            var models = window['powerbi-client'].models;

            // Embed configuration used to describe the what and how to embed.
            // This object is used when calling powerbi.embed.
            // This also includes settings and options such as filters.
            // You can find more information at
            // https://github.com/Microsoft/PowerBI-JavaScript/wiki/Embed-Configuration-Details.
            var config = {
              type: 'report',
              tokenType: models.TokenType.Embed,
              accessToken: accessToken,
              embedUrl: embedUrl,
              id: reportID,
              pageView: "fitToWidth",
              // permissions: permissions,
              // settings: {
              //   filterPaneEnabled: true,
              //   navContentPaneEnabled: true
              // }
            };

            // Get a reference to the embedded report HTML element.
            var embedContainer = $el.find('.power-bi-report')[0];

            var $messages = $el.find('.power-bi-report-messages');

            // Embed the report and display it within the div container.
            var report = powerbi.embed(embedContainer, config);

            // Report.off removes a given event handler if it exists.
            report.off("loaded");

            report.on("error", function (event) {
              console.error(event);
              $messages.html($messages.html() + '<p>' + event.detail.detailedMessage + '</p>');
              report.off("error");
            });

          })
          .fail(function (e) {
            console.error(e);
          });
    }
  };

})(jQuery);
