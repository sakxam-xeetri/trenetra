/*
 * =============================================================
 *  TRINETRA — ESP32-CAM Surveillance System
 *  Drone Flight Report Page (flight_report.h)
 * =============================================================
 *  Standalone flight report page served at /flight-report
 *  Shows Kathmandu map with drone flight path, telemetry data,
 *  and Haversine distance calculation.
 *
 *  Dependencies: Leaflet.js (CDN), OpenStreetMap tiles
 *  Theme: Matches TRINETRA glassmorphic dark UI
 * =============================================================
 */
#ifndef FLIGHT_REPORT_H
#define FLIGHT_REPORT_H
#include <pgmspace.h>

const char flight_report_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1,maximum-scale=5">
<meta name="theme-color" content="#060a11">
<title>Trinetra — Flight Report</title>

<!-- Leaflet CSS -->
<link rel="stylesheet" href="https://unpkg.com/leaflet@1.9.4/dist/leaflet.css"
  integrity="sha256-p4NxAoJBhIIN+hmNHrzRCf9tD/miZyoHS5obTRR9BMY="
  crossorigin=""/>

<!-- Leaflet JS -->
<script src="https://unpkg.com/leaflet@1.9.4/dist/leaflet.js"
  integrity="sha256-20nQCchB9co0qIjJZRGuk2/Z9VM+kNiyxNV1lvTlZBo="
  crossorigin=""></script>

<style>
/* ===== flight_report.css — TRINETRA theme ===== */
*,*::before,*::after{margin:0;padding:0;box-sizing:border-box}
:root{
  --bg:#060a11;--sf:#0f1520;--sf2:#1a2233;--sf3:#2a3548;
  --ac:#dc2626;--ac2:#f87171;--acg:rgba(220,38,38,.22);
  --ok:#10b981;--okd:rgba(16,185,129,.12);
  --tx:#f1f5f9;--tx2:#94a3b8;--tx3:#64748b;
  --bd:#1a2233;--bd2:#2a3548;
  --r:10px;--rs:6px;--rl:14px;
  --font:system-ui,-apple-system,'Segoe UI',Roboto,sans-serif;
}
html{font-size:16px;scroll-behavior:smooth}
body{font-family:var(--font);background:var(--bg);color:var(--tx);min-height:100vh;-webkit-font-smoothing:antialiased;line-height:1.5}
a{color:var(--ac);text-decoration:none}
a:hover{color:var(--ac2)}

