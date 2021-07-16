/**
 * Copyright (c) 2020 luk6xff
 *
 * @summary LedClock webserver client scripts
 * @author luk6xff <lukasz.uszko@gmail.com>
 *
 * Created at: 2020-11-20
 */

/* Device Status TAB */
var timer_updateDevInfo;

$('a[data-toggle=\"tab\"]').on('shown.bs.tab', function (e) {
  clearTimeout(timer_updateDevInfo);
  var target = $(e.target).attr("href")
  console.log('activeTab: ' + target);
  // IE10, Firefox, Chrome, etc.
  // if (history.pushState) {
  //   window.history.pushState(null, null, target);
  // }
  // else {
  //   window.location.hash = target;
  // }
  localStorage.setItem('activeTab', $(e.target).attr('href'));

  // Update Sysinfo
  if (target=='#tab_devapp') {
    getAppSystemInfo();
  }
});

/**
 * @summary Create a timer to update system info data every n secondes
 */
$('#tab_devapp').on('load-success.bs.table',function (e,data) {
  console.log("tab_devapp loaded");
  timer_updateDevInfo = setTimeout(function() {
    getAppSystemInfo();
  }, 5000);
});

/**
 * @summary Name column formatter for app-sysinfo-table
 */
function nameFormatter(value, row) {
  var label = value;
  return label;
}

/**
 * @summary Value column formatter for app-sysinfo-table
 */
function valueFormatter(value, row) {
  //console.log("valueFormatter");
  var label = "";
  if (row.unit != "") {
    row.unit = "[" + row.unit + "]"
  }
  label = value + " " + row.unit;
  return label;
}


/**
 * @summary Get info from system
 */
function getAppSystemInfo() {
  clearTimeout(timer_updateDevInfo);
  $('#app-sysinfo-table').bootstrapTable('refresh',{silent:true, url:'/dev-app-sysinfo'});
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

  hideAlerts();
  updateAppTime();
})

/* Succes, Error Alerts */
function hideAlerts() {
  $("#success-alert").hide();
  $("#error-alert").hide();
  $('#upload_status').html('');
}

function showAlert(alertType, text) {
  if (alertType == 'success') {
    $("#success-alert").text(text);
    $("#success-alert").fadeTo(2000, 500).slideUp(500, function() {
      $("#success-alert").slideUp(500);
    });
  }
  else if (alertType == 'error') {
    $("#error-alert").text(text);
    $("#error-alert").fadeTo(2000, 500).slideUp(500, function() {
      $("#error-alert").slideUp(500);
    });
  }
  else {
    console.log("Invalid Alert type: " + alertType);
  }
}

/* Configuration TAB */
var activConfigTabPane = null;

$('.tab-pane a').on('shown.bs.tab', function(event) {
    activConfigTabPane = $(event.target);         // active tab
    console.log("Active_ConfigTab: " + activConfigTabPane.attr("href"))
    if (activConfigTabPane != null) {
      $('.cfg-buttons').find('button').attr("disabled", false);
    }

    // Refresh data only on cfg tabs
    if (activConfigTabPane.attr("href").includes("dev-cfg"))
    {
      $('#cfg_refresh_button').trigger('click');
    }

    // // Refresh system info data only on app system tab
    if (activConfigTabPane.attr("href").includes("dev-app-sysinfo"))
    {
      getAppSystemInfo();
    }
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
      value = value * 1;
      var item = {[id] : value.toString()}
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
      success: function(data) {
        console.log("cfg_save_button - success" + JSON.stringify(data));
        showAlert('success', JSON.stringify(data));
      },
      error: function(data){
          console.log("cfg_save_button - failure" + JSON.stringify(data));
          showAlert('error', JSON.stringify(data));
          $('#cfg_refresh_button').trigger('click');
      }
    });
});


