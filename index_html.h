String INDEX_HTML = R"(<!DOCTYPE html>
<html>
<head>
  <meta http-equiv='Cache-Control' content='no-cache, no-store, must-revalidate' />
  <meta http-equiv='Pragma' content='no-cache' />
  <meta http-equiv='Expires' content='0' />
  <title>{NAME}</title>
  <link rel='stylesheet' href='https://unpkg.com/onsenui/css/onsenui.css'>
  <link rel='stylesheet' href='https://unpkg.com/onsenui/css/onsen-css-components.min.css'>
  <script src='https://unpkg.com/onsenui/js/onsenui.min.js'></script>
  <script src='https://unpkg.com/jquery/dist/jquery.min.js'></script>
  <script>
  var cversion = '{VERSION}';
  var controlDualBlinds = '{controlDualBlinds}';
  var useBME280Sensor = '{useBME280Sensor}';
  var wsUri = 'ws://'+location.host+':81/';
   var repo = 'https://github.com/drakecoldwinter/MotorOnARoller';

if(useBME280Sensor==true){
  setInterval(function() {
    // Call a function repetatively with 1 Second interval
    getTempData();
    getHumData();
    }, 1000); //1000mSeconds update rate
}

if(controlDualBlinds==true){
  setInterval(function(){
    $('.coverB').show();
    $('.coverA').show();
  },1000);
}

function getTempData() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("temperature").innerHTML = this.responseText + ' C';
      if(this.responseText != ""){
        $('#bme280').show();
      }     
    }
  };
  xhttp.open("GET", "readTemp", true);
  xhttp.send();
}

function getHumData() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("humidity").innerHTML = this.responseText + ' %';
      if(this.responseText != ""){
        $('#bme280').show();
      }     
    }
  };
  xhttp.open("GET", "readHum", true);
  xhttp.send();
}

  window.fn = {};
  window.fn.open = function() {
    var menu = document.getElementById('menu');
    menu.open();
  };

  window.fn.load = function(page) {
    var content = document.getElementById('content');
    var menu = document.getElementById('menu');
    content.load(page)
      .then(menu.close.bind(menu)).then(setActions());
  };

  var gotoPos1 = function(percent){
    doSend(percent, 1);
  };
  var gotoPos2 = function(percent){
    doSend(percent, 2);
  };
  var instr = function(action, motor){
    doSend('('+action+')', motor);
  };

  var setActions = function(){
    //doSend('(update)', 1);
    //doSend('(update)', 2);
    $.get(repo+'/releases', function(data){
      if (data.length>0 && data[0].tag_name !== cversion){
        $('#cversion').text(cversion);
        $('#nversion').text(data[0].tag_name);
        $('#update-card').show();
      }
    });

    setTimeout(function(){
      $('#arrow-close1').on('click', function(){$('#setrange1').val(100);gotoPos1(100);});
      $('#arrow-open1').on('click', function(){$('#setrange1').val(0);gotoPos1(0);});
      $('#setrange1').on('change', function(){gotoPos1($('#setrange1').val())});

      $('#arrow-close2').on('click', function(){$('#setrange2').val(100);gotoPos2(100);});
      $('#arrow-open2').on('click', function(){$('#setrange2').val(0);gotoPos2(0);});
      $('#setrange2').on('change', function(){gotoPos2($('#setrange2').val())});

      $('#arrow-up-man1').on('click', function(){instr('-1',1)});
      $('#arrow-down-man1').on('click', function(){instr('1',1)});
      $('#arrow-stop-man1').on('click', function(){instr('0',1)});
      $('#set-start1').on('click', function(){instr('start',1)});
      $('#set-max1').on('click', function(){instr('max',1);});

      $('#arrow-up-man2').on('click', function(){instr('-1',2)});
      $('#arrow-down-man2').on('click', function(){instr('1',2)});
      $('#arrow-stop-man2').on('click', function(){instr('0',2)});
      $('#set-start2').on('click', function(){instr('start',2)});
      $('#set-max2').on('click', function(){instr('max',2);});

    }, 200);
  };
  $(document).ready(function(){
    setActions();
  });

  var websocket;
  var timeOut;
  function retry(){
    clearTimeout(timeOut);
    timeOut = setTimeout(function(){
      websocket=null; init();},5000);
  };
  function init(){
    ons.notification.toast({message: 'Connecting...', timeout: 1000});
    try{
      websocket = new WebSocket(wsUri);
      websocket.onclose = function () {};
      websocket.onerror = function(evt) {
        ons.notification.toast({message: 'Cannot connect to device', timeout: 2000});
        retry();
      };
      websocket.onopen = function(evt) {
        ons.notification.toast({message: 'Connected to device', timeout: 2000});
        setTimeout(function(){doSend('(update)',1);}, 1000);
        setTimeout(function(){doSend('(update)',2);}, 1000);
      };
      websocket.onclose = function(evt) {
        ons.notification.toast({message: 'Disconnected. Retrying', timeout: 2000});
        retry();
      };
      websocket.onmessage = function(evt) {
        try{
          var msg = JSON.parse(evt.data);
          if (typeof msg.position !== 'undefined'){
              if (msg.motor == "1"){
                $('#pbar1').attr('value', msg.position);
              }else{
                $('#pbar2').attr('value', msg.position);
              }
          };
          if (typeof msg.set !== 'undefined'){
            if (msg.motor == "1"){
                $('#setrange1').val(msg.set);
            }else{
                $('#setrange2').val(msg.set);
            }        
          };
        } catch(err){}
      };
    } catch (e){
      ons.notification.toast({message: 'Cannot connect to device. Retrying...', timeout: 2000});
      retry();
    };
  };
  function doSend(msg, motor){
    if (websocket && websocket.readyState == 1){
      websocket.send(motor + '|' + msg);
    }
  };

  function wipeSettings() {
    if (confirm("Are you really sure to wipe ALL settings?")) {
      $.ajax({
        type: "POST",
        url: "/reset",
        contentType : 'application/json',
      })
    }
  }
  
  window.addEventListener('load', init, false);
  window.onbeforeunload = function() {
    if (websocket && websocket.readyState == 1){
      websocket.close();
    };
  };
  </script>
