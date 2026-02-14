/*
 * =============================================================
 *  TRINETRA - ESP32-CAM Surveillance System
 *  Web Interface (camera_index.h)
 * =============================================================
 *  Mobile-first, dark-themed, responsive HTML/CSS/JS UI.
 *  Stored as PROGMEM raw string literal.
 * =============================================================
 */

#ifndef CAMERA_INDEX_H
#define CAMERA_INDEX_H

#include <pgmspace.h>

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1,maximum-scale=1,user-scalable=no">
<title>Trinetra - Live Surveillance</title>
<style>
*{margin:0;padding:0;box-sizing:border-box}
:root{
  --bg:#0d0d0d;--surface:#1a1a1a;--surface2:#242424;
  --accent:#ff5722;--accent2:#ff7043;--accent-glow:rgba(255,87,34,0.3);
  --text:#e0e0e0;--text2:#9e9e9e;--success:#4caf50;--danger:#f44336;
  --border:#333;--radius:10px;
}
html,body{height:100%;font-family:'Segoe UI',system-ui,-apple-system,sans-serif;background:var(--bg);color:var(--text);overflow-x:hidden}
.header{
  background:linear-gradient(135deg,#1a1a1a 0%,#0d0d0d 100%);
  border-bottom:1px solid var(--border);
  padding:12px 16px;display:flex;align-items:center;justify-content:space-between;
  position:sticky;top:0;z-index:100;
}
.logo{display:flex;align-items:center;gap:10px}
.logo-icon{
  width:36px;height:36px;border-radius:50%;
  background:linear-gradient(135deg,var(--accent),#d84315);
  display:flex;align-items:center;justify-content:center;
  font-size:18px;font-weight:700;color:#fff;
  box-shadow:0 0 12px var(--accent-glow);
}
.logo h1{font-size:1.3rem;font-weight:700;letter-spacing:2px;
  background:linear-gradient(90deg,var(--accent),var(--accent2));
  -webkit-background-clip:text;-webkit-text-fill-color:transparent;
}
.status-dot{
  width:10px;height:10px;border-radius:50%;
  background:var(--danger);transition:background .3s;
  box-shadow:0 0 6px var(--danger);
}
.status-dot.live{background:var(--success);box-shadow:0 0 6px var(--success)}
.container{max-width:800px;margin:0 auto;padding:12px}
.stream-wrap{
  position:relative;border-radius:var(--radius);overflow:hidden;
  background:#000;border:1px solid var(--border);margin-bottom:14px;
  aspect-ratio:4/3;
}
.stream-wrap img{width:100%;height:100%;object-fit:contain;display:block}
.stream-overlay{
  position:absolute;top:0;left:0;right:0;
  padding:8px 12px;display:flex;justify-content:space-between;align-items:center;
  background:linear-gradient(180deg,rgba(0,0,0,0.7) 0%,transparent 100%);
  pointer-events:none;
}
.badge{
  font-size:0.65rem;font-weight:600;padding:3px 8px;border-radius:20px;
  text-transform:uppercase;letter-spacing:1px;pointer-events:auto;
}
.badge-live{background:var(--danger);color:#fff;animation:pulse 2s infinite}
.badge-res{background:rgba(255,255,255,0.15);color:#ccc;backdrop-filter:blur(4px)}
@keyframes pulse{0%,100%{opacity:1}50%{opacity:.6}}
.stream-placeholder{
  display:flex;align-items:center;justify-content:center;
  width:100%;height:100%;color:var(--text2);font-size:.9rem;
  flex-direction:column;gap:8px;
}
.stream-placeholder svg{width:48px;height:48px;opacity:.3}
.card{
  background:var(--surface);border:1px solid var(--border);
  border-radius:var(--radius);padding:14px;margin-bottom:12px;
}
.card-title{
  font-size:.75rem;font-weight:600;text-transform:uppercase;
  letter-spacing:1.5px;color:var(--accent);margin-bottom:10px;
  display:flex;align-items:center;gap:6px;
}
.btn-row{display:flex;gap:8px;flex-wrap:wrap}
.btn{
  flex:1;min-width:100px;padding:12px 10px;border:none;border-radius:8px;
  font-size:.8rem;font-weight:600;cursor:pointer;transition:all .2s;
  display:flex;align-items:center;justify-content:center;gap:6px;
  text-transform:uppercase;letter-spacing:.5px;
}
.btn:active{transform:scale(.96)}
.btn-primary{background:var(--accent);color:#fff}
.btn-primary:hover{background:var(--accent2);box-shadow:0 0 16px var(--accent-glow)}
.btn-secondary{background:var(--surface2);color:var(--text);border:1px solid var(--border)}
.btn-secondary:hover{border-color:var(--accent);color:var(--accent)}
.btn-success{background:#2e7d32;color:#fff}
.btn-success:hover{background:#388e3c}
.btn-danger{background:#c62828;color:#fff}
.btn-danger:hover{background:#d32f2f}
.btn-sm{padding:8px 10px;font-size:.7rem;min-width:70px;flex:0}
.control-group{display:flex;align-items:center;gap:10px;margin-bottom:8px}
.control-group:last-child{margin-bottom:0}
.control-label{font-size:.75rem;color:var(--text2);min-width:80px;flex-shrink:0}
.select-wrap{flex:1}
select{
  width:100%;padding:10px 12px;border-radius:8px;
  background:var(--surface2);color:var(--text);border:1px solid var(--border);
  font-size:.8rem;appearance:none;cursor:pointer;
  background-image:url("data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' width='12' height='12' fill='%239e9e9e'%3E%3Cpath d='M6 8L1 3h10z'/%3E%3C/svg%3E");
  background-repeat:no-repeat;background-position:right 12px center;
}
select:focus{outline:none;border-color:var(--accent)}
input[type=range]{
  flex:1;-webkit-appearance:none;height:6px;border-radius:3px;
  background:var(--surface2);outline:none;
}
input[type=range]::-webkit-slider-thumb{
  -webkit-appearance:none;width:18px;height:18px;border-radius:50%;
  background:var(--accent);cursor:pointer;box-shadow:0 0 6px var(--accent-glow);
}
.range-val{
  min-width:32px;text-align:center;font-size:.75rem;
  color:var(--accent);font-weight:600;
}
.led-status{display:flex;align-items:center;gap:8px;margin-top:6px}
.led-icon{
  width:14px;height:14px;border-radius:50%;
  background:#555;border:2px solid #444;transition:all .3s;
}
.led-icon.on{background:#ffc107;border-color:#ffca28;box-shadow:0 0 10px rgba(255,193,7,0.6)}
.led-label{font-size:.75rem;color:var(--text2)}
.toast{
  position:fixed;bottom:20px;left:50%;transform:translateX(-50%) translateY(100px);
  padding:12px 24px;border-radius:8px;font-size:.8rem;font-weight:500;
  z-index:999;transition:transform .3s ease;pointer-events:none;
  max-width:90%;text-align:center;
}
.toast.show{transform:translateX(-50%) translateY(0)}
.toast-success{background:var(--success);color:#fff}
.toast-error{background:var(--danger);color:#fff}
.toast-info{background:#1565c0;color:#fff}
.footer{
  text-align:center;padding:16px;font-size:.65rem;
  color:var(--text2);border-top:1px solid var(--border);
  margin-top:8px;
}
.footer a{color:var(--accent);text-decoration:none}
@media(max-width:480px){
  .header{padding:10px 12px}
  .logo h1{font-size:1.1rem}
  .container{padding:8px}
  .card{padding:12px}
  .btn{padding:10px 8px;font-size:.72rem;min-width:80px}
  .stream-wrap{border-radius:8px}
}
@media(min-width:600px){
  .btn-row-main{display:grid;grid-template-columns:1fr 1fr 1fr 1fr;gap:8px}
}
/* Modal Styles */
.modal{position:fixed;top:0;left:0;width:100%;height:100%;background:rgba(0,0,0,0.85);z-index:200;display:none;align-items:center;justify-content:center;padding:16px}
.modal.show{display:flex}
.modal-content{background:var(--surface);border:1px solid var(--border);border-radius:var(--radius);max-width:400px;width:100%;max-height:80vh;overflow:hidden;display:flex;flex-direction:column}
.modal-header{padding:14px 16px;border-bottom:1px solid var(--border);display:flex;justify-content:space-between;align-items:center}
.modal-header h2{font-size:1rem;margin:0}
.modal-close{background:none;border:none;color:var(--text2);font-size:1.5rem;cursor:pointer;padding:0;line-height:1}
.modal-close:hover{color:var(--accent)}
.modal-body{padding:14px 16px;overflow-y:auto;flex:1}
.network-list{max-height:240px;overflow-y:auto}
.network-item{display:flex;align-items:center;justify-content:space-between;padding:10px 12px;background:var(--surface2);border-radius:8px;margin-bottom:6px;cursor:pointer;transition:all .2s}
.network-item:hover{border-color:var(--accent);background:#2a2a2a}
.network-name{font-size:.85rem;font-weight:500}
.network-info{display:flex;align-items:center;gap:8px;font-size:.7rem;color:var(--text2)}
.network-open{color:var(--success)}
.network-secured{color:var(--accent)}
.wifi-form{margin-top:12px;display:none}
.wifi-form.show{display:block}
.wifi-input{width:100%;padding:10px 12px;border-radius:8px;background:var(--surface2);color:var(--text);border:1px solid var(--border);font-size:.85rem;margin-bottom:10px}
.wifi-input:focus{outline:none;border-color:var(--accent)}
.wifi-status{padding:10px;background:var(--surface2);border-radius:8px;margin-bottom:12px;font-size:.8rem}
.wifi-connected{color:var(--success)}
.wifi-disconnected{color:var(--text2)}
</style>
</head>
<body>

<!-- Header -->
<div class="header">
  <div class="logo">
    <div class="logo-icon">T</div>
    <h1>TRINETRA</h1>
  </div>
  <div style="display:flex;align-items:center;gap:8px">
    <span style="font-size:.65rem;color:var(--text2)" id="connLabel">Offline</span>
    <div class="status-dot" id="statusDot"></div>
  </div>
</div>

<!-- Main -->
<div class="container">

  <!-- Stream Viewer -->
  <div class="stream-wrap" id="streamWrap">
    <img id="streamView" style="display:none" alt="Live Stream">
    <div class="stream-placeholder" id="placeholder">
      <svg viewBox="0 0 24 24" fill="currentColor"><path d="M17 10.5V7a1 1 0 00-1-1H4a1 1 0 00-1 1v10a1 1 0 001 1h12a1 1 0 001-1v-3.5l4 4v-11l-4 4z"/></svg>
      <span>Tap START to begin streaming</span>
    </div>
    <div class="stream-overlay" id="streamOverlay" style="display:none">
      <span class="badge badge-live">&#9679; LIVE</span>
      <span class="badge badge-res" id="resLabel">QVGA</span>
    </div>
  </div>

  <!-- Stream Controls -->
  <div class="card">
    <div class="card-title">
      <svg width="14" height="14" viewBox="0 0 24 24" fill="var(--accent)"><path d="M8 5v14l11-7z"/></svg>
      Stream Control
    </div>
    <div class="btn-row btn-row-main">
      <button class="btn btn-primary" id="btnStart" onclick="startStream()">&#9654; Start</button>
      <button class="btn btn-danger" id="btnStop" onclick="stopStream()" style="display:none">&#9632; Stop</button>
      <button class="btn btn-success" id="btnCapture" onclick="capturePhoto()">&#128247; Capture</button>
      <button class="btn btn-secondary" id="btnSaveSD" onclick="saveToSD()">&#128190; Save SD</button>
    </div>
  </div>

  <!-- Camera Settings -->
  <div class="card">
    <div class="card-title">
      <svg width="14" height="14" viewBox="0 0 24 24" fill="var(--accent)"><path d="M19.14 12.94a7.07 7.07 0 000-1.88l2.03-1.58a.49.49 0 00.12-.61l-1.92-3.32a.49.49 0 00-.59-.22l-2.39.96a7.04 7.04 0 00-1.63-.94l-.36-2.54a.48.48 0 00-.48-.41h-3.84a.48.48 0 00-.48.41l-.36 2.54c-.59.24-1.13.57-1.63.94l-2.39-.96a.49.49 0 00-.59.22L2.74 8.87a.48.48 0 00.12.61l2.03 1.58a7.07 7.07 0 000 1.88l-2.03 1.58a.49.49 0 00-.12.61l1.92 3.32c.12.22.37.29.59.22l2.39-.96c.5.37 1.04.7 1.63.94l.36 2.54c.05.24.26.41.48.41h3.84c.24 0 .44-.17.48-.41l.36-2.54c.59-.24 1.13-.57 1.63-.94l2.39.96c.22.08.47 0 .59-.22l1.92-3.32a.49.49 0 00-.12-.61l-2.03-1.58zM12 15.6A3.6 3.6 0 1115.6 12 3.6 3.6 0 0112 15.6z"/></svg>
      Camera Settings
    </div>
    <div class="control-group">
      <span class="control-label">Resolution</span>
      <div class="select-wrap">
        <select id="resSelect" onchange="changeResolution()">
          <option value="3">QQVGA (160x120)</option>
          <option value="5" selected>QVGA (320x240)</option>
          <option value="8">VGA (640x480)</option>
          <option value="9">SVGA (800x600)</option>
          <option value="10">XGA (1024x768)</option>
          <option value="12">SXGA (1280x1024)</option>
        </select>
      </div>
    </div>
    <div class="control-group">
      <span class="control-label">Quality</span>
      <input type="range" id="qualitySlider" min="4" max="63" value="12" oninput="updateQualityLabel()" onchange="changeQuality()">
      <span class="range-val" id="qualityVal">12</span>
    </div>
    <div class="control-group">
      <span class="control-label">Brightness</span>
      <input type="range" id="brightnessSlider" min="-2" max="2" value="0" oninput="updateBrightnessLabel()" onchange="changeBrightness()">
      <span class="range-val" id="brightnessVal">0</span>
    </div>
    <div class="control-group">
      <span class="control-label">Contrast</span>
      <input type="range" id="contrastSlider" min="-2" max="2" value="0" oninput="updateContrastLabel()" onchange="changeContrast()">
      <span class="range-val" id="contrastVal">0</span>
    </div>
  </div>

  <!-- Flash LED -->
  <div class="card">
    <div class="card-title">
      <svg width="14" height="14" viewBox="0 0 24 24" fill="var(--accent)"><path d="M9 21c0 .55.45 1 1 1h4c.55 0 1-.45 1-1v-1H9v1zm3-19C8.14 2 5 5.14 5 9c0 2.38 1.19 4.47 3 5.74V17c0 .55.45 1 1 1h6c.55 0 1-.45 1-1v-2.26c1.81-1.27 3-3.36 3-5.74 0-3.86-3.14-7-7-7z"/></svg>
      Flash LED
    </div>
    <div class="btn-row">
      <button class="btn btn-primary btn-sm" onclick="ledControl(1)">&#128161; ON</button>
      <button class="btn btn-secondary btn-sm" onclick="ledControl(0)">OFF</button>
    </div>
    <div class="led-status">
      <div class="led-icon" id="ledIcon"></div>
      <span class="led-label" id="ledLabel">LED Off</span>
    </div>
  </div>

  <!-- System Monitor -->
  <div class="card">
    <div class="card-title">
      <svg width="14" height="14" viewBox="0 0 24 24" fill="var(--accent)"><path d="M20 3H4c-1.1 0-2 .9-2 2v11c0 1.1.9 2 2 2h3l-1 1v2h12v-2l-1-1h3c1.1 0 2-.9 2-2V5c0-1.1-.9-2-2-2zm0 13H4V5h16v11z"/></svg>
      System Monitor
    </div>
    <div style="display:grid;grid-template-columns:1fr 1fr;gap:8px;font-size:.7rem">
      <div style="padding:6px;background:var(--surface2);border-radius:6px">
        <div style="color:var(--text2);margin-bottom:2px">⏱️ Uptime</div>
        <div style="color:var(--accent);font-weight:600" id="statUptime">--</div>
      </div>
      <div style="padding:6px;background:var(--surface2);border-radius:6px">
        <div style="color:var(--text2);margin-bottom:2px">🌡️ Temp</div>
        <div style="color:var(--accent);font-weight:600" id="statTemp">--</div>
      </div>
      <div style="padding:6px;background:var(--surface2);border-radius:6px">
        <div style="color:var(--text2);margin-bottom:2px">🎥 FPS</div>
        <div style="color:var(--accent);font-weight:600" id="statFPS">--</div>
      </div>
      <div style="padding:6px;background:var(--surface2);border-radius:6px">
        <div style="color:var(--text2);margin-bottom:2px">📊 Frames</div>
        <div style="color:var(--accent);font-weight:600" id="statFrames">--</div>
      </div>
      <div style="padding:6px;background:var(--surface2);border-radius:6px">
        <div style="color:var(--text2);margin-bottom:2px">📡 Signal</div>
        <div style="color:var(--accent);font-weight:600" id="statSignal">--</div>
      </div>
      <div style="padding:6px;background:var(--surface2);border-radius:6px">
        <div style="color:var(--text2);margin-bottom:2px">👥 Clients</div>
        <div style="color:var(--accent);font-weight:600" id="statClients">--</div>
      </div>
      <div style="padding:6px;background:var(--surface2);border-radius:6px">
        <div style="color:var(--text2);margin-bottom:2px">💾 Heap</div>
        <div style="color:var(--accent);font-weight:600" id="statHeap">--</div>
      </div>
      <div style="padding:6px;background:var(--surface2);border-radius:6px">
        <div style="color:var(--text2);margin-bottom:2px">⚡ PSRAM</div>
        <div style="color:var(--accent);font-weight:600" id="statPSRAM">--</div>
      </div>
    </div>
    <div style="margin-top:8px;text-align:center">
      <button class="btn btn-secondary btn-sm" onclick="refreshStats()">🔄 Refresh Stats</button>
    </div>
  </div>

  <!-- WiFi Setup -->
  <div class="card">
    <div class="card-title">
      <svg width="14" height="14" viewBox="0 0 24 24" fill="var(--accent)"><path d="M1 9l2 2c4.97-4.97 13.03-4.97 18 0l2-2C16.93 2.93 7.08 2.93 1 9zm8 8l3 3 3-3c-1.65-1.66-4.34-1.66-6 0zm-4-4l2 2c2.76-2.76 7.24-2.76 10 0l2-2C15.14 9.14 8.87 9.14 5 13z"/></svg>
      WiFi Setup
    </div>
    <div class="wifi-status" id="wifiStatusBox">
      <span id="wifiStatusIcon">📶</span>
      <span id="wifiStatusText">Checking...</span>
    </div>
    <div class="btn-row">
      <button class="btn btn-primary btn-sm" onclick="openWiFiModal()">📡 Configure WiFi</button>
      <button class="btn btn-danger btn-sm" onclick="resetWiFi()">🗑️ Reset</button>
    </div>
  </div>

  <div class="footer">
    TRINETRA Surveillance System &bull; ESP32-CAM &bull; v1.0
  </div>
</div>

<!-- Toast -->
<div class="toast" id="toast"></div>

<!-- WiFi Setup Modal -->
<div class="modal" id="wifiModal">
  <div class="modal-content">
    <div class="modal-header">
      <h2>📡 WiFi Setup</h2>
      <button class="modal-close" onclick="closeWiFiModal()">&times;</button>
    </div>
    <div class="modal-body">
      <div style="margin-bottom:12px">
        <button class="btn btn-secondary btn-sm" onclick="scanNetworks()" id="btnScan">🔍 Scan Networks</button>
        <span style="font-size:.7rem;color:var(--text2);margin-left:8px" id="scanStatus"></span>
      </div>
      <div class="network-list" id="networkList">
        <div style="text-align:center;color:var(--text2);padding:20px;font-size:.8rem">
          Click "Scan Networks" to find available WiFi
        </div>
      </div>
      <div class="wifi-form" id="wifiForm">
        <div style="font-size:.85rem;margin-bottom:8px">
          <strong>Selected:</strong> <span id="selectedSSID"></span>
          <span id="selectedSecure" style="margin-left:8px"></span>
        </div>
        <input type="password" class="wifi-input" id="wifiPassword" placeholder="Enter WiFi password (if required)">
        <div class="btn-row">
          <button class="btn btn-primary" onclick="connectToNetwork()">🔗 Connect</button>
          <button class="btn btn-secondary" onclick="cancelConnect()">Cancel</button>
        </div>
      </div>
    </div>
  </div>
</div>

<script>
var streamOn=false;
var baseUrl=window.location.origin;
var streamUrl=window.location.protocol+'//'+window.location.hostname+':81/stream';
var resMap={'3':'QQVGA','5':'QVGA','8':'VGA','9':'SVGA','10':'XGA','12':'SXGA'};

function showToast(msg,type,dur){
  var t=document.getElementById('toast');
  t.textContent=msg;
  t.className='toast toast-'+(type||'info')+' show';
  setTimeout(function(){t.className='toast';},dur||2500);
}

function startStream(){
  var img=document.getElementById('streamView');
  var ph=document.getElementById('placeholder');
  var ov=document.getElementById('streamOverlay');
  img.src=streamUrl;
  img.style.display='block';
  ph.style.display='none';
  ov.style.display='flex';
  document.getElementById('btnStart').style.display='none';
  document.getElementById('btnStop').style.display='flex';
  streamOn=true;
  updateStatus(true);
  showToast('Stream started','success',1500);
}

function stopStream(){
  var img=document.getElementById('streamView');
  var ph=document.getElementById('placeholder');
  var ov=document.getElementById('streamOverlay');
  img.src='';
  img.style.display='none';
  ph.style.display='flex';
  ov.style.display='none';
  document.getElementById('btnStart').style.display='flex';
  document.getElementById('btnStop').style.display='none';
  streamOn=false;
  updateStatus(false);
  showToast('Stream stopped','info',1500);
}

function updateStatus(live){
  var dot=document.getElementById('statusDot');
  var lbl=document.getElementById('connLabel');
  if(live){dot.classList.add('live');lbl.textContent='Live';}
  else{dot.classList.remove('live');lbl.textContent='Idle';}
}

function capturePhoto(){
  showToast('Capturing...','info',1000);
  fetch(baseUrl+'/capture')
    .then(function(r){if(!r.ok)throw new Error('Capture failed');return r.blob();})
    .then(function(blob){
      var url=URL.createObjectURL(blob);
      var a=document.createElement('a');
      a.href=url;a.download='trinetra_'+Date.now()+'.jpg';a.click();
      URL.revokeObjectURL(url);
      showToast('Photo captured & downloaded!','success');
    })
    .catch(function(e){showToast('Capture failed: '+e.message,'error');});
}

function saveToSD(){
  showToast('Saving to SD card...','info',1500);
  fetch(baseUrl+'/save-photo')
    .then(function(r){return r.json();})
    .then(function(d){
      if(d.success) showToast('Saved: '+d.filename,'success',3000);
      else showToast('SD Error: '+d.error,'error',3000);
    })
    .catch(function(e){showToast('SD save failed: '+e.message,'error');});
}

function changeResolution(){
  var sel=document.getElementById('resSelect');
  var val=sel.value;
  fetch(baseUrl+'/control?var=framesize&val='+val)
    .then(function(r){
      if(r.ok){
        document.getElementById('resLabel').textContent=resMap[val]||val;
        showToast('Resolution: '+(resMap[val]||val),'success',1500);
        if(streamOn){stopStream();setTimeout(startStream,300);}
      }else{showToast('Resolution change failed','error');}
    })
    .catch(function(e){showToast('Error: '+e.message,'error');});
}

function updateQualityLabel(){
  document.getElementById('qualityVal').textContent=document.getElementById('qualitySlider').value;
}
function changeQuality(){
  var val=document.getElementById('qualitySlider').value;
  fetch(baseUrl+'/control?var=quality&val='+val)
    .then(function(r){
      if(r.ok) showToast('Quality set to '+val,'success',1200);
      else showToast('Quality change failed','error');
    })
    .catch(function(e){showToast('Error: '+e.message,'error');});
}

/* --- Brightness Control --- */
function updateBrightnessLabel(){
  document.getElementById('brightnessVal').textContent=document.getElementById('brightnessSlider').value;
}
function changeBrightness(){
  var val=document.getElementById('brightnessSlider').value;
  fetch(baseUrl+'/control?var=brightness&val='+val)
    .then(function(r){
      if(r.ok) showToast('Brightness: '+val,'success',1200);
      else showToast('Brightness change failed','error');
    })
    .catch(function(e){showToast('Error: '+e.message,'error');});
}

/* --- Contrast Control --- */
function updateContrastLabel(){
  document.getElementById('contrastVal').textContent=document.getElementById('contrastSlider').value;
}
function changeContrast(){
  var val=document.getElementById('contrastSlider').value;
  fetch(baseUrl+'/control?var=contrast&val='+val)
    .then(function(r){
      if(r.ok) showToast('Contrast: '+val,'success',1200);
      else showToast('Contrast change failed','error');
    })
    .catch(function(e){showToast('Error: '+e.message,'error');});
}

function ledControl(state){
  fetch(baseUrl+'/led?state='+state)
    .then(function(r){
      if(r.ok){
        var icon=document.getElementById('ledIcon');
        var lbl=document.getElementById('ledLabel');
        if(state){icon.classList.add('on');lbl.textContent='LED On';}
        else{icon.classList.remove('on');lbl.textContent='LED Off';}
        showToast('Flash LED '+(state?'ON':'OFF'),'info',1200);
      }else{showToast('LED control failed','error');}
    })
    .catch(function(e){showToast('Error: '+e.message,'error');});
}

/* --- System Stats --- */
function refreshStats(){
  fetch(baseUrl+'/system-stats')
    .then(function(r){return r.json();})
    .then(function(d){
      // Uptime
      var uptime='';
      if(d.uptime_days>0) uptime+=d.uptime_days+'d ';
      uptime+=d.uptime_hours%24+'h '+d.uptime_minutes%60+'m';
      document.getElementById('statUptime').textContent=uptime;
      
      // Temperature
      document.getElementById('statTemp').textContent=d.temp_celsius.toFixed(1)+'°C';
      
      // FPS & Frames
      document.getElementById('statFPS').textContent=d.fps+' fps';
      document.getElementById('statFrames').textContent=d.total_frames.toLocaleString();
      
      // WiFi
      var signalQuality='Excellent';
      if(d.wifi_rssi<-80) signalQuality='Poor';
      else if(d.wifi_rssi<-70) signalQuality='Fair';
      else if(d.wifi_rssi<-60) signalQuality='Good';
      document.getElementById('statSignal').textContent=d.wifi_rssi+' dBm';
      
      // Clients
      document.getElementById('statClients').textContent=d.wifi_clients+' online';
      
      // Memory
      var heapMB=(d.free_heap/1024/1024).toFixed(1);
      document.getElementById('statHeap').textContent=heapMB+'MB ('+d.heap_usage.toFixed(0)+'%)';
      
      var psramMB=(d.free_psram/1024/1024).toFixed(1);
      document.getElementById('statPSRAM').textContent=psramMB+'MB ('+d.psram_usage.toFixed(0)+'%)';
    })
    .catch(function(e){
      console.error('Stats fetch error:',e);
    });
}

window.addEventListener('load',function(){
  updateStatus(false);
  fetch(baseUrl+'/status')
    .then(function(r){return r.json();})
    .then(function(d){
      if(d.framesize!==undefined){
        document.getElementById('resSelect').value=d.framesize;
        document.getElementById('resLabel').textContent=resMap[String(d.framesize)]||'';
      }
      if(d.quality!==undefined){
        document.getElementById('qualitySlider').value=d.quality;
        document.getElementById('qualityVal').textContent=d.quality;
      }
      if(d.brightness!==undefined){
        document.getElementById('brightnessSlider').value=d.brightness;
        document.getElementById('brightnessVal').textContent=d.brightness;
      }
      if(d.contrast!==undefined){
        document.getElementById('contrastSlider').value=d.contrast;
        document.getElementById('contrastVal').textContent=d.contrast;
      }
      document.getElementById('connLabel').textContent='Idle';
    })
    .catch(function(){document.getElementById('connLabel').textContent='Offline';});
  
  // Load system stats initially and refresh every 3 seconds
  refreshStats();
  setInterval(refreshStats,3000);
  
  // Load WiFi status
  refreshWiFiStatus();
});

/* --- WiFi Manager --- */
var selectedNetwork=null;

function refreshWiFiStatus(){
  fetch(baseUrl+'/wifi-status')
    .then(function(r){return r.json();})
    .then(function(d){
      var statusBox=document.getElementById('wifiStatusBox');
      var statusIcon=document.getElementById('wifiStatusIcon');
      var statusText=document.getElementById('wifiStatusText');
      if(d.connected){
        statusIcon.textContent='✅';
        statusText.innerHTML='<span class="wifi-connected">Connected to <strong>'+d.ssid+'</strong> ('+d.ip+')</span>';
      }else if(d.savedSSID){
        statusIcon.textContent='🔄';
        statusText.innerHTML='<span class="wifi-disconnected">Saved: '+d.savedSSID+' (not connected)</span>';
      }else{
        statusIcon.textContent='📵';
        statusText.innerHTML='<span class="wifi-disconnected">No WiFi configured - AP mode only</span>';
      }
    })
    .catch(function(e){
      document.getElementById('wifiStatusText').textContent='Error checking status';
    });
}

function openWiFiModal(){
  document.getElementById('wifiModal').classList.add('show');
  document.getElementById('wifiForm').classList.remove('show');
  selectedNetwork=null;
}

function closeWiFiModal(){
  document.getElementById('wifiModal').classList.remove('show');
}

function scanNetworks(){
  var btn=document.getElementById('btnScan');
  var status=document.getElementById('scanStatus');
  var list=document.getElementById('networkList');
  btn.disabled=true;
  status.textContent='Scanning...';
  list.innerHTML='<div style="text-align:center;padding:20px">🔍 Scanning for networks...</div>';
  
  fetch(baseUrl+'/wifi-scan')
    .then(function(r){return r.json();})
    .then(function(networks){
      btn.disabled=false;
      status.textContent='Found '+networks.length+' networks';
      
      if(networks.length===0){
        list.innerHTML='<div style="text-align:center;padding:20px;color:var(--text2)">No networks found. Try again.</div>';
        return;
      }
      
      // Sort by signal strength
      networks.sort(function(a,b){return b.rssi-a.rssi;});
      
      var html='';
      for(var i=0;i<networks.length;i++){
        var n=networks[i];
        var signal='📶';
        if(n.rssi<-80) signal='📵';
        else if(n.rssi<-70) signal='📶';
        else if(n.rssi<-60) signal='📶';
        
        var security=n.secure?'<span class="network-secured">🔒 Secured</span>':'<span class="network-open">🔓 Open</span>';
        
        html+='<div class="network-item" onclick="selectNetwork(\''+n.ssid.replace(/'/g,"\\'")+'\','+n.secure+')">';
        html+='<span class="network-name">'+n.ssid+'</span>';
        html+='<span class="network-info">'+signal+' '+n.rssi+' dBm '+security+'</span>';
        html+='</div>';
      }
      list.innerHTML=html;
    })
    .catch(function(e){
      btn.disabled=false;
      status.textContent='Scan failed';
      list.innerHTML='<div style="text-align:center;padding:20px;color:var(--danger)">Scan failed: '+e.message+'</div>';
    });
}

function selectNetwork(ssid,secure){
  selectedNetwork={ssid:ssid,secure:secure};
  document.getElementById('selectedSSID').textContent=ssid;
  document.getElementById('selectedSecure').innerHTML=secure?'<span class="network-secured">🔒</span>':'<span class="network-open">🔓 Open</span>';
  document.getElementById('wifiPassword').value='';
  document.getElementById('wifiPassword').style.display=secure?'block':'none';
  document.getElementById('wifiForm').classList.add('show');
}

function cancelConnect(){
  document.getElementById('wifiForm').classList.remove('show');
  selectedNetwork=null;
}

function connectToNetwork(){
  if(!selectedNetwork) return;
  var password=document.getElementById('wifiPassword').value;
  var ssid=selectedNetwork.ssid;
  
  showToast('Connecting to '+ssid+'...','info',5000);
  closeWiFiModal();
  
  var url=baseUrl+'/wifi-connect?ssid='+encodeURIComponent(ssid)+'&password='+encodeURIComponent(password);
  fetch(url)
    .then(function(r){return r.json();})
    .then(function(d){
      if(d.success){
        showToast('Connected to '+d.ssid+'! IP: '+d.ip,'success',5000);
        refreshWiFiStatus();
      }else{
        showToast('Connection failed: '+d.message,'error',4000);
      }
    })
    .catch(function(e){
      showToast('Connection error: '+e.message,'error');
    });
}

function resetWiFi(){
  if(!confirm('Clear saved WiFi credentials? Device will use AP mode only.')) return;
  
  fetch(baseUrl+'/wifi-reset')
    .then(function(r){return r.json();})
    .then(function(d){
      showToast(d.message,'info',4000);
      refreshWiFiStatus();
    })
    .catch(function(e){
      showToast('Reset failed: '+e.message,'error');
    });
}
</script>
</body>
</html>
)rawliteral";

#endif  // CAMERA_INDEX_H
