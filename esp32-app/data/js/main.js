
/* Device Status TAB */
var timer_updateDevInfo;

// $(document).on('shown.bs.tab', 'a[data-toggle="tab"]', function (e) {

//   alert(e.target.href);
// })

$('a[data-toggle=\"tab\"]').on('shown.bs.tab', function (e) {
  clearTimeout(timer_updateDevInfo);
  var target = $(e.target).attr("href")
  console.log('activated ' + target);
  // IE10, Firefox, Chrome, etc.
  // if (history.pushState) {
  //   window.history.pushState(null, null, target);
  // }
  // else {
  //   window.location.hash = target;
  // }
  localStorage.setItem('activeTab', $(e.target).attr('href'));

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


/* TABs */
$(document).ready(function() {

  var activeTab = localStorage.getItem('activeTab');
  if (activeTab) {
    $('#tab a[href="' + activeTab + '"]').tab('show');
  }

  $('.nav-tabs a').click(function(){
    $(this).tab('show');
  });

  if (activConfigTabPane == null) {
    $('.cfg-buttons').find('button').attr("disabled", true);
  }
})

/* Configuration TAB */
var activConfigTabPane = null;

$('.tab-pane a').on('shown.bs.tab', function(event) {
    activConfigTabPane = $(event.target);         // active tab
    console.log("Active_ConfigTab: " + activConfigTabPane)
    if (activConfigTabPane != null) {
      $('.cfg-buttons').find('button').attr("disabled", false);
    }
    // Refresh data on config page
    $('#cfg_refresh_button').trigger('click');
});


// CFG_SAVE_BUTTON
$('#cfg_save_button').click(function() {

    console.log('#cfg_save_button CLICKED')
    console.log("CurrentActiveTab: " + activConfigTabPane)

    var target = $(activConfigTabPane).attr("href")
    console.log('TARGET: href:' + target);
    dataJson = [];
    // Input form
    $(target+" :input[class*='form-control']").each(function() {

        var id = $(this).attr("id");
        var value = $(this).val();

        var item = {[id] : value}
        dataJson.push(item);
    });

    // Checkbox
    $(target+" :input[class*='switcher-input']").each(function() {

      var id = $(this).attr("id");
      var value = $(this).is(':checked');

      var item = {[id] : value}
      dataJson.push(item);
    });

    // Select
    $(target+" :input[class*='custom-select']").each(function() {

      var id = $(this).attr("id");
      var value = $(this).val();

      var item = {[id] : value}
      dataJson.push(item);
    });

    console.log(dataJson);

    // Send post request
    const cfgName = target.replace("#", "");
    const cfgData = { [cfgName] : dataJson }
    $.ajax({
      url:"dev-cfg-save",
      type:"POST",
      data: JSON.stringify(cfgData),
      contentType:"application/json",
      dataType:"json",
      success: function(data){
        console.log("cfg_save_button - success" + JSON.stringify(data));
      },
      error: function(data){
          console.log("cfg_save_button - failure" + JSON.stringify(data));
      }
    });
});


// CFG_REFRESH_BUTTON
$('#cfg_refresh_button').click(function() {
    console.log('#cfg_refresh_button CLICKED')
    var target = $(activConfigTabPane).attr("href")
    const cfgName = target.replace("#", "");

    $.getJSON("dev-cfg-read", { cfg: cfgName }, function(data) {
      var items = [];
      const json = data;
      console.log(json);
      if (json) {
        if (json.hasOwnProperty(cfgName)) {
          console.log("It contains "+cfgName);
          $.each(json[cfgName], function(i, item) {
            //console.log(item);
            var keys = Object.keys(item);
            for(var i=0; i<keys.length; i++){
              var key = keys[i];
              //console.log(key, item[key]);

              // Is checkbox
              if ($('#'+key).hasClass("switcher-input")) {
                $('#'+key).prop('checked', item[key]).trigger('change');
                //console.log("Checkbox:")
              }
              // Is select
              else if ($('#'+key).hasClass("custom-select")) {
                $('#'+key).val(item[key]).trigger('change');
              }
              // Is Input form
              else {
                $('#'+key).val(item[key]).trigger('change');
              }
            }
          });
        }
      }
    })
    .done(function(data) {
      console.log("success: "+cfgName+" - "+data);
    })
    .fail(function(data) {
      console.log("error: "+cfgName+" - "+data);
    })
    .always(function(data) {
      console.log("finished: "+cfgName+" - "+data);
    });
});



///> Time cfg utils
// Timezone
function changeTimeTimezoneView() {
  var elem = $('#time-timezone-num').find('option:selected');
  if (elem.val() == '2') {
    $('#div-time-timezone-2').show();
  } else {
    $('#div-time-timezone-2').hide();
  }
  console.log(elem.val());
}

$('#time-timezone-num').change(function() {
  changeTimeTimezoneView();
});

$(changeTimeTimezoneView()); // Called on document.ready


// Ntp synchronization
function changeCfgTimeNtpView() {
    if ($('#time-ntp-enable').is(':checked')) {
    $('.cfg-time-ntp-settings').show();
  } else {
    $('.cfg-time-ntp-settings').hide();
  }
}

$('#time-ntp-enable').change(function() {
  changeCfgTimeNtpView();
});

$(changeCfgTimeNtpView()); // Called on document.ready


///> Weather cfg utils
function changeCfgWeatherView() {
  if ($('#weather-update-enable').is(':checked')) {
    $(".cfg-weather-settings").show();
  } else {
    $(".cfg-weather-settings").hide();
  }
}

$('#weather-update-enable').change(function() {
  changeCfgWeatherView();
});

$(changeCfgWeatherView()); // Called on document.ready


///> Display cfg utils
function changeDisplayIntensityView() {
  if ($('#display-auto-intensity').is(':checked')) {
    $("#display-intensity-val").prop("disabled", true);
  } else {
    $("#display-intensity-val").prop("disabled", false);
  }
}

$('#display-auto-intensity').change(function() {
  changeDisplayIntensityView();
});

$(changeDisplayIntensityView()); // Called on document.ready


$(document).ready(function() {
  $("#show_hide_password a").on('click', function(event) {
      event.preventDefault();
      if($('#show_hide_password input').attr("type") == "text"){
          $('#show_hide_password input').attr('type', 'password');
          $('#show_hide_password i').addClass( "glyphicon-eye-close" );
          $('#show_hide_password i').removeClass( "glyphicon-eye-open" );
      }else if($('#show_hide_password input').attr("type") == "password"){
          $('#show_hide_password input').attr('type', 'text');
          $('#show_hide_password i').removeClass( "glyphicon-eye-close" );
          $('#show_hide_password i').addClass( "glyphicon-eye-open" );
      }
  });
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
