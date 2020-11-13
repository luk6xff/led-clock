
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
// $(document).ready(function() {
//   $('selector').click(function() {
//       $('selector.active').removeClass("active");
//       $(this).addClass("active");
//   });
// })

// $(function(){
//   $('#dev-cfg-wifi').addClass('active');
// })

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
    dataJson = [];
    // Input form
    $(target+" :input[class*='form-control']").each(function() {

        var id = $(this).attr("id");
        var value = $(this).val();

        item = {}
        item ["id"] = id;
        item ["value"] = value;

        dataJson.push(item);
    });

    // Checkbox
    $(target+" :input[class*='switcher-input']").each(function() {

      var id = $(this).attr("id");
      var value = $(this).is(':checked');

      item = {}
      item ["id"] = id;
      item ["value"] = value;

      dataJson.push(item);
    });

    // Select
    $(target+" :input[class*='custom-select']").each(function() {

      var id = $(this).attr("id");
      var value = $(this).val();

      item = {}
      item ["id"] = id;
      item ["value"] = value;

      dataJson.push(item);
    });

    console.log(dataJson);

    // Send post request
    const cfgName = target.replace("#", "");
    $.post(cfgName, JSON.stringify(dataJson)).done(function(data) {
        console.log("save_button " + JSON.stringify(data));
      }).fail(function(err) {
        console.log("err save_button " + JSON.stringify(err));
      });

});

$('#refresh_button').click(function() {
    console.log('#refresh_button CLICKED')
});


// Time cfg utils
function changeTimeTimezoneView() {
  var elem = $('#time-timezone-num').find('option:selected');
  if (elem.val() == '2') {
    $('#div-time-timezone-2').show();
  } else {
    $('#div-time-timezone-2').hide();
  }
  console.log(elem.val());
}

$(changeTimeTimezoneView()); // Called on document.ready

$('#time-timezone-num').change(function() {
  changeTimeTimezoneView();
});


// Weather cfg utils
function changeCfgWeatherView() {
  if ($('#weather-update-enable').is(':checked')) {
    $(".cfg-weather-settings").show();
  } else {
    $(".cfg-weather-settings").hide();
  }
}

$(changeCfgWeatherView()); // Called on document.ready

$('#weather-update-enable').change(function() {
  changeCfgWeatherView();
});


// Display cfg utils
function changeDisplayIntensityView() {
  if ($('#display-auto-intensity').is(':checked')) {
    $("#display-intensity-val").prop("disabled", true);
  } else {
    $("#display-intensity-val").prop("disabled", false);
  }
}

$(changeDisplayIntensityView()); // Called on document.ready

$('#display-auto-intensity').change(function() {
  changeDisplayIntensityView();
});



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