/* ===== APPBAR ===== */
.bar{position:fixed;top:0;left:0;right:0;z-index:1000;height:48px;display:flex;align-items:center;justify-content:space-between;padding:0 16px;background:rgba(6,10,17,.92);backdrop-filter:blur(20px);-webkit-backdrop-filter:blur(20px);border-bottom:1px solid rgba(255,255,255,.04)}
.bar-l{display:flex;align-items:center;gap:10px}
.lm{width:30px;height:30px;border-radius:8px;background:linear-gradient(135deg,var(--ac),#b91c1c);display:flex;align-items:center;justify-content:center;box-shadow:0 0 14px var(--acg);flex-shrink:0}
.lm svg{width:16px;height:16px;fill:#fff}
.lt{font-size:1rem;font-weight:700;letter-spacing:1.5px}.lt span{color:var(--ac)}
.bar-tag{font-size:.6rem;font-weight:600;color:var(--ac);background:var(--acg);padding:2px 8px;border-radius:10px;letter-spacing:.5px}
.back-btn{display:flex;align-items:center;gap:6px;padding:6px 12px;border-radius:var(--rs);background:var(--sf2);color:var(--tx2);font-size:.72rem;font-weight:600;border:1px solid var(--bd);cursor:pointer;transition:.2s}
.back-btn:hover{border-color:var(--ac);color:var(--ac)}
.back-btn svg{width:16px;height:16px;fill:currentColor}

/* ===== MAP ===== */
.map-wrap{position:relative;margin-top:48px;height:50vh;min-height:300px;border-bottom:2px solid var(--ac)}
#flightMap{width:100%;height:100%;z-index:1}
.leaflet-control-attribution{display:none!important}

/* ===== TELEMETRY PANEL ===== */
.tele-panel{padding:20px 16px 40px;max-width:800px;margin:0 auto}
.tele-title{font-size:.68rem;font-weight:700;color:var(--tx3);text-transform:uppercase;letter-spacing:1.2px;margin-bottom:14px;display:flex;align-items:center;gap:8px}
.tele-title svg{width:16px;height:16px;fill:var(--ac)}

/* Glassmorphic card */
.glass{background:rgba(15,21,32,.8);backdrop-filter:blur(16px);-webkit-backdrop-filter:blur(16px);border:1px solid rgba(255,255,255,.06);border-radius:var(--rl);box-shadow:0 8px 32px rgba(0,0,0,.4);overflow:hidden}

/* Telemetry grid */
.tele-grid{display:grid;grid-template-columns:repeat(auto-fit,minmax(160px,1fr));gap:1px;background:rgba(255,255,255,.04)}
.tele-cell{padding:16px;background:rgba(15,21,32,.9)}
.tele-label{font-size:.6rem;font-weight:600;color:var(--tx3);text-transform:uppercase;letter-spacing:.8px;margin-bottom:4px;display:flex;align-items:center;gap:5px}
.tele-label svg{width:12px;height:12px;fill:var(--tx3)}
.tele-val{font-size:1.1rem;font-weight:700;color:var(--tx);font-variant-numeric:tabular-nums}
.tele-val.accent{color:var(--ac)}
.tele-val.green{color:var(--ok)}
.tele-sub{font-size:.58rem;color:var(--tx3);margin-top:2px}

/* Coordinates card */
.coord-grid{display:grid;grid-template-columns:1fr 1fr;gap:1px;background:rgba(255,255,255,.04);margin-top:16px}
.coord-cell{padding:14px 16px;background:rgba(15,21,32,.9)}
.coord-dot{width:10px;height:10px;border-radius:50%;display:inline-block;margin-right:6px;vertical-align:middle}
.coord-dot.start{background:var(--ok);box-shadow:0 0 8px rgba(16,185,129,.6)}
.coord-dot.end{background:var(--ac);box-shadow:0 0 8px var(--acg)}

/* Status badge */
.status-badge{display:inline-flex;align-items:center;gap:5px;padding:4px 12px;border-radius:14px;font-size:.62rem;font-weight:700;letter-spacing:.5px;text-transform:uppercase}
.status-badge.landed{background:rgba(16,185,129,.15);color:var(--ok);border:1px solid rgba(16,185,129,.3)}
.status-badge svg{width:10px;height:10px;fill:currentColor}

/* Footer */
.tele-footer{text-align:center;padding:20px;font-size:.6rem;color:var(--tx3)}
.tele-footer strong{color:var(--ac)}

/* ===== RESPONSIVE ===== */
@media(max-width:640px){
  .map-wrap{height:40vh;min-height:250px}
  .tele-grid{grid-template-columns:1fr 1fr}
  .tele-val{font-size:.95rem}
  .coord-grid{grid-template-columns:1fr}
  .bar{height:44px;padding:0 10px}
}
@media(max-width:380px){
  .tele-grid{grid-template-columns:1fr}
}
</style>
</head>
<body>

<!-- ===== APPBAR ===== -->
<header class="bar">
  <div class="bar-l">
    <div class="lm"><svg viewBox="0 0 24 24"><path d="M12 4.5C7 4.5 2.73 7.61 1 12c1.73 4.39 6 7.5 11 7.5s9.27-3.11 11-7.5c-1.73-4.39-6-7.5-11-7.5zm0 12.5c-2.76 0-5-2.24-5-5s2.24-5 5-5 5 2.24 5 5-2.24 5-5 5zm0-8c-1.66 0-3 1.34-3 3s1.34 3 3 3 3-1.34 3-3-1.34-3-3-3z"/></svg></div>
    <span class="lt">TRINE<span>TRA</span></span>
    <span class="bar-tag">FLIGHT REPORT</span>
  </div>
  <button class="back-btn" onclick="window.location.href='/'">
    <svg viewBox="0 0 24 24"><path d="M20 11H7.83l5.59-5.59L12 4l-8 8 8 8 1.41-1.41L7.83 13H20v-2z"/></svg>
    Back
  </button>
</header>

<!-- ===== MAP CONTAINER ===== -->
<div class="map-wrap">
  <div id="flightMap"></div>
</div>

<!-- ===== TELEMETRY PANEL ===== -->
<div class="tele-panel">

  <!-- Status -->
  <div style="display:flex;align-items:center;justify-content:space-between;margin-bottom:16px">
    <div class="tele-title" style="margin-bottom:0">
      <svg viewBox="0 0 24 24"><path d="M21 16v-2l-8-5V3.5A1.5 1.5 0 0011.5 2 1.5 1.5 0 0010 3.5V9l-8 5v2l8-2.5V19l-2 1.5V22l3.5-1 3.5 1v-1.5L13 19v-5.5l8 2.5z"/></svg>
      Flight Telemetry
    </div>
    <span class="status-badge landed">
      <svg viewBox="0 0 24 24"><path d="M12 2a10 10 0 100 20 10 10 0 000-20zm-2 15l-5-5 1.41-1.41L10 14.17l7.59-7.59L19 8l-9 9z"/></svg>
      Landed
    </span>
  </div>

  <!-- Telemetry Cards -->
  <div class="glass">
    <div class="tele-grid">
      <div class="tele-cell">
        <div class="tele-label"><svg viewBox="0 0 24 24"><path d="M11.99 2C6.47 2 2 6.48 2 12s4.47 10 9.99 10C17.52 22 22 17.52 22 12S17.52 2 11.99 2zM12 20a8 8 0 110-16 8 8 0 010 16zm.5-13H11v6l5.25 3.15.75-1.23-4.5-2.67z"/></svg>Takeoff Time</div>
        <div class="tele-val" id="teleTakeoff">--</div>
        <div class="tele-sub">Mission start</div>
      </div>
      <div class="tele-cell">
        <div class="tele-label"><svg viewBox="0 0 24 24"><path d="M11.99 2C6.47 2 2 6.48 2 12s4.47 10 9.99 10C17.52 22 22 17.52 22 12S17.52 2 11.99 2zM12 20a8 8 0 110-16 8 8 0 010 16zm.5-13H11v6l5.25 3.15.75-1.23-4.5-2.67z"/></svg>Landing Time</div>
        <div class="tele-val accent" id="teleLanding">--</div>
        <div class="tele-sub">QR trigger received</div>
      </div>
      <div class="tele-cell">
        <div class="tele-label"><svg viewBox="0 0 24 24"><path d="M15 1H9v2h6V1zm-4 13h2V8h-2v6zm8.03-6.61l1.42-1.42c-.43-.51-.9-.99-1.41-1.41l-1.42 1.42A8.962 8.962 0 0012 4a9 9 0 00-9 9c0 4.97 4.02 9 9 9a8.994 8.994 0 007.03-14.61zM12 20a7 7 0 110-14 7 7 0 010 14z"/></svg>Flight Duration</div>
        <div class="tele-val green" id="teleDuration">--</div>
        <div class="tele-sub">Total airborne time</div>
      </div>
      <div class="tele-cell">
        <div class="tele-label"><svg viewBox="0 0 24 24"><path d="M12 2C8.13 2 5 5.13 5 9c0 5.25 7 13 7 13s7-7.75 7-13c0-3.87-3.13-7-7-7zm0 9.5a2.5 2.5 0 010-5 2.5 2.5 0 010 5z"/></svg>Shortest Distance</div>
        <div class="tele-val accent" id="teleDistance">--</div>
        <div class="tele-sub">Haversine (air distance)</div>
      </div>
      <div class="tele-cell">
        <div class="tele-label"><svg viewBox="0 0 24 24"><path d="M20.38 8.57l-1.23 1.85a8 8 0 01-.22 7.58H5.07A8 8 0 0115.58 6.85l1.85-1.23A10 10 0 003.35 19a2 2 0 001.72 1h13.85a2 2 0 001.74-1 10 10 0 00-.27-10.44z"/><path d="M10.59 15.41a2 2 0 002.83 0l5.66-8.49-8.49 5.66a2 2 0 000 2.83z"/></svg>Avg Speed</div>
        <div class="tele-val" id="teleSpeed">--</div>
        <div class="tele-sub">distance &divide; duration</div>
      </div>
      <div class="tele-cell">
        <div class="tele-label"><svg viewBox="0 0 24 24"><path d="M12 2a10 10 0 100 20 10 10 0 000-20zm1 15h-2v-6h2v6zm0-8h-2V7h2v2z"/></svg>Mode</div>
        <div class="tele-val" style="font-size:.85rem">Direct Flight</div>
        <div class="tele-sub">Point-to-point path</div>
      </div>
    </div>
  </div>

  <!-- Coordinate Details -->
  <div class="tele-title" style="margin-top:24px">
    <svg viewBox="0 0 24 24"><path d="M12 2C8.13 2 5 5.13 5 9c0 5.25 7 13 7 13s7-7.75 7-13c0-3.87-3.13-7-7-7zm0 9.5a2.5 2.5 0 010-5 2.5 2.5 0 010 5z"/></svg>
    GPS Coordinates
  </div>
  <div class="glass">
    <div class="coord-grid">
      <div class="coord-cell">
        <div class="tele-label"><span class="coord-dot start"></span> Start Position</div>
        <div class="tele-val green" style="font-size:.88rem" id="coordStart">--</div>
        <div class="tele-sub">Takeoff location (Kathmandu)</div>
      </div>
      <div class="coord-cell">
        <div class="tele-label"><span class="coord-dot end"></span> Landing Position</div>
        <div class="tele-val accent" style="font-size:.88rem" id="coordEnd">--</div>
        <div class="tele-sub">QR landing zone</div>
      </div>
    </div>
  </div>

  <!-- Footer -->
  <div class="tele-footer">
    <strong>TRINETRA</strong> Drone Flight Report &bull; Demo Mode &mdash; GPS values are simulated<br>
    Preparing for future real GPS integration
  </div>
</div>

<!-- ===================================================================
     flight_logic.js — Haversine, map rendering, telemetry computation
     =================================================================== -->
<script>
(function(){
  'use strict';

  /* =============================================
   *  FAKE GPS COORDINATES (Kathmandu, Nepal)
   *  Replace with real GPS data in production
   * ============================================= */
  var FLIGHT = {
    start: { lat: 27.7172, lng: 85.3240, label: 'Takeoff Point' },
    end:   { lat: 27.7000, lng: 85.3333, label: 'Landing Zone'  }
  };

  /* =============================================
   *  FAKE FLIGHT TIMES (demo mode)
   *  Replace with real telemetry in production
   * ============================================= */
  var TIMES = {
    takeoff:  new Date(2026, 1, 25, 10, 15, 0),   // Feb 25, 2026 10:15:00
    landing:  new Date(2026, 1, 25, 10, 22, 45)    // Feb 25, 2026 10:22:45
  };

  /* =============================================
   *  HAVERSINE FORMULA
   *  Calculates shortest distance (meters)
   *  between two GPS coordinates on a sphere
   * ============================================= */
  function haversineDistance(lat1, lon1, lat2, lon2) {
    var R = 6371000; // Earth radius in meters
    var dLat = toRad(lat2 - lat1);
    var dLon = toRad(lon2 - lon1);
    var a = Math.sin(dLat / 2) * Math.sin(dLat / 2) +
            Math.cos(toRad(lat1)) * Math.cos(toRad(lat2)) *
            Math.sin(dLon / 2) * Math.sin(dLon / 2);
    var c = 2 * Math.atan2(Math.sqrt(a), Math.sqrt(1 - a));
    return R * c; // distance in meters
  }

  function toRad(deg) {
    return deg * (Math.PI / 180);
  }

  /* =============================================
   *  FORMAT HELPERS
   * ============================================= */
  function formatTime(date) {
    var h = String(date.getHours()).padStart(2, '0');
    var m = String(date.getMinutes()).padStart(2, '0');
    var s = String(date.getSeconds()).padStart(2, '0');
    return h + ':' + m + ':' + s;
  }

  function formatDuration(ms) {
    var totalSec = Math.floor(ms / 1000);
    var min = Math.floor(totalSec / 60);
    var sec = totalSec % 60;
    return min + 'm ' + sec + 's';
  }

  /* =============================================
   *  COMPUTE TELEMETRY
   * ============================================= */
  var distanceM = haversineDistance(
    FLIGHT.start.lat, FLIGHT.start.lng,
    FLIGHT.end.lat,   FLIGHT.end.lng
  );
  var distanceKm = (distanceM / 1000).toFixed(3);
  var durationMs = TIMES.landing - TIMES.takeoff;
  var durationSec = durationMs / 1000;
  var speedMps = distanceM / durationSec;
  var speedKmh = (speedMps * 3.6).toFixed(1);

  /* =============================================
   *  POPULATE TELEMETRY UI
   * ============================================= */
  function populateTelemetry() {
    document.getElementById('teleTakeoff').textContent  = formatTime(TIMES.takeoff);
    document.getElementById('teleLanding').textContent  = formatTime(TIMES.landing);
    document.getElementById('teleDuration').textContent = formatDuration(durationMs);
    document.getElementById('teleDistance').textContent  = distanceKm + ' km';
    document.getElementById('teleSpeed').textContent    = speedKmh + ' km/h';

    document.getElementById('coordStart').textContent =
      FLIGHT.start.lat.toFixed(4) + ', ' + FLIGHT.start.lng.toFixed(4);
    document.getElementById('coordEnd').textContent =
      FLIGHT.end.lat.toFixed(4) + ', ' + FLIGHT.end.lng.toFixed(4);
  }

  /* =============================================
   *  RENDER MAP (Leaflet + OpenStreetMap)
   * ============================================= */
  function renderMap() {
    // Center map between start and end
    var centerLat = (FLIGHT.start.lat + FLIGHT.end.lat) / 2;
    var centerLng = (FLIGHT.start.lng + FLIGHT.end.lng) / 2;

    var map = L.map('flightMap', {
      center: [centerLat, centerLng],
      zoom: 14,
      zoomControl: true,
      attributionControl: false
    });

    // Dark map tiles (CartoDB Dark Matter)
    L.tileLayer('https://{s}.basemaps.cartocdn.com/dark_all/{z}/{x}/{y}{r}.png', {
      maxZoom: 19,
      subdomains: 'abcd'
    }).addTo(map);

    // Custom marker icons
    var greenIcon = L.divIcon({
      className: '',
      html: '<div style="width:18px;height:18px;background:#10b981;border:3px solid #fff;border-radius:50%;box-shadow:0 0 12px rgba(16,185,129,.7)"></div>',
      iconSize: [18, 18],
      iconAnchor: [9, 9],
      popupAnchor: [0, -12]
    });

    var redIcon = L.divIcon({
      className: '',
      html: '<div style="width:18px;height:18px;background:#dc2626;border:3px solid #fff;border-radius:50%;box-shadow:0 0 12px rgba(220,38,38,.7)"></div>',
      iconSize: [18, 18],
      iconAnchor: [9, 9],
      popupAnchor: [0, -12]
    });

    // Start marker (green)
    L.marker([FLIGHT.start.lat, FLIGHT.start.lng], { icon: greenIcon })
      .addTo(map)
      .bindPopup(
        '<b style="color:#10b981">' + FLIGHT.start.label + '</b><br>' +
        FLIGHT.start.lat.toFixed(4) + ', ' + FLIGHT.start.lng.toFixed(4)
      );

    // End marker (red)
    L.marker([FLIGHT.end.lat, FLIGHT.end.lng], { icon: redIcon })
      .addTo(map)
      .bindPopup(
        '<b style="color:#dc2626">' + FLIGHT.end.label + '</b><br>' +
        FLIGHT.end.lat.toFixed(4) + ', ' + FLIGHT.end.lng.toFixed(4) +
        '<br><small>Distance: ' + distanceKm + ' km</small>'
      );

    // Straight red flight path polyline
    var flightPath = L.polyline(
      [
        [FLIGHT.start.lat, FLIGHT.start.lng],
        [FLIGHT.end.lat,   FLIGHT.end.lng]
      ],
      {
        color: '#dc2626',
        weight: 3,
        opacity: 0.9,
        dashArray: '10, 6',
        lineCap: 'round'
      }
    ).addTo(map);

    // Fit map to show entire path with padding
    map.fitBounds(flightPath.getBounds().pad(0.3));

    // Distance label at midpoint
    var midLat = (FLIGHT.start.lat + FLIGHT.end.lat) / 2;
    var midLng = (FLIGHT.start.lng + FLIGHT.end.lng) / 2;
    L.marker([midLat, midLng], {
      icon: L.divIcon({
        className: '',
        html: '<div style="background:rgba(220,38,38,.9);color:#fff;padding:3px 10px;border-radius:12px;font-size:11px;font-weight:700;white-space:nowrap;box-shadow:0 2px 8px rgba(0,0,0,.4);font-family:system-ui,sans-serif">' + distanceKm + ' km</div>',
        iconSize: [80, 24],
        iconAnchor: [40, 12]
      })
    }).addTo(map);
  }

  /* =============================================
   *  INITIALIZE ON DOM READY
   * ============================================= */
  window.addEventListener('load', function() {
    populateTelemetry();
    renderMap();
  });

})();
</script>

</body>
</html>
)rawliteral";
#endif
