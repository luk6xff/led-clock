
/* Device Status TAB */
var timer_updateDevInfo;

$('a[data-toggle=\"tab\"]').on('shown.bs.tab', function (e) {
  clearTimeout(timer_updateDevInfo);
  var target = $(e.target).attr("href")
  console.log('activated ' + target );

  // IE10, Firefox, Chrome, etc.
  if (history.pushState)
    window.history.pushState(null, null, target);
  else
    window.location.hash = target;

  if (target=='#tab_devinfo')  {
    $('#table_devinfo').bootstrapTable('refresh',{silent:true, url:'/devinfo'});
  }
});

// Create a timer to update device info data every n secondes
$('#tab_devinfo').on('load-success.bs.table',function (e,data) {
  console.log("tab_devinfo loaded");
  timer_updateDevInfo = setTimeout(function() {
    $('#table_devinfo').bootstrapTable('refresh',{silent: true, showLoading: false, url: '/devinfo'});
  }, 5000);
});

function nameFormatter(value, row) {
  //console.log("nameFormatter");
  //console.log(value);
  //console.log(row);
  var label = value + " " + "<span class='glyphicon " + row.glyph + " pull-left'></span>";
  return label;
}

function valueFormatter(value, row) {
  //console.log("valueFormatter");
  var label = "";
  if (row.unit != "") {
    row.unit = "[" + row.unit + "]"
  }
  label = value + " " + row.unit;
  return label;
}




/* Configuration TAB */
var activConfigTabPane = null;

$('.tab-pane a').on('shown.bs.tab', function(event){
    activConfigTabPane = $(event.target);         // active tab
    console.log("Active_ConfigTab: " + activConfigTabPane)
});

$('#save_button').click(function() {

    console.log('#save_button CLICKED')
    console.log("CurrentActiveTab: " + activConfigTabPane)

    var target = $(activConfigTabPane).attr("href")
    console.log('TARGET: href:' + target);
    jsonObj = [];
    $(target+" :input[class*='form-control']").each(function() {

        var id = $(this).attr("id");
        var value = $(this).val();

        item = {}
        item ["id"] = id;
        item ["value"] = value;

        jsonObj.push(item);
    });
    console.log(jsonObj);

    // Send post request
    const cfgName = target.replace("#", "");
    $.post(cfgName, jsonObj).done(function(data) {
        console.log("save_button " + JSON.stringify(data));
      }).fail(function(err) {
        console.log("err save_button " + JSON.stringify(err));
      });

});


$('#refresh_button').click(function() {

    console.log('#refresh_button CLICKED')
});




/* OTA TAB */

// $('form').submit(function(e){
//     e.preventDefault();
//     var form = $('#upload_form')[0];
//     var data = new FormData(form);
//     $.ajax({
//         url: '/update',
//         type: 'POST',
//         data: data,
//         contentType: false,
//         processData:false,
//         xhr: function() {
//             var xhr = new window.XMLHttpRequest();
//             xhr.upload.addEventListener('progress', function(evt) {
//                 if (evt.lengthComputable) {
//                     var per = evt.loaded / evt.total;
//                     $('#prg').html('progress: ' + Math.round(per*100) + '%');
//                 }
//             }, false);
//         return xhr;
//         },
//         success:function(d, s) {
//             console.log('OTA success!')
//         },
//         error: function (a, b, c) {
//             console.log('OTA error!')
//         }
//     });
//  });