// CFG_REFRESH_BUTTON
$('#cfg_refresh_button').click(function() {
    console.log('#cfg_refresh_button CLICKED')
    var target = $(activConfigTabPane).attr("href")
    const cfgName = target.replace("#", "");

    $.getJSON("dev-cfg-read", { cfg: cfgName }, function(data) {
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
      showAlert('error', "Refresh config data failed! " + JSON.stringify(data));
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

// Hide/Show passwords
$(document).ready(function() {
  $(".pass-input a").on('click', function(event) {
    parentId = '#'+$(this).parent().parent().attr('id');
    console.log(parentId)
      if($(parentId+' input').attr("type") == "text"){
          $(parentId+' input').attr('type', 'password');
          $(parentId+' object').attr('data', 'img/eye-closed.svg');
      }
      else if($(parentId+' input').attr("type") == "password"){
          $(parentId+' input').attr('type', 'text');
          $(parentId+' object').attr('data', 'img/eye.svg');
      }
  });
});


///> APP TAB
// App set date and time
$('#app-date-time-button').click(function() {
  console.log('#app-date-time-button CLICKED')

  var dtRead = new Date($('#app-date').val()+'T'+$('#app-time').val());
  var tzDiff = new Date(1970, 0, 1).getTime();
  console.log(dtRead, tzDiff);
  var dt = Math.floor((dtRead.valueOf() - tzDiff) / 1000); // time since Epoch to current now in seconds

  //console.log(dt.valueOf()); // Time converted to your timezone
  // $.post("dev-app-set-dt", {'dt' : dt}, function() {
  //   console.log("app-datetime set succesfully: "+ dt.toString());
  // })

  console.log(dtRead.valueOf()); // Time already in UTC
  $.post("dev-app-set-dt", {'dt' : dtRead.valueOf() / 1000}, function() {
    console.log("app-datetime set succesfully: "+ dtRead.toString());
  })
  .done(function(data) {
    showAlert('success',"app-datetime set succesfully! "+ JSON.stringify(data));
  })
  .fail(function(data) {
    showAlert('error',"app-datetime has not been set! "+ JSON.stringify(data));
  });
});

// App restart button
$('#app-reset-button').click(function() {
  console.log('#app-reset-button CLICKED')

  $.get("dev-app-reset", function(data) {
    console.log(data);
  })
  .done(function(data) {
    console.log("App successfully restarted "+data);
  })
  .fail(function(data) {
    console.log("error: "+data);
    showAlert('error', "App cannot be restarted " + data);
  });
});

// App print some text button
$('#app-print-text-button').click(function() {
  console.log('#app-print-text-button CLICKED')

  const text = {'txt' : $('#app-print-text').val() }
  console.log(text)
  $.ajax({
    url:"dev-app-print-text",
    type:"POST",
    data: JSON.stringify(text),
    contentType:"application/json",
    dataType:"json",
    success: function(data) {
      console.log("app-print-text-button - success" + JSON.stringify(data));
      showAlert('success', JSON.stringify(data));
    },
    error: function(data){
        console.log("app-print-text-button - failure" + JSON.stringify(data));
        showAlert('error', JSON.stringify(data));
    }
  });
});

// App get system time
function updateAppTime() {
  var dt = new Date();
  $.get("dev-app-get-dt", function(data) {
    console.log("app-datetime get succesfully: "+ data);
    epoch = data['dt'] * 1000;
    dt.setTime(epoch);
    console.log(dt, epoch);
  })
  .done(function(data) {

  })
  .fail(function(data) {
    showAlert('error',"app-datetime has not been read! "+ data);
  });

  // Update UI

  $('#app-date').val(dt.getFullYear() + '-' + ('0' + (dt.getMonth()+1)).slice(-2) + '-' + ('0' + dt.getDate()).slice(-2));
  $('#app-time').val(('0'+dt.getHours()).slice(-2) +':'+ ('0'+dt.getMinutes()).slice(-2) +':'+ ('0'+dt.getSeconds()).slice(-2));
  $('#app-date-time').val(dt.toString());
}

/* OTA TAB */
$('form').submit(function(e){
    e.preventDefault();
    var form = $('#upload_form')[0];
    var data = new FormData(form);
    $.ajax({
        url: '/update',
        type: 'POST',
        data: data,
        contentType: false,
        processData:false,
        xhr: function() {
            var xhr = new window.XMLHttpRequest();
            xhr.upload.addEventListener('progress', function(evt) {
                $('#upload_status').html('Uploading...');
                if (evt.lengthComputable) {
                    var per = evt.loaded / evt.total;
                    $('#upload_prg').html('progress: ' + Math.round(per*100) + '%');
                }
            }, false);
        return xhr;
        },
        success:function(d, s) {
            console.log('OTA success!')
            $('#upload_status').html('OTA success! resetting the device...');
        },
        error: function (a, b, c) {
            console.log('OTA error!')
            $('#upload_status').html('OTA Error, try again!');
        }
    });
 });