</head>
<body>

<ons-splitter>
  <ons-splitter-side id='menu' side='left' width='220px' collapse swipeable>
    <ons-page>
      <ons-list>
        <ons-list-item onclick='fn.load("home.html")' tappable>
          Home
        </ons-list-item>
        <ons-list-item onclick='fn.load("settings.html")' tappable>
          Settings
        </ons-list-item>
        <ons-list-item onclick='fn.load("about.html")' tappable>
          About
        </ons-list-item>
        <ons-list-item onclick='wipeSettings()' tappable>
          Wipe settings
        </ons-list-item>
      </ons-list>
    </ons-page>
  </ons-splitter-side>
  <ons-splitter-content id='content' page='home.html'></ons-splitter-content>
</ons-splitter>

<template id='home.html'>
  <ons-page>
    <ons-toolbar>
      <div class='left'>
        <ons-toolbar-button onclick='fn.open()'>
          <ons-icon icon='md-menu'></ons-icon>
        </ons-toolbar-button>
      </div>
      <div class='center'>
        {NAME}
      </div>
      <div class='right' id='bme280' style='display:none'>
        <ons-icon icon='fa-thermometer-half' >&nbsp;</ons-icon><span id='temperature'>0 C</span>&nbsp;&nbsp;
        <ons-icon icon='fa-tint' ></ons-icon>&nbsp;<span id='humidity'>0 %</span>&nbsp;&nbsp;
      </div>
    </ons-toolbar>
    
<ons-card>
    <div class='title'>Adjust position <span class='coverA' style='display:none'>(Cover A)</span></div>
  <div class='content'><p>Move the slider to the wanted position or use the arrows to open/close to the max positions</p></div>
  <ons-row>
      <ons-col width='40px' style='text-align: center; line-height: 31px;'>
      </ons-col>
      <ons-col>
         <ons-progress-bar id='pbar1' value='75'></ons-progress-bar>
      </ons-col>
      <ons-col width='40px' style='text-align: center; line-height: 31px;'>
      </ons-col>
  </ons-row>
    <ons-row>
      <ons-col width='40px' style='text-align: center; line-height: 31px;'>
        <ons-icon id='arrow-open1' icon='fa-arrow-up' size='2x'></ons-icon>
      </ons-col>
      <ons-col>
        <ons-range id='setrange1' style='width: 100%;' value='25'></ons-range>
      </ons-col>
      <ons-col width='40px' style='text-align: center; line-height: 31px;'>
        <ons-icon id='arrow-close1' icon='fa-arrow-down' size='2x'></ons-icon>
      </ons-col>
    </ons-row>
</ons-card>

<ons-card class='coverB'style='display:none'> 
  <div class='title'>Adjust position (Cover B)</div>
  <div class='content'><p>Move the slider to the wanted position or use the arrows to open/close to the max positions</p></div>
  <ons-row>
      <ons-col width='40px' style='text-align: center; line-height: 31px;'>
      </ons-col>
      <ons-col>
         <ons-progress-bar id='pbar2' value='75'></ons-progress-bar>
      </ons-col>
      <ons-col width='40px' style='text-align: center; line-height: 31px;'>
      </ons-col>
  </ons-row>
    <ons-row>
      <ons-col width='40px' style='text-align: center; line-height: 31px;'>
        <ons-icon id='arrow-open2' icon='fa-arrow-up' size='2x'></ons-icon>
      </ons-col>
      <ons-col>
        <ons-range id='setrange2' style='width: 100%;' value='25'></ons-range>
      </ons-col>
      <ons-col width='40px' style='text-align: center; line-height: 31px;'>
        <ons-icon id='arrow-close2' icon='fa-arrow-down' size='2x'></ons-icon>
      </ons-col>
    </ons-row>
