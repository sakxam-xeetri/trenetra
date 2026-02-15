/*
 * =============================================================
 *  TRINETRA — ESP32-CAM Surveillance System
 *  Web Interface v3.0 — Immersive Stream-First UI
 * =============================================================
 *  Hero stream fills viewport, floating control bar,
 *  slide-in drawers, shutter flash, no toasts, all SVG.
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
<meta name="viewport" content="width=device-width,initial-scale=1,maximum-scale=5">
<meta name="theme-color" content="#060a11">
<title>Trinetra</title>
<style>
*,*::before,*::after{margin:0;padding:0;box-sizing:border-box}
:root{
--bg:#060a11;--sf:#0f1520;--sf2:#1a2233;--sf3:#2a3548;
--ac:#dc2626;--ac2:#f87171;--acg:rgba(220,38,38,.22);--acd:rgba(220,38,38,.08);
--ok:#10b981;--okd:rgba(16,185,129,.12);
--er:#ef4444;--erd:rgba(239,68,68,.12);
--wn:#f59e0b;--wnd:rgba(245,158,11,.12);
--tx:#f1f5f9;--tx2:#94a3b8;--tx3:#64748b;
--bd:#1a2233;--bd2:#2a3548;
--r:10px;--rs:6px;--rl:14px;
--font:system-ui,-apple-system,'Segoe UI',Roboto,sans-serif;
--safe-b:env(safe-area-inset-bottom,0px);
--bar:48px;--cbar:52px;--mnav:0px;
}
html{font-size:16px;scroll-behavior:smooth}
body{font-family:var(--font);background:var(--bg);color:var(--tx);min-height:100vh;min-height:100dvh;overflow:hidden;-webkit-font-smoothing:antialiased;line-height:1.5}
button{font-family:inherit;cursor:pointer;border:none;background:none;color:inherit}
:focus-visible{outline:2px solid var(--ac);outline-offset:2px;border-radius:4px}
::-webkit-scrollbar{width:5px}
::-webkit-scrollbar-track{background:var(--sf)}
::-webkit-scrollbar-thumb{background:var(--sf3);border-radius:3px}

/* ===== APPBAR ===== */
.bar{position:fixed;top:0;left:0;right:0;z-index:100;height:var(--bar);display:flex;align-items:center;justify-content:space-between;padding:0 12px;background:rgba(6,10,17,.88);backdrop-filter:blur(20px);-webkit-backdrop-filter:blur(20px);border-bottom:1px solid rgba(255,255,255,.04)}
.bar-l{display:flex;align-items:center;gap:10px}
.bar-r{display:flex;align-items:center;gap:2px}
.lm{width:30px;height:30px;border-radius:8px;background:linear-gradient(135deg,var(--ac),#b91c1c);display:flex;align-items:center;justify-content:center;box-shadow:0 0 14px var(--acg);flex-shrink:0}
.lm svg{width:16px;height:16px;fill:#fff}
.lt{font-size:1rem;font-weight:700;letter-spacing:1.5px}.lt span{color:var(--ac)}
.chip{display:flex;align-items:center;gap:5px;padding:3px 8px;border-radius:14px;background:var(--sf2);font-size:.6rem;font-weight:600;color:var(--tx3);border:1px solid var(--bd);margin-right:4px}
.chip .d{width:6px;height:6px;border-radius:50%;background:var(--tx3);transition:.3s}
.chip .d.on{background:var(--ok);box-shadow:0 0 8px rgba(16,185,129,.6);animation:pulse 2s infinite}
@keyframes pulse{0%,100%{opacity:1}50%{opacity:.4}}
.ib{width:34px;height:34px;border-radius:var(--rs);display:flex;align-items:center;justify-content:center;color:var(--tx3);transition:.2s}
.ib:hover{background:var(--sf2);color:var(--tx)}
.ib.active{color:var(--ac);background:var(--acd)}
.ib svg{width:18px;height:18px;fill:currentColor}

/* ===== HERO STREAM (FULL VIEWPORT WITH CURVED EDGES) ===== */
.hero{position:fixed;top:calc(var(--bar) + 8px);left:8px;right:8px;bottom:calc(var(--mnav) + 8px);background:#000;display:flex;align-items:center;justify-content:center;overflow:hidden;border-radius:20px;box-shadow:0 4px 24px rgba(0,0,0,.3)}
.hero img{width:100%;height:100%;object-fit:contain;display:block;transition:none;border-radius:20px}
.hero img.hide{display:none}

/* Placeholder */
.ph{position:absolute;inset:0;display:flex;align-items:center;justify-content:center;cursor:pointer;z-index:3;transition:opacity .3s}
.ph.hide{opacity:0;pointer-events:none}
.ph-in{display:flex;flex-direction:column;align-items:center;gap:16px}
.ph-btn{width:80px;height:80px;border-radius:50%;border:2px solid var(--ac);display:flex;align-items:center;justify-content:center;transition:.3s;background:rgba(6,182,212,.06)}
.ph-btn svg{width:36px;height:36px;fill:var(--ac);margin-left:4px}
.ph:hover .ph-btn{transform:scale(1.08);box-shadow:0 0 32px var(--acg);background:rgba(6,182,212,.12)}
.ph-txt{font-size:.78rem;color:var(--tx3);font-weight:500}

/* HUD top */
.hud{position:absolute;top:0;left:0;right:0;padding:10px 14px;display:flex;justify-content:space-between;align-items:flex-start;background:linear-gradient(180deg,rgba(0,0,0,.65) 0%,transparent 100%);pointer-events:none;z-index:5;opacity:0;transition:opacity .3s}
.hud.show{opacity:1}
.bg{font-size:.58rem;font-weight:700;padding:3px 8px;border-radius:14px;letter-spacing:.8px;text-transform:uppercase;display:flex;align-items:center;gap:4px}
.bg svg{width:8px;height:8px;fill:currentColor}
.bg-live{background:var(--er);color:#fff}
.bg-rec{background:var(--ac);color:#fff;animation:recPulse 1.5s ease-in-out infinite}
@keyframes recPulse{0%,100%{opacity:1}50%{opacity:.7}}
.bg-res{background:rgba(255,255,255,.1);color:#ccc;backdrop-filter:blur(4px)}
.timer{font-variant-numeric:tabular-nums;margin-left:2px;font-weight:600}

/* Shutter Flash */
.shutter{position:absolute;inset:0;background:#fff;opacity:0;pointer-events:none;z-index:12}
.shutter.flash{animation:shutterAnim .28s ease-out}
@keyframes shutterAnim{0%{opacity:0}25%{opacity:.32}100%{opacity:0}}

/* Loading spinner */
.spinner{position:absolute;inset:0;display:flex;align-items:center;justify-content:center;z-index:4;opacity:0;pointer-events:none;transition:opacity .3s}
.spinner.show{opacity:1}
.spin-ring{width:40px;height:40px;border:3px solid var(--sf3);border-top-color:var(--ac);border-radius:50%;animation:spin .8s linear infinite}
@keyframes spin{to{transform:rotate(360deg)}}

/* Notify pill (minimal, replaces toasts) */
.notify{position:absolute;bottom:100px;left:50%;transform:translateX(-50%);padding:8px 18px;border-radius:24px;background:rgba(10,14,23,.95);backdrop-filter:blur(16px);color:var(--tx);font-size:.75rem;font-weight:500;opacity:0;transition:opacity .3s;pointer-events:none;z-index:15;white-space:nowrap;border:1px solid rgba(255,255,255,.08);display:flex;align-items:center;gap:8px;max-width:90%;box-shadow:0 4px 20px rgba(0,0,0,.4)}
.notify svg{width:14px;height:14px;flex-shrink:0}
.notify.show{opacity:1}
.notify.n-ok svg{fill:var(--ok)}
.notify.n-er svg{fill:var(--er)}
.notify.n-wn svg{fill:var(--wn)}

/* ===== CONTROL BAR (PRO FLOATING DOCK) ===== */
.cbar{position:absolute;bottom:16px;left:50%;transform:translateX(-50%);height:64px;display:flex;align-items:center;justify-content:center;gap:10px;padding:0 20px;background:rgba(10,14,23,.92);backdrop-filter:blur(24px);-webkit-backdrop-filter:blur(24px);border:1px solid rgba(255,255,255,.06);border-radius:32px;z-index:10;box-shadow:0 8px 32px rgba(0,0,0,.5)}
.cb{width:48px;height:48px;border-radius:50%;display:flex;align-items:center;justify-content:center;transition:all .2s ease;position:relative;flex-shrink:0;box-shadow:0 2px 8px rgba(0,0,0,.3)}
.cb svg{width:22px;height:22px;fill:currentColor}
.cb:active{transform:scale(.92)}
.cb:hover{transform:scale(1.08)}
.cb-play{background:linear-gradient(135deg,var(--ac),#b91c1c);color:#fff}
.cb-play:hover{box-shadow:0 0 24px var(--acg)}
.cb-stop{background:linear-gradient(135deg,var(--er),#dc2626);color:#fff}
.cb-stop:hover{box-shadow:0 0 24px var(--erd)}
.cb-def{background:rgba(255,255,255,.08);color:var(--tx);border:1px solid rgba(255,255,255,.1)}
.cb-def:hover{background:rgba(255,255,255,.15);color:#fff;box-shadow:0 0 16px rgba(255,255,255,.1)}
.cb-led-on{background:linear-gradient(135deg,#fbbf24,#f59e0b);color:#000;border:none;box-shadow:0 0 20px rgba(251,191,36,.5);animation:ledGlow 1.5s ease infinite}
@keyframes ledGlow{0%,100%{box-shadow:0 0 20px rgba(251,191,36,.5)}50%{box-shadow:0 0 30px rgba(251,191,36,.8)}}
.cb.fb-ok{background:linear-gradient(135deg,var(--ok),#059669)!important;color:#fff!important;border-color:var(--ok)!important;box-shadow:0 0 20px rgba(16,185,129,.5)!important}
.cb.fb-er{background:linear-gradient(135deg,var(--er),#dc2626)!important;color:#fff!important;border-color:var(--er)!important;box-shadow:0 0 20px rgba(239,68,68,.5)!important}
.cb-sep{width:1px;height:32px;background:rgba(255,255,255,.1);margin:0 6px;flex-shrink:0}
.cb-sel{appearance:none;background:rgba(255,255,255,.08);color:var(--tx);border:1px solid rgba(255,255,255,.1);border-radius:24px;padding:10px 32px 10px 14px;font-size:.75rem;font-weight:600;font-family:inherit;cursor:pointer;background-image:url("data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' width='10' height='10' fill='%23f1f5f9'%3E%3Cpath d='M5 7L1 3h8z'/%3E%3C/svg%3E");background-repeat:no-repeat;background-position:right 12px center;transition:.2s;flex-shrink:0}
.cb-sel:hover{background:rgba(255,255,255,.15);color:#fff}
.cb-sel:focus{outline:none;border-color:var(--ac);box-shadow:0 0 0 3px var(--acg)}

/* ===== DRAWERS ===== */
.backdrop{position:fixed;inset:0;background:rgba(0,0,0,.5);z-index:150;opacity:0;pointer-events:none;transition:opacity .3s}
.backdrop.show{opacity:1;pointer-events:auto}
.drawer{position:fixed;top:var(--bar);right:0;bottom:var(--mnav);width:360px;max-width:100%;background:var(--sf);border-left:1px solid var(--bd);transform:translateX(100%);transition:transform .32s cubic-bezier(.4,0,.2,1);z-index:200;display:flex;flex-direction:column;overflow:hidden}
.drawer.open{transform:translateX(0)}
.dh{display:flex;align-items:center;justify-content:space-between;padding:14px 16px;border-bottom:1px solid var(--bd);flex-shrink:0}
.dh h2{font-size:.85rem;font-weight:700;display:flex;align-items:center;gap:8px}
.dh h2 svg{width:18px;height:18px;fill:var(--ac)}
.db{flex:1;overflow-y:auto;padding:14px 16px}

/* Form controls in drawers */
.fg{margin-bottom:12px}
.fg:last-child{margin-bottom:0}
.fr{display:flex;align-items:center;gap:10px}
.fl{font-size:.68rem;font-weight:600;color:var(--tx2);min-width:72px;flex-shrink:0;text-transform:uppercase;letter-spacing:.5px}
.f-sel{flex:1;padding:9px 30px 9px 10px;border-radius:var(--rs);background:var(--sf2);color:var(--tx);border:1px solid var(--bd);font-size:.76rem;font-family:inherit;appearance:none;cursor:pointer;background-image:url("data:image/svg+xml,%3Csvg xmlns='http://www.w3.org/2000/svg' width='10' height='10' fill='%2394a3b8'%3E%3Cpath d='M5 7L1 3h8z'/%3E%3C/svg%3E");background-repeat:no-repeat;background-position:right 10px center;transition:.2s}
.f-sel:focus{outline:none;border-color:var(--ac);box-shadow:0 0 0 3px var(--acg)}
.f-in{flex:1;padding:9px 10px;border-radius:var(--rs);background:var(--sf2);color:var(--tx);border:1px solid var(--bd);font-size:.76rem;font-family:inherit;transition:.2s}
.f-in:focus{outline:none;border-color:var(--ac);box-shadow:0 0 0 3px var(--acg)}
.f-in::placeholder{color:var(--tx3)}
.rw{flex:1;display:flex;align-items:center;gap:8px}
input[type=range]{flex:1;-webkit-appearance:none;height:4px;border-radius:2px;background:var(--sf3);outline:none}
input[type=range]::-webkit-slider-thumb{-webkit-appearance:none;width:16px;height:16px;border-radius:50%;background:var(--ac);cursor:pointer;box-shadow:0 0 6px var(--acg)}
input[type=range]::-moz-range-thumb{width:16px;height:16px;border-radius:50%;border:none;background:var(--ac);cursor:pointer}
.rv{min-width:24px;text-align:center;font-size:.7rem;color:var(--ac);font-weight:700;font-variant-numeric:tabular-nums}

/* Section divider in drawer */
.dsep{height:1px;background:var(--bd);margin:14px 0}
.dsec{font-size:.62rem;font-weight:700;color:var(--tx3);text-transform:uppercase;letter-spacing:1px;margin-bottom:10px;display:flex;align-items:center;gap:6px}
.dsec svg{width:14px;height:14px;fill:var(--tx3)}

/* Toggle */
.tg{display:flex;align-items:center;justify-content:space-between;padding:6px 0;cursor:pointer}
.tg-label{font-size:.74rem;color:var(--tx2)}
.tg input{display:none}
.tg-track{width:36px;height:20px;border-radius:10px;background:var(--sf3);transition:.25s;position:relative;flex-shrink:0}
.tg-track::after{content:'';position:absolute;top:2px;left:2px;width:16px;height:16px;border-radius:50%;background:#fff;transition:.25s;box-shadow:0 1px 3px rgba(0,0,0,.3)}
.tg input:checked+.tg-track{background:var(--ac)}
.tg input:checked+.tg-track::after{left:18px}

/* ===== MONITOR STATS ===== */
.sg{display:grid;grid-template-columns:1fr 1fr;gap:8px}
.sc{padding:10px;background:var(--sf2);border-radius:var(--rs);border:1px solid var(--bd)}
.sc-h{display:flex;align-items:center;gap:5px;margin-bottom:4px}
.sc-h svg{width:12px;height:12px;fill:var(--tx3)}
.sc-l{font-size:.58rem;color:var(--tx3);font-weight:600;text-transform:uppercase;letter-spacing:.5px}
.sc-v{font-size:.88rem;font-weight:700;color:var(--ac);font-variant-numeric:tabular-nums}
.sc-s{font-size:.56rem;color:var(--tx3);margin-top:1px}
.prg{width:100%;height:3px;border-radius:2px;background:var(--sf3);margin-top:5px;overflow:hidden}
.prg-b{height:100%;border-radius:2px;transition:width .6s;background:var(--ac)}
.prg-b.wn{background:var(--wn)}
.prg-b.cr{background:var(--er)}

/* ===== WIFI ===== */
.wsc{display:flex;align-items:center;gap:10px;padding:10px;background:var(--sf2);border-radius:var(--rs);border:1px solid var(--bd);margin-bottom:10px}
.wsc-i{width:32px;height:32px;border-radius:var(--rs);display:flex;align-items:center;justify-content:center;flex-shrink:0}
.wsc-i svg{width:18px;height:18px}
.wsc-i.con{background:var(--okd)}.wsc-i.con svg{fill:var(--ok)}
.wsc-i.dis{background:var(--sf3)}.wsc-i.dis svg{fill:var(--tx3)}
.wsc-t{font-size:.78rem;font-weight:600}
.wsc-d{font-size:.66rem;color:var(--tx3);margin-top:1px;overflow:hidden;text-overflow:ellipsis;white-space:nowrap}
.ni{display:flex;align-items:center;justify-content:space-between;padding:9px 12px;background:var(--sf2);border-radius:var(--rs);margin-bottom:5px;cursor:pointer;transition:.2s;border:1px solid transparent}
.ni:hover{border-color:var(--ac);background:#0c1d2e}
.ni.sel{border-color:var(--ac);background:#0c2438}
.ni-n{font-size:.78rem;font-weight:600}
.ni-m{display:flex;align-items:center;gap:6px;font-size:.64rem;color:var(--tx3)}
.ni-m svg{width:13px;height:13px;fill:currentColor}

/* Info rows */
.irow{display:flex;justify-content:space-between;padding:7px 10px;background:var(--sf2);border-radius:var(--rs);font-size:.72rem;margin-bottom:4px}
.irow-l{color:var(--tx3)}
.irow-v{font-weight:600}
.irow-ac{color:var(--ac)}

/* Buttons */
.btn{padding:9px 14px;border-radius:var(--rs);font-size:.74rem;font-weight:600;display:inline-flex;align-items:center;justify-content:center;gap:6px;transition:.2s}
.btn:active{transform:scale(.97)}
.btn svg{width:14px;height:14px;fill:currentColor;flex-shrink:0}
.btn-p{background:var(--ac);color:#fff}.btn-p:hover{background:var(--ac2);box-shadow:0 0 16px var(--acg)}
.btn-s{background:var(--sf2);color:var(--tx);border:1px solid var(--bd)}.btn-s:hover{border-color:var(--ac);color:var(--ac)}
.btn-d{background:var(--er);color:#fff}.btn-d:hover{background:#dc2626}
.btn-g{background:transparent;color:var(--tx2);border:1px solid var(--bd2)}.btn-g:hover{background:var(--sf2);color:var(--tx)}
.btn:disabled{opacity:.4;cursor:not-allowed;transform:none}
.bg2{display:grid;gap:8px}.bg2.c2{grid-template-columns:1fr 1fr}

/* ===== MODAL ===== */
.mov{position:fixed;inset:0;z-index:300;background:rgba(0,0,0,.7);backdrop-filter:blur(8px);display:none;align-items:center;justify-content:center;padding:16px}
.mov.show{display:flex}
.mo{background:var(--sf);border:1px solid var(--bd);border-radius:var(--rl);max-width:400px;width:100%;max-height:80vh;display:flex;flex-direction:column;box-shadow:0 8px 32px rgba(0,0,0,.5);animation:moIn .3s ease}
@keyframes moIn{from{opacity:0;transform:scale(.95) translateY(8px)}to{opacity:1;transform:none}}
.mo-h{padding:14px 16px;border-bottom:1px solid var(--bd);display:flex;align-items:center;justify-content:space-between}
.mo-h h2{font-size:.9rem;font-weight:700;display:flex;align-items:center;gap:8px}
.mo-h h2 svg{width:18px;height:18px;fill:var(--ac)}
.mo-b{padding:16px;overflow-y:auto;flex:1}
.mo-f{padding:12px 16px;border-top:1px solid var(--bd);display:flex;justify-content:flex-end}
.about-icon{width:56px;height:56px;border-radius:14px;background:linear-gradient(135deg,var(--ac),#b91c1c);display:flex;align-items:center;justify-content:center;box-shadow:0 0 20px var(--acg)}
.about-icon svg{width:28px;height:28px;fill:#fff}

/* ===== FULLSCREEN ===== */
.fso{display:none;position:fixed;inset:0;z-index:500;background:#000;align-items:center;justify-content:center}
.fso.show{display:flex}
.fso img{max-width:100%;max-height:100%;object-fit:contain}
.fso-x{position:fixed;top:14px;right:14px;z-index:501;width:38px;height:38px;border-radius:50%;background:rgba(255,255,255,.08);backdrop-filter:blur(8px);display:flex;align-items:center;justify-content:center;cursor:pointer;transition:.2s}
.fso-x:hover{background:var(--er)}
.fso-x svg{width:20px;height:20px;fill:#fff}
.fso-hud{position:fixed;top:14px;left:14px;z-index:501;display:flex;gap:8px}

/* ===== MOBILE NAV ===== */
.mnav{display:none;position:fixed;bottom:0;left:0;right:0;z-index:100;background:rgba(15,21,32,.96);backdrop-filter:blur(14px);border-top:1px solid rgba(255,255,255,.04);padding:4px 0 calc(4px + var(--safe-b))}
.mnav-in{display:flex;justify-content:space-around}
.mn{display:flex;flex-direction:column;align-items:center;gap:2px;padding:5px 10px;border-radius:var(--rs);font-size:.56rem;font-weight:600;color:var(--tx3);transition:.2s;letter-spacing:.3px}
.mn svg{width:20px;height:20px;fill:currentColor}
.mn.active{color:var(--ac)}

/* ===== RESPONSIVE ===== */
@media(max-width:640px){
:root{--bar:44px;--mnav:52px}
.mnav{display:block}
.bar{height:44px;padding:0 8px}
.lt{font-size:.9rem}
.bar-r .ib.desk{display:none}
.drawer{top:44px;bottom:52px;width:100%}
.cb{width:42px;height:42px}.cb svg{width:20px;height:20px}
.cbar{height:56px;gap:8px;padding:0 14px;bottom:10px;border-radius:28px}
.cb-sel{padding:8px 28px 8px 12px;font-size:.68rem}
.sg{grid-template-columns:1fr 1fr}
}
@media(min-width:641px){.mnav{display:none}}
</style>
</head>
<body>

<!-- =============== APPBAR =============== -->
<header class="bar">
<div class="bar-l">
<div class="lm"><svg viewBox="0 0 24 24"><path d="M12 4.5C7 4.5 2.73 7.61 1 12c1.73 4.39 6 7.5 11 7.5s9.27-3.11 11-7.5c-1.73-4.39-6-7.5-11-7.5zm0 12.5c-2.76 0-5-2.24-5-5s2.24-5 5-5 5 2.24 5 5-2.24 5-5 5zm0-8c-1.66 0-3 1.34-3 3s1.34 3 3 3 3-1.34 3-3-1.34-3-3-3z"/></svg></div>
<span class="lt">TRINE<span>TRA</span></span>
</div>
<div class="bar-r">
<div class="chip" id="chip"><div class="d" id="dot"></div><span id="chipTxt">Idle</span></div>
<button class="ib desk" onclick="openDr('drSet')" title="Settings" aria-label="Settings" id="ibSet"><svg viewBox="0 0 24 24"><path d="M19.14 12.94c.04-.3.06-.61.06-.94 0-.32-.02-.64-.07-.94l2.03-1.58a.49.49 0 00.12-.61l-1.92-3.32a.49.49 0 00-.59-.22l-2.39.96c-.5-.38-1.03-.7-1.62-.94l-.36-2.54a.48.48 0 00-.48-.41h-3.84c-.24 0-.43.17-.47.41l-.36 2.54c-.59.24-1.13.56-1.62.94l-2.39-.96a.49.49 0 00-.59.22L2.74 8.87c-.12.21-.08.47.12.61l2.03 1.58c-.05.3-.07.62-.07.94s.02.64.07.94l-2.03 1.58a.49.49 0 00-.12.61l1.92 3.32c.12.22.37.29.59.22l2.39-.96c.5.38 1.03.7 1.62.94l.36 2.54c.05.24.24.41.48.41h3.84c.24 0 .44-.17.47-.41l.36-2.54c.59-.24 1.13-.57 1.62-.94l2.39.96c.22.08.47 0 .59-.22l1.92-3.32c.12-.22.07-.47-.12-.61l-2.01-1.58zM12 15.6A3.6 3.6 0 1112 8.4a3.6 3.6 0 010 7.2z"/></svg></button>
<button class="ib desk" onclick="openDr('drMon')" title="Monitor" aria-label="Monitor" id="ibMon"><svg viewBox="0 0 24 24"><path d="M19 3H5a2 2 0 00-2 2v11a2 2 0 002 2h3l-1 1v2h10v-2l-1-1h3a2 2 0 002-2V5a2 2 0 00-2-2zm0 13H5V5h14v11z"/></svg></button>
<button class="ib desk" onclick="openDr('drWifi')" title="WiFi" aria-label="WiFi" id="ibWifi"><svg viewBox="0 0 24 24"><path d="M1 9l2 2c4.97-4.97 13.03-4.97 18 0l2-2C16.93 2.93 7.08 2.93 1 9zm8 8l3 3 3-3a4.237 4.237 0 00-6 0zm-4-4l2 2c2.76-2.76 7.24-2.76 10 0l2-2C15.14 9.14 8.87 9.14 5 13z"/></svg></button>
<button class="ib" onclick="openAbout()" title="About" aria-label="About"><svg viewBox="0 0 24 24"><path d="M12 2a10 10 0 100 20 10 10 0 000-20zm1 15h-2v-6h2v6zm0-8h-2V7h2v2z"/></svg></button>
</div>
</header>

<!-- =============== HERO STREAM =============== -->
<main class="hero" id="hero">
<img id="stream" class="hide" alt="Live stream">
<div class="ph" id="ph" onclick="toggleStream()">
<div class="ph-in"><div class="ph-btn"><svg viewBox="0 0 24 24"><path d="M8 5v14l11-7z"/></svg></div><span class="ph-txt">Tap to start stream</span></div>
</div>
<div class="hud" id="hud">
<div style="display:flex;gap:8px">
<span class="bg bg-live"><svg viewBox="0 0 24 24"><circle cx="12" cy="12" r="6"/></svg>LIVE <span class="timer" id="timer">00:00</span></span>
<span class="bg bg-rec" id="recBadge" style="display:none"><svg viewBox="0 0 24 24"><circle cx="12" cy="12" r="8"/></svg>REC <span class="timer" id="recTimer">00:00</span></span>
</div>
<span class="bg bg-res" id="resLbl">QVGA</span>
</div>
<div class="shutter" id="shutter"></div>
<div class="spinner" id="spinner"><div class="spin-ring"></div></div>
<div class="notify" id="notify"></div>

<!-- Control bar -->
<div class="cbar">
<button class="cb cb-play" id="btnPS" onclick="toggleStream()" title="Start Stream"><svg viewBox="0 0 24 24" id="icoPS"><path d="M8 5v14l11-7z"/></svg></button>
<button class="cb cb-def" id="btnRec" onclick="toggleRecording()" title="Start Recording" style="display:none"><svg viewBox="0 0 24 24" id="icoRec"><circle cx="12" cy="12" r="8"/></svg></button>
<button class="cb cb-def" id="btnCap" onclick="capturePhoto()" title="Capture Photo"><svg viewBox="0 0 24 24"><circle cx="12" cy="12" r="3.2"/><path d="M9 2L7.17 4H4a2 2 0 00-2 2v12a2 2 0 002 2h16a2 2 0 002-2V6a2 2 0 00-2-2h-3.17L15 2H9zm3 15a5 5 0 110-10 5 5 0 010 10z"/></svg></button>
<button class="cb cb-def" id="btnSave" onclick="saveToSD()" title="Save to SD Card"><svg viewBox="0 0 24 24"><path d="M17 3H5a2 2 0 00-2 2v14a2 2 0 002 2h14a2 2 0 002-2V7l-4-4zm-5 16a3 3 0 110-6 3 3 0 010 6zm3-10H5V5h10v4z"/></svg></button>
<button class="cb cb-def" id="btnLED" onclick="toggleLED()" title="Flash LED"><svg viewBox="0 0 24 24"><path d="M12 2C8.13 2 5 5.13 5 9c0 2.38 1.19 4.47 3 5.74V17c0 .55.45 1 1 1h6c.55 0 1-.45 1-1v-2.26c1.81-1.27 3-3.36 3-5.74 0-3.87-3.13-7-7-7zm2 11h-1v2h-2v-2H9v-2h2V9h2v2h2v2zm-2 6h-2v1h2v-1zm0 2h-2c0 .55.45 1 1 1s1-.45 1-1z"/></svg></button>
<div class="cb-sep"></div>
<select class="cb-sel" id="resSel" onchange="changeRes()" title="Resolution">
<option value="3">QQVGA</option>
<option value="5" selected>QVGA</option>
<option value="8">VGA</option>
<option value="9">SVGA</option>
<option value="10">XGA</option>
<option value="12">SXGA</option>
</select>
<button class="cb cb-def" onclick="toggleFS()" title="Fullscreen"><svg viewBox="0 0 24 24"><path d="M7 14H5v5h5v-2H7v-3zm-2-4h2V7h3V5H5v5zm12 7h-3v2h5v-5h-2v3zM14 5v2h3v3h2V5h-5z"/></svg></button>
</div>
</main>

<!-- =============== SETTINGS DRAWER =============== -->
<div class="drawer" id="drSet">
<div class="dh"><h2><svg viewBox="0 0 24 24"><path d="M19.14 12.94c.04-.3.06-.61.06-.94 0-.32-.02-.64-.07-.94l2.03-1.58a.49.49 0 00.12-.61l-1.92-3.32a.49.49 0 00-.59-.22l-2.39.96c-.5-.38-1.03-.7-1.62-.94l-.36-2.54a.48.48 0 00-.48-.41h-3.84c-.24 0-.43.17-.47.41l-.36 2.54c-.59.24-1.13.56-1.62.94l-2.39-.96a.49.49 0 00-.59.22L2.74 8.87c-.12.21-.08.47.12.61l2.03 1.58c-.05.3-.07.62-.07.94s.02.64.07.94l-2.03 1.58a.49.49 0 00-.12.61l1.92 3.32c.12.22.37.29.59.22l2.39-.96c.5.38 1.03.7 1.62.94l.36 2.54c.05.24.24.41.48.41h3.84c.24 0 .44-.17.47-.41l.36-2.54c.59-.24 1.13-.57 1.62-.94l2.39.96c.22.08.47 0 .59-.22l1.92-3.32c.12-.22.07-.47-.12-.61l-2.01-1.58zM12 15.6A3.6 3.6 0 1112 8.4a3.6 3.6 0 010 7.2z"/></svg>Camera Settings</h2><button class="ib" onclick="closeDr('drSet')"><svg viewBox="0 0 24 24" fill="currentColor"><path d="M19 6.41L17.59 5 12 10.59 6.41 5 5 6.41 10.59 12 5 17.59 6.41 19 12 13.41 17.59 19 19 17.59 13.41 12z"/></svg></button></div>
<div class="db">
<div class="dsec"><svg viewBox="0 0 24 24"><path d="M3 17v2h6v-2H3zM3 5v2h10V5H3zm10 16v-2h8v-2h-8v-2h-2v6h2zM7 9v2H3v2h4v2h2V9H7zm14 4v-2H11v2h10zm-6-4h2V7h4V5h-4V3h-2v6z"/></svg>Image Adjustments</div>
<div class="fg"><div class="fr"><span class="fl">Quality</span><div class="rw"><input type="range" id="sQuality" min="4" max="63" value="12" oninput="rv('sQuality')"><span class="rv" id="vQuality">12</span></div></div></div>
<div class="fg"><div class="fr"><span class="fl">Brightness</span><div class="rw"><input type="range" id="sBright" min="-2" max="2" value="0" oninput="rv('sBright')"><span class="rv" id="vBright">0</span></div></div></div>
<div class="fg"><div class="fr"><span class="fl">Contrast</span><div class="rw"><input type="range" id="sContrast" min="-2" max="2" value="0" oninput="rv('sContrast')"><span class="rv" id="vContrast">0</span></div></div></div>
<div class="fg"><div class="fr"><span class="fl">Saturation</span><div class="rw"><input type="range" id="sSat" min="-2" max="2" value="0" oninput="rv('sSat')"><span class="rv" id="vSat">0</span></div></div></div>
<div class="dsep"></div>
<div class="dsec"><svg viewBox="0 0 24 24"><path d="M7 14a3 3 0 100-6 3 3 0 000 6zm6 7a3 3 0 100-6 3 3 0 000 6zm4-14a3 3 0 100-6 3 3 0 000 6z"/></svg>Toggles</div>
<label class="tg" onclick="tgSet('awb',this)"><span class="tg-label">Auto White Balance</span><input type="checkbox" checked><span class="tg-track"></span></label>
<label class="tg" onclick="tgSet('agc',this)"><span class="tg-label">Auto Gain Control</span><input type="checkbox" checked><span class="tg-track"></span></label>
<label class="tg" onclick="tgSet('aec',this)"><span class="tg-label">Auto Exposure</span><input type="checkbox" checked><span class="tg-track"></span></label>
<label class="tg" onclick="tgSet('hmirror',this)"><span class="tg-label">Horizontal Mirror</span><input type="checkbox"><span class="tg-track"></span></label>
<label class="tg" onclick="tgSet('vflip',this)"><span class="tg-label">Vertical Flip</span><input type="checkbox"><span class="tg-track"></span></label>
<label class="tg" onclick="tgSet('bpc',this)"><span class="tg-label">Black Pixel Correction</span><input type="checkbox"><span class="tg-track"></span></label>
<label class="tg" onclick="tgSet('wpc',this)"><span class="tg-label">White Pixel Correction</span><input type="checkbox" checked><span class="tg-track"></span></label>
<label class="tg" onclick="tgSet('lenc',this)"><span class="tg-label">Lens Correction</span><input type="checkbox" checked><span class="tg-track"></span></label>
<div class="dsep"></div>
<div class="dsec"><svg viewBox="0 0 24 24"><path d="M2 6H0v5h.01L0 20a2 2 0 002 2h18v-2H2V6zm20-2h-8l-2-2H6a2 2 0 00-2 2v10a2 2 0 002 2h16a2 2 0 002-2V6a2 2 0 00-2-2zM7 15l4.5-6 3.5 4.51 2.5-3.01L21 15H7z"/></svg>Effects &amp; White Balance</div>
<div class="fg"><div class="fr"><span class="fl">Effect</span><select class="f-sel" id="selFx" onchange="chgFx()"><option value="0" selected>None</option><option value="1">Negative</option><option value="2">Grayscale</option><option value="3">Red Tint</option><option value="4">Green Tint</option><option value="5">Blue Tint</option><option value="6">Sepia</option></select></div></div>
<div class="fg"><div class="fr"><span class="fl">WB Mode</span><select class="f-sel" id="selWB" onchange="chgWB()"><option value="0" selected>Auto</option><option value="1">Sunny</option><option value="2">Cloudy</option><option value="3">Office</option><option value="4">Home</option></select></div></div>
</div>
</div>

<!-- =============== MONITOR DRAWER =============== -->
<div class="drawer" id="drMon">
<div class="dh"><h2><svg viewBox="0 0 24 24"><path d="M19 3H5a2 2 0 00-2 2v11a2 2 0 002 2h3l-1 1v2h10v-2l-1-1h3a2 2 0 002-2V5a2 2 0 00-2-2zm0 13H5V5h14v11z"/></svg>System Monitor</h2><button class="ib" onclick="closeDr('drMon')"><svg viewBox="0 0 24 24" fill="currentColor"><path d="M19 6.41L17.59 5 12 10.59 6.41 5 5 6.41 10.59 12 5 17.59 6.41 19 12 13.41 17.59 19 19 17.59 13.41 12z"/></svg></button></div>
<div class="db">
<div class="sg">
<div class="sc"><div class="sc-h"><svg viewBox="0 0 24 24"><path d="M11.99 2C6.47 2 2 6.48 2 12s4.47 10 9.99 10C17.52 22 22 17.52 22 12S17.52 2 11.99 2zM12 20a8 8 0 110-16 8 8 0 010 16zm.5-13H11v6l5.25 3.15.75-1.23-4.5-2.67z"/></svg><span class="sc-l">Uptime</span></div><div class="sc-v" id="mUp">--</div></div>
<div class="sc"><div class="sc-h"><svg viewBox="0 0 24 24"><path d="M15 13V5a3 3 0 00-6 0v8a5 5 0 103.54 7.54A5 5 0 0015 13zM11 5a1 1 0 012 0h-1v1h1v2h-1v1h1v2h-2V5z"/></svg><span class="sc-l">Temp</span></div><div class="sc-v" id="mTemp">--</div></div>
<div class="sc"><div class="sc-h"><svg viewBox="0 0 24 24"><path d="M17 10.5V7a1 1 0 00-1-1H4a1 1 0 00-1 1v10a1 1 0 001 1h12a1 1 0 001-1v-3.5l4 4v-11l-4 4z"/></svg><span class="sc-l">FPS</span></div><div class="sc-v" id="mFPS">--</div><div class="sc-s" id="mFrames">0 frames</div></div>
<div class="sc"><div class="sc-h"><svg viewBox="0 0 24 24"><path d="M1 9l2 2c4.97-4.97 13.03-4.97 18 0l2-2C16.93 2.93 7.08 2.93 1 9zm8 8l3 3 3-3a4.237 4.237 0 00-6 0zm-4-4l2 2c2.76-2.76 7.24-2.76 10 0l2-2C15.14 9.14 8.87 9.14 5 13z"/></svg><span class="sc-l">Signal</span></div><div class="sc-v" id="mSig">--</div><div class="sc-s" id="mSigQ">--</div></div>
<div class="sc"><div class="sc-h"><svg viewBox="0 0 24 24"><path d="M16 11a3 3 0 100-6 3 3 0 000 6zm-8 0a3 3 0 100-6 3 3 0 000 6zm0 2c-2.33 0-7 1.17-7 3.5V19h14v-2.5c0-2.33-4.67-3.5-7-3.5zm8 0l-.97.05A4.22 4.22 0 0117 16.5V19h6v-2.5c0-2.33-4.67-3.5-7-3.5z"/></svg><span class="sc-l">Clients</span></div><div class="sc-v" id="mCli">--</div></div>
<div class="sc"><div class="sc-h"><svg viewBox="0 0 24 24"><path d="M2 20h20v-4H2v4zm2-3h2v2H4v-2zM2 4v4h20V4H2zm4 3H4V5h2v2zm-4 7h20v-4H2v4zm2-3h2v2H4v-2z"/></svg><span class="sc-l">Heap</span></div><div class="sc-v" id="mHeap">--</div><div class="prg"><div class="prg-b" id="pHeap" style="width:0"></div></div></div>
<div class="sc"><div class="sc-h"><svg viewBox="0 0 24 24"><path d="M19 3H5a2 2 0 00-2 2v14a2 2 0 002 2h14a2 2 0 002-2V5a2 2 0 00-2-2zm-7 14a3 3 0 110-6 3 3 0 010 6zm3-10H5V5h10v2z"/></svg><span class="sc-l">PSRAM</span></div><div class="sc-v" id="mPSRAM">--</div><div class="prg"><div class="prg-b" id="pPSRAM" style="width:0"></div></div></div>
<div class="sc"><div class="sc-h"><svg viewBox="0 0 24 24"><path d="M17 3H5a2 2 0 00-2 2v14a2 2 0 002 2h14a2 2 0 002-2V7l-4-4zm-5 16a3 3 0 110-6 3 3 0 010 6zm3-10H5V5h10v4z"/></svg><span class="sc-l">SD Card</span></div><div class="sc-v" id="mSD">--</div><div class="sc-s" id="mSDSize">--</div><div class="prg"><div class="prg-b" id="pSD" style="width:0"></div></div></div>
</div>
<div style="text-align:center;margin-top:12px"><button class="btn btn-s" onclick="getStats()"><svg viewBox="0 0 24 24"><path d="M17.65 6.35A7.96 7.96 0 0012 4a8 8 0 107.73 10h-2.08A6 6 0 1112 6c1.66 0 3.14.69 4.22 1.78L13 11h7V4l-2.35 2.35z"/></svg>Refresh</button></div>
</div>
</div>

<!-- =============== WIFI DRAWER =============== -->
<div class="drawer" id="drWifi">
<div class="dh"><h2><svg viewBox="0 0 24 24"><path d="M1 9l2 2c4.97-4.97 13.03-4.97 18 0l2-2C16.93 2.93 7.08 2.93 1 9zm8 8l3 3 3-3a4.237 4.237 0 00-6 0zm-4-4l2 2c2.76-2.76 7.24-2.76 10 0l2-2C15.14 9.14 8.87 9.14 5 13z"/></svg>WiFi</h2><button class="ib" onclick="closeDr('drWifi')"><svg viewBox="0 0 24 24" fill="currentColor"><path d="M19 6.41L17.59 5 12 10.59 6.41 5 5 6.41 10.59 12 5 17.59 6.41 19 12 13.41 17.59 19 19 17.59 13.41 12z"/></svg></button></div>
<div class="db">
<div class="wsc" id="wifiCard">
<div class="wsc-i dis" id="wIcon"><svg viewBox="0 0 24 24"><path d="M1 9l2 2c4.97-4.97 13.03-4.97 18 0l2-2C16.93 2.93 7.08 2.93 1 9zm8 8l3 3 3-3a4.237 4.237 0 00-6 0zm-4-4l2 2c2.76-2.76 7.24-2.76 10 0l2-2C15.14 9.14 8.87 9.14 5 13z"/></svg></div>
<div style="flex:1;min-width:0"><div class="wsc-t" id="wTitle">Checking...</div><div class="wsc-d" id="wDetail">Getting status</div></div>
</div>
<div class="bg2 c2" style="margin-bottom:14px">
<button class="btn btn-p" id="btnScan" onclick="scanNets()"><svg viewBox="0 0 24 24"><path d="M1 9l2 2c4.97-4.97 13.03-4.97 18 0l2-2C16.93 2.93 7.08 2.93 1 9zm8 8l3 3 3-3a4.237 4.237 0 00-6 0zm-4-4l2 2c2.76-2.76 7.24-2.76 10 0l2-2C15.14 9.14 8.87 9.14 5 13z"/></svg>Scan &amp; Connect</button>
<button class="btn btn-d" onclick="resetWifi()"><svg viewBox="0 0 24 24"><path d="M6 19a2 2 0 002 2h8a2 2 0 002-2V7H6v12zM19 4h-3.5l-1-1h-5l-1 1H5v2h14V4z"/></svg>Reset WiFi</button>
</div>
<div id="netArea"></div>
<div id="connForm" style="display:none;margin-top:12px;padding-top:12px;border-top:1px solid var(--bd)">
<div style="font-size:.78rem;margin-bottom:8px"><strong>Network:</strong> <span id="selSSID" style="color:var(--ac)"></span></div>
<input type="password" class="f-in" id="wPwd" placeholder="Enter password" style="width:100%;margin-bottom:8px">
<div class="bg2 c2">
<button class="btn btn-p" onclick="doConnect()"><svg viewBox="0 0 24 24"><path d="M3.9 12A3.1 3.1 0 017 8.9h4V7H7a5 5 0 000 10h4v-1.9H7A3.1 3.1 0 013.9 12zM8 13h8v-2H8v2zm9-6h-4v1.9h4a3.1 3.1 0 010 6.2h-4V17h4a5 5 0 000-10z"/></svg>Connect</button>
<button class="btn btn-g" onclick="cancelConn()">Cancel</button>
</div>
</div>
<div class="dsep"></div>
<div class="dsec"><svg viewBox="0 0 24 24"><path d="M12 2a10 10 0 100 20 10 10 0 000-20zm1 15h-2v-2h2v2zm0-4h-2V7h2v6z"/></svg>Access Point Info</div>
<div class="irow"><span class="irow-l">AP Network</span><span class="irow-v">Trinetra</span></div>
<div class="irow"><span class="irow-l">AP Password</span><span class="irow-v">88888888</span></div>
<div class="irow"><span class="irow-l">AP Address</span><span class="irow-v irow-ac">1.2.3.4</span></div>
<div class="irow"><span class="irow-l">mDNS</span><span class="irow-v irow-ac">trinetra.local</span></div>
<div class="irow"><span class="irow-l">Stream Port</span><span class="irow-v">81</span></div>
</div>
</div>

<!-- =============== DRAWER BACKDROP =============== -->
<div class="backdrop" id="bk" onclick="closeAll()"></div>

<!-- =============== FULLSCREEN =============== -->
<div class="fso" id="fso">
<div class="fso-hud"><span class="bg bg-live"><svg viewBox="0 0 24 24"><circle cx="12" cy="12" r="6"/></svg>LIVE</span><span class="bg bg-res" id="fsRes">QVGA</span></div>
<img id="fsImg" alt="Fullscreen">
<button class="fso-x" onclick="exitFS()"><svg viewBox="0 0 24 24"><path d="M19 6.41L17.59 5 12 10.59 6.41 5 5 6.41 10.59 12 5 17.59 6.41 19 12 13.41 17.59 19 19 17.59 13.41 12z"/></svg></button>
</div>

<!-- =============== ABOUT MODAL =============== -->
<div class="mov" id="aboutOv" onclick="if(event.target===this)closeAbout()">
<div class="mo">
<div class="mo-h"><h2><svg viewBox="0 0 24 24"><path d="M12 2a10 10 0 100 20 10 10 0 000-20zm1 15h-2v-6h2v6zm0-8h-2V7h2v2z"/></svg>About</h2><button class="ib" onclick="closeAbout()"><svg viewBox="0 0 24 24" fill="currentColor"><path d="M19 6.41L17.59 5 12 10.59 6.41 5 5 6.41 10.59 12 5 17.59 6.41 19 12 13.41 17.59 19 19 17.59 13.41 12z"/></svg></button></div>
<div class="mo-b" style="text-align:center">
<div style="display:flex;flex-direction:column;align-items:center;gap:10px;padding:10px 0">
<div class="about-icon"><svg viewBox="0 0 24 24"><path d="M12 4.5C7 4.5 2.73 7.61 1 12c1.73 4.39 6 7.5 11 7.5s9.27-3.11 11-7.5c-1.73-4.39-6-7.5-11-7.5zm0 12.5c-2.76 0-5-2.24-5-5s2.24-5 5-5 5 2.24 5 5-2.24 5-5 5zm0-8c-1.66 0-3 1.34-3 3s1.34 3 3 3 3-1.34 3-3-1.34-3-3-3z"/></svg></div>
<div style="font-size:1.1rem;font-weight:700">TRINETRA</div>
<span style="font-size:.65rem;color:var(--tx3);background:var(--sf2);padding:2px 10px;border-radius:10px">v3.0</span>
</div>
<p style="font-size:.76rem;color:var(--tx2);margin:12px 0;line-height:1.6">Professional ESP32-CAM surveillance with real-time MJPEG streaming, web-based WiFi management, and SD card storage. "Trinetra" means three eyes in Sanskrit.</p>
<div style="text-align:left">
<div class="irow"><span class="irow-l">Platform</span><span class="irow-v">ESP32-CAM AI-Thinker</span></div>
<div class="irow"><span class="irow-l">Camera</span><span class="irow-v">OV2640</span></div>
<div class="irow"><span class="irow-l">WiFi</span><span class="irow-v">STA + AP</span></div>
</div>
<div style="margin-top:16px;padding-top:14px;border-top:1px solid var(--bd);text-align:center">
<div style="font-size:.64rem;color:var(--tx3);text-transform:uppercase;letter-spacing:.8px;margin-bottom:5px">Developed by</div>
<div style="font-size:.82rem;font-weight:700;color:var(--ac)">Sakshyam Bastakoti</div>
<div style="font-size:.62rem;color:var(--tx3);margin-top:3px">Embedded Systems Engineer</div>
</div>
</div>
<div class="mo-f"><button class="btn btn-s" onclick="closeAbout()">Close</button></div>
</div>
</div>

<!-- =============== MOBILE NAV =============== -->
<nav class="mnav">
<div class="mnav-in">
<button class="mn active" onclick="goStream()" id="mn0"><svg viewBox="0 0 24 24"><path d="M17 10.5V7a1 1 0 00-1-1H4a1 1 0 00-1 1v10a1 1 0 001 1h12a1 1 0 001-1v-3.5l4 4v-11l-4 4z"/></svg>Stream</button>
<button class="mn" onclick="openDr('drSet')" id="mn1"><svg viewBox="0 0 24 24"><path d="M19.14 12.94c.04-.3.06-.61.06-.94 0-.32-.02-.64-.07-.94l2.03-1.58a.49.49 0 00.12-.61l-1.92-3.32a.49.49 0 00-.59-.22l-2.39.96c-.5-.38-1.03-.7-1.62-.94l-.36-2.54a.48.48 0 00-.48-.41h-3.84c-.24 0-.43.17-.47.41l-.36 2.54c-.59.24-1.13.56-1.62.94l-2.39-.96a.49.49 0 00-.59.22L2.74 8.87c-.12.21-.08.47.12.61l2.03 1.58c-.05.3-.07.62-.07.94s.02.64.07.94l-2.03 1.58a.49.49 0 00-.12.61l1.92 3.32c.12.22.37.29.59.22l2.39-.96c.5.38 1.03.7 1.62.94l.36 2.54c.05.24.24.41.48.41h3.84c.24 0 .44-.17.47-.41l.36-2.54c.59-.24 1.13-.57 1.62-.94l2.39.96c.22.08.47 0 .59-.22l1.92-3.32c.12-.22.07-.47-.12-.61l-2.01-1.58zM12 15.6A3.6 3.6 0 1112 8.4a3.6 3.6 0 010 7.2z"/></svg>Settings</button>
<button class="mn" onclick="openDr('drMon')" id="mn2"><svg viewBox="0 0 24 24"><path d="M19 3H5a2 2 0 00-2 2v11a2 2 0 002 2h3l-1 1v2h10v-2l-1-1h3a2 2 0 002-2V5a2 2 0 00-2-2zm0 13H5V5h14v11z"/></svg>Monitor</button>
<button class="mn" onclick="openDr('drWifi')" id="mn3"><svg viewBox="0 0 24 24"><path d="M1 9l2 2c4.97-4.97 13.03-4.97 18 0l2-2C16.93 2.93 7.08 2.93 1 9zm8 8l3 3 3-3a4.237 4.237 0 00-6 0zm-4-4l2 2c2.76-2.76 7.24-2.76 10 0l2-2C15.14 9.14 8.87 9.14 5 13z"/></svg>WiFi</button>
</div>
</nav>

<script>
/* ===== State ===== */
var G={
streaming:false,ledOn:false,recording:false,
base:location.origin,
sUrl:location.protocol+'//'+location.hostname+':81/stream',
resN:{'3':'QQVGA','5':'QVGA','8':'VGA','9':'SVGA','10':'XGA','12':'SXGA'},
tStart:0,tInt:null,sInt:null,selNet:null,openDrId:null,
recStart:0,recInt:null
};
var $=function(id){return document.getElementById(id)};

/* ===== Notify (minimal pill, auto-hides) ===== */
var nTO;
function nfy(msg,type){
var el=$('notify');
var ic={ok:'<svg viewBox="0 0 24 24"><path d="M12 2a10 10 0 100 20 10 10 0 000-20zm-2 15l-5-5 1.41-1.41L10 14.17l7.59-7.59L19 8l-9 9z"/></svg>',er:'<svg viewBox="0 0 24 24"><path d="M12 2a10 10 0 100 20 10 10 0 000-20zm1 15h-2v-2h2v2zm0-4h-2V7h2v6z"/></svg>',wn:'<svg viewBox="0 0 24 24"><path d="M1 21h22L12 2 1 21zm12-3h-2v-2h2v2zm0-4h-2v-4h2v4z"/></svg>'};
el.className='notify n-'+(type||'ok');
el.innerHTML=(ic[type||'ok']||ic.ok)+'<span>'+msg+'</span>';
clearTimeout(nTO);
void el.offsetWidth;
el.classList.add('show');
nTO=setTimeout(function(){el.classList.remove('show')},2200);
}

/* ===== Button feedback flash ===== */
function fbtn(el,ok){
el.classList.add(ok?'fb-ok':'fb-er');
setTimeout(function(){el.classList.remove('fb-ok','fb-er')},500);
}

/* ===== Status chip ===== */
function setChip(live,txt){
$('dot').className='d'+(live?' on':'');
$('chipTxt').textContent=txt||'Idle';
}

/* ===== Stream Timer ===== */
function startTimer(){G.tStart=Date.now();G.tInt=setInterval(updTimer,1000)}
function stopTimer(){clearInterval(G.tInt);$('timer').textContent='00:00'}
function updTimer(){
var e=Math.floor((Date.now()-G.tStart)/1000);
$('timer').textContent=String(Math.floor(e/60)).padStart(2,'0')+':'+String(e%60).padStart(2,'0');
}

/* ===== Stream Toggle ===== */
function toggleStream(){
if(G.streaming)stopStream();else startStream();
}
function startStream(){
var img=$('stream');
$('spinner').classList.add('show');
img.onload=function(){$('spinner').classList.remove('show');img.onload=null};
img.onerror=function(){$('spinner').classList.remove('show');nfy('Stream failed','er')};
img.src=G.sUrl;
img.classList.remove('hide');
$('ph').classList.add('hide');
$('hud').classList.add('show');
$('btnPS').className='cb cb-stop';
$('icoPS').innerHTML='<path d="M6 6h12v12H6z"/>';
$('btnRec').style.display='flex';
G.streaming=true;
setChip(true,'Live');
startTimer();
}
function stopStream(){
if(G.recording){stopRecording()}
var img=$('stream');
img.src='';img.classList.add('hide');
$('ph').classList.remove('hide');
$('hud').classList.remove('show');
$('btnPS').className='cb cb-play';
$('icoPS').innerHTML='<path d="M8 5v14l11-7z"/>';
$('btnRec').style.display='none';
$('spinner').classList.remove('show');
G.streaming=false;
setChip(false,'Idle');
stopTimer();
exitFS();
}

/* ===== Capture ===== */
function capturePhoto(){
var el=$('btnCap');
var sh=$('shutter');
sh.classList.remove('flash');void sh.offsetWidth;sh.classList.add('flash');
fetch(G.base+'/capture')
.then(function(r){if(!r.ok)throw new Error();return r.blob()})
.then(function(b){
var u=URL.createObjectURL(b);var a=document.createElement('a');
a.href=u;a.download='trinetra_'+Date.now()+'.jpg';a.click();
URL.revokeObjectURL(u);fbtn(el,true);
})
.catch(function(){fbtn(el,false);nfy('Capture failed','er')});
}

/* ===== Save to SD ===== */
function saveToSD(){
var el=$('btnSave');
fetch(G.base+'/save-photo')
.then(function(r){return r.json()})
.then(function(d){
if(d.success){fbtn(el,true);nfy(d.filename,'ok')}
else{fbtn(el,false);nfy(d.error||'SD error','er')}
})
.catch(function(){fbtn(el,false);nfy('SD save failed','er')});
}

/* ===== LED ===== */
function toggleLED(){
var next=G.ledOn?0:1;
fetch(G.base+'/led?state='+next)
.then(function(r){
if(r.ok){
G.ledOn=!!next;
$('btnLED').className='cb '+(G.ledOn?'cb-led-on':'cb-def');
}
}).catch(function(){nfy('LED failed','er')});
}

/* ===== Recording ===== */
function startRecTimer(){G.recStart=Date.now();G.recInt=setInterval(updRecTimer,1000)}
function stopRecTimer(){clearInterval(G.recInt);$('recTimer').textContent='00:00'}
function updRecTimer(){
var e=Math.floor((Date.now()-G.recStart)/1000);
$('recTimer').textContent=String(Math.floor(e/60)).padStart(2,'0')+':'+String(e%60).padStart(2,'0');
}

function toggleRecording(){
if(G.recording)stopRecording();else startRecording();
}

function startRecording(){
if(!G.streaming){nfy('Start stream first','wn');return}
var el=$('btnRec');
fetch(G.base+'/start-recording')
.then(function(r){return r.json()})
.then(function(d){
if(d.success){
G.recording=true;
el.className='cb cb-stop';
$('icoRec').innerHTML='<path d="M6 6h12v12H6z"/>';
el.title='Stop Recording';
$('recBadge').style.display='flex';
startRecTimer();
fbtn(el,true);
nfy('Recording started','ok');
}else{
fbtn(el,false);
nfy(d.error||'Recording failed','er');
}
}).catch(function(){fbtn(el,false);nfy('Recording error','er')});
}

function stopRecording(){
var el=$('btnRec');
fetch(G.base+'/stop-recording')
.then(function(r){return r.json()})
.then(function(d){
if(d.success){
G.recording=false;
el.className='cb cb-def';
$('icoRec').innerHTML='<circle cx="12" cy="12" r="8"/>';
el.title='Start Recording';
$('recBadge').style.display='none';
stopRecTimer();
fbtn(el,true);
var msg='Saved: '+d.filename+' ('+(d.size/1048576).toFixed(1)+'MB, '+d.frames+' frames, '+d.duration+'s)';
nfy(msg,'ok');
}else{
fbtn(el,false);
nfy(d.error||'Stop failed','er');
}
}).catch(function(){fbtn(el,false);nfy('Stop error','er')});
}

/* ===== Resolution ===== */
function changeRes(){
var v=$('resSel').value;
fetch(G.base+'/control?var=framesize&val='+v)
.then(function(r){
if(r.ok){
var n=G.resN[v]||v;
$('resLbl').textContent=n;
$('fsRes').textContent=n;
if(G.streaming){
/* Force stream reload for new resolution */
var img=$('stream');
img.src='';
setTimeout(function(){img.src=G.sUrl},200);
}
}else nfy('Failed','er');
}).catch(function(){nfy('Error','er')});
}

/* ===== Settings changes (sliders) ===== */
function rv(id){
var el=$(id);var vEl=$('v'+id.substring(1));
if(vEl)vEl.textContent=el.value;
}
var chgTO={};
function chgSlider(param,id){
clearTimeout(chgTO[param]);
chgTO[param]=setTimeout(function(){
fetch(G.base+'/control?var='+param+'&val='+$(id).value);
},300);
}
/* Attach change events */
function initSliders(){
var m=[['quality','sQuality'],['brightness','sBright'],['contrast','sContrast'],['saturation','sSat']];
for(var i=0;i<m.length;i++){
(function(p,id){
$(id).addEventListener('change',function(){chgSlider(p,id)});
})(m[i][0],m[i][1]);
}
}

/* ===== Toggle settings ===== */
function tgSet(name,el){
setTimeout(function(){
var cb=el.querySelector('input');
fetch(G.base+'/control?var='+name+'&val='+(cb.checked?1:0));
},50);
}

/* ===== Effects & WB ===== */
function chgFx(){fetch(G.base+'/control?var=special_effect&val='+$('selFx').value)}
function chgWB(){fetch(G.base+'/control?var=wb_mode&val='+$('selWB').value)}

/* ===== Fullscreen ===== */
function toggleFS(){
if(!G.streaming){nfy('Start stream first','wn');return}
$('fso').classList.add('show');
$('fsImg').src=G.sUrl;
$('fsRes').textContent=$('resLbl').textContent;
document.body.style.overflow='hidden';
}
function exitFS(){
$('fso').classList.remove('show');
$('fsImg').src='';
document.body.style.overflow='';
}

/* ===== Drawers ===== */
function openDr(id){
closeAll();
$(id).classList.add('open');
$('bk').classList.add('show');
G.openDrId=id;
/* highlight appbar icons */
var map={drSet:'ibSet',drMon:'ibMon',drWifi:'ibWifi'};
if(map[id]&&$(map[id]))$(map[id]).classList.add('active');
/* mobile nav highlight */
var mi={drSet:1,drMon:2,drWifi:3};
document.querySelectorAll('.mn').forEach(function(b,i){b.classList.toggle('active',i===(mi[id]||0))});
if(id==='drMon')getStats();
if(id==='drWifi')getWifiSt();
}
function closeDr(id){
$(id).classList.remove('open');
$('bk').classList.remove('show');
G.openDrId=null;
document.querySelectorAll('.ib.active').forEach(function(b){b.classList.remove('active')});
document.querySelectorAll('.mn').forEach(function(b,i){b.classList.toggle('active',i===0)});
}
function closeAll(){
if(G.openDrId)closeDr(G.openDrId);
}
function goStream(){closeAll()}

/* ===== System Stats ===== */
function getStats(){
fetch(G.base+'/system-stats')
.then(function(r){return r.json()})
.then(function(d){
var u='';if(d.uptime_days>0)u+=d.uptime_days+'d ';
u+=(d.uptime_hours%24)+'h '+(d.uptime_minutes%60)+'m';
$('mUp').textContent=u;
$('mTemp').textContent=d.temp_celsius.toFixed(1)+'\u00B0C';
$('mFPS').textContent=d.fps+' fps';
$('mFrames').textContent=d.total_frames.toLocaleString()+' frames';
var sq=d.wifi_rssi>-60?'Excellent':d.wifi_rssi>-70?'Good':d.wifi_rssi>-80?'Fair':'Poor';
$('mSig').textContent=d.wifi_rssi+' dBm';
$('mSigQ').textContent=sq;
$('mCli').textContent=d.wifi_clients;
var hp=d.heap_usage.toFixed(0);
$('mHeap').textContent=(d.free_heap/1048576).toFixed(1)+'MB free';
var hb=$('pHeap');hb.style.width=hp+'%';hb.className='prg-b'+(hp>85?' cr':hp>70?' wn':'');
var pp=d.psram_usage.toFixed(0);
$('mPSRAM').textContent=(d.free_psram/1048576).toFixed(1)+'MB free';
var pb=$('pPSRAM');pb.style.width=pp+'%';pb.className='prg-b'+(pp>85?' cr':pp>70?' wn':'');
/* SD Card */
if(d.sd_available){
var sdp=d.sd_percent.toFixed(0);
var sdfree=(d.sd_free/1073741824).toFixed(1);
var sdtotal=(d.sd_total/1073741824).toFixed(1);
$('mSD').textContent=sdfree+'GB free';
$('mSDSize').textContent=sdtotal+'GB total';
var sdb=$('pSD');sdb.style.width=sdp+'%';sdb.className='prg-b'+(sdp>90?' cr':sdp>75?' wn':'');
}else{
$('mSD').textContent='Not Available';
$('mSDSize').textContent='--';
$('pSD').style.width='0';
}
}).catch(function(){});
}

/* ===== WiFi ===== */
function getWifiSt(){
fetch(G.base+'/wifi-status')
.then(function(r){return r.json()})
.then(function(d){
if(d.connected){
$('wIcon').className='wsc-i con';
$('wTitle').innerHTML='Connected to <b>'+d.ssid+'</b>';
$('wTitle').style.color='var(--ok)';
$('wDetail').textContent='IP: '+d.ip;
}else if(d.savedSSID){
$('wIcon').className='wsc-i dis';
$('wTitle').textContent='Saved: '+d.savedSSID;
$('wTitle').style.color='var(--tx2)';
$('wDetail').textContent='Not connected';
}else{
$('wIcon').className='wsc-i dis';
$('wTitle').textContent='No WiFi';
$('wTitle').style.color='var(--tx3)';
$('wDetail').textContent='AP mode only';
}
}).catch(function(){});
}
function scanNets(){
var btn=$('btnScan');btn.disabled=true;
$('netArea').innerHTML='<div style="text-align:center;padding:20px;color:var(--tx3);font-size:.75rem"><div class="spin-ring" style="width:24px;height:24px;border-width:2px;margin:0 auto 8px"></div>Scanning...</div>';
$('connForm').style.display='none';G.selNet=null;
fetch(G.base+'/wifi-scan')
.then(function(r){return r.json()})
.then(function(nets){
btn.disabled=false;
if(!nets.length){$('netArea').innerHTML='<div style="text-align:center;padding:20px;color:var(--tx3);font-size:.75rem">No networks found</div>';return}
nets.sort(function(a,b){return b.rssi-a.rssi});
var h='';
for(var i=0;i<nets.length;i++){
var n=nets[i];
var sig=n.rssi>-60?'var(--ok)':n.rssi>-75?'var(--wn)':'var(--er)';
var lock=n.secure?'<svg viewBox="0 0 24 24" style="fill:var(--wn)"><path d="M18 8h-1V6a5 5 0 00-10 0v2H6a2 2 0 00-2 2v10a2 2 0 002 2h12a2 2 0 002-2V10a2 2 0 00-2-2zm-6 9a2 2 0 110-4 2 2 0 010 4zm3.1-9H8.9V6c0-1.71 1.39-3.1 3.1-3.1s3.1 1.39 3.1 3.1v2z"/></svg>':'';
h+='<div class="ni" onclick="pickNet(\''+n.ssid.replace(/'/g,"\\'")+'\','+n.secure+',this)"><span class="ni-n">'+n.ssid+'</span><span class="ni-m"><svg viewBox="0 0 24 24" style="fill:'+sig+'"><path d="M1 9l2 2c4.97-4.97 13.03-4.97 18 0l2-2C16.93 2.93 7.08 2.93 1 9zm8 8l3 3 3-3a4.237 4.237 0 00-6 0zm-4-4l2 2c2.76-2.76 7.24-2.76 10 0l2-2C15.14 9.14 8.87 9.14 5 13z"/></svg>'+n.rssi+'dBm '+lock+'</span></div>';
}
$('netArea').innerHTML='<div style="max-height:220px;overflow-y:auto">'+h+'</div>';
}).catch(function(){btn.disabled=false;$('netArea').innerHTML='<div style="text-align:center;padding:20px;color:var(--er);font-size:.75rem">Scan failed</div>'});
}
function pickNet(ssid,sec,el){
G.selNet={ssid:ssid,secure:sec};
$('selSSID').textContent=ssid;
$('wPwd').value='';$('wPwd').style.display=sec?'block':'none';
$('connForm').style.display='block';
document.querySelectorAll('.ni').forEach(function(n){n.classList.remove('sel')});
el.classList.add('sel');
}
function cancelConn(){$('connForm').style.display='none';G.selNet=null}
function doConnect(){
if(!G.selNet)return;
var s=G.selNet.ssid,p=$('wPwd').value;
nfy('Connecting to '+s+'...','ok');
fetch(G.base+'/wifi-connect?ssid='+encodeURIComponent(s)+'&password='+encodeURIComponent(p))
.then(function(r){return r.json()})
.then(function(d){
if(d.success){nfy('Connected! IP: '+d.ip,'ok');$('connForm').style.display='none';getWifiSt()}
else nfy('Failed to connect','er');
}).catch(function(){nfy('Connection error','er')});
}
function resetWifi(){
if(!confirm('Clear saved WiFi?'))return;
fetch(G.base+'/wifi-reset').then(function(r){return r.json()}).then(function(d){nfy(d.message,'ok');getWifiSt()}).catch(function(){nfy('Reset failed','er')});
}

/* ===== Keyboard shortcuts ===== */
document.addEventListener('keydown',function(e){
if(e.key==='Escape'){exitFS();closeAll();closeAbout()}
if(e.key==='f'&&!e.ctrlKey&&!e.metaKey&&document.activeElement.tagName!=='INPUT'&&document.activeElement.tagName!=='SELECT')toggleFS();
if(e.key===' '&&document.activeElement.tagName!=='INPUT'&&document.activeElement.tagName!=='SELECT'){e.preventDefault();toggleStream()}
});

/* ===== About ===== */
function openAbout(){$('aboutOv').classList.add('show')}
function closeAbout(){$('aboutOv').classList.remove('show')}

/* ===== Init ===== */
window.addEventListener('load',function(){
setChip(false,'Idle');
initSliders();

fetch(G.base+'/status')
.then(function(r){return r.json()})
.then(function(d){
if(d.framesize!==undefined){$('resSel').value=d.framesize;$('resLbl').textContent=G.resN[String(d.framesize)]||''}
if(d.quality!==undefined){$('sQuality').value=d.quality;$('vQuality').textContent=d.quality}
if(d.brightness!==undefined){$('sBright').value=d.brightness;$('vBright').textContent=d.brightness}
if(d.contrast!==undefined){$('sContrast').value=d.contrast;$('vContrast').textContent=d.contrast}
if(d.saturation!==undefined){$('sSat').value=d.saturation;$('vSat').textContent=d.saturation}
if(d.special_effect!==undefined)$('selFx').value=d.special_effect;
if(d.wb_mode!==undefined)$('selWB').value=d.wb_mode;
var tgs=document.querySelectorAll('#drSet .tg input');
var tk=['awb','agc','aec','hmirror','vflip','bpc','wpc','lenc'];
for(var i=0;i<tk.length;i++){if(d[tk[i]]!==undefined&&tgs[i])tgs[i].checked=!!d[tk[i]]}
if(d.sd_card){$('mSD').textContent='Available';$('mSDSize').textContent='Check monitor';}else{$('mSD').textContent='N/A';$('mSDSize').textContent='--';}
}).catch(function(){setChip(false,'Offline')});

G.sInt=setInterval(function(){if(G.openDrId==='drMon')getStats()},3000);
});
</script>
</body>
</html>
)rawliteral";
#endif