</ons-card>

    <ons-card id='update-card' style='display:none'>
      <div class='title'>Update available</div>
      <div class='content'>You are running <span id='cversion'></span> and <span id='nversion'></span> is the latest. Go to <a href='https://github.com/drakecoldwinter/MotorOnARoller'>the repo</a> to download</div>
    </ons-card>   
  </ons-page>
</template>

<template id='settings.html'>
  <ons-page>
    <ons-toolbar>
      <div class='left'>
        <ons-toolbar-button onclick='fn.open()'>
          <ons-icon icon='md-menu'></ons-icon>
        </ons-toolbar-button>
      </div>
      <div class='center'>
        Settings
      </div>
    </ons-toolbar>
  <ons-card>
    <div class='title'>Instructions</div>
    <div class='content'>
    <p>
    <ol>
      <li>Use the arrows and stop button to navigate to the top position i.e. the blind is opened</li>
      <li>Click the START button</li>
      <li>Use the down arrow to navigate to the max closed position</li>
      <li>Click the MAX button</li>
      <li>Calibration is completed!</li>
    </ol>
    </p>
  </div>
  </ons-card>
  <ons-card>
    <div class='title'>Control <span class='coverA' style='display:none'>(Cover A)</span></div>
    <ons-row style='width:100%'>
      <ons-col style='text-align:center'><ons-icon id='arrow-up-man1' icon='fa-arrow-up' size='2x'></ons-icon></ons-col>
      <ons-col style='text-align:center'><ons-icon id='arrow-stop-man1' icon='fa-stop' size='2x'></ons-icon></ons-col>
      <ons-col style='text-align:center'><ons-icon id='arrow-down-man1' icon='fa-arrow-down' size='2x'></ons-icon></ons-col>
    </ons-row>
    <ons-row style='width:100%'>
      <ons-col><br /></ons-col>
    </ons-row>
    <ons-row style='width:100%'>
      <ons-col style='text-align:center'><ons-button id='set-start1'>Set Start</ons-button></ons-col>
      <ons-col style='text-align:center'>&nbsp;</ons-col>
      <ons-col style='text-align:center'><ons-button id='set-max1'>Set Max</ons-button></ons-col>
    </ons-row>
  </ons-card>

  <ons-card class='coverB' style='display:none'>
    <div class='title'>Control (Cover B)</div>
    <ons-row style='width:100%'>
      <ons-col style='text-align:center'><ons-icon id='arrow-up-man2' icon='fa-arrow-up' size='2x'></ons-icon></ons-col>
      <ons-col style='text-align:center'><ons-icon id='arrow-stop-man2' icon='fa-stop' size='2x'></ons-icon></ons-col>
      <ons-col style='text-align:center'><ons-icon id='arrow-down-man2' icon='fa-arrow-down' size='2x'></ons-icon></ons-col>
      <ons-row style='width:100%'>
        <ons-col><br /></ons-col>
      </ons-row>
    <ons-row style='width:100%'>
      <ons-col style='text-align:center'><ons-button id='set-start2'>Set Start</ons-button></ons-col>
      <ons-col style='text-align:center'>&nbsp;</ons-col>
      <ons-col style='text-align:center'><ons-button id='set-max2'>Set Max</ons-button></ons-col>
    </ons-row>
  </ons-card>
  </ons-page>
</template>

<template id='about.html'>
  <ons-page>
    <ons-toolbar>
      <div class='left'>
        <ons-toolbar-button onclick='fn.open()'>
          <ons-icon icon='md-menu'></ons-icon>
        </ons-toolbar-button>
      </div>
      <div class='center'>
        About
      </div>
    </ons-toolbar>
  <ons-card>
    <div class='title'>Motor on a roller blind</div>
    <div class='content'>
    <p> The original project was created by <a href='https://github.com/nidayand/motor-on-roller-blind-ws' target='_blank'>nidayand</a> but I found the
      <a href='https://github.com/thehookup/motor-on-roller-blind-ws'>thehookup</a> version first, so that my code is inspired by thehookup version.
      I reprogrammed the whole project as a means to teach myself arduino code.
      <ul>
        <li>3d print files and instructions: <a href='https://www.thingiverse.com/thing:2392856' target='_blank'>https://www.thingiverse.com/thing:2392856</a></li>
        <li>My Github repository: <a href='https://github.com/drakecoldwinter/MotorOnARoller' target='_blank'>https://github.com/nidayand/motor-on-roller-blind-ws</a></li>
        <li>Licensed under <a href='https://raw.githubusercontent.com/nidayand/motor-on-roller-blind-ws/master/LICENSE' target='_blank'>MIT License</a></li>
      </ul>
    </p>
  </div>
  </ons-card>
  </ons-page>
</template>

</body>
</html>
)";
